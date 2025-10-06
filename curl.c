// SPDX-License-Identifier: Apache-2.0
// Copyright 2018-2025 Gliim LLC. 
// Licensed under Apache License v2. See LICENSE file.
// On the web http://rimstone-lang.com/ - this file is part of RimStone framework.

// 
// Curl-related module
//


#include "rim.h"

// Type that defines curl internal data for a single web-call
typedef struct s_curlcall 
{
    CURL *curl; // handle for curl call
    struct curl_slist *hlist; // custom headers
    curl_mime *form; // data for posting a form 
    char **error; // error text of curl call
} curlcall;

static void rim_end_web(char **error, rim_num res, rim_num *tries, curlcall *cc);
static CURLcode rim_process_post (curlcall *cc, char *fields[], char *files[], rim_num *tries);

// Response for web call. Chunked responses are added to the string ptr determined by total dynamic length of len.
//
typedef struct rim_s_url_callback {
    char *ptr;
    size_t len;
} rim_url_data;

static void rim_init_url_callback(rim_url_data *s);
static void rim_cleanup_curlcall (curlcall *cc);
static void rim_init_curlcall (curlcall *cc, char **error);
static size_t rim_write_url_callback(void *ptr, size_t size, size_t nmemb, void *s);
static rim_num rim_web_set_header (curlcall *cc, rim_num *tries, char *name, char *val);


// Initialize URL response (response received in rim_post_url_with_response(), i.e. when
// calling web service somewhere). 's' is URL response.
//
static void rim_init_url_callback(rim_url_data *s)
{
    s->len = 0;
    s->ptr = rim_malloc(s->len+1);
    s->ptr[0] = '\0';
}

//
// Write url response data to url response 's'. 'ptr' is the data received as a response from a
// web call in rim_post_url_with_response(), 'size'*'nmemb' is ptr's length. This is curl's callback
// function where pieces of response are channeled here, so this function by nature is cumulative.
// The signature of this function stems of the default curl's handler, which is a write() into a file.
// Returns the number of bytes "written" into our buffer, and that must match the number of bytes
// passed to it.
//
static size_t rim_write_url_callback(void *ptr, size_t size, size_t nmemb, void *s)
{
    // new len (so far plus what's incoming)
    size_t new_len = ((rim_url_data*)s)->len + size*nmemb;

    // reallocate buffer to hold new len (the input from curl doesn't include zero byte, so +1)
    ((rim_url_data*)s)->ptr = rim_realloc( rim_mem_get_id(((rim_url_data*)s)->ptr), new_len+1);

    // append incoming data after current input
    memcpy(((rim_url_data*)s)->ptr+((rim_url_data*)s)->len, ptr, size*nmemb);

    // zero terminate
    ((rim_url_data*)s)->ptr[new_len] = '\0';

    // set new len
    ((rim_url_data*)s)->len = new_len;


    return size*nmemb;
}


//
// End curl call. It must do cleanup, or some interesting crashes may happen
// error is the error returned from web-call, res is the result of curl op, tries is the
// number of tries for redirecting URL, curl is the handle of the call
//
static void rim_end_web(char **error, rim_num res, rim_num *tries, curlcall *cc)
{
     if (error != NULL) *error = rim_strdup ((char*)curl_easy_strerror(res));
     *tries = 0;
     rim_cleanup_curlcall(cc);
}

//
// Cleanup after a curl web-call, cc is the curl struct
//
static void rim_cleanup_curlcall (curlcall *cc)
{
   if (cc->curl != NULL) curl_easy_cleanup(cc->curl);
   if (cc->form != NULL) curl_mime_free(cc->form);
   if (cc->hlist != NULL) curl_slist_free_all(cc->hlist);
}

//
// Initialize curl web-call, cc is the curl struct, error is the output back to web-call in case of error
//
static void rim_init_curlcall (curlcall *cc, char **error)
{
    cc->curl = NULL;
    cc->hlist = NULL; 
    cc->form = NULL; 
    cc->error = error;
}

//
// Set header for curl response. cc is the curl struct, tries is retry number, name/val
// is the header
// returns RIM_OKAY or RIM_ERR_FAILED.
//
static rim_num rim_web_set_header (curlcall *cc, rim_num *tries, char *name, char *val)
{
    CURLcode res = CURLE_OK;
    struct curl_slist *tmp;
    char opt[1024];
    snprintf (opt, sizeof(opt), "%s: %s", name, val);
    if ((tmp = curl_slist_append(cc->hlist, opt)) == NULL)
    {
        rim_end_web(NULL, res, tries, cc);
        if (cc->error != NULL) 
        {
            char eopt[1024+200];
            snprintf (eopt, sizeof(eopt), "Cannot add [%s] to header list", opt);
            *(cc->error) = rim_strdup (eopt);
        }
        RIM_ERR0;
        return RIM_ERR_FAILED;
    }
    cc->hlist = tmp;
    return RIM_OKAY;
}

//
// Issue web call, which is url string, and obtain the response in 'result' which will be allocated here.
// 'head' is the header of the response, which if NULL, is not filled
// If there is an SSL certificate (authority for this system having https capability for instance), it is
// specified in 'cert'. Any error by curl is reported in 'error' - this is not response error from web server.
// 'cookiejar' is the file name (full path) of where the cookies are to be stored. Cookies are read from here before
// the call and store here after the call. When communicating with the same server, cookiejar serves the purpose of
// holding all known cookies.
// resp_code is is response code from server (200 for okay, 404 for not found etc.)
// timeout is max # of seconds request can take. It will abort the call after that.
// bodyreq is 1 if this is POST. URL can still have ? and have query params (kind of mix of GET/POST style)
// fields and files are POST fields and files in the form of field,value...; for files, value is file name. Only valid if bodyreq is 1
// vh is request reply, custom options
// 'method' is GET, PUT, POST, PATCH or DELETE (or any other)
// payload is the body content. It's either fields/files or payload, but NOT both. v1
// enforces this in call-web statement. payload_len is its length, or -1 if it's to be strlen-ed.
// Errors (RIM_ERR_FAILED, RIM_ERR_WEB_CALL) are reported in status as negatives
// Returns number of bytes received if okay, or negative (above) if curl error
// Redirections in response are handled with up to 5 at a time.
//
rim_num rim_post_url_with_response(char *url, char **result, char **head, char **error, char *cert, char *cookiejar, rim_num *resp_code, long timeout, char bodyreq, char *fields[], char *files[], rim_header *vh, char *method, char *payload, rim_num payload_len)
{

    curlcall cc;
    rim_init_curlcall (&cc, error);

    CURLcode res = CURLE_OK;
    rim_num len=0; // length as returned by curl
    rim_num lenhead=0; // length of header as returned by curl
    RIM_UNUSED(lenhead);

    // this static is okay because EVERY return sets it to zero. We could do tries-- with each return and
    // technically by the time we get back to the first call (in a series of recursive calls), it should be zero
    // again, but this way we're sure. See code below, we don't do anything with the result other than to pass it up,
    // so the unwind of recursive calls happens without any further action.
    // Also this static is for a single request only.
    static rim_num tries = 0;

    if (cc.error != NULL && *(cc.error) != NULL) *(cc.error) = RIM_EMPTY_STRING;
    if (resp_code != NULL) *resp_code = 0; // default

    // keep track of the depth of recursive calls to this function
    // EVERY RETURN FROM THIS FUNCTION MUST HAVE TRIES-- to keep this correct.
    tries++;
    if (tries>=5)
    {
        tries = 0;
        if (cc.error != NULL) *(cc.error) = rim_strdup ("Too many redirections in URL");
        return RIM_ERR_FAILED; // too many redirections followed, error out
    }

    cc.curl = curl_easy_init();
    if (cc.curl)
    {
        rim_url_data sresp;
        rim_url_data shead;
        rim_init_url_callback (&sresp);
        rim_init_url_callback (&shead);

        if (cookiejar!=NULL)
        {
            // the same file is used to read cookies and to write them.
            // CURL will keep this file the same way a browser would do. Cookies
            // can be received from server in one call and then read in another even
            // if they weren't sent in that call.
            curl_easy_setopt(cc.curl, CURLOPT_COOKIEFILE, cookiejar);
            curl_easy_setopt(cc.curl, CURLOPT_COOKIEJAR, cookiejar);
        }

        if (cert == NULL)
        {
            curl_easy_setopt(cc.curl, CURLOPT_SSL_VERIFYHOST, 0L);
            curl_easy_setopt(cc.curl, CURLOPT_SSL_VERIFYPEER, 0L);
            // this is with-no-cert
        }
        else
        {
            curl_easy_setopt(cc.curl, CURLOPT_SSL_VERIFYHOST, 1L);
            curl_easy_setopt(cc.curl, CURLOPT_SSL_VERIFYPEER, 1L);
            if (cert[0] == 0)
            {
                // this is default CA authority, installed on system
            }
            else
            {
                // this is with-cert <location> where cert is this location
                curl_easy_setopt(cc.curl, CURLOPT_CAINFO, cert);
            }
        }

        // we do not check status of verbose settings, because if it doesn't work, we wouldn't do anything
        //if (curl_easy_setopt(cc.curl, CURLOPT_STDERR, pc->trace.f) == CURLE_OK) curl_easy_setopt(cc.curl, CURLOPT_VERBOSE, 1L);

        if ((res = curl_easy_setopt(cc.curl, CURLOPT_URL, url)) != CURLE_OK)
        {
            rim_end_web(cc.error, res, &tries, &cc);
            return RIM_ERR_FAILED;
        }
        if ((res = curl_easy_setopt(cc.curl, CURLOPT_HEADERFUNCTION, rim_write_url_callback)) != CURLE_OK)
        {
            rim_end_web(cc.error, res, &tries, &cc);
            return RIM_ERR_FAILED;
        }
        if ((res = curl_easy_setopt(cc.curl, CURLOPT_WRITEFUNCTION, rim_write_url_callback)) != CURLE_OK)
        {
            rim_end_web(cc.error, res, &tries, &cc);
            return RIM_ERR_FAILED;
        }
        if ((res = curl_easy_setopt(cc.curl, CURLOPT_WRITEDATA, &sresp)) != CURLE_OK)
        {
            rim_end_web(cc.error, res, &tries, &cc);
            return RIM_ERR_FAILED;
        }
        if ((res = curl_easy_setopt(cc.curl, CURLOPT_HEADERDATA, &shead)) != CURLE_OK)
        {
            rim_end_web(cc.error, res, &tries, &cc);
            return RIM_ERR_FAILED;
        }
        if (timeout > 86400 || timeout <= 0)
        {
            rim_end_web(NULL, res, &tries, &cc); // don't set error, we set it
            if (cc.error != NULL) *(cc.error) = rim_strdup ("Timeout invalid or too long");
            return RIM_ERR_FAILED; // timeout too long
        }
        if ((res = curl_easy_setopt(cc.curl, CURLOPT_TIMEOUT, timeout)) != CURLE_OK)
        {
            rim_end_web(cc.error, res, &tries, &cc);
            return RIM_ERR_FAILED;
        }

        // set custom header before bodyreq or any header settings done by default, because curl
        // only honors first mention of it, or it doesn't add the second one at all
        if (vh != NULL)
        {
            // Set content type if specified - if specified then it takes precedence
            // as it was specified first
            if (vh->ctype != NULL)
            {
                rim_num r = rim_web_set_header (&cc, &tries, "Content-Type", vh->ctype);
                if (r != RIM_OKAY) return r;
            }
            rim_num i = 0;
            while (vh->control[i] != NULL)
            {
                rim_num r = rim_web_set_header (&cc, &tries, vh->control[i], vh->value[i]);
                if (r != RIM_OKAY) return r;
                i++;
            }
        }


        if (bodyreq == 1) // body-request specified
        {
            if (fields != NULL || files != NULL)
            {
                if (rim_process_post (&cc, fields, files, &tries) != CURLE_OK)
                {   
                    return RIM_ERR_FAILED;
                }
            } else if (payload != NULL)
            {
                rim_num plen = rim_mem_get_len(payload);
                if (payload_len == -1) payload_len = plen;
                else if (payload_len > plen) rim_report_error ("Memory used for request body content is of length [%ld] but only [%ld] allocated", payload_len, plen);
                // Note: this setting is no longer used:
                // do NOT set CURLOPT_POSTFIELDSIZE to -1 as docs for curl API suggest, results in sigsegv in curl
                // this would have been to have curl strlen() the body, but it does it anyways by default
                curl_easy_setopt(cc.curl, CURLOPT_POSTFIELDSIZE_LARGE, (curl_off_t)payload_len);
                curl_easy_setopt(cc.curl, CURLOPT_POSTFIELDS, payload);
            } else 
            {
                rim_end_web(NULL, res, &tries, &cc); // don't set error, we set it
                if (cc.error != NULL) *(cc.error) = rim_strdup ("Internal: cannot use files/fields and body-request at the same time");
                return RIM_ERR_FAILED; // timeout too long
            }
        }

        // setting headers must be after rim_process_post() so that any headers added/changed there are
        // taken into account
        if (cc.hlist != NULL) curl_easy_setopt(cc.curl, CURLOPT_HTTPHEADER, cc.hlist);

        // setting method & content type for request body is last - it overrides any method/content type already set in headers
        if (method != NULL && method[0] != 0) curl_easy_setopt(cc.curl, CURLOPT_CUSTOMREQUEST, method);

        res = curl_easy_perform(cc.curl);
        if(res != CURLE_OK)
        {
            rim_end_web(cc.error, res, &tries, &cc);
            return RIM_ERR_WEB_CALL;
        }
        else
        {
            long response_code;
            res = curl_easy_getinfo(cc.curl, CURLINFO_RESPONSE_CODE, &response_code);
            if (resp_code != NULL) *resp_code = (rim_num)response_code;
            if((res == CURLE_OK) &&
                   ((response_code / 100) != 3))
            {
                /* a redirect implies a 3xx response code , 301/302/308/307 temp and perm*/
                // this is if NOT a redirection (3xx). The actual result is obtained ONLY from the actual data response and NOT from redirection.
                *result = sresp.ptr;
                len = sresp.len;
                rim_mem_set_len (*result, len+1);
                if (head != NULL)
                {
                    *head = shead.ptr;
                    lenhead = shead.len;
                    rim_mem_set_len (*head, lenhead+1);
                }
            }
            else
            {
                char *location;
                res = curl_easy_getinfo(cc.curl, CURLINFO_REDIRECT_URL, &location);

                if((res == CURLE_OK) && location)
                {
                      /* This is the new absolute URL that you could redirect to, even if
                       *            * the Location: response header may have been a relative URL. */
                      rim_free_int (sresp.ptr); // free the old result
                      rim_free_int (shead.ptr); // free the old result
                      //
                      // Recursive call to this function is done so that its result is always immediately
                      // passed back to the caller, so that it is a clean winding and unwinding. There is no unwinding followed
                      // by winding followed by unwinding etc. There is only winding and then unwinding back to the original caller.
                      // So 'tries' is increased up to the last recursive call, and after that one returns without a recursion it goes
                      // back to the original one without any interruption. THat's why we can set tries to zero right away.
                      // So, when 'res' is obtained it MUST BE immediately passed back.
                      //
                      rim_num res = rim_post_url_with_response(location, result, head, cc.error, cert, cookiejar, resp_code, timeout, bodyreq, fields, files, vh, method, payload, payload_len);
                      tries = 0;
                      rim_cleanup_curlcall(&cc);
                      return res;
                }
                else
                {
                    // no location? result is empty by default
                }
           }
        }

        rim_cleanup_curlcall(&cc);
    }
    else
    {
        if (cc.error != NULL) *(cc.error) = rim_strdup ("Cannot initialize URL library");
        tries = 0;
        return RIM_ERR_FAILED;
    }
    tries = 0;
    return len;
}

static CURLcode rim_process_post (curlcall *cc, char *fields[], char *files[], rim_num *tries)
{
    CURLcode res = CURLE_OK;

    curl_mimepart *fld = NULL;

    // make a form
    if ((cc->form = curl_mime_init(cc->curl)) == NULL)
    {
        rim_end_web(NULL, res, tries, cc);
        if (cc->error != NULL) *(cc->error) = rim_strdup ("Cannot add to header list");
        return RIM_ERR_FAILED;
    }

    // add files, input parsing guarantees [i,i+1] are not NULL
    rim_num i = 0;
    if (files != NULL)
    {
        while (files[i] != NULL) 
        {
            if ((fld = curl_mime_addpart(cc->form)) == NULL)
            {
                rim_end_web(NULL, res, tries, cc);
                if (cc->error != NULL) *(cc->error) = rim_strdup ("Cannot add file to form");
                return RIM_ERR_FAILED;
            }
            if ((res = curl_mime_name(fld, files[i])) != CURLE_OK)
            {
                rim_end_web(cc->error, res, tries, cc);
                return RIM_ERR_FAILED;
            }
            if ((res = curl_mime_filedata(fld, files[i+1])) != CURLE_OK)
            {
                rim_end_web(cc->error, res, tries, cc);
                return RIM_ERR_FAILED;
            }
            i+=2;
        }
    }

    // add fields, input parsing guarantees [i,i+1] are not NULL
    i = 0;
    if (fields != NULL)
    {
        while (fields[i] != NULL) 
        {
            if ((fld = curl_mime_addpart(cc->form)) == NULL)
            {
                rim_end_web(NULL, res, tries, cc);
                if (cc->error != NULL) *(cc->error) = rim_strdup ("Cannot add field to form");
                return RIM_ERR_FAILED;
            }
            if ((res = curl_mime_name(fld, fields[i])) != CURLE_OK)
            {
                rim_end_web(cc->error, res, tries, cc);
                return RIM_ERR_FAILED;
            }
            if ((res = curl_mime_data(fld, fields[i+1], CURL_ZERO_TERMINATED)) != CURLE_OK)
            {
                rim_end_web(cc->error, res, tries, cc);
                return RIM_ERR_FAILED;
            }
            i+=2;
        }
    }

    // headers, say Expect: 100-continue is not wanted 
    static char buf[] = "Expect:";
    struct curl_slist *tmp;
    if ((tmp = curl_slist_append(cc->hlist, buf)) == NULL)
    {
        rim_end_web(NULL, res, tries, cc);
        if (cc->error != NULL) *(cc->error) = rim_strdup ("Cannot add 'expect' to header list");
        return RIM_ERR_FAILED;
    }
    cc->hlist = tmp;
    curl_easy_setopt(cc->curl, CURLOPT_MIMEPOST, cc->form);

    return CURLE_OK;
}

