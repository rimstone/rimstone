// SPDX-License-Identifier: Apache-2.0
// Copyright 2018-2025 Gliim LLC. 
// Licensed under Apache License v2. See LICENSE file.
// On the web http://rimstone-lang.com/ - this file is part of RimStone framework. 

// 
// Main library used at RIM runtime. Most of the functions used
// within markups are implemented here. See also rimtc.c which includes
// common functions shared between RIM runtime and RIM preprocessor.
//

#include "rim.h"


//  functions (local)
static rim_num rim_validate_output ();
static void rim_set_arg0 (char *program, char **arg0);
static void rim_gen_set_content_type(char *v);
static void rim_gen_add_header (char *n, char *v);
static void rim_gen_set_status (rim_num st, char *line);
static void rim_send_header(rim_input_req *iu);
static rim_num rim_gen_util_read (char *content, rim_num len);
static void rim_gen_set_content_length(char *v);
static void rim_server_error ();
static rim_num rim_header_err(rim_config *pc);
static void rim_cant_find_file ();
static inline void rim_check_web_param_type (char *n, rim_num par_ind);
#ifndef RIM_COMMAND
static char *rim_gen_get_env(char *n);
#endif
static void rim_read_child (int ofd, char **out_buf);
static void rim_gen_header_end ();
static void rim_check_set_cookie (char *name, char *val, char *secure, char *samesite, char *httponly, char *safety_clause, size_t safety_clause_len);
rim_num rim_find_par (char *n); 
static inline rim_num rim_write_after_header (bool iserr, rim_config *pc, char *s, rim_num nbyte);
// write-string macros
#define RIM_WRSTR_CUR (rim_get_config()->ctx.req->curr_write_to_string)
#define RIM_WRSTR (rim_get_config()->ctx.req->write_string_arr[RIM_WRSTR_CUR])
#define RIM_WRSTR_LEN (RIM_WRSTR.len)
#define RIM_WRSTR_BUF (RIM_WRSTR.string)
#define RIM_WRSTR_POS (RIM_WRSTR.buf_pos)
#define RIM_WRSTR_ADD (RIM_WRSTR.wlen)
#ifdef DEBUG
#define RIM_WRSTR_ADD_MIN 128 // min memory to add to write-string
#else
#define RIM_WRSTR_ADD_MIN 1024 // min memory to add to write-string
#endif
#define RIM_WRSTR_ADD_MAX 8192 // max memory to add to write-string
#define RIM_WRSTR_ADJMEM(x) ((x) = ((x) < RIM_WRSTR_ADD_MAX ? 2*(x):(x)))

//hash size for uploaded files
#define RIM_HASH_UPLOAD_SIZE 128

#ifndef RIM_COMMAND
#include "fcgi/fcgiapp.h"
static FCGX_Stream *rim_fcgi_in = NULL, *rim_fcgi_out = NULL, *rim_fcgi_err = NULL;
static FCGX_ParamArray rim_fcgi_envp;
#endif
char rim_finished_output = 0;

extern rim_num rim_end_program;
extern rim_num _rim_sprm_run_tot;
extern rim_ipar _rim_sprm_par[];



//
//
//Find the index in global set-params for name n and return it. Error out if param set/never used, or duplicated.
//This is for URL input params only!!
//
//
rim_num rim_find_par (char *n)
{
    rim_num found;
    rim_num *i = rim_find_hash (&rim_paramhash, n, 0, &found, false); // rim_paramhash is not rimstone memory
    if (found != RIM_OKAY)  // this is if set by URL but not used in the application
    {
        rim_bad_request();
        rim_report_error ("Parameter [%s] was set by the request URL, but is never used by the application", n);
        return -1; // never reaches
    }
    if (_rim_sprm_par[*i].set) // this is set more than once in URL
    {
        rim_bad_request();
        rim_report_error ("Parameter [%s] was set more than once by the request URL", n);
        return -1; // never reaches
    }
    return *i;
}


// 
// Initialize rim_input_req structure for fetching input URL data
// req is URL structure used to hold input data.
// DO NOT USE rim_get_config() here as it's not available yet!
//
void rim_init_input_req (rim_input_req *req)
{
    rim_num i;
    for (i=0; i < RIM_MAX_NESTED_WRITE_STRING; i++)
    {
        req->write_string_arr[i].string = NULL;
        req->write_string_arr[i].len = 0;
        req->write_string_arr[i].buf_pos = 0;
        req->write_string_arr[i].notrim = 0;
        req->write_string_arr[i].wlen = RIM_WRSTR_ADD_MIN;
    }
    req->curr_write_to_string = -1;// each write-to-string first increase it 
    req->disable_output = 0;
    req->app = NULL;
    req->if_none_match = NULL;
    req->cookies = NULL;
    req->num_of_cookies = 0;
    req->sent_header = 0;
    req->data_was_output = 0;
    req->silent = 0;
    req->ec = 0;
    req->is_shut = 0;
    req->header=NULL; // no custom headers, set to non-NULL for custom headers
    req->body = RIM_EMPTY_STRING;
    req->name = RIM_EMPTY_STRING;
    req->body_len = 0;
    req->method = RIM_OKAY;
    req->sub_depth = 1;
    req->upload = NULL; // no uploaded files to begin with
    rim_finished_output = 0; // reset finish-output indicator
}

//
//Do not trim the last new line in write-string
//
void rim_write_to_string_notrim ()
{
    // must be (RIM_WRSTR_CUR < RIM_MAX_NESTED_WRITE_STRING); // see comment in rim_write_to_string_length ()
    RIM_WRSTR.notrim = 1;
}

// 
// Returns length of current top-level write-string (or equivalent API which is
// rim_write_to_string()) string being written.
//
rim_num rim_write_to_string_length ()
{
    rim_input_req *req = rim_get_config()->ctx.req;
    // it must be RIM_WRSTR_CUR < RIM_MAX_NESTED_WRITE_STRING); // overflow if asking within the last level 
                // because the level above it does not exist. We always show the length of previous write-string
                // and that is one level up
    return req->write_string_arr[req->curr_write_to_string+1].buf_pos;
}

// 
// Write to string. str is either a RIM-allocated string into which to write
// or NULL, which signifies end of string writing.
// Once non-NULL string str is passed here, all future writing (such as print-noenc
// or print-web etc) goes to this string, until this function is called with NULL.
// Writing to string can be nested, so writing to string2 (while writing to string1)
// will write to string2 until NULL is passed, when it switches back to string1.
//
void rim_write_to_string (char **str)
{
    if (str == NULL)
    {
        // stop writing to string
        if (RIM_WRSTR_CUR<0)
        {
            rim_report_error ("Cannot stop writing to string if it was never initiated, or if stopped already");
        }
        if (RIM_WRSTR_BUF == NULL)
        {
            rim_report_error ("Cannot find write-string data block");
        }
        if (RIM_WRSTR.notrim == 0)
        {
            while (isspace(RIM_WRSTR_BUF[RIM_WRSTR_POS-1])) RIM_WRSTR_POS--;  
            RIM_WRSTR_BUF[RIM_WRSTR_POS] = 0;
        }
        RIM_WRSTR_BUF = rim_realloc (rim_mem_get_id(RIM_WRSTR_BUF), RIM_WRSTR_POS+1); // resize memory to just what's needed
        rim_mem_set_len (RIM_WRSTR_BUF, RIM_WRSTR_POS+1); // exact length of result
                                                                     // this is the exact length of written string that's allocated
        *(RIM_WRSTR.user_string) =  RIM_WRSTR_BUF;
        // Do NOT set RIM_WRSTR_POS = 0 because then function rim_write_to_string_length()
        // couldn't possibly work

        // restore notrim so all future write-strings are not 'notrim'
        RIM_WRSTR.notrim = 0;
        // no more string to write
        RIM_WRSTR_BUF = NULL;
        RIM_WRSTR_CUR--;
    }
    else
    {
        // start writing to string
        // Once curr_write_to_string is not -1 (i.e. 0 or more), there is a string writing in progress, even if 
        // nothing has been written to it yet. So the condition for "are we in string writing" is curr_write_to_string!=-1
        RIM_WRSTR_CUR++;
        if (RIM_WRSTR_CUR >= RIM_MAX_NESTED_WRITE_STRING)
        {
            rim_report_error ("Too many nesting levels of writing to string in progress, maximum [%d] nesting levels", RIM_MAX_NESTED_WRITE_STRING);
        }
        //
        // assign an empty string to *str, in case it's used somehow within write-string (trying to do recursion,
        // which is not allowed)
        *str = RIM_EMPTY_STRING;
        RIM_WRSTR.user_string = str;

        // Use internal pointer and memory to build a string. Original user string remains empty until the end.
        RIM_WRSTR_ADD = RIM_WRSTR_ADD_MIN; // start with min mem
        RIM_WRSTR_LEN = RIM_WRSTR_ADD; 
        RIM_WRSTR_BUF= (char*) rim_malloc (RIM_WRSTR_LEN); // this is not the final alloc, so this length will not be final
        RIM_WRSTR_POS = 0;

    }
}


// 
// Send html header out for a dynamically generated page. It is always no-caching.
// req is input request.
// If HTML output is disabled, NO header is sent.
//
void rim_output_http_header(rim_input_req *req)
{
    if (req->sent_header == 1) 
    {
        return;
    }
    if (rim_get_config()->ctx.req->disable_output == 1) return;
    req->sent_header = 1; 
                // complain that header hasn't been sent yet! and cause fatal error at that.
    rim_send_header(req);
}

static void rim_check_set_cookie (char *name, char *val, char *secure, char *samesite, char *httponly, char *safety_clause, size_t safety_clause_len)
{
    char *chk = name;
    // Per rfc6265, cookie name must adhere to this and be present
    while (*chk != 0)
    {
        if (*chk < 33 || *chk == 127 || // this excludes space, tab and control chars
            *chk == '(' || *chk == ')' || *chk == '@' || *chk == ',' || *chk == ';' || *chk == ':' || *chk == '\\' ||
            *chk == '"' || *chk == '/' || *chk == '[' || *chk == ']' || *chk == '?' || *chk == '=' ||
            *chk == '{' || *chk == '}')
        {
            rim_report_error ("Cookie name [%s] is invalid at [%s]", name, chk);
        }
        chk ++;
    }
    if (name[0] == 0) rim_report_error ("Cookie name is empty");
    // Per rfc6265, cookie value must adhere to this and be present
    chk = val;
    while (*chk != 0)
    {
        if (*chk < 33 || *chk == 127 || // this excludes space, tab and control chars
            *chk == ',' || *chk == ';' || *chk == '\\' ||
            (*chk == '"' && (chk != val && *(chk+1) != 0))) // quote is allowed only as first and last byte, not inside
        {
            rim_report_error ("Cookie value [%s] is invalid at [%s]", val, chk);
        }
        chk ++;
    }
    if (val[0] == 0) rim_report_error ("Cookie value is empty");
    if (strcmp(secure, "Secure; ") && strcmp (secure, ""))
    {
        rim_report_error ("Cookie 'secure' can be only on or off, it is [%s]", secure);
    }
    if (strcmp(httponly, "HttpOnly; ") && strcmp (httponly, ""))
    {
        rim_report_error ("Cookie HttpOnly can be only on or off, it is [%s]", httponly);
    }
    if (samesite[0] != 0 && strcmp(samesite, "Strict") && strcmp (samesite, "Lax") && strcmp (samesite, "None"))
    {
        rim_report_error ("Cookie SameSite can be only empty, Strict, Lax or None");
    }
    if (samesite[0] != 0) snprintf (safety_clause, safety_clause_len, "; %s%sSameSite=%s", secure, httponly, samesite);
    else snprintf (safety_clause, safety_clause_len, "; %s%s", secure, httponly);
}


// 
// Sets cookie that's to be sent out when header is sent. req is input request, cookie_name is the name of the cookie,
// cookie_value is its value, path is the URL for which cookie is valid, expires is the date of exiration.
// SameSite is empty by default. Strict is to prevent cross-site request exploitations, for enhanced safety. Otherwise samesite can be
// Strict, Lax or None.
// httponly can be either "HttpOnly; " or empty string
// cookies[].is_set_by_program is set to  1 if this is the cookie we changed (i.e. not original in the web input).
//
void rim_set_cookie (rim_input_req *req, char *cookie_name, char *cookie_value, char *path, char *expires, char *samesite, char *httponly, char *secure)
{
    if (req->data_was_output == 1) 
    {
        rim_report_error ("Cookie can only be set before any data is output, and either before or after header is output.");
    }

    char safety_clause[200];
    rim_check_set_cookie (cookie_name, cookie_value, secure, samesite, httponly, safety_clause, sizeof(safety_clause));

    rim_num ind;
    char *exp = NULL;
    rim_find_cookie (req, cookie_name, &ind, NULL, &exp);
    if (ind == -1)
    {
        if (req->num_of_cookies+1 >= RIM_MAX_COOKIES)
        {
            rim_report_error ("Too many cookies [%ld]", req->num_of_cookies+1);
        }
        ind = req->num_of_cookies;
        req->num_of_cookies++;
    }
    else
    {
        rim_free_int (req->cookies[ind].data);
    }
    char cookie_temp[RIM_MAX_COOKIE_SIZE + 1];
    if (expires == NULL || expires[0] == 0)
    {
        if (path == NULL || path[0] == 0)
        {
            snprintf (cookie_temp, sizeof(cookie_temp), "%s=%s%s", cookie_name, cookie_value, safety_clause);
        }
        else
        {
            snprintf (cookie_temp, sizeof(cookie_temp), "%s=%s; Path=%s%s", cookie_name, cookie_value, path, safety_clause);
        }
    }
    else
    {
        if (path == NULL || path[0] == 0)
        {
            snprintf (cookie_temp, sizeof(cookie_temp), "%s=%s; Expires=%s%s", cookie_name, cookie_value, expires, safety_clause);
        }
        else
        {
            snprintf (cookie_temp, sizeof(cookie_temp), "%s=%s; Path=%s; Expires=%s%s", cookie_name, cookie_value, path, expires, safety_clause);
        }
    }
    req->cookies[ind].data = rim_strdup (cookie_temp);
    req->cookies[ind].is_set_by_program = 1;
}

// 
// Find cookie based on name cookie_name. req is input request. Output: ind is the index in the cookies[] array in
// req, path/exp is path and expiration of the cookie. 
// When searching for a cookie, we search the cookie[] array, which we may have added to or deleted from, so it
// may not be the exact set of cookies from the web input.
// Returns cookie's value.
//
char *rim_find_cookie (rim_input_req *req, char *cookie_name, rim_num *ind, char **path, char **exp)
{

    rim_num ci;
    rim_num name_len = strlen (cookie_name);
    for (ci = 0; ci < req->num_of_cookies; ci++)
    {
        // Cookie (trimmed) must start with name=value. After that, other options may be in any order
        // But it is always ; Path=   ; Expires=  etc. - we set those in this exact format. We don't get any of 
        // those from the server - we set them, so we can search easily.
        if (!strncmp (req->cookies[ci].data, cookie_name, name_len) && req->cookies[ci].data[name_len] == '=')
        {
            if (ind != NULL) *ind = ci;
            char *val = req->cookies[ci].data+name_len+1;
            char *semi = strchr (val, ';');
            char *ret = NULL;
            if (semi == NULL)
            {
                ret = rim_strdup (val);
            }
            else
            {
                *semi = 0;
                ret = rim_strdup (val);
                *semi = ';';
            }
            if (path != NULL)
            {
                char *p = strcasestr (val, "; Path=");
                if (p != NULL)
                {
                    semi = strchr (p + 7, ';');
                    if (semi != NULL) *semi = 0;
                    *path = rim_strdup (p + 7);
                    if (semi != NULL) *semi = ';';
                }
                else
                {
                    *path = NULL;
                }
            }
            if (exp != NULL)
            {
                char *p = strcasestr (val, "; Expires=");
                if (p != NULL)
                {
                    semi = strchr (p + 10, ';');
                    if (semi != NULL) *semi = 0;
                    *exp = rim_strdup (p + 10);
                    if (semi != NULL) *semi = ';';
                }
                else
                {
                    *exp = NULL;
                }
            }
            return ret;
        }
    }
    if (ind != NULL) *ind = -1;
    return RIM_EMPTY_STRING;
}

// 
// Delete cookie with name cookie_name. req is input request. "Deleting" means setting value to 'deleted' and 
// expiration day to Epoch start. But the cookies is still there and will be sent out in header response, however
// it won't come back since browser will delete it (due to expiration date and not because of 'deleted').
// Returns index in cookies[] array of the cookie we just deleted or RIM_ERR_EXIST if does not exist.
// cookies[].is_set_by_program is set to  1 if this is the cookie we deleted (i.e. not original in the web input).
// If path is specified, we use it; if not, we assume it was the same default one (which generally works unless
// you mix different paths, such as via different reverse proxies
rim_num rim_delete_cookie (rim_input_req *req, char *cookie_name, char *path, char *secure)
{

    rim_num ci;
    char *rpath = NULL;
    char *exp = NULL;
    char *val = rim_find_cookie (req, cookie_name, &ci, &rpath, &exp);
    if (ci != -1)
    {
        rim_free_int (req->cookies[ci].data);
        char del_cookie[300];
        char safety_clause[200];
        rim_check_set_cookie (cookie_name, "deleted", secure, "", "", safety_clause, sizeof(safety_clause));
        if (path != NULL)  rpath=path;
        if (rpath != NULL)
        {
            snprintf (del_cookie, sizeof (del_cookie), "%s=deleted; Path=%s; Max-Age=0; Expires=Thu, 01 Jan 1970 01:01:01 GMT%s", cookie_name, rpath, safety_clause);
        }
        else
        {
            snprintf (del_cookie, sizeof (del_cookie), "%s=deleted; Max-Age=0; Expires=Thu, 01 Jan 1970 01:01:01 GMT%s", cookie_name, safety_clause);
        }
        req->cookies[ci].data = rim_strdup (del_cookie);
        req->cookies[ci].is_set_by_program = 1;
        rim_free_int (rpath);
        rim_free_int (exp);
        rim_free_int (val);
        return ci;
    }
    return RIM_ERR_EXIST;
}


//
// Send header out. This does it only for web output, i.e. nothing happens for command line program.
// Only changed cookies are sent back (since unchanged ones are already in the browser). Cache is no-cache
// because the html output is ALWAYS generated. Cookies are secure if this is over https connection.
// Charset is always UTF-8.
//
// req can be NULL here, if called from oops page, or req may have very little data in it
// We send header out ONLY:
// 1. if explicitly called via rim_send_header or rim_output_http_header
// 2. it is an error
// 3. we send out a file
// This is important because we do NOT want to send header out just because some text is going
// out - this would break a lot of functionality, such as sending custom headers. 
// HEADER MUST BE EXPLICITLY SENT OUT.
// If req->header is not-NULL (ctype, cache_control, status_id, status_text or control/value), then
// custom headers are sent out. This way any kind of header can be sent.
//
static void rim_send_header(rim_input_req *req)
{

    rim_header *header = req->header;


    if (header!=NULL  && header->ctype != NULL)
    {
        //
        // Set custom content type if available
        //
        rim_gen_set_content_type(header->ctype);
    }
    else
    {
        rim_gen_set_content_type("text/html;charset=utf-8");
    }

    
    if (header!=NULL  && header->cache_control != NULL)
    {
        //
        // Set custom cache control if available
        //
        rim_gen_add_header ("Cache-Control", header->cache_control);
    }
    else
    {
        // this is for output from RIM files only! for files, we cache-forever by default
        rim_gen_add_header ("Cache-Control", "max-age=0, no-cache");
        rim_gen_add_header ("Pragma", "no-cache");
        // the moment first actual data is sent, this is immediately flushed by fcgi?
    }

    //
    // Set status if available
    //
    if (header!=NULL && header->status_id != 0 && header->status_text != NULL)
    {
        rim_gen_set_status (header->status_id, header->status_text);
    } else rim_gen_set_status (200, "OK");

    // 
    // Set any custom headers if available
    //
    if (header!=NULL)
    {
        // add any headers set from the caller
        rim_num i;
        for (i = 0; i<RIM_MAX_HTTP_HEADER; i++)
        {
            if (header->control[i]!=NULL && header->value[i]!=NULL)
            {
                // we use add_header because it allows multiple directives of the same kind
                // but must not make duplicates of what's already there, except for Set-Cookie
                rim_gen_add_header (header->control[i], header->value[i]);
            }
            else break;
        }
    }
}


// 
// This is called when fatal error happens in program. Catches all errors, from program's report-error to SIGSEGV.
// Report an error in program. printf-like function that outputs error to stderr
// After this, we don't exit, we jump to the end of request, so it will process the next request for SERVICE
// NOTE: the two functional elements here:
// 1. rim_check_transaction() to rollback db transactions,
// 2. rim_error_request() to jump to the next request
// The above must be present in do_printf() under rep_error check.
//
//
void _rim_report_error (char *format, ...) 
{

    // THIS FUNCTION MUST NOT USE RIM_MALLOC NOR MALLOC
    // as it can be used to report out of memory errors


    // get error message passed, format it into a single string
    // This has no dependencies on pc or anything really
    static char errtext[RIM_MAX_ERR_LEN + 1];
    va_list args;
    va_start (args, format);
    rim_num err_len = vsnprintf (errtext, sizeof(errtext), format, args);
    va_end (args);

    rim_config *pc = rim_get_config();
    if (pc == NULL)
    {
        RIM_FATAL ("Program context is empty, error [%s]", errtext);
    }


    // Never error out more than once, if we do, say we did and move on to the next request
    // UnLikely to happen but still
    if (pc->ctx.rim_report_error_is_in_report == 1) 
    {
        // Reason for exiting: if rollback fails (in rim_check_transaction below)
        // , then we would proceed to next request, and this can continue
        // previous request's transaction, leading to corrupt data
        RIM_FATAL ("Fatal error during error reporting, error [%s]", errtext); 
        return; // should never happen
    }
    pc->ctx.rim_report_error_is_in_report = 1; 

    // Rollback any open transactions. Error can happen in there too, but if it dies, we exit the process (see above)
    // rather than risk complications
    rim_check_transaction (1);



    // send to stderr (for web client, goes to web server error log, for standalone to stderr)

    // we will try to make 500 Server Error here, but if the header has already been output, it won't come out
    // for example rim_bad_request() may have been done prior to this, or just out-header
    // We output generic error message to standard output, so no details. The details of the error are **always** in the error stream.
    // New line is important at the end of it so if you're looking in terminal, this message may be invisible as it gets overwritten visually
    rim_server_error();
#define RIM_GENERR "Your request has produced an error. Please check error log for more details.\n"
    rim_gen_write (false, RIM_GENERR, sizeof (RIM_GENERR)-1);

    // Output error stack trace before the actual error, so it's easy to see the error at the bottom
    rim_get_stack(errtext);
    //
    // write to stderror (server log)
    // The message is last, also so it is picked up by Apache log (which may split it into multiple lines so it gets hard to see where it was)
    // and especially hard to parse out in test
    rim_gen_write (true, "--- ", 4);
    rim_gen_write (true, errtext, err_len);
    rim_gen_write (true, "\n", 1);



    //
    //
    // if this report-error happens anywhere outside a request, we will just start a new one
    // Otherwise, close this request, release resources and process the next one (for SERVICE).
    //
    //
    pc->ctx.req->ec = RIM_DISTRESS_STATUS; // this is the value for abnormal termination, if exit status is still 0
    rim_error_request(1);// go to process the next request
}



// 
// Decode string encoded previously (web or url encoding). enc_type is RIM_WEB or
// RIM_URL. String v (which is encoded at the entry) holds decoded value on return.
// inlen is the number of bytes to decode, negative if all until null-char (strlen)
// alloc is true if this is allocated memory (rimstone mem), false otherwise
// Return value is the length of decoded string.
// status, if not null, is RIM_OKAY if all bytes decoded, or the bytes decoded (and the rest weren't)
//
rim_num rim_decode (rim_num enc_type, char *v, rim_num inlen, bool alloc, rim_num *status)
{

    if (alloc)
    {
        rim_num vlen= rim_mem_get_len (v);
        if (inlen < 0) inlen = vlen;
        else if (inlen > vlen) rim_report_error ("Memory read requested of length [%ld] but only [%ld] allocated", inlen, vlen);
    }
    else
    {
        if (inlen < 0) inlen = (rim_num)strlen(v);
    }

    rim_num i;
    rim_num j = 0;
    if (enc_type == RIM_WEB)
    {
        for (i = 0; i < inlen; i ++)
        {
            if (v[i] == '&')
            {
                if (!strncmp (v+i+1, "amp;", 4))
                {
                    v[j++] = '&';
                    i += 4;
                }
                else if (!strncmp (v+i+1, "quot;", 5))
                {
                    v[j++] = '"';
                    i += 5;
                }
                else if (!strncmp (v+i+1, "apos;", 5))
                {
                    v[j++] = '\'';
                    i += 5;
                }
                else if (!strncmp (v+i+1, "lt;", 3))
                {
                    v[j++] = '<';
                    i += 3;
                }
                else if (!strncmp (v+i+1, "gt;", 3))
                {
                    v[j++] = '>';
                    i += 3;
                }
                else v[j++] = v[i];
            }
            else
            {
                v[j++] = v[i];
            }
        }
        v[j] = 0;
    }
    else if (enc_type == RIM_URL)
    {
        // check if each examined hex digit is valid, and if not break and finish by setting 0 at the end
        // this covers %X<null> pr %XY where X or Y are invalid of %<null> and such
        // this macro is specific to the following loop only
#define URLDIG(x,r)  if ((x) >='A' && (x) <= 'F') (r)=((x)-'A')+10;\
                else if ((x) >='a' && (x) <= 'f') (r)=((x)-'a')+10;\
                else if ((x) >='0' && (x) <= '9') (r)=(x)-'0';\
                else break;
        for (i = 0; i < inlen; i ++)
        {
            if (v[i] == '%')
            {
                int h;
                int l;
                i++;
                URLDIG(v[i],h);
                i++;
                URLDIG(v[i],l);
                v[j++] = h*16+l;
            } else if (v[i] == '+')
            {
                v[j++] = ' ';
            }
            else
            {
                v[j++] = v[i];
            }
        }
        v[j] = 0;
        if (status) { if (i != inlen) *status = i; else *status = RIM_OKAY; }
    }
    else 
    {
        rim_report_error ("Unknown decoding type");
    }
    //
    // Should never realloc because decoding is done in place of an existing input, and realloc would create a new pointer
    // and the old one would become a dangling one, leading to a need to change the input pointer and to keep track of pointer assignments,
    // so that any other memory pointing there is now changed. We don't do this to keep memory management super high performance.
    //
    if (alloc) rim_mem_set_len (v, j+1);
    return j;
}

//
// Lock a file. filepath is the file. lock_fd is the file descriptor of the locked
// open file. Returns RIM_OKAY if locked and other error codes if not.
// This is used for locking resources between various processes. Once a process exits,
// the lock is released - meaning if you fork() a process and then exit, the forked process
// will NOT have the lock.
//
rim_num rim_lockfile(char *filepath, rim_num *lock_fd)
{
    struct flock lock;
    rim_num fd;

    /* Invalid path? */
    if (filepath == NULL || *filepath == '\0')
    {
        RIM_ERR0;
        return RIM_ERR_INVALID;
    }

    /* Open the file. No checking for EINTR, as it is fatal in chandle.c */
    fd = open(filepath, O_RDWR | O_CREAT, 0600);

    if (fd == -1)
    {
        RIM_ERR;
        return RIM_ERR_CREATE;
    }

    /* Application must NOT close input/output/error, or those may get occupied*/

    /* Exclusive lock, cover the entire file (regardless of size). */
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    if (fcntl(fd, F_SETLK, &lock) == -1)
    {
        RIM_ERR;
        /* Lock failed. Close file and report locking failure. */
        close(fd);
        return RIM_ERR_FAILED;
    }
    if (lock_fd != NULL) *lock_fd = fd;
    // success, the file is open and locked, and will remain locked until the process ends
    // or until the caller does close (*lock_fd);
    return RIM_OKAY;
}

// 
// Check web input param for type. It's always natively string, but we'll convert to number or boolean if the target 
// param is defined as such. If okay, then the string value will be converted to a number or boolean; if not, the error 
// message is displayed. If it's a string, then it's set (and it's not allocated b/c it's not in input). 
// par_ind is the parameter index in the global array where each has a unique and immutable entry; n is the name of param.
//
static inline void rim_check_web_param_type (char *n, rim_num par_ind)
{
    if (_rim_sprm_par[par_ind].type != RIM_DEFSTRING)
    {
        char *sv = _rim_sprm_par[par_ind].tval.value;
        if (_rim_sprm_par[par_ind].type == RIM_DEFNUMBER) // type of param , check it
        {
            rim_num st;
            _rim_sprm_par[par_ind].tval.numval = rim_str2num (sv, 10, &(st));
            if (st != RIM_OKAY) rim_report_error ("Parameter [%s] is a number, but input data [%s] is not a number", n, sv);
            _rim_sprm_par[par_ind].set = true;
        }
        else if (_rim_sprm_par[par_ind].type == RIM_DEFDOUBLE) // type of param , check it
        {
            rim_num st;
            _rim_sprm_par[par_ind].tval.dblval = rim_str2dbl (sv, &(st));
            if (st != RIM_OKAY) rim_report_error ("Parameter [%s] is a double, but input data [%s] is not a double", n, sv);
            _rim_sprm_par[par_ind].set = true;
        }
        else if (_rim_sprm_par[par_ind].type == RIM_DEFBOOL)
        {
            if (!strcmp (sv, "true")) _rim_sprm_par[par_ind].tval.logic = true;
            else if (!strcmp (sv, "false")) _rim_sprm_par[par_ind].tval.logic = false;
            else rim_report_error ("Parameter [%s] is a boolean, but input data [%s] is not a boolean", n, sv);
            _rim_sprm_par[par_ind].set = true;
        }
        else
        {
            rim_bad_request();
            rim_report_error ("Parameter [%s] is not of string type, but request URL tries to set its value to a string", n);
        }
    }
    else
    {
        _rim_sprm_par[par_ind].alloc = false;
        _rim_sprm_par[par_ind].set = true;
    }
}


// 
// Get input parameters from web input in the form of
// name/value pairs, meaning from a GET, POST, PUT, PATCH or DELETE (with OPTIONS to support for CORS).  
// If 'method' if NULL, environment REQUEST_METHOD is read, otherwise 'input' must be specified as input for query string.
// this is useful for passing URL from command line.
// For any method we read both body and query string. For some like GET, body is unusual but not forbidden.
// query string is based on environment variable QUERY_STRING
// Returns 1 if ok, 0 if not ok but handled (such as Forbidden), otherwise Errors out.
// Other information is obtained too such as HTTP_REFERRED that could be used to disallow
// viewing images unless referred to by this site (not a functionality here, it
// can be implemented in rimstone application).
// ETag/If-non-match is obtained for cache management. Cookies are obtained from the client
// ONLY the first time this is called in a request - we may alter cookies later, so if rim_get_input()
// is called again in the same request, cookies are NOT obtained again.
// Input parameters are stored in req variable, where they can be obtained from
// by the program. Files are uploaded automatically and are parameterized in the form of xxx_location, xxx_filename,
// xxx_ext, xxx_size, xxx_id. So if input parameter was myparam, it would be 
// myparam_location, myparam_filename etc. _location is the local file path where file is
// stored. _filename is the actual HTML filename parameter. _ext is extension (with . in 
// front of it, lower cased). _size is the size of the file. _id is the id produced by
// rim_get_document_id() and also id is what _location is based on. If file was empty, then
// all params but _filename are empty.
// If 'input' is specified, it overrides what's from QUERY_STRING. This is only if 'method' present.
//
rim_num rim_get_input(rim_input_req *req, char *method, char *input)
{

    rim_config *pc = rim_get_config();
    char *req_method = NULL;
    char *qry = NULL;
    char *cont_type = NULL;
    char *cont_len = NULL;
    rim_num cont_len_byte = 0; // default zero if content-length not specified
    char *content = NULL;
    char *cookie = NULL;

    // so now all strings are NULL (and other types), numbers 0, boolean false
    rim_num ap;
    for (ap = 0; ap < _rim_sprm_run_tot; ap++) _rim_sprm_par[ap].set = false;

    // some env vars are obtained right away, other are rarely used
    // and are obtaineable from $$ variables
    RIM_STRDUP (req->referring_url, rim_getenv ("HTTP_REFERER"));
    // when there is a redirection to home page, referring url is empty

    char *sil = rim_getenv ("RIM_SILENT_HEADER");
    // Silent header is special as it is passed from mrim, if -z is used there. So check for it.
    if (sil[0] == 0) 
    {
        sil = rim_getenv_os ("RIM_SILENT_HEADER");
    }
    if (!strcmp (sil, "yes")) 
    {
        req->silent = 1;
    }

    char *nm = rim_getenv ("HTTP_IF_NONE_MATCH");
    if (nm[0] != 0)
    {
        RIM_STRDUP (req->if_none_match, nm);
    }

    // this function is often called in "simulation" of a request. ONLY the first request gets cookies
    // from the client (which is HTTP_COOKIE). After this first request, we may alter cookies in memory,
    // and so we do NOT get cookies again from the client.
    if (req->cookies == NULL)
    {
        cookie = rim_getenv ("HTTP_COOKIE"); 
        // we can set cookies, so whether they come in or not from the caller, we have to have them and they are set to zero
        // since is_set_by_program flag MUST be zero, so memset() is a must.
        static rim_cookies cookies[RIM_MAX_COOKIES];
        memset (cookies, 0, sizeof (cookies));
        req->cookies = cookies;
        if (cookie[0] != 0)
        {
            rim_num tot_cookies = 0;
            char *semi;
            while (1)
            {
                if (tot_cookies >= RIM_MAX_COOKIES) 
                {
                    rim_bad_request();
                    rim_report_error("Too many cookies [%ld]", tot_cookies);
                }
                char *ew = strchr (cookie, ';');
                if (ew != NULL)
                {
                    semi = ew;
                    *ew = 0; // changing environment memory! (reverting to it below)
                    ew++;
                    while (isspace(*ew)) ew++;
                } else semi = NULL;
                req->cookies[tot_cookies].data = rim_strdup (cookie);
                if (semi != NULL) *semi = ';'; // make sure environment memory is unchanged!
                tot_cookies++;
                if (ew == NULL) break;
                cookie = ew;
            }
            req->num_of_cookies = tot_cookies;
        }
        else 
        {
            req->num_of_cookies = 0;
        }
    }

    // request method, GET or POST
    // method, input override environment
    // if method, 'input' will overrde QUERY_STRING
    if (method != NULL)
    {
        req_method = method;
    }
    else
    {
        req_method = rim_getenv ("REQUEST_METHOD");
    }
   
    if (req_method[0] == 0) 
    {
        rim_bad_request();
        rim_report_error ("REQUEST_METHOD environment variable is not found");
    }

    rim_num is_multipart = 0;

    // if method,input used, then we only take input for GET, not POST
    // because we convert any POST to GET in the previous code (or not and it remains body) and any files
    // are already saved as actual files
    if (method != NULL)
    {
        qry = input;
    }
    else
    {
        // get GET/DELETE query string
        // POST/PATCH/PUT request can also have additional query string in URL string. Add it if present
        qry = rim_getenv ("QUERY_STRING");
    }
    rim_num qry_len = (rim_num)strlen (qry); // length of query string

    // content type is generally not specified for GET or DELETE, but it may be
    // so this is generic processing with a few constraints
    cont_type = rim_getenv ("CONTENT_TYPE");
    // size of input data
    cont_len = rim_getenv ("CONTENT_LENGTH");
    if (cont_type[0] != 0) 
    {
        if (cont_len[0] != 0)
        {
            cont_len_byte = atoi (cont_len);
        } else cont_len_byte = 0;
    } else cont_len_byte = 0;


    char *new_url = rim_inp_url;
    char *new_body;
    if (cont_len_byte + 32 < RIM_MAX_SIZE_OF_BODY) new_body = rim_inp_body; else new_body = (char*)rim_malloc (cont_len_byte + 32);

    rim_num new_url_ptr = 0;
    rim_num would_write;

    bool is_post = false;
    bool is_patch = false;
    bool is_get = false;
    bool is_delete = false;
    bool is_put = false;
    bool is_other = false;
    bool is_query_string = false; // is the body x-www-form-urlencoded

    if (!strcasecmp(req_method, "POST")) {is_post=true;req->method=RIM_POST;}
    else if (!strcasecmp(req_method, "GET")) {is_get=true;req->method=RIM_GET;}
    else if (!strcasecmp(req_method, "PATCH")) {is_patch=true;req->method=RIM_PATCH;}
    else if (!strcasecmp(req_method, "PUT")) {is_put=true;req->method=RIM_PUT;}
    else if (!strcasecmp(req_method, "DELETE")) {is_delete=true;req->method=RIM_DELETE;}
    else {is_other=true;req->method=RIM_OTHER;}

    if (cont_type[0] != 0)
    {
        // handle POST request, first check content type
        // x-www-form-urlencoded and multipart/form-data are two content mime types that all clients
        // must support. urlencode is for non-binary and multipart is when files are involved. It is
        // one or the other. Per https://datatracker.ietf.org/doc/html/rfc7578, multipart/mixed is 
        // deprecated and is not implemented here.
        char *mult = "multipart/form-data;";
        char *mform = NULL;
        if ((mform = strcasestr (cont_type, mult)) != NULL)
        {
            if (mform == cont_type || *(mform - 1) == ';' || isspace (*(mform - 1)))
            {
                is_multipart = 1;
            }
        }
        if (!strcasecmp (cont_type, "application/x-www-form-urlencoded")) is_query_string = true;
        if (cont_len_byte  != 0)
        {
            // If query string, then limit of URL, otherwise max upload for file 
            if (!is_query_string)
            {
                if (cont_len_byte >= pc->app.max_upload_size)
                {
                    rim_bad_request();
                    rim_report_error ("Upload file size too large [%ld]", cont_len_byte);
                }
            }
            else
            {
                if (cont_len_byte >= RIM_MAX_SIZE_OF_URL)
                {
                    rim_bad_request();
                    rim_report_error ("Query string larger than the limit of [%d] bytes (1)", RIM_MAX_SIZE_OF_URL);
                }
            }
            // Must have 2 extra bytes b/c for URL two nulls in a row signify the end of name/value pairs
            // which are normally separated by a null
            content = new_body;
            // get input data
            if (rim_gen_util_read (content, cont_len_byte) != 1)
            {
                rim_bad_request();
                rim_report_error ("Error reading request body");
            }
            content [cont_len_byte] = content[cont_len_byte+1] = 0;

            // for example, GET might use body to convey an image of an object that needs to be found
            // plain query string may not be enough. Or a json document needed to find the resource.
            // Here we check if body of message is not multipart
            req->body = content;                
            req->body_len = cont_len_byte;
            rim_mem_set_len(req->body, cont_len_byte+1);
        }
    }
    else
    {
        // if content-type is empty then we specify empty "content" variable so that further code can for instance
        // add URL fields to it and content isn't NULL for the code that tries to copy it or realloc it.
        // we enforce that content-length must not be present or must be 0 in this case.
        // This works for POST/PUT/PATCH and for GET/DELETE (where content will be created for them)
        // Must have 2 extra bytes b/c for URL two nulls in a row signify the end of name/value pairs
        // which are normally separated by a null
        content = new_url;
        content[0] = 0;
    }

    // After this point, cont_len_byte is really length of any query string, including for file uploading
    // Because if it's any kind of content that's not form-data or urlencoded, then it's in req->body and req->body_len


    // If client asking for options, present options handled
    // For better CORS (Cross Origin Resource Sharing), use referrer in your rimstone code to determine
    // access or no-access
    if (!strcasecmp(req_method, "OPTIONS")) {
        rim_gen_set_status (204, "No Content");
        rim_gen_add_header ("Allow", "OPTIONS, GET, POST, PATCH, PUT, DELETE");
        rim_gen_add_header ("Access-Control-Allow-Methods", "OPTIONS, GET, POST, PATCH, PUT, DELETE");
        rim_gen_add_header ("Cache-Control", "max-age: 86400");
        rim_gen_header_end ();
        return 0;
    }
    else if (is_get || is_delete || is_other)
    {
        // if body is a query string, then it should be added to whatever query string is in URL
        // if not a query string, then content from body is ignored for the URL building purposes
        if (!is_query_string) 
        {
            cont_len_byte = 0; // if there is any query string after this, those params are on their own,
            // Must have 2 extra bytes b/c for URL two nulls in a row signify the end of name/value pairs
            // which are normally separated by a null
            content = new_url;
            content[0] = 0;
        }
    }
    else if (is_post || is_put || is_patch)
    {
        //
        // When POST has no files, it's really just one query string in the body, which is
        // application/x-www-form-urlencoded
        // If is_multipart  is 1, then there are binary documents encoded between "boundaries"
        // Any other content type is allowed too, for instance application/json or anything else
        //
        if (cont_type[0] != 0)
        {
            // for non-empty content type, content len must be present not 0
            if (cont_len[0] == 0)
            {
                rim_bad_request();
                rim_report_error ("Missing content length");
            }
            if (cont_len_byte == 0)
            {
                rim_bad_request();
                rim_report_error ("Content length is zero");
            }
            if (is_multipart == 1)
            {
                // for multipart, we get structured delivery, no body as it will be parsed and cut
                req->body = RIM_EMPTY_STRING;
                req->body_len = 0;

                // Based on RVM2045 (MIME types) and RVM1867 (file upload in html form)
                // Boundary is always CRLF (\r\n) and for 'multipart' type, the content-transfer-encoding must
                // always be 7bit/8bit/binary, i.e. no base64
                char *boundary_start = "boundary=";
                char *bnd = strcasestr (cont_type, boundary_start);
                if (bnd == NULL)
                {
                    rim_bad_request();
                    rim_report_error ("Cannot find boundary in content type header [%s]",cont_type);
                }
                if (bnd != cont_type && !isspace(*(bnd - 1)) && *(bnd - 1) != ';') 
                {
                    rim_bad_request();
                    rim_report_error ("Cannot find boundary in content type header [%s]",cont_type);
                }
                char *b = bnd + strlen (boundary_start); // b is now boundary string up new line or ;
                rim_num boundary_end =  strcspn (b, "\n;");
                b[boundary_end] = 0; // b is now boundary string
                rim_num boundary_len = strlen (b);
                b = rim_trim_ptr (b, &boundary_len);
                // look for multi-part elements, one by one
                char *c = content; // start with the beginning of content
                char *past_end = content + cont_len_byte;
                rim_num remainder_len;
                while (1)
                {
                    char *file_name = NULL;
                    char *name = NULL;
                    char *name_val = NULL;
                    char *multi_ctype = NULL;
                    rim_num multi_ctype_len = 0;

                    remainder_len = past_end - c; // calculate how much we advanced in the input data

                    char *el = memmem(c, remainder_len, b, boundary_len);// look for boundary in content
                    //char *el = strstr (c, b); 

                    // boundary is always preceded by -- but we don't look for that
                    // if boundary is superceded by -- it's the last one
                    if (el == NULL)  break;
                    el += boundary_len;
                    if (*(el + 1) == '-' && *(el + 2) == '-') break;
#define RIM_CONT_DISP_STR "Content-Disposition:"
                    char *c1 = RIM_CONT_DISP_STR;
                    char *prev = el;
                    el = strcasestr (el, c1); // pos of 'content-disposition'
                    if (el == NULL) break;
                    if (el != prev && !isspace(*(el - 1)) && *(el - 1) != ';') break;
                    char *end_of_element = strchr (el, '\n');
                    if (end_of_element != NULL) *end_of_element = 0; // mark the end of content-disp. line 
                                                // so we don't go beyond it
                    el += strlen (RIM_CONT_DISP_STR);

                    char *beg_of_line = el; // now we're past content-disposition
                    // Find name
                    char *c2 = "name=";
                    prev = el;
                    el = strcasestr (el, c2); // pos of name=
                    if (el == NULL) break;
                    if (el != prev && !isspace (*(el - 1)) &&  *(el - 1) != ';') break;
                    el += strlen (c2);
                    char *element_end = el + strcspn (el, ";"); // end of name=
                    char char_end = *element_end;
                    *element_end = 0;
                    name = el;
                    // copy string to rimstone mem, and get rid of quotes if there
                    rim_num name_len = element_end - el;
                    name = rim_trim_ptr (name, &name_len);
                    if (name[0] == '"') { name++; name_len --;}
                    if (name_len>0 && name[name_len-1] == '"') name_len--;
                    name = rim_strdupl (name, 0, name_len);
                    name[name_len] = 0;
                    //
                    *element_end = char_end; // restore char

                    // find file name, this one is optional
                    char *c3 = "filename=";
                    el = beg_of_line; // look for file name in the line again
                    prev = el;
                    el = strcasestr (el, c3); // pos of filename=
                    rim_num filename_len;
                    if (el != NULL) 
                    {
                        if (prev != el && !isspace (*(el - 1)) && *(el - 1) != ';') break;
                        el += strlen (c3);
                        // we use strcspn since there may not be ; but rather the string just ends
                        char *element_end = el + strcspn (el, ";"); // end of filename=
                        char char_end = *element_end;
                        *element_end = 0;
                        file_name = el;
                        filename_len = element_end - el;
                        file_name = rim_trim_ptr (file_name, &filename_len);
                        // copy string to rimstone mem, and get rid of quotes if there
                        if (file_name[0] == '"') { file_name++; filename_len --;}
                        if (filename_len>0 && file_name[filename_len-1] == '"') filename_len--;
                        file_name = rim_strdupl (file_name, 0, filename_len);
                        file_name[filename_len] = 0;
                        //
                        *element_end = char_end;
                    }
                    else 
                    {
                        RIM_STRDUP (file_name, "");
                        filename_len = 0;
                    }

                    // after looking up name/filename, go to next line. If empty, the following
                    // line is value. If not it could be Content-Type or something else
                    char *cval = end_of_element + 1;
                    char *end_of_val = strchrnul (cval, '\n');
                    if (*end_of_val == 0) break;
                    *end_of_val = 0;
                    rim_num len_cont_val = end_of_val - cval;
                    cval = rim_trim_ptr (cval, &len_cont_val);
                    rim_num len_of_name_val;
                    if (cval[0] == 0)
                    {
                        // the following line is the actual value of parameter [name], name_val is value
                        name_val = end_of_val + 1;
                        // now find the boundary
                        char *end_name = memmem(name_val, past_end - name_val, b, boundary_len);// look for boundary in content
                        if (end_name == NULL) break;
                        *(end_name - 4) = 0; // account for CRLF prior to boundary and for '--'
                        c = end_name - 3; // continue right after the end, in the middle of CRLF
                        len_of_name_val = (end_name - 4 - name_val);
                        // name_val is the value
                        // len_of_name_val is the length
                        name_val = rim_trim_ptr (name_val, &len_of_name_val);
                        RIM_STRDUP (name_val, name_val);
                    }
                    else
                    {
                        // Now, we should find Content-Type or something else
                        // We ignore all that is here since we will always just
                        // save the file as binary, whatever it is. It is ip to application
                        // to figure out
                        // this is file, and we should have filename
                        // but not fatal if we do not
                        RIM_STRDUP (name_val, "");
                        len_of_name_val = 0;
                        multi_ctype = end_of_val + 1;
                        char *nlBeforeCont = strchr (multi_ctype, '\n');
                        if (nlBeforeCont == NULL) break;
                        multi_ctype = nlBeforeCont + 1;
                        // now find the boundary
                        char *end_file = memmem(multi_ctype, past_end - multi_ctype, b, boundary_len);// look for boundary in content
                        if (end_file == NULL) break;
                        *(end_file - 4) = 0; // account for CRLF prior to boundary and for '--'
                        c = end_file - 3; // continue right after the end, in the middle of CRLF
                        multi_ctype_len = (end_file - 4 - multi_ctype);
                        // multi_ctype is now the file
                        // multi_ctype_len is the binary length
                    }
                    rim_num avail;
                    char *enc;
                    enc = NULL;
                    // name of attachment input parameter
                    rim_encode (RIM_URL, name_val, len_of_name_val, &enc, false);
                    would_write = snprintf (new_url + new_url_ptr, avail = RIM_MAX_SIZE_OF_URL - new_url_ptr - 2, "%s=%s&", name, enc);
                    rim_free_int (enc);
                    if (would_write >= avail)
                    {
                        rim_bad_request();
                        rim_report_error ("Web input larger than the limit of [%d] bytes (2)", RIM_MAX_SIZE_OF_URL);
                    }
                    

                    //
                    new_url_ptr += would_write;


                    // write file
                    // generate unique number for file and directory, create dir if it doesn't exist
                    // as well as file. We can only tell that file name was not uploaded if file name is
                    // empty. All files appear 'uploaded'. If a file was not specified, it still exists
                    // in the content, it's just empty. The only way to tell it was not uploaded is 
                    // by empty file_name.

                    if (multi_ctype != NULL)
                    {
                        //
                        if (req->upload == NULL) rim_create_hash (&(req->upload), RIM_HASH_UPLOAD_SIZE, NULL, 0);  
                        // Create uploaded object for this file so it can be accessed with get-upload
                        // The hash for it in the request is created the first time it's needed (released automatically at the end of request).
                        rim_upload *gup = rim_malloc (sizeof(rim_upload));
                        // add hash element  right away, even if filename empty. We will just change the contents of what's in gup
                        // but the pointer to it will *not* change
                        rim_num st;
                        rim_add_hash (req->upload, name, gup, NULL, &st); // key/value here *are* RimStone memory
                        if (st != RIM_OKAY) 
                        { // this really shouldn't happen since we check for duplicate params in URL
                            rim_bad_request();
                            rim_report_error ("Duplicate file upload [%s]", name);
                        }
                        //
                        if (file_name[0] != 0)
                        {
                            gup->fname = file_name; // already strdup-ed above
                            // get extension of filename
                            rim_num flen = filename_len;
                            rim_num j = flen - 1;
                            char *ext = "";
                            while (j > 0 && file_name[j] != '.') j--;
                            if (file_name[j] == '.')
                            {
                                RIM_STRDUP (ext, file_name + j); // .something extension captured
                                if (!strcasecmp (ext, ".jpeg")) rim_copy_data (&ext, ".jpg");
                                if (!strcasecmp (ext, ".jpg")) rim_copy_data (&ext, ".jpg");
                                if (!strcasecmp (ext, ".pdf")) rim_copy_data (&ext, ".pdf");
                            }
                            //lower case extension
                            flen = strlen (ext);
                            for (j = 0; j < flen; j++) ext[j] = tolower (ext[j]);

                            char *write_dir = RIM_EMPTY_STRING;
                            FILE *f = rim_make_document (&write_dir, 0);

                            // write the actual uploaded file contents to local file
                            if (fwrite_unlocked(multi_ctype, multi_ctype_len, 1, f) != 1)
                            {
                                // this is not a bad request, but server error
                                rim_report_error ("Cannot write file [%s], error [%s]", write_dir, strerror (errno));
                            }
                            fclose (f);

                            gup->loc = write_dir;

                            gup->ext = ext;

                            gup->size = multi_ctype_len;

                        }
                        else
                        {
                            gup->fname = RIM_EMPTY_STRING;
                            gup->size = 0;
                            gup->loc = RIM_EMPTY_STRING;
                            gup->ext = RIM_EMPTY_STRING;
                        }
                    }

                }
                if (new_url_ptr > 0) 
                {
                    new_url_ptr--;
                    new_url[new_url_ptr] = 0; // the extra '&' which is always appended
                }

                content = new_url; // have URL (built) and pass it along as if it were a regular URL GET
                cont_len_byte = new_url_ptr;
            }
            else
            {
                // if body is a query string, then it should be added to whatever query string is in URL
                // if not a query string, then content from body is ignored
                if (!is_query_string) 
                {
                    cont_len_byte = 0; // if there is any query string after this, those params are on their own,
                    // Must have 2 extra bytes b/c for URL two nulls in a row signify the end of name/value pairs
                    // which are normally separated by a null
                    content = new_url;
                    content[0] = 0;
                }
            }
        }
        // Empty content type is allowed if content length is zero. This is say for REST when
        // POST is issued with just a URL (no payload, so no need for content type)
        else if (cont_len_byte != 0)
        {
            rim_bad_request();
            rim_report_error ("Content-length found without Content-Type");
            return 0;
        }
    }
    else 
    {
        // for now request method must be specified and must be either GET POST PUT PATCH or DELETE
        rim_bad_request();
        rim_report_error ("Unsupported request method [%s]", req_method);
        return 0;
    }

    //
    // At this point content is what's from urlencoded body and cont_len_byte is the index of 0 byte at then 
    // regardless if this is multipart or not. This is for any method (get, post, put, delete, patch)
    //


    if (qry != NULL && qry[0] != 0)
    {
        // cont_len_byte is the index where this query string is to be copied, so length alloced is cont_len_byte+1
        if (cont_len_byte + qry_len +3 >= RIM_MAX_SIZE_OF_URL)
        {
            rim_bad_request();
            rim_report_error ("URL too long [%ld]", cont_len_byte+qry_len+3);
        }
        // content is the memory that's already prefilled
        // based on which path we took to get here, so if there's query
        // already in content, which would have come from body, we append to it
        if (cont_len_byte > 0) 
        {
            content[cont_len_byte] = '&'; // add & to the end of existing URL, if any
            cont_len_byte++; // and move one byte forward, to the point where we can copy query string from URL alone (as like GET)
        }
        memcpy (content + cont_len_byte, qry, qry_len+1);
        cont_len_byte = cont_len_byte+qry_len+1; // where any additional text would go to
    }
    // Now the URL is actually built from the POST request that can be parsed as an actual request


    // Convert URL format to a number of zero-delimited chunks
    // in form of name-value-name-value...
    // if name, convert - to _ so URL can use parameters like do-something or customer-id (because C variables can't have -)
    rim_num j;
    rim_num i;
    rim_num had_equal = 0;
    bool isname = true;
    rim_num num_of_input_params = 0;
    for (j = i = 0; content[i]; i++)
    {
        content[i] = (content[i] == '+' ? ' ' : content[i]);
        if (isname && content[i] == '-') content[j++] = '_';
        else if (content[i] == '%')
        {
            content[j++] = RIM_CHAR_FROM_HEX (content[i+1])*16+
                RIM_CHAR_FROM_HEX (content[i+2]);
            i += 2;
        }
        else
        {
            if (content[i] == '&')
            {
                if (had_equal == 0)
                {
                    rim_bad_request();
                    rim_report_error ("Malformed URL request, encountered ampersand without prior name=value");
                }
                content[j++] = 0;
                had_equal = 0;
                isname = true;
            }
            else if (content[i] == '=')
            {
                had_equal = 1;
                num_of_input_params++;
                content[j++] = 0;
                isname = false;
            }
            else
                content[j++] = content[i];
        }
    }
    // two nulls in a row signify the end of parameters, this is why content must have at least 2 bytes
    content[j++] = 0;
    content[j] = 0;


    j = 0;
    rim_num name_length;
    rim_num value_len;
    for (i = 0; i < num_of_input_params; i++)
    {
        char *n, *v;
        name_length = strlen (n = content + j); 
        rim_num par_ind = rim_find_par (n); // n is already converted from - to _, and the param hash holds _ only, so total match
        j += name_length+1;

        value_len = strlen (v = content + j); 
        j += value_len+1;

        // we ignore parameters that are set in request URL but are never retrieved
        if (par_ind != -1)
        {
            rim_num trimmed_len = value_len;
            _rim_sprm_par[par_ind].tval.value = rim_trim_ptr (v, &trimmed_len);// trim the input parameter for whitespaces (both left and right)
            // add as partial, parent is always  non-delete, so no need to set it
            rim_check_web_param_type (n, par_ind);
        } 
    }

    // 
    // Find out the full path used
    //
    // construct the full path, often path_info is empty for FastCGI
    // according to rfc3875/3986/3987, script_name must start with / and cannot end with / , and 
    // path_info is the same. Thus concatinating them is safe.
    // However, in some cases (like http://.../some/path/?x=2), script_name does end with / - this url
    // likely isn't valid, but it's possible, especially from generated sources. So we handle that.
    // according to rfc2396, both hyphen and equal sign are allowed (among other chars) in the path,
    // however, hyphen is common and equal sign may confuse some clients/servers and they may ignore it, 
    // encode it, or even unpredictably change it. Thus hyphen is used, because *all* clients and servers
    // process it correctly.
    // script_name is leading, path_info follows
    // Per rfc3875, PATH_INFO values MUST be URL-decoded by the caller, thus nothing there to be decoded.
    // Also per same, PATH_INFO values canNOT have "/" in it. SCRIPT_NAME must be a valid path, not URL-encoded, so nothing
    // to decode either, per rfc3875.
    // Result of rim_getenv() cannot be NULL, if returned as such, we get "".
    char *script_name = rim_getenv ("SCRIPT_NAME");
    char *path_info = rim_getenv ("PATH_INFO");
    static char full_path[RIM_MAX_PATH]; // this static is fine, it's filled every time, and its purpose is to keep the 
                                           // memory for path_req so it will be valid throughout request.
    int sn_len = strlen (script_name);
    // 
    //
    // if script_name ends with /, remove that / and update length of script_name (sn_len)
    // NOTE that we're actually changing the environment variable here. So if Gliim program obtains environment,
    // it will get script_name WITHOUT trailing /. And that's fine. This trailing / should have never been there.
    //
    while (sn_len > 0 && script_name[sn_len - 1] == '/') { script_name[sn_len - 1] = 0; sn_len--; }
    // make full path, use length of script_name to quickly print, use only half mem maxium
    rim_num full_path_len = snprintf (full_path, sizeof(full_path)/2, "%s%s", script_name, path_info);
    // remove any trailing /
    while (full_path_len > 0 && full_path[full_path_len - 1] == '/') { full_path[full_path_len - 1] = 0; full_path_len--; }

    //
    // URL path must start with application path which at minimum is application name
    //

    // Check if URL app path actually matches the leading portion of the URL path

    if (!strncasecmp (rim_app_path, full_path, rim_app_path_len))
    {
        char *p = full_path+rim_app_path_len;
        rim_num p_len = full_path_len - rim_app_path_len;
        // find all path segments
        //
        // Anything after app-path is the request path until = path
        // p is now /all/after/app-path
        // *p must be /
        // we check, app path NEVER ends with forward slash
        // *p must be / or there's no /request
        // Request path cannot be just / or start with /?, in this case there's no request path
        // We already handled ?query_string so having just "/?" followed by nothing - can that happen, seem so. But we don't have to
        // worry about ? in any other sense.
        if (*p != '/' || (*p == '/' && (*(p+1) == 0 || *(p+1) == '?'))) 
        {
            // there is no request name
            rim_bad_request();
            rim_report_error ("URL path [%s] is missing a request path", full_path);
        }


        static char reqname[RIM_MAX_REQ_NAME_LEN];
        char decres = rim_decorate_path (reqname, sizeof(reqname), &p, p_len);
        if (decres == 1) {
            req->name = reqname;
        }
        else 
        {
            // this means (when decres is 0), that in /req/x=2/..  request name (req) is missing
            // in the future rim_decorate_path may return more than just 0 or 1!
            rim_bad_request();
            rim_report_error ("URL path [%s] is incorrect (request name is missing)", full_path);
        }

        // p is NULL here if no params with = in request path + params
        while (p != NULL) 
        {
            rim_num ipar_len = 0;
            char *ipar;
            ipar = p + 1; // one after / 
                                                                                            //
            char *next_par = strchrnul (ipar, '/'); // pointer to either / or null at the end
            ipar_len = (rim_num)(next_par - ipar);
            if (ipar_len == 0) break; // this is just trailing /
            if (*next_par != 0) 
            {
                *next_par = 0; // found /
                p = next_par;
            } 
            else 
            {
                p = NULL; // didn't find /, so this is the last parameter 
            }
            char *name;
            char *value;
            name = ipar;
            // subst _ for - in name
            rim_num il;
            value = NULL; // means not found
            rim_num val_len;
            for (il = 0; il < ipar_len; il++) 
            {
                if (name[il] == '-') name[il] = '_';
                // Per  RFC 3986, it's allowed to use name=value in path segments
                if (value == NULL && name[il] == '=') // find only first =
                {
                    // first = in name means the value is following
                    value = name + il + 1; // we got value from /name=value
                    val_len = ipar_len - (il + 1); // length of value
                    name[il] = 0; // finish off the name
                    break; // we got the name and the value
                }
            }
            if (value == NULL)
            {
                rim_bad_request();
                rim_report_error ("Value missing (with =) for name [%s] in URL path [%s]", name, full_path);
            }
            // NO decoding value for any value in PATH_INFO and SCRIPT_NAME see above per rfc3875
            // if this is a command line program, then a value must be percent-decoded, since with web apps, the web server will do that,
            // but with command-line, we have to do it
#ifdef RIM_COMMAND 
            // Decode value no matter how we found it
            val_len = rim_decode (RIM_URL, value, val_len, false, NULL);
#else
            RIM_UNUSED(val_len);
#endif
            // this is /param/val
            rim_num par_ind = rim_find_par (name); // compares by accounting for all hyphen or underscore, but not mix?
            if (par_ind != -1)
            {
                // this name/value must be added
                rim_num trimmed_len = val_len;
                _rim_sprm_par[par_ind].tval.value = rim_trim_ptr (value, &trimmed_len);// trim the input parameter for whitespaces (both left and right)
                rim_check_web_param_type (name, par_ind);
            }
        }
    }
    else
    {
        rim_bad_request();
        rim_report_error ("URL path [%s] must begin with application path [%s]", full_path, rim_app_path);
    }


    return 1;
}



// 
// In URL list of inputs, set value for input name to val.
// req is the request structure.
// Returns >=0 index where it is in ip.ipars[].tval.value array 
// Value is set, and no copy of it is made. Use copy-string to make a copy if needed.
// type is the type of val
//
RIM_ALWAYS_INLINE inline rim_num rim_set_input (rim_num name_id, void *val, rim_num type)
{

    // set-input can be assigned any type (tree, boolean, string etc)
    // If it is string, it is always assigned allocated memory, so we must increase its refcount (unless it's already there)
    // in that way, params are like a tree , array or a list
    // check if they are equal first of course, then assign (or they'd be equal always)
    //
    if (cmp_type (type, RIM_DEFSTRING)) rim_mem_add_ref (val, 0);
    //
    _rim_sprm_par[name_id].tval.value = val;
    // The end result here is if val and _rim_sprm_par[name_id].tval.value are equal, then no refcount change as it should be
    //
    _rim_sprm_par[name_id].type = type;
    _rim_sprm_par[name_id].alloc = true;
    _rim_sprm_par[name_id].set = true;
    return name_id;
}


// 
// In URL list of inputs, find an index for an input with a given name-id, which is predetermined by source code 
// (static compilation for performance)
// Returns value of parameters, or "" if not found.
// input parameter is supposed to be of type 'type' unless type is RIM_DEFUNKN, in which case there's no
// error message if 'type' isn't the actual type of input param.
// RIM_DEFUNKN is used to just obtain the index of a parameter - this is the ONLY time RIM_DEFUNKN should be used - 
// we MUST always know the type of what we're requesting!
//
RIM_ALWAYS_INLINE inline void *rim_get_input_param (rim_num name_id, rim_num type, char *defval)
{

    // We check at compile time that set-param and get-param types for each parameters match. For those parameters which are set from outside,
    // we check in rim_get_input() that their type is string, or if they are never used; in either case we error out if not a correct match.
    // For all other cases, set-param and get-param must match due to compile time check, so at run-time we can only check if parameter set or not.

    if (!_rim_sprm_par[name_id].set)
    {
        if (type != RIM_DEFSTRING) rim_report_error ("Trying to get the value of parameter [%s], but it was not set", _rim_sprm_par[name_id].name);
        else
        {
            if (defval != NULL) return defval;
            else rim_report_error ("Trying to get the value of parameter [%s], but it was not set", _rim_sprm_par[name_id].name);
        }
    }
    //
    // So at this point, a param does have value set within a request (either from outside or from inside in code)
    //
    char *v = (char*)(_rim_sprm_par[name_id].tval.value);
    if (type == RIM_DEFSTRING)
    {
        if (!_rim_sprm_par[name_id].alloc ) 
        {
            v = _rim_sprm_par[name_id].tval.value = rim_strdup (v); 
            _rim_sprm_par[name_id].alloc = true; // so it's strdup'd only once even if requested many times
        } 
        // note that deletion of reference of target is done in v1.c, as well as adding reference to variable assigned to it.
        return v;
    } else 
    {
        // number is a special case, we have a value for it.
        // For parameters other than number, bool or string (say tree), if it's NULL then it's not set in this request; we set all parameters to 
        // init values at the beginning of the request; all objects other than basic types (like tree) have a life span of the request, so it can
        // never happen that say a tree param could have invalid value.
        if (v == NULL) rim_report_error("Parameter [%s] is not set", _rim_sprm_par[name_id].name); else return v; // otherwise it's just a pointer
    }
}


// 
// Copy 'value' to 'data' at offset 'off'. 
// Returns number of bytes written excluding zero at the end.
// 'data' will be a pointer to allocated data that has *data+(value at offset off of data)
// This is a base function used in other string manipulation routines.
//
rim_num rim_copy_data_at_offset (char **data, rim_num off, char *value)
{

    if (*data == NULL) 
    {
        RIM_STRDUP (*data, value);
        return 0;
    }
    else
    {
        if (*data == value) 
        {
            return 0; // copying to itself, with SIGSEGV
        }
        if (value == NULL) value = "";
        rim_num len_val = strlen (value);
        //
        // rim_realloc will handle if *data points to RIM_EMPTY_STRING, i.e. if it's uninitialized
        //
        *data = rim_realloc (rim_mem_get_id(*data), off+len_val + 1);
        memcpy (*data+off, value, len_val+1);
        return len_val; // returns bytes written, not the new length
    }
}


// 
// Copy string from 'value' to 'data', with 'data' being the output pointer.
// Returns the number of bytes written excluding zero at the end.
//
rim_num rim_copy_data (char **data, char *value)
{
    return rim_copy_data_at_offset(data, 0, value);
}


// 
// Check if string 's' is a number. Return 1 if it is, 0 if not.
// Number can have plus/minus in front and can have one dot somewhere
// in the middle. Outputs: 'prec' is precision: total number of digits, 'scale' is the number of 
// digits after the decimal point. If prec and scale aren't NULL, they are filled.
// Same for 'positive', if number is positive it is 1, otherwise 0.
//
rim_num rim_is_number (char *s, rim_num *prec, rim_num *scale, rim_num *positive)
{
    rim_num i = 0;
    if (prec != NULL ) *prec = 0;
    if (scale != NULL) *scale = 0;
    rim_num dot_pos = 0;
    rim_num sign = 0;
    if (positive!=NULL) *positive=1;
    while (s[i] != 0) 
    {
        if (isspace(s[i])) 
        {
            i++;
            continue;
        }
        if (!isdigit(s[i]))
        {
           if (s[i]=='+' || s[i]=='-')
           {
               if (i != 0)
               {
                    // + or - isn't the first
                    return 0;
               }
               else
               {
                   sign = 1;
                   if (s[i]=='-' && positive!=NULL) *positive = 0;
               }
           }
           else if (s[i]=='.' && i>0)
           {
               if (dot_pos > 0)
               {
                   // two dots
                   return 0;
               }
              dot_pos = i; 
           }
           else
           {
               return 0;
           }
        }
        i++;
    }
    if (dot_pos > 0)
    {
        rim_num c_scale= i - dot_pos - 1;
        if (c_scale == 0)
        {
            // this is for example 1234. 
            // i.e. no digits after dot
            return 0;
        }
        if (scale != NULL) *scale = c_scale;
    }
    else
    {
        if (scale != NULL) *scale = 0;
    }
    if (dot_pos > 0) i--;
    if (sign > 0) i--;
    // for example in -123.4, c_prec would be be 4 because 6th byte would be zero and we decrease i
    // after the loop: If there is a single dot, we decrease it by 1. If there is a single +
    // or -, we decrease it by 1. The result is the precision.
    if (prec != NULL) *prec = i;
    if (i == 0) return 0; // no digits, not a number
    return 1;
}

// 
// Returns 1 if string 's' is a positive unsigned integer.
//
rim_num rim_is_positive_num (char *s)
{
    rim_num i = 0;
    while (s[i] != 0) 
    {
        if (!isdigit(s[i]))
        {
           return 0;
        }
        i++;
    }
    return 1;
}

//
// Make sure argv[0] is correctly set to program name before executing - sometimes (not always) 
// but sometimes it is necessary for the executing program not to crash. 'program' is the full
// path of the executable. So if 'program' is '/a/b/c/d', then arg0 is 'd'
//
static void rim_set_arg0 (char *program, char **arg0)
{
    rim_num i =strlen(program) - 1;
    while (i >= 0)
    {
        if (program[i]=='/')
        {
            break;
        }
        i--;
    }
    *arg0 = program+i+1;
}


//
// Read data from file descriptor ofd, into out_buf (which is allocated inside),
// and if out_buf!=NULL, put length there. The buffer starts very small and grows to 4K as needed,
// with block read of max 4K. Useful for reading a stream.
//
static void rim_read_child (int ofd, char **out_buf)
{
    lseek (ofd, SEEK_SET, 0);
    // minimum allocation for dmalloc
#ifdef DEBUG
#define EXEC_BLEN 32
#else
#define EXEC_BLEN 2048
#endif
    rim_num tread = EXEC_BLEN;
    *out_buf = (char*) rim_malloc (tread); 
    rim_num curr = 0;
    while (1) 
    {
        rim_num rd = read (ofd, *out_buf + curr, tread - 1); // -1 so we can place null char at the end
        if (rd == 0) break;
        if (rd == -1) rim_report_error ("Cannot read from program execution, error [%d], error text [%s]", errno, strerror(errno));
        curr += rd;
        if (rd < tread - 1) tread = (rd < EXEC_BLEN ? EXEC_BLEN : rd+1); // if reading close to end, or stalling, just add as much as given, or EXEC_BLEN
                                                                         // whichever is greater
        else {
            if (tread < 4096) tread *= 2; // go up to 4K blocks, but if too big (8x), add quarter each time
            else if (curr > 8*tread) tread = curr/4; 
        }
        *out_buf = (char*) rim_realloc (rim_mem_get_id(*out_buf), curr + tread); 
    }
    (*out_buf)[curr] = 0;
    *out_buf = (char*) rim_realloc (rim_mem_get_id(*out_buf), curr+1);  // set to exact memory needed, sets length
    // *out_buf is new, so cannot use id for realloc above
    rim_mem_set_len (*out_buf, curr+1);
}



// 
// Execute program 
// It can be input data or input file, and with output data or output file. So input data can be mixed with output file etc.
// With command-line arguments and with capture of its exist status, 
// prg is the full path of program or just program name if in PATH (note PATH does not include current directory in latest Linux)
// If *fout is NULL and out_buf==NULL, a temporary file will be created
// Returns exit status of program execution.
// 
// 1. takes input arguments 'argv' with total number of them 'num_args'.
//    (argv[num_args] must be NULL, so if arguments are 'x' and 'y', then argv[0] is 'x', argv[1] is 'y' and 
//     argv[2] is NULL and num_args is 2)
// 2. input string 'inp' of length 'inp_len' is passed as stdin to the program. If inp is NULL or inp[0]==0,
//      then, do NOT write to stdin of a program being executed, rather use fin as input file, and if that is NULL too, then there is
//      no input to the program. If inp_len is 0, then strlen(inp) is used for length, assuming inp is not 
//      empty (NULL or inp[0]==0).
// 3. output of the program (both stdout and stderr) is saved in 'out_buf' if out_buf!=NULL. out_buf 
//      can be NULL, in which case no output is passed back out to string. 
//      If out_buf is NULL and *fout!=NULL, then output goes to this file.
// 4. ferr, err_buf are for stderr, and follow the same rules as fout, out_buf. If all ferr and *err_buff are NULL,
//      then both stdout/stderr go to fout/out_buf.
// Return value is the exit status.
//
rim_num rim_exec_program (char *prg, char *argv[], rim_num num_args, FILE *fin, FILE **fout, FILE **ferr, char *inp, rim_num inp_len, char **out_buf, char **err_buf, char **env)
{

    if (argv[num_args] != NULL)
    {
        rim_report_error ("Number of arguments does not match last NULL");
    }
    // make sure argv[0] is program name
    rim_set_arg0 (prg, &(argv[0]));

    //make sure we either use a file or a string (be it for input, output or error)
    if (fin != NULL && inp != NULL) rim_report_error ("Cannot use both input-file and input in exec-program");
    if (*fout != NULL && out_buf != NULL) rim_report_error ("Cannot use both output-file and output in exec-program");
    if (*ferr != NULL && err_buf != NULL) rim_report_error ("Cannot use both error-file and error in exec-program");

    pid_t pid;

    // redirect output to nothing if no file provided provided, and if out_buf==NULL
    if (*fout == NULL && out_buf == NULL) 
    {
        *fout = fopen ("/dev/null", "w");
        if (*fout==NULL) rim_report_error ("Cannot redirect output to /dev/null, error [%s]", strerror(errno));
    }
    // redirect error to nothing if no file provided provided, and if out_buf==NULL
    if (*ferr == NULL && err_buf == NULL) 
    {
        *ferr = fopen ("/dev/null", "w");
        if (*ferr==NULL) rim_report_error ("Cannot redirect output to /dev/null, error [%s]", strerror(errno));
    }


    int pipe2child[2] = {-1,-1}; // input from parent 2 child
    int pipe2parent[2] = {-1,-1}; // output from child 2 parent
    int errpipe[2] = {-1, -1}; // from child 2 parent (stderr only)

    if (inp != NULL)
    {
        // pipe from owner to child
        if (pipe(pipe2child) == -1) rim_report_error ("Cannot create pipes, error [%s]", strerror(errno));
    }
    if (out_buf != NULL)
    {
        // pipe from child to parent
        if (pipe(pipe2parent) == -1) rim_report_error ("Cannot create pipes, error [%s]", strerror(errno));
    }
    if (err_buf != NULL)
    {
        // stderr pipe from child to parent
        if (pipe(errpipe) == -1) rim_report_error ("Cannot create pipes, error [%s]", strerror(errno));
    }

    // The order in which pipes/files are closed is important. For example, not closing reading end of pipe in process
    // that only writes, makes for a possiblity that accidentaly process may read its own writing. What is not used, should
    // be closed right away.
    // Also, "closing a descriptor", such as pipe end, does NOT mean the file associated with it is closed. If it was dup2-ed
    // before, the file is still open. It is open for as long as there is a descriptor that still points to it. So "closing"
    // really just means dereferencing, while true "closing" happens when all descriptors that reference the stream/file are closed.
    
    // note dup2() cannot get EINTR (interrrupt) on LINUX, so no checking
    pid = fork();
    if (pid == -1) 
    { 
        rim_report_error ("Cannot create child, error [%s]", strerror(errno));
    }
    else if (pid == 0) 
    {
        // child. Note we check and can't have *fout!=NULL && out_buf!=NULL, and we can't have
        // fin!=NULL && inp!=NULL and ferr!=NULL && err_buf!=NULL. So below, for example, either output
        // goes to a file, or to a string but NOT both.
        // make stdout, stderr and stdin be substituted for files specified
        if (*fout != NULL) {dup2(fileno(*fout),STDOUT_FILENO); fclose(*fout);}  
        if (*ferr != NULL) {dup2(fileno(*ferr),STDERR_FILENO); fclose(*ferr);} 
        if (fin != NULL) {dup2(fileno(fin), STDIN_FILENO); fclose(fin);}

        // close read-end of stdout child2parent, so this pipe isn't accidentally reading program's own output
        if (pipe2parent[0] != -1) close(pipe2parent[0]);    
        if (out_buf != NULL) dup2(pipe2parent[1],STDOUT_FILENO); 
        // close read-end of stderr child2parent, so this pipe isn't accidentally reading program's own error stream
        if (errpipe[0] != -1) close(errpipe[0]);    
        if (err_buf != NULL) dup2(errpipe[1],STDERR_FILENO); 
        // close write-end of stdin parent2child, so this pipe isn't accidentally writing to program's own input
        if (pipe2child[1] != -1) close(pipe2child[1]);
        if (inp != NULL) dup2(pipe2child[0],STDIN_FILENO);  

        // once the pipes are pointed to by stdout, stderr and stdin, close original descriptors; those pipes are still open.
        if (errpipe[1] != -1) close(errpipe[1]);
        if (pipe2child[0] != -1) close(pipe2child[0]);    
        if (pipe2parent[1] != -1) close(pipe2parent[1]);

        // set environment (additional, since **environ of the parent is inherited) variables
        if (env != NULL)
        {
            // just advance env[] until no more, stop when NULL either for name or value in name=value
            while (1)
            {
                if (env[0] != NULL && env[1] != NULL) setenv (env[0], env[1], 1); else break;
                env = &(env[2]);
            }
        }

        int res = execvp(prg, (char *const*)argv); // will send data to parent
        if (res) // we're here only if execvp failed, otherwise execvp doesn't return
        {
            RIM_FATAL ("Failed to start program [%s], error [%m]", prg); // failed to exec, do not flush stdout twice (if exit()
                // were called, it would call atexit() of parent and flush its stdout
                // and html output to apache would be duplicated!
            exit(-1); // exit child that failed to execvp
        }
    }
    else 
    { 
        // parent
        // close pipes not used, so parent doesn't accidentaly read/write from/to itself (see above comments for child)
        if (pipe2child[0] != -1) close(pipe2child[0]);    
        if (pipe2parent[1] != -1) close(pipe2parent[1]);
        if (errpipe[1] != -1) close(errpipe[1]);

        // these are used by child only; close them. They just redirect child to files. See comment above for child, where
        // it is discussed how we can either have files OR strings for these, but NOT both.
        if (fin != NULL) fclose (fin);
        if (*fout != NULL) fclose (*fout);
        if (*ferr != NULL) fclose (*ferr);

        // if there is no input, do NOT send anything to stdin of executing program
        if (inp!=NULL && inp[0]!=0)
        {
            // Send string (or binary) to child input, this is the write end of that pipe, the child reads from pipe2child[0]
            rim_num linp = rim_mem_get_len(inp);
            if (inp_len == 0) inp_len = linp;
            else if (inp_len > linp) rim_report_error ("Memory read requested of length [%ld] but only [%ld] allocated", inp_len, linp);

            if (write(pipe2child[1], inp, inp_len) != inp_len) 
            {
                rim_report_error ("Cannot provide input data [%s] of length [%ld] to program [%s], error [%s]", inp, inp_len, prg, strerror (errno));
            }
        }
        if (pipe2child[1] != -1) close(pipe2child[1]); // close the pipe used to write to child's input

        // these must be BEFORE child exits. If this were after wait() below, parent would hang because child's (writing) end of 
        // pipe is now undefined, so can't get anything
        if (out_buf != NULL) rim_read_child (pipe2parent[0], out_buf); // read child's stdout
        if (err_buf != NULL) rim_read_child (errpipe[0], err_buf); // read child's stdout

        int st;
        while (wait (&st) != pid) ; // wait until child finishes

        // close pipes used above so we don't accumulate open descriptors. All pipes/files we opened MUST be totally closed.
        if (pipe2parent[0] != -1) close(pipe2parent[0]);    
        if (errpipe[0] != -1) close(errpipe[0]);    

        if (WIFEXITED(st)) 
        {
            return (rim_num)WEXITSTATUS(st);// exit status if child exited with exit() or such
        }
        else if (WIFSIGNALED(st)) 
        {
            return (rim_num)WTERMSIG(st)+128; // exit status 128+signal if child killed with signal
        }
        else
        {
            return (rim_num)126; // any other kind of child termination
        }
    }
    return 1; // will never actually reach here, only for compiler joy

}


// 
// This is to disable any output, but not for strings. This is typically used with finish-request.
// This is also done to output binary files and prevent other output going out.
//
void rim_disable_output()
{
    rim_get_config()->ctx.req->disable_output = 1;
}



// 
// Output string 's'. 'enc_type' is either RIM_WEB, RIM_URL or 
// RIM_NOENC. len (if not 0) is the length to output. Returns number of bytes written.
// "Output" means either to the browser (web output) or to the string.
// For example, if within write-string statement, it's to the string, 
// otherwise to the web (unless HTML output is disabled).
//
// There are only 2 output functions: rim_puts and rim_printf, and for string output they both
// call rim_puts_to_string(). NO OTHER way of output should be present and NOTHING
// else should call rim_puts_to_string.
// 'alloc' is true if this is rimstone-alloced mem, otherwise false.
//
//
rim_num rim_puts (rim_num enc_type, char *s, rim_num len, bool alloc)
{

    if (rim_validate_output()!=1) rim_report_error ("Cannot send file because output is disabled, or file already output");

    rim_config *pc = rim_get_config();

    rim_num buf_pos_start = RIM_WRSTR_POS;
    RIM_UNUSED(buf_pos_start); // used for tracing only
    rim_num vLen;
    if (alloc)
    { // this is rimstone alloc'd
        rim_num ls = rim_mem_get_len(s);
        if (len != 0)
        {
            vLen = len; 
            if (vLen > ls) rim_report_error ("String output requested of length [%ld] but only [%ld] allocated", vLen, ls);
        } else vLen = ls;
    } 
    else 
    { // this is non-rimstone alloc'd, i.e. internal
        if (len != 0) vLen = len; else vLen = strlen(s);
    }
    rim_num res = 0;
    if (enc_type==RIM_NOENC)
    { // no encoding
        if (RIM_WRSTR_CUR == -1) // this is to the web
        {
            rim_num res = rim_write_web (false, pc, s, vLen);
            return res;
        } 
        else 
        { // this is to a string
            return rim_puts_to_string (s, vLen); 
        }
    }
    //
    // if we're encoding, and not buffering, write directly to web socket
    //
    if (RIM_WRSTR_CUR == -1)
    {
        char *write_to = (char*)rim_malloc (RIM_MAX_ENC_BLOWUP(vLen));
        rim_num total_written = rim_encode_base (enc_type, s, vLen, &(write_to), 0);
        res = rim_write_web (false, pc, write_to, total_written);
        rim_free_int (write_to);
        return res;
    }
    //
    // writing to string
    //
    while (1)
    {
        // resize buffer to needed size and encode directly into the buffer
        // without having to memcpy needlessly
        int bup;
        if ((bup = RIM_MAX_ENC_BLOWUP(vLen)) > RIM_WRSTR_LEN -1-RIM_WRSTR_POS)
        {
            RIM_WRSTR_LEN = RIM_WRSTR_LEN + bup + RIM_WRSTR_ADD;
            RIM_WRSTR_ADJMEM(RIM_WRSTR_ADD);
            RIM_WRSTR_BUF= (char*)rim_realloc (rim_mem_get_id(RIM_WRSTR_BUF), RIM_WRSTR_LEN);
            continue;
        }
        else
        {
            char *write_to = RIM_WRSTR_BUF+RIM_WRSTR_POS;
            res = rim_encode_base (enc_type, s, vLen, &(write_to), 0);
            RIM_WRSTR_POS += res;
        }
        break;
    }
    return res;
}


// 
// Check if output can happen, if it can, make sure output buffer is present
// and if it needs flushing, flush it.
//
static rim_num rim_validate_output ()
{

    // if output is disabled, do NOT waste time printing to the bufer!!
    // UNLESS this is a write to a string, in which case write it!!
    // If we allow writing, then say output is disabled and we write and then flush happens at any time
    // and the program CRASHES because header wasn't sent!!!!
    rim_config *pc = rim_get_config();
    if (pc->ctx.req->disable_output == 1 && RIM_WRSTR_CUR == -1) return  0;

    return 1;
}

// 
// ** IMPORTANT: THis function is one of the TWO outputters (this and rim_puts) meaning 
// these are SOLE callers of rim_puts_to_string. This is to ensure there is no circumvention of 
// disabled output or anything else.
// Returns total number of bytes written.
// iserr is true if output goes to stderr, otherwise stdout - this is for web output only.
//
// Outputs to web or to strings. enc_type is RIM_NOENC, RIM_WEB, RIM_URL. 
//
rim_num rim_printf (bool iserr, rim_num enc_type, char *format, ...)
{
    
    if (rim_validate_output()!=1) rim_report_error ("Cannot send file because output is disabled, or file already output");
    rim_config *pc = rim_get_config();
    rim_num tot_written = 0;

    va_list args;
    va_start (args, format);

    if (RIM_WRSTR_CUR == -1)
    {
#ifdef DEBUG
        char ibuf[2];
#else
        char ibuf[4096];
#endif
        // Try first with a stack buffer, faster than malloc; most of the time it will succeed, saving time
        tot_written = vsnprintf (ibuf, sizeof(ibuf), format, args);
        char *ebuf;
        if (tot_written >= (rim_num)sizeof(ibuf))
        {
            va_end (args); // must restart the va_list before retrying!
            va_start (args, format);
            ebuf = (char*)rim_malloc0 (tot_written+2); // +1 will suffice
            tot_written = vsnprintf (ebuf, tot_written+2, format, args);
        } else ebuf = ibuf;
        va_end (args);
        if (enc_type == RIM_WEB || enc_type == RIM_URL)
        {
            char *final_out = NULL;
            // here final_out is allocated in rim_encode, and so is free 3 lines down
            // we can use MALLOC/REALLOC here because of 'false' last arg below - it says we're providing ebuf and tot_written
            // as unmanaged memory
            rim_num final_len = rim_encode (enc_type, ebuf, tot_written, &final_out, false); // must state total_written, since we haven't set exact length for ebuf
            tot_written = rim_write_web (iserr, pc, final_out, final_len);
            rim_free_int (final_out);
        }
        else
        {
            tot_written = rim_write_web (iserr, pc, ebuf, tot_written);
        }
        if (ebuf != ibuf) free (ebuf); // so there is no leak when unmanaged memory
        return tot_written;
    }


    // This is writing to string
    // Since this is always RIM_NOENC, we can print this (snprintf) directly , avoiding extra copying of buffer
    while (1)
    {
        // tot_written (i.e. the return value) is what would have been written if there was enough space EXCLUDING the null byte.
        // bytes_left (i.e. second parameter) is the number of bytes available to write INCLUDING the null byte.
        // So if bytes_left is 255, and the return value is 255, it means the space needed was 255+1 (because the return value 
        // excludes the null byte!), so if return value is grear OR EQUAL to the size available (ie. second parameter), then we need to realloc.
        rim_num bytes_left = RIM_WRSTR_LEN - RIM_WRSTR_POS;
        tot_written = vsnprintf (RIM_WRSTR_BUF+ RIM_WRSTR_POS, bytes_left, format, args);
        if (tot_written >= bytes_left)
        {
            RIM_WRSTR_LEN += tot_written + RIM_WRSTR_ADD;
            RIM_WRSTR_ADJMEM(RIM_WRSTR_ADD);
            RIM_WRSTR_BUF= rim_realloc (rim_mem_get_id(RIM_WRSTR_BUF), RIM_WRSTR_LEN);
            va_end (args); // must restart the va_list before retrying!
            va_start (args, format);
            continue;
        }
        else 
        {
            // buf_pos does NOT include trailing zero, even though we put it there. 
            RIM_WRSTR_POS += tot_written;
            break;
        }
    }
    va_end (args);
    //
    // TODO: combine printf with encoding, this eliminates one string copy!!! So that printf+encoding goes directly to RIM_WRSTR_BUF+RIM_WRSTR_POS, avoding copies
    //
    switch (enc_type)
    {
        case RIM_URL:
        case RIM_WEB:; // has to have ; because declaration (char *final... CANNOT be
                      // after label (which is case ...:)
            char *final_out = NULL;
            RIM_WRSTR_POS-=tot_written;
            rim_num final_len = rim_encode (enc_type, RIM_WRSTR_BUF+RIM_WRSTR_POS, tot_written, &final_out, false); // must set length to total_written, since we haven't set exact length for RIM_WRSTR_BUF+RIM_WRSTR_POS, and we can't either, since it's a part of continuous write-string built buffer, which is being built right here
            tot_written = rim_puts_to_string (final_out, final_len);
            rim_free_int (final_out);
            break;
        case RIM_NOENC:
            // nothing to do, what's printed to output buffer is there to stay unchanged
            break;
        default: rim_report_error ("Unknown encoding type [%ld]", enc_type);
    }
    return tot_written;
}

// ** IMPORTANT:
// ** This function can be called only from rim_printf or rim_puts!! The reason is this way we control
// ** exactly what goes out and there's nothing to circumvent that.
//
// Outputs to string ONLY - for outputting to the web, see rim_write_web().
// The memory can grow as needed to accomodate unlimited writes (but limited by virtual memory).
//
// 'final_out' is the string being output, and final_len is its length. Returns
// number of bytes written excluding zero at the end.
//
// NOTE: this might be optimized to account for immutable strings, so they are NOT copied
// every time a buffer is built. By using an array of pointers to immutable strings coupled with
// an array of where in the final buffer those go to, data can be sent without copying (this
// wouldn't work for string construction).
//
rim_num rim_puts_to_string (char *final_out, rim_num final_len)
{


    // This is writing to string.
    // here we add to the buffer, which may be periodically flushed, for example
    // if program writes a huge report and then sends it somewhere, but doesn't display
    // to web, this output can be huge and more than available memory.
    rim_num buf_pos_start = RIM_WRSTR_POS;
    RIM_UNUSED(buf_pos_start); // used for tracing only
    rim_num res = 0;
    while (1)
    {
        // if we need to write more than currently allocated memory, add more
        // final_len is the length of string, we check we have final_len+1 bytes left (at least)
        if (final_len > RIM_WRSTR_LEN -1-RIM_WRSTR_POS)
        {
            RIM_WRSTR_LEN = RIM_WRSTR_LEN + final_len + RIM_WRSTR_ADD;
            RIM_WRSTR_ADJMEM(RIM_WRSTR_ADD);
            RIM_WRSTR_BUF= (char*)rim_realloc (rim_mem_get_id(RIM_WRSTR_BUF), RIM_WRSTR_LEN);
            continue;
        }
        else
        {
            memcpy (RIM_WRSTR_BUF+ RIM_WRSTR_POS, final_out, final_len + 1);
            RIM_WRSTR_POS += final_len;
            res = final_len;
        }
        break;
    }
    if (res == 0) return 0; // return number of bytes written, minus null at the end
    return res;
}


// 
// Shut down the request, 'giu' is the request. 
// This will flush any outstanding header to the web. If giu or program context is NULL, it produces a fatal error.
// If command line program, set the flag to end it after this request
//
void rim_shut(rim_input_req *giu)
{

    // rim_shut can be called from rim_report_error AND at the end of rim_main
    // it should not be called twice
    if (giu != NULL && giu->is_shut ==1) return;

    if (giu != NULL) giu->is_shut = 1;

    // if program ended before header could have been finished, finish the header
    if (giu != NULL && giu->sent_header ==1 && giu->data_was_output == 0) rim_gen_header_end (); // send cookies and \r\n divider

    if (giu == NULL)
    {
        RIM_FATAL ("Shutting down, but request handler is NULL");
    }
    rim_config *pc = rim_get_config();
    if (pc == NULL)
    {
        RIM_FATAL ("Shutting down, but program context is NULL");
    }
#ifdef RIM_COMMAND
    rim_end_program = 1; // end program for command line
#endif

}


//
// Initialize reply/request html header. Default is cached forever (practically).
// 'header' is the html header structure. init_type: RIM_HEADER_FILE means caching (files),
// RIM_HEADER_PAGE means dynamic page.
// is_request is 1 if this is request header, 0 if reply
//
void rim_init_header (rim_header *header, rim_num init_type, char is_request)
{
    char const *errinit = "Unknown initialization type argument";
    if (init_type == RIM_HEADER_FILE)
    {
        header->etag=1; // send etag by default, so even if we cache forever, but browser decides it can't, it can still benefit from etag
    }
    else if (init_type == RIM_HEADER_PAGE)
    {
        header->etag=0; // no etag for generated page
    }
    else
    {
        rim_report_error ("%s",errinit);
    }

    if (is_request == 0) header->ctype="text/html;charset=utf-8"; // must always be set for reply
    else header->ctype=NULL; // if request header, don't set, will only be set if content-type used in call-web

    header->disp=NULL; // default is show object, not download
    header->file_name=NULL; // this is only if disp is not NULL
    // No status if set to 0
    header->status_id=0;
    header->status_text=NULL;
    // Default header for non-dynamic content such as images, or in general documents.
    // We deliver documents based on a database ID number which never changes if the
    // document doesn't. Any change and the ID changes too. 
    if (init_type == RIM_HEADER_FILE)
    {
        header->cache_control= "public, max-age=2000000000, post-check=2000000000, pre-check=2000000000";  // default is cache forever (actually 53 years) - we are staying within an signed int, so to work anywhere
    }
    else if (init_type == RIM_HEADER_PAGE)
    {
        header->cache_control= "max-age=0, no-cache";
    }
    else
    {
        rim_report_error ("%s",errinit);
    }
    rim_num i;
    // any number of headers. The first from index 0 that has control or value NULL is where we stop looking. So no gaps.
    for (i = 0; i<RIM_MAX_HTTP_HEADER; i++)
    {
        header->control[i]=NULL;
        header->value[i]=NULL;
    }
}



// 
// Create a new unique document and return FILE pointer to a newly created file associated
// with a document. 
// write_dir is the full path file name for which FILE * was opened. is_temp is 1 for temp table, 0 for documents.
// Returns FILE* to the file opened.
// The goal is to create a file and have a unique ID for it that can be used for future tracking and use
// of the file.
//
FILE *rim_make_document (char **write_dir, rim_num is_temp)
{

    rim_config *pc = rim_get_config();

    char path[180];
    rim_num file_size = 200;
    char *ufile = (char*)rim_malloc (file_size);
    char *rnd;
    rim_make_random (&rnd, 6, RIM_RANDOM_NUM, false); // random of size 5 as maximum is RIM_MAX_UPLOAD_DIR (40000), max 5 digits

    if (is_temp == 0)
    {
        snprintf (path, sizeof(path), "%s/%ld", pc->app.file_dir, atol(rnd)%RIM_MAX_UPLOAD_DIR);
    }
    else
    {
        snprintf (path, sizeof(path), "%s/t/%ld", pc->app.file_dir, atol(rnd)%RIM_MAX_UPLOAD_DIR);
    }
    rim_num wb = snprintf (ufile, file_size, "%s/%ldXXXXXX", path, (rim_num)getpid());
    rim_mem_set_len (ufile, wb+1);
    rim_free_int (rnd);
    //
    // CANNOT USE RND BEYOND THIS POINT
    //

    // 
    // make directory based on random number
    // even if mkdir fails, maybe mkstemp will not, so do not error out
    //
    mkdir (path, 06770);

    rim_num fd;
    if ((fd = mkstemp (ufile)) == -1)
    {
        rim_report_error ("Cannot create unique file, error [%s]", strerror(errno));
    }
    FILE *f = fdopen (fd, "w");
    if (f == NULL)
    {
        rim_report_error ("Cannot get file pointer from file descriptor [%ld], error [%s]", fd, strerror (errno));
    }
    *write_dir = ufile;
    return f;
}


//
// Upper-cases string 's', returns upped value as well
// 's' is both input and output param.
//
char *rim_upper(char *s)
{
    rim_num l = 0;
    while (s[l] != 0) {s[l] = toupper(s[l]); l++;}
    return s;
}

// 
// Lower-cases string 's' and returns it.
// 's' is both input and output param.
//
char *rim_lower(char *s)
{
    rim_num l = 0;
    while (s[l] != 0) {s[l] = tolower(s[l]); l++;}
    return s;
}

// 
// Copy file src to file dst. 
// Returns RIM_ERR_OPEN if cannot open source, RIM_ERR_CREATE if cannot open destination, RIM_ERR_READ if cannot read source,
// RIM_ERR_WRITE if cannot write destination, number of bytes copied if okay.
// Uses 8K buffer to copy file. 
//
rim_num rim_copy_file (char *src, char *dst)
{

    rim_num f_src = open(src, O_RDONLY);
    if (f_src < 0) {
        RIM_ERR; 
        return RIM_ERR_OPEN;
    }
    rim_num f_dst = open(dst, O_WRONLY|O_CREAT, S_IRWXU);
    if (f_dst < 0) 
    {
        RIM_ERR;
        close (f_src);
        return RIM_ERR_CREATE;
    }
    char buf[8192];
    rim_num total_written = 0;

    if (ftruncate64 (f_dst, 0) != 0) 
    {
        RIM_ERR;
        close (f_src);
        close (f_dst);
        return RIM_ERR_WRITE;
    }; // truncate destination just prior to write, not before
                            // otherwise copying smaller content into an existing file leaves the
                            // remainder of the existing data!!! (say copy "xy" to "xyz", the result is "xyz")
    while (1) 
    {
        ssize_t res = read(f_src, &buf[0], sizeof(buf));
        if (res == 0) break;
        if (res < 0) 
        {
            RIM_ERR;
            close (f_src);
            close (f_dst);
            return RIM_ERR_READ;
        }
        ssize_t rwrite= write(f_dst, &buf[0], res);
        if (rwrite != res) 
        {
            RIM_ERR;
            close(f_src);
            close(f_dst);
            return RIM_ERR_WRITE;
        } else total_written += rwrite;
    }
    close (f_src);
    close (f_dst);
    return total_written;
}




//
// Release split-string resources given by broken_ptr
//
void rim_delete_break_down (rim_split_str **broken_ptr)
{
    rim_num i;
    // delete data, any referenced will stay
    for (i = 0; i < (*broken_ptr)->num_pieces; i++) rim_free ((*broken_ptr)->pieces[i]);
    rim_free ((*broken_ptr)->pieces);
    (*broken_ptr)->num_pieces = 0;
    rim_free (*broken_ptr);
    *broken_ptr = NULL;
}


// 
// Break down 'value' string into pieces, with 'delim' being the delimiter.
// For example, 'x+y+z' could be 'value' and 'delim' could be '+'. 
// The result is stored into datatype rim_split_str's variable 'broken' which is
// allocated if currently NULL.
// This variable has 'num_pieces' as a number of values broken into, and 
// 'pieces[]' array that holds this number of pieces.
void rim_break_down (char *value, char *delim, rim_split_str **broken_ptr)
{

    // get object for parsing
    *(broken_ptr) = (rim_split_str*)rim_malloc (sizeof (rim_split_str));
    rim_split_str *broken = *broken_ptr;

    // setup memory for 128 pieces and expand later if necessary
#ifdef DEBUG
#define MAX_BREAK_DOWN 32
#else
#define MAX_BREAK_DOWN 128
#endif

    rim_num tot_break = MAX_BREAK_DOWN;

    // see comment for allocating ipars in rim_get_input()
    broken->pieces = (char**)rim_malloc(tot_break * sizeof(char*)+1);

    rim_num curr_break = 0;
    char *curr_value = value;
    rim_num delim_len = strlen (delim);
    char *pos_delim = NULL;

    while (1)
    {
        // pos_delim is what's after the next delim, or NULL if no delim
        pos_delim = rim_find_keyword0 (curr_value, delim, 0, 0);
        char *piece = curr_value; // piece is what we're currently parsing

        // right trim
        char *end_piece;
        char first_of_delim=0;
        char trim_char=0;
        // end when no delimiter found or string ends right after delimiter
        if (pos_delim==NULL)
        {
            end_piece = piece + strlen (piece); 
        }
        else
        {
            // if delimiter found, curr_value becomes the string right after delimiter
            first_of_delim = *pos_delim;
            curr_value = pos_delim + delim_len; // next value to parse
            *pos_delim = 0; // cap previous value
            // position end_piece to 0 after the end of previous string
            end_piece = pos_delim;
        }
        while (isspace(*piece)) piece++; // left trim of previous string
        if (*piece == '"') piece++; // get passed left quote in previous string
        rim_num len;
        if (end_piece != piece)  // check if not empty field
        {
            // if p isn't empty, i.e. if it doesn't point to 0
            end_piece--;
            // no need to check end_piece!=p because piece is either non-space or equal to end_piece
            while (isspace(*end_piece)) end_piece--; // right trim previous string
            if (*end_piece == '"') end_piece--; // get passed right quote
            end_piece++; // end_piece points to last non-white-space, so zero out the next one
            if (end_piece != pos_delim) // if there was nothing to trim, then end of piece (end_piece) is the same as keyword delimiter(pos_delim)
            {
                trim_char = *end_piece; // save char that's trimmed on the right of previous string
                *end_piece = 0;
            }
            len = end_piece - piece;
        } else { len = 0; end_piece = NULL;} // this is if previous string is empty (after trimming)
        // alloc mem for pieces
        (broken->pieces)[curr_break] = rim_strdupl(piece, 0, len);
        // restore break char after strdupl, because strdupl will copy at least one char, assuming there's a null afterwards
        if (pos_delim != NULL) *pos_delim = first_of_delim;
        if (end_piece != NULL && end_piece != pos_delim) *end_piece = trim_char;
        if (pos_delim==NULL)  break;

        // move forward, expand buffer if needed
        curr_break++;
        if (curr_break >= tot_break)
        {
            tot_break += MAX_BREAK_DOWN;
            broken->pieces = (char**) rim_realloc (rim_mem_get_id(broken->pieces), tot_break * sizeof(char*));
        }
    }
    broken->num_pieces = curr_break+1;
}


//
// Returns time string (now, in the future, or the past)
//
// Input parameter timezone is the name of TZ (timezone) variable to be set. So to get GMT time
// then timezone should be "GMT", if it is Mountain Standard then it is "MST" etc.
//
// 'format' is the format according to strftime(). If NULL, system time suitable for cookies is used.
//
// Input parameters year,month,day,hour,min,sec are the time to add to current time (can be negative too).
// So for example ..(0,0,1,0,0,1) adds 1 day and 1 second to the current time, while
// .. (0,-1,0,0,0) is one month in the past.
//
// Returns time suitable for many purposes, including cookie time (expires).
//
// The format of the return string is thus important and must NOT be changed.
// This time MUST be system time. Used ONLY for system purposes, such as
// cookie time, which must be system since system delivers Date in HTTP header
// to browser, so we MUST use system time in browser as well.
//
// This function will RESTORE the timezone back to what it was when the program first started. So it
// will temporarily set TZ to timezone variable, but before it exits, it will restore TZ to what it was.
// If curr is -1, then current time is the basis for any time transformations and subsequent formatting here; 
// if curr is otherwise, then that time is used (add/sub day, second etc) and format
//
char *rim_time (time_t curr, char *timezone, char *format, rim_num year, rim_num month, rim_num day, rim_num hour, rim_num min, rim_num sec)
{

    char set_gm[200];
    
    // set timezone to be used
    snprintf (set_gm, sizeof(set_gm), "TZ=%s", timezone);

    //make sure timezone is always GMT prior to using time function
    putenv(set_gm);
    tzset();

    // get absolute time in seconds
    time_t t;
    if (curr == (time_t)-1) t = time(NULL); else t=curr;
    //
    struct tm tm = *localtime(&t);
    struct tm future;       /* as in future date */

    // get future time
    future.tm_sec = tm.tm_sec+sec;
    future.tm_min = tm.tm_min+min;;
    future.tm_hour = tm.tm_hour+hour;
    future.tm_mday = tm.tm_mday+day;
    future.tm_mon = tm.tm_mon+month;
    future.tm_year = tm.tm_year+year; // years into the future 
    future.tm_isdst = -1;          /* try automaitic, may not work only within 1 hour before DST switch and 1 hour after*/

    // verify time is correct
    t = mktime( &future );
    if ( -1 == t )
    {
        //
        // Set result of rim_get_tz to mutable char *, since putenv does NOT 
        // modify its parameter. The result of rim_get_tz must NOT be modified.
        //
        putenv((char*)rim_get_tz());
        tzset();
        rim_report_error ("Error converting [%d-%d-%d] to time_t time since Epoch", future.tm_mon + 1, future.tm_mday, future.tm_year + 1900);
    }

    // convert time into GMT string suitable for cookies (this function is for
    // cookies ONLY or for anything that needs GMT time in this format)
#define GMT_BUFFER_SIZE 50
    char *buffer=(char*)rim_malloc(GMT_BUFFER_SIZE);
    size_t time_succ = strftime(buffer,GMT_BUFFER_SIZE-1, format==NULL ? "%a, %d %b %Y %H:%M:%S %Z":format, &future);
    if (time_succ == 0)
    {
        rim_report_error ("Error in storing time to buffer, buffer is too small [%d]", GMT_BUFFER_SIZE);
    }
    rim_mem_set_len (buffer, time_succ+1);
    
    // go back to default timezone. See above about casting rim_get_tz()
    // to (char*)
    putenv((char*)rim_get_tz());
    tzset();

    return buffer;
}



// Get string representation of integer 'var' into 'data'.
// Returns bytes written.
// 'data' will be a new pointer to allocated data.
//
rim_num rim_copy_data_from_num (char **data, rim_num val)
{
    char n[30];
    snprintf (n, sizeof (n), "%ld", val);
    return rim_copy_data (data, n);
}


//
// Set environment for URL passed to batch program from command line.
// Does the same as GET request from command line
// arg is the query string (i.e. 'req=request&par1=...')
//
void rim_set_env(char *arg)
{
    putenv ("REQUEST_METHOD=GET");
    
    char req[4096];
    snprintf (req, sizeof(req), "QUERY_STRING=%s", arg);
    putenv (req);
}


//
// Return value of env variable var from OS only
// Return "" if not found.
//
char *rim_getenv_os (char *var)
{
    char *v = secure_getenv (var);
    if (v == NULL) return RIM_EMPTY_STRING; else return v;
}


//
// Return value of env variable var from web server only (if server)
// or from system environment (if command line)
//
char *rim_getenv (char *var)
{
#ifndef RIM_COMMAND
    return rim_gen_get_env(var);
#else
    return rim_getenv_os (var); 
#endif
}


static inline rim_num rim_write_after_header (bool iserr, rim_config *pc, char *s, rim_num nbyte)
{
    // rim_gen_header_end() will set data_was_output to 1
    if (pc->ctx.req->data_was_output == 0) rim_gen_header_end (); // send cookies and \r\n divider
    return rim_gen_write (iserr, s, nbyte);  // send actual data
}

//
// Write web output. However, if header not sent, error out that header/data MAY NOT be output. 
// The worst that happens is that headers and/or data are not output. We also document this.
// pc is rimstone configuration. The rest the same as rim_gen_write().
// iserr is true if output goes to stderr, otherwise stdout - this is for web output only.
// Returns the same as rim_gen_write()
//
rim_num rim_write_web (bool iserr, rim_config *pc, char *s, rim_num nbyte)
{
    if (pc->ctx.req->sent_header == 1) 
    {
        return rim_write_after_header (iserr, pc, s, nbyte);
    } 
    else
    {
        if (pc->ctx.rim_report_error_is_in_report == 0) 
        {
            rim_output_http_header(pc->ctx.req); // if no header output, do it first. User can output with out-header in any way they want
            return rim_write_after_header (iserr, pc, s, nbyte);
            // used to be rim_report_error ("Attempting to write to web without outputting header first, use out-header statement");
        }
        else
        {
            // No need to try and output header because if pc->ctx.rim_report_error_is_in_report != 0, then in rim_report_error()
            // we will call rim_server_error() which will output 500 error, so it's better not to try
            // this allows to send to stderr even if header not output
            return rim_gen_write (iserr, s, nbyte);
        }
        return 0; // just to satisfy the compiler, since we're never coming here
    }
    return 0; // just to satisfy the compiler, since we're never coming here
}

//
//
//
//
// Begin FastCGI IO
//
//
//
//
   

//
// Write data to web output
// write data s of length nbyte
// if is_error is 1, write to stderr, otherwise for 0, write to stdout
// returns number of bytes written or -1 if error
// returns nbyte if output is disallowed or if fcgi request not initialized
//
rim_num rim_gen_write (bool is_error, char *s, rim_num nbyte)
{
    if (rim_finished_output == 0) 
    {
#ifndef RIM_COMMAND
        // when rim_fcgi_out is not NULL, neither are others like rim_fcgi_err
        if (rim_fcgi_out != NULL) 
        {
            if (FCGX_PutStr ((char*)s, nbyte, is_error ? rim_fcgi_err :rim_fcgi_out) != nbyte) return -1; else return nbyte;
        } else return nbyte;
#else
        if ((rim_num)fwrite_unlocked((char*)s, 1, nbyte, is_error ? stderr : stdout) != nbyte) return -1; else return nbyte;
#endif
    } else return nbyte;
}


#ifndef RIM_COMMAND
//
// Get environment variable 
// n is the name of environment variable. Returns "" if not found.
//
static char *rim_gen_get_env (char *n)
{
    char *v;
    if (rim_finished_output == 0) 
    {
#ifndef RIM_COMMAND
        v = FCGX_GetParam (n, rim_fcgi_envp);
#else
        v = secure_getenv (n);
#endif
        if (v == NULL) return RIM_EMPTY_STRING; else return v;
    } else return RIM_EMPTY_STRING;
}
#endif



//
// Set content length for web output
//
static void rim_gen_set_content_length(char *v)
{
    if (rim_finished_output == 0 && rim_get_config()->ctx.req != NULL && rim_get_config()->ctx.req->silent == 0) 
#ifndef RIM_COMMAND
        if (rim_fcgi_out != NULL) FCGX_FPrintF (rim_fcgi_out, "Content-length: %s\r\n", v);
#else
        fprintf (stdout, "Content-length: %s\r\n", v);
#endif
}



//
// Read data from web client (like POST upload for example)
// content is the data buffer where reading is done into; it must be allocated with at least
// len+1 bytes. len is the length to read. Returns 1 if okay, 0 if not.
//
static rim_num rim_gen_util_read (char *content, rim_num len)
{
    if (rim_finished_output == 0) 
    {
        rim_num bytes_read;
        rim_num total_read = 0;
        while (total_read < len) {
#ifndef RIM_COMMAND
            bytes_read = FCGX_GetStr (content + total_read, len - total_read, rim_fcgi_in);
#else
            bytes_read = (rim_num)fread_unlocked (content + total_read, 1, len - total_read, stdin);
#endif
            if (bytes_read == 0)
            {
                return 0; // could not read
            } 
            total_read += bytes_read;
        }
        content[len] = 0;
        return 1;
    }
    else
    {
        content[0] = 0;
        return 1;
    }
}


//
// Add header for web output
// RIght now, adding is the same as setting; in the future it should be different
// (adding always adds, setting replaces or adds if doesn't exist)
// n is the header name, v is value.
//
static void rim_gen_add_header (char *n, char *v)
{
    if (rim_finished_output == 0 && rim_get_config()->ctx.req != NULL && rim_get_config()->ctx.req->silent == 0) 
#ifndef RIM_COMMAND
        if (rim_fcgi_out != NULL) FCGX_FPrintF (rim_fcgi_out, "%s: %s\r\n", n, v);
#else
        fprintf (stdout, "%s: %s\r\n", n, v);
#endif
}

// 
// Send new line between header and body.
// This will send any cookies as well. It will remember that this was done and if called again for the
// same request, it will not perform these actions again, as they need to be done only once per request.
//
static void rim_gen_header_end ()
{
    // because this is done for all output, including errors and such, make sure req is not NULL,
    // also this output was not already done, and also this isn't silent header.
    // but do set data_was_output to 1 even if silent, so this function doesn't get called needlessly
    if (rim_get_config()->ctx.req != NULL && rim_get_config()->ctx.req->data_was_output == 0)
    {
        if (rim_get_config()->ctx.req->silent == 0)
        {
            // Cookies 
            rim_num ci;
            for (ci = 0; ci < rim_get_config()->ctx.req->num_of_cookies; ci++)
            {
                // we send back ONLY cookies set by set-cookie or delete-cookie. Cookies we received and are there
                // but were NOT changed, we do NOT send back because they already exist in the browser. Plus we do NOT
                // keep expired and path, so we would not know to send it back the way it was.
                if (rim_get_config()->ctx.req->cookies[ci].is_set_by_program == 1)
                {
                    rim_gen_add_header ("Set-Cookie", rim_get_config()->ctx.req->cookies[ci].data);
                }
            }
            // send final \r\n to mark the end of headers
            rim_gen_write (false, "\r\n", 2);
        }
        rim_get_config()->ctx.req->data_was_output = 1; // once data was output, do not output end of header again 
    }
}

//
// Set content type for web output
// v is content type (text/html for instance)
//
static void rim_gen_set_content_type(char *v)
{
    if (rim_finished_output == 0 && rim_get_config()->ctx.req != NULL && rim_get_config()->ctx.req->silent == 0) 
#ifndef RIM_COMMAND
        if (rim_fcgi_out != NULL) FCGX_FPrintF (rim_fcgi_out, "Content-Type: %s\r\n", v);
#else
        fprintf (stdout, "Content-Type: %s\r\n", v);
#endif
}


//
// FLush any fcgi output
//
void rim_flush_out(void)
{
#ifndef RIM_COMMAND
    if (rim_fcgi_out != NULL) FCGX_FFlush (rim_fcgi_out);
    if (rim_fcgi_err != NULL) FCGX_FFlush (rim_fcgi_err);
#else
    fflush (stdout);
    fflush (stderr); // not needed as it's done automatically; still.
#endif
}

//
// This is called for finish-output. It is server push of any output. After this, no 
// output already done is outstanding in the buffers.
//
void rim_SERVICE_Finish (void)
{
//    No finish, since we're in the loop, and once we exit it doesn't matter. 
//    FCGX_Accept will free all the data from the previous request.

    if (rim_finished_output == 0) 
    {
#ifndef RIM_COMMAND
        // set status both stdout and err. SERVICE says the code will go with the last
        // closed/flushed stream. Given they can be intermixed, it's not clear which one that is.
        // Looking at SERVICE source code, seems stdout is where it should be but not sure.
        // So set both
        FCGX_SetExitStatus(rim_get_config()->ctx.req->ec, rim_fcgi_err);
        FCGX_SetExitStatus(rim_get_config()->ctx.req->ec, rim_fcgi_out);
        if (rim_fcgi_out != NULL) FCGX_FFlush (rim_fcgi_out);
        FCGX_Finish();
        // these are closed, so do not try to use them , use NULL as a flag
        rim_fcgi_in = NULL;
        rim_fcgi_out = NULL; 
        rim_fcgi_err = NULL;
#else
        fflush (stdout);
#endif
    }
    rim_finished_output = 1; // this says that output is finished and this flag will guard against any
                         // output that follows (i.e. any output will not output anything)
}

//
//Exit, release resources. This is done in debug mode for testing only. The reason is if something fails,
//it doesn't really matter as it would be released anyway, and we'd miss the chance to properly return exit code, which
//is the only code currently that isn't DEBUG-shielded.
//
void rim_exit (void)
{
    rim_config *pc = rim_get_config();
    RIM_UNUSED(pc); // for web server non-debug, not used

#ifdef RIM_COMMAND
    // This must be *before* the cleaning down here, because ..req-> anything is managed
    // memory and will be destroyed in rim_done()!!!
    int retcode = (pc != NULL ? pc->ctx.req->ec : RIM_DISTRESS_STATUS); // return code for command line program
                                                                       // this is when say memory violation is caught
#endif

#ifdef DEBUG
    rim_end_all_db (); // end any db connections

    if (pc != NULL && pc->ctx.db->conn != NULL) free (pc->ctx.db->conn); // free database list of descriptors

    // Gliim memory shutdown and deallocation
    rim_done(); // clean up all the rimstone memory
#endif



#ifdef DEBUG
    // Free program context, the VERY LAST thing before exit
    if (pc != NULL) free(pc); 
#endif

    //
    // Final exit, nothing else must be here or after!!!
    //
#ifndef RIM_COMMAND
    exit(0);
#else
    exit (retcode);
#endif
}

//
// Called at the top of the loop for SERVICE. For command line, just return 1 and continue.
// Gliim (managed) memory doesn't exist at this point, and pc->ctx.req is not allocated yet.
// So this must not do anything that may use such memory.
//
rim_num rim_SERVICE_Accept (void)
{
#ifndef RIM_COMMAND
    rim_finished_output = 1; // if not, first time accept may try to write to SERVICE; this is not clear why it would
                         // but regardless, this will prevent any output until the next request starts
                         // It may have been because rim_SERVICE_Finish wasn't called properly until 16.9
                         // in the main loop at the end of each request (but rather it was called only for exiting)
    // FCGX_Accept calls FCGX_Finish first thing, so no need to call prior
    return FCGX_Accept(&rim_fcgi_in, &rim_fcgi_out, &rim_fcgi_err, &rim_fcgi_envp);
#else
    return 1; // just to enter the loop one time
#endif
}

//
// Set page status for web output
// st is the status number, line is the text (200, "OK" for example)
//
static void rim_gen_set_status (rim_num st, char *line)
{
    if (rim_finished_output == 0 && rim_get_config()->ctx.req != NULL && rim_get_config()->ctx.req->silent == 0) 
    {
        // fastcgi fprintf doesn't know %ld, and status text isn't taken
#ifndef RIM_COMMAND
        if (rim_fcgi_out != NULL) FCGX_FPrintF (rim_fcgi_out, "Status: %ld %s\r\n", (long)st, line);
#else
        fprintf (stdout, "Status: %ld %s\r\n", (long)st, line);
#endif
    }

}


//
//
//
//
// End FastCGI IO
//
//
//
//

//
// Returns 0 if header sent. Set status of header to 'sent'
// pc is program context
//
static rim_num rim_header_err(rim_config *pc)
{
    // if program receives TERM signal at acceping connection and thus exits, this will get called through stack/error reporting
    // and there is no request at that point (it's NULL)
    if (pc->ctx.req != NULL) 
    {
        if (pc->ctx.req->sent_header == 1) 
        {
            // if program ended before header could have been finished, finish the header
            if (pc->ctx.req->data_was_output == 0) rim_gen_header_end (); // send cookies and \r\n divider
            return 0;
        }
        pc->ctx.req->sent_header = 1;
    }
    return 1;
}


// 
// Output error message if Gliim or Gliim app encountered server or application error that cannot be fixed.
// This includes report-error, which is fatal.
// Output is 500 Internal Server Error
// No reason is sent to the client. Separately stderr is written with error message.
//
static void rim_server_error ()
{
    rim_config *pc = rim_get_config();
    if (rim_header_err(pc) != 1) return;
    rim_gen_set_status (500, "Internal Server Error");
    rim_gen_set_content_type("text/html;charset=utf-8");
    rim_gen_header_end ();
}

// 
// Output error message if file requested (image, document.., a binary file in 
// general) could not be served.
//
static void rim_cant_find_file ()
{
    rim_config *pc = rim_get_config();
    if (rim_header_err(pc) != 1) return;
    // never print out for batch mode
    rim_gen_set_status (404, "Not Found");
    rim_gen_set_content_type("text/html;charset=utf-8");
    rim_gen_header_end ();
}


// 
// Output a file. HTML output must be disabled for this to work. Typically, binary files
// (images, documents) are served to the web browser this way.
// fname is file name. 'header' is the header to be output (which must be set by the caller).
// This handles 'if-none-match' (timestamp) so that if web client already has this binary file,
// only the cache confirmation is sent back thus improving performance ('not modified' response).
// File must exist or it will return 'document requested not found' to the client.
//
void rim_out_file (char *fname, rim_header *header)
{
    rim_config *pc = rim_get_config();

    // this must be FIRST, before rim_disable_output() as we check if being output
    if (rim_validate_output()!=1) rim_report_error ("Cannot send file because output is disabled, or file already output");

    if (pc->ctx.req->sent_header == 1) 
    {
        rim_report_error ("HTTP header has already been output; you must not output header prior to sending file");
    }


    rim_disable_output();

    if (strstr (fname, "..") != NULL)
    {
        //
        // We do not serve files with .. in them to avoid path traversal attacks. Files must
        // not traverse backwards EVER.
        //
        rim_cant_find_file();
        return;
    }


    struct stat attr;
    if (stat(fname, &attr) != 0)
    {
        rim_cant_find_file();
        return;
    }
    long tstamp = (long)attr.st_mtime;

    int f = rim_fopen (fname, O_RDONLY);
    if (f == -1)
    {
        rim_cant_find_file();
        return;
    }
    else
    {
        //
        // We're using long longs throughout Gliim
        //
        off_t fsize_l = lseek(f, 0, SEEK_END);
        if (fsize_l == (off_t)-1)
        {
            rim_cant_find_file();
            return;
        }
        rim_num fsize = (rim_num) fsize_l;
        lseek(f, 0, SEEK_SET);

        // 
        // check if file has already been delivered to the client
        //
        if (pc->ctx.req->if_none_match != NULL && tstamp == atol(pc->ctx.req->if_none_match))
        {
            //
            // File NOT modified
            //

            rim_gen_add_header ("Status", "304 Not Modified");
            if (header->cache_control!=NULL)
            {
                rim_gen_add_header ("Cache-Control", header->cache_control);
            }
            else
            {
                rim_gen_add_header ("Cache-Control", "max-age=0, no-cache");
                rim_gen_add_header ("Pragma", "no-cache");
            }
            // Before sending any contents, must send \n 
            rim_gen_header_end ();

            close (f);
            return;
        }

        // 
        // read file to be sent to the client
        //
        char *str = rim_malloc0(fsize + 1);
        if (read(f, str, fsize) != fsize)
        {
            free (str);
            rim_cant_find_file();
            return;
        }
        close(f);

        //
        // The data read is in 'str' and the size of data is 'fsize'
        // which is what the following code expects.
        //


        char tm[50];
        char *val;

        if (header->ctype[0] == 0)
        {
            //
            // Content type is missing, we assume it's HTML
            //
            // html has to be empty because we have own set of headers

            if (header->etag==1)
            {
                snprintf (tm, sizeof(tm), "%ld", tstamp);
                RIM_STRDUP (val, tm);
                rim_gen_add_header ("Etag", val);
            }


            // outputting the header for html will not work if web output is disallowed
            // we will allow it momentarily and then go back to what it was.
            rim_num saved = pc->ctx.req->disable_output;
            pc->ctx.req->disable_output = 0;
            rim_output_http_header(pc->ctx.req); // all extract tags like Etag must come before because this will
                        // end the header section with two CRLF
            pc->ctx.req->disable_output = saved;
        }
        else
        {
            // 
            // Send file and appropriate header first
            //
            char disp_name[500];
            if (header->disp != NULL)
            {
                if (header->file_name != NULL)
                {
                    char *enc = NULL;
                    // This encode does NOT operate on allocated memory, so we must provide length instead of -1 (or it will attempt
                    // to get length from header->file_name, which isn't Gliim alloc-memory!
                    int bw = (int)rim_encode (RIM_URL, header->file_name, strlen(header->file_name), &enc, false); // header->file_name is not alloc'd mem
                    snprintf (disp_name, sizeof(disp_name), "%s; filename*=UTF8''%.*s", header->disp, bw, enc);
                    rim_free_int (enc);
                } 
                else 
                {
                    snprintf (disp_name, sizeof(disp_name), "%s", header->disp);
                }
            }

            RIM_STRDUP (val, header->ctype);
            rim_gen_set_content_type(val);
            snprintf (tm, sizeof(tm), "%ld", fsize);
            RIM_STRDUP (val, tm);
            rim_gen_set_content_length(val);
            if (header->disp != NULL)
            {
                rim_gen_add_header ("Content-Disposition", disp_name);
            }
            if (header->cache_control!=NULL)
            {
                rim_gen_add_header ("Cache-Control", header->cache_control);
            }
            else
            {
                rim_gen_add_header ("Cache-Control", "max-age=0, no-cache");
                rim_gen_add_header ("Pragma", "no-cache");
            }

            // send etag for client to send back when asking again. If ETag is the same
            // for this file name, then it will be considered cached at the client and
            // 'not modified' message will be sent back.
            if (header->etag==1)
            {
                snprintf (tm, sizeof(tm), "%ld", tstamp);
                RIM_STRDUP (val, tm);
                rim_gen_add_header ("Etag", val);
            }

            // add any headers set from the caller
            rim_num i;
            for (i = 0; i<RIM_MAX_HTTP_HEADER; i++)
            {
                if (header->control[i]!=NULL && header->value[i]!=NULL)
                {
                    // we use add_header because it allows multiple directives of the same kind
                    // but must not make duplicates of what's already there, except for Set-Cookie
                    rim_gen_add_header (header->control[i], header->value[i]);
                }
                else break;
            }
            pc->ctx.req->sent_header = 1; // because we just sent a header, and rim_write_web() needs this info    

        }

        // Send actual file contents
        if (rim_write_web (false, pc, str, fsize) != fsize)
        {
            // In this case, since rim_gen_write is synchronous, the server couldn't send all the data and it closed the connection
            // with the client. Nothing else to do for us.  
        }
        free (str);

    }
}


// 
// Output error message if request method is bad, for whatever reason
// Output is 400 Bad Request
// No reason is sent to the client. Separately stderr is written with error message.
// Does nothing for standalone
//
void rim_bad_request ()
{
    rim_config *pc = rim_get_config();
    if (rim_header_err(pc) != 1) return;
    rim_gen_set_status (400, "Bad Request");
    rim_gen_set_content_type("text/html;charset=utf-8");
    rim_gen_header_end ();
}



//
// report-error exit implementation. Go back prior to request, then goto right before rim_shut in the main loop.
// We guard against going nowhere by jumping only if sigsetjmp was done
// If we can't jump, we exit because we surely don't want to continue after report-error
//
void rim_error_request(rim_num retval)
{
    if (rim_done_err_setjmp == 1) 
    {
        siglongjmp (rim_err_jmp_buffer, retval);
    }
    else
    {
        // do not exit if in handling signal in chandle.c, which will fatally exit afterwards
        // this allows to continue and print any remaining error messages there
        if (rim_in_fatal_exit == 0)
        {
            RIM_FATAL ("Cannot complete long jump from report-error"); // if cannot jump and recover, exit
        }
    }
}

//
// exit-service implementation. Go back prior to request, then goto right after so after-request is done.
// It CAN be called in _before(), in which case it goes straight to after_request().
// We guard against going nowhere by jumping only if sigsetjmp was done
//
void rim_exit_request(rim_num retval)
{
    if (rim_done_setjmp == 1) siglongjmp (rim_jmp_buffer, retval);
    // else do nothing
}



//
//
// Set application params at run time. config file is not required. 
// 
//
// MUST NOT USE RIM_MALLOC and such. This is allocated once per execution and if rim_malloc/*, it would be deallocated at the end of each request in fcgi!!
//


void rim_get_runtime_options()
{

    rim_config *pc = rim_get_config ();

    static char db_dir_name[RIM_MAX_OS_UDIR_LEN];
    static char app_dir_name[RIM_MAX_OS_UDIR_LEN];
    static char file_dir_name[RIM_MAX_OS_UDIR_LEN];
    static char trace_dir_name[RIM_MAX_OS_UDIR_LEN];

    rim_dir (RIM_DIR_DB, db_dir_name, sizeof(db_dir_name), rim_app_name, NULL);
    pc->app.dbconf_dir = db_dir_name;

    rim_dir (RIM_DIR_APP, app_dir_name, sizeof(app_dir_name), rim_app_name, NULL);
    pc->app.home_dir = app_dir_name;

    pc->app.user_dir = rim_user_dir;


    rim_dir (RIM_DIR_FILE, file_dir_name, sizeof(file_dir_name), rim_app_name, NULL);
    pc->app.file_dir = file_dir_name;

    pc->app.max_upload_size  = rim_max_upload;

    // Make sure that program cannot setuid to root
    if (setuid(0) == 0 || seteuid(0) == 0)
    {
        RIM_FATAL ("Program can never run as effective user ID of root");
    }

    // Get working directory when the process has started
    if ((pc->app.run_dir = getcwd(NULL, 0)) == NULL) 
    {
        RIM_FATAL ("Cannot allocate memory for run directory, error [%m]");
    }

    // We can now set directories, and use ~ based on user to get home directory, if available 
    if (chdir (pc->app.home_dir) != 0) // set current directory to be HOME_DIR
    {
        RIM_FATAL ("Cannot change directory to [%s], error [%m]", pc->app.home_dir);
    }

    //
    // END OF SETTING UID and home directory 
    //

    //
    // This is always last!!!! Since we return the file pointer here.
    // trace dir is created by mrim
    rim_dir (RIM_DIR_TRACENAME, trace_dir_name, sizeof(trace_dir_name), rim_app_name, NULL);
    pc->app.btrace = trace_dir_name;
    return;
}

// CAN USE RIM_MALLOC AGAIN

//
// Make random string based on current time/PID as random generator for srand().
// 'rnd' is the output, and rnd_len-1 is its length - rnd buffer will be allocated with
// rnd_len bytes of storage allocated on output, regardless of what it was coming in.
// rnd_len must be at least 2 (for 1 random byte).
// Generated random string does ends with zero byte
// (i.e. it is a valid string. The result
// is the numeric string if type is RIM_RANDOM_NUM, mixed digits and upper/lower-case letters if 
// RIM_RANDOM_STR, and binary if RIM_RANDOM_BIN. RIM_RANDOM_STR is default.
// If this is to be cryptographically secure pseudo random generator (CSPRNG) generator, then
// crypto is true. If this can't be done, then the error will be raised.
//
void rim_make_random (char **rnd, rim_num rnd_len, char type, bool crypto)
{

    *rnd = rim_malloc (rnd_len);
    rim_mem_set_len (*rnd, rnd_len); 

    if (crypto) 
    {
        // CSPRNG generator called by random-crypto
        if (rim_RAND_bytes((unsigned char*)(*rnd),(int)(rnd_len - 1)) != 1)
        {
            rim_report_error ("Cannot produce crypto random strng");
        }
        (*rnd)[rnd_len-1] = 0; // finish with zero for sanity, not a part of the actual random data
        return;
    }

    // Ensure next call to this function has a bit more random seed because time() will return the same value within
    // the same second
    // This is okay as static, even if this value is carried over from request to request, serving different modules,
    // and as it is, it actually increases the randomness of the result.

    // Character Source for final result
    static char ranged[] = "0123456789";
    static char rangea[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    static char rangeb[256]; // for binary random

    static char isinit = 0; // per process init flag


    // init the random see with PID and time
    // this is a single srand per process. So different requests will get somewhat random intervals
    // making this stronger than if srand-ed for each request. Even if the sequence starts rotating,
    // it becomes less predictable because each request gets only a random interval of the roll.
    // Another randomization is that the same request would likely be served from a number of processes,
    // each serving to it partial random intervals based on different seeds, further enhancing the quality.
    if (isinit == 0) 
    {
        // init the seed, make low-bytes of PID the high of seed, and keep low bytes of current time
        rim_num s = getpid()<<16;
        srand ((time(NULL)&0xFFFF)+s);
        // init rangeb for binary, from 0 to 255
        // this way binary random is always null terminated
        rim_num i;
        for (i = 0; i < 256; i++) rangeb[i] = i;
        // init once
        isinit = 1;
    }


    rim_num i;
    // get number of random characters requested
    if (type == RIM_RANDOM_STR) for (i = 0; i < rnd_len - 1; i++) (*rnd)[i] = rangea[random()%62]; // 62 chars in rangea[], start with default
    else if (type == RIM_RANDOM_NUM) for (i = 0; i < rnd_len - 1; i++) (*rnd)[i] = ranged[random()%10]; // number 0-9 
    else if (type == RIM_RANDOM_BIN) for (i = 0; i < rnd_len - 1; i++) (*rnd)[i] = rangeb[random()%256]; // 256 values, from 0 to 255
    else rim_report_error ("Unknown random type [%d]", type);
    (*rnd)[i] = 0; // finish with zero for sanity, this is byte rnd[rnd_len], not a part of random data

}



//
// get realpath of file 'path' as rim_string. Path can be of a file or directory.
// returns realpath.
//
char *rim_realpath (char *path)
{
    char *res;
    char *pcopy = rim_strdup (path);
    if ((res = realpath (dirname (pcopy), NULL)) != NULL)
    {
        rim_free_int (pcopy);
        char *retval = rim_strdup(res); // in order to avoid memory leaks
                                          // correct length set in strdup
        free (res);
        return retval;
    } 
    else 
    {
        RIM_ERR;
        rim_free_int (pcopy);
        return RIM_EMPTY_STRING;
    }
}

//
// hex to bin data. Convert hex src to dst, which is allocated. src is of len 'ilen',
// ilen can be -1
// in which case length of src is computed via strlen.
//
void rim_hex2bin(char *src, char **dst, rim_num ilen)
{
    rim_num lsrc = rim_mem_get_len(src);
    if (ilen == -1) ilen = lsrc;
    else if (ilen > lsrc) rim_report_error ("Memory read requested of length [%ld] but only [%ld] allocated", ilen, lsrc);

    *dst = (char*)rim_malloc (ilen/2 + 2); // +2 in case bad data/odd # of bytes
    rim_num i;
    rim_num j = 0;
    for (i = 0; i < ilen; )
    {
        int h;
        int l;
        URLDIG(src[i],h);
        URLDIG(src[i+1],l);
        (*dst)[j++] = (h<<4)+l;
        i+=2;
    }
    (*dst)[j] = 0;
    rim_mem_set_len (*dst, j+1);
}

//
// Bin to hex data. Convert binary src to hex data, which is allocated. src
// is of len 'ilen'. 'pref' is added as prefix, unless NULL.
// ilen can be -1, in which case it is the string length of src.
//
void rim_bin2hex(char *src, char **dst, rim_num ilen, char *pref)
{
    rim_num lsrc = rim_mem_get_len(src);
    if (ilen == -1) ilen = lsrc;
    else if (ilen > lsrc) rim_report_error ("Memory read requested of length [%ld] but only [%ld] allocated", ilen, lsrc);
    
    rim_num l;
    if (pref != NULL) l = rim_mem_get_len(pref); else l = 0;
    *dst = (char*)rim_malloc (l+ilen*2+1);
    rim_num i;
    if (pref != NULL) memcpy (*dst, pref, l);
    rim_num j = l;
    for (i = 0; i < ilen; i ++)
    {
        RIM_HEX_FROM_BYTE ((*dst)+j, (unsigned int)src[i]);
        j += 2;
    }
    (*dst)[j] = 0;
    rim_mem_set_len (*dst, j+1);
}


//
// Return web environment variable from header h.
// HTTP_ is prefixed, and all - are replaced with _
//
char *rim_getheader(char *h)
{
    rim_num hlen = strlen (h);
    char *hd = rim_malloc0 (hlen + 6); // account for length of HTTP_ plus 1 byte
    memcpy (hd, "HTTP_", 5); // HTTP_ at the beginning
    memcpy (hd + 5, h, hlen + 1); // copy the rest of h after HTTP_ including null byte
    rim_upper (hd + 5); // convert to upper case, skip HTTP_
    rim_num i;
    for (i = 5; i < hlen + 5; i ++) if (hd[i] == '-') hd[i]='_'; // replace - with _, skip HTTP_
    char *res = rim_getenv(hd);
    free (hd);
    return res;
}




//
// Copy string src to dst, max of len bytes. Copies from byte from, which starts at 0.
// Works with binary strings as well
// Checks souce length, even if binary, so as not to over-read
//
void rim_copy_string (char *src, rim_num from, char **dst, rim_num len)
{
    if (len < 0)  rim_report_error ("Number of bytes to copy is negative [%ld]", len);
    rim_num src_len = rim_mem_get_len(src);
    if (from < 0 || from > src_len-1) rim_report_error ("Cannot copy from byte [%ld] which is beyond string [%ld]", from , src_len-1);
    rim_num to_copy = src_len+1-from; // length of source, including last trailing null
    if (to_copy > len) to_copy = len; // determine bytes to copy
    *dst = rim_malloc (len+1);
    ((char*)memcpy (*dst, src+from, len))[len] = 0; // copy len bytes and set len+1 byte to zero
}

//
// Sleep milli seconds.
//
void rim_sleepabit(rim_num milli) {
    struct timespec slp;
    slp.tv_sec = milli / 1000;
    slp.tv_nsec = (milli % 1000) * 1000000;
    nanosleep(&slp, NULL);
}

bool rim_is_service()
{
#ifdef RIM_COMMAND
    return false;
#else
    return true;
#endif
}


//
// Copy a string 'copy' inside string 'tgt', start at byte position swith (0 is the first byte) of
// length len. If begin is true, this is swith bytes from the beginning, otherwise swith bytes from the end.
// The null byte of 'copy' is not copied. Error is emitted in case of out-of-bounds situation.
// if len is negative, take the whole 'copy'. if swith is negative, it's zero if begin is true, or length(tgt)-length(copy) if false. 
// If len is zero, do nothing.
//
void rim_alter_string (char *tgt, char *copy, rim_num swith, rim_num len, bool begin)
{
    if (len == 0) return;
    rim_num t_len = rim_mem_get_len (tgt);
    rim_num c_len = rim_mem_get_len (copy);
    if (len < 0) len = c_len;
    else if (len > c_len)  rim_report_error ("Copying [%ld] bytes is more than length [%ld] of copy string", len, c_len);
    if (swith < 0) 
    {
        // this means with 'copy' in replace-string we copy at the beginning with copy-end we copy properly at the end so that 'copy' fits right to the end of target
        if (!begin) swith = len; else swith = 0;
    }
    // Start-with cannot go over length-1
    if (swith >= t_len) rim_report_error ("Starting position [%ld] would overrun buffer of [%ld] bytes", len, c_len);
    if (!begin) swith = t_len - swith;
    if (swith+len > t_len) rim_report_error ("Copying string from byte [%ld] of length [%ld] would overrun buffer of [%ld] bytes", swith, len, t_len);
    memcpy (tgt+swith, copy, len);
}


//
// Add 'add' to 'to' string, starting with beg in add of length len, and return new 'to', which can be realloc'd
// More memory is allocated than needed to avoid fragmentation, and after rim_add_string_part(s) are
// done (b/c this is meant for use in a+b+c... string expressions), final rim_realloc needs to be done.
//
void *rim_add_string_part (void *to, void *add, rim_num beg, rim_num len)
{
    rim_num l_to = rim_mem_get_len (to);
    rim_num l_add = rim_mem_get_len (add);

    // check beginning
    if (beg < 0 || beg > l_add - 1) rim_report_error ("Substring beginning [%ld] is beyond the string [%ld]", beg, l_add-1);
    // adjust len if -1 (through the end)
    if (len == -1) len = l_add-beg;
    // check length
    if (len < 0 || beg+len-1 > l_add - 1) rim_report_error ("Substring beginning [%ld] and length [%ld] are beyond the string [%ld]", beg, len, l_add-1);

    rim_num l_res = l_to+len+1;
    if (l_res < 256) l_res = 256;
    else l_res += 256;

    to = rim_realloc (rim_mem_get_id (to), l_res); // resize memory to just what's needed

    memcpy ((char*)to + l_to, (char*)add+beg, len);
    ((char*)to)[l_to+len] = 0;
    rim_mem_set_len (to, l_to+len+1);  // MUST use rim_mem_get_id() again
                                                          // because it changed with rim_realloc!
    return to;
}

//
// Add 'add' to 'to' string and return new 'to', which can be realloc'd
// More memory is allocated than needed to avoid fragmentation, and after rim_add_string(s) are
// done (b/c this is meant for use in a+b+c... string expressions), final rim_realloc needs to be done.
// 'to' can NOT be any variable that's used in RimStone, *only* a newly created result variable can be this.
//
void *rim_add_string (void *to, void *add)
{
    rim_num l_to = rim_mem_get_len (to);
    rim_num l_add = rim_mem_get_len (add);

    rim_num l_res = l_to+l_add+1;
    if (l_res < 256) l_res = 256;
    else l_res += 256;

    to = rim_realloc (rim_mem_get_id (to), l_res); // resize memory to just what's needed

    memcpy ((char*)to + l_to, (char*)add, l_add);
    ((char*)to)[l_to+l_add] = 0;
    rim_mem_set_len (to, l_to+l_add+1);  // MUST use rim_mem_get_id() again
                                                          // because it changed with rim_realloc!
    return to;
}




