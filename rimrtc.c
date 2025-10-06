// SPDX-License-Identifier: Apache-2.0
// Copyright 2018-2025 Gliim LLC. 
// Licensed under Apache License v2. See LICENSE file.
// On the web http://rimstone-lang.com/ - this file is part of RimStone framework. 

//
// Library used both by RIM utility and RIM run-time 
// trailing 'c' in the name of this file refers to 'common' 
// code.
//


#include "rim.h"
int rim_errno=0;

// Function prototypes
static rim_num rim_core_write_file(FILE *f, rim_num content_len, char *content, char ispos, rim_num pos);


// 
// Get PID
//
rim_num rim_getpid ()
{
    return (rim_num) getpid();
}


//
// Get current time
// Output: outstr is the time string.
// out_str_len is time string buffer size
// If can't get time, output is empty string.
//
void rim_current_time (char *outstr, rim_num out_str_len)
{
    time_t t;
    struct tm *tmp;

    // get current time zone - may be set by customer program!
    char *curr_time_zone = getenv("TZ");
#define restore_curr_time_zone if (curr_time_zone!=NULL && curr_time_zone[0]!=0) { putenv(curr_time_zone); tzset(); }

    // set time zone to local - we did this in main() first thing before customer code. We cast rim_get_tz()
    // into mutable char * because putenv does NOT modify its string. The result of rim_get_tz must NOT change by
    // callers.
    putenv((char*)rim_get_tz());
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
// EVERYTHING IN RIM_CONFIG MUST BE MALLOC (NOT RIM_MALLOC) AS IT IS USED ACROSS REQUESTS
//
rim_config *rim_pc;

//
// Get config and context data
//
rim_config *rim_alloc_config()
{
    // start /var/log/syslog so that fatal message have a place to go to
    openlog(rim_app_name, LOG_PERROR | LOG_PID, LOG_USER);

    // all of rimstone config (sub)structures must be zeroed-out - we rely on this when setting directories, user id and such
    rim_pc = (rim_config*)calloc (1, sizeof(rim_config));
    if (rim_pc == NULL)
    {
        RIM_FATAL ("Cannot allocate program context");
    }
    // pc->* are set to 0 or NULL - set here only what's not zero
    rim_pc->app.max_upload_size = 5000000;
    rim_reset_config (rim_pc);
    return rim_pc;
}


// 
// Reset program context. This is called for each new web request
//
RIM_ALWAYS_INLINE inline  void rim_reset_config(rim_config *pc)
{
    // these need to reset with each request
    // DO NOT RESET debug structure - should stay as it is for all request during the life of process!
    pc->ctx.req = NULL;
    pc->ctx.rim_report_error_is_in_report = 0;
}

//
// UP TO HERE THERE CAN BE NO RIM_MALLOC
//



//
// Find number of occurances in string 'str' of a substring 'find'
// If len_find isn't 0, then it's length of find.
// If case_sensitive is 1, then it's case sensitive, if 0 then not.
//
// Returns number of occurances of find in str
//
rim_num rim_count_substring (char *str, char *find, rim_num len_find, rim_num case_sensitive)
{
    rim_num count = 0;
    if (find == NULL || find[0] == 0) return 0;
    // here not empty or NULL
    rim_num len;
    if (len_find != 0) len = len_find; else len = rim_mem_get_len(find);
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
rim_num rim_replace_string (char *str, rim_num strsize, char *find, char *subst, rim_num all, char **last, rim_num case_sensitive)
{

    rim_num len = strlen (str);
    rim_num lenf = strlen (find);
    rim_num lens = strlen (subst);
    rim_num occ = 0;
    rim_num currsize = len + 1;

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
// str is the string to be rim_trimmed.
// len is the length of string on the input, and it has new length
// on the output. 'len' MUST be the strlen(str) on input!
// if alloc is true, then str is alloc'd mem
// MUST NOT realloc as that would invalidate other references to this string
//
void rim_trim (char *str, rim_num *len, bool alloc)
{
    
    rim_num i = 0;
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
    if (alloc) rim_mem_set_len (str, *len+1);
}

//
// Trim string on both left and right and return pointer to trimmed string- no movement of string
// Trims spaces, tabs, newlines.
// str is the string to be trimmed
// len is the length of string on the input, and it has new length
// on the output. 'len' MUST be the strlen(str) on input!
//
char *rim_trim_ptr (char *str, rim_num *len)
{

    char *res = str;
    
    rim_num s = 0;
    // clear leading spaces
    while (isspace (str[s])) s++;
    res = str + s;

    // start from the end
    rim_num e = *len - 1;

    // update length
    *len = *len -s;

    // find the last non-space char
    rim_num j = 0;
    while (e>=s && isspace (str[e])) { e--; j++;}
    // make the end of string there, check first in case str is a constant, which will cause sigsegv if we try to write here
    if (str[e+1] != 0) str[e + 1] = 0;

    // update length of string
    *len = *len - j;
    return res;
}


//
// Get Position from file FILE* f 
// Returns RIM_OKAY if okay, RIM_ERR_POSITION if cannot do it
//
rim_num rim_get_file_pos(FILE *f, rim_num *pos)
{
    long p = ftell (f);
    if (p == -1) {
        RIM_ERR;
        return RIM_ERR_POSITION;
    }
    *pos = p;
    return RIM_OKAY;
}

//
// Position file FILE* f to pos
// Returns RIM_OKAY if okay, RIM_ERR_POSITION if cannot do it
// RIM_ERR_OPEN if file not opened
//
rim_num rim_set_file_pos(FILE *f, rim_num pos)
{
    if (f==NULL)
    {
        RIM_ERR;
        return RIM_ERR_OPEN;
    }
    if (fseek (f,pos,SEEK_SET) != 0) {
        RIM_ERR;
        return RIM_ERR_POSITION;
    }
    return RIM_OKAY;
}

//
// Get size of file from open FILE*
// f is FILE *
// Returns size of the file
//
rim_num rim_get_open_file_size(FILE *f)
{
    long ppos = ftell(f);
    fseek(f, 0L, SEEK_END);
    long size=ftell(f);
    fseek(f, ppos, SEEK_SET); // position to where we were before
    return (rim_num)size;
}

// 
// Returns RIM_DIR if 'dir' is a directory,
// RIM_FILE if it's file, RIM_ERR_FAILED if can't stat
// Returns "size" of the file, or -1 if file cannot be stat'
//
void rim_file_stat (char *dir, rim_num *type, rim_num *size, rim_num *mode)
{
    struct stat sb;
    if (stat(dir, &sb) == 0)
    {
        if (type)
        {
            if (S_ISDIR(sb.st_mode)) *type = RIM_DIR; else *type = RIM_FILE;
        }
        if (size != NULL) *size = (rim_num)(sb.st_size);
        if (mode != NULL) *mode = (rim_num)(sb.st_mode);
    }
    else 
    {
        RIM_ERR;
        if (type) *type = RIM_ERR_FAILED;
        if (size) *size = RIM_ERR_FAILED;
        if (mode) *mode = RIM_ERR_FAILED;
    }
}


// 
// Checks if input parameter name (in URL) is valid for rimstone.
// Valid names are considered to have only alphanumeric characters and
// underscores/hyphens, and must start with an alphabet character.
// If hyphen is true, then it's allowed, otherwise it's not. If hyphen is allowed and conv_hyphen is true, then each hyphen
// is converted to underscore.
// Returns 1 if name is valid, 0 if not.
//
rim_num rim_is_valid_param_name (char *name, bool hyphen, bool conv_hyphen)
{

    rim_num i = 1; // we already check the first byte before entering while loop
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
void rim_store_init (rim_fifo **fdata_ptr)
{
    *fdata_ptr = (rim_fifo*)rim_malloc (sizeof(rim_fifo));
    rim_fifo *fdata = *fdata_ptr;

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
void rim_store_l (rim_fifo *fdata, char *name, void *data)
{
    rim_fifo_item *np = rim_malloc (sizeof (rim_fifo_item));
    // No need to check if np->name/data equal to name/data because np is just created here, has nothing to begin with
    //
    rim_mem_add_ref (data, rim_mem_process);
    np->data = data;
    //
    rim_mem_add_ref (name, rim_mem_process);
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
void rim_store (rim_fifo *fdata, char *name, void *data)
{
    rim_fifo_item *np = rim_malloc (sizeof (rim_fifo_item));
    // No need to check if np->name/data equal to name/data because np is just created here, has nothing to begin with
    //
    rim_mem_add_ref (data, rim_mem_process);
    np->data = data;
    //
    rim_mem_add_ref (name, rim_mem_process);
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
rim_num rim_retrieve (rim_fifo *fdata, char **name, void **data)
{

    if (fdata->retrieve_ptr == NULL)
    {
        return RIM_ERR_EXIST;
    }
    if (data != NULL) 
    {
        *data = fdata->retrieve_ptr->data;
    }
    if (name != NULL) 
    {
        *name = fdata->retrieve_ptr->name;
    }
    fdata->retrieve_ptr = fdata->retrieve_ptr->next;
    return RIM_OKAY;
}

// 
// Rewind name/value pair in storage 'fdata', so that next rim_retrieve()
// starts from the items first put in.
void rim_rewind (rim_fifo *fdata)
{
    fdata->retrieve_ptr = fdata->first_ptr;
}

//
// Delete the very first name/value pair in fifo 'fdata', all the way up to the current 
// element (which is one after last read, or NULL if already retrieved the last one), excluding.
//
void rim_fifo_delete (rim_fifo *fdata)
{
    if (fdata->first_ptr == NULL) return;
    
    while (fdata->first_ptr != fdata->retrieve_ptr)
    {
        rim_fifo_item *temp = fdata->first_ptr;
        if (temp == fdata->last_ptr) fdata->last_ptr = fdata->first_ptr = fdata->retrieve_ptr = NULL;
        else fdata->first_ptr = temp->next;
        rim_mem_dec_process (temp->data);
        rim_mem_dec_process (temp->name);
        rim_mem_dec_process (temp);
        fdata->num_of--;
    }
}


// 
// Purge all data from storage 'fdata' and initialize for another use.
//
void rim_purge (rim_fifo **fdata_p)
{
    rim_fifo *fdata = *fdata_p;
    fdata->retrieve_ptr = NULL; // delete up to the end, i.e. all of them
    rim_fifo_delete (fdata);
    rim_mem_dec_process(fdata);
    rim_store_init (fdata_p); // okay fdata_p since fdata_p!=NULL
}

// 
// Initialize linked list storage data
// fdata is storage data variable.
// process is true if this list is process-scoped
//
void rim_list_init (rim_list **fdata_ptr, unsigned char process)
{
    *fdata_ptr = (rim_list*)rim_malloc (sizeof(rim_list));
    rim_list *fdata = *fdata_ptr;

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
void rim_list_store (rim_list *fdata, char *name, void *data, bool append)
{
    rim_list_item *np = rim_malloc (sizeof (rim_list_item));
    // No need to check if np->name/data equal to name/data because np is just created here, has nothing to begin with
    //
    rim_mem_add_ref (data, rim_mem_process);
    np->data = data;
    //
    rim_mem_add_ref (name, rim_mem_process);
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
void rim_list_retrieve (rim_list *fdata, char **name, void **data)
{

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
rim_num rim_list_delete (rim_list *fdata)
{
    rim_list_item *temp = fdata->curr;
    if (temp == NULL) return RIM_ERR_EXIST;
    rim_mem_dec_process (temp->name);
    rim_mem_dec_process (temp->data);
    // no rim_mem_delete_and_return since value is not returned here
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
    rim_mem_dec_process (temp);
    fdata->num_of--;
    return RIM_OKAY;
}


// 
// Purge all data from linked list 'fdata_p' and initialize for another use.
//
void rim_list_purge (rim_list **fdata_p)
{
    rim_list *fdata = *fdata_p;
    unsigned char p = (*fdata_p)->process;
    while (fdata->num_of > 0)
    {
        fdata->curr = fdata->first;
        rim_list_delete (fdata);
    }
    rim_mem_dec_process(fdata);
    rim_list_init (fdata_p, p); // okay fdata_p since fdata_p!=NULL
}


// 
// The same as strncpy() except that zero byte is placed at the end and it returns
// the length of the dest string.
//
rim_num rim_strncpy(char *dest, char *src, rim_num max_len)
{
    rim_num len = strlen (src);
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
char *rim_init_string(char *s)
{
    if (s == NULL) return NULL;
    rim_num l = strlen (s);
    char *res = rim_malloc (l+1);
    memcpy (res, s, l+1);
    return res;
}

// 
// Get timezone that's local to this server.
// Returns string in the format TZ=<timezone>, eg. TZ=MST
//
char * rim_get_tz ()
{
    //
    // This static usage is okay because the timezone is the SAME for all modules that could
    // run in this process. We can set timezone once for any of the modules, and the rest can
    // use the timezone just fine.
    //
    static rim_num is_tz = 0;
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
// eof is set to true if this was end of file. eof can be NULL.
// Note: zero byte is place after the end, so if return value is 10, it means
// there are 11 bytes, with zero at the end, regardless of whether the data is a
// string or not.
// If there is not enough memory, rim_malloc will error out.
//
rim_num rim_read_file (char *name, char **data, rim_num pos, rim_num len, bool *eof)
{

    if (pos < 0) {RIM_ERR0; return RIM_ERR_POSITION;} // len is negative
    if (len < 0) {RIM_ERR0; return RIM_ERR_READ;} // pos is negative

    FILE *f = rim_fopen (name, "r");
    if (f == NULL)
    {
        //rim_fopen sets RIM_ERR
        return RIM_ERR_OPEN;
    }
    rim_num sz;
    if (len > 0) sz = len; else sz = rim_get_open_file_size(f) - pos;
    if (sz < 0) { RIM_ERR0; return RIM_ERR_POSITION;} // pos is beyond size
    if (sz == 0) 
    {
        *data = RIM_EMPTY_STRING;
        return 0; // nothing to read just an empty string
    }
    if (pos > 0)
    {
        if (fseek (f,pos,SEEK_SET) != 0) { 
            RIM_ERR;
            fclose (f);
            *data = RIM_EMPTY_STRING;
            return RIM_ERR_POSITION;
        }
    }
    *data = rim_malloc (sz + 1);
    rim_num rd;
    rd = fread_unlocked (*data, 1, sz, f);
    if (rd != sz) 
    {
        // if not read enough bytes it could be an error or FEOF
        if (ferror_unlocked (f))
        {
            if (eof) *eof = false; // no FEOF for sure
            // if error, it could be we read nothing, in which case RIM_ERR_READ is the status
            if (rd == 0)
            {
                RIM_ERR;
                rim_free_int (*data);
                fclose(f); // here no need for clearerror
                *data = RIM_EMPTY_STRING;
                return RIM_ERR_READ;
            }
            else
            {
                // if short read <>0, but some error, user can get errno, file is closed further down, don't close it twice
                RIM_ERR;
            }
        } 
        else { if (eof) *eof = true; } // what else can be the reason for a short read if not EOF or error???
                          // otherwise it's FEOF and less bytes are read
    } else { if (eof) *eof = false; } // no feof if all bytes read
    (*data)[rd] = 0;
    rim_mem_set_len (*data, rd+1);
    fclose(f);
    return rd;
}


// 
// Read file with FILE* f and store file contents in output 'data'
// len (default is 0, the rest of the file) is the number of bytes to read
// from position pos, if not specified then from current position . Returns -1 if cannot open file, -2 if cannot read, 
// -3 cannot position, -4 if nothing to read (pos/len bad), or size of data read.
// eof is set to true if this was end of file. eof can be NULL.
// Note: zero byte is place after the end, so if return value is 10, it means
// there are 11 bytes, with zero at the end, regardless of whether the data is a
// string or not.
// If there is not enough memory, rim_malloc will error out.
// ispos is true if position is given
//
rim_num rim_read_file_id (FILE *f, char **data, rim_num pos, rim_num len, bool ispos, bool *eof)
{

    if (ispos && pos < 0) {RIM_ERR0; return RIM_ERR_POSITION;} // len is negative
    if (len < 0) {RIM_ERR0; return RIM_ERR_READ;} // len is negative

    if (f == NULL)
    {
        RIM_ERR;
        return RIM_ERR_OPEN;
    }
    rim_num sz;
    if (len > 0) sz = len; else
    {
        // If position not specified, find the current position, so that "reading to the 
        // end of the file" is correct exactly.
        if (!ispos) {
            if (rim_get_file_pos (f, &pos) != RIM_OKAY) { return RIM_ERR_POSITION;}
        }
        sz = rim_get_open_file_size(f) - pos;
    }
    if (sz < 0) {RIM_ERR0; return RIM_ERR_POSITION;} // pos is beyond size
    if (sz == 0) 
    {
        *data = RIM_EMPTY_STRING;
        return 0; // nothing to read just an empty string
    }
    if (ispos)
    {
        if (fseek (f,pos,SEEK_SET) != 0) { 
            RIM_ERR;
            return RIM_ERR_POSITION;
        }
    }
    *data = rim_malloc (sz + 1);
    rim_num rd;
    rd = fread_unlocked (*data, 1, sz, f);
    if (rd != sz)
    {
        // if not read enough bytes it could be an error or FEOF
        if (ferror_unlocked (f))
        {
            if (eof) *eof = false; // no FEOF for sure
            // if error, it could be we read nothing, in which case RIM_ERR_READ is the status
            if (rd == 0)
            {
                RIM_ERR;
                clearerr_unlocked (f); // if ferror() was set
                rim_free_int (*data);
                // file not closed since we keep it open via file ID, and user can position somewhere where there's no error
                *data = RIM_EMPTY_STRING;
                return RIM_ERR_READ;
            }
            else
            {
                // if short read <>0, but some error, user can get errno, file is closed
                RIM_ERR;
                clearerr_unlocked (f); // if ferror() was set
                // file not closed since we keep it open via file ID, and user can position somewhere where there's no error
                // some data is returned, and user can check errno
            }
        } 
        else { if (eof) *eof = true; } // what else can be the reason for a short read if not EOF or error???
                          // user can now know this was a short read due to FEOF
    } else { if (eof) *eof = false; } // no feof if all bytes read
    (*data)[rd] = 0;
    return rd;
}


// 
// Encode string v, producing output result res. enc_type is RIM_WEB (for
// web encoding) or RIM_URL (for url encoding). Pointer to pointer 'res' is allocated here
// with sufficient memory in the worst case scenario
// vlen is the length of v, -1 if strlen() needed, otherwise length
// alloc is true if v is rimstone alloc'd mem
// Returns length of an encoded string.
//
rim_num rim_encode (rim_num enc_type, char *v, rim_num vlen, char **res, bool alloc)
{
    rim_num mlen;
    if (vlen < 0)
    {
        if (alloc) mlen = rim_mem_get_len(v); // this is used by RimStone code written by user
        else mlen = strlen (v); // this is when used internally by RimStone
    }
    else
    {
        if (alloc)
        {
            if (vlen > (mlen=rim_mem_get_len(v))) rim_report_error ("Cannot encode [%ld] bytes of a string with length [%ld]", vlen, mlen);
            mlen = vlen;
        } else mlen = vlen; // this is internal use by rimstone
    }
    return rim_encode_base (enc_type, v, mlen , res, 1);
}

//
// Compare types, t1 and t2, if they are compatible in storage and type
// true if equal or false returned.
//
bool cmp_type (rim_num t1, rim_num t2)
{
    if ((t1 == RIM_DEFSTRING || t1 == RIM_DEFSTRINGSTATIC) && (t2 == RIM_DEFSTRING || t2 == RIM_DEFSTRINGSTATIC)) return true;
    else if ((t1 == RIM_DEFNUMBER || t1 == RIM_DEFNUMBERSTATIC) && (t2 == RIM_DEFNUMBER || t2 == RIM_DEFNUMBERSTATIC)) return true;
    else if ((t1 == RIM_DEFDOUBLE || t1 == RIM_DEFDOUBLESTATIC) && (t2 == RIM_DEFDOUBLE || t2 == RIM_DEFDOUBLESTATIC)) return true;
    else if ((t1 == RIM_DEFBOOL || t1 == RIM_DEFBOOLSTATIC) && (t2 == RIM_DEFBOOL || t2 == RIM_DEFBOOLSTATIC)) return true;
    else if ((t1 == RIM_DEFHASH || t1 == RIM_DEFHASHSTATIC) && (t2 == RIM_DEFHASH || t2 == RIM_DEFHASHSTATIC)) return true;
    else if ((t1 == RIM_DEFARRAYSTRING || t1 == RIM_DEFARRAYSTRINGSTATIC) && (t2 == RIM_DEFARRAYSTRING || t2 == RIM_DEFARRAYSTRINGSTATIC)) return true;
    else if ((t1 == RIM_DEFARRAYNUMBER || t1 == RIM_DEFARRAYNUMBERSTATIC) && (t2 == RIM_DEFARRAYNUMBER || t2 == RIM_DEFARRAYNUMBERSTATIC)) return true;
    else if ((t1 == RIM_DEFARRAYDOUBLE || t1 == RIM_DEFARRAYDOUBLESTATIC) && (t2 == RIM_DEFARRAYDOUBLE || t2 == RIM_DEFARRAYDOUBLESTATIC)) return true;
    else if ((t1 == RIM_DEFARRAYBOOL || t1 == RIM_DEFARRAYBOOLSTATIC) && (t2 == RIM_DEFARRAYBOOL || t2 == RIM_DEFARRAYBOOLSTATIC)) return true;
    else if ((t1 == RIM_DEFTREE || t1 == RIM_DEFTREESTATIC) && (t2 == RIM_DEFTREE || t2 == RIM_DEFTREESTATIC)) return true;
    else if ((t1 == RIM_DEFLIST || t1 == RIM_DEFLISTSTATIC) && (t2 == RIM_DEFLIST || t2 == RIM_DEFLISTSTATIC)) return true;
    else if ((t1 == RIM_DEFJSON ) && (t2 == RIM_DEFJSON )) return true;
    else if ((t1 == RIM_DEFXML ) && (t2 == RIM_DEFXML )) return true;
    else if (t1 == t2) return true;
    else return false;
}




// 
// Encode string v, producing output result res. enc_type is RIM_WEB (for
// web encoding) or RIM_URL (for url encoding). Pointer to pointer 'res' is allocated
// with sufficient memory in the worst case scenario (if allocate_new is 1), or if it is 0, it MUST
// have enough space to hold RIM_MAX_ENC_BLOWUP(vLen) in it) and the caller MUST handle any setting of memory length!
// vLen is the string length of v.
// Null character added.
// String v can be smaller than length vLen, vLen is the maximum number of characters encoded.
// Returns length of an encoded string.
//
rim_num rim_encode_base (rim_num enc_type, char *v, rim_num vLen, char **res, rim_num allocate_new)
{

    if (allocate_new==1)
    {
        *res = (char*)rim_malloc (RIM_MAX_ENC_BLOWUP(vLen)); // worst case, see below for usage
    }
    rim_num i;
    rim_num j = 0;
    if (enc_type == RIM_WEB)
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
    else if (enc_type == RIM_URL)
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
        rim_report_error ("Unknown encoding type");
    }
    (*res)[j] = 0;
    if (allocate_new==1)
    {
        *res = (char*)rim_realloc (rim_mem_get_id(*res), j+1); // reduce memory usage
        rim_mem_set_len (*res, j+1);
    }
    return j;
}

//
// Position to pos position (if ispos is 1), and write content of content_len length (if 0, treat content
// as string and calculate length). Write to file 'f'.
// Return # of bytes written or error. The caller can close the file if needed, it's not closed here.
//
static rim_num rim_core_write_file(FILE *f, rim_num content_len, char *content, char ispos, rim_num pos)
{
    rim_num lcontent = rim_mem_get_len(content);
    if (content_len == 0) content_len = lcontent;
    else if (content_len > lcontent) rim_report_error ("Memory used to write file is of length [%ld] but only [%ld] allocated", content_len, lcontent);

    if (ispos == 1)  // positioning beyond the end of file is allowed. The gap will be filled with \0
    {
        if (fseek (f,pos,SEEK_SET) != 0) {
            RIM_ERR;
            return RIM_ERR_POSITION;
        }
    }
    if (content_len == 0) return 0; // asked for 0 bytes, none written 
    // Use unlocked  for performance since RimStone is multi-process, and not MT
    if (fwrite_unlocked(content, 1, (size_t)content_len, f) != (size_t) content_len)
    {
        RIM_ERR;
        return RIM_ERR_WRITE;
    }
    return content_len;
}

//
// Write file 'file_name' from data 'content' of length 'content_len'. If 'append' is 1,
// then this is appended to the file, otherwise, file is overwritten (or created if it didn't
// exist). If 'content_len' is 0, then write the whole string 'content' (must be 0 terminated).
// If pos is positive or 0 (and ispos==1), then position there in the file and then write - in this case file is not overwritten.
// ispos is 0 for no position, 1 otherwise.
// Cannot have append==1 and ispos==1. RimStone checks and errors out if append and position are used before calling this.
// Returns RIM_ERR_OPEN is cannot open file, RIM_ERR_WRITE if cannot write, RIM_ERR_POSITION if cannot
// position, or number of bytes written, which is always the number of bytes requested (otherwise it's an error).
// Maximum size of file is in 0..maxlonglong range.
//
rim_num rim_write_file (char *file_name, char *content, rim_num content_len, char append, rim_num pos, char ispos)
{

    if (ispos ==1 && pos < 0)  // any position that is not 0 or positive is an error
    {
        RIM_ERR0;
        return RIM_ERR_POSITION;
    }

    FILE *f = NULL;
    // ispos is 0 if no positioning
    if (ispos == 0) f=rim_fopen (file_name,  append==1 ? "a+" : "w+"); // a+ for append, and truncate if neither append nor position
    else f=rim_fopen (file_name,  "r+"); // need read+write for positioning

    if (f==NULL)
    {
        //rim_fopen sets RIM_ERR
        return RIM_ERR_OPEN;
    }
    rim_num retw = rim_core_write_file(f, content_len, content, ispos, pos);
    fclose(f);
    return retw;
}

//
// Write file FILE *f(open) from data 'content' of length 'content_len'. If 'append' is 1,
// then this is appended to the file, otherwise, data is written at current position (if ispos is 0).
// If 'content_len' is 0, then write the whole string 'content' (must be 0 terminated).
// If pos is positive or 0 (and ispos 1), then position there in the file and then write - 
// ispos is 0 for no position, 1 otherwise.
// Cannot have append==1 and ispos==1. RimStone checks and errors out if append and position are used before calling this.
// Returns RIM_ERR_OPEN is cannot open file, RIM_ERR_WRITE if cannot write, RIM_ERR_POSITION if cannot
// position, or number of bytes written, which is always the number of bytes requested (otherwise it's an error).
// Maximum size of file is in 0..maxlonglong range.
//
rim_num rim_write_file_id (FILE *f, char *content, rim_num content_len, char append, rim_num pos, char ispos)
{

    if (f==NULL)
    {
        RIM_ERR;
        return RIM_ERR_OPEN;
    }
    if (ispos ==1 && pos < 0)  // any position that is not 0 or positive is an error
    {
        RIM_ERR0;
        return RIM_ERR_POSITION;
    }

    if (append == 1) 
    {
        rim_num ef =  rim_get_open_file_size(f);
        if (fseek (f,ef,SEEK_SET) != 0) {
            RIM_ERR;
            return RIM_ERR_POSITION;
        }
    }

    return rim_core_write_file(f, content_len, content, ispos, pos);
}


//
// Add file to RimStone's list of files to close. f is the pointer to FILE*.
// When program closes file, it must set *f = NULL in order not to be double freed.
// Returns memory index for FILE* in RimStone's mem system.
//
rim_num rim_reg_file(FILE **f)
{
    rim_num ind = rim_add_mem (f); // add pointer to file pointer so it can be closed if programmer doesn't do it
                                     // thus preventing file descriptor leakage
    // no need to setup head for memory, it's not used for FILE
    rim_mem_set_status (ind, RIM_MEM_FILE);
    return ind;
}


//
// Close a file. 
// Returns EOF if can't close, otherwise 0.
//
int rim_fclose (FILE *f)
{
    if (f == NULL) { RIM_ERR0; return RIM_ERR_CLOSE; }
    int res= fclose (f);
    if (res == EOF) {
        RIM_ERR;
        return RIM_ERR_CLOSE;
    }
    return RIM_OKAY;
}

//
// Open a file. If open for writing, set permissions to 0770
// so it's read/write for owner/group
// Returns NULL if can't open, file pointer if it can
//
FILE *rim_fopen (char *file_name, char *mode)
{
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
        RIM_ERR;
    }
    return f;
}



// Return name(id) of operating system
//
char * rim_os_name() {return RIM_OS_NAME;}


//
// Return version of operating system
//
char * rim_os_version() {return RIM_OS_VERSION;}


//
// Find a keyword 'find' in str, making sure the keyword is NOT quoted or with ()
// () is used to group an expression, which may contain keyword. Quoted string may also contain it.
// Returns the pointer to keyword in str, or NULL if not there
// Note: since each keyword has a space before it in the original RimStone statement
// we look for either 1) the space or 2) 0 character that may have been put there - that
// must be the case, unless has_spaces is 0, for example a=b - keyword = does not need space before or after.
// If find is "", then we're looking for end-of-line (i.e. null character). The purpose of this is for
// recog_statement to scan through the rest of line and find unterminated string and unbalanced ()
// If paren is 0, then () is not looked at, only quoting matters for finding keyword (this is for break-string)
// Since a keyword may be contained in another (such as url and url-path in get-req), we also check that keyword
// is followed by a space or a null (unless has_spaces is 0).
// Any single-byte keyword that is surrounded by '' is ignored, such as '{' for example - we don't want to confuse it
// with the actual code opening.
//
char *rim_find_keyword0(char *str, char *find, rim_num has_spaces, rim_num paren)
{
    char *beg = str;
    char *f;
    int l = strlen (find);
    while (1)
    {
        if (find[0] == 0) f = beg + strlen (beg); else f = strstr (beg, find);
        if (f != NULL)
        {
            // check that single quote is not surrounding find, which must be a single character
            // if so, skip this, it's just a character. For instance, we don't want to find '{' character
            // and this it's the actual code blog begining.
            if (l == 1 && f != beg && *(f-1) == '\'')
            {
                if (*(f+1) == '\'')  { beg = f+2; continue;} // skip 'x'
            }
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
                if (f[l-1] != ' ' && f[l] != 0 && f[l] != ' ') 
                {
                    beg =f+1;
                    continue;
                }
            }
            // now go back from found keyword and see how many unescaped quotes there are
            // and how many left and right parenthesis outside a string there are
            // the keyword must be outside the quotes and a string
            rim_num quotes = 0;
            rim_num left_par = 0;
            rim_num right_par = 0;
            char *go_forth = str; // start from the beginning of text
            rim_num within_string = 0;
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
                    rim_report_error( "Unterminated string");
                }
                if (left_par != right_par)
                {
                    rim_report_error( "Unbalanced left and right parenthesis () in statement");
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
// Returns 1 if hierarchical path found, 0 if no request found
//
char rim_decorate_path (char *reqname, rim_num reqname_len, char **p, rim_num p_len)
{

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
    rim_num i = 0; // begin filling request
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
// Convert str to resulting double (return value), with *st being status (RIM_OKAY if okay).
// st can be NULL.
//
RIM_ALWAYS_INLINE inline  rim_dbl rim_str2dbl (char *str, rim_num *st)
{
    char *numend;
    rim_dbl val;

    errno = 0;   // errno is used in strtod
    val = strtod(str, &numend);


    if (errno != 0) 
    {
       // ERANGE, overflow or underflow
       if (st && errno == ERANGE) *st = RIM_ERR_OVERFLOW;
       else if (st) *st = RIM_ERR_INVALID; // should not really happen, not sure what error is according man
       return 0;
    }

    if (numend == str) 
    {
       // Empty or no digits seen
       if (st) *st = RIM_ERR_EXIST;
       return 0;
    }

    if (*numend != 0)
    {
       //There were trailing invalid digits, we can still extract num
       if (st) *st = RIM_ERR_TOO_MANY;
       return (rim_dbl)val;
    }
    if (st) *st = RIM_OKAY;
    return (rim_dbl)val;
}

// 
// Convert str to resulting number (return value) in base 'base', with *st being status (RIM_OKAY if okay).
// st can be NULL. If base is 0, automatic base discovery (see docs).
//
RIM_ALWAYS_INLINE inline  rim_num rim_str2num (char *str, int base, rim_num *st)
{
    char *numend;
    rim_num val;
    if (base != 0 && (base < 2 || base > 36)) { if (st) *st = RIM_ERR_INVALID; return 0;}

    errno = 0;   // errno is used in strtoll
    val = strtoll(str, &numend, base);


    if (errno != 0) 
    {
       // ERANGE, overflow or underflow, or base is incorrect  (EINVAL)
       if (st && errno == ERANGE) *st = RIM_ERR_OVERFLOW;
       else if (st && errno == EINVAL) *st = RIM_ERR_INVALID;
       else if (st) *st = RIM_ERR_INVALID; // should not really happen, not sure what error is
       return 0;
    }

    if (numend == str) 
    {
       // Empty or no digits seen
       if (st) *st = RIM_ERR_EXIST;
       return 0;
    }

    if (*numend != 0)
    {
       //There were trailing invalid digits, we can still extract num
       if (st) *st = RIM_ERR_TOO_MANY;
       return (rim_num)val;
    }
    if (st) *st = RIM_OKAY;
    return (rim_num)val;
}


//
// get basename of file 'path' as rim_string (allocated). Path can be of a file or directory.
// returns basename
//
char *rim_basename (char *path)
{   
    char *pcopy = rim_strdup(path);
    char *res = rim_strdup(basename (pcopy)); // res has exact length set with strdup
    rim_free_int (pcopy);     
    return res;
}




//
// Return type name based on type
// This specifically doesn't have "process-scope" designation, as set-param/get-param processing depends on this 
// (i.e. string and process-scope string must be the same for parameter purposes)
//
char *typename (rim_num type)
{
    if (type == RIM_DEFSTRING) return RIM_KEY_T_STRING;
    else if (type == RIM_DEFSTRINGSTATIC) return RIM_KEY_T_STRING;
    else if (type == RIM_DEFNUMBER) return RIM_KEY_T_NUMBER;
    else if (type == RIM_DEFDOUBLE) return RIM_KEY_T_DOUBLE;
    else if (type == RIM_DEFNUMBERSTATIC) return RIM_KEY_T_NUMBER;
    else if (type == RIM_DEFDOUBLESTATIC) return RIM_KEY_T_DOUBLE;
    else if (type == RIM_DEFMSG) return RIM_KEY_T_MESSAGE;
    else if (type == RIM_DEFBROKEN) return RIM_KEY_T_SPLITSTRING;
    else if (type == RIM_DEFHASH) return RIM_KEY_T_HASH;
    else if (type == RIM_DEFARRAYSTRING) return RIM_KEY_T_ARRAYSTRING;
    else if (type == RIM_DEFARRAYNUMBER) return RIM_KEY_T_ARRAYNUMBER;
    else if (type == RIM_DEFARRAYDOUBLE) return RIM_KEY_T_ARRAYDOUBLE;
    else if (type == RIM_DEFARRAYBOOL) return RIM_KEY_T_ARRAYBOOL;
    else if (type == RIM_DEFHASHSTATIC) return RIM_KEY_T_HASH;
    else if (type == RIM_DEFARRAYSTRINGSTATIC) return RIM_KEY_T_ARRAYSTRING;
    else if (type == RIM_DEFARRAYNUMBERSTATIC) return RIM_KEY_T_ARRAYNUMBER;
    else if (type == RIM_DEFARRAYDOUBLESTATIC) return RIM_KEY_T_ARRAYDOUBLE;
    else if (type == RIM_DEFARRAYBOOLSTATIC) return RIM_KEY_T_ARRAYBOOL;
    else if (type == RIM_DEFJSON) return RIM_KEY_T_JSON;
    else if (type == RIM_DEFXML) return RIM_KEY_T_XML;
    else if (type == RIM_DEFTREE) return RIM_KEY_T_TREE;
    else if (type == RIM_DEFTREESTATIC) return RIM_KEY_T_TREE;
    else if (type == RIM_DEFTREECURSOR) return RIM_KEY_T_TREECURSOR;
    else if (type == RIM_DEFFIFO) return RIM_KEY_T_FIFO;
    else if (type == RIM_DEFLIFO) return RIM_KEY_T_LIFO;
    else if (type == RIM_DEFLIST) return RIM_KEY_T_LIST;
    else if (type == RIM_DEFLISTSTATIC) return RIM_KEY_T_LIST;
    else if (type == RIM_DEFENCRYPT) return RIM_KEY_T_ENCRYPT;
    else if (type == RIM_DEFFILE) return RIM_KEY_T_FILE;
    else if (type == RIM_DEFSERVICE) return RIM_KEY_T_SERVICE;
    else if (type == RIM_DEFBOOL) return RIM_KEY_T_BOOL;
    else if (type == RIM_DEFBOOLSTATIC) return RIM_KEY_T_BOOL;
    else rim_report_error ("Unknown type [%ld]", type);
}


//
// Convert number al to string. A new string is allocated and returned.
// base is the base of a number (2-36). 
// *res_len is the length of the result. 
// Also, if base is not between 2 and 36 (inclusive), NULL is returned and res_len is set to 0.
// NOTE: For this (and others like it) function to be the fastest, it would have  to be in a header, i.e.
// compiled with the source as static. If done so, the 100,000,000 executions of this functions take
// only 2.5 secs, whereas as a library it takes nearly 7 seconds. This is just because gcc can optimize
// the code using it as an inline. It has nothing to do with static/shared libraries, signals, before/after
// events, dispatcher or anything else. This is the reason std:to_string (for instance) is so fast - it's a header function.
// Of course, this makes code bigger, and in a large application made of many modules, it starts to
// show. Thus, the performance taken at such face value may not be pertinent to real world applications.
// This particular function is the new algorithm I wrote, that in my tests is faster than any other implementation
// I tried. It also has the most (or equal) functionality.
//
RIM_ALWAYS_INLINE inline char *rim_num2str (rim_num al, rim_num *res_len, int base)
{
    if (base < 2 || base > 36) { if (res_len != NULL) *res_len = 0; return NULL;}
    rim_num len; // length of result
    char *res; // result
    rim_num a; // temp number
    // for negative numbers start with length of 1, for "-"
    // make temp number abs() of al
    if (al < 0) { a=-al; len = 1; } else { a=al;len = 0;}
    // mods is the array of moduos derived from temp number, in the reverse order
    int mods[RIM_NUMBER_LENGTH]; // largest 64 bit is 64 digits in binary base
    int k;
    // get the length of the rest of the number (as a string), calculate mods array
    if (al == 0) len = 1; else { for (k = 0; a != 0; mods[k++] = (a%base), a/=base){} len += k; }
    res = rim_malloc(len + 1); // if the result is to be allocated, do so
    res[len] = 0; // place null at the end of string that's about to be built
    if (al == 0) { res[0] = '0'; if (res_len != NULL) *res_len = 1; rim_mem_set_len(res, 1+1); return res; } // for zero, make it '0' and return
    rim_num wlen = len; // wlen is the length of actual digits, so for negatives, it's one less due to '-'
    if (al < 0) { res[0] = '-'; al = -al; wlen--;} // place '-' for negatives
    char *p = res + len - 1; // start filling in from the end; remember our mods are calculated
                             // in reverse, so they fit in.
    for (k = 0; k < wlen; k++) // go in reverse, and fill in string in a single pass
    {
        // fill in characters for bases 2-36 (10 digits + 26 chars)
        *p-- = "0123456789abcdefghijklmnopqrstuvwxyz"[mods[k]];
    }
    if (res_len != NULL) *res_len = len;
    rim_mem_set_len (res, len+1);
    return res; // result out
}

//
// Calculate base b to the power of p, both integer, return b^p
//              
rim_num rim_topower(rim_num b,rim_num p)
{
    rim_num i;
    rim_num res = 1;
  
    for (i = 0; i < p; i++) res *= b;
                    
    return res;     
} 


//
// Convert double to string. Uses snprintf but in the future it may use Ryu.
// al is a double, prec is 1) for 'g' how many decimal digits maximum (not total can be less!), 2) for 'e' and 'f' how many after the dot
// res_len is the length of string returned
// if prec is -1, then it's not used.
// width is the width of output. It's like '%20.12f' where 20 is width, 12 is precision.
// otype is 'e', 'f' or 'g' depending on what we want in terms of formatting (e=scientific, f=floating point, g=one or the other more compact)
// Returns output, allocated
// Since this is always inline, much of the code below never actually executes at run-time (especially all those ifs), rather only the code
// needed does; that's because we pass constants for many cases.
//
RIM_ALWAYS_INLINE inline char *rim_dbl2str (rim_dbl al, char otype, int width, int prec, rim_num *res_len)
{
    char res[400]; // cannot be larger than 320ish at worst
    int sres = (int)sizeof(res);
    rim_num len = 0;
    if (prec != -1) 
    { 
        prec = MIN(prec, 18); 
        if (width != -1)
        {
            if (width >= sres) width = sres-1; // make sure there's no buffer overflow with too high a width
            if (otype == 'f') len = snprintf (res, sres, "%*.*f", width, prec, al);
            else if (otype == 'e') len = snprintf (res, sres, "%*.*e", width, prec, al);
            else if (otype == 'g') len = snprintf (res, sres, "%*.*g", width, prec, al);
        }
        else 
        {
            if (otype == 'f') len = snprintf (res, sres, "%.*f", prec, al);
            else if (otype == 'e') len = snprintf (res, sres, "%.*e", prec, al);
            else if (otype == 'g') len = snprintf (res, sres, "%.*g", prec, al);
        }
    } 
    else 
    { 
        if (width != -1)
        {
            if (width >= sres) width = sres-1; // make sure there's no buffer overflow with too high a width
            if (otype == 'f') len = snprintf (res, sres, "%*f", width, al);
            else if (otype == 'e') len = snprintf (res, sres, "%*e", width, al);
            else if (otype == 'g') len = snprintf (res, sres, "%*g", width, al);
        }
        else 
        {
            if (otype == 'f') len = snprintf (res, sres, "%f", al);
            else if (otype == 'e') len = snprintf (res, sres, "%e", al);
            else if (otype == 'g') len = snprintf (res, sres, "%g", al);
        }
    }
    if (res_len != NULL) *res_len = len;
    return rim_strdupl (res, 0, len);
}

