// SPDX-License-Identifier: Apache-2.0
// Copyright 2019 Gliim LLC. 
// Licensed under Apache License v2. See LICENSE file.
// On the web http://golf-lang.com/ - this file is part of Golf framework. 

//
// Library used both by GOLF utility and GOLF run-time 
// trailing 'c' in the name of this file refers to 'common' 
// code.
//


#include "golf.h"
int gg_errno=0;
bool gg_optmem=false;

// Function prototypes
gg_num gg_core_write_file(FILE *f, gg_num content_len, char *content, char ispos, gg_num pos);

// 
// Close trace file 
// Returns GG_ERR_CLOSE if there is no context or not open, or the same
// as gg_fclose() if it's open
//
gg_num gg_close_trace ()
{
    gg_config *pc = gg_get_config();

    if (pc == NULL) return GG_ERR_CLOSE;

    // open trace file, for fcgi, it will not be NULL (from previous request)
    if (pc->trace.f != NULL)
    {
        return gg_fclose(pc->trace.f);    
    } else return GG_ERR_CLOSE;
}



// 
// Open trace file and write begin-trace message
// Returns 0 if opened, -1 if not
// Any memory alloc here MUST be malloc since it survives fcgi requests and continues
// over many such requests.
//
gg_num gg_open_trace ()
{
    gg_config *pc = gg_get_config();

    if (pc == NULL) return -1;

    // open trace file, for fcgi, it will not be NULL (from previous request)
    if (pc->trace.f != NULL)
    {
        return 0; // reuse tracing for the same process
    }


    if (pc->debug.trace_level > 0)
    {
        // timestamp, as PIDs can recycle
        gg_current_time (pc->trace.time, sizeof(pc->trace.time)-1);
        // append if file exists, otherwise open anew
        snprintf(pc->trace.fname, sizeof(pc->trace.fname), "%s/trace-%ld-%s", pc->app.trace_dir, gg_getpid(), pc->trace.time);
        pc->trace.f = gg_fopen (pc->trace.fname, "a+");
        if (pc->trace.f == NULL) 
        {
            pc->trace.f = gg_fopen (pc->trace.fname, "w+");
            if (pc->trace.f == NULL)
            {
                return -1; 
            }
        }
    }
    return 0;
}



// 
// Trace execution. This is called from GG_TRACE. 
// 'trace_level' is currently always 1. It is compared with the trace parameter in debug file, which is currently
// 0 or 1 (no tracing for 0 and tracing for 1). In the future, there may be more trace levels, with trace level 1
// including all tracing, trace level 2 including levels 2,3.. in trace file, etc.
// 'from_file' is the file name this trace is coming from.
// 'from_line' is the source line number, 'from_fun' is the function name.
// The rest (format,...) is printf-like data, the actual trace content.
// The trace also includes current time and PID.
//
// Trace can be called from memory function like gg_realloc. 
// If trace is called from anywhere else other than gg_* functions, it will  work the same way except there is no double calling of gg_check_memory.
//
void _gg_trace(gg_num trace_level, const char *from_file, gg_num from_line, const char *from_fun, char *format, ...)
{
    gg_config *pc = gg_get_config();
    if (pc == NULL) return; // do nothing if no config


    // THIS FUNCTON MUST NEVER USE ANY FORM OF MALLOC OR GG_MALLOC
    // or it may fail when memory is out or not available (such as in gg_malloc)


    // control which tracing will be done
    if (pc->debug.trace_level < trace_level) return;

    char trc[GG_TRACE_LEN + 1];

    // if this tracing came from this very function, ignore
    if (pc->trace.in_trace == 1) return;
    pc->trace.in_trace = 1;

    if (pc->trace.f == NULL) 
    {
        gg_open_trace(); // trace gets opened during fatal error, so open if not enabled
        if (pc->trace.f == NULL)  // if couldn't open, do not trace
        {
            pc->trace.in_trace = 0;
            return;
        }
    }

    va_list args;
    va_start (args, format);
    vsnprintf (trc, sizeof(trc) , format, args);
    va_end (args);

    // write time, code and message out
    // do NOT use pc->trace.time - this MUST stay constant during the request because it is
    // used in save_HTML to make sure name generated from this value is the same even if this name
    // is generated multiple times.
    // We do not specify PID as it is embedded in file name.
    char curr_time[200];
    gg_current_time (curr_time, sizeof(curr_time)-1);
    fprintf (pc->trace.f, "%s (%s:%ld)| %s %s\n", curr_time, from_file, from_line, from_fun, trc);
    //
    // We do not fflush() here - this is done either at the end of request (gg_shut()) or
    // when program crashes (gg_report_error())
    //
    pc->trace.in_trace = 0;
}

// 
// Get PID
//
gg_num gg_getpid ()
{
    GG_TRACE ("");
    return (gg_num) getpid();
}


//
// Get current time
// Output: outstr is the time string.
// out_str_len is time string buffer size
// If can't get time, output is empty string.
//
void gg_current_time (char *outstr, gg_num out_str_len)
{
    GG_TRACE("");
    time_t t;
    struct tm *tmp;

    // get current time zone - may be set by customer program!
    char *curr_time_zone = getenv("TZ");
#define restore_curr_time_zone if (curr_time_zone!=NULL && curr_time_zone[0]!=0) { putenv(curr_time_zone); tzset(); }

    // set time zone to local - we did this in main() first thing before customer code. We cast gg_get_tz()
    // into mutable char * because putenv does NOT modify its string. The result of gg_get_tz must NOT change by
    // callers.
    putenv((char*)gg_get_tz());
    tzset();

    t = time(NULL);
    tmp = localtime(&t);
    if (tmp == NULL)
    {
        // return to customer TZ
        restore_curr_time_zone
        outstr[0] = 0;
        return;
    }

    if (strftime(outstr, out_str_len, "%F-%H-%M-%S", tmp) == 0)
    {
        outstr[0] = 0;
    }
    // return to customer TZ
    restore_curr_time_zone
}

// 
// Both configuration and run-time information (context, debug, trace, etc.)
// This is really a program  context.
// EVERYTHING IN GG_CONFIG MUST BE MALLOC (NOT GG_MALLOC) AS IT IS USED ACROSS REQUESTS
//
gg_config *gg_pc;

//
// Get config and context data
//
gg_config *gg_alloc_config()
{
    // start /var/log/syslog so that fatal message have a place to go to
    openlog(gg_app_name, LOG_PERROR | LOG_PID, LOG_USER);

    // all of golf config (sub)structures must be zeroed-out - we rely on this when setting directories, user id and such
    gg_pc = (gg_config*)calloc (1, sizeof(gg_config));
    if (gg_pc == NULL)
    {
        GG_FATAL ("Cannot allocate program context");
    }
    gg_init_config (gg_pc);
    return gg_pc;
}


// 
// Initialize program context. This is called only once for the
// life of the process. pc is program context.
//
void gg_init_config(gg_config *pc)
{
    assert (pc);

    // pc->* are set to 0 or NULL - set here only what's not zero
    pc->app.max_upload_size = 5000000;

    gg_reset_config (pc);
}

// 
// Reset program context. This is called for each new web request
//
void gg_reset_config(gg_config *pc)
{
    assert (pc);
    // these need to reset with each request
    // DO NOT RESET debug structure - should stay as it is for all request during the life of process!
    pc->ctx.req = NULL;
    pc->ctx.gg_report_error_is_in_report = 0;
    pc->debug.trace_level = gg_is_trace; // reset tracing, as it is set to 1 during report-error
}

//
// UP TO HERE THERE CAN BE NO GG_MALLOC
//



//
// Find number of occurances in string 'str' of a substring 'find'
// If len_find isn't 0, then it's length of find.
// If case_sensitive is 1, then it's case sensitive, if 0 then not.
//
// Returns number of occurances of find in str
//
gg_num gg_count_substring (char *str, char *find, gg_num len_find, gg_num case_sensitive)
{
    GG_TRACE("");
    gg_num count = 0;
    if (find == NULL || find[0] == 0) return 0;
    // here not empty or NULL
    gg_num len;
    if (len_find != 0) len = len_find; else len = gg_mem_get_len(gg_mem_get_id(find));
    char *tmp = str;
    while((tmp = (case_sensitive == 1 ? strstr(tmp, find) : strcasestr(tmp, find))) != NULL)
    {
       count++;
       tmp += len;
    }
    return count;
}


// 
// Replace string 'find' with string 'subst' in string 'str', which is of size 'strsize' (total size in bytes of buffer 
// that is available). 'all' is 1 if all occurrance of 'find' are to be replaced.
// Output 'last' is the last byte position from which 'find' was searched for, but was not found (i.e.
// last byte position after which there is no more 'find' strings found).
// If case_sensitive is 1 then it's case sensitive, and 0 otherwise.
// Returns length of subst string, or -1 if not enough memory. If -1, whatever substitutions could have been
// made, were made, in which case use 'last' to know where we stopped.
//
gg_num gg_replace_string (char *str, gg_num strsize, char *find, char *subst, gg_num all, char **last, gg_num case_sensitive)
{
    GG_TRACE("");
    assert (str);
    assert (find);
    assert (subst);

    gg_num len = strlen (str);
    gg_num lenf = strlen (find);
    gg_num lens = strlen (subst);
    gg_num occ = 0;
    gg_num currsize = len + 1;

    char *curr = str;

    if (last != NULL) *last = NULL;
    while (1)
    {
        // find a string and move memory to kill the found 
        // string and install new one - minimal memmove
        // based on diff
        char *f = (case_sensitive==1 ? strstr (curr, find) : strcasestr (curr,find));
        if (f == NULL) break;
        currsize -= lenf;
        currsize += lens;

        if (currsize > strsize)
        {
            return -1;
        }

        memmove (f + lens, f + lenf, len - (f - str + lenf) + 1);
        memcpy (f, subst, lens);

        // update length
        len = len - lenf + lens;

        curr = f + lens; // next pos to look from 

        if (last != NULL) *last = curr; 
                        // for caller, where to look next, if in
                        // external loop, for 'all==0' only
        occ++;
        if (all == 0) break;
    }
    return len;
}


//
// Trim string on both left and right and place string back
// into the original buffer. Trims spaces, tabs, newlines.
// str is the string to be gg_trimmed.
// len is the length of string on the input, and it has new length
// on the output. 'len' MUST be the strlen(str) on input!
// if alloc is true, then str is alloc'd mem
// MUST NOT realloc as that would invalidate other references to this string
//
void gg_trim (char *str, gg_num *len, bool alloc)
{
    GG_TRACE("");
    assert (str);
    assert (len);
    
    gg_num i = 0;
    // clear leading spaces
    while (isspace (str[i])) i++;
    // move string back, overriding leading spaces
    if (i) memmove (str, str + i, *len - i + 1);
    // update length
    *len = *len -i;
    // start from the end
    i = *len - 1;
    // find the last non-space char
    while (i>=0 && isspace (str[i])) i--;
    // make the end of string there
    str[i + 1] = 0;
    // update length of string
    *len = i + 1;
    if (alloc) gg_mem_set_len (gg_mem_get_id(str), *len+1);
}

//
// Trim string on both left and right and return pointer to trimmed string- no movement of string
// Trims spaces, tabs, newlines.
// str is the string to be trimmed
// len is the length of string on the input, and it has new length
// on the output. 'len' MUST be the strlen(str) on input!
//
char *gg_trim_ptr (char *str, gg_num *len)
{
    GG_TRACE("");
    assert (str);
    assert (len);

    char *res = str;
    
    gg_num s = 0;
    // clear leading spaces
    while (isspace (str[s])) s++;
    res = str + s;

    // start from the end
    gg_num e = *len - 1;

    // update length
    *len = *len -s;

    // find the last non-space char
    gg_num j = 0;
    while (e>=s && isspace (str[e])) { e--; j++;}
    // make the end of string there
    str[e + 1] = 0;

    // update length of string
    *len = *len - j;
    return res;
}

// 
// Returns GG_DIR if 'dir' is a directory,
// GG_FILE if it's file, GG_ERR_FAILED if can't stat
//
gg_num gg_file_type (char *dir)
{
    GG_TRACE("");
    struct stat sb;
    if (stat(dir, &sb) == 0)
    {
        if (S_ISDIR(sb.st_mode)) return GG_DIR; else return GG_FILE;
    }
    else 
    {
        GG_ERR;
        return GG_ERR_FAILED;
    }
}


//
// Get Position from file FILE* f 
// Returns GG_OKAY if okay, GG_ERR_POSITION if cannot do it
//
gg_num gg_get_file_pos(FILE *f, gg_num *pos)
{
    GG_TRACE("");
    long p = ftell (f);
    if (p == -1) {
        GG_ERR;
        GG_TRACE("Cannot get position in file, errno [%d]", errno);
        return GG_ERR_POSITION;
    }
    *pos = p;
    return GG_OKAY;
}

//
// Position file FILE* f to pos
// Returns GG_OKAY if okay, GG_ERR_POSITION if cannot do it
// GG_ERR_OPEN if file not opened
//
gg_num gg_set_file_pos(FILE *f, gg_num pos)
{
    GG_TRACE("");
    if (f==NULL)
    {
        GG_ERR;
        return GG_ERR_OPEN;
    }
    if (fseek (f,pos,SEEK_SET) != 0) {
        GG_ERR;
        GG_TRACE("Cannot position in file to [%ld], errno [%d]", pos, errno);
        return GG_ERR_POSITION;
    }
    return GG_OKAY;
}

//
// Get size of file from open FILE*
// f is FILE *
// Returns size of the file
//
gg_num gg_get_open_file_size(FILE *f)
{
    GG_TRACE("");
    long ppos = ftell(f);
    fseek(f, 0L, SEEK_END);
    long size=ftell(f);
    fseek(f, ppos, SEEK_SET); // position to where we were before
    return (gg_num)size;
}

//
// Get size of file
// fn is file name.
// Returns size of the file, or -1 if file cannot be stat'
//
gg_num gg_get_file_size(char *fn)
{
    GG_TRACE("");
    struct stat st;
    if (stat(fn, &st) != 0) 
    {
        GG_ERR;
        return GG_ERR_FAILED;
    }
    return (gg_num)(st.st_size);
}


// 
// Checks if input parameter name (in URL) is valid for golf.
// Valid names are considered to have only alphanumeric characters and
// underscores/hyphens, and must start with an alphabet character.
// If hyphen is true, then it's allowed, otherwise it's not. If hyphen is allowed and conv_hyphen is true, then each hyphen
// is converted to underscore.
// Returns 1 if name is valid, 0 if not.
//
gg_num gg_is_valid_param_name (char *name, bool hyphen, bool conv_hyphen)
{
    GG_TRACE ("");
    assert (name);

    gg_num i = 1; // we already check the first byte before entering while loop
    if (!isalpha(name[0])) return 0;
    while (name[i] != 0)
    {
        if (isalnum(name[i])) { i++; continue;}
        if (name[i] == '_') { i++; continue;}
        if (name[i] == '-' && hyphen) 
        { 
            if (conv_hyphen) name[i] = '_'; 
            i++; 
            continue; 
        }
        return 0;
    }
    return 1;
}


// 
// Initialize sequential list storage data
// fdata is storage data variable.
// Data can be stored in order and retrieved in the same order and rewound
// any number of times. Once used, must be purged.
//
void gg_store_init (gg_fifo **fdata_ptr)
{
    GG_TRACE ("");
    *fdata_ptr = (gg_fifo*)gg_malloc (sizeof(gg_fifo));
    gg_fifo *fdata = *fdata_ptr;

    fdata->num_of = 0;
    fdata->last_ptr = NULL;
    fdata->retrieve_ptr = NULL;
    fdata->first_ptr = NULL;
}

// 
// Store name/value pair, with 'name' being the name and 'data' being the value
// in storage data 'fdata'. Both strings are stored in the list as pointers.
// This is for lifo variety.
//
void gg_store_l (gg_fifo *fdata, char *name, void *data)
{
    GG_TRACE ("");
    gg_fifo_item *np = gg_malloc (sizeof (gg_fifo_item));
    // No need to check if np->name/data equal to name/data because np is just created here, has nothing to begin with
    //
    if (gg_optmem) gg_mem_add_ref (1, NULL, data); 
    gg_mem_set_process (data, false);
    np->data = data;
    //
    if (gg_optmem) gg_mem_add_ref (1, NULL, name); 
    gg_mem_set_process (name, false);
    np->name = name;
    //
    np->next = NULL;
    if (fdata->num_of == 0)
    {
        fdata->first_ptr = np;
        fdata->last_ptr = np;
    }
    else
    {
        np->next = fdata->first_ptr;
        fdata->first_ptr = np;
    }
    fdata->retrieve_ptr = np;
    fdata->num_of++;
}

// 
// 
// Store name/value pair, with 'name' being the name and 'data' being the value
// in storage data 'fdata'. Both strings are stored in the list as pointers.
// This is for fifo variety.
//
void gg_store (gg_fifo *fdata, char *name, void *data)
{
    GG_TRACE ("");
    gg_fifo_item *np = gg_malloc (sizeof (gg_fifo_item));
    // No need to check if np->name/data equal to name/data because np is just created here, has nothing to begin with
    //
    if (gg_optmem) gg_mem_add_ref(1, NULL, data); 
    gg_mem_set_process (data, false);
    np->data = data;
    //
    if (gg_optmem) gg_mem_add_ref(1, NULL, name);
    gg_mem_set_process (name, false);
    np->name = name;
    //
    np->next = NULL;
    if (fdata->num_of == 0)
    {
        fdata->first_ptr = np;
        fdata->last_ptr = np;
        fdata->retrieve_ptr = np;
    }
    else
    {
        fdata->last_ptr->next = np;
        fdata->last_ptr = np;
    }
    fdata->num_of++;
}

// 
// Retrieve name/value pair from storage data 'fdata', with 'name' being the
// name and 'data' being the value. The name/data are simply assigned pointer
// values. Initially, this starts with fist name/value pair put in.
//
gg_num gg_retrieve (gg_fifo *fdata, gg_num is_def_n, char **name, gg_num is_def_d, void **data)
{
    GG_TRACE ("");
    assert (fdata != NULL);

    if (fdata->retrieve_ptr == NULL)
    {
        return GG_ERR_EXIST;
    }
    if (data != NULL) 
    {
        if (gg_optmem) 
        {
            if (*data != fdata->retrieve_ptr->data) 
            {
                gg_mem_add_ref(is_def_d, *data, fdata->retrieve_ptr->data); 
                *data = fdata->retrieve_ptr->data;
            }
        }
        else
        {
            *data = fdata->retrieve_ptr->data;
        }
    }
    if (name != NULL) 
    {
        if (gg_optmem) 
        {
            if (*name != fdata->retrieve_ptr->name) 
            {
                gg_mem_add_ref(is_def_n, *name, fdata->retrieve_ptr->name); 
                *name = fdata->retrieve_ptr->name;
            }
        }
        else
        {
            *name = fdata->retrieve_ptr->name;
        }
    }
    fdata->retrieve_ptr = fdata->retrieve_ptr->next;
    return GG_OKAY;
}

// 
// Rewind name/value pair in storage 'fdata', so that next gg_retrieve()
// starts from the items first put in.
void gg_rewind (gg_fifo *fdata)
{
    GG_TRACE ("");
    assert (fdata != NULL);
    fdata->retrieve_ptr = fdata->first_ptr;
}

//
// Delete the very first name/value pair in fifo 'fdata', all the way up to the current 
// element (which is one after last read, or NULL if already retrieved the last one), excluding.
//
void gg_fifo_delete (gg_fifo *fdata)
{
    GG_TRACE ("");
    if (fdata->first_ptr == NULL) return;
    
    while (fdata->first_ptr != fdata->retrieve_ptr)
    {
        gg_fifo_item *temp = fdata->first_ptr;
        if (temp == fdata->last_ptr) fdata->last_ptr = fdata->first_ptr = fdata->retrieve_ptr = NULL;
        else fdata->first_ptr = temp->next;
        gg_free (temp->data);
        gg_free (temp->name);
        gg_free (temp);
        fdata->num_of--;
    }
}


// 
// Purge all data from storage 'fdata' and initialize for another use.
//
void gg_purge (gg_fifo **fdata_p)
{
    GG_TRACE ("");
    gg_fifo *fdata = *fdata_p;
    fdata->retrieve_ptr = NULL; // delete up to the end, i.e. all of them
    gg_fifo_delete (fdata);
    gg_free(fdata);
    gg_store_init (fdata_p); // okay fdata_p since fdata_p!=NULL
}

// 
// Initialize linked list storage data
// fdata is storage data variable.
// process is true if this list is process-scoped
//
void gg_list_init (gg_list **fdata_ptr, bool process)
{
    GG_TRACE ("");
    *fdata_ptr = (gg_list*)gg_malloc (sizeof(gg_list));
    gg_list *fdata = *fdata_ptr;

    fdata->num_of = 0;
    fdata->last = NULL;
    fdata->curr = NULL;
    fdata->first = NULL;
    fdata->process = process;
}


// 
// Store name/value pair, with 'name' being the name and 'data' being the value
// in linked list storage data 'fdata'. Both strings are stored in the list as pointers.
// New element stored at current position (at the very end if current is NULL)
//
void gg_list_store (gg_list *fdata, char *name, void *data, bool append)
{
    GG_TRACE ("");
    gg_list_item *np = gg_malloc (sizeof (gg_list_item));
    // No need to check if np->name/data equal to name/data because np is just created here, has nothing to begin with
    //
    if (gg_optmem) gg_mem_add_ref(1, NULL, data); 
    gg_mem_set_process (data, false);
    np->data = data;
    //
    if (gg_optmem) gg_mem_add_ref(1, NULL, name); 
    gg_mem_set_process (name, false);
    np->name = name;
    //
    np->next = NULL;
    np->prev = NULL;
    if (fdata->num_of == 0)
    {
        fdata->first = np;
        fdata->last = np;
        fdata->curr = np;
    }
    else
    {
        if (append) fdata->curr = NULL;
        if (fdata->curr == NULL)
        {
            fdata->last->next = np;
            np->prev = fdata->last;
            fdata->last = np;
            fdata->curr = np;
        }
        else if (fdata->curr == fdata->first)
        {
            np->next = fdata->first;
            fdata->first->prev = np;
            fdata->first = np;
            fdata->curr = np;
        }
        else
        {
            fdata->curr->prev->next = np;
            np->prev = fdata->curr->prev;
            np->next = fdata->curr;
            fdata->curr->prev = np;
            fdata->curr = np;
        }
    }
    fdata->num_of++;
}

// 
// Retrieve name/value pair from linked list data 'fdata', with 'name' being the
// name and 'data' being the value. The name/data are simply assigned pointer
// values. Element retrieved is the current one.
//
void gg_list_retrieve (gg_list *fdata, char **name, void **data)
{
    GG_TRACE ("");

    if (fdata->curr == NULL)
    {
        return;
    }
    if (data != NULL) *data = fdata->curr->data;
    if (name != NULL) *name = fdata->curr->name;
}

//
// Delete current element in linked list 'fdata'
// Inline so purge doesn't waste time
//
inline gg_num gg_list_delete (gg_list *fdata)
{
    GG_TRACE ("");
    gg_list_item *temp = fdata->curr;
    if (temp == NULL) return GG_ERR_EXIST;
    gg_free (temp->data);
    gg_free (temp->name);
    if (temp->next != NULL) {
        temp->next->prev = temp->prev; 
        fdata->curr = temp->next;
    }
    else 
    {
        fdata->last = temp->prev;
        fdata->curr = temp->prev;
    }
    if (temp->prev != NULL) temp->prev->next = temp->next; else 
    {
        fdata->first = temp->next;
        fdata->curr = temp->next;
    }
    gg_free (temp);
    fdata->num_of--;
    return GG_OKAY;
}


// 
// Purge all data from linked list 'fdata_p' and initialize for another use.
//
void gg_list_purge (gg_list **fdata_p)
{
    GG_TRACE ("");
    gg_list *fdata = *fdata_p;
    bool p = (*fdata_p)->process;
    while (fdata->num_of > 0)
    {
        fdata->curr = fdata->first;
        gg_list_delete (fdata);
    }
    gg_free(fdata);
    gg_list_init (fdata_p, p); // okay fdata_p since fdata_p!=NULL
}


// 
// The same as strncpy() except that zero byte is placed at the end and it returns
// the length of the dest string.
//
gg_num gg_strncpy(char *dest, char *src, gg_num max_len)
{
    GG_TRACE("");
    gg_num len = strlen (src);
    if (len < max_len) 
    {
        memcpy (dest, src, len+1 );
        return len;
    }
    else
    {
        memcpy (dest, src, max_len-1 );
        dest[max_len - 1] = 0;
        return max_len - 1;
    }
}

//
// Initialize a string that is allocated on the heap, like malloc with value of string s
//
char *gg_init_string(char *s)
{
    GG_TRACE("");
    if (s == NULL) return NULL;
    gg_num l = strlen (s);
    char *res = gg_malloc (l+1);
    memcpy (res, s, l+1);
    return res;
}

// 
// Get timezone that's local to this server.
// Returns string in the format TZ=<timezone>, eg. TZ=MST
//
char * gg_get_tz ()
{
    //
    // This static usage is okay because the timezone is the SAME for all modules that could
    // run in this process. We can set timezone once for any of the modules, and the rest can
    // use the timezone just fine.
    //
    static gg_num is_tz = 0;
    static char tz[200]; 

    // TZ variable isn't set by default, so we cannot count on it. Functions
    // that operate on time do CHECK if it's set, but unless we set it, it
    // WONT be set
    if (is_tz == 0)
    {
        is_tz = 1;

        // get localtime zone 
        time_t t = time(NULL);
        struct tm *tm = localtime(&t);
        snprintf (tz, sizeof(tz), "TZ=%s", tm->tm_zone);

    }
    return tz;
}

// 
// Read entire file with path 'name' and store file contents in output 'data', unless
// pos/len is specified (len is <>0 or pos<>0), in which case read len (default is 0, the rest of the file) bytes 
// from position pos (default is 0, the beginning). Returns -1 if cannot open file, -2 if cannot read, 
// -3 cannot position, -4 if nothing to read (pos/len bad), or size of data read.
// Note: zero byte is place after the end, so if return value is 10, it means
// there are 11 bytes, with zero at the end, regardless of whether the data is a
// string or not.
// If there is not enough memory, gg_malloc will error out.
//
gg_num gg_read_file (char *name, char **data, gg_num pos, gg_num len)
{
    GG_TRACE ("");

    if (pos < 0) {GG_ERR0; return GG_ERR_POSITION;} // len is negative
    if (len < 0) {GG_ERR0; return GG_ERR_READ;} // pos is negative

    FILE *f = gg_fopen (name, "r");
    if (f == NULL)
    {
        //gg_fopen sets GG_ERR
        return GG_ERR_OPEN;
    }
    gg_num sz;
    if (len > 0) sz = len; else sz = gg_get_open_file_size(f) - pos;
    if (sz < 0) { GG_ERR0; return GG_ERR_POSITION;} // pos is beyond size
    if (sz == 0) 
    {
        *data = GG_EMPTY_STRING;
        return 0; // nothing to read just an empty string
    }
    if (pos > 0)
    {
        if (fseek (f,pos,SEEK_SET) != 0) { 
            GG_ERR;
            GG_TRACE("File [%s], cannot position to [%ld], errno [%d]", name, pos, errno);
            fclose (f);
            *data = GG_EMPTY_STRING;
            return GG_ERR_POSITION;
        }
    }
    *data = gg_malloc (sz + 1);
    gg_num id = gg_mem_get_id (*data);
    gg_num rd;
    rd = fread (*data, 1, sz, f);
    if (ferror (f))
    {
        GG_ERR;
        gg_free (*data);
        fclose(f);
        *data = GG_EMPTY_STRING;
        return GG_ERR_READ;
    }
    (*data)[rd] = 0;
    gg_mem_set_len (id, rd+1);
    fclose(f);
    return rd;
}


// 
// Read file with FILE* f and store file contents in output 'data'
// len (default is 0, the rest of the file) is the number of bytes to read
// from position pos, if not specified then from current position . Returns -1 if cannot open file, -2 if cannot read, 
// -3 cannot position, -4 if nothing to read (pos/len bad), or size of data read.
// Note: zero byte is place after the end, so if return value is 10, it means
// there are 11 bytes, with zero at the end, regardless of whether the data is a
// string or not.
// If there is not enough memory, gg_malloc will error out.
// ispos is true if position is given
//
gg_num gg_read_file_id (FILE *f, char **data, gg_num pos, gg_num len, bool ispos)
{
    GG_TRACE ("");

    if (ispos && pos < 0) {GG_ERR0; return GG_ERR_POSITION;} // len is negative
    if (len < 0) {GG_ERR0; return GG_ERR_READ;} // len is negative

    if (f == NULL)
    {
        GG_ERR;
        return GG_ERR_OPEN;
    }
    gg_num sz;
    if (len > 0) sz = len; else
    {
        // If position not specified, find the current position, so that "reading to the 
        // end of the file" is correct exactly.
        if (!ispos) {
            if (gg_get_file_pos (f, &pos) != GG_OKAY) { return GG_ERR_POSITION;}
        }
        sz = gg_get_open_file_size(f) - pos;
    }
    if (sz < 0) {GG_ERR0; return GG_ERR_POSITION;} // pos is beyond size
    if (sz == 0) 
    {
        *data = GG_EMPTY_STRING;
        return 0; // nothing to read just an empty string
    }
    if (ispos)
    {
        if (fseek (f,pos,SEEK_SET) != 0) { 
            GG_ERR;
            GG_TRACE("File cannot position to [%ld], errno [%d]", pos, errno);
            return GG_ERR_POSITION;
        }
    }
    *data = gg_malloc (sz + 1);
    gg_num rd;
    rd = fread (*data, 1, sz, f);
    if (ferror (f))
    {
        GG_ERR;
        return GG_ERR_READ;
    }
    (*data)[rd] = 0;
    return rd;
}


// 
// Encode string v, producing output result res. enc_type is GG_WEB (for
// web encoding) or GG_URL (for url encoding). Pointer to pointer 'res' is allocated
// with sufficient memory in the worst case scenario
// vlen is the length of v, -1 if strlen(), otherwise length
// alloc is true if v is golf alloc'd mem
// Returns length of an encoded string.
//
gg_num gg_encode (gg_num enc_type, char *v, gg_num vlen, char **res, bool alloc)
{
    GG_TRACE("");
    gg_num mlen;
    gg_num id;
    if (alloc) id = gg_mem_get_id(v);
    if (vlen < 0)
    {
        if (alloc) mlen = gg_mem_get_len(id); // this is used by Golf code written by user
        else mlen = strlen (v); // this is when used internally by Golf
    }
    else
    {
        if (alloc)
        {
            if (vlen > (mlen=gg_mem_get_len(id))) gg_report_error ("Cannot encode [%ld] bytes of a string with length [%ld]", vlen, mlen);
            mlen = vlen;
        } else mlen = vlen; // this is internal use by golf
    }
    return gg_encode_base (enc_type, v, mlen , res, 1);
}

//
// Compare types, t1 and t2, if they are compatible in storage and type
// true if equal or false returned.
//
bool cmp_type (gg_num t1, gg_num t2)
{
    if ((t1 == GG_DEFSTRING || t1 == GG_DEFSTRINGSTATIC) && (t2 == GG_DEFSTRING || t2 == GG_DEFSTRINGSTATIC)) return true;
    else if ((t1 == GG_DEFNUMBER || t1 == GG_DEFNUMBERSTATIC) && (t2 == GG_DEFNUMBER || t2 == GG_DEFNUMBERSTATIC)) return true;
    else if ((t1 == GG_DEFBOOL || t1 == GG_DEFBOOLSTATIC) && (t2 == GG_DEFBOOL || t2 == GG_DEFBOOLSTATIC)) return true;
    else if ((t1 == GG_DEFHASH || t1 == GG_DEFHASHSTATIC) && (t2 == GG_DEFHASH || t2 == GG_DEFHASHSTATIC)) return true;
    else if ((t1 == GG_DEFTREE || t1 == GG_DEFTREESTATIC) && (t2 == GG_DEFTREE || t2 == GG_DEFTREESTATIC)) return true;
    else if ((t1 == GG_DEFLIST || t1 == GG_DEFLISTSTATIC) && (t2 == GG_DEFLIST || t2 == GG_DEFLISTSTATIC)) return true;
    else if ((t1 == GG_DEFJSON ) && (t2 == GG_DEFJSON )) return true;
    else if (t1 == t2) return true;
    else return false;
}




// 
// Encode string v, producing output result res. enc_type is GG_WEB (for
// web encoding) or GG_URL (for url encoding). Pointer to pointer 'res' is allocated
// with sufficient memory in the worst case scenario (if allocate_new is 1), or if it is 0, it MUST
// have enough space to hold GG_MAX_ENC_BLOWUP(vLen) in it) and the caller MUST handle any setting of memory length!
// vLen is the string length of v.
// Null character added.
// String v can be smaller than length vLen, vLen is the maximum number of characters encoded.
// Returns length of an encoded string.
//
gg_num gg_encode_base (gg_num enc_type, char *v, gg_num vLen, char **res, gg_num allocate_new)
{
    GG_TRACE("");
    assert (res != NULL);
    assert (v != NULL);


    if (allocate_new==1)
    {
        *res = (char*)gg_malloc (GG_MAX_ENC_BLOWUP(vLen)); // worst case, see below for usage
    }
    gg_num i;
    gg_num j = 0;
    if (enc_type == GG_WEB)
    {
        for (i = 0; i < vLen; i ++)
        {
            switch (v[i])
            {
                case '&': memcpy (*res + j, "&amp;", 5); j+=5; break;
                case '"': memcpy (*res + j, "&quot;", 6); j+=6; break;
                case '\'': memcpy (*res + j, "&apos;", 6); j+=6; break;
                case '<': memcpy (*res + j, "&lt;", 4); j+=4; break;
                case '>': memcpy (*res + j, "&gt;", 4); j+=4; break;
                case 0: break;
                default: (*res)[j++] = v[i]; break;
            }
        }
    }
    else if (enc_type == GG_URL)
    {
        for (i = 0; i < vLen; i ++)
        {
            if (isalnum(v[i]) || v[i] == '-' || v[i] == '.' || v[i] == '_' || v[i] == '~') 
            {
                (*res)[j++] = v[i];
            }
            else
            {
                int h = v[i]/16;
                int l = v[i]%16;
                (*res)[j] = '%';
                (*res)[j+1] = h>=10 ? 'A'+h-10 : '0'+h;
                (*res)[j+2] = l>=10 ? 'A'+l-10 : '0'+l;
                j += 3;
            }
        }
    }
    else 
    {
        assert (1==2);
    }
    (*res)[j] = 0;
    if (allocate_new==1)
    {
        *res = (char*)gg_realloc (gg_mem_get_id(*res), j+1); // reduce memory usage
        gg_mem_set_len (gg_mem_get_id(*res), j+1);
    }
    return j;
}

//
// Position to pos position (if ispos is 1), and write content of content_len length (if 0, treat content
// as string and calculate length). Write to file 'f'.
// Return # of bytes written or error. The caller can close the file if needed, it's not closed here.
//
gg_num gg_core_write_file(FILE *f, gg_num content_len, char *content, char ispos, gg_num pos)
{
    gg_num id = gg_mem_get_id(content);
    if (content_len == 0) content_len = gg_mem_get_len(id);
    else if (content_len > gg_mem_get_len(id)) gg_report_error ("Memory used to write file is of length [%ld] but only [%ld] allocated", content_len, gg_mem_get_len(id));

    if (ispos == 1)  // positioning beyond the end of file is allowed. The gap will be filled with \0
    {
        if (fseek (f,pos,SEEK_SET) != 0) {
            GG_ERR;
            GG_TRACE("File writing, cannot position to [%ld], errno [%d]", pos, errno);
            return GG_ERR_POSITION;
        }
    }
    if (fwrite(content, (size_t)content_len, 1, f) != 1)
    {
        GG_ERR;
        GG_TRACE("Cannot write file, errno [%d]", errno);
        return GG_ERR_WRITE;
    }
    return content_len;
}

//
// Write file 'file_name' from data 'content' of length 'content_len'. If 'append' is 1,
// then this is appended to the file, otherwise, file is overwritten (or created if it didn't
// exist). If 'content_len' is 0, then write the whole string 'content' (must be 0 terminated).
// If pos is positive or 0 (and ispos==1), then position there in the file and then write - in this case file is not overwritten.
// ispos is 0 for no position, 1 otherwise.
// Cannot have append==1 and ispos==1. Golf checks and errors out if append and position are used before calling this.
// Returns GG_ERR_OPEN is cannot open file, GG_ERR_WRITE if cannot write, GG_ERR_POSITION if cannot
// position, or number of bytes written, which is always the number of bytes requested (otherwise it's an error).
// Maximum size of file is in 0..maxlonglong range.
//
gg_num gg_write_file (char *file_name, char *content, gg_num content_len, char append, gg_num pos, char ispos)
{
    GG_TRACE("");

    if (ispos ==1 && pos < 0)  // any position that is not 0 or positive is an error
    {
        GG_ERR0;
        return GG_ERR_POSITION;
    }

    FILE *f = NULL;
    // ispos is 0 if no positioning
    if (ispos == 0) f=gg_fopen (file_name,  append==1 ? "a+" : "w+"); // a+ for append, and truncate if neither append nor position
    else f=gg_fopen (file_name,  "r+"); // need read+write for positioning

    if (f==NULL)
    {
        //gg_fopen sets GG_ERR
        return GG_ERR_OPEN;
    }
    gg_num retw = gg_core_write_file(f, content_len, content, ispos, pos);
    fclose(f);
    return retw;
}

//
// Write file FILE *f(open) from data 'content' of length 'content_len'. If 'append' is 1,
// then this is appended to the file, otherwise, data is written at current position (if ispos is 0).
// If 'content_len' is 0, then write the whole string 'content' (must be 0 terminated).
// If pos is positive or 0 (and ispos 1), then position there in the file and then write - 
// ispos is 0 for no position, 1 otherwise.
// Cannot have append==1 and ispos==1. Golf checks and errors out if append and position are used before calling this.
// Returns GG_ERR_OPEN is cannot open file, GG_ERR_WRITE if cannot write, GG_ERR_POSITION if cannot
// position, or number of bytes written, which is always the number of bytes requested (otherwise it's an error).
// Maximum size of file is in 0..maxlonglong range.
//
gg_num gg_write_file_id (FILE *f, char *content, gg_num content_len, char append, gg_num pos, char ispos)
{
    GG_TRACE("");

    if (f==NULL)
    {
        GG_ERR;
        return GG_ERR_OPEN;
    }
    if (ispos ==1 && pos < 0)  // any position that is not 0 or positive is an error
    {
        GG_ERR0;
        return GG_ERR_POSITION;
    }

    if (append == 1) 
    {
        gg_num ef =  gg_get_open_file_size(f);
        if (fseek (f,ef,SEEK_SET) != 0) {
            GG_ERR;
            GG_TRACE("File writing, cannot position to [%ld], errno [%d]", ef, errno);
            return GG_ERR_POSITION;
        }
    }

    return gg_core_write_file(f, content_len, content, ispos, pos);
}


//
// Add file to Golf's list of files to close. f is the pointer to FILE*.
// When program closes file, it must set *f = NULL in order not to be double freed.
// Returns memory index for FILE* in Golf's mem system.
//
gg_num gg_reg_file(FILE **f)
{
    GG_TRACE("");
    gg_num ind = gg_add_mem (f); // add pointer to file pointer so it can be closed if programmer doesn't do it
                                     // thus preventing file descriptor leakage
    // no need to setup head for memory, it's not used for FILE
    gg_mem_set_status (ind, GG_MEM_FILE);
    return ind;
}


//
// Close a file. 
// Returns EOF if can't close, otherwise 0.
//
int gg_fclose (FILE *f)
{
    GG_TRACE ("");
    if (f == NULL) { GG_ERR0; return GG_ERR_CLOSE; }
    int res= fclose (f);
    if (res == EOF) {
        GG_ERR;
        return GG_ERR_CLOSE;
    }
    return GG_OKAY;
}

//
// Open a file. If open for writing, set permissions to 0770
// so it's read/write for owner/group
// Returns NULL if can't open, file pointer if it can
//
FILE *gg_fopen (char *file_name, char *mode)
{
    GG_TRACE ("");
    FILE *f = fopen (file_name, mode);
    // check if file opened
    if (f != NULL)
    {
        // if opened, check if mode has a(append) or w(write)
        // in which case, the file may be created, so change to 
        // -rw-rw----
        if (strchr (mode, 'a') != NULL || strchr (mode, 'w') != NULL)
        {
            fchmod (fileno (f), 0660);
        }
    } 
    else
    {
        GG_ERR;
    }
    return f;
}



// Return name(id) of operating system
//
char * gg_os_name() {return GG_OS_NAME;}


//
// Return version of operating system
//
char * gg_os_version() {return GG_OS_VERSION;}


//
// Find a keyword 'find' in str, making sure the keyword is NOT quoted or with ()
// () is used to group an expression, which may contain keyword. Quoted string may also contain it.
// Returns the pointer to keyword in str, or NULL if not there
// Note: since each keyword has a space before it in the original Golf statement
// we look for either 1) the space or 2) 0 character that may have been put there - that
// must be the case, unless has_spaces is 0, for example a=b - keyword = does not need space before or after.
// If find is "", then we're looking for end-of-line (i.e. null character). The purpose of this is for
// recog_statement to scan through the rest of line and find unterminated string and unbalanced ()
// If paren is 0, then () is not looked at, only quoting matters for finding keyword (this is for break-string)
// Since a keyword may be contained in another (such as url and url-path in get-req), we also check that keyword
// is followed by a space or a null (unless has_spaces is 0).
//
char *gg_find_keyword0(char *str, char *find, gg_num has_spaces, gg_num paren)
{
    char *beg = str;
    char *f;
    while (1)
    {
        if (find[0] == 0) f = beg + strlen (beg); else f = strstr (beg, find);
        if (f != NULL)
        {
            // check that previous char is empty or space
            // unless we found keyword as the very first char
            // this does not apply if we look for end-of-string
            if (f != beg && find[0] != 0)
            {
                if (has_spaces == 1 && *(f-1) != 0 && *(f-1) != ' ')
                {
                    beg=f+1;
                    continue;
                }
            }
            if (has_spaces == 1 && find[0] != 0) // check that after keyword is 0 or empty space
                                                 // this is not an issue if has_spaces is 0, where there may be no space 
                                                 // before or after
            {
                int l = strlen (find);
                if (f[l-1] != ' ' && f[l] != 0 && f[l] != ' ') 
                {
                    beg =f+1;
                    continue;
                }
            }
            // now go back from found keyword and see how many unescaped quotes there are
            // and how many left and right parenthesis outside a string there are
            // the keyword must be outside the quotes and a string
            gg_num quotes = 0;
            gg_num left_par = 0;
            gg_num right_par = 0;
            char *go_forth = str; // start from the beginning of text
            gg_num within_string = 0;
            while (go_forth != f) // move forth until hitting the found keyword
            {
                if (*go_forth == '"') 
                {
                    // find out if we're in the string (possibly within an expression)
                    if (go_forth == str) quotes++; // at the very beginning (so no char behind)
                    else if (*(go_forth-1)!='\\') quotes++;
                }
                if (quotes % 2 == 1) within_string = 1; else within_string = 0;
                // if paren == 0, then left_par and right_par remain both 0, so technically balanced
                if (paren == 1 && *go_forth == '(' && within_string == 0) left_par++;
                if (paren == 1 && *go_forth == ')' && within_string == 0) right_par++;
                go_forth++;
            }
            if (find[0] == 0)
            {
                // if looking for end-of-string, there is only one of those, so do NOT continue in while loop
                // if quoted or () not balanced. Rather, error out. Otherwise, return pointer to end-of-string.
                if (within_string == 1)
                {
                    _gg_report_error( "Unterminated string");
                }
                if (left_par != right_par)
                {
                    _gg_report_error( "Unbalanced left and right parenthesis () in statement");
                }
                return (char*)f;
            }
            // check if prior to found keyword, all strings are closed and () are balanced, so it means it's outside
            if (within_string == 0) 
            {
                if (left_par == right_par) 
                {
                    break; 
                }
                else beg=f+1;
            }
            else beg=f+1;
        } else break;
    }
    return (char*)f;
}



//
// Decorate a hierarchical request path.
// Substitute / with __  and - with _. Leading / (if any) is ignored and omitted from result.
// Path must end with path segment with = in it, or by ending string.
// reqname is the fixed buffer that will hold the decorated path, reqname_len is its size,
// p is the path to convert, p_len is its string length. Path is not checked for validity,
// Each path segment must be a valid C identifier, this must be assured by the caller.
// path must be trimmed by the caller
// if path is found, p is advanced to be on its last / so that parsing may continue (if there's /), or otherwise is NULL
// The caller must advance p by 1 to continue, or do nothing if it's NULL
// Returns 1 if hierarchical path found, 0 if not
//
char gg_decorate_path (char *reqname, gg_num reqname_len, char **p, gg_num p_len)
{
    GG_TRACE("");

    // Even if *p doesn't have any / in it (or just as a first char), it may still have hyphens in it that need converting
    char *use_p;
    if (**p != '/') use_p = *p; else { use_p = *p + 1; p_len--;}

    // Going forward, there's always leading /

    char *ereq = strchr (use_p, '='); // first =
    bool noparam = false;
    if (ereq == NULL) 
    {
        // no =, so the whole use_p is a request path
        ereq = use_p + p_len;
        noparam = true;
    }
    else
    {
        while (ereq != use_p && *ereq != '/') ereq--;
        if (ereq == use_p) return 0; // this is /...= instead of /.../..=, i.e no request path, immediately a parameter
    }


    // convert request name given as a path into __ substituting for / and _ for -
    // so add-customer/america/subscription would be add_customer__america__subscription
    char *pbeg = use_p; // begin to read req name from path segments which doesn't have / anymore in the front
    char *pres = reqname; // result into reqname
    gg_num i = 0; // begin filling request
    // exit loop if path segment for request ends, or if too long
    while (pbeg != ereq && i <= reqname_len - 3) // since we +2 for __ below, plus for null
    {
        if (*pbeg == '/')  // subst / for __
        {
            pres[i++]='_'; 
            pres[i++]='_';
        } 
        else if (*pbeg == '-')
        { 
            pres[i++]='_';   // subst - for _
        }
        else { pres[i++]=*pbeg; }  // copy otherwise
        pbeg++; // move path segment forward
    }
    pres[i] = 0; // end result
    if (noparam) *p = NULL; else *p = ereq; // either the last / or the last char at null byte if no /
    return 1;
}


// 
// Convert str to resulting number (return value) in base 'base', with *st being status (VV_OKAY if okay).
// st can be NULL. If base is 0, automatic base discovery (see docs).
//
inline gg_num gg_str2num (char *str, int base, gg_num *st)
{
    char *numend;
    gg_num val;
    if (base != 0 && (base < 2 || base > 36)) { if (st) *st = GG_ERR_INVALID; return 0;}

    errno = 0;   // errno is used in strtoll
    val = strtoll(str, &numend, base);


    if (errno != 0) 
    {
       // ERANGE, overflow or underflow, or base is incorrect  (EINVAL)
       if (st && errno == ERANGE) *st = GG_ERR_OVERFLOW;
       else if (st && errno == EINVAL) *st = GG_ERR_INVALID;
       else if (st) *st = GG_ERR_INVALID; // should not really happen, not sure what error is
       return 0;
    }

    if (numend == str) 
    {
       // Empty or no digits seen
       if (st) *st = GG_ERR_EXIST;
       return 0;
    }

    if (*numend != 0)
    {
       //There were trailing invalid digits, we can still extract num
       if (st) *st = GG_ERR_TOO_MANY;
       return (gg_num)val;
    }
    if (st) *st = GG_OKAY;
    return (gg_num)val;
}


//
// get basename of file 'path' as gg_string (allocated). Path can be of a file or directory.
// returns basename
//
char *gg_basename (char *path)
{   
    GG_TRACE("");
    char *pcopy = gg_strdup(path);
    char *res = gg_strdup(basename (pcopy)); // res has exact length set with strdup
    gg_free (pcopy);     
    return res;
}




//
// Return type name based on type
//
char *typename (gg_num type)
{
    if (type == GG_DEFSTRING) return GG_KEY_T_STRING;
    else if (type == GG_DEFSTRINGSTATIC) return "process-scope " GG_KEY_T_STRING;
    else if (type == GG_DEFNUMBER) return GG_KEY_T_NUMBER;
    else if (type == GG_DEFNUMBERSTATIC) return "process-scope " GG_KEY_T_NUMBER;
    else if (type == GG_DEFMSG) return GG_KEY_T_MESSAGE;
    else if (type == GG_DEFBROKEN) return GG_KEY_T_SPLITSTRING;
    else if (type == GG_DEFHASH) return GG_KEY_T_HASH;
    else if (type == GG_DEFHASHSTATIC) return "process-scope " GG_KEY_T_HASH;
    else if (type == GG_DEFJSON) return GG_KEY_T_JSON;
    else if (type == GG_DEFTREE) return GG_KEY_T_TREE;
    else if (type == GG_DEFTREESTATIC) return "process-scope " GG_KEY_T_TREE;
    else if (type == GG_DEFTREECURSOR) return GG_KEY_T_TREECURSOR;
    else if (type == GG_DEFFIFO) return GG_KEY_T_FIFO;
    else if (type == GG_DEFLIFO) return GG_KEY_T_LIFO;
    else if (type == GG_DEFLIST) return GG_KEY_T_LIST;
    else if (type == GG_DEFLISTSTATIC) return "process-scope " GG_KEY_T_LIST;
    else if (type == GG_DEFENCRYPT) return GG_KEY_T_ENCRYPT;
    else if (type == GG_DEFFILE) return GG_KEY_T_FILE;
    else if (type == GG_DEFSERVICE) return GG_KEY_T_SERVICE;
    else if (type == GG_DEFBOOL) return GG_KEY_T_BOOL;
    else gg_report_error ("Unknown type [%ld]", type);
}


