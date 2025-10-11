// SPDX-License-Identifier: Apache-2.0
// Copyright 2018-2025 Gliim LLC. 
// Licensed under Apache License v2. See LICENSE file.
// On the web http://rimstone-lang.com/ - this file is part of RimStone framework.

// 
// Include file for RIM run time development and apps
//


#ifndef _RIM_INC

#define _RIM_INC

// needed for crash handling (obtaining source file name and line numbers)
#ifndef _GNU_SOURCE
#define _GNU_SOURCE 
#endif

// Version+Release. Just a simple number.
#define RIM_VERSION "4.0.6"

// OS Name and Version
#define RIM_OS_NAME  RIM_OSNAME
#define RIM_OS_VERSION  RIM_OSVERSION

// database (MariaDB) related. Include if not application (i.e. if RimStone itself), and if application, only if mariadb actually used
#if RIM_APPMAKE==1 
#   if defined(RIM_MARIADB_INCLUDE)
#       define RIM_INC_MARIADB
#   endif
#   if defined(RIM_POSTGRES_INCLUDE)
#       define RIM_INC_POSTGRES
#   endif
#   if defined(RIM_SQLITE_INCLUDE)
#       define RIM_INC_SQLITE
#   endif
#endif
#ifdef RIM_INC_SQLITE
#   include <sqlite3.h>
#endif
#ifdef RIM_INC_POSTGRES
#   include <libpq-fe.h>
#endif
#ifdef RIM_INC_MARIADB
#   include <mysql.h>
#   include <mysqld_error.h>
#   include <errmsg.h>
#endif


// Includes
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <pwd.h>
#include <limits.h>
#include <fcntl.h>
#include <setjmp.h>
#include <stdbool.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/prctl.h>
#include <math.h>
#include <fenv.h>
#include <float.h>
// param.h for min/max without side effects
#include <sys/param.h>
#include <stdint.h>
// Fatal error logging
#include <syslog.h>
// File ops
#include <libgen.h>
#include <sys/file.h>
// Data conversion
#include <endian.h>
#include "rimcommon.h"

// PCRE2 calls, include pcre2 and use only one (pcre2 or glibc), depending on pcre2 version (use pcre2 if its version >=10.37, see pcre2.c)
#if RIM_APPMAKE==1 
#   if defined(RIM_PCRE2_INCLUDE)
#       define RIM_INC_PCRE2
#   endif
#endif
#ifdef RIM_INC_PCRE2
#   ifdef RIM_C_GLIBC_REGEX
#       include "regex.h"
#   else
#       include "pcre2posix.h"
#   endif
#else
#       include "regex.h"
#endif

// XML parsing 
#if RIM_APPMAKE==1 
#   if defined(RIM_XML_INCLUDE)
#       define RIM_INC_XML
#   endif
#endif
#ifdef RIM_INC_XML
#   include <libxml/parser.h>
#   include <libxml/SAX2.h>
#   include <libxml/parserInternals.h>
#endif

// Crypto calls (encrypt, hash)
#if RIM_APPMAKE==1 
#   if defined(RIM_CRYPTO_INCLUDE)
#       define RIM_INC_CRYPTO
#   endif
#endif
#ifdef RIM_INC_CRYPTO
#   include "openssl/sha.h"
#   include "openssl/evp.h"
#   include "openssl/aes.h"
#   include "openssl/bio.h"
#   include "openssl/buffer.h"
#   include "openssl/rand.h"
#   include <openssl/hmac.h>
#endif

// Fcgi calls (new-fcgi)
#if RIM_APPMAKE==1 
#   if defined(RIM_SERVICE_INCLUDE)
#       define RIM_INC_SERVICE
#   endif
#else
#   define RIM_INC_SERVICE
#endif
#ifdef RIM_INC_SERVICE
#   include "rcli.h"
#endif

// Web calls (curl)
#if RIM_APPMAKE==1 
#   if defined(RIM_CURL_INCLUDE)
#       define RIM_INC_CURL
#   endif
#endif
#ifdef RIM_INC_CURL
#   include <curl/curl.h>
#endif


// fast version of getting a config pointer
#define rim_get_config() (rim_pc)

// Stringize defines
#define RIM_DEF_STR(s) RIM_STR(s)
#define RIM_STR(s) #s



// Types
typedef uint32_t rim_unum32; // internal only
typedef int32_t rim_num32; // internal only
typedef char* rim_str;
// Request handler type
typedef rim_num (*rim_request_handler)(); // request handler in rimstone dispatcher

//
// Generic macros
//
#define RIM_UNUSED(x) (void)(x)
#define  RIM_FATAL(...) {syslog(LOG_ERR, __VA_ARGS__); _Exit(-1);}
// Note that in runtime rimrt.c, the exit(0) at the end does NOT happen as we perform a longjmp to go to the request.
// For exiting the process, use RIM_FATAL
#define  rim_report_error(...) _rim_report_error(__VA_ARGS__),exit(1)
void _rim_report_error (char *format, ...) __attribute__ ((format (printf, 1, 2)));
rim_num rim_gen_write (bool is_error, char *s, rim_num nbyte);


// Number double conversions, also implemented as functions in expressions
#define num(x) ((rim_num)(x))
#define dbl(x) ((rim_dbl)(x))
#define abn(x) (llabs(x))
#define abd(x) (fabs(x))

//
//
// RimStone memory
// process-request mem
#define RIM_MEM_PROCESS 4
// memory that's made from string literal (like process, but can never be released)
#define RIM_MEM_CONST 8
//
//
// Header prior to alloced memory. id is index into vm[]. This is the string in RimStone, and the actual string follows right after,
// allowing for good prefetching for a string.
//
typedef struct s_rim_head {
    rim_num id; 
    rim_num len;
} rim_head;
//  
//
// memory list, maximum of about 2^47-1 allocated memory blocks per request
// len is the length of memory pointed by ptr. null byte is always there and is included. Max length is 2^47-1.
// next is index to next freed block, could be anything by default. This relates to the number of variables in the program,
// ordinary memory is also linked into it, and added to the list of free memory at the end of the request, which is the fastest way to
// free it (imagine millions of process-scoped memory items interspersed, iterating through all that at the end of the request is very slow
// as opposed to just setting a single variable!).
// status has: 
// RIM_MEM_FREE bit set if this is freed block, 0 if not. 
// RIM_MEM_FILE bit set if this is a file that eventually needs to close (unless closed already)
// RIM_MEM_PROCESS bit set if this is process memory, i.e. not to be released at the end of request
// RIM_MEM_CONST bit set if this is memory that can't be freed
// ref is the number of references to process memory (max of 2^8-1 or 255), which is the number of "duplications" (by reference) of the same memory.
// The reason for a relatively small #of reference is because string constants do not have vm[] entry, which means there can be unlimited
// number of reference to a constant string; those references probably account for 99% of duplicate references. Typically otherwise it's just
// the same memory in different structures; if this is insufficient, then your program likely needs another look. Also, references beyond 1 only
// apply to process-scoped memory; it's unlikely such memory would be duplicated in many structures, or within the same structure (if so, use of
// a constant is advised).
typedef struct s_vml { // total size 8 bytes
    void *ptr;
    rim_num next:46;  
    rim_num status:9;  
    rim_num ref:9;
} vml;
#define RIM_MAX_REF ((rim_num)(1<<8)-1)
#define RIM_MAX_MEM (((rim_num)1<<45)-1)
// memory alignment of 16 bytes for alloc'd data storage
#define RIM_ALIGN (sizeof(rim_head))
#define rim_free(x) _rim_free(x)
// rim_free_int() is internal version of rim_free() which DOES always delete memory (rim_free() in general doesn't unless at the end of request)
//
extern vml *vm;
extern rim_num vm_curr;
extern char* RIM_EMPTY_STRING;
//
// Get length of memory block. RIM_ALIGN is 2*sizeof(rim_num)
// there's always trailing 0 set by all rimstone statements, so .len is useful length + 1
#define rim_mem_get_len(s) (rim_num)(((rim_head*)((unsigned char *)(s)-RIM_ALIGN))->len-1)
//
// Get index of pointer in memory block, or -1 if string constant (string constants are NOT in variable table!)
// at debug time, check each memory accessed is actually correct!
//
#ifndef DEBUG
#define rim_mem_get_id(ptr) ((rim_num)((rim_head*)((unsigned char*)(ptr)-RIM_ALIGN))->id)
#else
// in .h file, __atribute__((always_inline)) must be after the declaration
RIM_ALWAYS_INLINE static inline rim_num rim_mem_get_id (void *ptr)
{
    if (ptr == NULL) rim_report_error ("Invalid memory detected");
    rim_head *h = (rim_head*)((unsigned char*)ptr-RIM_ALIGN);
    rim_num r = h->id;
    if (r == -1) return -1;
    // at debug time, check each memory accessed is actually correct!
    if ((r < 0 || r >= vm_curr) || vm[r].ptr != ((unsigned char*)ptr-RIM_ALIGN)) rim_report_error("Attempted to get length of invalid memory");
    return r;
}
#endif
//
// Native malloc with a check
//
RIM_ALWAYS_INLINE static inline void *rim_malloc0(rim_num len) 
{
    void *res = malloc (len);
    if (res == NULL) rim_report_error ("Cannot allocate memory of size [%ld]", len);
    return res;
}
//
// Set length of memory. ptr is the memory alloc'd by RimStone.
// Memory in RimStone is almost always sized exactly as needed
// len is useful data plus 1 for nullbyte, so for "ab", it's 3
//
RIM_ALWAYS_INLINE static inline void rim_mem_set_len(void *ptr,rim_num length) {((rim_head*)((unsigned char*)ptr-RIM_ALIGN))->len = length;}
// Set memory length for C programmers
#define rim_mem_set_length(str,len) rim_mem_set_len(str, len)



// 
// Defines
//
#define RIM_DISTRESS_STATUS 107 // RimStone's distress code: report-error (if exit-code is still 0), or actuall distress (say memory access caught)
//Number to string conversion from defineds
#define RIM_STRINGIZE_(x) #x
#define RIM_STRINGIZE(x) RIM_STRINGIZE_(x)
//Concatenate defined strings
#define RIM_CONCAT_(a, b) a ## b
#define RIM_CONCAT(a, b) RIM_CONCAT_(a, b)
//
#define RIM_ERR rim_errno=errno // save errno at the point of error for further examination later, if desired
#define RIM_ERR0 rim_errno=0 // no error, we caught it
#define RIM_INIT_STRING(x) x = RIM_EMPTY_STRING // initialize existing string as empty for use with rim_malloc etc
// for rim_<memory handling> inline or not?  Currently not, otherwise would be 'inline' instead of empty
#define RIM_MAX_REQ_NAME_LEN 512 // max length of request name
#define RIM_MAX_NESTED_WRITE_STRING 5 // max # of nests of write-string
// max # of custom header a programmer can add to custom reply when replying with a file
#define RIM_MAX_HTTP_HEADER 32
#define RIM_MAX_SIZE_OF_BODY 32768 /* maximum length of body that uses fixed buffer */
#define RIM_MAX_SIZE_OF_URL 4096 /* maximum length of browser url (get) that uses fixed buffer */
#define RIM_MAX_ERR_LEN 12000 /* maximum error length in report error */
#define RIM_MAX_UPLOAD_DIR  64000 /* max directories in file directory */
#define RIM_MAX_PATH_LEN 1024 /* max path length */
#define RIM_MAX_SOCK_LEN 256 /* max file name for a local rimstone socket path */
#define RIM_ERROR_EXIT_CODE 99 // exit code of command line program when it hits any error
#define RIM_MAX_FILE_NAME 300 // max length of file name
// constants for encoding
#define RIM_URL 1
#define RIM_WEB 2
#define RIM_NOENC 3
// Max length of Linux user name allowed
#define RIM_MAX_OS_UNAME_LEN 64
// Max cookies per request and max length of a single cookie
#define RIM_MAX_COOKIES 256
#define RIM_MAX_COOKIE_SIZE 2048
// Max length of URL path (between web address and query string) 
#define RIM_MAX_PATH 2512
#define RIM_TIME_LEN 200 // max length of time strings
// maximum number of bytes needed to encode either URL or WEB encoded string
#define RIM_MAX_ENC_BLOWUP(x) ((x)*6+1)
#define RIM_MAX_QUERY_OUTPUTS 1000 // maximum # of output parameters for each query, per query, essentially # of result columns
// Constants for encryption
#define RIM_KEYLEN 64 // SHA256 hashed key or encryption key - 64 bytes - the shared memory allocated will be 64+1 bytes for zero terminator byte
// database connection limit
#define RIM_CURR_DB (rim_get_config()->ctx.db->conn[rim_get_config()->ctx.db->ind_current_db]) // context of execution
// on-error for database (continue or exit), statement level only
#define RIM_ON_ERROR_CONTINUE 1
#define RIM_ON_ERROR_EXIT 2
// Buffered output 
#define RIM_BUFFERED_OUTPUT 1
#define RIM_UNBUFFERED_OUTPUT 0
// Type of header init
#define RIM_HEADER_FILE 0
#define RIM_HEADER_PAGE 1
// modes of compilation
#define RIM_MODE_SAFE 0
#define RIM_MODE_EXTENDED 1
#define RIM_MODE_INTERNAL 2
//
#define RIM_MIN_STACK_DEPTH 100
//
//
#define RIM_SOURCE_LINE __LINE__
//
//
//
// The following are user-interfacing constants
// This must be added to variable hash to be recognized
//
// Types of request methods
#define RIM_GET 1
#define RIM_PUT 2
#define RIM_POST 3
#define RIM_PATCH 4
#define RIM_DELETE 5
#define RIM_OTHER 6
// Type of object in file system
#define RIM_FILE 1
#define RIM_DIR 2
// Info codes, when it's okay, but it needs to give more info
#define RIM_INFO_EXIST 1
// Error codes
#define RIM_OKAY 0
#define RIM_ERR_OPEN -1
#define RIM_ERR_OPEN_TARGET -2
#define RIM_ERR_READ -3
#define RIM_ERR_WRITE -4
#define RIM_ERR_POSITION -5
#define RIM_ERR_TOO_MANY -6
#define RIM_ERR_DELETE -7
#define RIM_ERR_FAILED -8
#define RIM_ERR_WEB_CALL -9
#define RIM_ERR_CREATE -10 
#define RIM_ERR_EXIST -11
#define RIM_ERR_INVALID -12
#define RIM_ERR_RENAME -13
#define RIM_ERR_MEMORY -14
#define RIM_ERR_UTF -15
#define RIM_ERR_FORMAT -16
#define RIM_ERR_CLOSE -17
#define RIM_ERR_OVERFLOW -18
#define RIM_ERR_LENGTH -20
#define RIM_ERR_REFERENCE -21
#define RIM_ERR_JSON -22
#define RIM_ERR_XML -23
// new errors below
// the last error, it's NOT user interfacing
// Note that there's RIM_CLI_ERR_TOTAL error in cli.h under -255 (so -254, -253 etc.) and those can NOT
// be used here in rim.h (must not overlap), so currently its 16, so the last error number here is 
// actually -255+16 (and not -255), which is -239 currently
#define RIM_ERR_UNKNOWN -255
//types of database
#define RIM_DB_MARIADB 0
#define RIM_DB_POSTGRES 1
#define RIM_DB_SQLITE 2
// Type for 'define' statements
#define RIM_DEFNONE 0
#define RIM_DEFSTRING 1
#define RIM_DEFNUMBER 4
#define RIM_DEFDOUBLE 5
#define RIM_DEFBROKEN 8
#define RIM_DEFJSON 9
#define RIM_DEFHASH 10
#define RIM_DEFARRAYSTRING 11
#define RIM_DEFFIFO 12
#define RIM_DEFLIFO 13
#define RIM_DEFENCRYPT 15
#define RIM_DEFFILE 16
#define RIM_DEFXML 17
#define RIM_DEFSERVICE 18
#define RIM_DEFTREE 19
#define RIM_DEFTREECURSOR 20
#define RIM_DEFHASHSTATIC 21
#define RIM_DEFTREESTATIC 22
#define RIM_DEFLIST 23
#define RIM_DEFARRAYSTRINGSTATIC 24
#define RIM_DEFLISTSTATIC 25
#define RIM_DEFBOOL 26
#define RIM_DEFSTRINGSTATIC 27
#define RIM_DEFNUMBERSTATIC 28
#define RIM_DEFMSG 29
#define RIM_DEFBOOLSTATIC 30
#define RIM_DEFARRAYNUMBERSTATIC 31
#define RIM_DEFARRAYBOOLSTATIC 32
#define RIM_DEFARRAYNUMBER 33
#define RIM_DEFARRAYBOOL 34
#define RIM_DEFDOUBLESTATIC 35
#define RIM_DEFARRAYDOUBLE 36
#define RIM_DEFARRAYDOUBLESTATIC 37
#define RIM_DEFUNKN 1024
// type names
#define RIM_KEY_T_STRING "string"
#define RIM_KEY_T_BOOL "bool"
#define RIM_KEY_T_NUMBER "number"
#define RIM_KEY_T_DOUBLE "double"
#define RIM_KEY_T_MESSAGE "message"
#define RIM_KEY_T_SPLITSTRING "split-string"
#define RIM_KEY_T_HASH "hash"
#define RIM_KEY_T_ARRAYSTRING "string-array"
#define RIM_KEY_T_ARRAYNUMBER "number-array"
#define RIM_KEY_T_ARRAYDOUBLE "double-array"
#define RIM_KEY_T_ARRAYBOOL "bool-array"
#define RIM_KEY_T_TREE "tree"
#define RIM_KEY_T_JSON "json"
#define RIM_KEY_T_XML "xml"
#define RIM_KEY_T_TREECURSOR "tree-cursor"
#define RIM_KEY_T_FIFO "fifo"
#define RIM_KEY_T_LIFO "lifo"
#define RIM_KEY_T_LIST "list"
#define RIM_KEY_T_ENCRYPT "encrypt-decrypt"
#define RIM_KEY_T_FILE "file"
#define RIM_KEY_T_SERVICE "service"
//
//
// End of user-interfacing constants
//
//

//
// Constants used to denote deleted array element
//
#define RIM_STRING_NONE NULL
#define RIM_NUMBER_NONE LLONG_MIN
#define RIM_DOUBLE_NONE NAN
#define RIM_BOOL_NONE 2
//types of random data generation
#define RIM_RANDOM_NUM 0
#define RIM_RANDOM_STR 1
#define RIM_RANDOM_BIN 2
//types of memory
#define RIM_MEM_FREE 1
#define RIM_MEM_FILE 2
//mode of message usage
#define RIM_MSG_NONE 0
#define RIM_MSG_READ 1
#define RIM_MSG_WRITE 2

// used as a silly placeholder to replace with actual length in rim_puts to increase output performance
#define RIM_EMPTY_LONG_PLAIN_ZERO 0
// maximum length of a number written (64 bit, sign for binary)
#define RIM_NUMBER_LENGTH 70

// types of json value
#define RIM_JSON_TYPE_STRING 0
#define RIM_JSON_TYPE_NUMBER 1
#define RIM_JSON_TYPE_REAL 2
#define RIM_JSON_TYPE_BOOL 3
#define RIM_JSON_TYPE_NULL 4
#define RIM_JSON_TYPE_NONE 5
// errors in json parsing
#define RIM_ERR_JSON_UNKNOWN "Unknown entity (true, false or null)"
#define RIM_ERR_JSON_NUMBER "Cannot parse a number"
#define RIM_ERR_JSON_COLON_EXPECTED "Colon is expected here"
#define RIM_ERR_JSON_NAME_EXPECTED "Name is expected here"
#define RIM_ERR_JSON_COMMA_END_OBJECT_EXPECTED "Comma or end of object is expected here"
#define RIM_ERR_JSON_COMMA_END_ARRAY_EXPECTED "Comma or end of array is expected here"
#define RIM_ERR_JSON_UNRECOGNIZED "Unrecognized token"
#define RIM_ERR_JSON_SYNTAX "Extra characters remaining unparsed"
#define RIM_ERR_JSON_BADUTF "Bad UTF character"
#define RIM_ERR_JSON_BADESCAPE "Unknown escape sequence"
#define RIM_ERR_JSON_NOQUOTE "Double quote is missing"
#define RIM_ERR_JSON_SYNTAX "Extra characters remaining unparsed"
#define RIM_ERR_JSON_DEPTH "Depth of leaf node too great"
#define RIM_ERR_JSON_SURROGATE "Surrogate UTF-8 value missing"
#define RIM_ERR_JSON_INTERRUPTED "JSON parsing interrupted by a request handler"

//UTF errors
#define RIM_UTF_ERR_ILLEGAL_CHARACTER_FEFF "Illegal character code 0xFEFF"
#define RIM_UTF_ERR_UTF16_SURROGATE "Illegal UTF16 surrogate range"
#define RIM_UTF_ERR_OUT_OF_RANGE "UTF character out of range"
#define RIM_UTF_ERR_SECOND_BYTE "Second UTF byte invalid"
#define RIM_UTF_ERR_THIRD_BYTE "Third UTF byte invalid"
#define RIM_UTF_ERR_FOURTH_BYTE "Fourth UTF byte invalid"
#define RIM_UTF_INVALID "Invalid UTF value"
#define RIM_UTF_NO_SURROGATE "Invalid surrogate UTF value"
#define RIM_ERR_UTF_BADUTF "Bad UTF character"
#define RIM_ERR_UTF_SURROGATE "Surrogate UTF-8 value missing"
#define RIM_ERR_UTF_BADESCAPE "Unknown escape sequence"
#define RIM_ERR_UTF_NOQUOTE "Double quote is missing"

//Linked list positioning constants
#define RIM_LIST_FIRST 0
#define RIM_LIST_LAST 1
#define RIM_LIST_END 2
#define RIM_LIST_PREV 3
#define RIM_LIST_NEXT 4
#define RIM_LIST_SET_CURRENT 5
#define RIM_LIST_GET_CURRENT 6

//Array memory increment to begin with
#define RIM_ARRAY_INC 256


//Request parsing errors
#define RIM_ERR_DUPREQ "Input parameter 'req' specified more than once."

// 
// Data type definitions
//


// 
// Array type internal representation
//
typedef struct rim_arraybool_s {
    char *logic; // for booleans
    rim_num max_elem; // how many total elements there can be - we don't allocate this before hand!
    rim_num alloc_elem; // how many elements are actually allocated
    unsigned char process; // holds bits for process/const
} rim_arraybool;
typedef struct rim_arraydouble_s {
    rim_dbl *dbl; // for numbers
    rim_num max_elem; // how many total elements there can be - we don't allocate this before hand!
    rim_num alloc_elem; // how many elements are actually allocated
    unsigned char process; // holds bits for process/const
} rim_arraydouble;
typedef struct rim_arraynumber_s {
    rim_num *num; // for numbers
    rim_num max_elem; // how many total elements there can be - we don't allocate this before hand!
    rim_num alloc_elem; // how many elements are actually allocated
    unsigned char process; // holds bits for process/const
} rim_arraynumber;
typedef struct rim_arraystring_s {
    char **str;  // for strings
    rim_num max_elem; // how many total elements there can be - we don't allocate this before hand!
    rim_num alloc_elem; // how many elements are actually allocated
    unsigned char process; // holds bits for process/const
} rim_arraystring;



// 
// Name/value pair for double-linked list API
//
typedef struct rim_list_item_s
{
    void *data;
    char *name;
    struct rim_list_item_s *next;
    struct rim_list_item_s *prev;
} rim_list_item;
// Information needed to traverse and rewide the linked list API
typedef struct rim_list_s
{
    rim_list_item *first; // first item in linked list
    rim_num num_of; // # of items
    rim_list_item *last; // end of list
    rim_list_item *curr; // where to apply the following action
    unsigned char process; // holds bits for process/const
} rim_list;
// 
// Name/value pair for fifo list API
//
typedef struct rim_fifo_item_s
{
    void *data;
    char *name;
    struct rim_fifo_item_s *next;
} rim_fifo_item;
// Information needed to traverse and rewide the sequential list API
typedef struct rim_fifo_s
{
    rim_fifo_item *first_ptr; // first item in fifo list
    rim_num num_of; // # of items
    rim_fifo_item *last_ptr; // end of list
    rim_fifo_item *retrieve_ptr; // where to get next one
} rim_fifo;
//
// Message structure (generic)
//
typedef struct rim_msg_s
{
    char *data; // message itself
    rim_num len; // current message length
    rim_num tot; // total allocated, which can be far greater than current message length to avoid massive realloc-s
    rim_num addinc; // add memory increment, starts with 128 and goes up to 4K
    rim_num curr; // where in parsing, or adding to, is message currently
    char mode; // RIM_MSG_NONE, RIM_MSG_READ, RIM_MSG_WRITE, default MSG_NONE, must delete msg to change from read to write and vice versa
} rim_msg;
//
// File structure, for now just FILE *
//
typedef struct rim_file_s
{
    FILE **f; // pointer to file pointer
    union {
        rim_num memind; // pointer to file pointer's location in RimStone's memory mgmt system
        rim_num id;     // OR it's a file ID for lock-file
    } attr;
} rim_file;
// 
// Configuration context data for application, read from config file. Does not change during a request.
//
typedef struct s_rim_app_data
{
    char *btrace; // backtrace file
    char *run_dir; // the current directory at the moment program starts
    char *home_dir; // home app directory
    char *user_dir; // user directory
    char *dbconf_dir; // database connections dir
    char *file_dir; // directory for uploads
    long max_upload_size; // maximum upload size for any file
} rim_app_data;
// 
//
//
// Hash structures (needed in req structure below)
//
// Hash table structure, this is an element of a linked list
typedef struct rim_s_hash_table
{
    char *key; // key for hashing
    void *data; // data
    struct rim_s_hash_table *next; // next element in the list
} rim_hash_table;
// Hash structure, top-level object
typedef struct rim_s_hash
{
    rim_num num_buckets; // size of hash table
    rim_hash_table **table; // actual table, array of lists, array of size 'size'
    rim_num dnext; // the index in table[] from which to continue dumping key/value pairs
    rim_hash_table *dcurr; // pointer to current dumping member of linked list within [dnext] bucket
    rim_hash_table *dprev; // pointer to previous dumping member of linked list within [dnext] bucket
    rim_num tot; // total how many elements in cache
    rim_num hits; // total number of searches
    rim_num reads; // total number of comparisons
    unsigned char process; // holds bits for process/const
} rim_hash;
// 
// Input arguments (argc,argv) from main for SERVICE program 
//
typedef struct s_rim_args
{
    rim_num num_of_args; // number of arguments (minus 1, we don't count argv[0])
    char **args; // arguments, starts with argv[1]
} rim_args;
// 
// Input parameters from a request (URL input parameters or POST without uploads).
//
// Single name/value pair
typedef struct s_rim_ipar 
{
    char *name; // URL names for GET/POST request
    union {
        void *value; // URL values for GET/POST request, or any param set with set-param
                 // which can be of any type, not just string
        rim_num numval; // number value, if type is number, used for set-param from number so we can get-param from it (using *value won't work as C is iffy on this).
        rim_dbl dblval; // double value, if type is double, used for set-param from double so we can get-param from it (using *value won't work as C is iffy on this).
        bool logic; // bool value
    } tval; // it's either value or numval, both are 8 bytes
    bool set; // true if variable set, false otherwise
    bool is_file; // true if this parameter is part of file upload. Used to not produce an error if file_xxx (other than _location) is not used.
    rim_num type; // type of variable
    bool alloc; // is allocated? true if so. it means it's rimmem; say input from web isn't alloc'd; string only
} rim_ipar;
//
//
// Uploaded file 
typedef struct rim_upload_t
{
    char *fname; // file name on client
    char *loc; // location on rimstone installation
    char *ext; // extension
    rim_num size; // size
} rim_upload;
// 
// Write string (write-string markup) information
typedef struct rim_write_string_t
{
    char *string; // Actual data being built. 
    char **user_string; // user string to be built
    rim_num len; // allocated length
    rim_num buf_pos; // current write position
    rim_num notrim; // default 0; if 1, do not trim the last newline
    rim_num wlen; // length of writing in the buffer (variable)
} rim_write_string;
// 
// Cookies. Array of cookies from input requests and any changes.
//
typedef struct rim_cookies_s
{
    // These are cookies as received from the client mixed with cookies set by the program
    char *data; // cookie string
    char is_set_by_program; // if 1, this cookie has been changed (deleted, or added)
} rim_cookies;
// header structure to send back file to a web client
typedef struct s_rim_header
{
    char *ctype; // content type
    rim_num clen; // content length (call-web only?)
    char *disp; // header content disposition
    char *file_name; // file name being sent
    char *cache_control; // cache control http header
    rim_num etag; // if 1,include etag which is the time stamp of last modification date of the file
    // the status_* are for status setting. status_id is the status id (such as 302) and status_text is it's corresponding text (such as '302 Found')
    // The example here is for redirection, but can be anything
    rim_num status_id;
    char *status_text;
    // the following are for generic http header of any kind, in fact content type, cache control etc. can all be done here while leaving others empty
    char *control[RIM_MAX_HTTP_HEADER+1];
    char *value[RIM_MAX_HTTP_HEADER+1];
} rim_header;
// 
// Input request. Overarching structure that contains much information not just about
// input request, but about current configuration, run-time state of the program.
typedef struct rim_input_req_s
{
    rim_app_data *app; // context, could be obtained as pc->app, but here is for convenience
    rim_num sent_header; // 1 if http header sent already 
    rim_num data_was_output; // 1 if any data (after header) was output
    char *if_none_match; // IF_NONE_MATCH from HTTP request
    rim_num disable_output; // if 1, HTML output is disabled 
    rim_write_string write_string_arr[RIM_MAX_NESTED_WRITE_STRING]; // holds a stack for write-string
    rim_num curr_write_to_string; // current write-to-string nest level from 0 to RIM_MAX_NESTED_WRITE_STRING-1
    // cookies
    rim_cookies *cookies;
    rim_num num_of_cookies;
    rim_args args; // main() input params to SERVICE program 
    char *referring_url; // where we came from 
    rim_num from_here; // did the current request come from this web server? 0 if not, 1 if yes.
    rim_num is_shut; // 1 if rim_shut already called
    rim_header *header; // if NULL, do nothing (no custom headers), if not-NULL use it to set custom headers
    char silent; // if 1, headers not output
    int ec; // exit code for command-line
    char *body; // if POST/PUT/PATCH.. has just a body (no multipart), this is it
    rim_num body_len; // if POST/PUT/PATCH.. this is body length (for no multipart)
    rim_num method; // RIM_GET/POST/PATCH/PUT/DELETE
    char *name; // request name, value of "req" param
    rim_num sub_depth; // how many handlers called on stack (used to limit) and determine if current call is top one
    rim_hash *upload; // hash for uploaded files
} rim_input_req;
// 
// Context of execution. Contains input request, flags
typedef union s_rim_dbc
{
    // In all RimStone libraries, RIM_INC_POSTGRES/MARIADB are defined. So, db-specific pointers are used instead of void*
    // In generated RimStone program, we could have any of the db-specific pointers, or void, depending on which dbs are used.
    // So if MariaDB is used, we will have MYSQL* data in the union. If Postgres is used, we will have PGresult. If any of
    // those is not used, we will have void pointers instead in the appropriate part of the union. The important thing is 
    // that the size of any pointer is the same; thus size and access to fields does not change, as long as we have the same
    // number of pointers as a substitute in each union. So:
    // 1. nothing but pointers must be used here
    // 2. the number of pointers and their names must be the same in #ifdef part as it is in #else part
    // otherwise there will be unpredictable results, because accessing fields, and allocating will be different in 
    // libraries than it is in generated code.
    struct s_pg
    {
#ifdef RIM_INC_POSTGRES
        PGresult *res;
        PGconn *con;
        char *name;
#else
        // generic substitute if no db used 
        void *con; 
        void *res; 
        void *name;
#endif
    } pg;
    struct s_l
    {
#ifdef RIM_INC_SQLITE
        sqlite3 *con;
        sqlite3_stmt *stmt;
#else
        // generic substitute if no db used 
        void *con; 
        void *stmt;
#endif
    } sqlite;
    struct s_m
    {
#ifdef RIM_INC_MARIADB
        MYSQL *con;
        MYSQL_RES *res;
        MYSQL_BIND *bind;
        MYSQL_BIND *bindout;
        MYSQL_STMT *stmt;
#else
        // generic substitute if no db used 
        void *con; 
        void *res; 
        void *bind;
        void *bindout;
        void *stmt;
#endif
    } maria;
} rim_dbc;

typedef struct one_db_s
{
    // Important: dbc is a pointer and should stay that way. The sizeof(rim_dbc) may mutate, while the size of
    // pointer to it (or to whatever) never will
    rim_dbc *dbc; // database connector. Includes any databases actually used, so one generic database connector
                  // can mutate into any database supported.
    rim_num is_begin_transaction; // are we in transaction in this process
    rim_num num_inp; // number of input parameters to query
    char need_copy; // 1 if result fields must be copied to newly alloced storage
    rim_num has_connected; // are we connected to db at this moment
    char *db_name; // the name of current connection
    rim_num db_type; // the type of current connection (0 is "mariadb" etc) - used only in compilation, not in runtime
    rim_num exit_on_error; // if 1, exit on error, otherwise, let app handle. Can be changed at run-time.
} one_db;
typedef struct s_rim_db_connections
{
    one_db *conn;
    rim_num ind_current_db; // the index (in conn array) of the currently connected db
} rim_db_connections;

typedef struct s_rim_context
{
    rim_input_req *req; // input request (see definition)
    rim_num rim_report_error_is_in_report; // 1 if in progress of reporting an error 

    //
    // Each server KEEPS connection between requests.
    // We keep a static value in the application, and application sets the pointer below to this application static value.
    //
    rim_db_connections *db;
    rim_num tot_dbs; // number of dbs

} rim_context;
// 
// Configuration and run-time status. Config file, debug file, tracing, output buffers, context of request
//
typedef struct s_rim_config
{
    // these stay the same once set
    rim_app_data app; // does not change during a request
    //
    // these change during a request
    rim_context ctx; // context of execution, not config, but convenient to
                // have it handy. That is why it's separate type. Changes at run-time
} rim_config;
// 
// Structure for breaking up strings into pieces based on delimiters
//
typedef struct rim_split_str_s
{
    char **pieces; // array of pieces
    rim_num num_pieces; // #num of pieces in 'pieces'
} rim_split_str;
//
// Information we collect about each shared library that is linked here
//
typedef struct s_rim_so_info 
{
    // Module information, used in addr2line 
    void *mod_addr; // module start load address
    long mod_offset; // offset of elf in the file (for addr2line)
    void *mod_end; // module end load address
    char mod_name[256]; // module name
} rim_so_info;



//
// An array of normalized JSON name/value pairs
//
typedef struct rim_jsonn_s
{
    char *name; // name of object, normalized
    char type; // type of value, see RIM_JSON_TYPE_...
    char *str; // string value
    bool alloced; // true if str is alloc'd, we do that only if user asks for it (lazy)
} rim_jsonn;
//
// A normalized path in json file being traversed
//
typedef struct rim_s_json_node
{
    char *name; // name of node
    rim_num name_len; // length of name of node
    rim_num index; // index of node if array, otherwise -1
    rim_num index_len; // length of array number, so for 23 it's 2
} json_node;
//
// Json structure sent back to RimStone
//
typedef struct rim_json_s
{
    rim_jsonn *nodes; // list of nodes
    rim_num node_c; // number of nodes
    rim_num node_r; // node to be read
    bool noenum; // true if do not enumerate names (i.e. with [] for arrays
    char *data; // data to parse (copied from original, but then parsed items are not copied, which is overall faster!
} rim_json;


//
// An array of normalized XML name/value pairs
// Cannot use "lazy" approach where RimStone memory is allocated later due to how libxml2 works
// (chunky values)
//
typedef struct rim_xmln_s
{
    char *name; // name of object, normalized
    char *str; // string value
} rim_xmln;
//
// A normalized path in json file being traversed
//
typedef struct rim_s_xml_node
{
    char *name; // name of node
    rim_num name_len; // length of name of node
} xml_node;
//
// XML structure sent back to RimStone
//
typedef struct rim_xml_s
{
    rim_xmln *nodes; // list of nodes
    rim_num node_c; // number of nodes
    rim_num node_r; // node to be read
} rim_xml;



//
// Tree data structures
//
typedef struct rim_tree_node_s 
{
    void *data; 
    // these are for tree implementation
    struct rim_tree_node_s *lesser_node;
    struct rim_tree_node_s *greater_node;
    unsigned char height:7; // top bit is whether key is there or not
    unsigned char key_present:1; 
    char *key; // array of flexible keys
    struct rim_tree_node_s *dlist[]; // flexible array when linked list is used
    // double linked list - this is what's there (if used)
    // struct rim_tree_node_s *lesser_list;
    // struct rim_tree_node_s *greater_list;
} rim_tree_node;
typedef struct rim_tree_s
{
    rim_tree_node *root_node; // never used directly
    rim_tree_node *tree;
    rim_tree_node *min;
    rim_tree_node *max;
    rim_num count;
    rim_num hops;
    char key_type;
    bool sorted; // true if linked list allocated for
    unsigned char process; // holds bits for process/const
} rim_tree;
typedef struct rim_tree_cursor_s {
    rim_tree *root;
    rim_tree_node *current;
    char *key;
    rim_num key_len; // length of key
    rim_num status;
    void *res;
    char *rkey;
} rim_tree_cursor;


//
//
// Inline comparisons
//
//
//
// Set of comparison functions used by _Generic to perform type-specific operation
// Self-explanatory. "_c" in function name means case insensitive. "_l" means length
// comparison. "cm" means compare. "str" is for strngs". "num" is for numbers". "less"
// is lesser than, "gr" is greater than", greq is greater or equal, lesseq is lesser or
// equal. 
// strncmp (and similar) compares "at most" l bytes, so there's never invalid memory access. Even for binary memory, there's ALWAYS and ending 0 byte after
// the useful (actual) data, so it can never happen that comparison continues beyond either memory comparison operand.
// Equality for string works for binary as well, b/c it uses memcmp.
//
RIM_ALWAYS_INLINE static inline bool rim_cm_str (char *a, rim_num la, char *b, rim_num lb)  {
    if (la != lb) return false; // strings not equal if lengths not equal
    return !memcmp(a,b, (size_t) la);
}
RIM_ALWAYS_INLINE static inline bool rim_cm_str_l (char *a, rim_num la, char *b, rim_num lb, rim_num l)  {
    if ((l > la || l > lb)) {
        if (la != lb) return false; else return !memcmp(a,b, (size_t) la);
    } else {
        // l is smaller or equal than both la and lb
        return !memcmp(a,b, (size_t) l);
    }
} 
RIM_ALWAYS_INLINE static inline bool rim_cm_str_c (char *a, rim_num la, char *b, rim_num lb)  {
    if (la != lb) return false; // strings not equal if lengths not equal
    return !strcasecmp(a,b);
}
RIM_ALWAYS_INLINE static inline bool rim_cm_str_c_l (char *a, rim_num la, char *b, rim_num lb, rim_num l) {
    if ((l > la || l > lb)) {
        if (la != lb) return false; else return !strncasecmp(a,b, (size_t) la);
    } else {
        // l is smaller than both la and lb
        return !strncasecmp(a,b, (size_t) l);
    }
} 
RIM_ALWAYS_INLINE static inline bool rim_cm_str_less (char *a, rim_num la, char *b, rim_num lb)  {
    rim_num lmin = MIN(la,lb)+1; 
    return strncmp(a,b,lmin)<0;
}
RIM_ALWAYS_INLINE static inline bool rim_cm_str_less_l (char *a, rim_num la, char *b, rim_num lb, rim_num l)  {
    rim_num lmin = MIN(la,lb); if (lmin < l) l = lmin+1;
    return strncmp(a,b,l)<0;
} 
RIM_ALWAYS_INLINE static inline bool rim_cm_str_less_c (char *a, rim_num la, char *b, rim_num lb) {
    rim_num lmin = MIN(la,lb)+1;
    return strncasecmp(a,b,lmin)<0;
}
RIM_ALWAYS_INLINE static inline bool rim_cm_str_less_c_l (char *a, rim_num la, char *b, rim_num lb, rim_num l) {
    rim_num lmin = MIN(la,lb); if (lmin < l) l = lmin+1;
    return strncasecmp(a,b,l)<0;
}
RIM_ALWAYS_INLINE static inline bool rim_cm_str_gr (char *a, rim_num la, char *b, rim_num lb)  {
    rim_num lmin = MIN(la,lb)+1; 
    return strncmp(a,b,lmin)>0;
}
RIM_ALWAYS_INLINE static inline  bool rim_cm_str_gr_l (char *a, rim_num la, char *b, rim_num lb, rim_num l)  {
    rim_num lmin = MIN(la,lb); if (lmin < l) l = lmin+1;
    return strncmp(a,b,l)>0;
} 
RIM_ALWAYS_INLINE static inline bool rim_cm_str_gr_c (char *a, rim_num la, char *b, rim_num lb) {
    rim_num lmin = MIN(la,lb)+1;
    return strncasecmp(a,b,lmin)>0;
}
RIM_ALWAYS_INLINE static inline bool rim_cm_str_gr_c_l (char *a, rim_num la, char *b, rim_num lb, rim_num l)  {
    rim_num lmin = MIN(la,lb); if (lmin < l) l = lmin+1;
    return strncasecmp(a,b,l)>0;
}
RIM_ALWAYS_INLINE static inline bool rim_cm_str_lesseq (char *a, rim_num la, char *b, rim_num lb)  {
    rim_num lmin = MIN(la,lb)+1; 
    return strncmp(a,b,lmin)<=0;
}
RIM_ALWAYS_INLINE static inline bool rim_cm_str_lesseq_l (char *a, rim_num la, char *b, rim_num lb, rim_num l) 
{
    rim_num lmin = MIN(la,lb); if (lmin < l) l = lmin+1;
    return strncmp(a,b,l)<=0;
} 
RIM_ALWAYS_INLINE static inline bool rim_cm_str_lesseq_c (char *a, rim_num la, char *b, rim_num lb) 
{
    rim_num lmin = MIN(la,lb)+1;
    return strncasecmp(a,b,lmin)<=0;
}
RIM_ALWAYS_INLINE static inline bool rim_cm_str_lesseq_c_l (char *a, rim_num la, char *b, rim_num lb, rim_num l) 
{
    rim_num lmin = MIN(la,lb); if (lmin < l) l = lmin+1;
    return strncasecmp(a,b,l)<=0;
}
RIM_ALWAYS_INLINE static inline bool rim_cm_str_greq (char *a, rim_num la, char *b, rim_num lb) 
{
    rim_num lmin = MIN(la,lb)+1; 
    return strncmp(a,b,lmin)>=0;
}
RIM_ALWAYS_INLINE static inline bool rim_cm_str_greq_l (char *a,rim_num la, char *b, rim_num lb, rim_num l) 
{
    rim_num lmin = MIN(la,lb); if (lmin < l) l = lmin+1;
    return strncmp(a,b,l)>=0;
} 
RIM_ALWAYS_INLINE static inline bool rim_cm_str_greq_c (char *a, rim_num la, char *b, rim_num lb) 
{
    rim_num lmin = MIN(la,lb)+1; // handles lesser/greater-or-equal and such. compare the lesser length +1
                             // +1 to avoid equality when they are not the same length
    return strncasecmp(a,b,lmin)>=0;
}
RIM_ALWAYS_INLINE static inline bool rim_cm_str_greq_c_l (char *a, rim_num la, char *b, rim_num lb, rim_num l) 
{
    rim_num lmin = MIN(la,lb); if (lmin < l) l = lmin+1; // handles length clause, if length > lesser of lengths, use lesser+1
                                                     // +1 to avoid equality when they are not the same length
    return strncasecmp(a,b,l)>=0;
}
RIM_ALWAYS_INLINE static inline char* rim_contain_str (char* a, rim_num la, char *b, rim_num lb) 
{ // works for binary!
    return memmem(a,la,b,lb);
}
RIM_ALWAYS_INLINE static inline char* rim_contain_str_c (char* a, rim_num la, char *b) 
{ 
    char save = a[la]; // this is when we use @(), which supplies length of string which is less than it's actual length
                       // it doesn't matter that we shorten a (in case b is a part of a); if b is beyond length la it's not a problem; if
                       // b is below length la, than it can't be longer by definition either!
    a[la] = 0;
    char *res = strcasestr(a,b);
    a[la]=save;
    return res;
}
RIM_ALWAYS_INLINE static inline char* rim_contain_int (char* a, rim_num la, int b)  
{
    return memchr(a,b,la);
}
RIM_ALWAYS_INLINE static inline char* rim_contain_int_c (char* a, rim_num la, int b) 
{
    char *res; res = memchr(a,b, la); if (res!=NULL) return res; else return memchr(a,isupper(b)?tolower(b):toupper(b),la);
}
//
RIM_ALWAYS_INLINE static inline bool rim_cm_dbl (rim_dbl a,rim_dbl b) 
{
    return a==b;
}
RIM_ALWAYS_INLINE static inline bool rim_cm_dbl_eps (rim_dbl a,rim_dbl b, rim_dbl eps) 
{
    return fabs(a-b)<eps;
}
RIM_ALWAYS_INLINE static inline bool rim_cm_dbl_less (rim_dbl a,rim_dbl b) 
{
    return a<b;
}
RIM_ALWAYS_INLINE static inline bool rim_cm_dbl_gr (rim_dbl a,rim_dbl b) 
{
    return a>b;
}
RIM_ALWAYS_INLINE static inline bool rim_cm_dbl_lesseq (rim_dbl a,rim_dbl b) 
{
    return a<=b;
}
RIM_ALWAYS_INLINE static inline bool rim_cm_dbl_greq (rim_dbl a,rim_dbl b) 
{
    return a>=b;
}
//
RIM_ALWAYS_INLINE static inline bool rim_cm_num (rim_num a,rim_num b) 
{
    return a==b;
}
RIM_ALWAYS_INLINE static inline bool rim_cm_num_less (rim_num a,rim_num b) 
{
    return a<b;
}
RIM_ALWAYS_INLINE static inline bool rim_cm_num_gr (rim_num a,rim_num b) 
{
    return a>b;
}
RIM_ALWAYS_INLINE static inline bool rim_cm_num_lesseq (rim_num a,rim_num b) 
{
    return a<=b;
}
RIM_ALWAYS_INLINE static inline bool rim_cm_num_greq (rim_num a,rim_num b) 
{
    return a>=b;
}
RIM_ALWAYS_INLINE static inline bool rim_cm_bool (bool a,bool b) 
{
    return a==b;
}
RIM_ALWAYS_INLINE static inline bool rim_cm_err () 
{
    rim_report_error("Wrong type in comparison"); return false;
}
//
//
//

//
//
//
//
// Check if type is num, string or bool
//
#define RIM_IS_NUM(A) _Static_assert(_Generic((A),  rim_num : true, unsigned int:true, int:true, long long:true, char:true, signed char:true, unsigned char:true, size_t: true, default: false),"Variable " #A " is not a number");
#define RIM_IS_BOOL(A) _Static_assert(_Generic((A),  bool:true, int:true, default: false),"Variable " #A " is not a bool");
#define RIM_IS_STRING(A) _Static_assert(_Generic((A),  char*:true, void*:true, default: false),"Variable " #A " is not a string");
//
#define RIM_STRDUP(x, y) {char *_rim_temp = (y); (x) = rim_strdup (_rim_temp == NULL ? "" : _rim_temp); }
/*hexadecimal conversions*/
#define RIM_CHAR_FROM_HEX(x) (((x)>'9') ? (((x)>='a') ? ((x)-'a'+10) : ((x)-'A'+10)) : ((x)-'0')) /* for conversion in URL - ASCII ONLY!
                        numbers are lower than capital letter are lower than lower case letters!! */
#define RIM_TO_HEX(x) ((x) <= 9 ? '0' + (x) : 'a' - 10 + (x))
#define RIM_HEX_FROM_BYTE(p,x) ((p)[0] = RIM_TO_HEX(((x)&0xF0)>>4), (p)[1] = RIM_TO_HEX((x)&0x0F))
#define RIM_HEX_FROM_INT16(p,x) (RIM_HEX_FROM_BYTE(p, (((x)&0xFF00)>>8)), RIM_HEX_FROM_BYTE(p+2, ((x)&0xFF)))
#define RIM_HEX_FROM_INT32(p,x) (RIM_HEX_FROM_BYTE(p, (((x)&0xFF000000)>>24)), RIM_HEX_FROM_BYTE(p+2, (((x)&0xFF0000)>>16)), RIM_HEX_FROM_BYTE(p+4, (((x)&0xFF00)>>8)), RIM_HEX_FROM_BYTE(p+6, ((x)&0xFF)))
/*end hexadecimal conversions*/
// Tree defines
#define RIM_TREE_TYPE_STR 's'
#define RIM_TREE_TYPE_NUM 'n'
#define RIM_TREE_LESSER 0
#define RIM_TREE_GREATER 1
#define RIM_TREE_LESSER_LIST 0
#define RIM_TREE_GREATER_LIST 1
#define rim_cur_key_len (rim_cursor->key_len)
#define rim_cur_key (rim_cursor->key)


// 
//
// Function declarations
//
//
void rim_init_input_req (rim_input_req *iu);
void rim_make_SQL (char **dest, rim_num num_of_args, char *format, ...) __attribute__ ((format (printf, 3, 4)));
void rim_output_http_header(rim_input_req *iu);
rim_num rim_encode (rim_num enc_type, char *v, rim_num vlen, char **res, bool alloc);
rim_num rim_get_input(rim_input_req *req, char *method, char *input);
void *rim_get_input_param (rim_num name_id, rim_num type, char *defval);
rim_num rim_is_positive_num (char *s);
void rim_copy_string (char *src, rim_num from, char **dst, rim_num len);
void rim_alter_string (char *tgt, char *copy, rim_num swith, rim_num len, bool begin);
rim_num rim_exec_program (char *prg, char *argv[], rim_num num_args, FILE *fin, FILE **fout, FILE **ferr, char *inp, rim_num inp_len, char **out_buf, char **err_buf);
rim_num rim_subs(char *s, void **call_handler);
void rim_get_debug_options();
rim_num rim_flush_printf(rim_num fin);
void rim_printf_close();
rim_num rim_printf (bool iserr, rim_num enc_type, char *format, ...) __attribute__ ((format (printf, 3, 4)));
void rim_shut(rim_input_req *giu);
FILE * rim_make_document (char **write_dir, rim_num is_temp);
char *rim_getenv (char *var);
char *rim_getenv_os (char *var);
void rim_putenv (char *env);
void rim_replace_all (rim_num v, char *look, char *subst);
void rim_current_time (char *outstr, rim_num out_str_len);
rim_config *rim_alloc_config();
void rim_reset_config(rim_config *pc);
rim_num rim_count_substring (char *str, char *find, rim_num len_find, rim_num case_sensitive);
rim_num rim_replace_string (char *str, rim_num strsize, char *find, char *subst, rim_num all, char **last, rim_num case_sensitive);
void rim_trim (char *str, rim_num *len, bool alloc);
char *rim_trim_ptr (char *str, rim_num *len);
void rim_file_stat (char *dir, rim_num *type, rim_num *size, rim_num *mode);
rim_num rim_get_open_file_size(FILE *f);
void rim_memory_init ();
void *rim_malloc(size_t size);
void rim_free_int(void *x);
void *rim_calloc(size_t nmemb, size_t size);
void *rim_realloc(rim_num r, size_t size);
void rim_mem_add_ref (char *s, unsigned char mem_process);
void rim_mem_release(rim_num r);
void rim_mem_dec_process (void *s);
rim_num rim_get_memory_len (void *ptr);
rim_num rim_memid (void *ptr);
void rim_set_memory_len (void *ptr, rim_num len);
void _rim_free (void *ptr);
char *rim_strdupl (char *s, rim_num from, rim_num l);
char *rim_strdup (char *s);
void *rim_mem_add_const (void *p, rim_num len);
void rim_mem_set_status (rim_num id, unsigned char s);
rim_num rim_add_mem (void *p);
void *rim_vmset (void *p, rim_num r);
rim_num rim_mem_size ();
void *rim_add_string_part (void *to, void *add, rim_num beg, rim_num len);
void *rim_add_string (void *to, void *add);
void rim_done ();
void rim_get_stack(char *err);
rim_dbc *rim_get_db_connection (rim_num abort_if_bad);
void rim_close_db_conn ();
rim_num rim_begin_transaction(char *t, char erract, char **err, char **errt);
rim_num rim_commit(char *t, char erract, char **err, char **errt);
char *typename (rim_num type);
rim_num rim_rollback(char *t, char erract, char **err, char **errt);
void rim_get_insert_id(char *val, rim_num sizeVal);
void rim_select_table (char *s, rim_num *arow, rim_num *nrow, rim_num *ncol, char ***col_names, char ***data, rim_num **dlen, char **er, char **errm, char is_prep, void **prep, rim_num paramcount, char **params, char erract);
char *rim_hash_data( char *val, char *digest_name, bool binary);
char *rim_hmac (char *key, char *data, char *digest_name, bool binary);
char *rim_derive_key( char *val, rim_num val_len, char *digest_name, rim_num iter_count, char *salt, rim_num salt_len, rim_num key_len, bool binary );
rim_num rim_ws_util_read (void * rp, char *content, rim_num len);
rim_num rim_main (void *r);
rim_num rim_ws_printf (void *r, char *fmt, ...) __attribute__ ((format (printf, 2, 3)));
rim_num rim_write_web (bool iserr, rim_config *pc, char *s, rim_num nbyte);
void rim_disable_output();
void rim_file_being_output();
void rim_dispatch_request();
rim_num rim_copy_data (char **data, char *value);
rim_num rim_puts_to_string (char *final_out, rim_num final_len);
char *rim_init_string(char *s);
rim_num rim_puts (rim_num enc_type, char *s, rim_num len, bool alloc);
rim_num rim_copy_data_at_offset (char **data, rim_num off, char *value);
rim_num rim_is_valid_param_name (char *name, bool hyphen, bool conv_hyphen);
void rim_write_to_string (char **str);
rim_num rim_write_to_string_length ();
void rim_write_to_string_notrim ();
rim_num _rim_check_memory(void *ptr);
bool rim_is_service();
void rim_set_cookie (rim_input_req *req, char *cookie_name, char *cookie_value, char *ypath, char *expires, char *samesite, char *httponly, char *secure);
char *rim_find_cookie (rim_input_req *req, char *cookie_name, rim_num *ind, char **path, char **exp);
rim_num rim_delete_cookie (rim_input_req *req, char *cookie_name, char *path, char *secure);
rim_num rim_decode (rim_num enc_type, char *v, rim_num inlen, bool alloc, rim_num *status);
char *rim_lower(char *s);
char *rim_upper(char *s);
void rim_location (char **fname, rim_num *lnum, rim_num set);
void rim_list_purge (rim_list **fdata_p);
rim_num rim_list_delete (rim_list *fdata);
rim_num rim_retrieve (rim_fifo *fdata, char **name, void **data);
void rim_list_retrieve (rim_list *fdata, char **name, void **data);
void rim_list_store (rim_list *fdata, char *name, void *data, bool append);
rim_num rim_list_pos(rim_list *fdata, int where, rim_list_item *item);
void rim_list_init (rim_list **fdata_ptr, unsigned char process);
void rim_store_init (rim_fifo **fdata);
void rim_store (rim_fifo *fdata, char *name, void *data);
void rim_store_l (rim_fifo *fdata, char *name, void *data);
void rim_rewind (rim_fifo *fdata);
void rim_purge (rim_fifo **fdata);
void rim_fifo_delete (rim_fifo *fdata);
rim_num rim_lockfile(char *filepath, rim_num *lock_fd);
void rim_append_string (char *from, char **to);
void rim_get_runtime_options();
void rim_out_file (char *fname, rim_header *header);
rim_num rim_strncpy(char *dest, char *src, rim_num max_len);
rim_num rim_getpid ();
rim_num rim_post_url_with_response(char *url, char **result, char **head, char **error, char *cert, char *cookiejar, rim_num *resp_code, long timeout, char post, char *fields[], char *files[], rim_header *vh, char *method, char *payload, rim_num payload_len);
rim_num rim_copy_file (char *src, char *dst);
void rim_b64_decode (char* in, rim_num ilen, char** out);
void rim_b64_encode(char* in, rim_num in_len, char** out);
rim_num rim_read_file (char *name, char **data, rim_num pos, rim_num len, bool *eof);
rim_num rim_read_file_id (FILE *f, char **data, rim_num pos, rim_num len, bool ispos, bool *eof);
rim_num rim_is_number (char *s, rim_num *prec, rim_num *scale, rim_num *positive);
void rim_clear_config();
void rim_init_header (rim_header *header, rim_num init_type, char is_request);
rim_num rim_write_file (char *file_name, char *content, rim_num content_len, char append, rim_num pos, char ispos);
rim_num rim_write_file_id (FILE *f, char *content, rim_num content_len, char append, rim_num pos, char ispos);
rim_num rim_get_file_pos(FILE *f, rim_num *pos);
rim_num rim_set_file_pos(FILE *f, rim_num pos);
rim_num rim_reg_file(FILE **f);
void rim_set_json (rim_json **j, bool noenum, char *data);
void rim_del_json (rim_json **j);
char *rim_json_err();
rim_num rim_json_new (char *val, rim_num *curr, rim_num len, char dec, rim_num *errc, rim_num *errl);
rim_num rim_xml_new (char *val, rim_num len, rim_num *errc, rim_num *errl);
void rim_set_xml (rim_xml **x);
void rim_del_xml (rim_xml **x);
char *rim_xml_err();
void rim_check_transaction(rim_num check_mode);
void rim_break_down (char *value, char *delim, rim_split_str **broken);
void rim_delete_break_down (rim_split_str **broken_ptr);
char * rim_get_tz ();
rim_dbc *rim_execute_SQL (char *s,  rim_num *rows, char **er, char **err_message, rim_num returns_tuples, rim_num user_check, char is_prep, void **prep, rim_num paramcount, char **params, char erract);
char *rim_num2str (rim_num al, rim_num *res_len, int base);
char *rim_dbl2str (rim_dbl al, char otype, int width, int prec, rim_num *res_len);
char *rim_time (time_t curr, char *timezone, char *format, rim_num year, rim_num month, rim_num day, rim_num hour, rim_num min, rim_num sec);
rim_num rim_encode_base (rim_num enc_type, char *v, rim_num vLen, char **res, rim_num allocate_new);
void rim_make_random (char **rnd, rim_num rnd_len, char type, bool crypto);
void rim_checkmem ();
rim_num rim_copy_data_from_num (char **data, rim_num val);
void file_too_large(rim_input_req *iu, rim_num max_size);
void oops(rim_input_req *iu, char *err);
rim_num rim_total_so(rim_so_info **sos);
FILE *rim_fopen (char *file_name, char *mode);
int rim_fclose (FILE *f);
rim_num rim_regex(char *look_here, char *find_this, char *replace, char **res, rim_num utf, rim_num case_insensitive, rim_num single_match, regex_t **cached);
void rim_regfree(regex_t *preg);
void rim_set_env(char *arg);
char * rim_os_name();
char * rim_os_version();
bool cmp_type (rim_num t1, rim_num t2);
char *rim_home_dir();
void rim_SERVICE_Finish (void);
rim_num rim_SERVICE_Accept (void);
void rim_exit_request(rim_num retval);
void rim_error_request(rim_num retval);
void after_handler();
void before_handler ();
void on_startup ();
char *rim_basename (char *path);
char *rim_realpath (char *path);
void rim_end_connection(rim_num close_db);
char *rim_find_keyword0(char *str, char *find, rim_num has_spaces, rim_num paren);
void rim_db_prep(void **prep);
char *rim_db_prep_text(char *t);
int rim_db_escape(char *from, char *to, rim_num *len);
void rim_hex2bin(char *src, char **dst, rim_num ilen);
void rim_bin2hex(char *src, char **dst, rim_num ilen, char *pref);
void rim_db_free_result (char is_prep);
rim_num rim_topower(rim_num b,rim_num p);
rim_num unicode_to_utf8(uint32_t un, unsigned char* u8);
rim_num rim_utf_get_code (char *val, rim_num *ubeg, rim_num *totjv, char **o_errm);
rim_num rim_utf_get_code8 (char *val, rim_num *ubeg, rim_num *totjv, char **o_errm);
rim_num rim_decode_utf (rim_num32 u, unsigned char *r, char **e);
rim_num rim_encode_utf (char *r, rim_num32 *u, char **e);
rim_num32 rim_make_from_utf_surrogate (rim_num32 u0, rim_num32 u1);
rim_num32 rim_get_hex(char *v, char **err, char bytes);
void rim_get_utf_surrogate (rim_num32 u, rim_num32 *u0, rim_num32 *u1);
void rim_create_hash (rim_hash **hres_ptr, rim_num size, rim_hash_table **in_h, unsigned char process);
void rim_delete_hash (rim_hash **h, bool del);
void *rim_find_hash (rim_hash *h, char *key, bool del, rim_num *found, bool is_rim);
void rim_add_hash (rim_hash *h, char *key, void *data, void **old_data, rim_num *st);
char *rim_next_hash(rim_hash *h, void **data, rim_num *st, bool del);
void rim_del_hash_traverse (rim_hash *h);
void rim_rewind_hash(rim_hash *h);
rim_num rim_total_hash (rim_hash *h);
rim_num rim_hash_size (rim_hash *h);
void rim_resize_hash (rim_hash **h, rim_num newsize);
rim_num rim_hash_reads (rim_hash *h);
char *rim_hash_get (rim_hash *hash, char *key);
char *rim_text_to_utf (char *val, char quoted, char **o_errm, char dec, bool alloced);
rim_num rim_utf_to_text (char *val, rim_num len, char **res, char **err, bool json);
char *rim_getheader(char *h);
void rim_bad_request ();
rim_num rim_set_input (rim_num name_id, void *val, rim_num type);
void rim_set_input_startup (rim_num name_id);
char *rim_getpath ();
int rim_fcgi_client_request (char *fcgi_server, char *req_method, char *path_info, char *script_name, char *content_type, int content_len, char *payload);
void rim_flush_out(void);
void rim_end_all_db();
void rim_exit (void);
char rim_decorate_path (char *reqname, rim_num reqname_len, char **p, rim_num p_len);
rim_num rim_str2num (char *str, int base, rim_num *st);
rim_dbl rim_str2dbl (char *str, rim_num *st);
rim_num rim_read_msg(rim_msg *msg, char **key, char **val);
void rim_write_msg(rim_msg *msg, char *key, char *val);
void rim_del_msg(rim_msg *msg);
char *rim_get_msg(rim_msg *msg);
rim_msg * rim_new_msg (char *from);
void rim_sleepabit(rim_num milli);
void rim_set_crash_handler(char *btrace);
//
rim_arraystring *rim_new_arraystring (rim_num max_elem, unsigned char process);
rim_arraynumber *rim_new_arraynumber (rim_num max_elem, unsigned char process);
rim_arraydouble *rim_new_arraydouble (rim_num max_elem, unsigned char process);
rim_arraybool *rim_new_arraybool (rim_num max_elem, unsigned char process);
void rim_purge_arraystring (rim_arraystring *arr);
void rim_purge_arraydouble (rim_arraydouble *arr);
void rim_purge_arraynumber (rim_arraynumber *arr);
void rim_purge_arraybool (rim_arraybool *arr);
char *rim_write_arraystring (rim_arraystring *arr, rim_num key, char **old_val);
void rim_write_arraydouble (rim_arraydouble *arr, rim_num key, rim_num *old_val);
void rim_write_arraynumber (rim_arraynumber *arr, rim_num key, rim_num *old_val);
void rim_write_arraybool (rim_arraybool *arr, rim_num key, bool *old_val);
char *rim_read_arraystring (rim_arraystring *arr, rim_num key, rim_num *st);
rim_dbl rim_read_arraydouble (rim_arraydouble *arr, rim_num key, rim_num *st);
rim_num rim_read_arraynumber (rim_arraynumber *arr, rim_num key, rim_num *st);
bool rim_read_arraybool (rim_arraybool *arr, rim_num key, rim_num *st);
//

rim_num rim_tree_bal (rim_tree_node *tree);
rim_tree *rim_tree_create(char key_type, bool sorted, unsigned char process);
void rim_tree_insert_f (rim_tree_cursor *lcurs, rim_tree *orig_tree, char *key, rim_num key_len, void *data);
void rim_tree_search_f (rim_tree_cursor *lcurs, rim_tree *orig_tree, char *key, rim_num key_len);
void rim_tree_delete_f (rim_tree_cursor *lcurs, rim_tree *orig_tree, char *key);
void rim_tree_search_greater_equal_f (rim_tree_cursor *lcurs, rim_tree *orig_tree, bool equal, char *key, rim_num key_len);
void rim_tree_search_lesser_equal_f (rim_tree_cursor *lcurs, rim_tree *orig_tree, bool equal, char *key, rim_num key_len);
void rim_tree_max_f (rim_tree_cursor *lcurs, rim_tree *orig_tree);
void rim_tree_min_f (rim_tree_cursor *lcurs, rim_tree *orig_tree);
void rim_tree_purge_f (rim_tree *parent_tree);
void rim_tree_create_root (rim_tree *res, bool sorted);

#ifdef RIM_INC_SERVICE
void rim_set_fcgi (rim_cli **callin, char *server, char *req_method, char *app_path, char *req, char *url_params, char *ctype, char *body, int clen, int timeout, char **env, bool simple_server, bool simple_url);
void rim_cli_delete (rim_cli *callin);
rim_num rim_call_fcgi (rim_cli **req, rim_num threads, rim_num *finokay, rim_num *started);
#endif

void rim_sec_load_algos(void);
int rim_RAND_bytes(unsigned char *buf, int num);
#ifdef RIM_INC_CRYPTO
rim_num rim_get_enc_key(char *password, char *salt, rim_num salt_len, rim_num iter_count, EVP_CIPHER_CTX *e_ctx, EVP_CIPHER_CTX *d_ctx,  char *cipher_name, char *digest_name);
char *rim_encrypt(EVP_CIPHER_CTX *e, const unsigned char *plaintext, rim_num *len, rim_num is_binary, unsigned char *iv);
char *rim_decrypt(EVP_CIPHER_CTX *e, unsigned char *ciphertext, rim_num *len, rim_num is_binary, unsigned char *iv);
#endif

void rim_pg_close();
rim_num rim_pg_nfield();
rim_dbc *rim_pg_connect (rim_num abort_if_bad);
rim_num rim_pg_exec(char *s, rim_num returns_tuple, char is_prep, void **prep, rim_num paramcount, char **params);
rim_num rim_pg_affected();
char *rim_pg_fieldname(rim_num fnum);
void rim_pg_free();
rim_num rim_pg_nrows();
void rim_pg_rows(char ***row, rim_num num_fields, rim_num nrow, unsigned long **lens);
char *rim_pg_error(char *s);
char *rim_pg_errm(char *errm, rim_num errmsize, char *s, char *sname, rim_num lnum, char *er);
rim_num rim_pg_checkc();
void rim_pg_close_stmt (void *st);
int rim_pg_escape(char *from, char *to, rim_num *len);

char *rim_lite_error(char *s, char is_prep);
void rim_lite_close ();
rim_dbc *rim_lite_connect (rim_num abort_if_bad);
void rim_lite_insert_id(char *val, rim_num sizeVal);
rim_num rim_lite_affected(char is_prep);
rim_num rim_lite_exec(char *s, char is_prep, void **prep, rim_num paramcount, char **params);
int rim_lite_store(char is_prep);
int rim_lite_use(char is_prep);
rim_num rim_lite_nfield();
char *rim_lite_fieldname();
void rim_lite_free(char is_prep);
rim_num rim_lite_nrows();
int rim_lite_rows (char ***row, unsigned long **lens);
char *rim_lite_errm(char *errm, rim_num errmsize, char *s, char *sname, rim_num lnum, char *er, char is_prep);
rim_num rim_lite_checkc();
void rim_lite_close_stmt (void *st);
int rim_lite_escape(char *from, char *to, rim_num *len);

char *rim_maria_error(char *s, char is_prep);
void rim_maria_close ();
rim_dbc *rim_maria_connect (rim_num abort_if_bad);
void rim_maria_insert_id(char *val, rim_num sizeVal);
rim_num rim_maria_affected(char is_prep);
rim_num rim_maria_exec(char *s, char is_prep, void **prep, rim_num paramcount, char **params);
int rim_maria_store(char is_prep);
int rim_maria_use(char is_prep);
rim_num rim_maria_nfield();
char *rim_maria_fieldname();
void rim_maria_free();
rim_num rim_maria_nrows(char is_prep);
int rim_maria_rows (char ***row, unsigned long **lens, char is_prep);
char *rim_maria_errm(char *errm, rim_num errmsize, char *s, char *sname, rim_num lnum, char *er, char is_prep);
rim_num rim_maria_checkc();
void rim_maria_close_stmt (void *st);
int rim_maria_escape(char *from, char *to, rim_num *len);

// 
// Globals
//
// Empty string. Its own value is the pointer itself, which is used
// as a differentiator between allocated and un-allocated strings.
// A string that points to 'RIM_EMPTY_STRING' is not allocated, and cannot be
// realloc-ed, otherwise it can.
//
extern rim_config *rim_pc;
extern jmp_buf rim_jmp_buffer;
extern volatile rim_num rim_done_setjmp;
extern jmp_buf rim_err_jmp_buffer;
extern volatile rim_num rim_done_err_setjmp;
extern volatile rim_num rim_in_fatal_exit;
extern char * rim_app_name;
extern char * rim_url_path;
extern char * rim_app_path;
extern char * rim_root;
extern unsigned long rim_app_path_len;
extern rim_num rim_max_upload;
extern rim_num rim_client_timeout;
extern rim_num _rim_st;
extern char *_rim_st_str;
extern bool _rim_st_bool;
extern int rim_errno;
extern int rim_stmt_cached;
extern bool rim_mem_os;
extern unsigned char rim_mem_process;
extern rim_hash rim_dispatch;
extern rim_hash rim_paramhash;
extern rim_tree_cursor *rim_cursor;
extern bool rim_true;
extern bool rim_false;
extern bool rim_path_changed;
extern rim_num rim_sub_max_depth;
extern rim_input_req rim_req;
extern rim_config *rim_s_pc;
extern char *rim_inp_body;
extern char *rim_inp_url;
extern char rim_finished_output;
extern char rim_user_dir[RIM_MAX_OS_UDIR_LEN]; // user dir


//
//
// The following through to MIG_END is the temporary wrapper for RIM_ to RIM_
// Will be removed within 3-6 months
//
//
// GG_COMPAT
#include "rim_temp.h"
//
//
// MIG_END
//
//




// DO not include rimapp.h for RimStone itself, only for applications at source build time
#if RIM_APPMAKE==1
// include generated RimStone include file
#include "rimapp.h"
       
#endif

#endif





