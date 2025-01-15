// SPDX-License-Identifier: Apache-2.0
// Copyright 2019 Gliim LLC. 
// Licensed under Apache License v2. See LICENSE file.
// On the web http://golf-lang.com/ - this file is part of Golf framework.

// 
// Include file for GLIIMLY run time development and apps
//


#ifndef _GG_INC

#define _GG_INC

// needed for crash handling (obtaining source file name and line numbers)
#ifndef _GNU_SOURCE
#define _GNU_SOURCE 
#endif

// Version+Release. Just a simple number.
#define GG_VERSION "155"

// OS Name and Version
#define GG_OS_NAME  GG_OSNAME
#define GG_OS_VERSION  GG_OSVERSION

// database (MariaDB) related. Include if not application (i.e. if Golf itself), and if application, only if mariadb actually used
#if GG_APPMAKE==1 
#   if defined(GG_MARIADB_INCLUDE)
#       define GG_INC_MARIADB
#   endif
#   if defined(GG_POSTGRES_INCLUDE)
#       define GG_INC_POSTGRES
#   endif
#   if defined(GG_SQLITE_INCLUDE)
#       define GG_INC_SQLITE
#   endif
#else
#   define GG_INC_MARIADB
#   define GG_INC_POSTGRES
#   define GG_INC_SQLITE
#endif
#ifdef GG_INC_SQLITE
#   include <sqlite3.h>
#endif
#ifdef GG_INC_POSTGRES
#   include <libpq-fe.h>
#endif
#ifdef GG_INC_MARIADB
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


// PCRE2 calls
#if GG_APPMAKE==1 
#   if defined(GG_PCRE2_INCLUDE)
#       define GG_INC_PCRE2
#   endif
#else
#   define GG_INC_PCRE2
#endif
#ifdef GG_INC_PCRE2
#   ifdef GG_C_POSIXREGEX
#       include "regex.h"
#   else
#       include "pcre2posix.h"
#   endif
#endif

// Crypto calls (encrypt, hash)
#if GG_APPMAKE==1 
#   if defined(GG_CRYPTO_INCLUDE)
#       define GG_INC_CRYPTO
#   endif
#else
#   define GG_INC_CRYPTO
#endif
#ifdef GG_INC_CRYPTO
#   include "openssl/sha.h"
#   include "openssl/evp.h"
#   include "openssl/aes.h"
#   include "openssl/bio.h"
#   include "openssl/buffer.h"
#   include "openssl/rand.h"
#   include <openssl/hmac.h>
#endif

// Fcgi calls (new-fcgi)
#if GG_APPMAKE==1 
#   if defined(GG_SERVICE_INCLUDE)
#       define GG_INC_SERVICE
#   endif
#else
#   define GG_INC_SERVICE
#endif
#ifdef GG_INC_SERVICE
#   include "gcli.h"
#endif

// Web calls (curl)
#if GG_APPMAKE==1 
#   if defined(GG_CURL_INCLUDE)
#       define GG_INC_CURL
#   endif
#else
#   define GG_INC_CURL
#endif
#ifdef GG_INC_CURL
#   include <curl/curl.h>
#endif


// XML parser (libxml2)
#if GG_APPMAKE==1 
#   if defined(GG_XML_INCLUDE)
#       define GG_INC_XML
#   endif
#else
#   define GG_INC_XML
#endif
#ifdef GG_INC_XML
#   include <libxml/parser.h>
#   include <libxml/tree.h>
#endif

// fast version of getting a config pointer
#define gg_get_config() (gg_pc)

// Stringize defines
#define GG_DEF_STR(s) GG_STR(s)
#define GG_STR(s) #s

// Set memory length for C programmers
#define gg_mem_set_length(str,len) gg_mem_set_len(gg_mem_get_id (str), len)

// Types
typedef int64_t gg_num;
typedef int32_t gg_num32;
typedef char* gg_str;
// Request handler type
typedef void (*gg_request_handler)(); // request handler in golf dispatcher

// 
// Defines
// 
#define GG_ERR gg_errno=errno // save errno at the point of error for further examination later, if desired
#define GG_ERR0 gg_errno=0 // no error, we caught it
#define GG_INIT_STRING(x) x = GG_EMPTY_STRING // initialize existing string as empty for use with gg_malloc etc
#define GG_TRACE_DIR "trace" // the name of trace directory is always 'trace'
// since we're only supporting Centos 7+, this is what comes with it, or if it's not there, user needs to install
// When we go to Centos 8, the code for Centos 7 remains in its own branch, and we set something else here for 8
// (that is, if the default changes)
// for gg_<memory handling> inline or not?  Currently not, otherwise would be 'inline' instead of empty
#define GG_TRACE_LEN 12000 // max length of line in trace file and max length of line in verbose output of 
#define GG_MAX_REQ_NAME_LEN 512 // max length of request name
#define GG_MAX_NESTED_WRITE_STRING 5 // max # of nests of write-string
// max # of custom header a programmer can add to custom reply when replying with a file
#define GG_MAX_HTTP_HEADER 32
#define GG_MAX_SIZE_OF_URL 32000 /* maximum length of browser url (get) */
#define GG_MAX_ERR_LEN 12000 /* maximum error length in report error */
#define GG_MAX_UPLOAD_DIR  40000 /* max directories in file directory */
#define GG_MAX_FILENAME_LEN 512 /* max file name for make document */
#define GG_MAX_SOCK_LEN 256 /* max file name for a local golf socket path */
#define GG_ERROR_EXIT_CODE 99 // exit code of command line program when it hits any error
// constants for encoding
#define GG_URL 1
#define GG_WEB 2
#define GG_NOENC 3
// Max cookies per request and max length of a single cookie
#define GG_MAX_COOKIES 256
#define GG_MAX_COOKIE_SIZE 2048
// Max length of URL path (between web address and query string) 
#define GG_MAX_PATH 2512
#define GG_TIME_LEN 200 // max length of time strings
// maximum number of bytes needed to encode either URL or WEB encoded string
#define GG_MAX_ENC_BLOWUP(x) ((x)*6+1)
#define GG_MAX_QUERY_OUTPUTS 1000 // maximum # of output parameters for each query, per query, essentially # of result columns
// Constants for encryption
#define GG_KEYLEN 64 // SHA256 hashed key or encryption key - 64 bytes - the shared memory allocated will be 64+1 bytes for zero terminator byte
// database connection limit
#define GG_CURR_DB (gg_get_config()->ctx.db->conn[gg_get_config()->ctx.db->ind_current_db]) // context of execution
// on-error for database (continue or exit), statement level only
#define GG_ON_ERROR_CONTINUE 1
#define GG_ON_ERROR_EXIT 2
// Buffered output 
#define GG_BUFFERED_OUTPUT 1
#define GG_UNBUFFERED_OUTPUT 0
// Type of header init
#define GG_HEADER_FILE 0
#define GG_HEADER_PAGE 1
// modes of compilation
#define GG_MODE_SAFE 0
#define GG_MODE_EXTENDED 1
#define GG_MODE_INTERNAL 2
//
//
// The following are user-interfacing constants
// This must be added to variable hash to be recognized
//
// Types of request methods
#define GG_GET 1
#define GG_PUT 2
#define GG_POST 3
#define GG_PATCH 4
#define GG_DELETE 5
#define GG_OTHER 6
// Type of object in file system
#define GG_FILE 1
#define GG_DIR 2
// Info codes, when it's okay, but it needs to give more info
#define GG_INFO_EXIST 1
// Error codes
#define GG_OKAY 0
#define GG_ERR_OPEN -1
#define GG_ERR_OPEN_TARGET -2
#define GG_ERR_READ -3
#define GG_ERR_WRITE -4
#define GG_ERR_POSITION -5
#define GG_ERR_TOO_MANY -6
#define GG_ERR_DELETE -7
#define GG_ERR_FAILED -8
#define GG_ERR_WEB_CALL -9
#define GG_ERR_CREATE -10 
#define GG_ERR_EXIST -11
#define GG_ERR_INVALID -12
#define GG_ERR_RENAME -13
#define GG_ERR_MEMORY -14
#define GG_ERR_UTF8 -15
#define GG_ERR_FORMAT -16
#define GG_ERR_CLOSE -17
#define GG_ERR_OVERFLOW -18
#define GG_ERR_LENGTH -20
#define GG_ERR_REFERENCE -21
#define GG_ERR_JSON -22
#define GG_ERR_XML_PARSE -23
#define GG_ERR_XML_EMPTY -24
// new errors below
// the last error, it's NOT user interfacing
// Note that there's GG_CLI_ERR_TOTAL error in cli.h under -255 (so -254, -253 etc.) and those can NOT
// be used here in golf.h (must not overlap), so currently its 16, so the last error number here is 
// actually -255+16 (and not -255), which is -239 currently
#define GG_ERR_UNKNOWN -255
//types of database
#define GG_DB_MARIADB 0
#define GG_DB_POSTGRES 1
#define GG_DB_SQLITE 2
// Type for 'define' statements
#define GG_DEFNONE 0
#define GG_DEFSTRING 1
#define GG_DEFNUMBER 4
#define GG_DEFBROKEN 8
#define GG_DEFJSON 9
#define GG_DEFHASH 10
#define GG_DEFFIFO 12
#define GG_DEFLIFO 13
#define GG_DEFENCRYPT 15
#define GG_DEFFILE 16
#define GG_DEFSERVICE 18
#define GG_DEFTREE 19
#define GG_DEFTREECURSOR 20
#define GG_DEFHASHSTATIC 21
#define GG_DEFTREESTATIC 22
#define GG_DEFLIST 23
#define GG_DEFLISTSTATIC 25
#define GG_DEFBOOL 26
#define GG_DEFSTRINGSTATIC 27
#define GG_DEFNUMBERSTATIC 28
#define GG_DEFMSG 29
#define GG_DEFBOOLSTATIC 30
#define GG_DEFUNKN_CHECK 1023
#define GG_DEFUNKN 1024
// type names
#define GG_KEY_T_STRING "string"
#define GG_KEY_T_BOOL "bool"
#define GG_KEY_T_NUMBER "number"
#define GG_KEY_T_MESSAGE "message"
#define GG_KEY_T_SPLITSTRING "split-string"
#define GG_KEY_T_HASH "array"
#define GG_KEY_T_TREE "index"
#define GG_KEY_T_JSON "json"
#define GG_KEY_T_TREECURSOR "index-cursor"
#define GG_KEY_T_FIFO "fifo"
#define GG_KEY_T_LIFO "lifo"
#define GG_KEY_T_LIST "list"
#define GG_KEY_T_ENCRYPT "encrypt-decrypt"
#define GG_KEY_T_FILE "file"
#define GG_KEY_T_SERVICE "service"

//
//
// End of user-interfacing constants
//
//


//types of random data generation
#define GG_RANDOM_NUM 0
#define GG_RANDOM_STR 1
#define GG_RANDOM_BIN 2
//types of memory
#define GG_MEM_FREE 1
#define GG_MEM_FILE 2
//mode of message usage
#define GG_MSG_NONE 0
#define GG_MSG_READ 1
#define GG_MSG_WRITE 2

// used as a silly placeholder to replace with actual length in gg_puts to increase output performance
#define GG_EMPTY_LONG_PLAIN_ZERO 0
// maximum length of a number written (64 bit, sign for binary)
#define GG_NUMBER_LENGTH 70

// types of json value
#define GG_JSON_TYPE_STRING 0
#define GG_JSON_TYPE_NUMBER 1
#define GG_JSON_TYPE_REAL 2
#define GG_JSON_TYPE_BOOL 3
#define GG_JSON_TYPE_NULL 4
#define GG_JSON_TYPE_NONE 5
// errors in json parsing
#define GG_ERR_JSON_UNKNOWN "Unknown entity (true, false or null)"
#define GG_ERR_JSON_NUMBER "Cannot parse a number"
#define GG_ERR_JSON_COLON_EXPECTED "Colon is expected here"
#define GG_ERR_JSON_NAME_EXPECTED "Name is expected here"
#define GG_ERR_JSON_COMMA_END_OBJECT_EXPECTED "Comma or end of object is expected here"
#define GG_ERR_JSON_COMMA_END_ARRAY_EXPECTED "Comma or end of array is expected here"
#define GG_ERR_JSON_UNRECOGNIZED "Unrecognized token"
#define GG_ERR_JSON_SYNTAX "Extra characters remaining unparsed"
#define GG_ERR_JSON_BADUTF "Bad UTF character"
#define GG_ERR_JSON_BADESCAPE "Unknown escape sequence"
#define GG_ERR_JSON_NOQUOTE "Double quote is missing"
#define GG_ERR_JSON_SYNTAX "Extra characters remaining unparsed"
#define GG_ERR_JSON_DEPTH "Depth of leaf node too great"
#define GG_ERR_JSON_SURROGATE "Surrogate UTF-8 value missing"
#define GG_ERR_JSON_INTERRUPTED "JSON parsing interrupted by a request handler"

//UTF8 errors
#define GG_UTF8_ERR_ILLEGAL_CHARACTER_FEFF "Illegal character code 0xFEFF"
#define GG_UTF8_ERR_UTF16_SURROGATE "Illegal UTF16 surrogate range"
#define GG_UTF8_ERR_OUT_OF_RANGE "UTF8 character out of range"
#define GG_UTF8_ERR_SECOND_BYTE "Second UTF8 byte invalid"
#define GG_UTF8_ERR_THIRD_BYTE "Third UTF8 byte invalid"
#define GG_UTF8_ERR_FOURTH_BYTE "Fourth UTF8 byte invalid"
#define GG_UTF8_INVALID "Invalid UTF8 value"
#define GG_UTF8_NO_SURROGATE "Invalid surrogate UTF8 value"
#define GG_ERR_UTF8_BADUTF "Bad UTF character"
#define GG_ERR_UTF8_SURROGATE "Surrogate UTF-8 value missing"
#define GG_ERR_UTF8_BADESCAPE "Unknown escape sequence"
#define GG_ERR_UTF8_NOQUOTE "Double quote is missing"

//Linked list positioning constants
#define GG_LIST_FIRST 0
#define GG_LIST_LAST 1
#define GG_LIST_END 2
#define GG_LIST_PREV 3
#define GG_LIST_NEXT 4
#define GG_LIST_SET_CURRENT 5
#define GG_LIST_GET_CURRENT 6


//Request parsing errors
#define GG_ERR_DUPREQ "Input parameter 'req' specified more than once."

// memory alignment of 8 bytes for alloc'd data storage
#define GG_ALIGN (sizeof(uint64_t))
#define gg_free(x) _gg_free(x,0)

// 
// Data type definitions
//

// 
// Debug information obtained from trace/debug file
//
typedef struct s_gg_debug_app
{
    gg_num trace_level; // trace level, currently 0 (no trace) or 1 (trace)
    gg_num trace_size;  // # of stack items in stack dump after the crash (obtained at crash from backtrace())
} gg_debug_app;
// 
// Name/value pair for double-linked list API
//
typedef struct gg_list_item_s
{
    void *data;
    char *name;
    struct gg_list_item_s *next;
    struct gg_list_item_s *prev;
} gg_list_item;
// Information needed to traverse and rewide the linked list API
typedef struct gg_list_s
{
    gg_list_item *first; // first item in linked list
    gg_num num_of; // # of items
    gg_list_item *last; // end of list
    gg_list_item *curr; // where to apply the following action
    bool process; // true if this is process-scoped
} gg_list;
// 
// Name/value pair for fifo list API
//
typedef struct gg_fifo_item_s
{
    void *data;
    char *name;
    struct gg_fifo_item_s *next;
} gg_fifo_item;
// Information needed to traverse and rewide the sequential list API
typedef struct gg_fifo_s
{
    gg_fifo_item *first_ptr; // first item in fifo list
    gg_num num_of; // # of items
    gg_fifo_item *last_ptr; // end of list
    gg_fifo_item *retrieve_ptr; // where to get next one
} gg_fifo;
//
// Message structure (generic)
//
typedef struct gg_msg_s
{
    char *data; // message itself
    gg_num len; // current message length
    gg_num tot; // total allocated, which can be far greater than current message length to avoid massive realloc-s
    gg_num addinc; // add memory increment, starts with 128 and goes up to 4K
    gg_num curr; // where in parsing, or adding to, is message currently
    char mode; // GG_MSG_NONE, GG_MSG_READ, GG_MSG_WRITE, default MSG_NONE, must delete msg to change from read to write and vice versa
} gg_msg;
//
// File structure, for now just FILE *
//
typedef struct gg_file_s
{
    FILE **f; // pointer to file pointer
    gg_num memind; // pointer to file pointer's location in Golf's memory mgmt system
} gg_file;
// 
// Configuration context data for application, read from config file. Does not change during a request.
//
typedef struct s_gg_app_data
{
    char *run_dir; // the current directory at the moment program starts
    char *home_dir; // home directory
    char *dbconf_dir; // database connections dir
    char *trace_dir; // directory for tracing
    char *file_dir; // directory for uploads
    long max_upload_size; // maximum upload size for any file
} gg_app_data;
// 
// Run-time information for tracing
//
typedef struct s_gg_conf_trace
{
    gg_num in_trace; // if 1, the caller that is attempting to use tracing function which originated in tracing code
    FILE *f; // file used for tracing file, located in trace directory
    char fname[300]; // name of trace file
    char time[GG_TIME_LEN + 1]; // time of last tracing
} gg_conf_trace;
// 
// 
// Input arguments (argc,argv) from main for SERVICE program 
//
typedef struct s_gg_args
{
    gg_num num_of_args; // number of arguments (minus 1, we don't count argv[0])
    char **args; // arguments, starts with argv[1]
} gg_args;
// 
// Input parameters from a request (URL input parameters or POST without uploads).
//
// Single name/value pair
typedef struct s_gg_ipar 
{
    char *name; // URL names for GET/POST request
    union {
        void *value; // URL values for GET/POST request, or any param set with set-param
                 // which can be of any type, not just string
        gg_num numval; // number value, if type is number, used for set-param from number so we can get-param from it (using *value won't work as C is iffy on this).
    } tval; // it's either value or numval, both are 8 bytes
    gg_num type; // type of variable
    bool alloc; // is allocated? true if so. it means it's golfmem; say input from web isn't alloc'd
    gg_num version; // current version of parameter, any parameter that doesn't match global _gg_run_version wasn't set
} gg_ipar;
// 
// Write string (write-string markup) information
typedef struct gg_write_string_t
{
    char *string; // Actual data being built. 
    char **user_string; // user string to be built
    gg_num len; // allocated length
    gg_num buf_pos; // current write position
    gg_num notrim; // default 0; if 1, do not trim the last newline
    gg_num wlen; // length of writing in the buffer (variable)
} gg_write_string;
// 
// Cookies. Array of cookies from input requests and any changes.
//
typedef struct gg_cookies_s
{
    // These are cookies as received from the client mixed with cookies set by the program
    char *data; // cookie string
    char is_set_by_program; // if 1, this cookie has been changed (deleted, or added)
} gg_cookies;
// header structure to send back file to a web client
typedef struct s_gg_header
{
    char *ctype; // content type
    gg_num clen; // content length (call-web only?)
    char *disp; // header content disposition
    char *file_name; // file name being sent
    char *cache_control; // cache control http header
    gg_num etag; // if 1,include etag which is the time stamp of last modification date of the file
    // the status_* are for status setting. status_id is the status id (such as 302) and status_text is it's corresponding text (such as '302 Found')
    // The example here is for redirection, but can be anything
    gg_num status_id;
    char *status_text;
    // the following are for generic http header of any kind, in fact content type, cache control etc. can all be done here while leaving others empty
    char *control[GG_MAX_HTTP_HEADER+1];
    char *value[GG_MAX_HTTP_HEADER+1];
} gg_header;
// 
// Input request. Overarching structure that contains much information not just about
// input request, but about current configuration, run-time state of the program.
typedef struct gg_input_req_s
{
    gg_app_data *app; // context, could be obtained as pc->app, but here is for convenience
    gg_num sent_header; // 1 if http header sent already 
    gg_num data_was_output; // 1 if any data (after header) was output
    char *if_none_match; // IF_NONE_MATCH from HTTP request
    gg_num disable_output; // if 1, HTML output is disabled 
    gg_write_string write_string_arr[GG_MAX_NESTED_WRITE_STRING]; // holds a stack for write-string
    gg_num curr_write_to_string; // current write-to-string nest level from 0 to GG_MAX_NESTED_WRITE_STRING-1
    // cookies
    gg_cookies *cookies;
    gg_num num_of_cookies;
    gg_args args; // main() input params to SERVICE program 
    char *referring_url; // where we came from 
    gg_num from_here; // did the current request come from this web server? 0 if not, 1 if yes.
    gg_num is_shut; // 1 if gg_shut already called
    gg_header *header; // if NULL, do nothing (no custom headers), if not-NULL use it to set custom headers
    char silent; // if 1, headers not output
    int ec; // exit code for command-line
    char *body; // if POST/PUT/PATCH.. has just a body (no multipart), this is it
    gg_num body_len; // if POST/PUT/PATCH.. this is body length (for no multipart)
    gg_num method; // GG_GET/POST/PATCH/PUT/DELETE
    char *name; // request name, value of "req" param
    bool sub; // true if current call (top or sub) is as sub-service at run-time
    bool optmem; // true if we're optimizing memory (cleanup ASAP, which does slow down things, but good if you're short on memory). false by default.
} gg_input_req;
// 
// Context of execution. Contains input request, flags
typedef union s_gg_dbc
{
    // In all Golf libraries, GG_INC_POSTGRES/MARIADB are defined. So, db-specific pointers are used instead of void*
    // In generated Golf program, we could have any of the db-specific pointers, or void, depending on which dbs are used.
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
#ifdef GG_INC_POSTGRES
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
#ifdef GG_INC_SQLITE
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
#ifdef GG_INC_MARIADB
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
} gg_dbc;

typedef struct one_db_s
{
    // Important: dbc is a pointer and should stay that way. The sizeof(gg_dbc) may mutate, while the size of
    // pointer to it (or to whatever) never will
    gg_dbc *dbc; // database connector. Includes any databases actually used, so one generic database connector
                  // can mutate into any database supported.
    gg_num is_begin_transaction; // are we in transaction in this process
    gg_num num_inp; // number of input parameters to query
    char need_copy; // 1 if result fields must be copied to newly alloced storage
    gg_num has_connected; // are we connected to db at this moment
    char *db_name; // the name of current connection
    gg_num db_type; // the type of current connection (0 is "mariadb" etc) - used only in compilation, not in runtime
    gg_num exit_on_error; // if 1, exit on error, otherwise, let app handle. Can be changed at run-time.
} one_db;
typedef struct s_gg_db_connections
{
    one_db *conn;
    gg_num ind_current_db; // the index (in conn array) of the currently connected db
} gg_db_connections;

typedef struct s_gg_context
{
    gg_input_req *req; // input request (see definition)
    gg_num gg_report_error_is_in_report; // 1 if in progress of reporting an error 

    //
    // Each server KEEPS connection between requests.
    // We keep a static value in the application, and application sets the pointer below to this application static value.
    //
    gg_db_connections *db;
    gg_num tot_dbs; // number of dbs

} gg_context;
// 
// Configuration and run-time status. Config file, debug file, tracing, output buffers, context of request
//
typedef struct s_gg_config
{
    // these stay the same once set
    gg_app_data app; // does not change during a request
    gg_debug_app debug; // does not change during a request

    // these change during a request
    gg_conf_trace trace; // tracing info
    gg_context ctx; // context of execution, not config, but convenient to
                // have it handy. That is why it's separate type. Changes at run-time
} gg_config;
// 
// Structure for breaking up strings into pieces based on delimiters
//
typedef struct gg_split_str_s
{
    char **pieces; // array of pieces
    gg_num num_pieces; // #num of pieces in 'pieces'
} gg_split_str;
//
// Information we collect about each shared library that is linked here
//
typedef struct s_gg_so_info 
{
    // Module information, used in addr2line 
    void *mod_addr; // module start load address
    long mod_offset; // offset of elf in the file (for addr2line)
    void *mod_end; // module end load address
    char mod_name[256]; // module name
} gg_so_info;


//
// Hash structures
//
// Hash table structure, this is an element of a linked list
typedef struct gg_s_hash_table
{
    char *key; // key for hashing
    void *data; // data
    struct gg_s_hash_table *next; // next element in the list
} gg_hash_table;
// Hash structure, top-level object
typedef struct gg_s_hash
{
    gg_num num_buckets; // size of hash table
    gg_hash_table **table; // actual table, array of lists, array of size 'size'
    gg_num dnext; // the index in table[] from which to continue dumping key/value pairs
    gg_hash_table *dcurr; // pointer to current dumping member of linked list within [dnext] bucket
    gg_hash_table *dprev; // pointer to previous dumping member of linked list within [dnext] bucket
    gg_num tot; // total how many elements in cache
    gg_num hits; // total number of searches
    gg_num reads; // total number of comparisons
    bool process; // true if process-scoped, false if request-scoped
} gg_hash;

//
// An array of normalized JSON name/value pairs
//
typedef struct gg_jsonn_s
{
    char *name; // name of object, normalized
    char type; // type of value, see GG_JSON_TYPE_...
    char *str; // string value
    bool alloced; // true if str is alloc'd, we do that only if user asks for it (lazy)
} gg_jsonn;
//
// A normalized path in json file being traversed
//
typedef struct gg_s_json_node
{
    char *name; // name of node
    gg_num name_len; // length of name of node
    gg_num index; // index of node if array, otherwise -1
    gg_num index_len; // length of array number, so for 23 it's 2
} json_node;
//
// Json structure sent back to Golf
//
typedef struct gg_json_s
{
    gg_jsonn *nodes; // list of nodes
    gg_num node_c; // number of nodes
    gg_num node_r; // node to be read
    bool noenum; // true if do not enumerate names (i.e. with [] for arrays
} gg_json;


//
// Tree data structures
//
typedef struct gg_tree_node_s 
{
    void *data; 
    // these are for tree implementation
    struct gg_tree_node_s *lesser_node;
    struct gg_tree_node_s *greater_node;
    unsigned char height:7; // top bit is whether key is there or not
    unsigned char key_present:1; 
    char *key; // array of flexible keys
    struct gg_tree_node_s *dlist[]; // flexible array when linked list is used
    // double linked list - this is what's there (if used)
    // struct gg_tree_node_s *lesser_list;
    // struct gg_tree_node_s *greater_list;
} gg_tree_node;
typedef struct gg_tree_s
{
    gg_tree_node *root_node; // never used directly
    gg_tree_node *tree;
    gg_tree_node *min;
    gg_tree_node *max;
    gg_num count;
    gg_num hops;
    char key_type;
    bool sorted; // true if linked list allocated for
    bool process; // true if process-wide memory used
} gg_tree;
typedef struct gg_tree_cursor_s {
    gg_tree *root;
    gg_tree_node *current;
    char *key;
    gg_num key_len; // length of key
    gg_num status;
    void *res;
    char *rkey;
} gg_tree_cursor;


// 
// Macros and function call related
// Some of these may have a double evaluation problem (TODO) in macros
//
#define GG_UNUSED(x) (void)(x)
#define  GG_FATAL(...) {syslog(LOG_ERR, __VA_ARGS__); _Exit(-1);}
//trace is available only if Golf compiled with DI=1
#ifdef DEBUG
#define  GG_TRACE(...) (gg_get_config()->debug.trace_level !=0 ? _gg_trace(1, __FILE__, __LINE__, __FUNCTION__,  __VA_ARGS__) : 0)
#else
#define  GG_TRACE(...) true
#endif
#define  gg_report_error(...) {_gg_report_error(__VA_ARGS__);exit(0);}
//
// Generic compare, evaluated at compile time, so the actual comparison doesn't have any runtime overhead, since we use inline functions
//
#define  GG_CMP_EQ(A,B) \
  _Generic((A), \
    char* : gg_cm_str, \
    gg_num : gg_cm_num, \
    char : gg_cm_num, \
    int : gg_cm_num, \
    size_t: gg_cm_num, \
    bool : gg_cm_bool, \
    default : gg_cm_err \
  ) (A,B)
#define  GG_CMP_LESSEQ(A,B) \
  _Generic((A), \
    char* : gg_cm_str_lesseq, \
    gg_num : gg_cm_num_lesseq, \
    char : gg_cm_num_lesseq, \
    int : gg_cm_num_lesseq, \
    size_t : gg_cm_num_lesseq, \
    default : gg_cm_err \
  ) (A,B)
#define  GG_CMP_LESS(A,B) \
  _Generic((A), \
    char* : gg_cm_str_less, \
    gg_num : gg_cm_num_less, \
    char : gg_cm_num_less, \
    int : gg_cm_num_less, \
    size_t : gg_cm_num_less, \
    default : gg_cm_err \
  ) (A,B)
#define  GG_CMP_GREQ(A,B) \
  _Generic((A), \
    char* : gg_cm_str_greq, \
    gg_num : gg_cm_num_greq, \
    char : gg_cm_num_greq, \
    int : gg_cm_num_greq, \
    default : gg_cm_err \
  ) (A,B)
#define  GG_CMP_GR(A,B) \
  _Generic((A), \
    char* : gg_cm_str_gr, \
    gg_num : gg_cm_num_gr, \
    char : gg_cm_num_gr, \
    int : gg_cm_num_gr, \
    size_t : gg_cm_num_gr, \
    default : gg_cm_err \
  ) (A,B)
#define  GG_CONTAINS(A,B) \
  _Generic((B), \
    char* : gg_contain_str, \
    char : gg_contain_int, \
    int : gg_contain_int, \
    default : gg_cm_err \
  ) (A,B)
//
//Regular compare with length
//
#define  GG_CMP_EQ_L(A,B,L) \
  _Generic((A), \
    char* : gg_cm_str_l, \
    default : gg_cm_err \
  ) (A,B,L)
#define  GG_CMP_LESSEQ_L(A,B,L) \
  _Generic((A), \
    char* : gg_cm_str_lesseq_l, \
    default : gg_cm_err \
  ) (A,B,L)
#define  GG_CMP_LESS_L(A,B,L) \
  _Generic((A), \
    char* : gg_cm_str_less_l, \
    default : gg_cm_err \
  ) (A,B,L)
#define  GG_CMP_GREQ_L(A,B,L) \
  _Generic((A), \
    char* : gg_cm_str_greq_l, \
    default : gg_cm_err \
  ) (A,B,L)
#define  GG_CMP_GR_L(A,B,L) \
  _Generic((A), \
    char* : gg_cm_str_gr_l, \
    default : gg_cm_err \
  ) (A,B,L)
//
// Case insensitive compare, strings, only, others are an error
//
#define  GG_CMP_EQ_C(A,B) \
  _Generic((A), \
    char* : gg_cm_str_c, \
    default : gg_cm_err \
  ) (A,B)
#define  GG_CMP_LESSEQ_C(A,B) \
  _Generic((A), \
    char* : gg_cm_str_lesseq_c, \
    default : gg_cm_err \
  ) (A,B)
#define  GG_CMP_LESS_C(A,B) \
  _Generic((A), \
    char* : gg_cm_str_less_c, \
    default : gg_cm_err \
  ) (A,B)
#define  GG_CMP_GREQ_C(A,B) \
  _Generic((A), \
    char* : gg_cm_str_greq_c, \
    default : gg_cm_err \
  ) (A,B)
#define  GG_CMP_GR_C(A,B) \
  _Generic((A), \
    char* : gg_cm_str_gr_c, \
    default : gg_cm_err \
  ) (A,B)
#define  GG_CONTAINS_C(A,B) \
  _Generic((B), \
    char* : gg_contain_str_c, \
    char : gg_contain_int, \
    default : gg_cm_err \
  ) (A,B)
//
//Case insensitive with length
//
#define  GG_CMP_EQ_C_L(A,B,L) \
  _Generic((A), \
    char* : gg_cm_str_c_l, \
    default : gg_cm_err \
  ) (A,B,L)
#define  GG_CMP_LESSEQ_C_L(A,B,L) \
  _Generic((A), \
    char* : gg_cm_str_lesseq_c_l, \
    default : gg_cm_err \
  ) (A,B,L)
#define  GG_CMP_LESS_C_L(A,B,L) \
  _Generic((A), \
    char* : gg_cm_str_less_c_l, \
    default : gg_cm_err \
  ) (A,B,L)
#define  GG_CMP_GREQ_C_L(A,B,L) \
  _Generic((A), \
    char* : gg_cm_str_greq_c_l, \
    default : gg_cm_err \
  ) (A,B,L)
#define  GG_CMP_GR_C_L(A,B,L) \
  _Generic((A), \
    char* : gg_cm_str_gr_c_l, \
    default : gg_cm_err \
  ) (A,B,L)
//
// Check if type is num, string or bool
//
#define GG_IS_NUM(A) _Static_assert(_Generic((A),  gg_num : true, unsigned int:true, int:true, long long:true, char:true, signed char:true, unsigned char:true, size_t: true, default: false),"Variable " #A " is not a number");
#define GG_IS_BOOL(A) _Static_assert(_Generic((A),  bool:true, int:true, default: false),"Variable " #A " is not a bool");
#define GG_IS_STRING(A) _Static_assert(_Generic((A),  char*:true, void*:true, default: false),"Variable " #A " is not a string");
//
#define GG_STRDUP(x, y) {char *_gg_temp = (y); (x) = gg_strdup (_gg_temp == NULL ? "" : _gg_temp); }
/*hexadecimal conversions*/
#define GG_CHAR_FROM_HEX(x) (((x)>'9') ? (((x)>='a') ? ((x)-'a'+10) : ((x)-'A'+10)) : ((x)-'0')) /* for conversion in URL - ASCII ONLY!
                        numbers are lower than capital letter are lower than lower case letters!! */
#define GG_TO_HEX(x) ((x) <= 9 ? '0' + (x) : 'a' - 10 + (x))
#define GG_HEX_FROM_BYTE(p,x) ((p)[0] = GG_TO_HEX(((x)&0xF0)>>4), (p)[1] = GG_TO_HEX((x)&0x0F))
#define GG_HEX_FROM_INT16(p,x) (GG_HEX_FROM_BYTE(p, (((x)&0xFF00)>>8)), GG_HEX_FROM_BYTE(p+2, ((x)&0xFF)))
#define GG_HEX_FROM_INT32(p,x) (GG_HEX_FROM_BYTE(p, (((x)&0xFF000000)>>24)), GG_HEX_FROM_BYTE(p+2, (((x)&0xFF0000)>>16)), GG_HEX_FROM_BYTE(p+4, (((x)&0xFF00)>>8)), GG_HEX_FROM_BYTE(p+6, ((x)&0xFF)))
/*end hexadecimal conversions*/
// Tree defines
#define GG_TREE_TYPE_STR 's'
#define GG_TREE_TYPE_NUM 'n'
#define GG_TREE_LESSER 0
#define GG_TREE_GREATER 1
#define GG_TREE_LESSER_LIST 0
#define GG_TREE_GREATER_LIST 1
#define gg_cur_key_len (gg_cursor->key_len)
#define gg_cur_key (gg_cursor->key)


// 
//
// Function declarations
//
//
bool gg_cm_str (char *a,char *b);
bool gg_cm_str_l (char *a,char *b, gg_num l);
bool gg_cm_str_c (char *a,char *b);
bool gg_cm_str_c_l (char *a,char *b, gg_num l);
bool gg_cm_num (gg_num a,gg_num b);
bool gg_cm_str_less (char *a,char *b);
bool gg_cm_str_less_l (char *a,char *b, gg_num l);
bool gg_cm_str_less_c (char *a,char *b);
bool gg_cm_str_less_c_l (char *a,char *b, gg_num l);
bool gg_cm_num_less (gg_num a,gg_num b);
bool gg_cm_str_lesseq (char *a,char *b);
bool gg_cm_str_lesseq_l (char *a,char *b, gg_num l);
bool gg_cm_str_lesseq_c (char *a,char *b);
bool gg_cm_str_lesseq_c_l (char *a,char *b, gg_num l);
bool gg_cm_num_lesseq (gg_num a,gg_num b);
bool gg_cm_str_greq (char *a,char *b);
bool gg_cm_str_greq_l (char *a,char *b, gg_num l);
bool gg_cm_str_greq_c (char *a,char *b);
bool gg_cm_str_greq_c_l (char *a,char *b, gg_num l);
bool gg_cm_num_greq (gg_num a,gg_num b);
bool gg_cm_str_gr (char *a,char *b);
bool gg_cm_str_gr_l (char *a,char *b, gg_num l);
bool gg_cm_str_gr_c (char *a,char *b);
bool gg_cm_str_gr_c_l (char *a,char *b, gg_num l);
bool gg_cm_num_gr (gg_num a,gg_num b);
bool gg_cm_bool (bool a,bool b);
char* gg_contain_str (char *a,char *b);
char* gg_contain_str_c (char *a,char *b);
char* gg_contain_int (char *a,int b);
char* gg_contain_int_c (char *a,int b);
bool gg_cm_err ();
void gg_init_input_req (gg_input_req *iu);
gg_num gg_open_trace ();
gg_num gg_close_trace();
void gg_make_SQL (char **dest, gg_num num_of_args, char *format, ...) __attribute__ ((format (printf, 3, 4)));
void gg_output_http_header(gg_input_req *iu);
void _gg_report_error (char *format, ...) __attribute__ ((format (printf, 1, 2)));
gg_num gg_encode (gg_num enc_type, char *v, gg_num vlen, char **res, bool alloc);
gg_num gg_get_input(gg_input_req *req, char *method, char *input);
void *gg_get_input_param (gg_num name_id, gg_num type);
gg_num gg_is_positive_num (char *s);
void gg_copy_string (char *src, gg_num from, char **dst, gg_num len);
void gg_alter_string (char *tgt, char *copy, gg_num swith, gg_num len, bool begin);
gg_num gg_exec_program (char *prg, char *argv[], gg_num num_args, FILE *fin, FILE **fout, FILE **ferr, char *inp, gg_num inp_len, char **out_buf, char **err_buf);
void gg_subs(char *s, void **call_handler);
void gg_get_debug_options();
gg_num gg_flush_printf(gg_num fin);
void gg_printf_close();
gg_num gg_printf (bool iserr, gg_num enc_type, char *format, ...) __attribute__ ((format (printf, 3, 4)));
void gg_shut(gg_input_req *giu);
FILE * gg_make_document (char **write_dir, gg_num is_temp);
char *gg_getenv (char *var);
char *gg_getenv_os (char *var);
void gg_putenv (char *env);
void gg_replace_all (gg_num v, char *look, char *subst);
void gg_current_time (char *outstr, gg_num out_str_len);
gg_config *gg_alloc_config();
void gg_init_config(gg_config *pc);
void gg_reset_config(gg_config *pc);
gg_num gg_count_substring (char *str, char *find, gg_num len_find, gg_num case_sensitive);
gg_num gg_replace_string (char *str, gg_num strsize, char *find, char *subst, gg_num all, char **last, gg_num case_sensitive);
void gg_trim (char *str, gg_num *len, bool alloc);
char *gg_trim_ptr (char *str, gg_num *len);
gg_num gg_file_type (char *dir);
gg_num gg_get_open_file_size(FILE *f);
gg_num gg_get_file_size(char *fn);
void gg_memory_init ();
void *gg_malloc(size_t size);
void *gg_calloc(size_t nmemb, size_t size);
void *gg_realloc(gg_num r, size_t size);
void gg_mem_add_ref (gg_num tgt_def, char *tgt, char *m);
void gg_mem_del_ref (gg_num r);
void gg_mem_set_process (char *m, bool force);
void gg_mem_release(gg_num r);
gg_num gg_get_memory_len (void *ptr);
gg_num gg_memid (void *ptr);
void gg_set_memory_len (void *ptr, gg_num len);
gg_num gg_mem_get_id (void *ptr);
void _gg_free (void *ptr, char check);
void gg_mem_set_len (gg_num r, gg_num len);
gg_num gg_mem_get_len (gg_num r);
char *gg_strdupl (char *s, gg_num from, gg_num l);
char *gg_strdup (char *s);
void *gg_mem_add_const (void *p, gg_num len);
void gg_mem_set_status (gg_num id, unsigned char s);
gg_num gg_add_mem (void *p);
void *gg_vmset (void *p, gg_num r);
gg_num gg_mem_items ();
gg_num gg_mem_size ();
void gg_done ();
void gg_get_stack(char *fname);
#if defined(GG_INC_MARIADB) || defined(GG_INC_POSTGRES)
gg_dbc *gg_get_db_connection (gg_num abort_if_bad);
#endif
void gg_close_db_conn ();
gg_num gg_begin_transaction(char *t, char erract, char **err, char **errt);
gg_num gg_commit(char *t, char erract, char **err, char **errt);
char *typename (gg_num type);
gg_num gg_rollback(char *t, char erract, char **err, char **errt);
void gg_get_insert_id(char *val, gg_num sizeVal);
void gg_select_table (char *s, gg_num *arow, gg_num *nrow, gg_num *ncol, char ***col_names, char ***data, gg_num **dlen, char **er, char **errm, char is_prep, void **prep, gg_num paramcount, char **params, char erract);
void _gg_trace(gg_num trace_level, const char *fromFile, gg_num fromLine, const char *fromFun, char *format, ...) __attribute__((format(printf, 5, 6)));
char *gg_hash_data( char *val, char *digest_name, bool binary);
char *gg_hmac (char *key, char *data, char *digest_name, bool binary);
char *gg_derive_key( char *val, gg_num val_len, char *digest_name, gg_num iter_count, char *salt, gg_num salt_len, gg_num key_len, bool binary );
gg_num gg_ws_util_read (void * rp, char *content, gg_num len);
gg_num gg_main (void *r);
gg_num gg_ws_printf (void *r, char *fmt, ...) __attribute__ ((format (printf, 2, 3)));
gg_num gg_write_web (bool iserr, gg_config *pc, char *s, gg_num nbyte);
void posix_print_stack_trace();
void gg_disable_output();
void gg_file_being_output();
void gg_set_crash_handler(char *dir);
void gg_dispatch_request();
gg_num gg_copy_data (char **data, char *value);
gg_num gg_puts_to_string (char *final_out, gg_num final_len);
char *gg_init_string(char *s);
gg_num gg_puts (gg_num enc_type, char *s, gg_num len, bool alloc);
gg_num gg_copy_data_at_offset (char **data, gg_num off, char *value);
gg_num gg_is_valid_param_name (char *name, bool hyphen, bool conv_hyphen);
void gg_write_to_string (char **str);
gg_num gg_write_to_string_length ();
void gg_write_to_string_notrim ();
gg_num _gg_check_memory(void *ptr);
bool gg_is_service();
void gg_set_cookie (gg_input_req *req, char *cookie_name, char *cookie_value, char *ypath, char *expires, char *samesite, char *httponly, char *secure);
char *gg_find_cookie (gg_input_req *req, char *cookie_name, gg_num *ind, char **path, char **exp);
gg_num gg_delete_cookie (gg_input_req *req, char *cookie_name, char *path, char *secure);
gg_num gg_decode (gg_num enc_type, char *v, gg_num inlen, bool alloc, gg_num *status);
char *gg_lower(char *s);
char *gg_upper(char *s);
void gg_location (char **fname, gg_num *lnum, gg_num set);
void gg_list_purge (gg_list **fdata_p);
gg_num gg_list_delete (gg_list *fdata);
gg_num gg_retrieve (gg_fifo *fdata, gg_num is_def_n, char **name, gg_num is_def_d, void **data);
void gg_list_retrieve (gg_list *fdata, char **name, void **data);
void gg_list_store (gg_list *fdata, char *name, void *data, bool append);
gg_num gg_list_pos(gg_list *fdata, int where, gg_list_item *item);
void gg_list_init (gg_list **fdata_ptr, bool process);
void gg_store_init (gg_fifo **fdata);
void gg_store (gg_fifo *fdata, char *name, void *data);
void gg_store_l (gg_fifo *fdata, char *name, void *data);
void gg_rewind (gg_fifo *fdata);
void gg_purge (gg_fifo **fdata);
void gg_fifo_delete (gg_fifo *fdata);
gg_num gg_lockfile(char *filepath, gg_num *lock_fd);
void gg_append_string (char *from, char **to);
void gg_get_runtime_options();
void gg_out_file (char *fname, gg_header *header);
gg_num gg_strncpy(char *dest, char *src, gg_num max_len);
gg_num gg_getpid ();
gg_num gg_post_url_with_response(char *url, char **result, char **head, char **error, char *cert, char *cookiejar, gg_num *resp_code, long timeout, char post, char *fields[], char *files[], gg_header *vh, char *method, char *payload, gg_num payload_len);
gg_num gg_copy_file (char *src, char *dst);
void gg_b64_decode (char* in, gg_num ilen, char** out);
void gg_b64_encode(char* in, gg_num in_len, char** out);
gg_num gg_read_file (char *name, char **data, gg_num pos, gg_num len);
gg_num gg_read_file_id (FILE *f, char **data, gg_num pos, gg_num len, bool ispos);
gg_num gg_is_number (char *s, gg_num *prec, gg_num *scale, gg_num *positive);
void gg_clear_config();
void gg_init_header (gg_header *header, gg_num init_type, char is_request);
gg_num gg_write_file (char *file_name, char *content, gg_num content_len, char append, gg_num pos, char ispos);
gg_num gg_write_file_id (FILE *f, char *content, gg_num content_len, char append, gg_num pos, char ispos);
gg_num gg_get_file_pos(FILE *f, gg_num *pos);
gg_num gg_set_file_pos(FILE *f, gg_num pos);
gg_num gg_reg_file(FILE **f);
void gg_set_json (gg_json **j, bool noenum);
void gg_del_json (gg_json **j);
char *gg_json_err();
gg_num gg_json_new (char *val, gg_num *curr, gg_num len, char dec);
char *gg_web_name(char *url);
void gg_check_transaction(gg_num check_mode);
void gg_break_down (char *value, char *delim, gg_split_str **broken);
void gg_delete_break_down (gg_split_str **broken_ptr);
char * gg_get_tz ();
gg_dbc *gg_execute_SQL (char *s,  gg_num *rows, char **er, char **err_message, gg_num returns_tuples, gg_num user_check, char is_prep, void **prep, gg_num paramcount, char **params, char erract);
char *gg_num2str (gg_num al, gg_num *res_len, int base);
char *gg_time (char *timezone, char *format, gg_num year, gg_num month, gg_num day, gg_num hour, gg_num min, gg_num sec);
gg_num gg_encode_base (gg_num enc_type, char *v, gg_num vLen, char **res, gg_num allocate_new);
void gg_make_random (char **rnd, gg_num rnd_len, char type, bool crypto);
void gg_checkmem ();
gg_num gg_copy_data_from_num (char **data, gg_num val);
void file_too_large(gg_input_req *iu, gg_num max_size);
void oops(gg_input_req *iu, char *err);
gg_num gg_total_so(gg_so_info **sos);
FILE *gg_fopen (char *file_name, char *mode);
int gg_fclose (FILE *f);
#ifdef GG_INC_PCRE2
gg_num gg_regex(char *look_here, char *find_this, char *replace, char **res, gg_num utf8, gg_num case_insensitive, gg_num single_match, regex_t **cached);
void gg_regfree(regex_t *preg);
#endif 
void gg_set_env(char *arg);
char * gg_os_name();
char * gg_os_version();
bool cmp_type (gg_num t1, gg_num t2);
char *gg_home_dir();
void gg_SERVICE_Finish (void);
gg_num gg_SERVICE_Accept (void);
void gg_exit_request(gg_num retval);
void gg_error_request(gg_num retval);
void after_handler();
void before_handler ();
char *gg_basename (char *path);
char *gg_realpath (char *path);
void gg_end_connection(gg_num close_db);
char *gg_find_keyword0(char *str, char *find, gg_num has_spaces, gg_num paren);
void gg_db_prep(void **prep);
char *gg_db_prep_text(char *t);
int gg_db_escape(char *from, char *to, gg_num *len);
void gg_hex2bin(char *src, char **dst, gg_num ilen);
void gg_bin2hex(char *src, char **dst, gg_num ilen, char *pref);
void gg_db_free_result (char is_prep);
gg_num gg_topower(gg_num b,gg_num p);
gg_num gg_decode_utf8 (gg_num32 u, unsigned char *r, char **e);
gg_num gg_encode_utf8 (char *r, gg_num32 *u, char **e);
gg_num32 gg_make_from_utf8_surrogate (gg_num32 u0, gg_num32 u1);
gg_num32 gg_get_hex(char *v, char **err);
void gg_get_utf8_surrogate (gg_num32 u, gg_num32 *u0, gg_num32 *u1);
void gg_create_hash (gg_hash **hres_ptr, gg_num size, gg_hash_table **in_h, bool process);
void gg_delete_hash (gg_hash **h, bool del);
void *gg_find_hash (gg_hash *h, char *key, char **keylist, bool del, gg_num *found);
void gg_add_hash (gg_hash *h, char *key, char **keylist, void *data, void **old_data, gg_num *st);
char *gg_next_hash(gg_hash *h, void **data, gg_num *st, bool del);
void gg_del_hash_traverse (gg_hash *h);
void gg_del_hash_entry (gg_hash *h, gg_hash_table *todel, gg_hash_table *prev, gg_num hashind, bool keydel);
void gg_rewind_hash(gg_hash *h);
gg_num gg_total_hash (gg_hash *h);
gg_num gg_hash_size (gg_hash *h);
void gg_resize_hash (gg_hash **h, gg_num newsize);
gg_num gg_hash_reads (gg_hash *h);
char *gg_text_to_utf8 (char *val, char quoted, char **o_errm, char dec, bool alloced);
gg_num gg_utf8_to_text (char *val, gg_num len, char **res, char **err);
char *gg_getheader(char *h);
void gg_bad_request ();
gg_num gg_set_input (gg_num name_id, void *val, gg_num type);
char *gg_getpath ();
int gg_fcgi_client_request (char *fcgi_server, char *req_method, char *path_info, char *script_name, char *content_type, int content_len, char *payload);
void gg_flush_out(void);
void gg_end_all_db();
void gg_exit (void);
char gg_decorate_path (char *reqname, gg_num reqname_len, char **p, gg_num p_len);
gg_num gg_str2num (char *str, int base, gg_num *st);
gg_num gg_read_msg(gg_msg *msg, char **key, char **val);
void gg_write_msg(gg_msg *msg, char *key, char *val);
void gg_del_msg(gg_msg *msg);
char *gg_get_msg(gg_msg *msg);
gg_msg * gg_new_msg (char *from);
void gg_sleepabit(gg_num milli);

gg_num gg_tree_bal (gg_tree_node *tree);
gg_tree *gg_tree_create(char key_type, bool sorted, bool process);
void gg_tree_insert_f (gg_tree_cursor *lcurs, gg_tree *orig_tree, char *key, gg_num key_len, void *data);
void gg_tree_search_f (gg_tree_cursor *lcurs, gg_tree *orig_tree, char *key, gg_num key_len);
void gg_tree_delete_f (gg_tree_cursor *lcurs, gg_tree *orig_tree, char *key);
void gg_tree_search_greater_equal_f (gg_tree_cursor *lcurs, gg_tree *orig_tree, bool equal, char *key, gg_num key_len);
void gg_tree_search_lesser_equal_f (gg_tree_cursor *lcurs, gg_tree *orig_tree, bool equal, char *key, gg_num key_len);
void gg_tree_max_f (gg_tree_cursor *lcurs, gg_tree *orig_tree);
void gg_tree_min_f (gg_tree_cursor *lcurs, gg_tree *orig_tree);
void gg_tree_purge_f (gg_tree *parent_tree);
void gg_tree_create_root (gg_tree *res, bool sorted);

#ifdef GG_INC_SERVICE
void gg_set_fcgi (gg_cli **callin, char *server, char *req_method, char *app_path, char *req, char *url_params, char *ctype, char *body, int clen, int timeout, char **env, bool simple_server, bool simple_url);
void gg_cli_delete (gg_cli *callin);
gg_num gg_call_fcgi (gg_cli **req, gg_num threads, gg_num *finokay, gg_num *started);
#endif

#ifdef GG_INC_CRYPTO
void gg_sec_load_algos(void);
gg_num gg_get_enc_key(char *password, char *salt, gg_num salt_len, gg_num iter_count, EVP_CIPHER_CTX *e_ctx, EVP_CIPHER_CTX *d_ctx,  char *cipher_name, char *digest_name);
char *gg_encrypt(EVP_CIPHER_CTX *e, const unsigned char *plaintext, gg_num *len, gg_num is_binary, unsigned char *iv);
char *gg_decrypt(EVP_CIPHER_CTX *e, unsigned char *ciphertext, gg_num *len, gg_num is_binary, unsigned char *iv);
int gg_RAND_bytes(unsigned char *buf, int num);
#endif

#ifdef GG_INC_POSTGRES
void gg_pg_close();
gg_num gg_pg_nfield();
gg_dbc *gg_pg_connect (gg_num abort_if_bad);
gg_num gg_pg_exec(char *s, gg_num returns_tuple, char is_prep, void **prep, gg_num paramcount, char **params);
gg_num gg_pg_affected();
char *gg_pg_fieldname(gg_num fnum);
void gg_pg_free();
gg_num gg_pg_nrows();
void gg_pg_rows(char ***row, gg_num num_fields, gg_num nrow, unsigned long **lens);
char *gg_pg_error(char *s);
char *gg_pg_errm(char *errm, gg_num errmsize, char *s, char *sname, gg_num lnum, char *er);
gg_num gg_pg_checkc();
void gg_pg_close_stmt (void *st);
int gg_pg_escape(char *from, char *to, gg_num *len);
#endif
#ifdef GG_INC_SQLITE
char *gg_lite_error(char *s, char is_prep);
void gg_lite_close ();
gg_dbc *gg_lite_connect (gg_num abort_if_bad);
void gg_lite_insert_id(char *val, gg_num sizeVal);
gg_num gg_lite_affected(char is_prep);
gg_num gg_lite_exec(char *s, char is_prep, void **prep, gg_num paramcount, char **params);
int gg_lite_store(char is_prep);
int gg_lite_use(char is_prep);
gg_num gg_lite_nfield();
char *gg_lite_fieldname();
void gg_lite_free(char is_prep);
gg_num gg_lite_nrows();
int gg_lite_rows (char ***row, unsigned long **lens);
char *gg_lite_errm(char *errm, gg_num errmsize, char *s, char *sname, gg_num lnum, char *er, char is_prep);
gg_num gg_lite_checkc();
void gg_lite_close_stmt (void *st);
int gg_lite_escape(char *from, char *to, gg_num *len);
#endif

#ifdef GG_INC_MARIADB
char *gg_maria_error(char *s, char is_prep);
void gg_maria_close ();
gg_dbc *gg_maria_connect (gg_num abort_if_bad);
void gg_maria_insert_id(char *val, gg_num sizeVal);
gg_num gg_maria_affected(char is_prep);
gg_num gg_maria_exec(char *s, char is_prep, void **prep, gg_num paramcount, char **params);
int gg_maria_store(char is_prep);
int gg_maria_use(char is_prep);
gg_num gg_maria_nfield();
char *gg_maria_fieldname();
void gg_maria_free();
gg_num gg_maria_nrows(char is_prep);
int gg_maria_rows (char ***row, unsigned long **lens, char is_prep);
char *gg_maria_errm(char *errm, gg_num errmsize, char *s, char *sname, gg_num lnum, char *er, char is_prep);
gg_num gg_maria_checkc();
void gg_maria_close_stmt (void *st);
int gg_maria_escape(char *from, char *to, gg_num *len);
#endif

// 
// Globals
//
// Empty string. Its own value is the pointer itself, which is used
// as a differentiator between allocated and un-allocated strings.
// A string that points to 'GG_EMPTY_STRING' is not allocated, and cannot be
// realloc-ed, otherwise it can.
//
extern char *GG_EMPTY_STRING;
extern gg_config *gg_pc;
extern jmp_buf gg_jmp_buffer;
extern volatile gg_num gg_done_setjmp;
extern jmp_buf gg_err_jmp_buffer;
extern volatile gg_num gg_done_err_setjmp;
extern volatile gg_num gg_in_fatal_exit;
extern char * gg_app_name;
extern char * gg_url_path;
extern char * gg_app_path;
extern unsigned long gg_app_path_len;
extern gg_num gg_max_upload;
extern gg_num gg_is_trace;
extern int gg_errno;
extern int gg_stmt_cached;
extern bool gg_mem_os;
extern bool gg_mem_process;
extern bool gg_mem_process_key;
extern bool gg_mem_process_data;
extern gg_hash gg_dispatch;
extern gg_hash gg_paramhash;
extern gg_tree_cursor *gg_cursor;
extern bool gg_true;
extern bool gg_false;
extern bool gg_optmem;

// DO not include golfapp.h for Golf itself, only for applications at source build time
#if GG_APPMAKE==1
// include generated Golf include file
#include "golfapp.h"
       
#endif

#endif





