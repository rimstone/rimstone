// SPDX-License-Identifier: Apache-2.0
// Copyright 2018-2025 Gliim LLC. 
// Licensed under Apache License v2. See LICENSE file.
// On the web http://golf-lang.com/ - this file is part of Golf framework. 

// 
// Main GOLF processor. Takes input and output parameters from the command line
// and generates C file ready for compilation.
//

#include "golf.h"


// 
//
// Defines (most of them)
//
//

// Maximum number of SQL queries in a single file, otherwise unlimited in application
// Queries's code is generated based on the name of the query, and not the index into an array.
// This limit is for internal code generation purposes.
#define GG_MAX_QUERY 300
// level of queries to allow for nesting (i.e. query inside a query)
#define GG_MAX_QUERY_NESTED 10
// maximum # of input parameters, '%s' in a query, per query
#define GG_MAX_QUERY_INPUTS 200
// maximum length of input parameter, be it C code, or a string. This is the length of what goes in the 
// SQL query input, per input.
#define GG_MAX_QUERY_INPUT_LEN 250
// maximum length of input line in a  source code file.
#define GG_FILE_LINE_LEN 65536
// maximum length of file name
#define GG_FILE_NAME_LEN 200
// maximum space to write out all output columns of a query
#define GG_TOT_COLNAMES_LEN (GG_MAX_COLNAME_LEN * GG_MAX_QUERY_OUTPUTS)
// maximym length of query name, which is the name in run-query...=
#define GG_MAX_QUERYNAME_LEN 200
// keep track up to 10 levels in if-true statement
#define GG_MAX_OTHER 31 
// maximum length of any error this utility may produce
#define GG_MAX_ERR_LEN 12000
// various keywords used. What's recognized is a keyword. Only keywords that have something following them
// have a space afterwards. No whitespace other than space can be used in commands!
#define GG_KEYMAXSIZE "max-size "
#define GG_KEYHASHSIZE "hash-size "
#define GG_KEYWITHVALUE "with-value "
#define GG_KEYREPLACE "replace"
#define GG_KEYTOERROR "to-error"
#define GG_KEYPARAM "param "
#define GG_KEYEXIT "exit"
#define GG_KEYHOME "home"
#define GG_KEYOPTIONS "options "
#define GG_KEYNEWLINE "new-line"
#define GG_KEYMODE "mode "
#define GG_KEYCREATE "create "
#define GG_KEYTYPE "type "
#define GG_KEYONERRORCONTINUE "on-error-continue"
#define GG_KEYONERROREXIT "on-error-exit"
#define GG_KEYADD "add "
#define GG_KEYON "on"
#define GG_KEYOFF "off"
#define GG_KEYERRNO "errno"
#define GG_KEYPREFIX "prefix "
#define GG_KEYNAME "name "
#define GG_KEYNAME0 "name"
#define GG_KEYSIZE "size "
#define GG_KEYEXTERNALCALL "external-call"
#define GG_KEYPUT "put"
#define GG_KEYGET "get "
#define GG_KEYSET "set "
#define GG_KEYBASE "base "
#define GG_KEYFIRST "first"
#define GG_KEYLAST "last"
#define GG_KEYEND "end"
#define GG_KEYPREVIOUS "previous"
#define GG_KEYNEXT "next"
#define GG_KEYKEY "key "
#define GG_KEYRETURNVALUE "return-value "
#define GG_KEYGETLESSER "get-lesser"
#define GG_KEYLESSER "lesser "
#define GG_KEYLESSERTHAN "lesser-than "
#define GG_KEYLESSEREQUAL "lesser-equal "
#define GG_KEYGETGREATER "get-greater"
#define GG_KEYCURRENT "current"
#define GG_KEYGREATER "greater "
#define GG_KEYCONTAIN "contain "
#define GG_KEYNOTCONTAIN "not-contain "
#define GG_KEYGREATERTHAN "greater-than "
#define GG_KEYEVERY "every "
#define GG_KEYSOURCEFILE "source-file"
#define GG_KEYSOURCELINE "source-line"
#define GG_KEYNOTEVERY "not-every "
#define GG_KEYGREATEREQUAL "greater-equal "
#define GG_KEYUPDATEKEY "update-key "
#define GG_KEYUPDATEVALUE "update-value "
#define GG_KEYKEYAS "key-as "
#define GG_KEYRUNDIR "run-dir"
#define GG_KEYUNSORTED "unsorted"
#define GG_KEYNOENUM "no-enum"
#define GG_KEYMAXKEY "max-key"
#define GG_KEYMINKEY "min-key"
#define GG_KEYKEYLIST "key-list "
#define GG_KEYVALUE "value "
#define GG_KEYNEWCURSOR "new-cursor "
#define GG_KEYIFVAR "if-true "
#define GG_KEYCOPY "copy "
#define GG_KEYCOPYEND "copy-end "
#define GG_KEYREPEAT "repeat "
#define GG_KEYREWIND "rewind "
#define GG_KEYPASSWORD "password "
#define GG_KEYPRIVATE "private"
#define GG_KEYPUBLIC "public"
#define GG_KEYINPUTLENGTH "input-length "
#define GG_KEYITERATIONS "iterations "
#define GG_KEYSALT "salt "
#define GG_KEYSALTLENGTH "salt-length "
#define GG_KEYINITVECTOR "init-vector "
#define GG_KEYBINARY "binary"
#define GG_KEYHTML "html "
#define GG_KEYPOSITION "position "
#define GG_KEYUSE "use "
#define GG_KEYWITH "with "
#define GG_KEYDELIMITER "delimiter "
#define GG_KEYDELETE0 "delete"
#define GG_KEYDELETE "delete "
#define GG_KEYNOLOOP "no-loop"
#define GG_KEYOLDVALUE "old-value "
#define GG_KEYCIPHER "cipher "
#define GG_KEYDIGEST "digest "
#define GG_KEYSKIPDATA "skip-data"
#define GG_KEYCRYPTO "crypto"
#define GG_KEYPROCESSSCOPE "process-scope"
#define GG_KEYSTARTWITH "start-with "
#define GG_KEYIN "in "
#define GG_KEYID "id "
#define GG_KEYTO "to "
#define GG_KEYISSERVICE "is-service"
#define GG_KEYSTATUS "status "
#define GG_KEYENDOFFILE "end-of-file "
#define GG_KEYFROMCONDITION "from-condition "
#define GG_KEYFROM "from "
#define GG_KEYFROMLENGTH "from-length "
#define GG_KEYTRAVERSE "traverse"
#define GG_KEYUNQUOTED "unquoted"
#define GG_KEYBEGIN "begin"
#define GG_KEYAPPEND "append"
#define GG_KEYLENGTH "length "
#define GG_KEYRESPONSE "response "
#define GG_KEYRESPONSECODE "response-code "
#define GG_KEYCERT "cert "
#define GG_KEYCOOKIEJAR "cookie-jar "
#define GG_KEYNUMBER "number"
// no-cert has NO space afterwards because it doesn't need anything to follow it
#define GG_KEYNOCERT "no-cert"
#define GG_KEYBUFFERED "buffered"
#define GG_KEYDEFAULT "default"
#define GG_KEYERROR "error "
#define GG_KEYERRORFILE "error-file "
#define GG_KEYERRORTEXT "error-text "
#define GG_KEYERRORPOSITION "error-position "
#define GG_KEYERRORLINE "error-line "
#define GG_KEYERRORCHAR  "error-char "
#define GG_KEYROWCOUNT "row-count "
#define GG_KEYAFFECTEDROWS "affected-rows "
#define GG_KEYNEWTRUNCATE "new-truncate"
#define GG_KEYSECURE "secure"
#define GG_KEYEXPIRES "expires "
#define GG_KEYAPPPATH "app-path "
#define GG_KEYPATH "path "
#define GG_KEYFILEID "file-id "
#define GG_KEYSAMESITE "same-site "
#define GG_KEYDIRECTORY "directory"
#define GG_KEYTRACEDIRECTORY "trace-directory"
#define GG_KEYROOTDIRECTORY "root-directory"
#define GG_KEYFILEDIRECTORY "file-directory"
#define GG_KEYUPLOADSIZE "upload-size"
#define GG_KEYDBVENDOR "db-vendor "
#define GG_KEYTRACEFILE "trace-file"
#define GG_KEYNOHTTPONLY "no-http-only"
#define GG_KEYOUTPUT "output "
#define GG_KEYMETHOD "method "
#define GG_KEYLOCATION "location "
#define GG_KEYLOCAL "local "
#define GG_KEYARGS "args "
#define GG_KEYAVERAGEREADS "average-reads "
#define GG_KEYOUTPUTFILE "output-file "
#define GG_KEYINPUTFILE "input-file "
#define GG_KEYSUBJECT "subject "
#define GG_KEYHEADER "header "
#define GG_KEYHEADERS "headers "
#define GG_KEYRESPONSEHEADERS "response-headers "
#define GG_KEYREQUESTHEADERS "request-headers "
#define GG_KEYREQUESTPATH "request-path "
#define GG_KEYFILES "files "
#define GG_KEYFIELDS "fields "
#define GG_KEYREQUESTBODY "request-body "
#define GG_KEYCONTINUE "continue"
#define GG_KEYCONTENT "content "
#define GG_KEYCONTENTTYPE "content-type "
#define GG_KEYCONTENTLENGTH "content-length "
#define GG_KEYURLPARAMS "url-params "
#define GG_KEYTHREADID "thread-id "
#define GG_KEYETAG "etag"
#define GG_KEYFILENAME "file-name "
#define GG_KEYNOCACHE "no-cache"
#define GG_KEYCACHECONTROL "cache-control "
#define GG_KEYCACHE "cache"
#define GG_KEYCLEARCACHE "clear-cache "
#define GG_KEYSTATUSID "status-id "
#define GG_KEYSTATUSTEXT "status-text "
#define GG_KEYCUSTOM "custom "
#define GG_KEYDOWNLOAD "download"
#define GG_KEYEQUAL "equal "
#define GG_KEYEQUALSHORT "="
#define GG_KEYCOMMA ","
#define GG_KEYCOMMENT "//"
#define GG_KEYCURLYLEFT "{"
#define GG_KEYCURLYRIGHT "}"
#define GG_KEYPARRIGHT ")"
#define GG_KEYPARLEFT "("
#define GG_KEYPLUS "+"
#define GG_KEYAT "@"
#define GG_KEYCOLON ":"
#define GG_KEYNOTEQUAL "not-equal "
#define GG_KEYAND0 "&&"
#define GG_KEYOR0 "||"
#define GG_KEYNOT0 "!"
#define GG_KEYAND "and "
#define GG_KEYOR "or "
#define GG_KEYNOT "not"
#define GG_KEYNOENCODE "noencode"
#define GG_KEYWEBENCODE "web-encode"
#define GG_KEYURLENCODE "url-encode"
#define GG_KEYURLPATH "url-path "
#define GG_KEYDATA "data "
#define GG_KEYEXITSTATUS "exit-status "
#define GG_KEYYEAR "year "
#define GG_KEYMONTH "month "
#define GG_KEYDAY "day "
#define GG_KEYHOUR "hour "
#define GG_KEYMIN "minute "
#define GG_KEYSEC "second "
#define GG_KEYTIMEOUT "timeout "
#define GG_KEYTIMEZONE "timezone "
#define GG_KEYFORMAT "format "
#define GG_KEYREPLACEWITH "replace-with "
#define GG_KEYRESULT "result "
#define GG_KEYNOTRIM "notrim"
#define GG_KEYCASEINSENSITIVE "case-insensitive"
#define GG_KEYUTF "utf"
#define GG_KEYTEMPORARY "temporary"
#define GG_KEYSINGLEMATCH "single-match"
#define GG_KEYENVIRONMENT "environment "
#define GG_KEYOSNAME "os-name"
#define GG_KEYOSVERSION "os-version"
#define GG_KEYARGCOUNT "arg-count"
#define GG_KEYCOUNT "count "
#define GG_KEYHOPS "hops "
#define GG_KEYARGVALUE "arg-value "
#define GG_KEYREFERRINGURL "referring-url"
#define GG_KEYPROCESSID "process-id"
#define GG_KEYCOOKIECOUNT "cookie-count"
#define GG_KEYCOOKIE "cookie "
#define GG_KEYINPUT "input "
#define GG_KEYFINISHEDOKAY "finished-okay "
#define GG_KEYSTARTED "started "
#define GG_KEYSINCEEPOCH "since-epoch"
#define GG_KEYFROMEPOCH "from-epoch "
// Input or output variable
#define GG_OUT 0
#define GG_IN 1
#define GG_INOUT 2
// maximum length of generated code line (in .c file, final line)
#define GG_MAX_CODE_LINE GG_FILE_LINE_LEN
// error messages
#define GG_STMT_FAIL_CHECK "File [%s], line [%ld], Statement failed without status being obtained and/or checked [%ld]"
#define GG_NAME_INVALID "Name [%s] is not valid, must be a valid identifier because a variable is required in this context, or this clause has already been specified"
#define GG_VAR_NOT_EXIST "Variable [%s] does not exist"
#define GG_MSG_NESTED_QRY "Qry ID [%ld] is nested too deep, maximum nesting of [%d]"
#define GG_MSG_SRC_NAME "Source file name [%s] is not valid. Use underscore and double underscore in source file name, and hyphen and forward slash in request name, respectively"
// maximum length of a column name in db select query result 
#define GG_MAX_COLNAME_LEN 64
// pre-processing status of a qry, describes the way it is used in the code
#define GG_QRY_UNUSED 0
#define GG_QRY_USED 1
#define GG_QRY_INACTIVE 0
#define GG_QRY_ACTIVE 1
// used as a silly placeholder to replace with actual length in gg_puts to increase output performance
#define GG_EMPTY_LONG_PLAIN_ZERO 0

//guard against if (..) gg_statement, causes if (..) char ...; which is illegal
#define GG_GUARD   oprintf("char _gg_statement_must_be_within_code_block_here_%ld; GG_UNUSED (_gg_statement_must_be_within_code_block_here_%ld);\n", vnone, vnone), vnone++;
// check if query returns a tuple, if not, cannot use row-count on it
#define GG_CHECK_TUPLE(k) if (gen_ctx->qry[k].returns_tuple == 0) gg_report_error( "row-count cannot be used on query [%s] because it does not output any columns", gen_ctx->qry[k].name)
// current query id - 0 for the first, 1 for nested in it, 2 for nested further, then 0 again for the one next to the first etc.
#define query_id (gen_ctx->curr_qry_ptr-1)

// cipher+digest for encryption (default)
#define GG_DEF_DIGEST "\"sha256\""
#define GG_DEF_CIPHER "\"aes-256-cbc\""

// modes of compilation
#define GG_MODE_SAFE 0
#define GG_MODE_EXTENDED 1
// string constant, is it, and if so, what kind, see is_constant_string()
#define GG_CONST_OK 0
#define GG_CONST_BAD 1
#define GG_CONST_NOT 2
#define GG_CONST_EMPTY 3
//
// Variables
// Decorated variable name max length
#define GG_MAX_DECNAME 200
// Max number of levels for code blocks
#define GG_MAX_CODE_LEVEL 30 
// Max clauses in a statement, that have no data. Used to check there's no extraneous data after them
#define GG_MAX_CLAUSE 50
// Backward-compatibility print functions (since 600.3.28)
#define GG_P_SOURCELINE 2
#define GG_P_SOURCEFILE 3
#define GG_P_NUM 4
#define GG_P_URL 5
#define GG_P_WEB 6
//
//when using undecorate , get just the part passed the flat source directory
#define GG_BLD_DIR_OFFSET gg_bld_dir_len + strlen (".flatsrc") + 1
//

//
//
// Type definitions
//
//

// Variable name and type and line number where defined
typedef struct s_gg_var
{
    char *name; // generated name
    gg_num type;
    gg_num lnum;
    bool is_process; // true if is process-scoped
    char *rname; // real variable name
    bool used; // true if variable asked for (i.e. used)
} gg_var;


// 
// Query structure, describes run-query and all that it entails
//
typedef struct gg_qry_info_t
{
    char *text; // sql text of query
    char name[GG_MAX_QUERYNAME_LEN + 1]; // name of query, generated by Golf and used for naming variables and such

    gg_num returns_tuple; // 0 if this doesn't return a tuple, like INSERT, but not INSERT..RETURNING(), 1 otherwise

    // 1 if unknown outputs
    char unknown_output;

    // number of, and qry outputs 
    gg_num qry_total_outputs; 
    char *qry_outputs[GG_MAX_QUERY_OUTPUTS + 1];
    
    // input parameters from URL bound to each query
    char qry_inputs[GG_MAX_QUERY_INPUTS + 1][GG_MAX_QUERY_INPUT_LEN + 1];

    // number of query inputs actually found as opposed to the number of '%s'
    gg_num qry_found_total_inputs; 

    // the database on which this index operates, used to switch back when end-query is encountered
    // note this is not spanning the code, but only set when needed. In other words, *every* db operation
    // has this set.
    gg_num ind_current_db;
} qry_info;
// 
// Context data for GOLF preprocessor. Used to hold information
// during the preprocessing.
//
typedef struct gg_gen_ctx_s
{
    // list of db queries - actual SQL with %s for input params
    // from URL
    qry_info qry[GG_MAX_QUERY + 1]; // used for generation code phase only

    gg_num qry_used[GG_MAX_QUERY + 1]; // whether the query was used in the past or not
    gg_num qry_active[GG_MAX_QUERY + 1]; // whether the query is active now or not
    gg_num total_queries; // total number of queries that we have. 
    gg_num total_write_string; // used to detect unclosed write-strings

    // when nesting, query IDs are indexes into qry_info.
    // curr_qry_ptr pointing to one just above the deepest. 
    gg_num curr_qry_ptr;


} gg_gen_ctx;

//
// Database connections
//
typedef struct connections_s
{
    char *name;
} connections;

// For use with run-query
// pointers to parsed out statement parts, must be all NULL to begin with (meaning
// they are not found by default),
// what each has is self-explanatory, for instance eq is the data right after = 
// we make sure no other option can ever be
// on_error_action is GG_ON_ERROR_CONTINUE/EXIT for statement-specific on-error continue/exit or GG_OKAY if db-level on-error is in effect.
//
typedef struct s_gg_db_parse {
    char on_error_action;
    char *on_error_exit;
    char *on_error_cont;
    char *with_output;
    char *colon;
    char *query_result;
    gg_num cur_query_result;
    char *eq;
    char *at;
    char *noloop;
    char *err;
    char *errtext;
    char *arows;
    char *rcount;
} gg_db_parse;

typedef struct s_gg_if {
    bool else_done; // true if else with no condition done, false otherwise
    gg_num num_elses; // how many elses in this if, need that many } closings, since else-if is "} else { if ()... }"
} gg_if;



// 
//
// Global variables (and a few related macros)
//
//

gg_num gg_curr_errors = 0; // current number of errors
gg_num gg_max_errors = 5; // max number of errors before giving up
bool gg_ignore_warn = false; // if true, do not display warnings
bool gg_single_file = false; // by default request handlers can be grouped in same-prefix files, with this being true, each must be in own file
bool gg_public = false; // by default all request handlers are private unless made public, with --public this flag will be switched to true
bool do_once_open = false; // true if do-once open
gg_num do_once_level; // the scope level at the beginning of do-once
char *emptyc[GG_MAX_CLAUSE]; // empty statement
gg_num emptyc_c = 0; // counter for finding empty clauses (0 in carve_statement)
char *oline = NULL;
gg_num oline_len = 0; // length of buffer
gg_num oline_prev_line_beg = 0; // beginning of prev line
gg_num oline_size = 0; // size in bytes written 
gg_num oline_written = 0; // current # of bytes written 
bool gg_is_sub = false; // true if this is request handler with call-handler clause
// Resurrection count for a level: each time code block level drops, resurr count increases
// this is to make sure "old" level N does not interfere with "new" level N
gg_num gg_resurr[GG_MAX_CODE_LEVEL+1];
gg_hash *gg_varh = NULL; // hash of variable names/types etc
gg_num gg_level = 0; // level of code blocks, used to decorate variable names
gg_num num_recog = 0; // number of recognized statements
bool after_started = false; // true if passed after-handler
bool before_started = false; // true if passed before-handler
bool after_ended = false; // true if passed end-after-handler
bool before_ended = false; // true if passed end-before-handler
gg_num gg_mode = GG_MODE_SAFE; // mode of compilation, depends which features are enfored
gg_num wcurs = 0; // id of a tree cursor
bool done_handler = false; // true if begin-handler is found
bool done_end_handler = false; // true if end-handler is found
gg_if if_nest[GG_MAX_OTHER+1] = {{false,0}}; // set all to false and 0
gg_num gg_plain_diag = 0; // use color diagnostics
gg_num vnone = 0; // used in detecting if () gg_statement
char line[GG_FILE_LINE_LEN + 1]; // buffer for each line from HTML file
gg_num open_loops = 0; // number of start-loop currently open
gg_num open_queries = 0; // number of queries currently open
gg_num gg_is_trace = 0; // is tracing 0 or 1
gg_num gg_max_upload = 25000000; // max upload default
char *app_path = ""; // app prefix not defined
char *gg_app_name=NULL;
char *gg_dbconf_dir = NULL;
char *gg_bld_dir = NULL;
int gg_bld_dir_len =0;
gg_num lnum = 0; // current line worked on 
char *src_file_name = NULL; // file_name of the source
gg_num print_mode = 0; // 1 if @  to print out
gg_num die_now = 0; // used in golf run-time only for now to gracefully exit program, not used here (not yet,
                // and maybe not ever).
FILE *outf = NULL; // golf output file (.c generated file)
gg_num usedGOLF = 0; // 1 if golf statement is used on the line
gg_num gg_is_inline = 0; // if 1, the last statement just processed was inline
gg_num within_inline = 0; // if statement is within <<...>>
gg_num last_line_readline_closed = 0; // line number of the last read-line that has closed
gg_num last_line_string_closed = 0; // line number of the last IF that has closed
gg_num last_line_number_closed = 0; // line number of the last IF that has closed
// setup variables to be able to report the location of unclosed readline
#define check_next_readline {if (open_readline==0) last_line_readline_closed = lnum; open_readline++;}
// setup variables to be able to report the location of unclosed IF
gg_num last_line_loop_closed = 0; // line number of where the last loop closed
gg_num last_line_query_closed = 0; // line number of where the last query closed
// setup variables to report the location of unclosed query
#define check_next_query {if (open_queries==0) last_line_query_closed = lnum; open_queries++;}
// setup variables to report the location of unclosed loop
#define check_next_loop {if (open_loops==0) last_line_loop_closed = lnum; open_loops++;}
gg_num verbose = 0; // 1 if verbose output of preprocessing steps is displayed
gg_num no_gg_line = 0; // if 1, no #line generated
gg_num total_exec_programs=0; // enumerates instance of exec-program so generated argument array variables are unique
gg_num total_body=0; // enumerates instance of call-web for body fields and files
gg_num total_fcgi_arr=0; // enumerates instance of new-remote for env
// Application name - we must know it in order to figure out database config file
// Database connections
gg_db_connections gg_dbs;
gg_num totconn = 0;
//
// satisfy gcc, this is for signal handling for SERVICE/standalone programs, no purpose in gg
gg_num gg_in_request=0;
gg_num volatile gg_done_err_setjmp=0;
gg_num volatile gg_in_fatal_exit=0;
gg_num volatile gg_done_setjmp=0;
char gg_query_id_str[GG_MAX_QUERYNAME_LEN + 1]; // query ID being worked on

//
//
// Function/macro declarations
//
//
void init_gg_gen_ctx (gg_gen_ctx *gen_ctx);
void gg_gen_c_code (gg_gen_ctx *gen_ctx, char *file_name);
gg_num recog_statement (char *cinp, gg_num pos, char *opt, char **mtext, gg_num *msize, gg_num isLast, gg_num *is_inline);
gg_num find_query (gg_gen_ctx *gen_ctx);
void new_query (gg_gen_ctx *gen_ctx, gg_db_parse *vp);
void oprintf (char *format, ...)  __attribute__ ((format (printf, 1, 2)));
void get_passed_whitespace (char **s);
void get_until_comma (char **s);
void get_until_whitespace (char **s);
void end_query (gg_gen_ctx *gen_ctx, gg_num close_block);
void get_all_input_param (gg_gen_ctx *gen_ctx, char *iparams);
gg_num terminal_width();
void out_verbose(gg_num gg_line, char *format, ...);
void add_input_param (gg_gen_ctx *gen_ctx, char *inp_par);
void carve_statement (char **statement, char *statement_name, char *keyword, gg_num is_mandatory, gg_num has_data);
gg_num define_statement (char **statement, gg_num type, bool always);
// true if to emit line number in source file, do not do it if golf -x used, lnum>1 is so that the very first line
// isn't emitting #line twice
#define GG_EMIT_LINE (no_gg_line == 0 && lnum>1)
#define  GG_VERBOSE(lnum,...) out_verbose(lnum,  __VA_ARGS__)
void parse_param_list (char *parse_list, gg_fifo **params, gg_num *tot);
char *find_keyword(char *str, char *find, gg_num has_spaces);
gg_num find_connection(char *conn);
gg_num add_connection(char *conn, gg_num dbtype);
void get_db_config (char *dbname);
void get_query_output (gg_gen_ctx *gen_ctx, gg_db_parse *vp);
void statement_SQL (gg_gen_ctx *gen_ctx, char is_prep);
void start_loop_query (gg_gen_ctx *gen_ctx);
void generate_sql_code (gg_gen_ctx *gen_ctx, char is_prep);
void generate_query_result (gg_gen_ctx *gen_ctx, gg_db_parse *vp);
void query_execution (gg_gen_ctx *gen_ctx,const gg_num run_query, const gg_num query_result, char is_prep, gg_db_parse *vp);
void prepare_query (gg_gen_ctx *gen_ctx, gg_db_parse *vp);
void process_query (gg_gen_ctx *gen_ctx, const gg_num query_result, const gg_num run_query, char is_prep, gg_db_parse *vp);
char *gg_db_vendor(gg_num dbconn);
gg_num trimit(char *var);
void check_golf (char *c);
char *id_from_file (char *file);
gg_num outargs(char *args, char *outname, char *type, gg_num startwith, char pair, gg_num check_type_val, gg_num check_type_name);
void process_http_header (char *statement, char *header, char *temp_header, gg_num http_header_count, char request, char **content_len, char **content_type);
char *make_default_header(int inittype, gg_num http_header_count, char request);
void on_error_act (char *on_error_cont, char *on_error_exit, char *act);
void envsub ();
void gg_is_valid_app_path (char *name);
void query_result (gg_gen_ctx *gen_ctx, char *mtext, gg_num cur_query_result);
char *opt_clause(char *clause, char *param, char *antiparam, gg_num type);
void name_query (gg_gen_ctx *gen_ctx);
void free_query (char *qryname, bool skip_data);
void convert_puts(char *pline);
void do_numstr (char *to, char *num0, char *base);
void setup_internal_hash(char *fname, char *hash, bool is_req);
void gg_report_warning (char *format, ...)  __attribute__ ((format (printf, 1, 2)));
gg_num carve_stmt_obj (char **mtext, bool has_value, bool is_constant);
void match_file (char *file_name, char *reqname);
void parse_cond(char *cm, char *bifs, char *ifs, char *ife);
void make_mem (char **clause);
void make_unq (char **clause);
void new_service (char *mtext);
void read_service (char *mtext);
void call_service (char *mtext);
void call_service_find (char *mtext, char **finok, char **started);
void call_service_carve (char **finok, char **started);
void call_service_define(char **finok, char **started);
void call_service_impl (char *st, char *finok, char *started, char *mtext);
void read_service_find(char *mtext, char **data, char **error, char **rmsg, char **rstatus);
void read_service_carve(char **data, char **error, char **rmsg, char **rstatus);
void read_service_define(char **data, char **error, char **rmsg, char **rstatus);
void read_service_impl(char *mtext, char *data, char *error, char *st, char *rmsg, char *rstatus);
void new_service_find(char *mtext, char **server, char **method, char **apath, char **req, char **body, char **upay, char **timeout, char **env, char **loc, char **url);
void new_service_carve(char **server, char **method, char **apath, char **req, char **body, char **upay, char **timeout, char **env, char **loc, char **url);
void new_service_impl (char *mtext, char *server, char *method, char *apath, char *req, char *body, char *upay, char *loc, char *url, char *env, char *timeout);
void run_service (char *mtext);
gg_num check_var (char **v, gg_num type);
void check_format(char *mtext, char *comma, char **list);
gg_num is_constant_string (char *s);
gg_num is_len_format (char *s, char *f, gg_num flen);
void check_level(char *prline);
gg_num typeid (char *type);
bool add_var (char *var, gg_num type, char *realname, bool watched_for_unused);
bool find_var (char *name, gg_num *type, gg_num *line, char **realname, bool *is_process);
void make_var (char **v, gg_num type);
void check_c (char *mtext);
char *check_bool (char *e, bool *found, bool *err);
char *check_exp (char *e, bool *found, bool *err);
char *check_str (char *e, bool *err);
void save_param (char *pname);
void check_sub (char *sub);
char *undecorate (char *src);
void do_print (char *mtext, gg_num backmode);
void do_printf (char *mtext, bool rep_error, bool trace_none, bool trace, gg_gen_ctx *gen_ctx, gg_num backmode);
bool is_reserved (char *word);
bool var_exists (char *name, gg_num type, gg_num *real_type, gg_num *lnum);
char *process_bool_exp (char *v);
char *process_string_exp (char *v);
char *process_number_exp (char *v);

//
//
// Implementation of functions used in GOLF alone
//
//

//
// Check if variable 'name' exists and is of type 'type', returns true if it does, false if it doesn't exist or not of right type
// If it doesn't exist real_type is GG_DEFUNKN; it can be NULL.
// If it exists, real_type is its real type, and lnum is where it's defined; both can be NULL.
//
bool var_exists (char *name, gg_num type, gg_num *real_type, gg_num *lnum)
{
    gg_num vtype;
    gg_num l;
    char *rname;
    bool found = find_var (name, &vtype, &l, &rname, NULL);
    if (found)
    {
        if (!cmp_type (type, vtype)) 
        {
            if (real_type) *real_type = vtype;
            if (lnum) *lnum = l;
            return false; 
        } else 
        {
            if (real_type) *real_type = vtype; // if we're looking for GG_DEFSTRING but get GG_DEFSTRINGSTATIC for instance!!!
                                               // so we know which one we got!
            return true;
        }
    }
    else 
    {
        if (real_type) *real_type = GG_DEFUNKN;
        return false;
    }
}


//
// Returns true if word is a reserved C words, those can't be used as names for anything
//
bool is_reserved (char *word)
{
    // NOTE: if updating here, update in gg when building 'void' list of function prototypes
    char *keys[] = {
        "auto", "break", "bool", "case", "char", "const", "continue", "default", "do",
        "double", "else", "enum", "extern", "float", "for", "goto", "if",
        "int", "long", "register", "return", "short", "signed", "sizeof",
        "static", "struct", "switch", "typedef", "union", "unsigned", "void",
        "volatile", "while", "main"
    };
    int numkeys = sizeof(keys) / sizeof(char*);

    gg_num i;
    for (i = 0; i < numkeys; i++) {
        if (strcmp(word, keys[i]) == 0) return true;
    }
    return false;
}



//
//
// Perform print-format statement. This is encapsulated like this so that old-style statements (pf-out, etc.) will still work.
// mtext is the statement's text (minus the statement name itself)
// backmode is 0, if this is new print-out (or >0) otherwise, see below
// rep_error is true if this is report-error, trace is true if this is trace, trace_none is true if just trace without any params.
// gen_ctx is the compiler context.
//
//
void do_printf (char *mtext, bool rep_error, bool trace_none, bool trace, gg_gen_ctx *gen_ctx, gg_num backmode)
{


    // Example:
    // printf "%s %ld", str, int
    // this is no-enc printout

    //printf statements must have a comma, so instead of printf-url "xxx" it must be printf-url "%s",xxx
    //to avoid hard to track bugs
    //look also for format (starting with unquoted quote), to know where the printf part starts
    char *comma = find_keyword (mtext, GG_KEYCOMMA, 0);
    char *toerr = find_keyword (mtext, GG_KEYTOERROR, 1);
    char *to = find_keyword (mtext, GG_KEYTO, 1);
    char *webe = find_keyword (mtext, GG_KEYWEBENCODE, 1);
    char *urle = find_keyword (mtext, GG_KEYURLENCODE, 1);


    carve_statement (&toerr, "print-format", GG_KEYTOERROR, 0, 0);
    carve_statement (&to, "print-format", GG_KEYTO, 0, 1);
    carve_statement (&comma, "print-format", GG_KEYCOMMA, 0, 1);
    carve_statement (&webe, "print-format", GG_KEYWEBENCODE, 0, 0);
    carve_statement (&urle, "print-format", GG_KEYURLENCODE, 0, 0);
    //
    // Simulate various parts of statements, if they are not there but we need them
    if (backmode != 0)
    {
        if (backmode == GG_P_URL) urle=GG_KEYURLENCODE;
        else if (backmode == GG_P_WEB) webe=GG_KEYWEBENCODE;
        else gg_report_error ("Unknown backward compatibility mode for print-out (%ld)", backmode);
    }
    //
    // printf to stdout for standalone app, or to web/string otherwise 
    // Simulate various parts of statements, if they are not there but we need them
    gg_num c_printf_web = (webe != NULL ? 1:0);
    gg_num c_printf_url = (urle != NULL ? 1:0);
    gg_num c_printf = ((c_printf_web == 0 && c_printf_url == 0) ? 1:0);

    if (rep_error && (to != NULL || toerr != NULL)) gg_report_error ("Cannot use to-error or 'to' clause with report-error statement");
    if ((trace || trace_none) && (to != NULL || toerr != NULL)) gg_report_error ("Cannot use to-error or 'to' clause with trace statement");


    // check it's a string constant
    gg_num flen = trimit (mtext);
    if (!trace_none && (mtext[0] != '"' || mtext[flen-1] != '"')) gg_report_error ("format must be a string literal, found [%s]", mtext);

    if (comma == NULL && !trace_none) {comma = mtext; mtext=gg_strdup("\"%s\"");} // make up format for gcc, format must be dynamic as it will be carved
    char *format = mtext; // since mtext will become a generated variable below in carve_stmt_obj
    carve_stmt_obj (&mtext, trace_none?false:true, true); // mtext is format
    if (trace) oprintf("GG_UNUSED(%s);\n", mtext); // if trace with some string, this string will be unused if not DEBUG


    if (toerr != NULL && to != NULL) gg_report_error( "cannot use both to-error and 'to' clause; use one or the other");
    define_statement (&to, GG_DEFSTRING, false); // exact length is set by gg_write_to_string
    //
    if (!trace_none) check_var (&mtext, GG_DEFSTRING);

    //Check only %s or %ld

    char *list;

    //
    // Look for each option and collect relevant info
    // First we MUST get each options position
    if (trace_none)
    {
        oprintf("GG_TRACE(\"\");\n");
    }
    else if (trace)
    {
        char *list;
        check_format(format, comma, &list);
        oprintf("GG_TRACE(%s", mtext);
        oprintf("%s", list);
        oprintf(");\n");
        gg_free (list);
    }
    else if (rep_error)
    {
        check_format(format, comma, &list);
        oprintf("gg_report_error(%s", mtext);
        oprintf("%s", list);
        oprintf(");\n");
        gg_free (list);
    }
    else if (to == NULL) 
    {
        check_format(format, comma, &list);
        oprintf ("gg_printf(%s, %s, %s", toerr!=NULL?"true":"false", rep_error?"GG_NOENC":(c_printf!=0?"GG_NOENC":(c_printf_web!=0?"GG_WEB":"GG_URL")),   mtext);
        oprintf("%s", list);
        oprintf(");\n");
        gg_free (list);
    }
    else
    {
        // this is ... to [ define ] <str>
        //
        oprintf("gg_write_to_string (&(%s));\n", to);
        gen_ctx->total_write_string++;
        //toerr is guaranteed to be NULL here, see above
        check_format(format, comma, &list);
        oprintf ("gg_printf(true, %s, %s", c_printf!=0?"GG_NOENC":(c_printf_web!=0?"GG_WEB":"GG_URL"),mtext);
        oprintf("%s", list);
        oprintf(");\n");
        gg_free (list);
        oprintf("gg_write_to_string_notrim();\n"); 
        oprintf("gg_write_to_string (NULL);\n"); 
        gen_ctx->total_write_string--;
    }
}


//
//
// Perform print-out statement. This is encapsulated like this so that old-style statements (p-out, etc.) will still work.
// mtext is the statement's text (minus the statement name itself)
// backmode is 0, if this is new print-out (or >0) otherwise, see below
//
//
void do_print (char *mtext, gg_num backmode)
{
    char *length = find_keyword (mtext, GG_KEYLENGTH, 1);
    char *nl = find_keyword (mtext, GG_KEYNEWLINE, 1); // can be used with any other clause
    char *sfile = find_keyword (mtext, GG_KEYSOURCEFILE, 1);
    char *sline = find_keyword (mtext, GG_KEYSOURCELINE, 1);
    char *webe = find_keyword (mtext, GG_KEYWEBENCODE, 1);
    char *urle = find_keyword (mtext, GG_KEYURLENCODE, 1);

    carve_statement (&nl, "print-out", GG_KEYNEWLINE, 0, 0);
    carve_statement (&sfile, "print-out", GG_KEYSOURCEFILE, 0, 0);
    carve_statement (&sline, "print-out", GG_KEYSOURCELINE, 0, 0);
    carve_statement (&webe, "print-out", GG_KEYWEBENCODE, 0, 0);
    carve_statement (&urle, "print-out", GG_KEYURLENCODE, 0, 0);
    carve_statement (&length, "print-out", GG_KEYLENGTH, 0, 1);
    //
    // Simulate various parts of statements, if they are not there but we need them
    if (backmode != 0)
    {
        if (backmode == GG_P_SOURCELINE) sline=GG_KEYSOURCELINE;
        else if (backmode == GG_P_SOURCEFILE) sfile=GG_KEYSOURCEFILE;
        else if (backmode == GG_P_NUM) ;
        else if (backmode == GG_P_URL) urle=GG_KEYURLENCODE;
        else if (backmode == GG_P_WEB) webe=GG_KEYWEBENCODE;
        else gg_report_error ("Unknown backward compatibility mode for print-out (%ld)", backmode);
    }

    if (webe != NULL && (sfile != NULL || sline != NULL || urle != NULL)) gg_report_error ("Cannot use web-encode clause with other clauses");
    if (urle != NULL && (sfile != NULL || sline != NULL || webe != NULL)) gg_report_error ("Cannot use url-encode clause with other clauses");
    if (length != NULL && (sfile != NULL || sline != NULL)) gg_report_error ("Cannot use length clause with other clauses, except url-encode and web-encode");

    if (sfile != NULL)
    {
        if (sline != NULL || webe != NULL || urle != NULL) gg_report_error ("Cannot use source-file clause with other clauses");
        carve_stmt_obj (&mtext, false, false);
        char *osrc = undecorate (src_file_name+GG_BLD_DIR_OFFSET);
        oprintf("gg_puts (GG_NOENC, \"%s\", %ld, false);\n", osrc, strlen(osrc)); // optimized to compute strlen at compile time
        gg_free (osrc);
        if (nl != NULL) oprintf("gg_puts (GG_NOENC, \"\\n\", 1, false);\n"); // output 1 byte non-alloc'd
    }
    else if (sline != NULL)
    {
        if (sfile != NULL || webe != NULL || urle != NULL) gg_report_error ("Cannot use source-line clause with other clauses");
        carve_stmt_obj (&mtext, false, false);
        do_numstr (NULL, "__LINE__",  NULL);
        if (nl != NULL) oprintf("gg_puts (GG_NOENC, \"\\n\", 1, false);\n"); // output 1 byte non-alloc'd
    }
    else
    {
        gg_num t = check_var (&mtext, GG_DEFUNKN);
        if (cmp_type (t, GG_DEFNUMBER))
        {
            if (webe != NULL || sfile != NULL || sline != NULL || urle != NULL || length != NULL) gg_report_error ("Cannot use other clauses (except new-line) when outputting a number");
            carve_stmt_obj (&mtext, true, true);
            do_numstr (NULL, mtext, NULL);
            if (nl != NULL) oprintf("gg_puts (GG_NOENC, \"\\n\", 1, false);\n"); // output 1 byte non-alloc'd
        }
        // print-out assumes it's a string if internal mode!
        else if (cmp_type (t, GG_DEFSTRING))
        {
            if (sfile != NULL || sline != NULL) gg_report_error ("Cannot use other clauses (except new-line, length, web-encode and url-encode) when outputting a string");
            if (webe != NULL && urle != NULL) gg_report_error ("Cannot use both url-encode and web-encode");
            carve_stmt_obj (&mtext, true, true);
            check_var (&mtext, GG_DEFSTRING);
            check_var (&length, GG_DEFNUMBER);
            if (length == NULL) length = "GG_EMPTY_LONG_PLAIN_ZERO";

            if (urle != NULL) oprintf("gg_puts (GG_URL, %s, %s, true);\n", mtext, length); 
            else if (webe != NULL) oprintf("gg_puts (GG_WEB, %s, %s, true);\n", mtext, length); 
            else oprintf("gg_puts (GG_NOENC, %s, %s, true);\n", mtext, length); 
            if (nl != NULL) oprintf("gg_puts (GG_NOENC, \"\\n\", 1, false);\n"); // output 1 byte non-alloc'd
        }
        else if (cmp_type (t, GG_DEFUNKN))
        {
            gg_report_error( "Unsupported type in print-out");
        }
    }
}


//
//
//
// Replace __ with / and then _ with - in string, result should be freed by caller
//
//
char *undecorate (char *src)
{
    char *r_src = gg_strdup(src);
    gg_num len = gg_replace_string (r_src, strlen(r_src)+1, "__", "/", 1, NULL, 1);
    gg_replace_string (r_src, len+1, "_", "-", 1, NULL, 1);
    return r_src;
}

//
//
// Check if this sub is a constant and if it is, check if it exists
// sub is the name of request handler
//
//
void check_sub (char *sub)
{
    // trim request path 
    gg_num ml = strlen (sub);
    sub = gg_trim_ptr(sub,  &ml);
    // check if string - TODO: this won't take "xx" "yy", must be single constant!
    if (is_constant_string (sub) == GG_CONST_OK)
    {
        // .reqlist file contains the list of all requests and we check to see if this request is there
        char rn[GG_MAX_REQ_NAME_LEN];
        // check length of request
        size_t len = strlen (sub)-1; // -1 to account that we will skip leading "
        // skip " in the beginning
        char *p = sub+1;
        // 
        // remove trailing ", we will restore it
        size_t zero;
        p[zero = len-1] = 0; // remove last ", we will restore it
        len--; // account we take out the last "
        //
        // get file name without .golf
        char *q = p; // p may be NULL after decorating below
        if (gg_decorate_path (rn, sizeof(rn), &q, len) != 1) gg_report_error( "request path [%s] is not valid", p);
        //
        // restore "
        p[zero] = '"'; // restore last "" because we don't want sub changed after this function
        //
        char gg_req_file[300];
        snprintf (gg_req_file, sizeof(gg_req_file), "%s/.reqlist", gg_bld_dir);
        FILE *f = fopen (gg_req_file, "r");
        if (f == NULL) gg_report_error( "Error [%s] opening file [%s]", strerror (errno), gg_req_file);
    
        char line[GG_MAX_REQ_NAME_LEN]; // line to read from fname
        bool done_count = false; // true when first line (count of requests) read
        bool exist = false;
        while (1)
        {
            char *req = fgets_unlocked (line, sizeof (line), f);
            if (req == NULL) // either end or error
            {
                gg_num err = ferror_unlocked (f);
                if (err) gg_report_error( "Error [%s] reading file [%s]", strerror (errno), gg_req_file);
                break; // nothing read in, done 
            }
            trimit (req);     
            if (!done_count) 
            {
                done_count = true; // atol(req) is # of lines
                continue;
            }
            else
            {
                if (!strcmp (req, rn)) { exist = true; break;}
            }
        }
        fclose(f);
        if (!exist) gg_report_error ("Request [%s] does not exist", sub);
    }
}

//
//
// Save the name of a parameter for an application-wide list of parameters, which then will be indexed in a fixed fashion,
// so that there's no "searching" for a parameter, rather we compile it as a direct memory access, making it very very fast.
// The index list will be generated in gg
// pname is the name of a parameter. A parameter with the same name can be used unrelated in different requests, but a process
// does only one request at a time, so each such parameter can have just one index in the list of params.
//
//
void save_param (char *pname)
{
    gg_num plen = strlen (pname);
    if (pname[0] == '(') *pname = ' ';
    if (pname[plen-1] == ')') pname[plen-1] = ' ';
    gg_trim (pname, &plen, false);
    char gg_sparam_file[300];
    char *base = gg_basename (src_file_name);
    snprintf (gg_sparam_file, sizeof(gg_sparam_file), "%s/.setparam/%s.sp", gg_bld_dir, base);
    FILE *f = fopen (gg_sparam_file, "a");
    if (f == NULL) gg_report_error ("Cannot open file [%s]", gg_sparam_file);
    fprintf (f, "%s\n", pname);
    fclose(f);
    gg_free (base);
}


//
// Recursively parse number expression for validity, and check if number variables in it are proper
// Emits messages if errors. 
// cur is the expression, found is used internally (to make sure expressions are not empty), and err is true
// if there's a syntax error or false if not.
// Returns current point in parsing, not useful to caller, only err is useful.
//
char *check_exp (char *cur, bool *found, bool *err)
{
    *found = false;
    *err = false;
    while (isspace(*cur)) cur++;
    if (*cur == '(') 
    {
        // First find enclosed expressions
        cur = check_exp (cur+1, found, err); if (*err) return ""; if (!*found) { *err = true; return "";}
        if (*cur != ')') {*err = true; return "";}
        cur++;
    }
    else
    {

        // Find begin token of an expression
        if (*cur == '-') 
        {
            cur = check_exp (cur+1, found, err); if (*err) return ""; if (!*found) { *err = true; return "";}
        }
        else if (isalpha(*cur) || *cur == '_') 
        {
            *found = true;
            gg_num vtype;
            gg_num l;
            char *rname;
            // allow string[] notation for an integer expression
            // if it is, then remove [] and verify that index is integer, and allow this as a valid C expression (that evaluates to int)
            char *ind;
            char *open_p = cur+1; // +1 since we know *cur is alpha or _
            while (isalnum (*open_p)) open_p++; // search for anything other than name, if found '[', look for index
                                                // if not, move to the next possibility
                                                // here we use isalnum, since variable name cannot start with a digit but can contain it
            if (*open_p == '[')
            {
                // this is number = str[index]
                ind = open_p + 1;
                *open_p = 0;
                char *close_p = ind;
                // this now looks for str[str[str[a]]] and such and returns position of only the last ]
                gg_num p_open = 1; // current [
                while (*close_p != 0) 
                {
                    // skip any 'X', which could be '[' or ']' that can mess up this calculation
                    if (*close_p == '\'') 
                    {
                        close_p++;
                        if (*close_p == 0) gg_report_error ("Syntax error in character constant");
                        close_p++;
                        if (*close_p != '\'') gg_report_error ("Syntax error in character constant");
                        close_p++;
                        continue;
                    }
                    if (*close_p == '[') p_open ++;
                    if (*close_p == ']') 
                    {
                        p_open --;
                        if (p_open == 0) break;
                    }
                    close_p++;
                }
                if (*close_p == 0) gg_report_error ("Syntax error in index, expected ']'");
                *close_p = 0;
                // check that index is a number
                bool sub_found;
                bool sub_err;
                check_exp (ind, &sub_found, &sub_err);
                if (sub_err || !sub_found) 
                {
                    // restore text of expression so error reporting shows it
                    *open_p = '[';
                    *close_p = ']';
                    *err =  true; *found = false; return "";
                }
                // check that variable is a string
                bool str_found = find_var (cur, &vtype, &l, &rname, NULL);
                if (!str_found) 
                {
                    gg_report_error ("Variable [%s] is not found", rname);
                }
                if (!cmp_type (vtype, GG_DEFSTRING)) 
                {
                    gg_report_error ("Variable [%s] is not a string", rname);
                }
                // check at run time accessing this byte is valid memory, this is in order
                // so the deepest expressions are checked first, as it should be
                oprintf ("if (gg_mem_get_len(gg_mem_get_id(%s))-1 < (%s) || (%s) < 0) gg_report_error (\"Cannot access byte [%%ld] in string\", (long)(%s));\n", cur, ind, ind, ind);
                // restore []
                *open_p = '[';
                *close_p = ']';
                cur = close_p + 1; // move passed []
            }
            else
            {
                char *vname = cur;
                cur++;
                while (isalnum(*cur) || *cur == '_') cur++;
                // get var name
                char *name = gg_strdupl (vname, 0, cur-vname);
                name[cur-vname] = 0; // end string
                bool found = find_var (name, &vtype, &l, &rname, NULL);
                if (!found) 
                {
                    gg_report_error ("Variable [%s] is not found", rname);
                }
                if (!cmp_type (vtype, GG_DEFNUMBER)) 
                {
                    gg_report_error ("Variable [%s] is not a number", rname);
                }
            }
        }
        else if (isdigit (*cur))
        {
            *found = true;
            cur++;
            // account for hex numbers as well, we're 1 offset ahead, so -1 is appropriate as an index
            bool is_hex = false;
            if (*(cur-1) == '0' && (*cur == 'x' || *cur == 'X')) {is_hex = true; cur+=1;} // continue syntax check after 0x
            if (!is_hex)
            {
                while (isdigit(*cur)) cur++;
            }
            else
            {
                while (isxdigit(*cur) ) cur++;
            }
        } 
        else if (*cur == '\'')
        {
            cur++;
            if (*cur == 0) gg_report_error ("Incomplete character after single quote"); 
            if (*cur == '\\')  // this is say '\n'
            {
                cur++;
                if (!(*cur == 'a' || *cur == 'b' || *cur == 'f' || *cur == 'n' || *cur == 'r' || *cur == 't' || *cur == 'v' || *cur == '\\' || *cur == '\'')) gg_report_error ("Invalid escape sequence (\\%c)", *cur); 
                cur++;
            } else cur++;
            if (*cur != '\'') gg_report_error ("Expected single quote");
            cur++;
            *found = true;
        }
    }
    while (isspace(*cur)) cur++;

    // Once token found, find if it's connected to another token
    if (*cur == '+' || *cur == '-' || *cur == '*' || *cur == '/' || *cur == '%') 
    {
        if (!*found) { *err = true;; return "";} // something must have been found prior to operators
        cur = check_exp(cur+1, found, err); if (*err) return ""; if (!*found) { *err = true; return "";}
    }

    if (*found && *cur != ')' && *cur != 0) *err = true; // if there's something and it's not followed by an operator 
                                                         // what follows must be either ) or nothing

    return cur;
    
}


//
// Parse boolean expression using !, && and || (as well as parenthesis)
// Returns current point in parsing, not useful to caller, only err is useful.
//
char *check_bool (char *cur, bool *found, bool *err)
{
    *found = false;
    *err = false;
    while (isspace(*cur)) cur++;

    if (*cur == '(') 
    {
        // First find enclosed expressions
        cur = check_bool (cur+1, found, err); if (*err) return ""; if (!*found) { *err = true; return "";}
        if (*cur != ')') {*err = true; return "";}
        cur++;
        while (isspace(*cur)) cur++;
    }
    else
    {
        // Find begin token of an expression
        if (cur[0] == '!') 
        {
            cur = check_bool (cur+1, found, err); if (*err) return ""; if (!*found) { *err = true; return "";}
        }
        else if (isalpha(*cur) || *cur == '_') 
        {
            *found = true;
            gg_num vtype;
            gg_num l;
            char *rname;
            char *vname = cur;
            cur++; // we already know *cur is alphanum or underscore
            while (isalnum(*cur) || *cur == '_') cur++;
            // get var name
            char *name = gg_strdupl (vname, 0, cur-vname);
            name[cur-vname] = 0; // end string
            bool found = find_var (name, &vtype, &l, &rname, NULL);
            if (!found) 
            {
                gg_report_error ("Variable [%s] is not found", rname);
            }
            if (!cmp_type (vtype, GG_DEFBOOL)) 
            {
                gg_report_error ("Variable [%s] is not a boolean", rname);
            }
        }
    }
    while (isspace(*cur)) cur++;

    // Once token found, find if it's connected to another token (first char has been advanced to non-space)
    bool is_and = (cur[0] == '&' && cur[1] == '&');
    bool is_or = (cur[0] == '|' && cur[1] == '|');
    if (is_and || is_or) 
    {
        if (!*found) { *err = true;; return "";} // something must have been found prior to operators
        cur = check_bool(cur+2, found, err); if (*err) return ""; if (!*found) { *err = true; return "";}
    }

    if (*found && *cur != ')' && *cur != 0) *err = true; // if there's something and it's not followed by an operator 
                                                         // what follows must be either ) or nothing

    return cur;
    
}


//
// Parse string expression for validity, and check if string variables in it are proper. This does solely concatenation of strings with plus sign.
// Emits messages if errors. 
// cur is the expression, and err is true if there's a syntax error or false if not.
// Returns actual code that computes the string expression,(so code that does computation or cur itself if it's just a variable), or GG_EMPTY_STRING if error.
// String expressions are a+b+c... i.e. concatenation only
//
char *check_str (char *cur, bool *err)
{
    static char sres[300];
    char *c_elem;
    static gg_num sres_id = 0;
    *err = false;
    bool first = true;
    gg_fifo *vars;
    gg_store_init(&vars);
    while (1)
    {
        char *lp = gg_find_keyword0 (cur, GG_KEYPLUS, 0, 0);
        if (first) 
        { 
            if (lp == NULL) 
            {
                if (cur[0] == '"') // check if starts with double quote, and interpret as string expression
                {
                    if (is_constant_string(cur) == GG_CONST_OK) 
                    {
                        make_mem (&cur);
                        return cur;
                    }
                    else gg_report_error ("Syntax error in a string literal [%s]", cur);
                }
                else if (!var_exists (cur, GG_DEFSTRING, NULL, NULL)) { *err = true; return GG_EMPTY_STRING; }
                else return cur;
            }
            // if there's a plus, create a variable that will hold the result
            snprintf (sres, sizeof(sres), "_gg_sres_%ld", sres_id++);
        }

        // produce curr trimmed, ready for variable check
        if (lp != NULL) *lp = 0;
        gg_num lcur = strlen (cur);
        cur = gg_trim_ptr(cur,  &lcur); // must use this form of trim, because trimit() would alter cur, and thus stored
                                        // pointers below in gg_store, causing memory failure

        if (lp == NULL) 
        {
            if (cur[0] == 0) gg_report_error ("String expression missing");
        }

        // check if expression is a variable or string constant
        if (is_constant_string(cur) != GG_CONST_OK)
        {
            if (!var_exists (cur, GG_DEFSTRING, NULL, NULL)) { *err = true; return GG_EMPTY_STRING; }
        }
        c_elem=cur;
        gg_store(vars, gg_strdup(sres), gg_strdup(c_elem));
        //
        first = false;
        if (lp == NULL) break; else *lp = '+'; // restore plus sign sign we stored the variable into fifo with gg_store
        cur = lp + 1; // skip the plus
    }
    oprintf ("char *%s = GG_EMPTY_STRING;\n", sres);
    //
    // we get here is the expression parsed is in indeed in the form of a+b+... were each is a string
    //
    gg_rewind(vars);
    while (1) 
    {
        char *res = GG_EMPTY_STRING;
        c_elem = GG_EMPTY_STRING;
        gg_num st = gg_retrieve (vars, &res, (void**)&c_elem);
        if (st != GG_OKAY) break;
        make_mem (&c_elem);
        oprintf ("%s = gg_add_string (%s, %s);\n", res, res, c_elem);
    }
    // make memory occupied to be as much as needed. 
    oprintf ("{gg_num tmp_id = gg_mem_get_id(%s); %s=gg_realloc(tmp_id, gg_mem_get_len(tmp_id)+1);}\n", sres, sres);
    return sres;
}



//
// Check C call (a function/macro) is with valid parameters and of proper syntax
//
void check_c (char *m)
{
    char *mtext = gg_strdup (m);
    char *lp = gg_find_keyword0 (mtext, GG_KEYPARLEFT, 0, 0);
    char *rp = gg_find_keyword0 (mtext, GG_KEYPARRIGHT, 0, 0);
    if (!lp) gg_report_error ("Left parenthesis missing in C function/macro call");
    if (!rp) gg_report_error ("Right parenthesis missing in C function/macro call");
    carve_statement (&lp, "call-extended", GG_KEYPARLEFT, 1, 1);
    carve_statement (&rp, "call-extended", GG_KEYPARRIGHT, 1, 0);
    trimit(mtext);
    if (gg_is_valid_param_name (mtext, false, false) != 1) gg_report_error ("Function/macro name in call-extended is not valid [%s]", mtext);
    char *nc;
    while (1)
    {
        nc = find_keyword (lp, GG_KEYCOMMA, 0);
        carve_statement (&nc, "format parameters", GG_KEYCOMMA, 0, 1);
        trimit (lp);
        if (lp[0] == '&') lp++;
        trimit (lp);
        if (lp[0] != 0) // make sure it works for fun() meaning no arguments
        {
            // if input variable, check it
            check_var (&lp, GG_DEFUNKN);
            oprintf("GG_UNUSED (%s);\n", lp);
        }
        lp = nc;
        if (lp == NULL) break;
    }
}


//
// Find variable 'name' and return true if found and fill in type and line number where defined
// Return false if not found, true if exists (i.e. found)
// Variable is found to exist if it exists on any level outside this one (prior to it)
// rname is the real name of variable
// if is_process is not NULL, then variable is set to be either process-scoped or not, based on its value
// type, line, rname can be NULL if we don't need some of them
//
bool find_var (char *name, gg_num *type, gg_num *line, char **rname, bool *is_process)
{
    gg_num lev = gg_level;
    while (lev >= 0)
    {
        char decname[GG_MAX_DECNAME+1];
        snprintf (decname, sizeof(decname), "%ld_%ld+%s", lev, gg_resurr[lev], name);
        gg_num st;
        gg_var *var = gg_find_hash (gg_varh, decname, NULL, 0, &st);
        if (st == GG_OKAY) 
        { 
            if (type) *type = var->type;
            if (is_process) var->is_process = *is_process; // we SET variable (the other flags are output, this is INPUT)
            if (line) *line = var->lnum;
            if (rname) 
            {
                char *n = strchr (var->name, '+');
                if (var->rname[0] != 0) *rname = var->rname; else *rname = (n != NULL ? n+1:n);
            }
            var->used = true;
            return true; // variable found on leven 'lev'
        }
        lev --;
    }
    if (rname) *rname = name; // set it because it's not found
    return false; // not found
}

//
// Add variable 'var' to variable hash
// Returns true if existed, false if created
// Checks if variable of different type exists, and if so, errors out.
// realname is the real name, i.e. what's in the source file
// watched_for_unused is true if this variable will be watched for non-use
//
bool add_var (char *var, gg_num type, char *realname, bool watched_for_unused)
{
    // Check if it exists
    gg_num st;
    gg_num ot;
    gg_num ol;
    char *rname;
    if (find_var (var, &ot, &ol, &rname, NULL)) 
    {
        if (!cmp_type (type,ot)) gg_report_error ("Variable [%s] of type [%s] is already defined on line [%ld], cannot create a variable with the same name but of type [%s]", rname, typename(ot), ol, typename(type));
        return true;  
    }
    // Add if not found, this is hash for exact variable scope
    char *decname=gg_malloc (GG_MAX_DECNAME+1);
    snprintf (decname, GG_MAX_DECNAME, "%ld_%ld+%s", gg_level, gg_resurr[gg_level], var);
    gg_var *v = gg_malloc (sizeof(gg_var));
    v->name = decname;
    v->rname = (realname == NULL ? "":gg_strdup (realname));
    v->type = type;
    v->lnum = lnum;
    v->used = watched_for_unused?false:true; // if watched, we start with false, otherwise declared it "used", meaning
                                                   // we won't generate error messages;
    v->is_process = false;
    gg_add_hash (gg_varh, v->name, NULL, (void*)v, NULL, &st);
    return false;
}



//
// Return type id based on type name
//
gg_num typeid (char *type)
{
    if (!strcmp (type, GG_KEY_T_STRING)) return GG_DEFSTRING; 
    else if (!strcmp (type, GG_KEY_T_NUMBER)) return GG_DEFNUMBER;
    else if (!strcmp (type, GG_KEY_T_MESSAGE)) return GG_DEFMSG;
    else if (!strcmp (type, GG_KEY_T_SPLITSTRING)) return GG_DEFBROKEN;
    else if (!strcmp (type, GG_KEY_T_HASH)) return GG_DEFHASH;
    else if (!strcmp (type, GG_KEY_T_ARRAYSTRING)) return GG_DEFARRAYSTRING;
    else if (!strcmp (type, GG_KEY_T_ARRAYNUMBER)) return GG_DEFARRAYNUMBER;
    else if (!strcmp (type, GG_KEY_T_ARRAYBOOL)) return GG_DEFARRAYBOOL;
    else if (!strcmp (type, GG_KEY_T_JSON)) return GG_DEFJSON;
    else if (!strcmp (type, GG_KEY_T_XML)) return GG_DEFXML;
    else if (!strcmp (type, GG_KEY_T_TREE)) return GG_DEFTREE;
    else if (!strcmp (type, GG_KEY_T_TREECURSOR)) return GG_DEFTREECURSOR;
    else if (!strcmp (type, GG_KEY_T_FIFO))  return GG_DEFFIFO;
    else if (!strcmp (type, GG_KEY_T_LIFO))  return GG_DEFLIFO;
    else if (!strcmp (type, GG_KEY_T_LIST))  return GG_DEFLIST;
    else if (!strcmp (type, GG_KEY_T_ENCRYPT)) return GG_DEFENCRYPT;
    else if (!strcmp (type, GG_KEY_T_FILE)) return GG_DEFFILE;
    else if (!strcmp (type, GG_KEY_T_SERVICE)) return GG_DEFSERVICE;
    else if (!strcmp (type, GG_KEY_T_BOOL)) return GG_DEFBOOL;
    else gg_report_error ("Unknown type [%s]", type);
}

//
// Check for {} in prline and adjust gg_level
// prline is generated code that has { and } and that's how we determine scope
// We pay attention to { or } being within a string (and comments have been stripped already) and so we account 
// only for real code block opening and closing. This is important to make sure variables are processed correctly.
// dir is true for left and false for right
// User comments have been stripped by this stage. However, there are // comments (to the end of the line)
// that we add - so must discount those.
//
// NOTE: IMPORTANT: when generating code, no variable must be added outside it's scope. Meaning ...{ var used }... and
// then add variable before { or after }. It must be ...{ ... <var added> var used ... } .... Basically the scope must be
// correct when adding variables.
// We do not automatically cleanup variables after } (after scope ends) so the performance doesn't suffer. Requests are meant
// to be small and memory is automatically cleaned up at the end; this doesn't impact performance because another process can handle
// the next request while cleanup takes place. Use delete-string to minimize this if needed.
//
// Note that prline can be anything, not necessarily the actual generated code. For instance we can pass "{" to prline when some
// new statement starts and "}" when it ends, which has the effect; for example this is done with do-once (with the exception that
// process-scope variables operate on the base do-once level, but this is an exception just for do-once)
//
void check_level(char *prline)
{
    char *cbl;
    char *cbr;
    char *cb = NULL;
    char *comm;
    comm =  gg_find_keyword0 (prline, GG_KEYCOMMENT, 0, 0);
    if (comm != NULL ) *comm = 0; // cap the line, restore later
    while (1)
    {
        // 0 means don't need a space after keyword, 1 means ignore () and account only for ""
        cbl = gg_find_keyword0 (prline, GG_KEYCURLYLEFT, 0, 0);
        cbr = gg_find_keyword0 (prline, GG_KEYCURLYRIGHT, 0, 0);
        // cbls is { and cbr is }. We adjust scope level
        // based on the order these appear, which the below does
        if (cbl != NULL)
        {
            if (cbr != NULL)
            {
                if (cbl-prline < cbr - prline)
                {
                    gg_level++;
                    cb = cbl;
                }
                else 
                {
                    gg_resurr[gg_level]++;
                    gg_level--;
                    cb = cbr;
                }
            }
            else
            {
                gg_level++;
                cb = cbl;
            }
        }
        else
        {
            if (cbr != NULL)
            {
                gg_resurr[gg_level]++;
                gg_level--;
                cb = cbr;
            } else break;
        }
        if (gg_level >= GG_MAX_CODE_LEVEL) gg_report_warning ("Code levels too deep, restructure code for less code blocks in depth");
        if (gg_level < 0) { gg_level = 0; gg_report_warning ("Code block opening/closing imbalance"); }
        if (cb == NULL) break;
        prline = cb + 1;
        cb = NULL;
        if (prline[0] == 0) break;
    }
    if (comm != NULL) *comm = '/';
}

//
// Returns >0 if s is in the form of [0-9]*<f>, as in %40s where <f> is "s", or just <f>
// Returned number is such that s+<return> is the first byte after <f>, it returns 'true' in this case
// If not in this form, returns 0 (i.e. false)
//
gg_num is_len_format (char *s, char *f, gg_num flen)
{
    char *os = s;
    while (*s && isdigit(*s)) s++;
    if (!memcmp(s,f,flen)) {s+=flen; return (gg_num)(s-os);} else return 0;
}


//
// Returns GG_CONST_OK if 's' is "something" valid string, with backslash (\) escaping whatever accounted for
// It also accounts if there's a + sign in there, which would be an expression of concatenating strings
// Otherwise return GG_CONST_BAD if it starts with " but isn't a valid string, or GG_CONST_NOT if it doesn't start with "
// or GG_CONST_EMPTY if it's just whitespace
// A string can also be a concatenation of "xxx" strings, such as "xxx""yyy"...
//
gg_num is_constant_string (char *s)
{
    while (*s && isspace(*s)) s++;
    if (*s != '"') return GG_CONST_NOT; // not string
    if (*s == 0) return GG_CONST_EMPTY; // empty
    if (gg_find_keyword0 (s, GG_KEYPLUS, 0, 0) != NULL) return  GG_CONST_NOT; // this will be checked out later
                                                                                // to see if it's a correct expression
                                                                                // in check_str() from check_var()
    s++;
    bool found = false;
    while (*s != 0)  
    {
        if (*s == '\\') { if (*(s+1) != 0) { s+=2; continue; } else return GG_CONST_BAD; } 
        if (*s == '"')
        {
            s++;
            while (*s && isspace(*s)) s++;
            if (*s == '"') { s++; continue;} // this is "xxx" "yyy" etc
            found =true; 
            break;
        }
        s++;
    }
    while (*s && isspace(*s)) s++;
    if (*s != 0) return GG_CONST_BAD; else 
    {
        if (found) return GG_CONST_OK; else return GG_CONST_BAD;
    }
}

//
// Check format whenever needed. It supports %%, %s, %.*s, %ld/o/x and %*ld/o/x and %c
// Following parameters are checked for accuracy of type
// list is a list of comma separated params, starting with a comma
// caller must release *list
//
void check_format(char *mtext, char *comma, char **list)
{
    gg_num mlist;
    *list = gg_malloc (mlist=200+strlen(mtext)*15); // 15 is max length of _gg_dexp%ld, so worst case scenario
    gg_num clist = 0;
    char *p = gg_strdup(mtext);
    char *next = p;
    char *c;
    if (comma != NULL) c = gg_strdup(comma); else c=NULL;
    char *par = c;
    gg_num pos = 0;
    char *nc = NULL;
    bool perc = false;
    gg_num fmt_len;
    while (1)
    {
        pos++;
        // find next comma, if any, but only if previously wasn't just a percent sign, there's no advancement of parameter in that case
        if (!perc && par != NULL)
        {
            nc = find_keyword (par, GG_KEYCOMMA, 0);
            carve_statement (&nc, "format parameters", GG_KEYCOMMA, 0, 1);
        }
        // find a percent sign
        next = strchr (next, '%');
        if (next == NULL) 
        {
            if (par != NULL) gg_report_error ("There is more output parameters than percent-placeholders in format [%s]", mtext);
            break;
        }
        else
        {
            if (par == NULL) gg_report_error ("There are more percent-placeholders than output parameters, format [%s]", mtext);
        }
        // check for valid percent-placeholders
        perc = false;
        fmt_len = 0; // since assignment of fmt_len below is in ||, meaning it may not execute
        if (*(next+1) == '%') { next+=2; perc = true; continue;} // this is printing out %, do not advance nc since %% doesn't use up parameter
        else if (*(next+1) == 's' || (fmt_len = is_len_format(next+1, "s", 1)))
        {
            // should be GG_DEFSTRING
            if (is_constant_string(par) != GG_CONST_OK &&  !cmp_type (check_var (&par, GG_DEFUNKN), GG_DEFSTRING)) gg_report_error ("Parameter #%ld in format should be of string type", pos);
            if (fmt_len == 0) next+=2; else next+=1+fmt_len;
            clist += snprintf (*list+clist, mlist-clist, ", %s ",par);
        }
        // this supports ld, lo, lx, %20ld, %14lo, %8lx etc.
        else if ((*(next+1) == 'l' && (*(next+2) == 'd' || *(next+2) == 'o' || *(next+2) == 'x')) || ((fmt_len = is_len_format(next+1, "ld", 2)) || (fmt_len = is_len_format(next+1, "lo", 2)) || (fmt_len = is_len_format(next+1, "lx", 2))))
        {
            // should be GG_DEFNUMBER
            gg_num t = check_var (&par, GG_DEFUNKN);
            if ((!cmp_type (t, GG_DEFNUMBER) && !cmp_type (t, GG_DEFBOOL))) gg_report_error ("Parameter #%ld in format should be of number type", pos);
            if (fmt_len == 0) next+=3; else next+=1+fmt_len;
            clist += snprintf (*list+clist, mlist-clist, ", %s ",par);
        }
        else if ((*(next+1) == 'c'))
        {
            // should be GG_DEFNUMBER (this is character from string)
            gg_num t = check_var (&par, GG_DEFUNKN);
            if ((!cmp_type (t, GG_DEFNUMBER))) gg_report_error ("Parameter #%ld in format should be of number type", pos);
            next+=2;
            clist += snprintf (*list+clist, mlist-clist, ", %s ",par);
        }
        else gg_report_error ("Only %%s, %%<length>s, %%ld, %%<length>ld and %%c are valid placeholders in format string, along with %%%% to output the percent sign, found [%s]", mtext);
        par = nc;
    }
}

//
//call-remote from statement text (mtext)
//
void call_service_find (char *mtext, char **finok, char **started)
{
    *finok = find_keyword (mtext, GG_KEYFINISHEDOKAY, 1);
    *started = find_keyword (mtext, GG_KEYSTARTED, 1);
}
void call_service_carve (char **finok, char **started)
{
    carve_statement (finok, "call-remote", GG_KEYFINISHEDOKAY, 0, 1);
    carve_statement (started, "call-remote", GG_KEYSTARTED, 0, 1);
}
void call_service_define(char **finok, char **started)
{
    define_statement (finok, GG_DEFNUMBER, false);
    define_statement (started, GG_DEFNUMBER, false);
}

void call_service (char *mtext)
{
    char *st, *finok, *started;
    call_service_find (mtext, &finok, &started);
    st = find_keyword (mtext, GG_KEYSTATUS, 1);

    call_service_carve (&finok, &started);
    carve_statement (&st, "call-remote", GG_KEYSTATUS, 0, 1);

    carve_stmt_obj (&mtext, true, false);
    call_service_define (&finok, &started);
    define_statement (&st, GG_DEFNUMBER, false);

    call_service_impl (st, finok, started, mtext);
}

//
// Implementation of call-service. st is status (can be NULL), finok
// is finished-okay, started is 'started' clause, mtext is the service
// NOTE: _gg_st is the status regardless of 'status' clause used or not, so that's only checked in run-service
// This works the same as standalone or part of run-service. Note that checking
// if status is present is fine for either case, because it doesn't matter at
// what stage it errors out (if there's no status and there's an error)
//
void call_service_impl (char *st, char *finok, char *started, char *mtext)
{

    char req_var[100];
    char totreq_s[30];
    // this is a list of fcgi calls, otherwise var is mtext, count is the length of array
    snprintf (req_var, sizeof (req_var), "_gg_cli_req_arr%ld", total_fcgi_arr);
    gg_num totreq = outargs(mtext, req_var, "gg_cli *", 0, 0, GG_DEFSERVICE, GG_DEFUNKN);
    snprintf (totreq_s, sizeof(totreq_s), "%ld", totreq);
    oprintf ("{_gg_st=gg_call_fcgi (%s, %s, %s%s%s, %s%s%s);}\n", req_var, totreq_s, finok==NULL?"":"&(", finok==NULL?"NULL":finok, finok==NULL?"":")", started==NULL?"":"&(", started==NULL?"NULL":started, started==NULL?"":")" );
    if (st != NULL) oprintf("%s=_gg_st;\n",st);
    else oprintf("if (_gg_st != GG_OKAY) gg_report_error (\"%s\", \"%s\", (gg_num)%ld, _gg_st);\n", GG_STMT_FAIL_CHECK, undecorate (src_file_name+GG_BLD_DIR_OFFSET), lnum);
    total_fcgi_arr++;
}


void read_service_find(char *mtext, char **data, char **error, char **rmsg, char **rstatus)
{
    *data = find_keyword (mtext, GG_KEYDATA, 1);
    *error = find_keyword (mtext, GG_KEYERROR, 1);
    *rmsg = find_keyword (mtext, GG_KEYSTATUSTEXT, 1);
    *rstatus = find_keyword (mtext, GG_KEYEXITSTATUS, 1);
}
void read_service_carve(char **data, char **error, char **rmsg, char **rstatus)
{
    carve_statement (data, "read-remote", GG_KEYDATA, 0, 1);
    carve_statement (error, "read-remote", GG_KEYERROR, 0, 1);
    carve_statement (rmsg, "read-remote", GG_KEYSTATUSTEXT, 0, 1);
    carve_statement (rstatus, "read-remote", GG_KEYEXITSTATUS, 0, 1);
}
void read_service_define(char **data, char **error, char **rmsg, char **rstatus)
{
    define_statement (data, GG_DEFSTRING, false); // exact length set in gg_cli_data
    define_statement (error, GG_DEFSTRING, false); // exact length set in gg_cli_error
    define_statement (rstatus, GG_DEFNUMBER, false);
    define_statement (rmsg, GG_DEFSTRING, false); // exact length set in strdup below
}

//
// read-service implementation. mtext is service, data is 'data' clause,
// error is 'error' clause, st is status, rmsg is status-text and rstatus is
// request-status
// This works the same as standalone or part of run-service. Note that checking
// if status is present is fine for either case, because it doesn't matter at
// what stage it errors out (if there's no status and there's an error)
// In run-service, if we get to this point, it means that implied call-service is done (and status checking there 
// is done, and execution stopped if call-service failed without status being specified), and here if any of read-service
// fail without status, execution will stop too. 
// Note _gg_st is NOT used here, as this is the end of execution of run-service
//
void read_service_impl(char *mtext, char *data, char *error, char *st, char *rmsg, char *rstatus)
{
    check_var (&mtext, GG_DEFSERVICE);
    //
    if (data == NULL && error == NULL && st == NULL && rstatus == NULL) gg_report_error( "at least one clause must be specified in new-remote statement");
    if (data != NULL) oprintf ("%s = gg_cli_data(%s);\n", data, mtext);
    if (error != NULL) oprintf ("%s = gg_cli_error(%s);\n", error, mtext);
    if (st != NULL) oprintf ("%s = (%s)->return_code;\n", st, mtext);
    else oprintf("if ((%s)->return_code != GG_OKAY) gg_report_error (\"%s\", \"%s\", (gg_num)%ld, (gg_num)((%s)->return_code));\n", mtext, GG_STMT_FAIL_CHECK, undecorate (src_file_name+GG_BLD_DIR_OFFSET), lnum, mtext);
    if (rmsg != NULL) oprintf ("%s = gg_strdup((%s)->errm);\n", rmsg, mtext);
    if (rstatus != NULL) oprintf ("%s = (%s)->req_status;\n", rstatus, mtext);
}

//
//read-remote from statement text (mtext)
//
void read_service (char *mtext)
{
    char *data, *error, *st, *rmsg, *rstatus;
    read_service_find(mtext, &data, &error, &rmsg, &rstatus);
    st = find_keyword (mtext, GG_KEYSTATUS, 1);
    read_service_carve(&data, &error, &rmsg, &rstatus);
    carve_statement (&st, "read-remote", GG_KEYSTATUS, 0, 1);

    carve_stmt_obj (&mtext, true, false);
    read_service_define(&data, &error, &rmsg, &rstatus);
    define_statement (&st, GG_DEFNUMBER, false);
    
    read_service_impl(mtext, data, error, st, rmsg, rstatus);
}

void new_service_find(char *mtext, char **server, char **method, char **apath, char **req, char **body, char **upay, char **timeout, char **env, char **loc, char **url)
{
    *server = find_keyword (mtext, GG_KEYLOCATION, 1);
    *method = find_keyword (mtext, GG_KEYMETHOD, 1);
    *apath = find_keyword (mtext, GG_KEYAPPPATH, 1);
    *req = find_keyword (mtext, GG_KEYREQUESTPATH, 1);
    *body = find_keyword (mtext, GG_KEYREQUESTBODY, 1);
    *upay = find_keyword (mtext, GG_KEYURLPARAMS, 1);
    *timeout = find_keyword (mtext, GG_KEYTIMEOUT, 1);
    *env = find_keyword (mtext, GG_KEYENVIRONMENT, 1);
    *loc = find_keyword (mtext, GG_KEYLOCAL, 1);
    *url = find_keyword (mtext, GG_KEYURLPATH, 1);
}
void new_service_carve(char **server, char **method, char **apath, char **req, char **body, char **upay, char **timeout, char **env, char **loc, char **url)
{
    carve_statement (server, "new-remote", GG_KEYLOCATION, 0, 1);
    carve_statement (method, "new-remote", GG_KEYMETHOD, 0, 1);
    carve_statement (apath, "new-remote", GG_KEYAPPPATH, 0, 1);
    carve_statement (req, "new-remote", GG_KEYREQUESTPATH, 0, 1);
    carve_statement (body, "new-remote", GG_KEYREQUESTBODY, 0, 1);
    carve_statement (upay, "new-remote", GG_KEYURLPARAMS, 0, 1);
    carve_statement (timeout, "new-remote", GG_KEYTIMEOUT, 0, 1);
    carve_statement (env, "new-remote", GG_KEYENVIRONMENT, 0, 1);
    carve_statement (loc, "new-remote", GG_KEYLOCAL, 0, 1);
    carve_statement (url, "new-remote", GG_KEYURLPATH, 0, 1);
}

//
//new-remote from statement text (mtext)
//
void new_service (char *mtext)
{
    //
    // Look for each option and collect relevant info
    //
    char *server, *method, *apath, *req, *body, *upay, *timeout, *env, *loc, *url;
    new_service_find(mtext, &server, &method, &apath, &req, &body, &upay, &timeout, &env, &loc, &url);
    new_service_carve(&server, &method, &apath, &req, &body, &upay, &timeout, &env, &loc, &url);

    carve_stmt_obj (&mtext, true, false);
    make_mem(&server);    
    make_mem(&method);    
    make_mem(&apath);    
    make_mem(&req);    
    make_mem(&upay);    
    make_mem(&loc);    
    make_mem(&url);    

    new_service_impl(mtext, server, method, apath, req, body, upay, loc, url, env, timeout);
}

void new_service_impl (char *mtext, char *server, char *method, char *apath, char *req, char *body, char *upay, char *loc, char *url, char *env, char *timeout)
{
    check_var (&server, GG_DEFSTRING);
    check_var (&method, GG_DEFSTRING);
    check_var (&apath, GG_DEFSTRING);
    check_var (&req, GG_DEFSTRING);
    check_var (&upay, GG_DEFSTRING);
    check_var (&loc, GG_DEFSTRING);
    check_var (&timeout, GG_DEFNUMBER);
    check_var (&url, GG_DEFSTRING);
    //
    if (server != NULL && loc != NULL) gg_report_error ("Only one of 'local' or 'location' can be used");
    if (server == NULL && loc == NULL) gg_report_error ("Either 'local' or 'location' must be specified");
    if (url != NULL && (apath != NULL || req != NULL || upay != NULL)) gg_report_error ("Use either 'url-path' clause, or app-path/request-path/url-payload clauses");
    if (url == NULL && (apath == NULL || req == NULL)) gg_report_error ("If 'url-path' clause is not used, then you must specify app-path and request-path clauses");

    // process body clause after ward because it is done within a copy of carved out 'body' above
    char *ctype = NULL;
    char *content = NULL;
    char *clen = NULL;
    char *mbody = NULL;
    if (body != NULL)
    {
        mbody = gg_strdup (body);
        content = find_keyword (mbody, GG_KEYCONTENT, 1);
        clen = find_keyword (mbody, GG_KEYCONTENTLENGTH, 1);
        ctype = find_keyword (mbody, GG_KEYCONTENTTYPE, 1);
        carve_statement (&content, "new-remote", GG_KEYCONTENT, 0, 1);
        carve_statement (&clen, "new-remote", GG_KEYCONTENTLENGTH, 0, 1);
        carve_statement (&ctype, "new-remote", GG_KEYCONTENTTYPE, 0, 1);
        // can place them here because there's no more carve_statement, make_mem is ALWAYS after
        // ALL carve_statement ARE DONE!
        make_mem (&content);
        make_mem (&ctype);
        //
        check_var (&content, GG_DEFSTRING);
        check_var (&ctype, GG_DEFSTRING);
        check_var (&clen, GG_DEFNUMBER);
    }

    if (method == NULL) { method = "\"GET\""; make_mem(&method);} // by default method is get 
    if (body == NULL && ctype != NULL) gg_report_error( "'content-type' cannot be specified without 'request-body' in new-remote statement");
    if (body == NULL && clen != NULL) gg_report_error( "'content-length' cannot be specified without 'request-body' in new-remote statement");
    if (body != NULL && ctype == NULL) gg_report_error( "'content-type' must be specified for 'request-body' in new-remote statement");
    if (body != NULL && content == NULL) gg_report_error( "'content' must be specified for 'request-body' in new-remote statement");


    char env_var[100];
    snprintf (env_var, sizeof (env_var), "_gg_cli_env_arr%ld", total_fcgi_arr);
    if (env != NULL) outargs(env, env_var, "char *", 0, 1, GG_DEFSTRING, GG_DEFSTRING);

    // define for fcgi thread
    define_statement (&mtext, GG_DEFSERVICE, true);
    oprintf ("gg_set_fcgi (&(%s), %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s);\n", mtext, server==NULL?loc:server, method, apath==NULL?"GG_EMPTY_STRING":apath, req==NULL?"GG_EMPTY_STRING":req, upay==NULL?url:upay,ctype==NULL?"NULL":ctype, body==NULL?"NULL":content, clen==NULL?"0":clen, timeout==NULL?"0":timeout, env==NULL?"NULL":env_var, loc!=NULL?"true":"false", url!=NULL?"true":"false");

    total_fcgi_arr++;
}

void run_service (char *mtext)
{
    char *server, *method, *apath, *req, *body, *upay, *timeout, *env, *loc, *url; // new-remote
    char *data, *error, *rmsg, *rstatus; // read-remote
    char *finok, *started; // call-remote
    char *st; //status common to read-remote and call-remote

    //find keywords
    new_service_find(mtext, &server, &method, &apath, &req, &body, &upay, &timeout, &env, &loc, &url);
    read_service_find(mtext, &data, &error, &rmsg, &rstatus);
    call_service_find (mtext, &finok, &started);
    st = find_keyword (mtext, GG_KEYSTATUS, 1); // common to read-/call-remote

    // carve keywords
    new_service_carve(&server, &method, &apath, &req, &body, &upay, &timeout, &env, &loc, &url);
    call_service_carve (&finok, &started);
    read_service_carve(&data, &error, &rmsg, &rstatus);
    carve_statement (&st, "run-remote", GG_KEYSTATUS, 0, 1); // carve common to read/call service
    carve_stmt_obj (&mtext, true, false);

    //define statements
    read_service_define(&data, &error, &rmsg, &rstatus);
    define_statement (&st, GG_DEFNUMBER, false);
    call_service_define (&finok, &started);

    // stringify constants
    // this is for new-remote
    make_mem(&server);    
    make_mem(&method);    
    make_mem(&apath);    
    make_mem(&req);    
    make_mem(&body);    
    make_mem(&upay);    
    make_mem(&loc);    
    make_mem(&url);    
    // nothing for new/run-remote

    // run new-remote
    new_service_impl(mtext, server, method, apath, req, body, upay, loc, url, env, timeout);
    // run call-remote
    call_service_impl (st, finok, started, mtext);
    // we do want to read-remote if GG_OKAY or GG_ERR_FAILED, because we get the actual error messsage/code there
    // only for GG_ERR_TOO_MANY or GG_ERR_MEMORY we don't want to.
    oprintf ("if (_gg_st == GG_OKAY || _gg_st == GG_ERR_FAILED) {\n");
    read_service_impl(mtext, data, error, st, rmsg, rstatus);
    oprintf ("} else {\n");
    // this is if err_too_many (which can't happen since it's only one, or out of memory, so say it
    if (rmsg != NULL) oprintf ("if (_gg_st == GG_ERR_MEMORY) %s = gg_strdup(\"Out of memory\"); else %s = gg_strdup (\"Unknown error\");\n", rmsg,rmsg);
    else oprintf(";"); // for compiler peace of mind
    if (st==NULL) oprintf("gg_report_error (\"%s\", \"%s\", (gg_num)%ld, _gg_st);\n", GG_STMT_FAIL_CHECK, undecorate (src_file_name+GG_BLD_DIR_OFFSET), lnum); // this really should never happen since call_service_impl() should catch this
    oprintf ("}\n");
}

//
// Turn unquoted string into quoted one. This is for set-unquoted-string
// This doesn't affect check_level because real string in a single line is constructed before
// it goes to oprintf and subsequently to check_level
//
void make_unq (char ** clause)
{
    if (*clause == NULL) return;
    gg_num tot;
    gg_num len;
    char *q = gg_malloc (tot=2*(len=strlen(*clause))+3+1); // 2x for each char escaped, 2 quotes plus null, plus 1 for luck
    char *curr = q;
    *(curr++) = '"';
    gg_num i;
    for (i = 0; i < len; i++)
    {
        if ((*clause)[i] == '"') { *(curr++) = '\\'; *(curr++) = '"'; }
        // NOTE: removed the following line, because it makes more sense to keep escaped characters, or otherwise you
        // wouldn't be able to have them in the first place in unquoted string!
        /*else if ((*clause)[i] == '\\') { *(curr++) = '\\'; *(curr++) = '\\';}*/
        else if ((*clause)[i] == '\n') { *(curr++) = '\\'; *(curr++) = 'n';} // this is when \\ is used at the end of the line,
                                                                             // which inserts 
        else *(curr++) = (*clause)[i];
    }
    *(curr++) = '"';
    *curr = 0;
    *clause = q;
    make_mem (clause);
}

//
// Make golf memory out of string literal clause. 
// We use this system instead of a structure that has a pointer/length etc. because with that system we would use
// a pointer to this structure, which has a pointer to memory. That's double redirection which slows down everything
// a lot. Making a bit of memory in a few special cases is a much better performing system.
// *clause is the memory to convert, and is the result, which holds the name of variable created.
//
void make_mem (char **clause)
{
    if (*clause == NULL) return;
    char *s = *clause;
    while (isspace(*s)) s++;
    gg_num cs = is_constant_string(s);
    if (cs != GG_CONST_OK && cs != GG_CONST_NOT) gg_report_error ("String [%s] is not correctly formed", s);
    if (cs != GG_CONST_OK) return; // this captures variables and anything with _ in front
                                            // which we assume is a define-d literal (like __FILE__)

    static gg_num gmem = 0;
    // create new value, copy string literal to it (once for the life of process), set it process-scoped
    // then return the name of that variable instead of constant
    // variable is made only once per process
    // it is "literal", because we declare it as a char array really!
    oprintf ("if (!__builtin_constant_p(%s)) gg_report_error(\"Unknown variable or constant\");\n", *clause);
    // source file name is immutable, or at least is supposed to 
    // static makes sure the pointer value remains outside this scope
    // and constant initi ensures it remains exactly the same pointer
    oprintf ("static char _gg_gstr%ld[] = \"        \" %s;\n",gmem,*clause); // 8 bytes is for GG_ALIGN
                                                                            // so whatever number that is, that's how many spaces are in front of *clause
    // without this bool guard, a new variable would be created with invocation here
    oprintf ("static bool _gg_gstr_once%ld = true;\n", gmem); 
    //
    //
    // NOTE: internal variable that starts with _gg__<string def code (see golf.h)>__ is **always** a string!! check_var **depends** on this
    //
    //
    oprintf ("static char *_gg__" GG_STRINGIZE(GG_DEFSTRING)  "__ret%ld;\n", gmem); 
    //
    // Adding variable must be done at the level when it's generated since we count levels by {} in code generated
    // So add it before the next { down here
    //
#define GG_MAKE_MEM_LEN 35
    char *makem = gg_malloc(GG_MAKE_MEM_LEN);
    // if we're converting literal into string variable, this is where we give it name which propogates to caller
    snprintf (makem, GG_MAKE_MEM_LEN, "_gg__" GG_STRINGIZE(GG_DEFSTRING)  "__ret%ld", gmem);
    add_var (makem, GG_DEFSTRING, *clause, false);
    *clause = makem; // must be AFTER add_var or otherwise realname in add_var would be generated name!
    oprintf ("if (_gg_gstr_once%ld) {\n", gmem); 
    oprintf ("_gg__" GG_STRINGIZE(GG_DEFSTRING)  "__ret%ld = gg_mem_add_const (_gg_gstr%ld, sizeof(_gg_gstr%ld)-GG_ALIGN);\n", gmem, gmem, gmem); 
    oprintf ("_gg_gstr_once%ld = false;\n}\n", gmem); 

    //
    gmem++;
}


// 
// Parse condition, in the form of say 'x equal y or z not-equal w'.
// Generates code for this, but not the surrounding () if any needed, the caller must do it.
// cm is the text of condition
// bifs is the code that goes before the entire "if" statement, currently it's only for else-if, which is "} else {"
// we do not generate "if () {} else if () {}", but rather "if () {} else { if() {} }"
// ifs is the leading portion of "if" statement that should be generated, with the parsed condition to follow.
// ife is the ending portion of "if" statement that should be generated
//
//
//
void parse_cond(char *cm, char *bifs, char *ifs, char *ife)
{
    char *eq = NULL;
    char *neq = NULL;
    char *less = NULL;
    char *lesseq = NULL;
    char *gr = NULL;
    char *mod = NULL;
    char *notmod = NULL;
    char *greq = NULL;
    char *cont = NULL;
    char *notcont = NULL;
    char *caseins = NULL;
    char *len = NULL;
    char *or = NULL;
    char *and = NULL;

    static gg_num gvar_stmt = 0; // used for the entire condition
    static gg_num gvar = 0; // used for variables for each condition in a service
    bool done_stmt = false;
    gg_num tot_open = 0; // number of open { which we need to close with }
    bool is_or = false;
    bool is_and =false;
    oprintf ("%s", bifs);
    while (1) 
    {
        // Look for any and/or first
        or = find_keyword (cm, GG_KEYOR, 1);
        and = find_keyword (cm, GG_KEYAND, 1);
        if (or != NULL && and != NULL) gg_report_error ("Can use 'or' or 'and' clauses, but not both");
        gg_num or_pos = INT_MAX;
        gg_num and_pos = INT_MAX;
        // Cut off first and/or segment, if there is one (if not, it's just a simple condition)
        if (or) or_pos = or - cm;
        if (and) and_pos = and - cm;
        gg_num op = MIN (or_pos, and_pos);
        if (op != INT_MAX)
        {
            if (or_pos == op) { is_or = true; carve_statement (&or, "if-true", GG_KEYOR, 0, 1); }
            else if (and_pos == op) { is_and = true; carve_statement (&and, "if-true", GG_KEYAND, 0, 1); }
        }
        // else both is_or and is_true remain false
        //
        if (!done_stmt)
        {
            done_stmt = true;
            // statement boolean (the one we actually check in if) is initially false for OR and true for AND
            // that's because of the nature of or/and operators, so that initial value doesn't affect boolean computation outcome
            if (is_or )oprintf ("bool _gg_if_stmt%ld = false;\n", gvar_stmt);
            else if (is_and )oprintf ("bool _gg_if_stmt%ld = true;\n", gvar_stmt);
            else oprintf ("bool _gg_if_stmt%ld;\n", gvar_stmt);
        }

        // then find any relevant keywords in the first segment (which may be the only (remaining) one)
        eq = find_keyword (cm, GG_KEYEQUAL, 1);
        neq = find_keyword (cm, GG_KEYNOTEQUAL, 1);
        less = find_keyword (cm, GG_KEYLESSERTHAN, 1);
        lesseq = find_keyword (cm, GG_KEYLESSEREQUAL, 1);
        gr = find_keyword (cm, GG_KEYGREATERTHAN, 1);
        greq = find_keyword (cm, GG_KEYGREATEREQUAL, 1);
        mod = find_keyword (cm, GG_KEYEVERY, 1);
        notmod = find_keyword (cm, GG_KEYNOTEVERY, 1);
        cont = find_keyword (cm, GG_KEYCONTAIN, 1);
        notcont = find_keyword (cm, GG_KEYNOTCONTAIN, 1);
        caseins = find_keyword (cm, GG_KEYCASEINSENSITIVE, 1);
        len = find_keyword (cm, GG_KEYLENGTH, 1);

        if (eq == NULL && neq == NULL && less == NULL && lesseq == NULL && gr == NULL && mod == NULL && notmod == NULL && greq == NULL && cont == NULL && notcont == NULL && caseins == NULL && len == NULL) gg_report_error ("conditional clause is missing or empty");

        gg_num eq_pos = INT_MAX;
        gg_num neq_pos = INT_MAX;
        gg_num less_pos = INT_MAX;
        gg_num lesseq_pos = INT_MAX;
        gg_num gr_pos = INT_MAX;
        gg_num mod_pos = INT_MAX;
        gg_num notmod_pos = INT_MAX;
        gg_num greq_pos = INT_MAX;
        gg_num cont_pos = INT_MAX;
        gg_num notcont_pos = INT_MAX;
        if (eq) eq_pos = eq - cm;
        if (neq) neq_pos = neq - cm;
        if (less) less_pos = less - cm;
        if (lesseq) lesseq_pos = lesseq - cm;
        if (mod) mod_pos = mod - cm;
        if (notmod) notmod_pos = notmod - cm;
        if (gr) gr_pos = gr - cm;
        if (greq) greq_pos = greq - cm;
        if (cont) cont_pos = cont - cm;
        if (notcont) notcont_pos = notcont - cm;
        if (caseins) carve_statement (&caseins, "if-true", GG_KEYCASEINSENSITIVE, 0, 0);  // carve it out, so it doesn't mess up other clauses
        if (len) carve_statement (&len, "if-true", GG_KEYLENGTH, 0, 1);  // carve it out, so it doesn't mess up other clauses

        check_var (&len, GG_DEFNUMBER);

        gg_num m = MIN(MIN (MIN (MIN (MIN (MIN(eq_pos, neq_pos), MIN(less_pos,lesseq_pos)), MIN(gr_pos,greq_pos)), MIN(notcont_pos,cont_pos)), mod_pos), notmod_pos);
        if (m == INT_MAX) break;
        // must be after break or _gg_ifcond.. would be duplicated or unused
        oprintf ("bool _gg_ifcond%ld;\n", gvar);

        if (eq_pos == m) 
        { 
            carve_statement (&eq, "if-true", GG_KEYEQUAL, 1, 1); 
            // n_cm is so that cm is preserved for the next loop of search for the next or/and
            // make_mem for n_cm must be repeated for each condition b/c it must be after carve_statements
            char *n_cm = cm;
            make_mem (&n_cm);
            gg_num leq = strlen (eq);
            eq = gg_trim_ptr(eq,  &leq);
            //comparison with NULL is undocumented and unsuported! Only for testing where C is allowed.
            if (!strcmp (eq, "NULL")) oprintf("_gg_ifcond%ld = ((%s) == NULL ? true:false);\n", gvar, n_cm);  else 
            {
                make_mem (&eq);
                //
                gg_num t1 = check_var (&n_cm, GG_DEFUNKN);
                check_var (&eq, t1); 
                // Here now can call specific function based on type and there's no need for Generic()
                if (!len) oprintf("_gg_ifcond%ld = GG_CMP_EQ%s((%s), (%s));\n", gvar, caseins?"_C":"", n_cm, eq);  
                else oprintf("_gg_ifcond%ld = GG_CMP_EQ%s_L((%s), (%s), (%s));\n",gvar, caseins?"_C":"", n_cm, eq, len);  
            }
        }
        else if (neq_pos == m) 
        { 
            carve_statement (&neq, "if-true", GG_KEYNOTEQUAL, 1, 1); 
            // n_cm is so that cm is preserved for the next loop of search for the next or/and
            // make_mem for n_cm must be repeated for each condition b/c it must be after carve_statements
            char *n_cm = cm;
            make_mem (&n_cm);
            gg_num lneq = strlen (neq);
            neq = gg_trim_ptr(neq,  &lneq);
            //comparison with NULL is undocumented and unsuported! Only for testing where C is allowed.
            if (!strcmp (neq, "NULL")) oprintf("_gg_ifcond%ld = ((%s) != NULL ? true:false);\n",gvar, n_cm);  else 
            {
                make_mem (&neq);
                //
                gg_num t1 = check_var (&n_cm, GG_DEFUNKN);
                check_var (&neq, t1);
                if (!len) oprintf("_gg_ifcond%ld = !GG_CMP_EQ%s((%s), (%s));\n", gvar, caseins?"_C":"", n_cm, neq); 
                else oprintf("_gg_ifcond%ld = !GG_CMP_EQ%s_L((%s), (%s),(%s));\n", gvar, caseins?"_C":"", n_cm, neq, len); 
            }
        }
        else if (lesseq_pos == m) 
        { 
            carve_statement (&lesseq, "if-true", GG_KEYLESSEREQUAL, 1, 1); 
            // n_cm is so that cm is preserved for the next loop of search for the next or/and
            // make_mem for n_cm must be repeated for each condition b/c it must be after carve_statements
            char *n_cm = cm;
            make_mem (&n_cm);
            make_mem (&lesseq);
            //
            gg_num t1 = check_var (&n_cm, GG_DEFUNKN);
            check_var (&lesseq, t1);
            if (!len) oprintf("_gg_ifcond%ld = GG_CMP_LESSEQ%s((%s), (%s));\n", gvar, caseins?"_C":"", n_cm, lesseq); 
            else oprintf("_gg_ifcond%ld = GG_CMP_LESSEQ%s_L((%s), (%s), (%s));\n", gvar, caseins?"_C":"", n_cm, lesseq, len); 
        }
        else if (greq_pos == m) 
        { 
            carve_statement (&greq, "if-true", GG_KEYGREATEREQUAL, 1, 1); 
            // n_cm is so that cm is preserved for the next loop of search for the next or/and
            // make_mem for n_cm must be repeated for each condition b/c it must be after carve_statements
            char *n_cm = cm;
            make_mem (&n_cm);
            make_mem (&greq);
            //
            gg_num t1 = check_var (&n_cm, GG_DEFUNKN);
            check_var (&greq, t1);
            if (!len) oprintf("_gg_ifcond%ld = GG_CMP_GREQ%s((%s), (%s));\n", gvar, caseins?"_C":"", n_cm, greq); 
            else oprintf("_gg_ifcond%ld = GG_CMP_GREQ%s_L((%s), (%s), (%s));\n", gvar, caseins?"_C":"", n_cm, greq, len); 
        }
        else if (less_pos == m) 
        {
            carve_statement (&less, "if-true", GG_KEYLESSERTHAN, 1, 1); 
            // n_cm is so that cm is preserved for the next loop of search for the next or/and
            // make_mem for n_cm must be repeated for each condition b/c it must be after carve_statements
            char *n_cm = cm;
            make_mem (&n_cm);
            make_mem (&less);
            //
            gg_num t1 = check_var (&n_cm, GG_DEFUNKN);
            check_var (&less, t1);
            if (!len) oprintf("_gg_ifcond%ld = GG_CMP_LESS%s((%s), (%s));\n", gvar, caseins?"_C":"", n_cm, less); 
            else oprintf("_gg_ifcond%ld = GG_CMP_LESS%s_L((%s), (%s), (%s));\n", gvar, caseins?"_C":"", n_cm, less, len); 
        }
        else if (notmod_pos == m) 
        { 
            carve_statement (&notmod, "if-true", GG_KEYNOTEVERY, 1, 1); 
            // n_cm is so that cm is preserved for the next loop of search for the next or/and
            // make_mem for n_cm must be repeated for each condition b/c it must be after carve_statements
            char *n_cm = cm;
            // no need for make_me since these must be numbers
            //
            gg_num t1 = check_var (&n_cm, GG_DEFNUMBER);
            check_var (&notmod, t1);
            oprintf("_gg_ifcond%ld = ((%s)%%(%s));\n", gvar, n_cm, notmod); 
        }
        else if (mod_pos == m) 
        { 
            carve_statement (&mod, "if-true", GG_KEYEVERY, 1, 1); 
            // n_cm is so that cm is preserved for the next loop of search for the next or/and
            // make_mem for n_cm must be repeated for each condition b/c it must be after carve_statements
            char *n_cm = cm;
            // no need for make_me since these must be numbers
            //
            gg_num t1 = check_var (&n_cm, GG_DEFNUMBER);
            check_var (&mod, t1);
            oprintf("_gg_ifcond%ld = !((%s)%%(%s));\n", gvar, n_cm, mod); 
        }
        else if (gr_pos == m) 
        { 
            carve_statement (&gr, "if-true", GG_KEYGREATERTHAN, 1, 1); 
            // n_cm is so that cm is preserved for the next loop of search for the next or/and
            // make_mem for n_cm must be repeated for each condition b/c it must be after carve_statements
            char *n_cm = cm;
            make_mem (&n_cm);
            make_mem (&gr);
            //
            gg_num t1 = check_var (&n_cm, GG_DEFUNKN);
            check_var (&gr, t1);
            if (!len) oprintf("_gg_ifcond%ld = GG_CMP_GR%s((%s), (%s));\n", gvar, caseins?"_C":"", n_cm, gr); 
            else oprintf("_gg_ifcond%ld = GG_CMP_GR%s_L((%s), (%s), (%s));\n", gvar, caseins?"_C":"", n_cm, gr, len); 
        }
        else if (cont_pos == m) 
        { 
            carve_statement (&cont, "if-true", GG_KEYCONTAIN, 1, 1); 
            // n_cm is so that cm is preserved for the next loop of search for the next or/and
            // make_mem for n_cm must be repeated for each condition b/c it must be after carve_statements
            char *n_cm = cm;
            make_mem (&n_cm);
            make_mem (&cont);
            //
            check_var (&n_cm, GG_DEFSTRING);
            check_var (&cont, GG_DEFSTRING);
            oprintf("_gg_ifcond%ld = (GG_CONTAINS%s((%s), (%s))!=NULL);\n", gvar, caseins?"_C":"", n_cm, cont);
        }
        else if (notcont_pos == m) 
        { 
            carve_statement (&notcont, "if-true", GG_KEYNOTCONTAIN, 1, 1); 
            // n_cm is so that cm is preserved for the next loop of search for the next or/and
            // make_mem for n_cm must be repeated for each condition b/c it must be after carve_statements
            char *n_cm = cm;
            make_mem (&n_cm);
            make_mem (&notcont);
            //
            check_var (&n_cm, GG_DEFSTRING);
            check_var (&notcont, GG_DEFSTRING);
            oprintf("_gg_ifcond%ld = (GG_CONTAINS%s((%s), (%s))==NULL);\n",gvar, caseins?"_C":"", n_cm, notcont);
        }
        else gg_report_error ("Internal error in logic condition");

        if (op != INT_MAX) 
        {
            if (is_or) { oprintf ("if (!_gg_ifcond%ld) {\n",  gvar); cm = or; }
            else if (is_and) { oprintf ("if (_gg_ifcond%ld) {\n", gvar); cm = and; }
            else gg_report_error ("Internal error in logic condition");
            tot_open++;
        }
        else
        {
            // last and/or which determines the whole condition, or there was no and/or(just a single condition)
            oprintf ("_gg_if_stmt%ld = _gg_ifcond%ld;\n", gvar_stmt, gvar);

        }
        gvar++;
        if (op == INT_MAX) break;
    }
    gg_num i;
    for (i = 0; i < tot_open; i++) 
    {
        if (is_or) oprintf("} else  _gg_if_stmt%ld = true;\n", gvar_stmt);
        else if (is_and) oprintf("} else  _gg_if_stmt%ld = false;\n", gvar_stmt);
    }
    oprintf("\n%s _gg_if_stmt%ld %s\n", ifs, gvar_stmt, ife); // final result
    gvar_stmt++;
}


//
// Check if currently processed file name (file_name) matches request name (reqname). Error out if not.
//
void match_file (char *file_name, char *reqname)
{
    // file_name and reqname are decorated, so no /, they are just plain flattened names, no need for basename. So in most cases just a copy of file_name would suffice. HOWEVER, in some cases, like for dispatcher, the actual /var/lib/gg/lib/... file name is passed as file_name - this is JUST for generated files - and reqname is just the name, so getting basename works for that. But for regular source files, it just makes a copy
    char *base = gg_basename (file_name);
    char *dot = strchr (base, '.');
    if (dot == NULL)  gg_report_error( "Source file name [%s] must have .golf extension", file_name);
    *dot = 0; // cut file_name short for easy comparison
    // do not use source file name b/c gg_report_error will print it properly
    char *r = undecorate (reqname);
    char *b = undecorate (base);
    if (gg_single_file) // must be a full match for --single-file in gg
    {
        if (strcmp (reqname, base)) gg_report_error( "Source file name does not match request handler name [%s]", r);
    }
    else
    {
        gg_num base_len = strlen(base);
        // request name has to match the base in the beginning and either end there, or have __ afterwards for / (since these are undecorated names)
        if (strncmp (reqname, base, base_len) || (!strncmp (reqname, base, base_len) && !(reqname[base_len] == 0 || (reqname[base_len] == '_' && reqname[base_len+1] == '_')))) gg_report_error( "Request handler name [%s] does not begin with source file path [%s]", r, b);
    }
    gg_free (r);
    gg_free (b);
    gg_free (base);
}

//
// Check the result of parsing (after last carve_statement()), so that mtext is trimmed,
// and if has_value is true, check that it's not empty
// is_constant is true if this can be a string constant, false otherwise
// Returns length of mtext
// This MUST ALWAYS be after ALL other carve_statement calls!!!!! Or otherwise junk may just pass parsing as okay (even if it doesn't do anything)!
//
gg_num carve_stmt_obj (char **mtext, bool has_value, bool is_constant)
{
    // Make sure mtext is not empty, there has to be something
    gg_num ml = strlen (*mtext);
    *mtext = gg_trim_ptr(*mtext,  &ml);
    if (has_value && (*mtext)[0] == 0) gg_report_error("Object argument following the statement name is missing");
    if (!has_value && (*mtext)[0] != 0) gg_report_error("This statement should have no object argument");
    if (is_constant) make_mem (mtext);
    return ml;
}


//
// Make variable out of expression *v of type 'type'. *v contains the name of newly generated variable name.
// This prevents double evaluation, as whatever is used is assigned once (and only if it's an expression).
// This is for input params only.
//
void make_var (char **v, gg_num type)
{
    static gg_num gg_dexp = 0; // counter to create variables to prevent double evaluation problem
    char *var;
#define GG_MAX_VEXP_LEN 50
    var = gg_malloc(GG_MAX_VEXP_LEN);
    //
    //
    // The name of this variable has its type encoded in the name. We use this in check_var
    // so *do not* change this naming covention (_gg__<code>__<some counter>) without a **very** good reason!!
    //
    //
    snprintf (var, GG_MAX_VEXP_LEN, "_gg__%ld__dexp%ld", type, gg_dexp); 
    char rtype[100];
    bool simple_type = true;
    if (cmp_type(GG_DEFSTRING, type)) strcpy (rtype, "char*");
    else if (cmp_type(GG_DEFNUMBER, type)) strcpy (rtype, "gg_num");
    else if (cmp_type(GG_DEFBOOL, type)) strcpy (rtype, "bool");
    else { simple_type = false; snprintf(rtype, sizeof(rtype), "typeof (%s)", *v); }
    oprintf( "%s %s = %s;\n", rtype,  var, *v);
    char *realname = *v; // MUST be before *v=var below or otherwise realname would be gg_dexp...
    *v = var;
    // add new variable gg_dexp... to the hash of variables only if simple type, because we can't specify constants for other types in code anyway
    if (simple_type)
    {
        add_var (*v, type, realname, false);
        oprintf( "GG_UNUSED(%s);\n", *v);
    }
    gg_dexp++;
}


//
// When we're sure v is boolean, we check it and either return or error out
//
char *process_bool_exp (char *v)
{
    if (!strcmp (v, "true") || !strcmp (v, "false")) return v;
    bool found;
    bool err;
    check_bool (v, &found, &err);
    if (!err) 
    {
        make_var (&v, GG_DEFBOOL); // create new variable to avoid double evaluation (say if *var is 'gg_strdup(..)'!)
        return v; // new variable instead of expression
    }
    else 
    {
        gg_report_error ("Syntax error in boolean expression [%s]", v);
    }
}

//
// When we're sure v is string, we check it and either return string var to be used or error out
//
char *process_string_exp (char *v)
{
    // check if string expression
    bool err;
    char *res = check_str (v, &err);
    if (!err)
    { 
        return res; 
    } else gg_report_error ("Expression [%s] is not of string type", v); 
}

//
// When we're sure v is string, we check it and either return string var to be used or error out
//
char *process_number_exp (char *v)
{
    bool found;
    bool err;
    check_exp (v, &found, &err);
    if (!err) make_var (&v, GG_DEFNUMBER); // create new variable to avoid double evaluation (say if *var is 'gg_strdup(..)'!)
    else gg_report_error ("Syntax error in number expression [%s]", v);
    return v;
}

//
// Make sure every object/clause argument is a variable of some type. String constants have been converted to variables at this point.
// However, numbers are still possible here.
// type if GG_DEFNUMBER, GG_DEFSTRING etc. in which case variable v is checked to be of this type and errors out if not
// returns type of v
// If type if GG_DEFUNKN, then return type found is returned and of course no check is performed
// check_var must be called after any define_statement, and it must be after any make_mem as it checks variables
// and not constants.
// Note: internal variables MUST start with '_gg' to be taken as correct!
// IMPORTANT: check_var is ONLY for input variables; NEVER for output, which is handled with define statement!!!!! The only exception is use with type of GG_DEFUNKN to get the type.
//
gg_num check_var (char **var, gg_num type)
{
    
    char *v = *var;
    if (v == NULL) return type; // when a clause is not found (so find_keyword returns NULL) we still check variable
                                // and so if NULL (which it would be if not found), just rubber stamp it

    // trim expression and check if it's missing
    gg_num lv = strlen (v);
    v = gg_trim_ptr(v,  &lv);
    if (v[0] == 0) return type; // in some cases like with opt_clause(), the variable is not present (like case-insensitive in count-substring)
                                // and it's actuall "", and not NULL , so just rubberstamp it
                               

    // if the var has () around it, remove it. There's no need for it, other than to use the same name
    // for a variable or function as is a keyword. If really needed, use (()), so the inner ones remain
    if (lv >= 3 && v[0] == '(' && v[lv-1] == ')')
    {
        // remove trailing )
        v[lv - 1] = 0;
        // We check what's in (..) because it could be (gg_num)strlen(x), which starts with ( and ends with ) but isn't a variable!
        // (this is only relevant in internal mode)
        // remove leading ( 
        v++;
        *var = v; // we altered the location of var, make it correct
    }


    if (v[0] == '_' && v[1] == 'g' && v[2] == 'g')  
    {
        if (!strncmp (v, "_gg__", 5)) // this is internal variable with type encoded in name
        {
            char *tcode = v+5;
            char *ecode = strstr (tcode, "__");
            if (ecode != NULL)
            {
                *ecode = 0;
                gg_num code = atol (tcode);
                *ecode = '_';

                return code;
            }
        }
        return type; // internal vars are always of the right type (aka _gg...)
                     // but not "gg_..." vars, those are synonyms for user vars
    }
    if (cmp_type (type, GG_DEFSTRING)) // we check this for internal too. We however, detect if the expression is in the 
                                       // form a+b+... where each is a string var, or just a string, or...
    {
        *var = process_string_exp (v);
        return GG_DEFSTRING;
    }

    if (cmp_type (type, GG_DEFNUMBER))
    {
        // check if number expression
        *var = process_number_exp (*var);
        return GG_DEFNUMBER;
    }

    if (cmp_type(type, GG_DEFBOOL))
    {
        // or true false
        *var = process_bool_exp (v);
        return GG_DEFBOOL;
    }

    //
    // Here, we now determine the type of expression
    //

    // Get passed any left parenthesis
    gg_num i = 0;
    while (v[i] == '(') i++;
    gg_num ibeg = i; // beginning of something to check

    if (v[ibeg] == '"') // check if starts with double quote, and interpret as string expression
    {
        if (is_constant_string(v) == GG_CONST_OK) { *var = v; return GG_DEFSTRING; } // must be string
        else gg_report_error ("Syntax error in a string literal [%s]", v);
    }
    else if (v[ibeg] == '!') // this is partial recognition of boolean expression, just for negation
    {
        *var = process_bool_exp (v+ibeg);
        return GG_DEFBOOL;
    }
    else if (isdigit(v[ibeg]) || v[ibeg] == '-' || v[ibeg] == '+') // check if starts with digit, + or -, and interpret as number expression
    {
        *var = process_number_exp (*var);
        return GG_DEFNUMBER;
    }
    else if (isalpha(v[ibeg]))
    {
        i = ibeg;
        i++; // we already know v[ibeg] is alpha
        while (isalnum(v[i]) || v[i] == '_') i++; // advance to catch the name of variable, stop when anything else found
                                                    // (such as [ or + or ) or whatever

        char char_end = v[i]; // remember byte we temporarily nullify
        v[i] = 0; // put the zero in ending to cap var name
        char *firstn = gg_strdup (v+ibeg); // remember first variable name, so we don't have to save+nullify+restore a byte!
        v[i] = char_end; // restore byte right after name
        while (isspace(v[i])) i++; // advance character after the name to skip spaces
        gg_num var_end = i; // and now var_end is position of whatever comes after name, after spaces skipped; and we have the name of variable
                     // it may be the same as passed_end, or maybe a bit after it if there were spaces passed variable name
        if (gg_is_valid_param_name (firstn, false, false) != 1) 
        {
            gg_report_error ("Unknown variable or expression [%s]", firstn);
        }
        else
        {
            gg_num tp;
            gg_num lnum;
            // Start checking with string, it could be any type really, but string is probably the most common
            if (var_exists (firstn, GG_DEFSTRING, &tp, &lnum)) 
            {
                // check if what follows is [, in which case it's a number
                if (v[var_end] == '[')
                {
                    *var = process_number_exp (v);
                    return GG_DEFNUMBER;
                }
                else
                {
                    *var = process_string_exp (v);
                    return tp;
                }
            }
            else if (tp == GG_DEFBOOL || tp == GG_DEFBOOLSTATIC)
            {
                *var = process_bool_exp (v);
                return tp;
            }
            else if (tp == GG_DEFNUMBER || tp == GG_DEFNUMBERSTATIC)
            {
                *var = process_number_exp (*var);
                return tp;
            } else return tp; // just return type found, other types do NOT have expressions (like trees, hashes etc)
        }
    } else gg_report_error ("Unknown expression type or bad syntax for an expression");

    return GG_DEFUNKN; //  will never get here, just for compiler sanity
}



//
// Generate code that loads pre-computed hash to memory. This hash allows near-instantaneous
// request routing in Golf's dispatcher. The entire hash table is then in a data segment of the program,
// being loaded with large memcpy, and then used. So there is no loading of hash, only querying. There is
// also no deleting, resizing etc. - this method is for hash table that stay loaded and *unchanged* for
// the life of the process.
// fname is the file containing the list of names to hash, one line at a time
// hash is the name of the hash generated.
// is_req is true if this is a list of requests for dispatcher
// Here we also check that all file names in the app have proper names. We do another check in v1 input to 
// see if the source file name is proper named.
//
void setup_internal_hash(char *fname, char *hash, bool is_req)
{
    // Build request list file name. This file is built in gg -q prior to calling v1 for processing.
    // The first line of this file is the number of request names that follow, each in a separate line.
    char gg_req_file[300];
    snprintf (gg_req_file, sizeof(gg_req_file), "%s/%s", gg_bld_dir, fname);

    FILE *f = fopen (gg_req_file, "r");
    if (f == NULL) gg_report_error( "Error [%s] opening file [%s]", strerror (errno), gg_req_file);

    char line[GG_MAX_REQ_NAME_LEN]; // line to read from fname
    gg_hash *gg_hardhash; // hash we will build at compile time, and then make a memory representation of it
    bool done_count = false; // true when first line (count of requests) read
                             //
    while (1)
    {
        char *req = fgets_unlocked (line, sizeof (line), f);
        if (req == NULL) // either end or error
        {
            gg_num err = ferror_unlocked (f);
            if (err) // if error, quit
            {
                gg_report_error( "Error [%s] reading file [%s]", strerror (errno), gg_req_file);
            }
            break; // nothing read in, done with hash building
        }
        trimit (req);     
        if (!done_count) 
        {
            done_count = true;
            gg_create_hash (&gg_hardhash, atol (req), NULL, false); // create hash, first line is the
                                                          // number of elements in the hash
        }
        else
        {
            // check if file name is proper, as it's converted into request name
            if (is_req && gg_is_valid_param_name (req, false, false) != 1) gg_report_error (GG_MSG_SRC_NAME, req);
            // must use gg_strdup() so that each new element is new memory, otherwise
            // new req would override all previous additions
            if (is_req) gg_add_hash (gg_hardhash, gg_strdup(req), NULL, GG_EMPTY_STRING, NULL, NULL);
            else 
            {
                // this is the address of index into param array added as value associated with hash key (which is the name of param), _gg_aprm_xxxx is generated in vmakefile
                char hdata[300];
                snprintf(hdata, sizeof(hdata), "(void*)&_gg_aprm_%s", req);
                gg_add_hash (gg_hardhash, gg_strdup(req), NULL, gg_strdup(hdata), NULL, NULL);
            }
        }
    }
    // Now that we have the hash, we need to generate the code that unwinds this process into a 
    // list of static buckets that get loaded in data text of the program.
    // hash is deterministic; the buckets we get here is what gets computed at run time
    // we're just saving time precomputing them at compile time, i.e. here.

    // gg_hardhash is a global hash ptr variable, we declare a pointer to it as hash ops expect a pointer
    oprintf ("gg_hash *%s_ptr = &(%s);\n", hash, hash);
    gg_num i;
    // Calculate how many hash buckets are empty. We still need to have a representation for them, as they
    // are part of the hash image we want to recreate in a running Golf program.
    gg_num empty = 0;
    for (i = 0; i < gg_hardhash->num_buckets; i++)
    {
        if (gg_hardhash->table[i] == NULL) empty++;
    }

    // harr is the total number of entries we must declare. There's ->tot (total number of elements stored
    // in the hash) plus the empty buckets, plus one for the ending.
    gg_num harr = gg_hardhash->tot+1+empty;

    // Declare internal hash table. Normally, this is allocated at run-time for a hash, but Golf API allows
    // for it to be provided as a fixed array. We generate the code for it with the initializator (not an assignment)
    // so that it can be linked as a data segment and copied extremely fast into a running program for use.
    oprintf ("gg_hash_table _%s_arr[%ld] = { ", hash,harr);
    // We do not allocate memory. Instead, any overflow bucket elements are stored as the trailing element in the 
    // array, and we manipulate the ->next to point to them. So the hash structure is NOT exactly the same as it would
    // have been with the normal hash. This is faster, since there's no allocation, and memory is in a single contiguous block.
    gg_num bottom = gg_hardhash->num_buckets; 
    gg_num nx;
    // Go through all buckets
    for (i = 0; i < gg_hardhash->num_buckets; i++)
    {
        char *next;
        char next_s[300];
        // Start with each leading bucket element
        gg_hash_table *t = gg_hardhash->table[i];
        gg_num curr_i = i;
        // Go through the list of next element with the same hash
        while (t != NULL) 
        {
            if (t->next == NULL)
            {
                next = "NULL"; // nothing after it
                nx = -1;
            }
            else
            {
                // all other buckets are in the same array, so memory copy into data segment will work on program load
                // this way there is no pointer translation and memory fetching is very fast.
                // nx is the ->next element. It is at the current bottom of the array, which we advance with each added ->next
                // we will process that one (nx) next, by setting curr_i, and t=t->next, which refer to the same element.
                snprintf (next_s, sizeof(next_s), "&(_%s_arr[%ld])", hash,nx=bottom++);
                next = next_s; // [].next for the current element
            }
            // initialize the current hash element
            if (is_req) oprintf ("[%ld].key = \"%s\", [%ld].data=%s, [%ld].next = %s,\n", curr_i, t->key, curr_i, t->key, curr_i, next);
            else oprintf ("[%ld].key = \"%s\", [%ld].data=%s, [%ld].next = %s,\n", curr_i, t->key, curr_i, (char*)(t->data), curr_i, next);
            // set the -> element which is processed next, and nx and t refer to the same element. We do this until end of list,
            // then move on to the next bucket
            curr_i = nx;
            t = t->next;
        }
        // after done, going to next bucket (advance i)
    }
    // add the final one as an end of list (not used for now)
    oprintf ("[%ld].key = \"\", [%ld].data=NULL, [%ld].next = NULL\n", harr-1, harr-1, harr-1);
    // unused is because if the project doesn't have any params, then this causes unused variable error
    oprintf(" };\nGG_UNUSED(_%s_arr);\n", hash); // finish the list
    // What we created so far is the entire hash bucket structure, including overflow elements
    // What hash API expects is a list of pointers that point to buckets.
    // That's what we initialize here, an array of pointers to buckets, and that's what we use
    // to create a request hash, which then can be queried
    oprintf ("gg_hash_table *_%s_arr_ptr[%ld] = { ", hash,harr-1);
    for (i = 0; i < gg_hardhash->num_buckets; i++) 
    {
        // for those buckets that are NULL, we must have NULL too in runtime hash table, or otherwise
        // the key in such a non-NULL bucket would be NULL, causing Golf to crash in hash.c(142) when comparing a key
        if (gg_hardhash->table[i] == NULL) oprintf ("%s NULL \n", i!=0?",":""); else oprintf ("%s &(_%s_arr[%ld]) \n", i!=0?",":"", hash, i);
    }
    oprintf ("};\n");
    // generate create hash
    oprintf ("gg_create_hash (&%s_ptr, %ld, _%s_arr_ptr, false);\n", hash, gg_hardhash->tot, hash); 
    

    // hash is typically a fairly small structure. We do not delete it here, as it would just slow down processing.
    fclose (f);
}

//
// Print out a number or write it to a string. 'to' is a string where to write, can be NULL for printing out.
// 'to' is allocated (unless it's NULL, then it's printed out).
// num0 is the number to convert to string. base is
// the base, 2-36, default 10.
// _gg_st is set to be the status of conversion on return
//
void do_numstr (char *to, char *num0, char *base)
{
    // to == NULL means print it out. 
    bool go_out = false;
    if (to == NULL)
    {
        go_out = true;
        oprintf ("{\nchar *_gg_numstr;\n");
        to = "_gg_numstr";
    }

    oprintf ("%s=gg_num2str (%s, &_gg_st, %s);\n", to,num0, base!=NULL?base:"10");
    if (go_out) 
    {
        //_gg_st (i.e. the length of string) is never <0
        oprintf ("gg_puts (GG_NOENC, %s, _gg_st, true);\n", to);
        oprintf ("gg_free_int (%s);\n", to); // for printing, release memory right away

        oprintf ("}\n");
    }
}

//
// Convert gg_puts (...) with only a string into gg_puts(..., length) for better output performance
// Basically, all literals are strlen-ed at compile time so there's no strlen() at run-time
// pline is the line to convert. This is done by replacing GG_EMPTY_LONG_PLAIN_ZERO with the actual 
// number (plus some spaces), so there is never a question if there's enough memory (there is since this constant
// is bigger than any number). Also, this constant is used here ONLY with "..." (string constants), so we know just before
// this constant is a comma, and before that a string. This makes for easy replacement with constant's length.
// The nice thing here is even if we miss some, it still works, though a bit slower.
//
void convert_puts(char *pline)
{
    char *zero;
    static int elen = strlen ("GG_EMPTY_LONG_PLAIN_ZERO");

    char *search = pline;
    while (1) 
    {
        // look for next gg_puts placeholder for length
        char *ozero = strstr (search, "GG_EMPTY_LONG_PLAIN_ZERO");
        zero = ozero;
        if (zero == NULL) break; // done, none found any more
        search = ozero+elen; // setup for next search cycle
        //
        // first a comma first going back
        //
        while (zero-- != pline) if (isspace(*zero)) continue; else break;
        if (*zero != ',')  continue; // must find comma first
        if (zero == pline) return; // none found, stop
        //
        // first a quote next going back
        // the next nonspace char behind must be quote
        //
        while (zero-- != pline) if (isspace(*zero)) continue; else break;
        if (*zero != '"')  continue; // must find quote, not found (this is an expression), continue
        if (zero == pline) return; // none found, stop
        //
        // go back, look for next quote, and process escaped chars
        //
        gg_num totstr = 0;
        char *i = zero - 1; // start with one before quote
        while (*(i-1) != 0)  // look for one before, since we do
                                 // look-behind with escape and don't want to go to
                                 // memory before pline!
        {
            if (*(i-1) == '\\') { i-=2; totstr++; continue; } // just skip whatever's escaped
            if (*i == '"') break;
            i--;
            totstr++;
        }
        // It's possible to have an expression like x[1]=='x'?"a":"b", which isn't a constant or even
        // "abc"[x] == y ?"a":"b", which starts and ends with quote; so the only criteria is that after the 
        // leading quote, we find gg_puts (XXX, 
        while (i-- != pline) if (isspace(*i)) continue; else break;
        // comma means we're done, because string literal by itself is NOT an expression, so this wouldn't be valid C
        if (*i != ',')  continue; // must find comma, per above 
        //
        // put in the length
        //
        char after = ozero[elen];
        sprintf (ozero, "%*ld", elen, totstr); 
        ozero[elen] = after; // restore char after the length, since we put null char in sprintf
    }
}


//
// Returns code for optional clause. This is a clause where it may not have any data
// or it may. For example:
// set-cookie ... secure
// or
// set-cookie ... secure true|false
// If true|false, then it's either secure or not. Without true|false then it's secure.
// This helps write easier code.
// clause is "secure" in this case (the clause), "param" is what is generated if it applies
// and "antiparam" is what is generated if it does not apply which is if clause is NULL or
// it's there but false.
//
char *opt_clause(char *clause, char *param, char *antiparam, gg_num type)
{
    char *res = NULL;
    if (clause != NULL)
    {
        if (clause[0] != 0) // there is true|false
        {
            res = gg_malloc(strlen(clause)+strlen(param)+strlen(antiparam) + 30);
            check_var (&clause, GG_DEFBOOL); // make sure it's a boolean variable
            sprintf (res, "((%s)?(%s):(%s))", clause, param, antiparam);
            make_var (&res, type);
        }
        else // clause is present but no true|false
        {
            res = gg_strdup(param);
        }
    }
    else // clause is not present (NULL)
    {
        res = gg_strdup(antiparam);
    }
    return res;
}

//
// This processes query-result statement, see documentation
//
void query_result (gg_gen_ctx *gen_ctx, char *mtext, gg_num cur_query_result)
{
    gg_db_parse vp;
    memset  (&vp, 0, sizeof(gg_db_parse)); // all are NULL now

    vp.query_result = gg_strdup (mtext);
    vp.cur_query_result = cur_query_result;

    process_query (gen_ctx, 1, 0, 0, &vp);
}

//
// Checks if app-path is valid
// Valid paths are considered to have only alphanumeric characters and
// underscores, hyphens and forward slashes.
// Must end with "/appname"
// Returns 1 if name is valid, 0 if not.
//
void gg_is_valid_app_path (char *name)
{
    GG_TRACE ("");

    gg_num i = 0;
    while (name[i] != 0)
    {
        if (!isalnum(name[i]) && name[i] != '_' && name[i] != '-' && name[i] != '/') gg_report_error ("Application path is not valid; it can contain only alphanumeric characters, underscores, hyphens and forward slashes");
        i++;
    }
    gg_num l = (gg_num)strlen(gg_app_name);
    // app patch must end with /<app name>
    if (i < l+1 || strcmp (name+i-l, gg_app_name) || name[i-l-1] != '/') gg_report_error ("Application path [%s] must end with /%s", name, gg_app_name);
}


//
// Reads stdin, and substs $<curlybrace>XYZ<curlybrace> to the stdout. Max var len 250. Non-existent is empty. 
// Anything bad with $<curlybrace>XYZ<curlybrace> just
// copied to the stdout. XYZ can be alphanumeric or underscore.
// This does NOT and MUST NOT use gg_malloc. If it does, the calling of envsub in main() must be after the mem init.
//
void envsub () 
{
    char varname[256];
    gg_num varlen = 0;
    while (1)
    {
        // read input until error or EOF
        int c = fgetc(stdin);
        if (c == EOF) break;
        // Treat dollar separately, anything else goes to stdout
        if (c == '$')
        {
            // Expecting open brace after $
            int c = fgetc(stdin);
            // If end of file, print $ out and exit
            if (c == EOF) { fputc('$', stdout); break; }
            // Start of variable $<curlybrace>...<curlybrace>
            if (c == '{')
            {
                // get variable name
                varlen = 0;
                while (1)
                {
                    // one by one char variable name
                    int c = fgetc(stdin);
                    // check if variable name ended with closed brace
                    if (c == '}')
                    {
                        if (varlen == 0)
                        {
                            // nothing in it, just print out weird construct
                            fputs_unlocked ("${}", stdout);
                            break;
                        }
                        else
                        {
                            // found var name, get its value, print it out. Account for empty or non-existing.
                            varname[varlen] = 0;
                            char *ev = secure_getenv (varname);
                            if (ev == NULL) ev="";
                            fputs_unlocked (ev, stdout);
                            break;
                        }
                    }
                    // If this is isn't a variable, print it all out. It could be the end of stream,
                    // variable name isn't alphanum or underscore, or too long. We just then ignore it and print it out.
                    if (c == EOF || !(isalnum(c) || c == '_') || varlen >= (gg_num)sizeof(varname)-2)
                    {
                        fputs_unlocked ("${", stdout);
                        if (c != EOF) varname[varlen++] = c; // include the offending char unless EOF
                        varname[varlen] = 0;
                        fputs_unlocked (varname, stdout);
                        break;
                    }
                    // Add read-in char to variable name
                    varname[varlen++] = c;
                }
            } else 
            {
                // this dollar with something other than <curlybrace> following
                fputc ('$', stdout);
                fputc (c, stdout);
            }
        } else fputc(c, stdout); // not a dollar
    }
}


//
// Based on on-error statement on_error, determine if it's continue or exit (or error out if neither)
// on_error_cont/exit is carved out on-error-cont/exit value, act is either GG_ON_ERROR_CONTINUE/EXIT or GG_OKAY
//
void on_error_act (char *on_error_cont, char *on_error_exit, char *act)
{
    if (on_error_cont != NULL) *act = GG_ON_ERROR_CONTINUE;
    else if (on_error_exit != NULL) *act = GG_ON_ERROR_EXIT;
    else *act = GG_OKAY;

    // check for sanity
    if (on_error_cont != NULL && on_error_exit != NULL) gg_report_error ("on-error can be either 'continue' or 'exit', but not both");
}

//
// Generate code to create and init default gg_header. 'inittype' is GG_HEADER_FILE for file serving, i.e.
// when there's caching; GG_HEADER_PAGE for page serving, i.e when there's no caching.
// http_header_count is the ID of the temp gg_header-type.
// Returns the name of the gg_header variable for which the code is generated.
// request is 0 if this is reply header, or 1 if request. Some http headers are just for reply and vice versa.
//
char *make_default_header(int inittype, gg_num http_header_count, char request)
{
    static char temp_header[200];
    if (inittype != GG_HEADER_FILE && inittype != GG_HEADER_PAGE) gg_report_error( "unsupported header type [%d] (internal)", inittype);
    snprintf (temp_header, sizeof(temp_header), "_gg_tmp_header_%ld", http_header_count); 
    oprintf ("gg_header %s;\n", temp_header);
    // by default it is to show the file (disposition is empty)
    // use disposition as "attachment" or "download" for download
    oprintf ("gg_init_header (&(%s), %d, %d) ;\n", temp_header, inittype, request);
    return temp_header;
}

//
// Generates code that constructs gg_header variable. User can specify content-type, download, etag and others.
// Also allows for "custom" header in the form of expr=expr,... making it easy to create custom headers.
// "statement" is the name of the statement (such as "send-file"). "header" is the actual text of headers clause in the
// statement. temp_header is the name of the gg_header var generated via make_default_header().
// http_header_count is the ID of the temp gg_header-type, the same as used in make_default_header().
// request is 0 if this is reply header, or 1 if request. Some http headers are just for reply and vice versa.
// content_type is content-type if set, NULL otherwise,
// content_len is content-length if set, NULL otherwise (originally gg_header was to be used to set the values, but
// there is an issue of types: here all types are char*, while at run-time, they can be different); can be NULL.
//
void process_http_header (char *statement, char *header, char *temp_header, gg_num http_header_count, char request, char **content_len, char **content_type)
{
    if (content_len != NULL) *content_len = NULL;
    if (content_type != NULL) *content_type = NULL;
    if (header != NULL)
    {
        // process header clause
        char *ctype = NULL;
        char *clen = NULL;
        char *download = NULL;
        char *cachecontrol = NULL;
        char *nocache = NULL;
        char *etag = NULL;
        char *filename = NULL;
        char *statusid = NULL;
        char *statustext = NULL;
        char *custom = NULL;
        // get fixed fields
        char *mheader = gg_strdup (header);
        if (request == 0)
        {
            download = find_keyword (mheader, GG_KEYDOWNLOAD, 1);
            etag = find_keyword (mheader, GG_KEYETAG, 1);
            filename = find_keyword (mheader, GG_KEYFILENAME, 1);
            cachecontrol = find_keyword (mheader, GG_KEYCACHECONTROL, 1);
            nocache = find_keyword (mheader, GG_KEYNOCACHE, 1);
            statusid = find_keyword (mheader, GG_KEYSTATUSID, 1);
            statustext = find_keyword (mheader, GG_KEYSTATUSTEXT, 1);
        }
        if (request == 1) 
        {
            clen = find_keyword (mheader, GG_KEYCONTENTLENGTH, 1);
        }
        ctype = find_keyword (mheader, GG_KEYCONTENTTYPE, 1);
        custom = find_keyword (mheader, GG_KEYCUSTOM, 1);
        if (request == 0)
        {
            carve_statement (&download, statement, GG_KEYDOWNLOAD, 0, 2);
            carve_statement (&etag, statement, GG_KEYETAG, 0, 2);
            carve_statement (&filename, statement, GG_KEYFILENAME, 0, 1);
            carve_statement (&cachecontrol, statement, GG_KEYCACHECONTROL, 0, 1);
            carve_statement (&nocache, statement, GG_KEYNOCACHE, 0, 0);
            carve_statement (&statusid, statement, GG_KEYSTATUSID, 0, 1);
            carve_statement (&statustext, statement, GG_KEYSTATUSTEXT, 0, 1);
        }
        if (request == 1) 
        {
            carve_statement (&clen, statement, GG_KEYCONTENTLENGTH, 0, 1);
        }
        carve_statement (&ctype, statement, GG_KEYCONTENTTYPE, 0, 1);
        carve_statement (&custom, statement, GG_KEYCUSTOM, 0, 1);

        // check_var MUST be after ALL carve_statements
        if (request == 1) 
        {
            check_var (&clen, GG_DEFNUMBER);
        }

        make_mem (&ctype);
        char *downloadc = opt_clause(download, "\"attachment\"", "NULL", GG_DEFSTRING);
        char *etagc = opt_clause(etag, "1", "0", GG_DEFNUMBER);
        if (request == 0)
        {
            make_mem (&filename);
            make_mem (&cachecontrol);
            make_mem (&statustext);
            //
            check_var (&filename, GG_DEFSTRING);
            check_var (&cachecontrol, GG_DEFSTRING);
            check_var (&statusid, GG_DEFNUMBER);
            check_var (&statustext, GG_DEFSTRING);
            check_var (&etagc, GG_DEFNUMBER);
            check_var (&downloadc, GG_DEFSTRING);
        }


        if (nocache != NULL && cachecontrol != NULL) gg_report_error( "both no-cache and cache-control specified, only one can be used");
        // gen header
        if (ctype != NULL) { if (content_len != NULL) *content_type = ctype;  oprintf ("(%s).ctype = %s;\n", temp_header, ctype); }
        if (clen != NULL) {if (content_len != NULL) *content_len = clen; oprintf ("(%s).clen = %s;\n", temp_header, clen);}
        if (download != NULL) oprintf ("(%s).disp = %s;\n", temp_header, downloadc);
        if (etag != NULL) oprintf ("(%s).etag = %s;\n", temp_header, etagc);
        if (filename != NULL) oprintf ("(%s).file_name = %s;\n", temp_header, filename);
        if (statusid != NULL) oprintf ("(%s).status_id = %s;\n", temp_header, statusid);
        if (statustext != NULL) oprintf ("(%s).status_text = %s;\n", temp_header, statustext);
        if (cachecontrol != NULL) oprintf ("(%s).cache_control = %s;\n", temp_header, cachecontrol);
        if (nocache != NULL) oprintf ("(%s).cache_control = \"max-age=0, no-cache\";\n", temp_header);
        gg_free(downloadc);
        gg_free(etagc);
        // get custom fields
        if (custom != NULL)
        {
            char *mcustom = gg_strdup (custom);
            // store custom headers
            char chead_var[100];
            snprintf (chead_var, sizeof (chead_var), "_gg_chead_arr%ld", http_header_count);
            gg_num tothead = outargs(mcustom, chead_var, "char *", 0, 1, GG_DEFSTRING, GG_DEFSTRING);
            gg_num i;
            gg_num j = 0;
            // This pointer copying, first to _gg_chead_arr, then to control/value will
            // be optimized away by gcc; no effect on performance
            for (i = 0; i < tothead; i+=2)
            {
                oprintf ("(%s).control[%ld]=_gg_chead_arr%ld[%ld];\n", temp_header, j, http_header_count, i);
                oprintf ("(%s).value[%ld]=_gg_chead_arr%ld[%ld+1];\n", temp_header, j, http_header_count, i);
                j++;
            }
        }
    }
}


//
// Generate code for 1) name=value,... statement, for example in call-web or 2) something,... statement
// for example in exec-program. args is the actual text of what's parsed in the statement.
// outname is the name of <type>* array where the results are stored in generated code. 
// startwith is 0 or 1, depending on whether to put the first element of the array
// in [0] or [1]. 'pair' is 0 for something,... or 1 for name=value,.... For name=value situation,
// array is filled so it has name,value,name,value,etc.
// 'something','name','value' can be quoted or in () in order to avoid conflicts.
// The last element in the generated array always has NULL in it.
// So type is the type of these elements.
// check_type is the type of an array to make sure each element conforms
// Returns the index of the last element in array, the one that has NULL in it (in the code generated for the array).
//
gg_num outargs(char *args, char *outname, char *type, gg_num startwith, char pair, gg_num check_type_val, gg_num check_type_name)
{


    gg_num exec_inputs=startwith; // we start with index 1 in order to fill in program name as arg[0] at run-time
                        // args[0] is filled with program name at run-time

    gg_num tote;
    if (args != NULL)
    {
        gg_num tot;
        // must duplicate string because we CHANGE curr_start in parse_param_list (because it removes
        // escaped quotes) and thus we change the 'line' (the current parsing line). As a result, we will
        // probably error out at the end of line because we will see null characters since the line has
        // shifted to the left during removal of escaped quotes).
        char *curr_start = gg_strdup(args);

        // parse program arguments
        gg_fifo *params = NULL;
        parse_param_list (curr_start, &params, &tot);
        // define run-time list of program arguments, account for the last NULL
        // also account double when there's x=y pair, since that'll take 2 entries
        // add startwith if not zero to account for pre-filled elements
        oprintf("%s%s[%ld];\n", type,outname, tote=((pair == 1 ? 2:1)*tot+1+startwith));

        char *value;
        while (1)
        {
            gg_num st = gg_retrieve (params, NULL, (void*)&value);
            if (st != GG_OKAY) break;

            if (pair == 1)
            {
                // find equal sign outside the quotes or parenthesis
                char *eq = find_keyword(value, "=", 0);
                if (eq == NULL)
                {
                    gg_report_error( "Name/value pair missing equal sign ('=') in [%s]", value);
                }
                *eq = 0;
                // generate code for name/value pair array (run time variable argument)
                make_mem (&value);
                check_var (&value, check_type_name);
                oprintf("%s[%ld] = %s;\n", outname, exec_inputs, value);
                exec_inputs++;
                char *equals = eq+1;
                make_mem (&equals);
                check_var (&equals, check_type_val);
                oprintf("%s[%ld] = %s;\n", outname, exec_inputs, equals);
                exec_inputs++;
            }
            else
            {
                // generate code for program arguments array (run time variable argument)
                make_mem(&value);
                check_var (&value, check_type_val);
                oprintf("%s[%ld] = %s;\n", outname, exec_inputs, value);
                exec_inputs++;
            }
        }
    }
    else
    {
        oprintf("%s%s[%ld];\n", type,outname, tote=startwith+1); // no list (otherthan prefilled
                                                                  // startwith, just one element for NULL
    }

    // final arg MUST be NULL
    oprintf("%s[%ld] = NULL;\n", outname, exec_inputs);
    if (exec_inputs >= tote) gg_report_error( "Internal error in building a list [%ld, %ld]", exec_inputs, tote);

    return exec_inputs;
}

//
// Check if (maybe) command c looks like golf, and error out if it does
// (because it's likely misspelled golf command)
// Check for 'ab..-cd..<space|eol>' or 'ab..-cd..#'
//
void check_golf (char *c)
{
    // first find the space or #, or end-of-line
    gg_num i = 0;
    while (!isspace(c[i]) && c[i] != '#' && c[i] != 0) i++;
    gg_num j;
    gg_num isgolf = 1;
    gg_num hasdash = 0;
    for (j = 0; j < i; j++)
    {
        if (c[j] == '-') hasdash = 1;
        if (!isalpha(c[j]) && c[j] != '-') {isgolf = 0; break;}
    }
    if (hasdash == 1 && isgolf == 1) 
    {
        gg_report_error ("Unrecognized Golf statement [%s]", c);
    }
}

//
// Trim string var in-place. Returns its length.
//
gg_num trimit(char *var)
{
    gg_num var_len = strlen (var);
    gg_trim (var, &var_len, false);
    return var_len;
}

//
// Return name of database based on its index in the list of used dbs (dbconn)
// Return empty if not found or not recognized
//
char *gg_db_vendor(gg_num dbconn)
{
    GG_TRACE("");
    if (dbconn < 0 || dbconn >= totconn ) return "";
    if (gg_get_config()->ctx.db->conn[dbconn].db_type == GG_DB_SQLITE) return "sqlite";
    if (gg_get_config()->ctx.db->conn[dbconn].db_type == GG_DB_MARIADB) return "mariadb";
    if (gg_get_config()->ctx.db->conn[dbconn].db_type == GG_DB_POSTGRES) return "postgres";
    return "";
}


//
// Create and return C variable name from file name 'file'
//
char *id_from_file (char *file)
{
    char *id = gg_strdup(file);
    gg_num i;
    if (!isalpha(id[0])) id[0] = '_';
    for (i = 1; i < (gg_num)strlen(id); i++)
    {
        if (!isalnum(id[i])) id[i] = '_';
    }
    return id;
}


// 
// Entire query processing is in here. All input params are here. 
// query_result, run_query  are the statements to process 
// vp contains all the pieces of any query statement parsed out
// is_prep is 1 if this is run-prepared-query, otherwise 0
// gen_ctx is execution context
//
void process_query (gg_gen_ctx *gen_ctx, const gg_num query_result, const gg_num run_query, char is_prep, gg_db_parse *vp)
{

    // query_id is the location in a stack-array of a query. First query is 0, the one inside it is 1, inside it 2 etc.
    // once the loop ends, goes back to 1, 0. It is -1 if no query is active.
    // It is calculated on the fly, when run-query is used, and just used for query-result and current-row or any other

    if (run_query == 1)
    {
        // create new query, query id
        prepare_query (gen_ctx, vp);

        // perform most of query code generation, loop generation, result processing
        // also update query_id to keep track of query loops
        query_execution (gen_ctx, run_query, query_result, is_prep, vp);
        return;

    }
    else if (query_result == 1)
    {
        // Now check for query-result
        if (query_id == -1)
        {
            gg_report_error( "Query usage attempted, but no query present and active");
        }
        query_execution (gen_ctx, run_query, query_result, is_prep, vp);
        return;
    }
}


//
// Name a query. A query is numbered (as in 0,1,2..)
// gen_ctx is a query context, vp is the context of a parsed run-query statement
//
void name_query (gg_gen_ctx *gen_ctx)
{
    snprintf (gen_ctx->qry[query_id].name, sizeof (gen_ctx->qry[query_id].name), "_gg_qryname_%ld", gen_ctx->total_queries);
}

//
// Examine proposed query statement, perform checks to its validity, and generate initial defining code for it.
// gen_ctx is execution context, vp has all clauses parsed out
//
void prepare_query (gg_gen_ctx *gen_ctx, gg_db_parse *vp)
{

    // new_query either adds a query or uses existing one
    new_query (gen_ctx, vp);

    // get query output columns (or unknown)
    get_query_output (gen_ctx, vp); 

    //
    // Generate query variables etc
    //
    // generate variables used by query
    oprintf("gg_num _gg_qry_on_error_%s = %d;\n", gen_ctx->qry[query_id].name, vp->on_error_action);
    oprintf("GG_UNUSED (_gg_qry_on_error_%s);\n", gen_ctx->qry[query_id].name);
    oprintf("gg_num _gg_qry_executed_%s = 0;\n", gen_ctx->qry[query_id].name);
    oprintf("GG_UNUSED (_gg_qry_executed_%s);\n", gen_ctx->qry[query_id].name);
    oprintf("char *%s = NULL;\n", gen_ctx->qry[query_id].name); 
    //
    // Generate the C code to allocate a query. gen_ctx is the context, and query_id is the query id.
    // Depending on what kind of code we generate later, some of these may not be used, and we mark them
    // as unused to avoid compiler warning.
    //
    // affected rows, and then number of rows and columns retrieved
    // set to zero in case run-query doesn't run so that they don't use random values
    oprintf("gg_num _gg_arow_%s = 0;\n", gen_ctx->qry[query_id].name);
    oprintf("gg_num _gg_nrow_%s = 0;\n", gen_ctx->qry[query_id].name);
    oprintf("gg_num _gg_ncol_%s = 0;\n", gen_ctx->qry[query_id].name);
    oprintf("GG_UNUSED(_gg_arow_%s);\n", gen_ctx->qry[query_id].name);
    oprintf("GG_UNUSED(_gg_nrow_%s);\n", gen_ctx->qry[query_id].name);
    oprintf("GG_UNUSED(_gg_ncol_%s);\n", gen_ctx->qry[query_id].name);
    // when we do 'run-query' with empty text followed by start-query, we don't
    // know if query is DML or not, so we have to specify all variables
    oprintf("char * _gg_errm_%s = GG_EMPTY_STRING;\n", gen_ctx->qry[query_id].name);
    oprintf("GG_UNUSED (_gg_errm_%s);\n", gen_ctx->qry[query_id].name);
    oprintf("char * _gg_err_%s = GG_EMPTY_STRING;\n", gen_ctx->qry[query_id].name);
    oprintf("GG_UNUSED (_gg_err_%s);\n", gen_ctx->qry[query_id].name);
    oprintf("char **_gg_data_%s = NULL;\n", gen_ctx->qry[query_id].name);
    oprintf("GG_UNUSED (_gg_data_%s);\n", gen_ctx->qry[query_id].name);
    oprintf("char **_gg_col_names_%s = NULL;\n", gen_ctx->qry[query_id].name);
    oprintf("GG_UNUSED (_gg_col_names_%s);\n", gen_ctx->qry[query_id].name);
    // allocate SQL buffer
    oprintf("static char *_gg_sql_buf_%s;\n", gen_ctx->qry[query_id].name);
    oprintf("char **_gg_sql_params_%s = NULL;\n", gen_ctx->qry[query_id].name);
    oprintf("gg_num _gg_sql_paramcount_%s = 0;\n", gen_ctx->qry[query_id].name);
    // _gg_is_input_used specifies if there's an input parameter. 
    // There may be some zeroes in it, but normally it is a block of 1's corresponding to input parameters
    // see comments on gg_make_SQL() function for more on this.
    oprintf ("gg_num _gg_is_input_used_%s[%d];\n", gen_ctx->qry[query_id].name, GG_MAX_QUERY_INPUTS + 1);
    oprintf ("memset (_gg_is_input_used_%s, 0, sizeof (gg_num)*%d);\n", gen_ctx->qry[query_id].name, GG_MAX_QUERY_INPUTS + 1);
    //_dlen is length of data fields
    oprintf ("gg_num *_gg_dlen_%s = NULL;\n", gen_ctx->qry[query_id].name);
    oprintf("GG_UNUSED (_gg_dlen_%s);\n", gen_ctx->qry[query_id].name);
    // _iter is the iterator through output data loop (incrementing by 1 for each row)
    oprintf("gg_num _gg_iter_%s;\n", gen_ctx->qry[query_id].name);
    oprintf("GG_UNUSED (_gg_iter_%s);\n", gen_ctx->qry[query_id].name);
    // set if prepared statement, if so, make the unique variable used for it
    // unique void * for prepared stmt, important to be NULL. It is only non-NULL when it has been prepared. 
    // It cannot be deallocated (when connection goes down) if we cannot distinguish whether it has been prepared or not
    oprintf("static void *_gg_sql_prep_%s = NULL;\nGG_UNUSED(_gg_sql_prep_%s);\n", gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].name);
}


//
// Free resources used by query
// gen_ctx is execution context, qryname is the name of the query
// skip_data is true if not to free data (but rather free everything else)
//
void free_query (char *qryname, bool skip_data)
{
    char qname[GG_MAX_QUERYNAME_LEN+1];
    snprintf (qname, sizeof (qname), "_gg_qryname_%s", qryname);
    // qname itself is just sql text
    // the error msg/cde are either GG_EMPTY_STRING or allocated, so they can be freed
    oprintf("gg_free(_gg_errm_%s);\n", qname);
    oprintf("gg_free(_gg_err_%s);\n", qname);
    // free data if there, also free each individual chunk
    oprintf("if (_gg_data_%s != NULL) { if (!%s) {gg_num i; for (i=0;i<_gg_ncol_%s*_gg_nrow_%s;i++) gg_free(_gg_data_%s[i]);} gg_free(_gg_data_%s);}\n", qname, skip_data?"true":"false", qname, qname, qname, qname);
    // free names if there, also free each individual chunk
    oprintf("if (_gg_col_names_%s != NULL) {gg_num i; for (i=0;i<_gg_ncol_%s;i++) gg_free(_gg_col_names_%s[i]); gg_free (_gg_col_names_%s);}\n", qname, qname, qname, qname);
    // free length of data if there
    oprintf ("if (_gg_dlen_%s != NULL) gg_free (_gg_dlen_%s);\n", qname, qname);
    // _gg_sql_buf_... is freed if it was allocated, it is a dynamic buffer for SQL text to be executed, search on it
    // _gg_sql_params_... is freed if it was allocated, it's dynamic params when statement is prepared
    // _gg_sql_prep is allocated once with strdup() and never freed unless re-prepared, just a unique pointer, search on it
}


//
// Do most of query code generation and processing of query result. Do all of query stack processing.
// query_result is 1 if this is query-result. run_query is 1 if this is run-query 
//
void query_execution (gg_gen_ctx *gen_ctx, const gg_num run_query, const gg_num query_result, char is_prep, gg_db_parse *vp)
{

    if (run_query == 1)
    {
        //
        // Check if there is : after which there must be input params
        //
        if (vp->colon != NULL) get_all_input_param (gen_ctx, vp->colon);


        // ready to generate query execution code in C
        generate_sql_code (gen_ctx, is_prep);

        //
        // get any information about query user requested
        // This must be done before opening the loop below (start_loop_*) b/c otherwise 
        // these would execute many times in a loop
        //
        if (vp->err != NULL)
        {
            oprintf("%s = _gg_err_%s;\n", vp->err, gen_ctx->qry[query_id].name);
        } 
        if (vp->errtext != NULL)
        {
            oprintf("%s = _gg_errm_%s;\n", vp->errtext, gen_ctx->qry[query_id].name);
        }
        if (vp->arows != NULL)
        {
            oprintf("%s = _gg_arow_%s;\n", vp->arows, gen_ctx->qry[query_id].name);
        }
        if (vp->rcount != NULL)
        {
            GG_CHECK_TUPLE(query_id);
            oprintf("%s = _gg_nrow_%s;\n", vp->rcount, gen_ctx->qry[query_id].name);
        }

        // start the loop (that ends with end-query) if requested
        if (vp->noloop == NULL) start_loop_query (gen_ctx); else end_query (gen_ctx, 0);

    }

    else if (query_result == 1)
    {
        // 
        // this is query-result statement
        //
        generate_query_result (gen_ctx, vp);
        return;
    }
    else
    {
       // should never happen
       gg_report_error ("Syntax error in query");
    }
}

//
//  Processing of query-result.  vp->query_result is the name of column being asked to provide value for.
//
void generate_query_result (gg_gen_ctx *gen_ctx, gg_db_parse *vp)
{
    char *col_out = vp->query_result;

    // get column encoding when retrieving query column
    gg_num no_encode = 0;
    gg_num url_encode = 0;
    gg_num web_encode = 0;
    char *noenc = find_keyword (col_out, GG_KEYNOENCODE, 1);
    if (noenc != NULL) no_encode = 1;
    char *webenc = find_keyword (col_out, GG_KEYWEBENCODE, 1);
    if (webenc != NULL) web_encode = 1;
    char *urlenc = find_keyword (col_out, GG_KEYURLENCODE, 1);
    if (urlenc != NULL) url_encode = 1;
        
    carve_statement (&noenc, "query-result", GG_KEYNOENCODE, 0, 0);
    carve_statement (&webenc, "query-result", GG_KEYWEBENCODE, 0, 0);
    carve_statement (&urlenc, "query-result", GG_KEYURLENCODE, 0, 0);

    gg_num len_name = carve_stmt_obj (&col_out, true, false);
    if (len_name > GG_MAX_COLNAME_LEN) gg_report_error ("Query name in [%s] is too long", col_out);

    define_statement (&col_out, GG_DEFSTRING, false); 
    //
    // no check_var needed above as col_out (name of column) is defined, and the other have no clause values

    // we can change the mtext all we want, but we can't memmove, which is fine, that's why we use col_out here
    if (urlenc != NULL) *urlenc = 0;
    if (webenc != NULL) *webenc = 0;
    if (noenc != NULL) *noenc = 0;

    if (no_encode+web_encode+url_encode > 1)
    {
        gg_report_error ("Query output can be either noencode, webencode (default) or urlencode, but not any combination of these");
    }
    if (no_encode+web_encode+url_encode == 0) web_encode = 1; // default is web-encode


    //
    // to 'var' is always used, run-query ALWAYS stores data into defined variables
    //

    // Check if query ID is active. If it is used, but not active, it means
    // we're using a query column outside the action block for that query
    if (gen_ctx->qry_active[query_id] != GG_QRY_ACTIVE)
    {
        gg_report_error( "Query [%s] is not active", gen_ctx->qry[query_id].name);
    }

    //
    //
    // column_id is the id of the query column
    //
    //
    gg_num column_id =  vp->cur_query_result;

    // we now have numerical positional ID for a column in the result set
    // which is column_id

    // we cannot have error, affected_rows here, because this is result set and it may be 0 rows,
    // so without rows, query-result is never reached, making it impossible to get the aforementioned fields.

    // if we added too many output columns, but query has not enough of them, the data will be undefined; prevent that
    oprintf ("if (_gg_ncol_%s <= %ld) gg_report_error (\"Column #%ld does not exist in the database result set\");\n", gen_ctx->qry[query_id].name,column_id, column_id);

    
    if (no_encode == 1) 
    {
        oprintf("%s = (_gg_data_%s[_gg_iter_%s*_gg_ncol_%s+%ld]);\n", col_out, gen_ctx->qry[query_id].name,gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].name, column_id);
    }
    else 
    {
        oprintf ("gg_encode (%s, _gg_data_%s[_gg_iter_%s*_gg_ncol_%s+%ld], -1, &(%s), true);\n", web_encode==1?"GG_WEB":"GG_URL", gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].name, column_id , col_out);
    }


    //
    //
    // column_id is not used beyond this point. Very careful with it, because it can be negative!!!
    //
    //
}

//
// Generate SQL for SELECT and DMLs for start-query for query index query_id
//
void generate_sql_code (gg_gen_ctx *gen_ctx, char is_prep)
{

    oprintf("%s = %s;\n", gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].text);

    oprintf("char *fname_loc_%s = \"%s\";\n",gen_ctx->qry[query_id].name, src_file_name); // to overcome constness
    oprintf("gg_num lnum_%s = %ld;\n",gen_ctx->qry[query_id].name,lnum); 
    oprintf("gg_location (&fname_loc_%s, &lnum_%s, 1);\n",gen_ctx->qry[query_id].name,gen_ctx->qry[query_id].name);


    statement_SQL (gen_ctx, is_prep);

    if (gen_ctx->qry[query_id].returns_tuple == 1)
    {
        // generate select call for any SQL that returns tuple
        oprintf("gg_select_table (_gg_sql_buf_%s, &_gg_arow_%s, &_gg_nrow_%s, &_gg_ncol_%s, %s%s, &_gg_data_%s, &_gg_dlen_%s, (char**)&_gg_err_%s, (char**)&_gg_errm_%s, %d, &_gg_sql_prep_%s, _gg_sql_paramcount_%s, _gg_sql_params_%s, _gg_qry_on_error_%s);\n",
        gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].name,gen_ctx->qry[query_id].name, 
        // get query columns names from db only if unknown output is used, otherwise not
        gen_ctx->qry[query_id].unknown_output == 1 ? "&_gg_col_names_": "NULL", 
        gen_ctx->qry[query_id].unknown_output == 1 ? gen_ctx->qry[query_id].name:"",
        gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].name, is_prep, gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].name);
        oprintf("gg_db_free_result (%d);\n", (int)is_prep);


    }
    else
    {
        // this is no-tuple call

        oprintf("gg_execute_SQL (_gg_sql_buf_%s, &_gg_arow_%s, (char**)&_gg_err_%s, (char**)&_gg_errm_%s, %ld, 1, %d, &_gg_sql_prep_%s, _gg_sql_paramcount_%s, _gg_sql_params_%s, _gg_qry_on_error_%s);\n",
            gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].returns_tuple, (int)is_prep, gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].name);
        // set nrow to 0
        oprintf("_gg_nrow_%s=0;\n", gen_ctx->qry[query_id].name);
        oprintf("gg_db_free_result (%d);\n", (int)is_prep);
    }

    // deallocate _gg_sql_buf_<query name> if not prepared and no input params (which is the condition for it to be allocated)
    if (gen_ctx->qry[query_id].qry_found_total_inputs > 0 && is_prep == 0) oprintf("gg_free_int (_gg_sql_buf_%s);\n", gen_ctx->qry[query_id].name);
    // deallocate SQL params used for prepared query
    oprintf("if (_gg_sql_params_%s != NULL) gg_free_int (_gg_sql_params_%s);\n", gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].name);
}

//
// open the loop query in generated code. query_id is the index number of query.
//
void start_loop_query (gg_gen_ctx *gen_ctx)
{

    oprintf("for (_gg_iter_%s = 0; _gg_iter_%s < _gg_nrow_%s; _gg_iter_%s++)\n",gen_ctx->qry[query_id].name, 
        gen_ctx->qry[query_id].name,gen_ctx->qry[query_id].name,gen_ctx->qry[query_id].name);
    oprintf("{\n");
    
    // if outputs defined with "col1, col2..." then define them and fill values
    gg_num i;
    for (i = 0; i < gen_ctx->qry[query_id].qry_total_outputs; i++) query_result (gen_ctx, gen_ctx->qry[query_id].qry_outputs[i], i);


    // done with loop query
}



//
// Statement SQL to be executed for run-query etc. This is after the analysis of the statement,
// so we have filled out structures based on query_id. The statement SQL is output to generated source file
//
void statement_SQL (gg_gen_ctx *gen_ctx, char is_prep)
{
    if (is_prep == 1)
    {
        // 
        // statement is evaluated only the first time for prepared queries, so we save a copy
        // of the query in case we need to reconnect. TODO: if query is a string literal, we can
        // avoid strdup below.
        //
        oprintf("static char _gg_sql_buf_set_%s=0;\n", gen_ctx->qry[query_id].name);
        //
        // _gg_sql_buf_<query name> is a copy of query text. It should stay and not be freed for the life of the process,
        // because if the db server goes down, it will be needed to prepare statement again.
        // So no gg_ alloc functions, as they are deallocated at the end of the request
        // Also set void** as prepared statement pointer, so it can be cleared when connection is lost (gg_db_prep() call)
        oprintf("if (_gg_sql_buf_set_%s==0) {if ((_gg_sql_buf_%s=strdup(%s))==NULL) gg_report_error(\"Cannot allocate query [%s] for a prepared statement\");_gg_sql_buf_set_%s=1; gg_db_prep(&_gg_sql_prep_%s); }\n", gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].name);
        oprintf("if (_gg_qry_executed_%s == 1) {gg_report_error(\"Query [%s] has executed the second time; if you want to execute the same query twice in a row without a run-query, use different queries with the same query text (or variable) if that is your intention, file [%s], line number [%ld] \");}\n", gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].name, src_file_name, lnum);
    }

    // with dynamic queries, we cannot count how many '%s' in SQL text (i.e. inputs) there are. 
    // For dynamic, the number of inputs is known  only by
    // the number of actual input parameters in start-query (this is qry_found_total_inputs). Because
    // SQL statement (_gg_sql_buf...) is only known at run-time, we need to pass this number of input params to it
    // and verify we are not using bad memory or missing arguments.
    if (gen_ctx->qry[query_id].qry_found_total_inputs == 0)
    {
        //
        // If there are no parameters, this would look like:
        // gg_make_SQL (_gg_sql_buf_something, 32000, 0, something);
        // and that would cause gcc error 'format not a string literal and no format arguments [-Werror=format_security]
        // because gcc doesn't know that generated code will NOT make a mistake of including %s in 'something'
        // So make it a simple copy, as it's all that's needed.
        // is_prep can be 0 or 1 here, if 1, this is prepared statement without any input params
        //
        // for non-prepared statements without any input params, just use the query text
        if (is_prep == 0) oprintf("_gg_sql_buf_%s=%s;\n", gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].name);
    }
    else
    {
        if (is_prep == 0)
        {
            // _gg_sql_buf_%s may be NULL here, in general it is undefined. And it will be allocated in gg_make_SQL at run time
            // and it will be deallocated once the query executes.
            oprintf("gg_make_SQL (&_gg_sql_buf_%s, %ld, %s ",
                gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].qry_found_total_inputs,  gen_ctx->qry[query_id].name); 
            gg_num z;
            for (z = 0; z < gen_ctx->qry[query_id].qry_found_total_inputs; z++)
            {
                oprintf(", _gg_is_input_used_%s[%ld]==1 ?  (%s) : NULL ", gen_ctx->qry[query_id].name, z, gen_ctx->qry[query_id].qry_inputs[z]);
            }
            oprintf(");\n");
        }
        else
        {
            // 
            // set params count and params themselves for prepared statement
            //
            oprintf("_gg_sql_paramcount_%s = %ld;\n", gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].qry_found_total_inputs);
            oprintf("_gg_sql_params_%s = (char**)gg_calloc (%ld, sizeof(char*));\n", gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].qry_found_total_inputs);
            gg_num z;
            for (z = 0; z < gen_ctx->qry[query_id].qry_found_total_inputs; z++)
            {
                oprintf("_gg_sql_params_%s[%ld]=(_gg_is_input_used_%s[%ld]==1 ?  (%s) : NULL);\n", gen_ctx->qry[query_id].name, z, gen_ctx->qry[query_id].name, z, gen_ctx->qry[query_id].qry_inputs[z]);
            }
        }
    }

    // We execute the actual db query right at the beginning of the action block
    // We use 'query ID' decorated variables, so all our results are separate
    // file/line must be here because this is generated code and file name/line number are NOT available then unless 
    // we plug it in now
    oprintf("if (_gg_qry_executed_%s == 1) {gg_report_error(\"Query [%s] has executed the second time; if you want to execute the same query twice in a row without a run-query, use different queries with the same query text (or variable) if that is your intention, file [%s], line number [%ld] \");}\n", gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].name, src_file_name, lnum);
    oprintf("_gg_qry_executed_%s = 1;\n", gen_ctx->qry[query_id].name);
}


// 
// Process text of dynamic query. gen_ctx is execution context, vp is parsed statement clauses.
// Get output query, the columns. 
//
void get_query_output (gg_gen_ctx *gen_ctx,  gg_db_parse *vp)
{
    char *with_output = vp->with_output;

    if (with_output != NULL)
    {
        while (isspace(*with_output)) with_output++;
        // get all the column names
        while (1)
        {
            gg_num end_of_list = 0;
            char *collist = with_output;
            // now finding column list after output
            char *colend = collist;
            get_until_comma (&colend);
            if (*colend == 0) end_of_list = 1; // no more in comma-separated list, last item
            *colend = 0;
            gg_num colLen = strlen (collist);
            gg_trim (collist, &colLen, false);
            // these column names are checked to be valid names in start_loop_query
            gen_ctx->qry[query_id].qry_outputs[gen_ctx->qry[query_id].qry_total_outputs] = gg_strdup (collist);
            gen_ctx->qry[query_id].qry_total_outputs++;
            if (gen_ctx->qry[query_id].qry_total_outputs >= GG_MAX_QUERY_OUTPUTS)
            {
                gg_report_error( "Too many query outputs [%ld]", gen_ctx->qry[query_id].qry_total_outputs);
            }
            with_output = colend + 1;
            if (end_of_list == 1) break;
        }
    }
    else
    {
        // no output, this is not a SELECT, such a dynamic query is DML
        gen_ctx->qry[query_id].returns_tuple = 0;
    }
}


//
// Setup database config from db name, which is dbname
//
void get_db_config (char *dbname)
{
    // if db name not specified, that's valid if there is only one db in use
    if (dbname == NULL)
    {
        if (totconn == 1) dbname = gg_get_config()->ctx.db->conn[0].db_name;
        else if (totconn == 0) {gg_report_error ("No database is configured. Configure a database before attempting to use it");}
        else {gg_report_error ("There is more than one database configured. Use '@' to specify the database used here");}
    }

    gg_num l = strlen (dbname);
    gg_trim (dbname, &l, false);

    // dbname is now pure db conn name
    if (gg_is_valid_param_name(dbname, false, false) != 1)
    {
        gg_report_error( "Database name must have only alphanumeric characters and underscore");
    }

    // generate database switch to this database
    // no need for runtime to have dbtype info, each library already knows what it is
    oprintf ("gg_get_config()->ctx.db->ind_current_db=%ld;\n", find_connection (dbname));

}

//
// Build a list of connections, and return a fixed index that identifies the
// database connection we're adding. 'conn' is the connection name. It is assume no duplicates arrive here - this is
// fed from .dbvendors file generated by golf utility.
// dbtype is db vendor (such as GG_DB_MARIADB)
// There is no limit on number of db connections
//
gg_num add_connection(char *conn, gg_num dbtype)
{
    gg_num i;
    static gg_num conn_num = 0;
    if (gg_get_config()->ctx.db->conn == NULL)
    {
        conn_num = 1; // start with 1 db connection since that's what most programs will do.
        // calloc initializes to zero here - we rely on that to make sure members like g_con are NULL, etc.
        gg_get_config()->ctx.db->conn = gg_calloc (conn_num, sizeof(one_db));
    }
    for (i = 0; i < totconn; i++)
    {
        if (!strcmp (conn, gg_get_config()->ctx.db->conn[i].db_name))
        {
            gg_report_error ("Database connection [%s] duplicated in the list of connections", conn);
            return i;
        }
    }
    totconn++;
    if (totconn > conn_num)
    {
        gg_num old_conn_num = conn_num;
        conn_num+=3; // add three connections at a time
        gg_get_config()->ctx.db->conn = gg_realloc (gg_mem_get_id(gg_get_config()->ctx.db->conn), conn_num*sizeof(one_db));
        // set to zero so g_con is NULL etc.
        gg_num j;
        for (j = old_conn_num; j < conn_num; j++) 
        {
            memset (&(gg_get_config()->ctx.db->conn[j]), 0, sizeof(one_db));
        }
    }
    gg_get_config()->ctx.db->conn[totconn-1].db_name=gg_strdup (conn);
    gg_get_config()->ctx.db->conn[totconn-1].db_type=dbtype; // the default (empty) is mariadb
    return totconn - 1;
}

//
// Build a list of connections, and return a fixed number between 0 and GG_MAX_DB_CONNECTIONS that identifies the
// database connection we're dealing with. 'conn' is the connection name.
//
gg_num find_connection(char *conn)
{
    gg_num i;
    // make sure no spaces around the db name, there usually are from parsing
    char *db = gg_strdup (conn);
    gg_num dlen = strlen (db);
    gg_trim (db, &dlen, false);

    for (i = 0; i < totconn; i++)
    {
        if (!strcmp (db, gg_get_config()->ctx.db->conn[i].db_name))
        {
            return i;
        }
    }
    gg_report_error ("Database connection [%s] not found. There must be file [%s] in current directory describing the database, and you must use --db option to specify what kind of database it is", db, db);
    return -1;
}

//
// See gg_find_keyword0
//
char *find_keyword(char *str, char *find, gg_num has_spaces)
{
    return  gg_find_keyword0 (str, find, has_spaces, 1);
}


//
// Get value of option in statement. For example, if statement is 
// send-mail from "x@y.com" to "z@w.com" ...
// then you can get "x@y.com" by doing this:
// 
// char *from = find_keyword (...);
// carve_statement (&from, "send-mail", GG_KEYFROM, 1, 1);
//
// where variable 'from' will be "x@y.com", '1' as 'is_mandatory' parameter means this parameter MUST be present,
// indicate where we're at in the parsing process (file name and line number).
// 'send-mail' is the name of top statement, and GG_KEYFROM is "from", and we're parsing out the data after it.
// NOTE that 'from' MUST point to actual "x@y.com" within original send-mail string. This means ALL options must be first
// found with find_keyword() before calling this function for any of them.
// This function MUST be called for ALL statement options - if not then some statement values will contain other statement and will be 
// INCORRECT. 
// 'has_data' is 0 if the option is alone without data, for example 'no-cert'. Typically it's 1. It can also be 2, which is
// when there may be data after it, but it may also be missing.
// *statement is the value of what's carved out.
//
void carve_statement (char **statement, char *statement_name, char *keyword, gg_num is_mandatory, gg_num has_data)
{
    //
    // *statement is the result of strstr(mtext, keyword) - and these MUST
    // be done for ALL options prior to calling this function
    //
    if (*statement != NULL) 
    {
        int ksize = strlen(keyword);
        // keyword that is not a 'human word' may have no space at the end, such as =, ==, != (hence isalpha() check)
        // also keyword may be " ", which is the very first space after the statement name, like in get-cookie
        // but keyword[ksize-1] being ' ' is when there's supposed to be data afterwards
        if (has_data > 0 && keyword[ksize-1]!=' ' && isalpha(keyword[0]))
        {
            // any keyword that has data must have trailing space
            // but if has_data is 2, data may be missing after keyword (see above)
            if (has_data != 2) gg_report_error( "Internal: keyword [%s] is missing trailing space", keyword);
        }
        char *end_of_url = *statement;
        // advance past the keyword
        *(*statement = (*statement + ksize-1)) = 0;
        (*statement)++;
        if (has_data == 2)
        {
            // advance passed any white spaces, this is important when has_data is 2, so we know if there is something there or not
            // if what follows is a keyword, it will be cut off by the processing of that keyword and still be empty
            // if this were to be for all (i.e. not just has_data == 2), then pf-out wouldn't work because this would lead up
            // to it's format string and below it would set the first byte of it to 0, meaning the whole format and any data to print
            // would just vanish.
            while (isspace(**statement)) (*statement)++;
        }
        *end_of_url = 0; // cut off keyword, so previous clause ends at the beginning of the next one

        if (has_data==0)
        {
            emptyc[emptyc_c++] = *statement;
            //(*statement)[0] = 0; // no data for this option, we only care if present or not present.
        }
    }
    else if (is_mandatory==1)
    {
        gg_report_error( "%s keyword is missing in %s", keyword, statement_name);
    }
}

//
// Call this if clause (which is current *statement) might need to have the variable defined. This is the case for output variables
// i.e. resulting variables.
// This will generate code to create such variables, if needed.
// 'statement' is the pointer to the pointer to data associated with current clause we're processing, for example in 'get-time to res year 1'
// 'statement' could point to 'res' and create it as a string if it doesn't exist. 
// If the variable exists, then no new one is created. This forces creation of new variable names if needed, which is a good thing because
// creating new variable with the same name in an inner scope often leads to problems.
// 'type is GG_DEFSTRING('char *') or GG_DEFNUMBER or... (see below) - this is what variable should be (if it exists) or to be created
// If 'always' is true, then create new variable regardless, thus error out if same variable is used in the same scope - this is for
// indexes, lists, arrays etc.
// Returns 0 if variable existed and was not created, 1 if it didn't exist and was created.
//
// if the '*statement' has () around it, those are removed (see comments below) - this is to allow keywords to be used as variable names
// check that variable doesn't start with gg, golf, _gg or _golf
//
gg_num define_statement (char **statement, gg_num type, bool always)
{
    if (*statement == NULL) return 0;

    // if the result has () around it, remove it. There's no need for it, other than to use the same name
    // for a variable or function as is a keyword. If really needed, use (()), so the inner ones remain
    gg_num ls = strlen (*statement);
    // make sure *statement is not empty - this happens when data to a clause is not supplied
    *statement = gg_trim_ptr(*statement,  &ls);

    if ((*statement)[0] == 0) gg_report_error("Parameter missing in the statement");

    if (ls >= 3 && (*statement)[0] == '(' && (*statement)[ls-1] == ')')
    {
        // remove leading ( and trailing )
        (*statement)[ls - 1] = 0;
        (*statement)++;
        ls -= 2; // update length
    }


    // output is always a variable
    if (gg_is_valid_param_name(*statement, false, false) != 1) gg_report_error(GG_NAME_INVALID, *statement);

    if (!strncmp (*statement, "gg_", sizeof("gg_")-1) ||
        !strncmp (*statement, "gg_", sizeof("gg_")-1) ||
        !strncmp (*statement, "_gg_", sizeof("_gg_")-1) ||
        !strncmp (*statement, "_gg_", sizeof("_gg_")-1)) gg_report_error ("Variable name starting with _gg_, _gg_, gg_ or gg_ are reserved and cannot be used");

    bool add_var_res;
    //
    // For static types within do-once, lower their gg_level, but only if current level is do_once_level+1 - see "do-once" handler elsewhere here in this file
    // Note that do-once CANNOT be nested, so it makes this simpler here.
    //
    if (do_once_open && gg_level == do_once_level+1 && (type == GG_DEFSTRINGSTATIC || type == GG_DEFNUMBERSTATIC || type == GG_DEFHASHSTATIC || type == GG_DEFARRAYSTRINGSTATIC || type == GG_DEFARRAYNUMBERSTATIC || type == GG_DEFARRAYBOOLSTATIC || type == GG_DEFTREESTATIC || type == GG_DEFLISTSTATIC || type == GG_DEFBOOLSTATIC))
    {
        // make this variable in the same scope as do-once. All other vars are in +1 level. 
        // Note that generated C code is all the same level as do-once. We simulate higher level. The other variables
        // within do-once (other than process-scoped) could theoretically be accessed outside do-once block, but Golf will
        // consider them non-existant, so they cannot be used - this prevents gcc from generating uninitialized warnings/errors.
        gg_level--; // this is the level C compiler see
        add_var_res = add_var (*statement, type, NULL, true);
        gg_level++;
    }
    else
    {
        // when do-once is in effect, gg_level is +1 (even though C compiler's level is one less), which prevents user
        // from using variables made in do-once outside it (technically in C they could, but in Golf they can't, see above comment)
        add_var_res = add_var (*statement, type, NULL, true);
    }

    // Create var name  and store it to hash
    // Do not create if already exisiting
    if (!add_var_res)
    {
        // string is initialized to NULL (and not to GG_EMPTY_STRING), since NULL is treated the same in gg_realloc/free, and 
        // because when static, gcc won't initialize to variable, even if it's a constant assigned (per C)
        if (type == GG_DEFSTRING) oprintf ("char *%s = GG_EMPTY_STRING;\n", *statement); // GG_EMPTY_STRING cannot  be NULL in user execution
        else if (type == GG_DEFSTRINGSTATIC) oprintf ("static char *%s;\n", *statement); // cannot use GG_EMPTY_STRING b/c it's a variable
        else if (type == GG_DEFNUMBER) oprintf ("gg_num %s = 0;\n", *statement);
        else if (type == GG_DEFNUMBERSTATIC) oprintf ("static gg_num %s = 0;\n", *statement);
        else if (type == GG_DEFMSG) oprintf ("gg_msg *%s = NULL;\n",*statement);
        else if (type == GG_DEFBROKEN) oprintf ("gg_split_str *%s = NULL;\n",*statement);
        else if (type == GG_DEFHASH) oprintf ("gg_hash *%s = NULL;\n", *statement);
        else if (type == GG_DEFHASHSTATIC) oprintf ("static gg_hash *%s = NULL;\n", *statement);
        else if (type == GG_DEFARRAYSTRING) oprintf ("gg_arraystring *%s = NULL;\n", *statement);
        else if (type == GG_DEFARRAYSTRINGSTATIC) oprintf ("static gg_arraystring *%s = NULL;\n", *statement);
        else if (type == GG_DEFARRAYNUMBER) oprintf ("gg_arraynumber *%s = NULL;\n", *statement);
        else if (type == GG_DEFARRAYNUMBERSTATIC) oprintf ("static gg_arraynumber *%s = NULL;\n", *statement);
        else if (type == GG_DEFARRAYBOOL) oprintf ("gg_arraybool *%s = NULL;\n", *statement);
        else if (type == GG_DEFARRAYBOOLSTATIC) oprintf ("static gg_arraybool *%s = NULL;\n", *statement);
        else if (type == GG_DEFJSON) oprintf ("gg_json *%s = NULL;\n", *statement);
        else if (type == GG_DEFXML) oprintf ("gg_xml *%s = NULL;\n", *statement);
        else if (type == GG_DEFTREE) oprintf ("gg_tree *%s = NULL;\n", *statement);
        else if (type == GG_DEFTREESTATIC) oprintf ("static gg_tree *%s = NULL;\n", *statement);
        else if (type == GG_DEFTREECURSOR) oprintf ("gg_tree_cursor *%s = NULL;\n", *statement);
        else if (type == GG_DEFFIFO) oprintf ("gg_fifo *%s = NULL;\n", *statement);
        else if (type == GG_DEFLIFO) oprintf ("gg_fifo *%s = NULL;\n", *statement);
        else if (type == GG_DEFLIST) oprintf ("gg_list *%s = NULL;\n", *statement);
        else if (type == GG_DEFLISTSTATIC) oprintf ("static gg_list *%s = NULL;\n", *statement);
        else if (type == GG_DEFENCRYPT) oprintf ("EVP_CIPHER_CTX *%s = NULL;\n", *statement);
        else if (type == GG_DEFFILE) oprintf ("gg_file *%s = NULL;\n", *statement);
        else if (type == GG_DEFSERVICE) oprintf ("gg_cli *%s = NULL;\n", *statement);
        else if (type == GG_DEFBOOL) oprintf ("bool %s = false;\n", *statement);
        else if (type == GG_DEFBOOLSTATIC) oprintf ("static bool %s = false;\n", *statement);
        else gg_report_error( "Unknown type (%ld)", type);
        return 1;
    } else 
    {
        if (always) gg_report_error ("Variable [%s] cannot be created again", *statement);
        return 0;
    }
} 


// 
// Display verbosely steps taken during the preprocessing
// gg_line is the line number is source, plus the printf-like output
//
void out_verbose(gg_num gg_line, char *format, ...)
{
    if (verbose==0) return;
    // THIS FUNCTON MUST NEVER USE ANY FORM OF MALLOC OR GG_MALLOC
    // or it may fail when memory is out or not available (such as in gg_malloc)

    char trc[GG_TRACE_LEN + 1];

    va_list args;
    va_start (args, format);
    vsnprintf (trc, sizeof(trc) - 1, format, args);
    va_end (args);

    fprintf (stdout, "Line %ld: %s\n", 
        gg_line, trc);
    fflush (stdout);
}


// 
// Find terminal width for help display.
// Returns width in characters.
//
gg_num terminal_width()
{
    static gg_num twidth = 0;
    if (twidth == 0)
    {
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

        twidth = (gg_num)(w.ws_col);
    }
    return twidth;
}

//
// Parse a list of parameters. Parameters are a list of strings or non-string parameters separated
// by commas. Parameter can have a comma if it is a string, or within ()
//
// String representing list of parameters is parse_list. GOLF list structure is 'params', which will
// hold the parsed parameters. 
// parse_list is advanced to its end during processing.
// tot is the number of elements (single or x=y pairs encountered); it can be NULL.
//
// This is for a LIST of parameters separated by comma ONLY (i.e. start-query# and exec-program).
// Each param is trimmed.
// On return, parse_list is one byte passed the end of last character in the list, which is null character.
//
void parse_param_list (char *parse_list, gg_fifo **params, gg_num *tot)
{
    gg_store_init(params);
    if (tot != NULL) *tot = 0;

    while (1)
    {
        // input param can be anything separate by comma, which must be outside quotes
        // or parenthesis, i.e. free-comma.
        // so find free comma, or if none found returns pointer
        // to the ending null char of *parse_list string.
        char *milestone = find_keyword(parse_list, ",", 0);
        char *end_of_parse_list; // this is where parse_list should be after looking for parameter
        char _gg_inp_par[GG_MAX_QUERY_INPUT_LEN + 1];
        char *inp_par=_gg_inp_par; // because we will ++ this pointer, can't work otherwise

        if (milestone == NULL)
        {
            milestone = parse_list + strlen (parse_list); // points to 0 of its end, so grasping param works below w/par_len
            // we reached the end of string, this is the last parameter
            end_of_parse_list = milestone;
        }
        else
        {
            end_of_parse_list = milestone+1;// get one beyond comma
            // skip white space after comma
            while (isspace(*end_of_parse_list)) end_of_parse_list++;
        }

        // milestone now points to either the next parameter or is NULL (meaning end of param list)

        // length of input parameter
        gg_num par_len = milestone - (parse_list);

        if (par_len > GG_MAX_QUERY_INPUT_LEN - 1)
        {
            gg_report_error( "Parameter too long [%ld], parameter [%.100s]", par_len, parse_list);
        }

        memcpy (inp_par, parse_list, par_len);
        inp_par[par_len] = 0;

        gg_trim (inp_par, &par_len, false);
        gg_store(*params, GG_EMPTY_STRING, (void*)gg_strdup(inp_par));

        parse_list = end_of_parse_list; // positioned to next parameter
                                         // this needs to be before break below
                                         // because caller relies on parse_list being exhausted
        if (tot != NULL) (*tot)++;

        // break if at the end
        if (*end_of_parse_list==0) break;

    } 
    gg_rewind(*params);
}

//
// Find next input parameter in SQL text. gen_ctx is the context of parsing, and 
// This parses part of statement such as start-query#xyz: a,b,c... i.e. it finds a, b, c etc. In this case query text uses '%s' 
// The parameter is a C expression or a quoted string (technically also a C expression, but treated differently).
// A parameter must be either a string, or not have comma, or be within ()
// So for example a parameter like func_call("x",2) would be (func_call("x",2))
//
// gen_ctx is the context. query_id is index into gen_ctx->qry[] array. iparams is the list of parameters to be parsed
//
void get_all_input_param (gg_gen_ctx *gen_ctx, char *iparams)
{

    // parse params. 
    gg_fifo *params = NULL;
    parse_param_list (iparams, &params, NULL);


    char *value;
    while (1)
    {
        gg_num st = gg_retrieve (params, NULL, (void**)&value);
        if (st != GG_OKAY) break;

        make_mem (&value);
        check_var (&value, GG_DEFSTRING);
        add_input_param (gen_ctx, value);
    } 
}

// 
// Add input parameter to a query.
// gen_ctx is the context. query_id is the index into gen_ctx->qry[] (i.e. queries). inp_par is the string
// representing input parameter 
//
void add_input_param (gg_gen_ctx *gen_ctx, char *inp_par)
{
    GG_TRACE("");

    gg_strncpy (gen_ctx->qry[query_id].qry_inputs[gen_ctx->qry[query_id].qry_found_total_inputs], inp_par, 
       GG_MAX_QUERY_INPUT_LEN - 1);

    oprintf("_gg_is_input_used_%s[%ld]=1;\n",  gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].qry_found_total_inputs);
    gen_ctx->qry[query_id].qry_found_total_inputs++;

}

//
// Process end-query statement. Update relevant data structures such as query stack.
// gen_ctx is preprocessor context, close_block is 1 if this is run-query (where we generate for loop)
// and 0 for run-query  with no-loop
// Output: query_id will be set to either -1 if there is no active query once we end this query, or to 
// that query's id (for example in case of nested queries).
// Error out if there is no active query at the point in source code.
//
void end_query (gg_gen_ctx *gen_ctx, gg_num close_block)
{
    // end of action block found , but there was not beginning
    if (query_id == -1)
    {
        gg_report_error( "query ending found, but no matching beginning");
    }
    // Ending of query with ID of leaving ID
    gg_num leaving_id = gen_ctx->curr_qry_ptr;
    // must be (leaving_id != -1);

    // no longer active, and its variables are defined, in case we
    // need to use them again - but they won't be defined again 
    gen_ctx->qry_active[leaving_id] = GG_QRY_INACTIVE;

    // reset number of inputs found in <<qry XX, in case we have another 
    // same query later on, for which we need to calculate this 'actual'
    // number of found input URL params
    // This can be with start-query, where multiple start-queries can have
    // different kinds of inputs
    gen_ctx->qry[leaving_id].qry_found_total_inputs = 0;

    // go down one level
    gen_ctx->curr_qry_ptr --;
    // must be (gen_ctx->curr_qry_ptr >= 0);

    if (close_block == 1)
    {
        oprintf("}\n"); // end of FOR loop we started with the QRY statement beginning
    }

    open_queries--;
}


// 
// Output generated C code. We remove empty lines (of which is generally plenty in order not to clutter
// HTML output, and to consistently output only non-empty lines. Works like printf.
// Error out if generated source code line too long.
// If format is NULL, flush the output.
//
void oprintf (char *format, ...)
{
    va_list args;
    va_start (args, format);

    static gg_num oline_prev_line = 0; // has previous line ended?

    if (format == NULL)
    {
        if (oline != NULL)
        {
            // remove empty printouts
            gg_num len = strlen (oline);
            gg_replace_string (oline, len+1, "gg_puts (GG_NOENC, \"\", GG_EMPTY_LONG_PLAIN_ZERO, false);\n", "", 1, NULL, 1); // remove idempotent printouts
                                                    // will always succeed because output is shorter 
            gg_replace_string (oline, len+1, "gg_puts (GG_NOENC, \"\", GG_EMPTY_LONG_PLAIN_ZERO, false);", "", 1, NULL, 1); // replace with or without new line

            //
            // make sure consecutive new lines are gone as they confuse gdb: when stepping through golf code, gdb sometimes goes to the next line, meaning
            // showing like it goes there, but it doesn't execute it. Bogus step. This is because of consecuitve new lines.
            gg_num lsub;
            lsub = gg_replace_string (oline, len+1, "\n\n", "\n", 1, NULL, 1); 
            // subst new lines until nothing done
            while (1)
            {
                gg_num nsub = gg_replace_string (oline, len+1, "\n\n", "\n", 1, NULL, 1); 
                if (lsub == nsub) break;
                lsub = nsub;
            }
            //

            convert_puts(oline);

            // The output goes to outf, i.e. the global file description tied to generated C code,
            // OR it goes to stdout, which would be if this is a command line program
            if (outf != NULL)
            {
                fprintf (outf, "%s", oline);
                va_end (args);
            }
            else
            {
                printf ("%s", oline);
                va_end (args);
            }
            //start from the beginning since just flushed
            gg_free (oline);
            oline = NULL;
            oline_len = 0;
            oline_size = 0;
            oline_prev_line = 0;
            oline_prev_line_beg = 0;
            return;
        }
        else
        {
            // nothing to flush
            va_end (args);
            return;
        }
    }

    if (oline == NULL) oline = gg_malloc (oline_size += 2*GG_MAX_CODE_LINE + 1);

    // add line number so all the code generated is the same line number
    // but only if the previous line had a new line (meaning this is the beginning of the new line of source code)
    // for example a single code line may be output by several oprintfs (this is oline_prev_line)
    if (oline_prev_line == 1 && GG_EMIT_LINE)
    {
        // make reported line as line*10000+generated_line, so each generated line is tied to original
        // line (final_line/10000), but it also has information about which generated line the diagnostic is about
        // this is used in error reporting
        static gg_num old_lnum = -1;
        static gg_num in_lnum = 0;
        if (old_lnum != lnum) { old_lnum = lnum; in_lnum = 0; } else in_lnum++;
        gg_num tot_written = snprintf (oline + oline_len, oline_size - oline_len - 1, "#line %ld \"%s\"\n", gg_plain_diag == 1 ? lnum : lnum*10000+in_lnum, src_file_name);
        if (tot_written >= oline_size - oline_len - 1)
        {
            gg_report_error ("Source code line too long, exiting");
            exit (1);
        }
        oline_len += tot_written;
        oline_prev_line = 0;
    }

    // add the actual code generated
    if (!strcmp (format, "%c"))
    {
        // optimize writing into compiler buffer for a special case of %c, avoid snprintf
        if (oline_size - oline_len - 1 < 2) oline_written = 0;
        else
        {
            oline_written = 1;
            oline[oline_len] = va_arg(args, int);
            oline[oline_len+1] = 0;
        }
    } else if (strchr (format, '%') == NULL)
    {
        gg_num l = strlen (format);
        if (oline_size - oline_len - 1 < l+1) oline_written = 0;
        else
        {
            oline_written = l;
            memcpy (oline+oline_len, format, l+1);
        }

    }
    else oline_written = vsnprintf (oline + oline_len, oline_size - oline_len - 1, format, args);
    // checking for oline_written>0 because if it's 0, it means nothing's been written or everything's flushed,
    // however oline_size and oline_len will be 0, and technically 0>=-1, but it doesn't make sense
    if (oline_written > 0 && oline_written >= oline_size - oline_len- 1)
    {
        gg_report_error ("Source code line too long, exiting");
        exit (1);
    }
    //
    bool nl;
    if ((nl = (strchr (oline + oline_len, '\n') != NULL))) 
    {
        oline_prev_line = 1;
        // check for { and } as keyword (i.e. not within strings) and adjust gg_level accordingly
    }
    oline_len += oline_written;
    if (nl)
    {
        check_level(oline + oline_prev_line_beg);
        oline_prev_line_beg = oline_len;
    }

    // expand if getting long, meaning what's left is less than GG_MAX_CODE_LINE
    if (oline_size-oline_len <= GG_MAX_CODE_LINE)
    {
        oline = gg_realloc (gg_mem_get_id(oline), oline_size += 2*GG_MAX_CODE_LINE + 1);
    }
    va_end (args);
}

#define GG_COLOR_BLUE "\033[34m"
#define GG_COLOR_NORMAL "\033[0;39m"
#define GG_COLOR_BOLD "\033[0;1m"
#define GG_COLOR_PINK "\033[35m"
// 
// Output warning to stdout. During the preprocessing with GOLF.
// There's a maximum length for it, and if it's more than that, ignore the rest.
// If there is no dot at the end, put it there.
// After that, continue program.
//
void gg_report_warning (char *format, ...)
{
    if (gg_ignore_warn) return;
    char wrntext[GG_MAX_ERR_LEN + 1];

    va_list args;
    va_start (args, format);
    vsnprintf (wrntext, sizeof(wrntext) - 1, format, args);
    va_end (args);
    fprintf (stderr, "%s%sWarning: %s", GG_COLOR_BOLD, GG_COLOR_PINK, wrntext);
    if (src_file_name != NULL)
    {
        // make sure path is displayed
        char *r_src = undecorate (src_file_name+GG_BLD_DIR_OFFSET);
        fprintf (stderr, ", reading file [%s], line [%ld]", r_src, lnum);
        gg_free(r_src);
    }
    if (wrntext[0] != 0 && wrntext[strlen (wrntext) - 1] != '.')  fprintf(stderr, ".");
    fprintf (stderr, "%s\n",GG_COLOR_NORMAL);
}

// 
// Output error to stderr. The error means error during the preprocessing with GOLF.
// There's a maximum length for it, and if it's more than that, ignore the rest.
// If there is no dot at the end, put it there.
// When gg_report_error is called, exit(1) is appended. However,
// _gg_report_error can be called multiple times before it exits (see below)
//
void _gg_report_error (char *format, ...)
{
    char errtext[GG_MAX_ERR_LEN + 1];

    oprintf (NULL); //  flush output

    va_list args;
    va_start (args, format);
    vsnprintf (errtext, sizeof(errtext) - 1, format, args);
    va_end (args);
    fprintf (stderr, "%s%s%s", GG_COLOR_BOLD, GG_COLOR_BLUE, errtext);
    if (src_file_name != NULL)
    {
        // make sure path is displayed
        char *r_src = undecorate (src_file_name+GG_BLD_DIR_OFFSET);
        fprintf (stderr, ", reading file [%s], line [%ld]", r_src, lnum);
        gg_free(r_src);
    }
    if (errtext[0] != 0 && errtext[strlen (errtext) - 1] != '.')  fprintf(stderr, ".");
    fprintf (stderr, "%s\n",GG_COLOR_NORMAL);
    fflush (stderr);
    gg_curr_errors++;
    if (gg_curr_errors >= gg_max_errors) exit (1);
}


// 
// Add new query. gen_ctx is the context, vp is the query parsing context.
// Error messages are emitted if there are syntax errors.
//
void new_query (gg_gen_ctx *gen_ctx, gg_db_parse *vp)
{
    char *orig_qry = vp->eq; // orig_qry is the SQL text of query

    //
    // This creates new query, nested if needed, gen_ctx->curr_qry_ptr is the depth of it and 
    // gen_ctx->curr_qry_ptr-1 is also query_id
    //
    // this increases the number of open queries
    check_next_query
    // move the query stack pointer one up. At this location in the stack, there is
    // nothing, i.e. this pointer always points to the next (as of yet non-existent)
    // query ID
    gen_ctx->curr_qry_ptr ++;
    // check if we're nesting too much
    if (gen_ctx->curr_qry_ptr >= GG_MAX_QUERY_NESTED) gg_report_error( GG_MSG_NESTED_QRY, query_id, GG_MAX_QUERY_NESTED);
    // now this query ID is active. We use it to prohibit nesting queries with the same ID
    gen_ctx->qry_active[query_id] = GG_QRY_ACTIVE;
    // this is start-query, so the query has now officially been used
    gen_ctx->qry_used[query_id] = GG_QRY_USED;
    //
    // End new query creation
    //

    char *qry=gg_strdup(orig_qry);
    gg_num ql = strlen(qry);
    // 
    // We do not touch % signs, because we do NOT use snprintf like function at run time in gg_make_SQL
    // '%s' will be replaced with input parameters, others won't be touched
    //
    // Sanity check and error
    if (qry[0] != 0 && qry[ql - 1] == ';')
    {
        gg_report_error( "Query cannot end with a semicolon");
    }
    // actually add final query into the array of queries, curr_query is query ID
    // 
    // Initialize query once the text of query has been processed 
    // Note that query already exists - it is under [query_id] in array of queries
    // Set status of query (UNUSED), initialize number of input parameters, set returns_tuple flag, increase total number of
    // queries (if needed) and such. Other things about the query, such as query input parameters, are already set.
    //
    gen_ctx->qry[query_id].text = qry;
    // create name for a query, it's an internal unique name. It is based on total # of queries so far,
    // since there can be many queries on the same level, say query (id 0), inside of it query (id 1), then
    // both end, so the next one is id 0 again, and one inside it id 1 again.
    name_query (gen_ctx);
    gen_ctx->qry[query_id].returns_tuple = 1;
    gen_ctx->qry[query_id].qry_found_total_inputs = 0;
    gen_ctx->qry[query_id].unknown_output = 0;
    gen_ctx->qry[query_id].qry_total_outputs = 0;
    // only up the number of queries if this is a new query
    // query, once used, is always used. So mark it used only once it added the very first time
    gen_ctx->qry_used[query_id] = GG_QRY_UNUSED;
    gen_ctx->total_queries++;
    if (gen_ctx->total_queries >= GG_MAX_QUERY)
    {
        gg_report_error("Too many queries specified");
    }
}

// 
// Find current query ID - all references to queries are implicit and refer to the current one. gen_ctx is the context.
// Returns query ID or -1 if none found..
//
gg_num find_query (gg_gen_ctx *gen_ctx)
{
    return gen_ctx->curr_qry_ptr - 1;
}


// 
// Change pointer to string pointer s, so it advances until either 1) a comma is encountered or 2) the end of the string
// The pointer is now at either one
//
void get_until_comma (char **s)
{
    gg_num i = 0;
    while (((*s)[i])!=',' && (*s)[i]!=0) i++;
    *s = *s + i;
}

// 
// Change pointer to string pointer s, so it advances until either 1) a whitespace is encountered or 2) the end of the string
// The pointer is now at whitespace
//
void get_until_whitespace (char **s)
{
    gg_num i = 0;
    while (!isspace ((*s)[i])) i++;
    *s = *s + i;
}

// 
// Change pointer to string pointer s, so it advances until either 1) a non-whitespace is encountered or 2) the end of the string
// The pointer is now non-whitespace (presumably after skipping bunch (or zero) of whitespaces)
//
void get_passed_whitespace (char **s)
{
    gg_num i = 0;
    while (isspace ((*s)[i])) i++;
    *s = *s + i;
}



//
// Recognition of a statement. cinp is the line read from source file, pos is the current byte position in it (from where to 
// search for a statement), opt is the statement itself (a string to search for). Outputs mtext (which is the pointer to just after
// the statement, minus leading spaces), msize is the length of it. 
// isLast is 1 if this statement should NOT have any data after it, 0 if it should.
// Returns byte position in cinp right after where statement is found, or 0 if opt statement not recognized or if recognized, but
// there is more immediately appended text (else but there could be else-if actually).
// mtext is null-terminated so it can be gg_trim()-ed.
// is_inline is 1 if this was the end (>>) of <<..>> inline statement
// Errors out if there is an unterminated string after statement, or if arguments aren't there (either end of string or >>)
//
gg_num recog_statement (char *cinp, gg_num pos, char *opt, char **mtext, gg_num *msize, gg_num isLast, gg_num *is_inline)
{


    *is_inline = 0; // by default this isn't closed by >>

    // do not assign mtext in case of not-found, because it could override the found one when
    // we check for multiple recog_statement() in the same if(), same for msize
    gg_num opt_len = strlen (opt);
    gg_num orig_position = pos;
    if (!memcmp (cinp+pos, opt, opt_len))
    {
        num_recog++;        
        *mtext = cinp+pos+opt_len;
        // can NOT skip leading spaces, because there's at least one space, and it is considered a keyword
        // in carve_statement(), so it must be there
        pos += opt_len;
        if (isLast == 1)
        {
            //
            // Nothing is expected after this statement, i.e. no text
            //
            while (isspace(cinp[pos])) pos++;
            // we consider end of line to be the same as >> 
            // we already handle \ (line continuation) before we encounter the first recog_statement
            // (this is done during reading from the C file)
            gg_num is_end_of_inline = !memcmp (cinp+pos, ">>", 2) ? 1: 0;
            if (is_end_of_inline == 1)
            {
                *is_inline = 1;
                if (within_inline != 1)  gg_report_error ("End of inline detected (>>), but no beginning (<<)");
                within_inline = 0;
            }
            if (cinp[pos] == 0 || is_end_of_inline == 1)
            {
                usedGOLF = 0;
                *msize = (cinp+pos)-*mtext;
                (*mtext)[*msize] = 0; // zero the >>
                GG_VERBOSE(lnum,"Markup [%s] found", opt);
                // golf line never has new line in it, so this should be fine
                // if statement has new line, as in set-string unquoted with \\ at the end
                // At this point statements have no original comments to interfere
                char *nl = strchr (cinp+orig_position, '\n');
                if (nl) *nl = 0;
                oprintf("\n//%ld: %s\n", lnum,cinp+orig_position);
                if (nl) *nl = '\n';
                return pos + 1; // cinp[pos+1] must be '>' in the last '>>', i.e. the last char
            }
            else
            {
                return 0; // there can be else versus else-if : do not say 'extra characters...'
            }
        }
        else
        {
            // find >> (if present), which would finish the statement, so we can null-bind mtext.
            // if not, then mtext ends at end-of-line. In both cases, unterminated strings and unbalanced
            // () are found and error raised.
            char *eofc = NULL;
            if ((eofc = find_keyword(cinp+pos, ">>", 0)) != NULL)
            {
                *is_inline = 1;
                if (within_inline != 1)  gg_report_error ("End of inline detected (>>), but no beginning (<<)");
                within_inline = 0;
                *eofc = 0;
            }
            else
            {
               eofc = find_keyword(cinp+pos, "", 0);
            }
            pos += (eofc - (cinp+pos));
            //
            // After this statement, we expect more text
            // We expect space after statement keyword
            // We also don't look for statement ending (i.e. >>) if it is quoted, such as 
            // for example with query text.
            // We will look for \" inside a string (an escaped quote), because C code might have this, so an
            // escaped quote is NOT end of the string. This is useful primarily in C code (c statement).
            // We do NOT interpret \", it is specified
            // so in the string and it remains so (i.e. we don't switch \" for " or anything at all). 
            // This does not affect SQL queries (think injection) because this affects only the code
            // programmer writes, and we also use ANSI single quotes for all sessions.
            //
            *msize = (cinp+pos)-*mtext;
            // Check that statement doesn't end with semicolon. @ and ! are covered outside recog_statement()
            // and we check this isn't C code
            if (strcmp(opt, ".") && *msize>0 && (*mtext)[*msize-1] == ';') 
            {
                gg_report_error( "Statement '%s' cannot end with semicolon.", opt);
            }
            // if opt is empty, the whole golf marker is c code, just pass it back
            //
            // in case some statements are subsets of other (such as else versus else-if)
            // we expect a space after it (since it's not the last!)
            // The exception is '.' statement, which doesn't need a space but has
            // text after it.
            if (cinp[orig_position+opt_len] != ' ' && strcmp(opt,"."))
            {
                // check if there's nothing after keyword - something must be there
                // note that input is trimmed, so even if bunch of spaces after keyword, it will be as nothing.
                if (orig_position+opt_len == pos)
                {
                    // error out if only spaces (through the end of statement) follow the statement key word - this means
                    // else and else-if will NOT error out, but only legitimate issues
                    gg_report_error( "Statement '%s' has no arguments, but it should", opt);
                }
                return 0; // must have space afterward
            }
            usedGOLF = 0;
            GG_VERBOSE(lnum,"Markup [%s] found", opt);
            char *nl = strchr (cinp+orig_position, '\n');
            if (nl) *nl = 0;
            oprintf("//%ld: %s\n", lnum, cinp+orig_position);
            if (nl) *nl = '\n';
            return pos + 1;
        }
    }
    return 0;
}


// 
// Initialize context gen_ctx. Initialize command mode, all queries, query stack, string writing balancer
// and database location.
//
void init_gg_gen_ctx (gg_gen_ctx *gen_ctx)
{
    GG_TRACE("");

    gg_num i;
    gg_num j;
    for (j = 0; j < GG_MAX_QUERY; j++) 
    {
        gen_ctx->qry[j].returns_tuple = 1;
        for (i = 0; i < GG_MAX_QUERY_INPUTS; i++)  
        {
            gen_ctx->qry[j].qry_inputs[i][0] = 0;
        }
    }

    gen_ctx->total_queries = 0;

    gen_ctx->curr_qry_ptr = 0;

    gen_ctx->total_write_string = 0;


}




// 
// Main code generation function. gen_ctx is the context, file_name is the file
// that is being processed.
// Errors out if cannot open file and for many other reasons (including syntax and 
// other errors in statement). 
// Generates the code which is output according to the options used (see the documentation).
//
void gg_gen_c_code (gg_gen_ctx *gen_ctx, char *file_name)
{
    FILE *f;


    f = gg_fopen (file_name, "r");
    GG_VERBOSE(0,"Starting");
    if (f == NULL)
    {
        gg_report_error( "Error opening file [%s]", file_name);
    }


    //
    // Start reading line by line from the source file
    //
    char *res;

    GG_VERBOSE(0,"Opened your file [%s]",file_name);

    gg_num json_id = 0;
    gg_num xml_id = 0;
    gg_num open_readline = 0;
    gg_num open_ifs = 0;
    gg_num open_curly=0;
    gg_num line_len = 0; // unlike 'len' below, used only for concatenation of lines

    gg_num http_header_count = 0; // used for _gg_tmp_header_%ld 
    gg_num encrypt_count=0; // used for encryption length var
    gg_num file_count=0; // used for static file descriptors
    gg_num trim_count=0; // used for trim length var
    gg_num code_text = 0; // counter for status/errt vars
    gg_num regex_cache = 0; // used to cache regex compilation


    gg_num do_once = 0;
    gg_num do_once_line = 0;
    bool ccomm_open = false; // true if C comment opened in some line prior
    gg_num ccomm_line = -1; // line where /* C comment opened


    // Include golf.h so it's not necessary, doubles are prevented with ifdefs inside
    oprintf("#include \"golf.h\"\n");
    oprintf("#include \"gg_sparam_%s.h\"\n",gg_basename(file_name));

    bool line_split = false;

    // 
    // Main loop in which lines are read from the source file
    //
    while (1)
    {
        res = fgets_unlocked (line + line_len, sizeof (line) - line_len- 1, f);
        if (res == NULL) // either end or error
        {
            gg_num err = ferror_unlocked (f);
            if (err) // if error, quit
            {
                gg_report_error( "Error [%s] reading file [%s]", strerror (errno), file_name);
            }
            break; // nothing read in, exit normally
        }
        lnum++;

        // 
        // Setup line number for error reporting. Print new line in front
        // so if we forget to finish a line with new line, this would ruin
        // C syntax.
        // For internal debugging, may be useful not to have it.
        //
        if (GG_EMIT_LINE)
        {
            oprintf("\n#line %ld \"%s\"\n", gg_plain_diag == 1 ? lnum : lnum*10000, file_name);
        }

        gg_num len;
        // 
        // if this is continuation, trim the continuation to avoid big gaps in between pieces
        // Lines can be concatenated
        //
        if (line_len > 0)
        {
            gg_num cont_len = strlen (line+line_len);
            // check if continuation starts with space. If it doesn't, we cannot trim.
            if (cont_len > 0)
            {
                // Concatenation means just that - concatenation.
                // each line in concatenation is trimmed both ends
                gg_trim (line+line_len, &cont_len, false);
            }
            len = line_len + cont_len;
        }
        else
        {
            // if line_len is zero then we trim line (meaning also on the left). If line_line > 0, then the above will trim on the right,
            // no need to trim on the left too again
            len = strlen (line);
            gg_trim (line, &len, false);
        }


        if (len >= (gg_num)sizeof (line) - 16) // don't allow too big of a line
        {
            gg_report_error( "Line too long");
        }

        gg_num i;

        if (len > 0 && line[len - 1] == '\\')
        {
            // check if two \ (meaning \\) are at the end - this is continuation but with new line
            if (len > 1 && line[len-2] == '\\')
            {
                line[len - 2] = '\n';
            }
            // continuation of the line
            line[len - 1] = 0;
            line_len = len - 1;
            line_split = true;
            continue; // read more
        }
        line_len = 0;

        GG_VERBOSE(lnum,"Got [%s]", line);

        char *mtext = NULL;
        gg_num msize = 0;
        gg_num first_on_line = 1;
        gg_num is_verbatim = 0; // 1 if #  to print out
        within_inline = 0; // if statement is within <<...>>
        usedGOLF = 0;
        print_mode = 0; // if in @ or !
        emptyc_c = 0;

        // 
        //
        // Comments: both // and /**/ are handled here
        // Not trivial. Matches gcc behavior as best as possible. Generally /**/
        // takes precedence over //, unless /* is after //
        // /**/ at the beginning of statement is not allowed
        //
        //
        // Check for // comment, if found (and not in quotes), anything after that is ignored
        //
        // // comment: if not quoted, everyting after it is blanked out
        gg_num op_i = 0;
        bool line_comm = false; // true if line starts with /**/ comment, we disallow those before beginning of statement as it's hard to read
                                    // and also detecting statements include/libs in gg become way more difficult
        gg_num ccomm_pos = -1; // position of /* that starts multiline comment
        while (isspace (line[op_i])) op_i++; // find beginning of line (ignore whitespace)
        bool linebeg = op_i; // this is where non-space of line begins
        // if this is @ or !, all comments are ignored, this is whole-line output statement
        if (ccomm_open || (memcmp(line+op_i, "@",1) && memcmp(line+op_i, "!",1))) 
        {
            // Check for /**/ comment
            // There can be multiple /**/ comments on a line, so this must be a loop
            bool skip_line=false;
            gg_num lcheck = 0;
            bool closing_ccomm_found = false;// true if */ potentially found without matching /* on the same line
                                             // may be a false alarm if */ is after //
            gg_num closing_ccomm_pos = -1;// position of closing */ per above bool
            while (1)
            {
                char *cl_comment = gg_find_keyword0 (line+lcheck, "/*", 0, 0);
                char *cr_comment = gg_find_keyword0 (line+lcheck, "*/", 0, 0);
                // calculate where /* and */ are, meaning the first ones found
                gg_num l_pos = -1;
                gg_num r_pos = -1;
                if (cl_comment != NULL) l_pos = cl_comment - line;
                //
                // Processing right */ MUST be after processing left /* because l_pos is necessary below
                //
                if (cr_comment != NULL) 
                {
                    r_pos = cr_comment - line;
                    // either this */ with /* being on prior line OR the /* is the first that starts on line
                    // so below if the actual statement starts after either one of these, it's an error
                    // note that if neither has start of useful code afterward, then it won't matter, for instance
                    // if some comments continues to next line, then that line will be the one to judge, or if there's more /*...*/
                    // continuing on the same line we do memmove to make it look like it was the beginning.
                    if (!line_comm && (ccomm_open || linebeg == l_pos)) line_comm = true;
                }
                // If /* started at some point and not */ here, then ignore line
                if (ccomm_open)
                {
                    if (cr_comment == NULL) 
                    {
                        skip_line = true;
                        break; // this is under multiline /* */ comment
                    }
                    else
                    {
                        // this is ...*/... which must be closure of multiline /**/ comment
                        // So ccomm_open must be true. We don't care if there was /* before */, that's part of the comment
                        memmove (line, cr_comment + 2, len-r_pos-2+1); //+1 at the end is to include null byte
                        len = len - r_pos - 2; // length is smaller
                                             // we ignore all up to */ and continue afterwards
                        lcheck = 0; // continue from the beginning of code after /*
                        ccomm_open = false; // no longer multiline /**/ comment
                        continue;
                    }
                }
                if (l_pos == -1 && r_pos == -1) break; // no comments on line that's to be processed
                                                                     // since it's not in multiline /**/ comment
                if (l_pos != -1 && r_pos == -1) 
                {
                    // has /* but no closing */
                    len = cl_comment - line; // length is smaller once you cut out the rest of the line
                    line[len] = 0; // /* is not ending in this line, so cut it out
                    ccomm_open = true; // we're now in open-ended /* which will stretch more than one line
                    ccomm_line = lnum; // line where open-ended /* started
                    ccomm_pos = len; // where /* is
                    break; // because after /* without */, anything after that is a comment, so we got line
                } 
                else if (l_pos != -1 && r_pos != -1) 
                {
                    // we have left /* and right */ comment on the same line, so blank it out
                    // but which one comes first?
                    if (l_pos < r_pos) 
                    {
                        char *ci;
                        for (ci = cl_comment; ci != cr_comment+2; ci++) *ci=' ';
                        lcheck = r_pos + 2; // start looking again after this comment blanked out
                    } 
                    else
                    {
                        // we have already handled closure of multiline comment */ above, so this is an error
                        // to have */ to lead the line - this will be handled further out of this loop under cpp_comment!=NULL
                        // because if there's // prior to it, then it's a comment and not an error - in which case the rest of the 
                        // line doesn't matter, so break
                        closing_ccomm_found = true;
                        closing_ccomm_pos = r_pos;
                        break;
                    }
                } 
                else if (l_pos == -1 && r_pos != -1) 
                {
                    // this is ...*/... which must be closure of multiline /**/ comment
                    // But we already handled that, so this is an error,
                    // to have */ to lead the line - this will be handled further out of this loop under cpp_comment!=NULL
                    // because if there's // prior to it, then it's a comment and not an error - in which case the rest of the
                    // line doesn't matter, so break
                    closing_ccomm_found = true;
                    closing_ccomm_pos = r_pos;
                    break;
                }
                // cannot happen l_pos == -1 && r_pos == -1, as we checked for that above
            }
            // Look for // only after all /**/ have been processed. If // still here, then the only
            // case where it affect the outcome is if there's /* after //, which should be ignored
            char *cpp_comment = gg_find_keyword0 (line, "//", 0, 0);
            if (cpp_comment != NULL)
            {
                *cpp_comment = 0;
                len = cpp_comment - line; // after removing the comment, length is smaller
                // if /* for multiline is after //, then ignore it
                if (ccomm_pos !=-1 && ccomm_pos > len) ccomm_open = false;
                // if */ is found after //, then ignore it, unless it closes a multi-line /**/. If it doesn't
                // then it's ignored 
                if (closing_ccomm_found && closing_ccomm_pos < len) gg_report_error( "Closing */ C comment found, but there was no opening /* comment");
            }
            if (skip_line) continue; // this line is a line where /* started before it, but no */ found yet, so ignore
        }
        //
        // END comment handling
        //

        // If in @, this is if >> was just processed (end of inline), so start printing again
        // as we have temporarily suspended printing when << was found (by doing gg_puts(<end of string>...)
        // reset gg_is_inline afterwards so any new checks (aside from recog_statement() which sets gg_is_inline)
        // do not get wrong value
#define GG_OPEN_PRINT  if (gg_is_inline == 1 && print_mode == 1) oprintf("gg_puts (GG_NOENC, \""); \
        gg_is_inline = 0; 


        // check if line is empty, if not, and if there was /**/ prior to it, that's not allowed (see above comments)
        op_i = 0;
        while (isspace (line[op_i])) op_i++; // find beginning of line (ignore whitespace)
        if (line[op_i] != 0 && line_comm) gg_report_error ("Statement cannot start with /**/ comment");

        //
        // In this loop, a line is examined char by char. However, once certain patterns are recognized, parsing
        // progresses more rapidly. Basically, recog_statement() is called to recognized the beginning of the GOLF statement,
        // and from there the rest is parsed in a more rapid fashion. Each statement is checked, and once recognized and
        // processed, the next line is read in the outer loop.
        // Skip whitespaces
        //
        for (i = op_i; i < len; i++)
        {
            gg_num newI = 0;
            gg_num newI1 = 0;
            gg_num newI2 = 0;
            gg_num newI3 = 0;
            gg_num newI4 = 0;
            gg_num newI5 = 0;
            gg_num newI6 = 0;
            gg_num newI7 = 0;
            gg_num newI8 = 0;

            // since every statement, after it's recognized and consumed, does continue, it comes back here.
            // so if statement was inline, and within @, then we must start printing. This is first thing right 
            // after >>. We stopped printing at <<. So this continues printing; see the GG_OPEN_PRINT after this for loop
            GG_OPEN_PRINT

            if (is_verbatim == 0) 
            {

                if (!memcmp (line + i, "<<", 2)) // short directive
                {
                    i = i + 2; // past "<<"
                    within_inline = 1;
                    usedGOLF = 1;
                    if (print_mode == 1)
                    {
                        // Finish current portion of @ line, so that <<>> can do its thing
                        // if not in @, then just execute
                        oprintf("\", GG_EMPTY_LONG_PLAIN_ZERO, false);\n");
                    }
                }
                else
                {
                    ; // else? nothing. We consider new line start within GG_BEGIN/GG_END block to be the start
                    // of the <<golf statement>>, for simplicity of coding.
                }


                // checking for GOLF statement without <<..>> can only work in those statements where they are the first on the 
                // line, OR if this is actual statement << ...>>
                if (first_on_line == 1 || usedGOLF == 1)
                {
                    while (isspace (line[i])) i++;
                    first_on_line = 0;

                    //
                    // All the following statements use the same recipe, calling recog_statement()
                    // and advancing 'i' which points to the next segment to be parsed, while mtext/msize are the text of the 
                    // statement (after the initial statement keyword). The 1 or 0 after msize tells if this statement is alone or if it
                    // needs more data after the statement keyword. For example, end-query has nothing after the statement keyword 'end-query'
                    // and so it is alone on the line and argument following msize is 1.
                    //
                    // Parsing done typically involves finding keywords and then dividing the string into smaller pieces (based on
                    // the location of these keywords and the information contained between them or between such keywords and the end of string), typically
                    // a single piece of information is to be found in each piece. It's a
                    // parsing mechanism that's more suited for statement-like statements, where such statements are given through parameters
                    // which control what is being done, on what data, and how. Examples of statements given in the documentation present the 
                    // use scenarios. Markups are generally quite simple and more importantly are designed to be simple
                    // and to streamline commonly used functionality in an 80/20 fashion. The 20% functionality is typically done in pure C, and
                    // 80% functionality in simple statements. This tends to reduce not only the number of bugs, but also to force clearer and simpler
                    // programming (while the code generate is still C and thus typically faster than most anything else).
                    // 
                    //
                    if ((newI=recog_statement(line, i, "end-query", &mtext, &msize, 1, &gg_is_inline)) != 0) // is it end of query action block
                    {
                        GG_GUARD
                        i = newI;
                        carve_stmt_obj (&mtext, false, false);

                        end_query (gen_ctx,  1);

                        continue; // skip the statement and continue analyzing 
                    }
                    else if ((newI=recog_statement(line, i, "start-loop", &mtext, &msize, 1, &gg_is_inline)) != 0 
                        || (newI1=recog_statement(line, i, "start-loop", &mtext, &msize, 0, &gg_is_inline)) != 0) 
                    {
                        GG_GUARD
                        i = newI+newI1;
                        check_next_loop
                        if (newI != 0)
                        {
                            oprintf ("while (1) {\n");
                        }
                        else
                        {
                            char *repeat = find_keyword (mtext, GG_KEYREPEAT, 0);
                            char *use = find_keyword (mtext, GG_KEYUSE, 1);
                            char *swith = find_keyword (mtext, GG_KEYSTARTWITH, 1);
                            char *add = find_keyword (mtext, GG_KEYADD, 1);
                            carve_statement (&repeat, "start-loop", GG_KEYREPEAT, 0, 1);
                            carve_statement (&use, "start-loop", GG_KEYUSE, 0, 1);
                            carve_statement (&swith, "start-loop", GG_KEYSTARTWITH, 0, 1);
                            carve_statement (&add, "start-loop", GG_KEYADD, 0, 1);
                            carve_stmt_obj (&mtext, false, false);
                            define_statement (&use, GG_DEFNUMBER, false); 
                            //
                            check_var (&repeat, GG_DEFNUMBER);
                            check_var (&swith, GG_DEFNUMBER);
                            check_var (&add, GG_DEFNUMBER);

                            static gg_num _gg_loop = 0;
                            //if (repeat == NULL  && use != NULL) gg_report_error("Must specify 'repeat' clause with 'use'");
                            if ((swith != NULL || add != NULL) && use == NULL) gg_report_error("Must specify 'use' clause with 'start-with' or 'add' clause");
                            if (use != NULL && gg_is_valid_param_name(use, false, false) != 1) gg_report_error(GG_NAME_INVALID, use);
                            if (use != NULL) oprintf("%s=%s-%s;\n", use, swith!=NULL?swith:"1", add==NULL?"1":add); // -<add> as we do ++/+= at the beginning of the loop
                            // allow for loop where we have repeat and don't have it
                            // if we don't have repeat, then just keep incrementing 'use' and we must use continue or break to get out
                            if (repeat != NULL) 
                            {
                                // in a common scenario when add is 1 and using a variable, just use this variable and avoid two counters (better performance)
                                if ((add == NULL || !strcmp (add, "1")) && use != NULL) 
                                {
                                    oprintf ("gg_num _gg_loop_end%ld=(%s)+(%s);\n", _gg_loop, swith==NULL?"1":swith, repeat);
                                    oprintf ("for (%s=(%s); %s < _gg_loop_end%ld; (%s)++) {\n", use, swith==NULL?"1":swith, use, _gg_loop, use);
                                }
                                else 
                                {
                                    if (use != NULL)
                                    {
                                        // in this case add!=NULL (we checked above add=NULL && use != NULL)
                                        oprintf ("gg_num _gg_loop_end%ld=(%s)+1;\n", _gg_loop, repeat);
                                        oprintf ("gg_num _gg_loop%ld;\nfor (_gg_loop%ld=1, (%s)=(%s); _gg_loop%ld < _gg_loop_end%ld; _gg_loop%ld++,(%s)+=(%s)) {\n", _gg_loop, _gg_loop, use, swith==NULL?"1":swith, _gg_loop, _gg_loop, _gg_loop, use, add); // +1 needed with 'repeat' because we want the 'use' to come out at number over after the loop
                                    }
                                    else
                                    {
                                        oprintf ("gg_num _gg_loop%ld;\nfor (_gg_loop%ld=1; _gg_loop%ld < (%s)+1; _gg_loop%ld++) {\n", _gg_loop, _gg_loop, _gg_loop, repeat, _gg_loop); // +1 needed with 'repeat' because we want the 'use' to come out at number over after the loop
                                    }
                                }
                            }
                            else oprintf ("gg_num _gg_loop%ld;\nfor (_gg_loop%ld=1; ; _gg_loop%ld++) {\n", _gg_loop, _gg_loop,  _gg_loop);

                            _gg_loop++;
                        }
                        continue; // skip the statement and continue analyzing 
                    }
                    else if ((newI=recog_statement(line, i, "end-defined", &mtext, &msize, 1, &gg_is_inline)))
                    {
                        //not part of code block
                        //GG_GUARD
                        i = newI;

                        oprintf ("#endif\n");

                        continue; // skip the statement and continue analyzing 
                    }
                    else if ((newI=recog_statement(line, i, "if-defined", &mtext, &msize, 0, &gg_is_inline)) != 0
                     || (newI1=recog_statement(line, i, "if-not-defined", &mtext, &msize, 0, &gg_is_inline)) != 0) 
                    {
                        //not part of code block
                        //GG_GUARD
                        i = newI+newI1;

                        char *eq = find_keyword (mtext, GG_KEYEQUAL, 1);
                        char *neq = find_keyword (mtext, GG_KEYNOTEQUAL, 1);
                        char *less = find_keyword (mtext, GG_KEYLESSERTHAN, 1);
                        char *lesseq = find_keyword (mtext, GG_KEYLESSEREQUAL, 1);
                        char *gr = find_keyword (mtext, GG_KEYGREATERTHAN, 1);
                        char *greq = find_keyword (mtext, GG_KEYGREATEREQUAL, 1);

                        carve_statement (&eq, "if-defined", GG_KEYEQUAL, 0, 1); 
                        carve_statement (&neq, "if-defined", GG_KEYNOTEQUAL, 0, 1); 
                        carve_statement (&less, "if-defined", GG_KEYLESSERTHAN, 0, 1); 
                        carve_statement (&lesseq, "if-defined", GG_KEYLESSEREQUAL, 0, 1); 
                        carve_statement (&gr, "if-defined", GG_KEYGREATERTHAN, 0, 1); 
                        carve_statement (&greq, "if-defined", GG_KEYGREATEREQUAL, 0, 1); 
                        carve_stmt_obj (&mtext, true, false);
                        // the name that's defined from either gg compilation (--cflag) or specified by the platform (such as
                        // __x86_64__), can start with "_"
                        if (!(mtext[0] == '_' || isalpha (mtext[0])) && gg_is_valid_param_name(mtext+1, false, false) != 1) gg_report_error(GG_NAME_INVALID, mtext);

                        if (newI1 != 0 && (eq != NULL || neq != NULL || less != NULL || lesseq != NULL || gr != NULL || greq != NULL)) gg_report_error ("Cannot use comparison clauses with if-not-defined");
                        gg_num tot = (eq != NULL ?1:0)+(neq != NULL ?1:0)+(less != NULL ?1:0)+(lesseq != NULL ?1:0)+(gr != NULL ?1:0)+(greq != NULL ?1:0);
                        if (tot > 1) gg_report_error ("Only a single comparison operator can be used with if-defined");
                        char *op = GG_EMPTY_STRING;
                        if (tot == 1) 
                        {
                            op = (eq!=NULL?eq:(neq!=NULL?neq:(less!=NULL?less:(lesseq!=NULL?lesseq:(gr!=NULL?gr:(greq!=NULL?greq:NULL))))));
                            gg_num st;
                            gg_str2num (op, 0, &st);
                            if (st != GG_OKAY)  gg_report_error ("Invalid number in if-defined comparison");
                        }

                        if (newI != 0) 
                        {
                            if (eq != NULL) oprintf ("#if %s==%s\n", mtext, op);
                            else if (neq != NULL) oprintf ("#if %s!=%s\n", mtext, op);
                            else if (less != NULL) oprintf ("#if %s<%s\n", mtext, op);
                            else if (lesseq != NULL) oprintf ("#if %s<=%s\n", mtext, op);
                            else if (gr != NULL) oprintf ("#if %s>%s\n", mtext, op);
                            else if (greq != NULL) oprintf ("#if %s>=%s\n", mtext, op);
                            else oprintf ("#ifdef %s\n",mtext);
                        }
                        else oprintf ("#ifndef %s\n",mtext);

                        continue; // skip the statement and continue analyzing 
                    }
                    else if ((newI=recog_statement(line, i, "{", &mtext, &msize, 1, &gg_is_inline)) != 0) 
                    {
                        //not part of code block
                        //GG_GUARD
                        i = newI;

                        carve_stmt_obj (&mtext, false, false);
                        oprintf ("{\n");
                        open_curly++;
                        continue; // skip the statement and continue analyzing 
                    }
                    else if ((newI=recog_statement(line, i, "}", &mtext, &msize, 1, &gg_is_inline)) != 0) 
                    {
                        //not part of code block
                        //GG_GUARD
                        i = newI;

                        carve_stmt_obj (&mtext, false, false);
                        oprintf ("}\n");
                        open_curly--;

                        continue; // skip the statement and continue analyzing 
                    }
                    else if ((newI=recog_statement(line, i, "pause-program", &mtext, &msize, 0, &gg_is_inline)) != 0) 
                    {
                        GG_GUARD
                        i = newI;

                        carve_stmt_obj (&mtext, true, false);
                        check_var (&mtext, GG_DEFNUMBER);
                        oprintf ("gg_sleepabit (%s);\n", mtext);

                        continue; // skip the statement and continue analyzing 
                    }
                    else if ((newI=recog_statement(line, i, "continue-loop", &mtext, &msize, 1, &gg_is_inline)) != 0) 
                    {
                        GG_GUARD
                        i = newI;

                        carve_stmt_obj (&mtext, false, false);
                        if (open_loops <= 0 && query_id == -1 && open_readline <= 0) gg_report_error ("continue-loop must be between start-loop and end-loop statements, or within a run-query/end-query statements, or within read-line and end-read-line");
                        oprintf ("continue;\n");

                        continue; // skip the statement and continue analyzing 
                    }
                    else if ((newI=recog_statement(line, i, "break-loop", &mtext, &msize, 1, &gg_is_inline)) != 0) // is it end of query action block
                    {
                        GG_GUARD
                        i = newI;

                        carve_stmt_obj (&mtext, false, false);
                        if (open_loops <= 0 && query_id == -1 && open_readline <= 0) gg_report_error ("break-loop must be between start-loop and end-loop statements, or within a run-query/end-query statements, or within read-line and end-read-line");
                        oprintf ("break;\n");

                        continue; // skip the statement and continue analyzing 
                    }
                    else if ((newI=recog_statement(line, i, "end-loop", &mtext, &msize, 1, &gg_is_inline)) != 0) // is it end of query action block
                    {
                        GG_GUARD
                        i = newI;

                        carve_stmt_obj (&mtext, false, false);
                        oprintf ("}\n");
                        open_loops--;

                        continue; // skip the statement and continue analyzing 
                    }
                    // Checking for non-mandatory (6th param is 1) MUST be done first.
                    else if (((newI=recog_statement(line, i, "rollback-transaction", &mtext, &msize, 1, &gg_is_inline)) != 0) 
                     || ((newI1=recog_statement(line, i, "rollback-transaction", &mtext, &msize, 0, &gg_is_inline)) != 0)) 
                    {
                        GG_GUARD
                        i = newI+newI1;
                        char *opt = find_keyword (mtext, GG_KEYOPTIONS, 1);
                        char *database = find_keyword (mtext, GG_KEYAT, 0);
                        char *oncont = find_keyword (mtext, GG_KEYONERRORCONTINUE, 1);
                        char *onexit = find_keyword (mtext, GG_KEYONERROREXIT, 1);
                        char *errt = find_keyword (mtext, GG_KEYERRORTEXT, 1);
                        char *err = find_keyword (mtext, GG_KEYERROR, 1);
                        carve_statement (&opt, "rollback-transaction", GG_KEYOPTIONS, 0, 1);
                        carve_statement (&database, "rollback-transaction", GG_KEYAT, 0, 1);
                        carve_statement (&oncont, "rollback-transaction", GG_KEYONERRORCONTINUE, 0, 0);
                        carve_statement (&onexit, "rollback-transaction", GG_KEYONERROREXIT, 0, 0);
                        carve_statement (&errt, "rollback-transaction", GG_KEYERRORTEXT, 0, 1);
                        carve_statement (&err, "rollback-transaction", GG_KEYERROR, 0, 1);
                        carve_stmt_obj (&mtext, false, false);
                        make_mem(&opt);
                        // database is not a string constant or variable, though it's stringified internally, so no make_mem
                        define_statement (&errt, GG_DEFSTRING, false); // allocated with strdup in db.c, exact length
                        define_statement (&err, GG_DEFSTRING, false); // allocated with strdup in db.c, exact length
                        //
                        check_var (&opt, GG_DEFSTRING);

                        get_db_config (database);
                        //erract is GG_ON_ERROR_CONTINUE/EXIT for statement-specific on-error continue/exit or GG_OKAY if db-level on-error is in effect
                        oprintf("gg_rollback (\"%s\", %s, %s%s%s, %s%s%s);\n", opt==NULL?"":opt, oncont!=NULL?"GG_ON_ERROR_CONTINUE":(onexit!=NULL?"GG_ON_ERROR_EXIT":"GG_OKAY"),
                            err!=NULL?"&(":"", err !=NULL?err:"NULL", err!=NULL?")":"", 
                            errt!=NULL?"&(":"", errt !=NULL?errt:"NULL", errt!=NULL?")":""
                            );
                        continue;
                    }
                    else if (((newI=recog_statement(line, i, "db-error", &mtext, &msize, 0, &gg_is_inline)) != 0)) // commit transaction
                    {
                        GG_GUARD
                        i = newI;
                        char *database = find_keyword (mtext, GG_KEYAT, 0);
                        char *ex = find_keyword (mtext, GG_KEYEXIT, 1);
                        char *cont = find_keyword (mtext, GG_KEYCONTINUE, 1);
                        carve_statement (&database, "db-error", GG_KEYAT, 0, 1);
                        carve_statement (&ex, "db-error", GG_KEYEXIT, 0, 0);
                        carve_statement (&cont, "db-error", GG_KEYCONTINUE, 0, 0);

                        make_mem (&database);
                        //

                        if (ex == NULL && cont == NULL) gg_report_error( "Missing 'exit' or 'continue'");
                        if (ex != NULL && cont != NULL) gg_report_error( "Cannot have both 'exit' or 'continue'");

                        get_db_config (database);
                        oprintf ("gg_get_config()->ctx.db->conn[gg_get_config()->ctx.db->ind_current_db].exit_on_error=%d;\n", ex != NULL ? 1 : 0);
                        continue;
                    }
                    // Checking for non-mandatory (6th param is 1) MUST be done first.
                    else if (((newI=recog_statement(line, i, "commit-transaction", &mtext, &msize, 1, &gg_is_inline)) != 0)
                     || ((newI1=recog_statement(line, i, "commit-transaction", &mtext, &msize, 0, &gg_is_inline)) != 0)) 
                    {
                        GG_GUARD
                        i = newI+newI1;
                        char *opt = find_keyword (mtext, GG_KEYOPTIONS, 1);
                        char *database = find_keyword (mtext, GG_KEYAT, 0);
                        char *oncont = find_keyword (mtext, GG_KEYONERRORCONTINUE, 1);
                        char *onexit = find_keyword (mtext, GG_KEYONERROREXIT, 1);
                        char *errt = find_keyword (mtext, GG_KEYERRORTEXT, 1);
                        char *err = find_keyword (mtext, GG_KEYERROR, 1);
                        carve_statement (&opt, "commit-transaction", GG_KEYOPTIONS, 0, 1);
                        carve_statement (&database, "commit-transaction", GG_KEYAT, 0, 1);
                        carve_statement (&oncont, "commit-transaction", GG_KEYONERRORCONTINUE, 0, 0);
                        carve_statement (&onexit, "commit-transaction", GG_KEYONERROREXIT, 0, 0);
                        carve_statement (&errt, "commit-transaction", GG_KEYERRORTEXT, 0, 1);
                        carve_statement (&err, "commit-transaction", GG_KEYERROR, 0, 1);
                        carve_stmt_obj (&mtext, false, false);
                        make_mem (&opt);
                        // database is not a string constant or variable, though it's stringified internally, so no make_mem
                        define_statement (&errt, GG_DEFSTRING, false); // allocated with strdup in db.c, exact length
                        define_statement (&err, GG_DEFSTRING, false); // allocated with strdup in db.c, exact length
                        //
                        check_var (&opt, GG_DEFSTRING);
                        //
                        get_db_config (database);
                        //erract is GG_ON_ERROR_CONTINUE/EXIT for statement-specific on-error continue/exit or GG_OKAY if db-level on-error is in effect
                        oprintf("gg_commit (\"%s\", %s, %s%s%s, %s%s%s);\n", opt==NULL?"":opt, oncont!=NULL?"GG_ON_ERROR_CONTINUE":(onexit!=NULL?"GG_ON_ERROR_EXIT":"GG_OKAY"),
                            err!=NULL?"&(":"", err !=NULL?err:"NULL", err!=NULL?")":"", 
                            errt!=NULL?"&(":"", errt !=NULL?errt:"NULL", errt!=NULL?")":""
                            );
                        continue;
                    }
                    // Checking for non-mandatory (6th param is 1) MUST be done first.
                    else if (((newI=recog_statement(line, i, "begin-transaction", &mtext, &msize, 1, &gg_is_inline)) != 0) 
                        || ((newI1=recog_statement(line, i, "begin-transaction", &mtext, &msize, 0, &gg_is_inline)) != 0))
                    {
                        GG_GUARD
                        i = newI+newI1;
                        char *database = find_keyword (mtext, GG_KEYAT, 0);
                        char *opt = find_keyword (mtext, GG_KEYOPTIONS, 1);
                        char *oncont = find_keyword (mtext, GG_KEYONERRORCONTINUE, 1);
                        char *onexit = find_keyword (mtext, GG_KEYONERROREXIT, 1);
                        char *errt = find_keyword (mtext, GG_KEYERRORTEXT, 1);
                        char *err = find_keyword (mtext, GG_KEYERROR, 1);
                        carve_statement (&opt, "begin-transaction", GG_KEYOPTIONS, 0, 1);
                        carve_statement (&database, "begin-transaction", GG_KEYAT, 0, 1);
                        carve_statement (&oncont, "begin-transaction", GG_KEYONERRORCONTINUE, 0, 0);
                        carve_statement (&onexit, "begin-transaction", GG_KEYONERROREXIT, 0, 0);
                        carve_statement (&errt, "begin-transaction", GG_KEYERRORTEXT, 0, 1);
                        carve_statement (&err, "begin-transaction", GG_KEYERROR, 0, 1);
                        carve_stmt_obj (&mtext, false, false);
                        make_mem (&opt);
                        // database is not a string constant or variable, though it's stringified internally, so no make_mem
                        define_statement (&errt, GG_DEFSTRING, false); // allocated with strdup in db.c, exact length
                        define_statement (&err, GG_DEFSTRING, false); // allocated with strdup in db.c, exact length
                        //
                        check_var (&opt, GG_DEFSTRING);
                        //
                        get_db_config (database);
                        //erract is GG_ON_ERROR_CONTINUE/EXIT for statement-specific on-error continue/exit or GG_OKAY if db-level on-error is in effect
                        oprintf("gg_begin_transaction (\"%s\", %s, %s%s%s, %s%s%s);\n", opt==NULL?"":opt, oncont!=NULL?"GG_ON_ERROR_CONTINUE":(onexit!=NULL?"GG_ON_ERROR_EXIT":"GG_OKAY"),
                            err!=NULL?"&(":"", err !=NULL?err:"NULL", err!=NULL?")":"", 
                            errt!=NULL?"&(":"", errt !=NULL?errt:"NULL", errt!=NULL?")":""
                            );
                        continue;
                    }
                    // Checking for non-mandatory (6th param is 1) MUST be done first.
                    else if (((newI1=recog_statement(line, i, "current-row", &mtext, &msize, 1, &gg_is_inline)) != 0)  // this is query row (number)
                         || (newI=recog_statement(line, i, "current-row", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI+newI1;
                        if (query_id == -1) gg_report_error( "Query usage attempted, but no query present and active");

                        if (gen_ctx->qry[query_id].returns_tuple == 0)
                        {
                            gg_report_error( "current-row cannot be used on query [%s] because it does not output any columns", gen_ctx->qry[query_id].name);
                        }
                        char *to = NULL;
                        if (newI != 0)
                        {
                            to = find_keyword (mtext, GG_KEYTO, 1);
                            carve_statement (&to, "current-row", GG_KEYTO, 1, 1);
                            define_statement (&to, GG_DEFNUMBER, false);
                        }

                        if (to != NULL)
                        {
                            // this happens right in the run-time, as gg_printf_noenc is going on
                            oprintf("%s = _gg_iter_%s+1;\n", to, gen_ctx->qry[query_id].name);
                        }
                        else
                        {
                            oprintf("gg_printf (false, GG_NOENC, \"%%ld\", _gg_iter_%s+1);\n", gen_ctx->qry[query_id].name); 
                        }
                        
                        continue;
                    }
                    else if (((newI4=recog_statement(line, i, "run-prepared-query", &mtext, &msize, 0, &gg_is_inline)) != 0)
                        || ((newI8=recog_statement(line, i, "run-query", &mtext, &msize, 0, &gg_is_inline)) != 0))
                    {
                        GG_GUARD
                        i = newI8+newI4;

                        //
                        // these are keywords needed to perform syntax highlighting, do NOT remove this comment!
                        // GG_KEYNOLOOP GG_KEYAFFECTEDROWS GG_KEYROWCOUNT GG_KEYERROR GG_KEYERRORTEXT GG_KEYONERRORCONTINUE GG_KEYONERROREXIT GG_KEYURLENCODE GG_KEYWEBENCODE GG_KEYNOENCODE
                        //

                        char is_prep = ((newI4 !=0) ? 1:0);

                        gg_db_parse vp;
                        memset  (&vp, 0, sizeof(gg_db_parse)); // all are NULL now
                        bool is_colon_subst = false;

                        //
                        // find keyword must be first
                        //

                        // run-query 
                        vp.on_error_cont = find_keyword (mtext, GG_KEYONERRORCONTINUE, 1);
                        vp.on_error_exit = find_keyword (mtext, GG_KEYONERROREXIT, 1);
                        vp.with_output = find_keyword (mtext, GG_KEYOUTPUT, 1);
                        vp.colon = find_keyword (mtext, GG_KEYCOLON, 0);
                        char *colon_subst = find_keyword (mtext, GG_KEYINPUT, 1);
                        if (vp.colon != NULL)
                        {
                            if (colon_subst != NULL) gg_report_error("Cannot use both ':' and input clause, use one or the other");
                        }
                        else
                        {
                            if (colon_subst != NULL) 
                            {
                                vp.colon = colon_subst;
                                is_colon_subst = true;
                            }
                        }
                        vp.eq = find_keyword (mtext, GG_KEYEQUALSHORT, 0);
                        vp.at = find_keyword (mtext, GG_KEYAT, 0);
                        vp.noloop = find_keyword (mtext, GG_KEYNOLOOP, 0);

                        // Stated as char* (the 4 below) so getvim for coloring will pick them up, expects char *....
                        char *err = find_keyword (mtext, GG_KEYERROR, 1);
                        char *errtext = find_keyword (mtext, GG_KEYERRORTEXT, 1);
                        char *arows = find_keyword (mtext, GG_KEYAFFECTEDROWS, 1);
                        char *rcount = find_keyword (mtext, GG_KEYROWCOUNT, 1);
                        // this commented out is just so getvim will color output - DO NOT REMOVE
                        //char *out = find_keyword (mtext, GG_KEYOUTPUT, 1);
                        //


                        //
                        // Carving must be after find_keyword
                        //

                        // carve for run-query, match above defines
                        carve_statement (&(vp.on_error_cont), "run-query", GG_KEYONERRORCONTINUE, 0, 0);
                        carve_statement (&(vp.on_error_exit), "run-query", GG_KEYONERROREXIT, 0, 0);
                        carve_statement (&(vp.with_output), "run-query", GG_KEYOUTPUT, 0, 1);
                        carve_statement (&(vp.colon), "run-query", is_colon_subst ? GG_KEYINPUT : GG_KEYCOLON, 0, 1);
                        carve_statement (&(vp.eq), "run-query", GG_KEYEQUALSHORT, 1, 1);
                        carve_statement (&(vp.at), "run-query", GG_KEYAT, 0, 1);
                        carve_statement (&(vp.noloop), "run-query", GG_KEYNOLOOP, 0, 0);
                        carve_statement (&err, "run-query", GG_KEYERROR, 0, 1);
                        carve_statement (&errtext, "run-query", GG_KEYERRORTEXT, 0, 1);
                        carve_statement (&arows, "run-query", GG_KEYAFFECTEDROWS, 0, 1);
                        carve_statement (&rcount, "run-query", GG_KEYROWCOUNT, 0, 1);
                        carve_stmt_obj (&mtext, false, false);

                        make_mem (&(vp.eq));
                        //vp.at is not a string (we stringify it, but never subject to string processing), so no make_mem

                        //
                        // Define must be after carving
                        //

                        // run-query has output vars that can be defined
                        define_statement (&err, GG_DEFSTRING, false); // allocated with strdup in db.c, exact length
                        define_statement (&errtext, GG_DEFSTRING, false); // allocated with strdup in db.c, exact length
                        define_statement (&arows, GG_DEFNUMBER, false);
                        define_statement (&rcount, GG_DEFNUMBER, false);
                        //this define_statement MUST stay for coloring
                        //define_statement (&out, GG_DEFSTRING, false);

                        // assign back to struct members
                        vp.err = err;
                        vp.errtext = errtext;
                        vp.arows = arows;
                        vp.rcount = rcount;

                        //
                        check_var (&(vp.eq), GG_DEFSTRING);

                        // check on-error: can be only continue or exit
                        on_error_act (vp.on_error_cont, vp.on_error_exit, &(vp.on_error_action));

                        get_db_config (vp.at);

                        // now @db has been cutoff with 0 char  and
                        // also gg_get_config()->ctx.db->conn[gg_get_config()->ctx.db->ind_current_db].db_name is now correct

                        process_query (gen_ctx, 0, 1, is_prep, &vp);

                        continue;
                    }

                    // web-output line
                    else if (!memcmp(line+i, "@",1) || !memcmp(line+i, "!",1))
                    {
                        if (within_inline == 1)
                        {
                            gg_report_error("Cannot use output statements (such as @ or !) within an inline statement (<< ... >>)");
                        }
                        oprintf("//%s\n", line+i); // there can never be \n in line, so this should work
                        // @, !  is a synonym for an output line.  ! is verbatim, @ respects << ... >>
                        usedGOLF = 0; // reset usedGOLF because "@/!" isn't actual directive, it only
                                    // says what follows is just printed out
                        if (line[i+1] == 0)
                        {
                            // if empty, print empty line, and continue
                            oprintf("\n");

                        }
                        oprintf("gg_puts (GG_NOENC, \""); // open the text line for free-text unencoded output
                        print_mode = 1;
                        if (line[i] == '!') is_verbatim = 1;
                        continue;
                    }
                    // Note: if 1 in 5th param in recog_statement, it means nothing afterwards, it must always be first!
                    else if ((newI2=recog_statement(line, i, "p-source-line", &mtext, &msize, 1, &gg_is_inline)) != 0 ||
                        (newI4=recog_statement(line, i, "p-source-file", &mtext, &msize, 1, &gg_is_inline)) != 0 ||
                        (newI3=recog_statement(line, i, "p-source-line", &mtext, &msize, 0, &gg_is_inline)) != 0 ||
                        (newI=recog_statement(line, i, "p-out", &mtext, &msize, 0, &gg_is_inline)) != 0 ||
                        (newI5=recog_statement(line, i, "p-source-file", &mtext, &msize, 0, &gg_is_inline)) != 0 ||
                        (newI6=recog_statement(line, i, "p-num", &mtext, &msize, 0, &gg_is_inline)) != 0 ||
                        (newI7=recog_statement(line, i, "p-url", &mtext, &msize, 0, &gg_is_inline)) != 0 ||
                        (newI8=recog_statement(line, i, "p-web", &mtext, &msize, 0, &gg_is_inline)) != 0)
                    {
                        GG_GUARD
                        i = newI+newI2+newI3+newI4+newI5+newI6+newI7+newI8;
                        gg_report_warning ("p-* family of statements has been deprecated. Use print-out instead");
                        // GG_KEYLENGTH GG_KEYNEWLINE GG_KEYSOURCEFILE GG_KEYSOURCELINE GG_KEYWEBENCODE GG_KEYURLENCODE
                        if (newI != 0) do_print (mtext, 0);
                        else if (newI2 != 0 || newI3 != 0) do_print (mtext, GG_P_SOURCELINE);
                        else if (newI4 != 0 || newI5 != 0) do_print (mtext, GG_P_SOURCEFILE);
                        else if (newI6 != 0) do_print (mtext, GG_P_NUM);
                        else if (newI7 != 0) do_print (mtext, GG_P_URL);
                        else if (newI8 != 0) do_print (mtext, GG_P_WEB);
                        continue;
                    }
                    else if ((newI1=recog_statement(line, i, "pf-web", &mtext, &msize, 0, &gg_is_inline)) != 0 ||
                        (newI2=recog_statement(line, i, "pf-url", &mtext, &msize, 0, &gg_is_inline)) != 0 ||
                        (newI=recog_statement(line, i, "pf-out", &mtext, &msize, 0, &gg_is_inline)) != 0)
                    {
                        GG_GUARD
                        i = newI+newI2+newI1;
                        // GG_KEYCOMMA GG_KEYTOERROR GG_KEYTO GG_KEYWEBENCODE GG_KEYURLENCODE
                        gg_report_warning ("pf-* family of statements has been deprecated. Use print-format instead");
                        if (newI1 != 0) do_printf (mtext, false, false, false, gen_ctx, GG_P_WEB);
                        else if (newI2 != 0) do_printf (mtext, false, false, false, gen_ctx, GG_P_URL);
                        else if (newI != 0) do_printf (mtext, false, false, false, gen_ctx, 0);
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "print-path", &mtext, &msize, 0, &gg_is_inline)) != 0 ||
                        (newI1=recog_statement(line, i, "p-path", &mtext, &msize, 0, &gg_is_inline)))
                    {
                        GG_GUARD
                        i = newI+newI1;
                        char *nl = find_keyword (mtext, GG_KEYNEWLINE, 1); // can be used with any other clause
                        check_sub (mtext); // this must be BEFORE carve_stmt_obj as it may create temp variable which is just some generated var name
                                           // it trims mtext, but we're about to do that anyway
                        carve_statement (&nl, "print-out", GG_KEYNEWLINE, 0, 0);
                        carve_stmt_obj (&mtext, true, true);

                        // mtext is mandatory request path
                        make_mem(&mtext);
                        check_var (&mtext, GG_DEFSTRING);

                        oprintf("gg_puts (GG_WEB, gg_app_path, gg_app_path_len, false);\n"); // optimized to compute strlen at compile time
                        oprintf("gg_puts (GG_WEB, %s, gg_mem_get_len(gg_mem_get_id(%s)), false);\n", mtext, mtext); // optimized to compute strlen at compile time
                        if (nl != NULL) oprintf("gg_puts (GG_NOENC, \"\\n\", 1, false);\n"); // output 1 byte non-alloc'd

                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "print-out", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;
                        // GG_KEYLENGTH GG_KEYNEWLINE GG_KEYSOURCELINE GG_KEYSOURCEFILE GG_KEYWEBENCODE GG_KEYURLENCODE
                        do_print (mtext, 0);
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "print-format", &mtext, &msize, 0, &gg_is_inline)) != 0 ||
                            (newI3=recog_statement(line, i, "report-error", &mtext, &msize, 0, &gg_is_inline)) != 0 ||
                            (newI4=recog_statement(line, i, "trace-run", &mtext, &msize, 1, &gg_is_inline)) != 0 ||
                            (newI5=recog_statement(line, i, "trace-run", &mtext, &msize, 0, &gg_is_inline)) != 0 
                        )  
                    {
                        GG_GUARD
                        i = newI+newI3+newI4+newI5;
                        bool rep_error;
                        bool trace_none=false;
                        bool trace=false;
                        // GG_KEYCOMMA GG_KEYTOERROR GG_KEYTO GG_KEYWEBENCODE GG_KEYURLENCODE
                        if (newI3 != 0) rep_error = true; else rep_error = false;
                        if (newI4 != 0) trace_none=true;
                        if (newI5 != 0) trace=true;
                        do_printf (mtext, rep_error, trace_none, trace, gen_ctx, 0);
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "exit-status", &mtext, &msize, 0, &gg_is_inline)) != 0 )
                    {
                        GG_GUARD
                        i = newI;
                        oprintf("gg_get_config ()->ctx.req->ec=(%s);\n", mtext);

                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "silent-header", &mtext, &msize, 1, &gg_is_inline)) != 0 ) 
                    {
                        GG_GUARD
                        i = newI;
                        oprintf("gg_get_config ()->ctx.req->silent=1;\n");

                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "out-header", &mtext, &msize, 0, &gg_is_inline)) != 0 ) 
                    {
                        GG_GUARD
                        i = newI;
                        char *use_default = find_keyword (mtext, GG_KEYDEFAULT, 1);
                        char *use = find_keyword (mtext, GG_KEYUSE, 1);
                        carve_statement (&use, "out-header", GG_KEYUSE, 0, 1);
                        carve_statement (&use_default, "out-header", GG_KEYDEFAULT, 0, 0);

                        if (use == NULL && use_default == NULL) gg_report_error( "Must use either 'use' or 'default' to specify header in out-header");
                        if (use != NULL && use_default != NULL) gg_report_error( "Cannot use both 'use' or 'default' in out-header");
                        if (use != NULL) 
                        {
                            //GG_HEADER_PAGE because output from a page isn't cached, it's dynamic
                            char *temp_header = make_default_header(GG_HEADER_PAGE, http_header_count, 0);
                            // GG_KEY* must be specified, even as a header, for getvim to pick up keywords for highlighting!
                            process_http_header ("out-header", use, temp_header, http_header_count, 0, NULL, NULL); // GG_KEYCONTENTTYPE GG_KEYDOWNLOAD GG_KEYETAG GG_KEYFILENAME GG_KEYCACHECONTROL GG_KEYNOCACHE GG_KEYSTATUSID GG_KEYSTATUSTEXT GG_KEYCUSTOM
                            oprintf("gg_get_config ()->ctx.req->header=&(%s);\n", temp_header);
                            http_header_count++;
                        }
                        oprintf("gg_output_http_header(gg_get_config ()->ctx.req);\n");

                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "exec-program", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;
                        

                        // Get keywords (if used)
                        char *program_args = find_keyword (mtext, GG_KEYARGS, 1);
                        char *program_status = find_keyword (mtext, GG_KEYSTATUS, 1);
                        char *program_error = find_keyword (mtext, GG_KEYERROR, 1);
                        char *program_output = find_keyword (mtext, GG_KEYOUTPUT, 1);
                        char *program_input = find_keyword (mtext, GG_KEYINPUT, 1);
                        char *program_input_length = find_keyword (mtext, GG_KEYINPUTLENGTH, 1);
                        char *out_file = find_keyword (mtext, GG_KEYOUTPUTFILE, 1);
                        char *err_file = find_keyword (mtext, GG_KEYERRORFILE, 1);
                        char *in_file = find_keyword (mtext, GG_KEYINPUTFILE, 1);

                        carve_statement (&program_args, "exec-program", GG_KEYARGS, 0, 1);
                        carve_statement (&program_status, "exec-program", GG_KEYSTATUS, 0, 1);
                        carve_statement (&program_output, "exec-program", GG_KEYOUTPUT, 0, 1);
                        carve_statement (&program_error, "exec-program", GG_KEYERROR, 0, 1);
                        carve_statement (&program_input, "exec-program", GG_KEYINPUT, 0, 1);
                        carve_statement (&program_input_length, "exec-program", GG_KEYINPUTLENGTH, 0, 1);
                        carve_statement (&out_file, "exec-program", GG_KEYOUTPUTFILE, 0, 1);
                        carve_statement (&err_file, "exec-program", GG_KEYERRORFILE, 0, 1);
                        carve_statement (&in_file, "exec-program", GG_KEYINPUTFILE, 0, 1);
                        carve_stmt_obj (&mtext, true, true);



                        define_statement (&program_output, GG_DEFSTRING, false); // exact length set in read_child in golfrt.c
                        define_statement (&program_error, GG_DEFSTRING, false); // exact length set in read_child in golfrt.c
                        define_statement (&program_status, GG_DEFNUMBER, false);

                        make_mem(&program_input);
                        make_mem(&out_file);
                        make_mem(&err_file);
                        make_mem(&in_file);

                        //
                        check_var (&mtext, GG_DEFSTRING);
                        check_var (&program_input, GG_DEFSTRING);
                        check_var (&program_input_length, GG_DEFNUMBER);
                        check_var (&out_file, GG_DEFSTRING);
                        check_var (&err_file, GG_DEFSTRING);
                        check_var (&in_file, GG_DEFSTRING);

                        // cannot have output-* and output (to string) at the same time
                        if (out_file != NULL && program_output != NULL) gg_report_error( "Specify either output-file or output in exec-program statement");
                        if (err_file != NULL && program_error != NULL) gg_report_error( "Specify either error-file or error in exec-program statement");
                        if (in_file != NULL && program_input != NULL) gg_report_error( "Specify either input-file or input in exec-program statement");
                        if (program_input == NULL && program_input_length != NULL) gg_report_error( "Cannot use input-length without 'input' in exec-program statement");

                        oprintf ("gg_num _gg_prg_status%ld = 0;\n", total_exec_programs);
                        oprintf("GG_UNUSED (_gg_prg_status%ld);\n", total_exec_programs); // if status not used by the developer
                        oprintf ("FILE *_gg_prg_out_file%ld = NULL;\nGG_UNUSED(_gg_prg_out_file%ld);\n", total_exec_programs, total_exec_programs);
                        oprintf ("FILE *_gg_prg_err_file%ld = NULL;\nGG_UNUSED(_gg_prg_err_file%ld);\n", total_exec_programs, total_exec_programs);
                        if (err_file != NULL)
                        {
                            // this is error-file. 
                            oprintf ("_gg_prg_err_file%ld = gg_fopen ((%s), \"w+\");\n", total_exec_programs, err_file);
                            // we set status to -8 if cannot open for writing
                            //GG_ERR is set in gg_fopen
                            oprintf ("if (_gg_prg_err_file%ld == NULL) {_gg_prg_status%ld=GG_ERR_WRITE;} else { \n", total_exec_programs, total_exec_programs);
                        }
                        if (out_file != NULL)
                        {
                            // this is output-file. We open file for writing, and we close it after we're done
                            oprintf ("_gg_prg_out_file%ld = gg_fopen ((%s), \"w+\");\n", total_exec_programs, out_file);
                            // we set status to -8 if cannot open for writing
                            //GG_ERR is set in gg_fopen
                            oprintf ("if (_gg_prg_out_file%ld == NULL) {_gg_prg_status%ld=GG_ERR_WRITE;} else { \n", total_exec_programs, total_exec_programs);
                        }

                        oprintf ("FILE *_gg_prg_in_file%ld = NULL;\nGG_UNUSED(_gg_prg_in_file%ld);\n", total_exec_programs, total_exec_programs);
                        if (in_file != NULL)
                        {
                            // this is input-file. We open it for reading, and close after we're done
                            oprintf ("_gg_prg_in_file%ld = gg_fopen ((%s), \"r\");\n", total_exec_programs, in_file);
                            // for status, we set it to -9 if cannot read
                            //GG_ERR is set in gg_fopen
                            oprintf ("if (_gg_prg_in_file%ld == NULL) {_gg_prg_status%ld=GG_ERR_READ;} else {\n", total_exec_programs, total_exec_programs);
                        }



                        // store exec-program args arguments in an array
                        char prog_var[100];
                        snprintf (prog_var, sizeof(prog_var), "_gg_prg_arr%ld", total_exec_programs);
                        gg_num exec_inputs = outargs(program_args, prog_var, "char *", 1, 0, GG_DEFSTRING, GG_DEFSTRING);

                        // generate run-time call to execute program
                        // exec_inputs is always at least 1 (to account for args[0] being program name)
                        // this is output (i.e. string output)
                        oprintf ("_gg_st = gg_exec_program(%s, _gg_prg_arr%ld, %ld, _gg_prg_in_file%ld, &(_gg_prg_out_file%ld), &(_gg_prg_err_file%ld), %s, %s, %s%s%s, %s%s%s);\n",
                        mtext, 
                        total_exec_programs, 
                        exec_inputs, 
                        total_exec_programs,
                        total_exec_programs,
                        total_exec_programs,
                        program_input==NULL ? "NULL":program_input, 
                        program_input_length==NULL ?"0":program_input_length, 
                        program_output==NULL ? "":"&(", 
                        program_output==NULL ? "NULL":program_output, 
                        program_output==NULL ? "":")", 
                        program_error==NULL ? "":"&(", 
                        program_error==NULL ? "NULL":program_error, 
                        program_error==NULL ? "":")");
                        if (program_status!=NULL) oprintf("%s=_gg_st;\n", program_status);
                        else oprintf ("if (_gg_st != GG_OKAY) gg_report_error (\"%s\", \"%s\", (gg_num)%ld, _gg_st);\n", GG_STMT_FAIL_CHECK, undecorate (src_file_name+GG_BLD_DIR_OFFSET), lnum);
                        if (err_file != NULL) oprintf ("}\n");
                        if (out_file != NULL) oprintf ("}\n");
                        if (in_file != NULL) oprintf ("}\n");

                        total_exec_programs++; // advance exec-program counter so generating specific variables is unique
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "send-file", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;

                        //
                        // Look for each option and collect relevant info
                        //
                        char *header = find_keyword (mtext, GG_KEYHEADERS, 1);
                        carve_statement (&header, "send-file", GG_KEYHEADERS, 0, 1);
                        carve_stmt_obj (&mtext, true, true);
                        //
                        check_var (&mtext, GG_DEFSTRING);

                        // if there's no header, by default:
                        char *temp_header = make_default_header(GG_HEADER_FILE, http_header_count, 0);
                        oprintf ("(%s).file_name = basename(%s);\n", temp_header, mtext);

                        // specify keywords used in function for syntax highlighting to work properly
                        process_http_header ("send-file", header, temp_header, http_header_count, 0, NULL, NULL); // GG_KEYCONTENTTYPE GG_KEYDOWNLOAD GG_KEYETAG GG_KEYFILENAME GG_KEYCACHECONTROL GG_KEYNOCACHE GG_KEYSTATUSID GG_KEYSTATUSTEXT GG_KEYCUSTOM
                        http_header_count++;


                        oprintf ("gg_out_file(%s, &(%s));\n", mtext, temp_header);


                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "lock-file", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;
                        // Example:
                        // lock-file "/home/golf/files" id [define] x status [define] st
                        // later
                        // unlock-file x
                        char *id = find_keyword (mtext, GG_KEYID, 1);
                        char *status = find_keyword (mtext, GG_KEYSTATUS, 1);

                        carve_statement (&id, "lock-file", GG_KEYID, 1, 1);
                        carve_statement (&status, "lock-file", GG_KEYSTATUS, 0, 1);
                        carve_stmt_obj (&mtext, true, true);

                        define_statement (&id, GG_DEFNUMBER, false);
                        define_statement (&status, GG_DEFNUMBER, false);
                        //
                        check_var (&mtext, GG_DEFSTRING);

                        oprintf ("_gg_st=gg_lockfile (%s, &(%s));\n", mtext, id);
                        if (status != NULL) oprintf ("%s=_gg_st;\n", status);
                        else oprintf("if (_gg_st != GG_OKAY) gg_report_error (\"%s\", \"%s\", (gg_num)%ld, _gg_st);}\n", GG_STMT_FAIL_CHECK, undecorate (src_file_name+GG_BLD_DIR_OFFSET), lnum);

                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "unlock-file", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;
                        // Example:
                        // unlock-file id x
                        char *id = find_keyword (mtext, GG_KEYID, 1);

                        carve_statement (&id, "lock-file", GG_KEYID, 1, 1);

                        oprintf ("close(%s);\n", id);


                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "random-string", &mtext, &msize, 0, &gg_is_inline)) != 0)
                    {
                        GG_GUARD
                        i = newI;
                        char *to = find_keyword (mtext, GG_KEYTO, 1);
                        char *length = find_keyword (mtext, GG_KEYLENGTH, 1);
                        char *number = find_keyword (mtext, GG_KEYNUMBER, 1);
                        char *binary = find_keyword (mtext, GG_KEYBINARY, 1);

                        carve_statement (&to, "random-string", GG_KEYTO, 1, 1);
                        carve_statement (&length, "random-string", GG_KEYLENGTH, 0, 1);
                        carve_statement (&number, "random-string", GG_KEYNUMBER, 0, 0);
                        carve_statement (&binary, "random-string", GG_KEYBINARY, 0, 0);
                        carve_stmt_obj (&mtext, false, false);

                        char type;
                        if (number != NULL) type = GG_RANDOM_NUM;
                        else if (binary != NULL) type = GG_RANDOM_BIN;
                        else type = GG_RANDOM_STR;

                        if (number != NULL && binary != NULL) gg_report_error( "cannot use both 'number' and 'binary' in random-string statement");

                        define_statement (&to, GG_DEFSTRING, false); // exact length set in gg_make_random
                        //
                        check_var (&length, GG_DEFNUMBER);

                        if (length == NULL)
                        {
                            length="20";
                        }

                        oprintf ("gg_make_random (&(%s), (%s)+1, %d, false);\n", to, length, type);


                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "random-crypto", &mtext, &msize, 0, &gg_is_inline)) != 0)
                    {
                        GG_GUARD
                        i = newI;
                        char *to = find_keyword (mtext, GG_KEYTO, 1);
                        char *length = find_keyword (mtext, GG_KEYLENGTH, 1);

                        carve_statement (&to, "random-string", GG_KEYTO, 1, 1);
                        carve_statement (&length, "random-string", GG_KEYLENGTH, 0, 1);
                        carve_stmt_obj (&mtext, false, false);

                        define_statement (&to, GG_DEFSTRING, false); // exact length set in gg_make_random
                        //
                        check_var (&length, GG_DEFNUMBER);

                        if (length == NULL)
                        {
                            length="20";
                        }

                        oprintf ("gg_make_random (&(%s), (%s)+1, GG_RANDOM_BIN, true);\n", to, length);


                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "upper-string", &mtext, &msize, 0, &gg_is_inline)) != 0)
                    {
                        GG_GUARD
                        i = newI;
                        // Example:
                        // upper-string str

                        oprintf ("gg_upper (%s);\n", mtext);


                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "lower-string", &mtext, &msize, 0, &gg_is_inline)) != 0)
                    {
                        GG_GUARD
                        i = newI;
                        // Example:
                        // lower-string str

                        oprintf ("gg_lower (%s);\n", mtext);


                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "string-length", &mtext, &msize, 0, &gg_is_inline)) != 0)
                    {
                        GG_GUARD
                        i = newI;

                        char *to = find_keyword (mtext, GG_KEYTO, 1);

                        carve_statement (&to, "string-length", GG_KEYTO, 0, 1);
                        carve_stmt_obj (&mtext, true, true);

                        define_statement (&to, GG_DEFNUMBER, false);
                        //
                        check_var (&mtext, GG_DEFSTRING);

                        if (to != NULL) oprintf ("%s=(gg_num)gg_mem_get_len (gg_mem_get_id(%s));\n", to, mtext);
                        else 
                        {
                            oprintf("{gg_num _gg_slen=gg_mem_get_len(gg_mem_get_id(%s));\n", mtext);
                            do_numstr (NULL, "_gg_slen", NULL);
                            oprintf("}\n");
                        }

                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "count-substring", &mtext, &msize, 0, &gg_is_inline)) != 0)
                    {
                        GG_GUARD
                        i = newI;

                        char *to = find_keyword (mtext, GG_KEYTO, 1);
                        char *in = find_keyword (mtext, GG_KEYIN, 1);
                        char *case_insensitive = find_keyword (mtext, GG_KEYCASEINSENSITIVE, 1);

                        carve_statement (&to, "count-substring", GG_KEYTO, 1, 1);
                        carve_statement (&in, "count-substring", GG_KEYIN, 1, 1);
                        carve_statement (&case_insensitive, "count-substring", GG_KEYCASEINSENSITIVE, 0, 2);
                        carve_stmt_obj (&mtext, true, true);
                        make_mem (&in);

                        char *case_insensitivec = opt_clause(case_insensitive, "0", "1", GG_DEFNUMBER);

                        define_statement (&to, GG_DEFNUMBER, false);
                        //
                        check_var (&mtext, GG_DEFSTRING);
                        check_var (&in, GG_DEFSTRING);
                        check_var (&case_insensitivec, GG_DEFNUMBER);

                        oprintf ("%s=gg_count_substring (%s, %s, 0, %s);\n", to, in, mtext, case_insensitivec);
                        gg_free(case_insensitivec);


                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "derive-key", &mtext, &msize, 0, &gg_is_inline)) != 0)
                    {
                        GG_GUARD
                        i = newI;

                        char *from = find_keyword (mtext, GG_KEYFROM, 1);
                        char *fromlen = find_keyword (mtext, GG_KEYFROMLENGTH, 1);
                        char *digest = find_keyword (mtext, GG_KEYDIGEST, 1);
                        char *salt = find_keyword (mtext, GG_KEYSALT, 1);
                        char *salt_len = find_keyword (mtext, GG_KEYSALTLENGTH, 1);
                        char *iterations = find_keyword (mtext, GG_KEYITERATIONS, 1);
                        char *length = find_keyword (mtext, GG_KEYLENGTH, 1);
                        char *binary = find_keyword (mtext, GG_KEYBINARY, 1);

                        carve_statement (&length, "derive-key", GG_KEYLENGTH, 1, 1);
                        carve_statement (&from, "derive-key", GG_KEYFROM, 1, 1);
                        carve_statement (&fromlen, "derive-key", GG_KEYFROMLENGTH, 0, 1);
                        carve_statement (&digest, "derive-key", GG_KEYDIGEST, 0, 1);
                        carve_statement (&salt, "derive-key", GG_KEYSALT, 0, 1);
                        carve_statement (&salt_len, "derive-key" , GG_KEYSALTLENGTH, 0, 1);
                        carve_statement (&iterations, "derive-key", GG_KEYITERATIONS, 0, 1);
                        carve_statement (&binary, "derive-key", GG_KEYBINARY, 0, 2);
                        carve_stmt_obj (&mtext, true, true);

                        make_mem(&from);
                        make_mem(&digest);
                        make_mem(&salt);

                        define_statement (&mtext, GG_DEFSTRING, false); // exact length set in gg_derive_key
                        char *binaryc = opt_clause(binary, "true", "false", GG_DEFBOOL);
                        //
                        check_var (&length, GG_DEFNUMBER);
                        check_var (&from, GG_DEFSTRING);
                        check_var (&fromlen, GG_DEFNUMBER);
                        check_var (&digest, GG_DEFSTRING);
                        check_var (&salt, GG_DEFSTRING);
                        check_var (&salt_len, GG_DEFNUMBER);
                        check_var (&iterations, GG_DEFNUMBER);
                        check_var (&binaryc, GG_DEFBOOL);

                        oprintf ("%s=gg_derive_key( %s, %s, %s, %s, %s, %s, %s, %s );\n", mtext, from, fromlen==NULL?"-1":fromlen, digest==NULL?GG_DEF_DIGEST:digest, iterations==NULL?"-1":iterations, salt==NULL?"NULL":salt, salt_len==NULL?"0":salt_len, length, binaryc);
                        gg_free(binaryc);


                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "hmac-string", &mtext, &msize, 0, &gg_is_inline)) != 0)
                    {
                        GG_GUARD
                        i = newI;

                        char *key = find_keyword (mtext, GG_KEYKEY, 1);
                        char *to = find_keyword (mtext, GG_KEYTO, 1);
                        char *digest = find_keyword (mtext, GG_KEYDIGEST, 1);
                        char *binary = find_keyword (mtext, GG_KEYBINARY, 1);

                        carve_statement (&key, "hmac-string", GG_KEYKEY, 1, 1);
                        carve_statement (&to, "hmac-string", GG_KEYTO, 1, 1);
                        carve_statement (&digest, "hmac-string", GG_KEYDIGEST, 0, 1);
                        carve_statement (&binary, "hmac-string", GG_KEYBINARY, 0, 2);
                        carve_stmt_obj (&mtext, true, true);
                        make_mem (&digest);
                        make_mem (&key);

                        define_statement (&to, GG_DEFSTRING, false); // exact length set in gg_hash_data
                        char *binaryc = opt_clause(binary, "true", "false", GG_DEFBOOL);
                        //
                        check_var (&mtext, GG_DEFSTRING);
                        check_var (&key, GG_DEFSTRING);
                        check_var (&digest, GG_DEFSTRING);
                        check_var (&binaryc, GG_DEFBOOL);

                        oprintf ("%s=gg_hmac (%s, %s, %s, %s);\n", to, key, mtext, digest==NULL?GG_DEF_DIGEST:digest, binaryc);
                        gg_free(binaryc);



                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "hash-string", &mtext, &msize, 0, &gg_is_inline)) != 0)
                    {
                        GG_GUARD
                        i = newI;

                        char *to = find_keyword (mtext, GG_KEYTO, 1);
                        char *digest = find_keyword (mtext, GG_KEYDIGEST, 1);
                        char *binary = find_keyword (mtext, GG_KEYBINARY, 1);

                        carve_statement (&to, "hash-string", GG_KEYTO, 1, 1);
                        carve_statement (&digest, "hash-string", GG_KEYDIGEST, 0, 1);
                        carve_statement (&binary, "hash-string", GG_KEYBINARY, 0, 2);
                        carve_stmt_obj (&mtext, true, true);
                        make_mem (&digest);

                        define_statement (&to, GG_DEFSTRING, false); // exact length set in gg_hash_data
                        char *binaryc = opt_clause(binary, "true", "false", GG_DEFBOOL);
                        //
                        check_var (&mtext, GG_DEFSTRING);
                        check_var (&digest, GG_DEFSTRING);
                        check_var (&binaryc, GG_DEFBOOL);

                        oprintf ("%s=gg_hash_data (%s, %s, %s);\n", to, mtext, digest==NULL?GG_DEF_DIGEST:digest, binaryc);
                        gg_free(binaryc);



                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "decode-base64", &mtext, &msize, 0, &gg_is_inline)) != 0)
                    {
                        GG_GUARD
                        i = newI;

                        char *ilen = find_keyword (mtext, GG_KEYINPUTLENGTH, 1);
                        char *to = find_keyword (mtext, GG_KEYTO, 1);

                        carve_statement (&ilen, "decode-base64", GG_KEYINPUTLENGTH, 0, 1);
                        carve_statement (&to, "decode-base64", GG_KEYTO, 1, 1);
                        carve_stmt_obj (&mtext, true, true);

                        define_statement (&to, GG_DEFSTRING, false); // exact length set in gg_b64_decode
                        //
                        check_var (&mtext, GG_DEFSTRING);
                        check_var (&ilen, GG_DEFNUMBER);

                        oprintf ("gg_b64_decode ((char*)(%s), %s, &((%s)));\n", mtext, ilen==NULL?"-1":ilen,to);



                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "encode-base64", &mtext, &msize, 0, &gg_is_inline)) != 0)
                    {
                        GG_GUARD
                        i = newI;

                        char *ilen = find_keyword (mtext, GG_KEYINPUTLENGTH, 1);
                        char *to = find_keyword (mtext, GG_KEYTO, 1);

                        carve_statement (&ilen, "encode-base64", GG_KEYINPUTLENGTH, 0, 1);
                        carve_statement (&to, "encode-base64", GG_KEYTO, 1, 1);
                        carve_stmt_obj (&mtext, true, true);

                        define_statement (&to, GG_DEFSTRING, false); // exact length set in gg_b64_encode
                        //
                        check_var (&mtext, GG_DEFSTRING);
                        check_var (&ilen, GG_DEFNUMBER);

                        oprintf ("gg_b64_encode ((char*)(%s), %s, &(%s));\n", mtext, ilen==NULL?"-1":ilen,to);



                        continue;
                    }

                    else if ((newI=recog_statement(line, i, "encode-hex", &mtext, &msize, 0, &gg_is_inline)) != 0)
                    {
                        GG_GUARD
                        i = newI;

                        char *ilen = find_keyword (mtext, GG_KEYINPUTLENGTH, 1);
                        char *pref = find_keyword (mtext, GG_KEYPREFIX, 1);
                        char *to = find_keyword (mtext, GG_KEYTO, 1);

                        carve_statement (&ilen, "encode-hex", GG_KEYINPUTLENGTH, 0, 1);
                        carve_statement (&pref, "encode-hex", GG_KEYPREFIX, 0, 1);
                        carve_statement (&to, "encode-hex", GG_KEYTO, 1, 1);
                        carve_stmt_obj (&mtext, true, true);

                        make_mem(&pref);

                        define_statement (&to, GG_DEFSTRING, false); // exact length set in gg_bin2hex
                        //
                        check_var (&mtext, GG_DEFSTRING);
                        check_var (&ilen, GG_DEFNUMBER);
                        check_var (&pref, GG_DEFSTRING);

                        oprintf ("gg_bin2hex((char*)(%s), &(%s), %s, %s);\n", mtext, to, ilen==NULL?"-1":ilen, pref==NULL?"NULL":pref);

                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "decode-hex", &mtext, &msize, 0, &gg_is_inline)) != 0)
                    {
                        GG_GUARD
                        i = newI;

                        char *ilen = find_keyword (mtext, GG_KEYINPUTLENGTH, 1);
                        char *to = find_keyword (mtext, GG_KEYTO, 1);

                        carve_statement (&ilen, "decode-hex", GG_KEYINPUTLENGTH, 0, 1);
                        carve_statement (&to, "decode-hex", GG_KEYTO, 1, 1);
                        carve_stmt_obj (&mtext, true, true);

                        define_statement (&to, GG_DEFSTRING, false); // exact length set in gg_hex2bin
                        //
                        check_var (&mtext, GG_DEFSTRING);
                        check_var (&ilen, GG_DEFNUMBER);

                        oprintf ("gg_hex2bin ((char*)(%s), &(%s), %s);\n", mtext, to, ilen==NULL?"-1":ilen);

                        continue;
                    }

                    else if ((newI=recog_statement(line, i, "decode-url", &mtext, &msize, 0, &gg_is_inline)) != 0 ||
                     (newI1=recog_statement(line, i, "decode-web", &mtext, &msize, 0, &gg_is_inline)) != 0)
                    {
                        GG_GUARD
                        i = newI+newI1;

                        gg_num is_web=(newI1 != 0 ? 1:0);
                        char *inlength = find_keyword (mtext, GG_KEYINPUTLENGTH, 1);
                        char *status = NULL;
                        if (newI != 0) status = find_keyword (mtext, GG_KEYSTATUS, 1); // only for decode-url

                        carve_statement (&inlength, is_web==1?"decode-web":"decode-url", GG_KEYINPUTLENGTH, 0, 1);
                        if (newI != 0) carve_statement (&status, "decode-url", GG_KEYSTATUS, 0, 1);
                        carve_stmt_obj (&mtext, true, true);
                        if (newI != 0) define_statement (&status, GG_DEFNUMBER, false);

                        //
                        check_var (&mtext, GG_DEFSTRING);
                        check_var (&inlength, GG_DEFNUMBER);

                        if (inlength == NULL) inlength="-1";

                        oprintf ("gg_decode (%s, %s, %s, true, &_gg_st);\n",is_web==1?"GG_WEB":"GG_URL", mtext, inlength);
                        if (newI != 0)
                        {
                            if (status != NULL) oprintf("%s=_gg_st;\n", status);
                            else oprintf("if (_gg_st != GG_OKAY) gg_report_error (\"%s\", \"%s\", (gg_num)%ld, _gg_st);\n", GG_STMT_FAIL_CHECK, undecorate (src_file_name+GG_BLD_DIR_OFFSET), lnum);
                        }


                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "encode-url", &mtext, &msize, 0, &gg_is_inline)) != 0 ||
                     (newI1=recog_statement(line, i, "encode-web", &mtext, &msize, 0, &gg_is_inline)) != 0)
                    {
                        GG_GUARD
                        i = newI+newI1;

                        gg_num is_web=(newI1 != 0 ? 1:0);
                        char *inlength = find_keyword (mtext, GG_KEYINPUTLENGTH, 1);
                        char *to = find_keyword (mtext, GG_KEYTO, 1);

                        carve_statement (&inlength, is_web==1?"encode-web":"encode-url", GG_KEYINPUTLENGTH, 0, 1);
                        carve_statement (&to, is_web==1?"encode-web":"encode-url", GG_KEYTO, 1, 1);
                        carve_stmt_obj (&mtext, true, true);

                        define_statement (&to, GG_DEFSTRING, false); // exact length set in golf encode
                        //
                        check_var (&mtext, GG_DEFSTRING);
                        check_var (&inlength, GG_DEFNUMBER);

                        if (inlength == NULL) inlength = "-1";

                        oprintf ("gg_encode (%s, %s, %s, &(%s), true);\n", is_web==1?"GG_WEB":"GG_URL", mtext, inlength, to);



                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "trim-string", &mtext, &msize, 0, &gg_is_inline)) != 0)
                    {
                        GG_GUARD
                        i = newI;
                        // Example:
                        // trim-string str 

                        carve_stmt_obj (&mtext, true, true);

                        //
                        check_var (&mtext, GG_DEFSTRING);

                        oprintf ("gg_num _gg_trim_len%ld=gg_mem_get_len (gg_mem_get_id(%s));\n", trim_count, mtext);
                        oprintf ("gg_trim (%s, &_gg_trim_len%ld, true);\n", mtext, trim_count);
                        trim_count++;


                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "encrypt-data", &mtext, &msize, 0, &gg_is_inline)) != 0 ||
                        (newI1=recog_statement(line, i, "decrypt-data", &mtext, &msize, 0, &gg_is_inline)) != 0)
                    {
                        GG_GUARD
                        i = newI+newI1;
                        bool enc = (newI!=0); // is it encrypt (otherwise decrypt)

                        // Example:
                        // encrypt-data data [input-length l] output-length [define] ol password p [salt s] to [define] res [binary]

                        char *password = find_keyword (mtext, GG_KEYPASSWORD, 1);
                        char *inlength = find_keyword (mtext, GG_KEYINPUTLENGTH, 1);
                        char *salt = find_keyword (mtext, GG_KEYSALT, 1);
                        char *salt_len = find_keyword (mtext, GG_KEYSALTLENGTH, 1);
                        char *iv = find_keyword (mtext, GG_KEYINITVECTOR, 1);
                        char *iterations = find_keyword (mtext, GG_KEYITERATIONS, 1);
                        char *to = find_keyword (mtext, GG_KEYTO, 1);
                        char *binary = find_keyword (mtext, GG_KEYBINARY, 1);
                        char *cipher = find_keyword (mtext, GG_KEYCIPHER, 1);
                        char *digest = find_keyword (mtext, GG_KEYDIGEST, 1);
                        char *cache = find_keyword (mtext, GG_KEYCACHE, 1);
                        char *ccache = find_keyword (mtext, GG_KEYCLEARCACHE, 1);

                        carve_statement (&password, enc?"encrypt-data":"decrypt-data", GG_KEYPASSWORD, 1, 1);
                        carve_statement (&inlength, enc?"encrypt-data":"decrypt-data" , GG_KEYINPUTLENGTH, 0, 1);
                        carve_statement (&iv, enc?"encrypt-data":"decrypt-data", GG_KEYINITVECTOR, 0, 1);
                        carve_statement (&salt, enc?"encrypt-data":"decrypt-data", GG_KEYSALT, 0, 1);
                        carve_statement (&salt_len, enc?"encrypt-data":"decrypt-data" , GG_KEYSALTLENGTH, 0, 1);
                        carve_statement (&iterations, enc?"encrypt-data":"decrypt-data", GG_KEYITERATIONS, 0, 1);
                        carve_statement (&to, enc?"encrypt-data":"decrypt-data" , GG_KEYTO, 1, 1);
                        carve_statement (&binary, enc?"encrypt-data":"decrypt-data", GG_KEYBINARY, 0, 2);
                        carve_statement (&cipher, enc?"encrypt-data":"decrypt-data", GG_KEYCIPHER, 0, 1);
                        carve_statement (&digest, enc?"encrypt-data":"decrypt-data", GG_KEYDIGEST, 0, 1);
                        carve_statement (&cache, enc?"encrypt-data":"decrypt-data", GG_KEYCACHE, 0, 0);
                        carve_statement (&ccache, enc?"encrypt-data":"decrypt-data", GG_KEYCLEARCACHE, 0, 1);
                        carve_stmt_obj (&mtext, true, true);

                        make_mem(&password);
                        make_mem(&salt);
                        make_mem(&iv);
                        make_mem(&cipher);
                        make_mem(&digest);

                        define_statement (&to, GG_DEFSTRING, false); // gg_encrypt sets exact length
                        char *binaryc = opt_clause(binary, "1", "0", GG_DEFNUMBER);
                        //
                        check_var (&mtext, GG_DEFSTRING);
                        check_var (&inlength, GG_DEFNUMBER);
                        check_var (&salt_len, GG_DEFNUMBER);
                        check_var (&iterations, GG_DEFNUMBER);
                        check_var (&iv, GG_DEFSTRING);
                        check_var (&password, GG_DEFSTRING);
                        check_var (&salt, GG_DEFSTRING);
                        check_var (&cipher, GG_DEFSTRING);
                        check_var (&digest, GG_DEFSTRING);
                        check_var (&binaryc, GG_DEFNUMBER);
                        check_var (&ccache, GG_DEFBOOL);

                        if (ccache != NULL && cache == NULL) gg_report_error( "clear-cache cannot be used without cache in encrypt/decrypt-data statement");

                        if (iv == NULL)
                        {
                            iv="NULL";
                            if (cache != NULL) gg_report_error( "cache cannot be used if init-vector is omitted in encrypt/decrypt-data statement");
                        } 
                        if (salt == NULL)
                        {
                            if (salt_len != NULL) gg_report_error( "cannot specify salt-len without salt in encrypt/decrypt-data statement");
                            salt="NULL";
                        }

                        char *to_crypt = mtext;
                        
                        oprintf ("%sEVP_CIPHER_CTX *_gg_e_ctx%ld = NULL;\n", cache!=NULL?"static ":"", encrypt_count);
                        if (ccache != NULL) 
                        {
                            // if clear-cache is true, then first check if there's existing encryption context, if so, clear it
                            // then set context to NULL so it's created again.
                            oprintf ("if ((%s)) {if (_gg_e_ctx%ld != NULL) EVP_CIPHER_CTX_free(_gg_e_ctx%ld); _gg_e_ctx%ld = NULL;}\n", ccache, encrypt_count, encrypt_count, encrypt_count);
                        }
                        oprintf ("if (_gg_e_ctx%ld == NULL) { _gg_e_ctx%ld = EVP_CIPHER_CTX_new(); gg_get_enc_key(%s, %s, %s, %s, %s?_gg_e_ctx%ld:NULL, %s?_gg_e_ctx%ld:NULL, %s, %s);}\n", encrypt_count, encrypt_count, password, salt, salt_len == NULL ? "0":salt_len, iterations==NULL?"-1":iterations, enc?"true":"false", encrypt_count, (!enc)?"true":"false", encrypt_count, cipher==NULL?GG_DEF_CIPHER:cipher, digest==NULL?GG_DEF_DIGEST:digest);
                        // regardless of whether input-length is used or not, we have it set
                        if (inlength != NULL) oprintf ("gg_num _gg_encrypt_count%ld = %s;\n", encrypt_count, inlength);
                        else oprintf ("gg_num _gg_encrypt_count%ld = -1;\n", encrypt_count);
                        oprintf ("%s = %s(_gg_e_ctx%ld, (unsigned char*)(%s), &_gg_encrypt_count%ld, %s, (unsigned char*)(%s));\n", to, enc?"gg_encrypt":"gg_decrypt", encrypt_count, to_crypt, encrypt_count, binaryc, iv==NULL?"NULL":iv);
                        if (cache == NULL) oprintf ("EVP_CIPHER_CTX_free(_gg_e_ctx%ld);\n", encrypt_count);
                        encrypt_count++;
                        gg_free(binaryc);


                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "stat-file", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;
                        char *path = find_keyword (mtext, GG_KEYPATH, 1);
                        char *basename = find_keyword (mtext, GG_KEYNAME, 1);
                        char *type = find_keyword (mtext, GG_KEYTYPE, 1);
                        char *size = find_keyword (mtext, GG_KEYSIZE, 1);
                        char *mode = find_keyword (mtext, GG_KEYMODE, 1);

                        carve_statement (&path, "stat-file", GG_KEYPATH, 0, 1);
                        carve_statement (&basename, "stat-file", GG_KEYNAME, 0, 1);
                        carve_statement (&type, "stat-file", GG_KEYTYPE, 0, 1);
                        carve_statement (&size, "stat-file", GG_KEYSIZE, 0, 1);
                        carve_statement (&mode, "stat-file", GG_KEYMODE, 0, 1);
                        carve_stmt_obj (&mtext, true, true);


                        gg_num tot_opt = (path!=NULL?1:0)+(basename!=NULL?1:0)+(type!=NULL?1:0)+(size!=NULL?1:0)+(mode!=NULL?1:0);
                        if (tot_opt == 0)
                        {
                            gg_report_error( "At least one clause must be in stat-file statement");
                        }
                        define_statement (&size, GG_DEFNUMBER, false);
                        define_statement (&type, GG_DEFNUMBER, false);
                        define_statement (&mode, GG_DEFNUMBER, false);
                        define_statement (&path, GG_DEFSTRING, false);
                        define_statement (&basename, GG_DEFSTRING, false);

                        check_var (&mtext, GG_DEFSTRING);

                        if (size != NULL || type != NULL || mode != NULL) 
                        {
                            oprintf ("gg_file_stat (%s, %s%s%s, %s%s%s, %s%s%s);\n", mtext, type==NULL?"":"&(",type==NULL?"NULL":type,type==NULL?"":")",  size==NULL?"":"&(",size==NULL?"NULL":size,size==NULL?"":")",   mode==NULL?"":"&(",mode==NULL?"NULL":mode,mode==NULL?"":")");
                        }
                        // basename and path must use strdup on argument because it may be altered
                        // also strdup the result because it may be overwritten by subsequent calls
                        if (basename != NULL) 
                        {
                            oprintf ("%s=gg_basename (%s);\n", basename, mtext);
                        }
                        if (path != NULL) 
                        {
                            oprintf ("%s=gg_realpath (%s);\n", path, mtext);
                        }


                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "utf-text", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;

                        char *len = find_keyword (mtext, GG_KEYLENGTH, 1);
                        char *status = find_keyword (mtext, GG_KEYSTATUS, 1);
                        char *errt = find_keyword (mtext, GG_KEYERRORTEXT, 1);
                        char *to = find_keyword (mtext, GG_KEYTO, 1);
                        carve_statement (&len, "utf-text", GG_KEYLENGTH, 0, 1);
                        carve_statement (&status, "utf-text", GG_KEYSTATUS, 0, 1);
                        carve_statement (&errt, "utf-text", GG_KEYERRORTEXT, 0, 1);
                        carve_statement (&to, "utf-text", GG_KEYTO, 0, 1);
                        carve_stmt_obj (&mtext, true, true);
                        define_statement (&status, GG_DEFNUMBER, false);
                        define_statement (&errt, GG_DEFSTRING, false); // exact length set in glim_utf_to_text
                        define_statement (&to, GG_DEFSTRING, false); // exact length set in glim_utf_to_text
                        //
                        check_var (&mtext, GG_DEFSTRING);

                        oprintf ("char *_gg_errt%ld = GG_EMPTY_STRING;\n", code_text);
                        oprintf ("_gg_st = gg_utf_to_text (%s, %s, %s%s%s, &_gg_errt%ld);\n", mtext, len == NULL ? "-1":len, to==NULL?"":"&(", to==NULL?"NULL":to, to==NULL?"":")", code_text);
                        if (status != NULL) oprintf ("if (_gg_st == -1) {GG_ERR0;%s=GG_ERR_UTF;} else %s=_gg_st;\n", status, status); else oprintf("if (_gg_st == -1) gg_report_error (\"%s\", \"%s\", (gg_num)%ld, (gg_num)GG_ERR_UTF);\n", GG_STMT_FAIL_CHECK, undecorate (src_file_name+GG_BLD_DIR_OFFSET), lnum);
                        if (errt != NULL) oprintf ("%s = _gg_errt%ld;\n", errt, code_text); 

                        code_text++;

                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "text-utf", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;

                        char *status = find_keyword (mtext, GG_KEYSTATUS, 1);
                        char *errt = find_keyword (mtext, GG_KEYERRORTEXT, 1);
                        carve_statement (&status, "text-utf", GG_KEYSTATUS, 0, 1);
                        carve_statement (&errt, "text-utf", GG_KEYERRORTEXT, 0, 1);
                        carve_stmt_obj (&mtext, true, true);
                        define_statement (&status, GG_DEFNUMBER, false);
                        define_statement (&errt, GG_DEFSTRING, false); // exact length set in glim_text_to_utf
                        //
                        check_var (&mtext, GG_DEFSTRING);

                        oprintf ("char *_gg_errt%ld = NULL;\n", code_text);
                        oprintf ("_gg_st_str = gg_text_to_utf (%s, 0, &_gg_errt%ld, 1, true);\n", mtext, code_text);
                        if (status != NULL) oprintf ("if (_gg_st_str != NULL) %s=GG_OKAY; else {GG_ERR0;%s=GG_ERR_UTF;}\n", status, status); else oprintf("if (_gg_st_str == NULL) gg_report_error (\"%s\", \"%s\", (gg_num)%ld, (gg_num)GG_ERR_UTF);\n", GG_STMT_FAIL_CHECK, undecorate (src_file_name+GG_BLD_DIR_OFFSET), lnum);
                        if (errt != NULL) oprintf ("%s = _gg_errt%ld;\n", errt, code_text);


                        code_text++;

                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "rename-file", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;
                        // Example:
                        // rename-file "/home/golf/files/somefile.jpg" to "file..." status st
                        char *to = find_keyword (mtext, GG_KEYTO, 1);
                        char *status = find_keyword (mtext, GG_KEYSTATUS, 1);

                        carve_statement (&status, "rename-file", GG_KEYSTATUS, 0, 1);
                        carve_statement (&to, "rename-file", GG_KEYTO, 1, 1);
                        carve_stmt_obj (&mtext, true, true);
                        make_mem(&to);    

                        define_statement (&status, GG_DEFNUMBER, false);
                        //
                        check_var (&mtext, GG_DEFSTRING);
                        check_var (&to, GG_DEFSTRING);

                        //
                        // Look for each option and collect relevant info
                        //
                        oprintf ("_gg_st = rename ((%s), (%s));GG_ERR;\n", mtext, to);
                        if (status == NULL)
                        {
                            oprintf("if (_gg_st != GG_OKAY) gg_report_error (\"%s\", \"%s\", (gg_num)%ld, (gg_num)GG_ERR_RENAME);\n", GG_STMT_FAIL_CHECK, undecorate (src_file_name+GG_BLD_DIR_OFFSET), lnum);
                        }
                        else
                        {
                            oprintf ("if (_gg_st != 0) {GG_TRACE (\"Cannot rename file [%%s] to [%%s], error [%%s]\",%s,%s,strerror(errno));%s=GG_ERR_RENAME;} else %s=GG_OKAY;\n",  mtext,to, status, status);
                        }


                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "delete-dir", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;
                        char *status = find_keyword (mtext, GG_KEYSTATUS, 1);

                        carve_statement (&status, "delete-dir", GG_KEYSTATUS, 0, 1);

                        carve_stmt_obj (&mtext, true, true);

                        define_statement (&status, GG_DEFNUMBER, false);
                        //
                        check_var (&mtext, GG_DEFSTRING);

                        //
                        // Look for each option and collect relevant info, namely OS errno
                        //
                        oprintf ("_gg_st = rmdir (%s);GG_ERR;\n", mtext);
                        if (status == NULL)
                        {
                            oprintf("if (_gg_st != GG_OKAY) gg_report_error (\"%s\", \"%s\", (gg_num)%ld, (gg_num)((errno == EEXIST||errno==ENOTEMPTY)?GG_ERR_EXIST:GG_ERR_FAILED));\n", GG_STMT_FAIL_CHECK, undecorate (src_file_name+GG_BLD_DIR_OFFSET), lnum);
                        }
                        else
                        {
                            // if directory exists, GG_ERR_EXIST is the error
                            oprintf ("if (_gg_st != 0 ) {if (errno == EEXIST || errno == ENOTEMPTY) %s=GG_ERR_EXIST; else {GG_ERR;GG_TRACE (\"Cannot remove directory [%%s], error [%%s]\",%s, strerror(errno)); %s=GG_ERR_FAILED;}} else %s=GG_OKAY;\n", status, mtext, status, status);
                        }


                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "change-mode", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;
                        char *status = find_keyword (mtext, GG_KEYSTATUS, 1);
                        char *mode = find_keyword (mtext, GG_KEYMODE, 1);

                        carve_statement (&status, "change-mode", GG_KEYSTATUS, 0, 1);
                        carve_statement (&mode, "change-mode", GG_KEYMODE, 1, 1);

                        carve_stmt_obj (&mtext, true, true);

                        define_statement (&status, GG_DEFNUMBER, false);
                        check_var (&mode, GG_DEFNUMBER);
                        //
                        check_var (&mtext, GG_DEFSTRING);

                        //
                        // Look for each option and collect relevant info, namely OS errno
                        //
                        oprintf ("_gg_st = chmod (%s, %s);GG_ERR;\n", mtext, mode);
                        if (status == NULL)
                        {
                            oprintf("if (_gg_st != GG_OKAY) gg_report_error (\"%s\", \"%s\", (gg_num)%ld, (gg_num)(errno==ENOENT?GG_ERR_EXIST:GG_ERR_FAILED));\n", GG_STMT_FAIL_CHECK, undecorate (src_file_name+GG_BLD_DIR_OFFSET), lnum);
                        }
                        else
                        {
                            // if directory exists, GG_ERR_EXIST is the error
                            oprintf ("if (_gg_st != 0 ) {if (errno == ENOENT) %s=GG_ERR_EXIST; else {GG_ERR;GG_TRACE (\"Cannot change file or directory permission mode [%%s], error [%%s]\",%s, strerror(errno)); %s=GG_ERR_FAILED;}} else %s=GG_OKAY;\n", status, mtext, status, status);
                        }
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "new-dir", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;
                        char *status = find_keyword (mtext, GG_KEYSTATUS, 1);
                        char *mode = find_keyword (mtext, GG_KEYMODE, 1);

                        carve_statement (&status, "new-dir", GG_KEYSTATUS, 0, 1);
                        carve_statement (&mode, "new-dir", GG_KEYMODE, 0, 1);

                        carve_stmt_obj (&mtext, true, true);

                        define_statement (&status, GG_DEFNUMBER, false);
                        check_var (&mode, GG_DEFNUMBER);
                        //
                        check_var (&mtext, GG_DEFSTRING);
                        if (mode == NULL) mode = "0700";

                        //
                        // Look for each option and collect relevant info, namely OS errno
                        //
                        oprintf ("_gg_st = mkdir (%s, %s);GG_ERR;\n", mtext, mode);
                        if (status == NULL)
                        {
                            oprintf("if (_gg_st != GG_OKAY) gg_report_error (\"%s\", \"%s\", (gg_num)%ld, (gg_num)(errno==EEXIST?GG_ERR_EXIST?GG_ERR_FAILED));\n", GG_STMT_FAIL_CHECK, undecorate (src_file_name+GG_BLD_DIR_OFFSET), lnum);
                        }
                        else
                        {
                            // if directory exists, GG_ERR_EXIST is the error
                            oprintf ("if (_gg_st != 0 ) {if (errno == EEXIST) %s=GG_ERR_EXIST; else {GG_ERR;GG_TRACE (\"Cannot create directory [%%s], error [%%s]\",%s, strerror(errno)); %s=GG_ERR_FAILED;}} else %s=GG_OKAY;\n", status, mtext, status, status);
                        }
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "change-dir", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;
                        char *status = find_keyword (mtext, GG_KEYSTATUS, 1);
                        char *home = find_keyword (mtext, GG_KEYHOME, 1);
                        char *edir = find_keyword (mtext, GG_KEYRUNDIR, 1);

                        carve_statement (&status, "change-dir", GG_KEYSTATUS, 0, 1);
                        carve_statement (&home, "change-dir", GG_KEYHOME, 0, 0);
                        carve_statement (&edir, "change-dir", GG_KEYRUNDIR, 0, 0);
                        if (home == NULL && edir == NULL) carve_stmt_obj (&mtext, true, true); else carve_stmt_obj (&mtext, false, false);

                        define_statement (&status, GG_DEFNUMBER, false);
                        //
                        check_var (&mtext, GG_DEFSTRING);

                        //
                        // Look for each option and collect relevant info, namely OS errno
                        //
                        oprintf ("_gg_st = chdir (%s);GG_ERR;\n", home == NULL ? (edir == NULL ? mtext:"gg_get_config()->app.run_dir"): "gg_get_config()->app.home_dir");
                        if (status == NULL)
                        {
                            oprintf("if (_gg_st != GG_OKAY) gg_report_error (\"%s\", \"%s\", (gg_num)%ld, (gg_num)GG_ERR_FAILED);\n", GG_STMT_FAIL_CHECK, undecorate (src_file_name+GG_BLD_DIR_OFFSET), lnum);
                        }
                        else
                        {
                            oprintf ("if (_gg_st != 0 ) {GG_ERR;GG_TRACE (\"Cannot change to directory [%%s], error [%%s]\",%s, strerror(errno)); %s=GG_ERR_FAILED;} else %s=GG_OKAY;\n", home == NULL ? ( edir == NULL ? mtext:"gg_get_config()->app.run_dir"): "gg_get_config()->app.home_dir", status, status);
                        }
                        // we set this global boolean so we don't have to chdir at the beginning of each request
                        // note that in internal mode or in external, using directly C' chdir() will not affect this, and gg_path_changed should be manually set!!!
                        // nothing to do for edir, since home should for the next request either way
                        // Plus, this shouldn't be used for command-line programs
                        oprintf("gg_path_changed=%s;\n", home == NULL?"true":"false");


                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "delete-file", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;
                        // Example:
                        // delete-file "/home/golf/files/somefile.jpg" status st
                        char *status = find_keyword (mtext, GG_KEYSTATUS, 1);

                        carve_statement (&status, "delete-file", GG_KEYSTATUS, 0, 1);
                        carve_stmt_obj (&mtext, true, true);

                        define_statement (&status, GG_DEFNUMBER, false);
                        //
                        check_var (&mtext, GG_DEFSTRING);

                        //
                        // Look for each option and collect relevant info
                        //
                        oprintf ("_gg_st = unlink (%s);GG_ERR;\n", mtext);
                        if (status == NULL)
                        {
                            oprintf("if (_gg_st != GG_OKAY && errno != ENOENT) gg_report_error (\"%s\", \"%s\", (gg_num)%ld,  (gg_num)GG_ERR_DELETE);\n", GG_STMT_FAIL_CHECK, undecorate (src_file_name+GG_BLD_DIR_OFFSET), lnum);
                        }
                        else
                        {
                            oprintf ("if (_gg_st != 0  && errno != ENOENT) {GG_ERR;GG_TRACE (\"Cannot delete file [%%s], error [%%s]\",%s, strerror(errno)); %s=GG_ERR_DELETE;} else %s=GG_OKAY;\n", mtext, status, status);
                        }


                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "call-web", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;

                        //
                        // Look for each option and collect relevant info
                        //
                        char *resp = find_keyword (mtext, GG_KEYRESPONSE, 1);
                        char *head = find_keyword (mtext, GG_KEYRESPONSEHEADERS, 1);
                        char *rhead = find_keyword (mtext, GG_KEYREQUESTHEADERS, 1);
                        char *resp_code = find_keyword (mtext, GG_KEYRESPONSECODE, 1);
                        char *len = find_keyword (mtext, GG_KEYSTATUS, 1);
                        char *err = find_keyword (mtext, GG_KEYERROR, 1);
                        char *cert = find_keyword (mtext, GG_KEYCERT, 1);
                        char *nocert = find_keyword (mtext, GG_KEYNOCERT, 1);
                        char *cookiejar = find_keyword (mtext, GG_KEYCOOKIEJAR, 1);
                        char *timeout = find_keyword (mtext, GG_KEYTIMEOUT, 1);
                        char *body = find_keyword (mtext, GG_KEYREQUESTBODY, 1);
                        char *method = find_keyword (mtext, GG_KEYMETHOD, 1);

                        carve_statement (&body, "call-web", GG_KEYREQUESTBODY, 0, 1);
                        carve_statement (&timeout, "call-web", GG_KEYTIMEOUT, 0, 1);
                        carve_statement (&method, "call-web", GG_KEYMETHOD, 0, 1);
                        carve_statement (&resp, "call-web", GG_KEYRESPONSE, 1, 1);
                        carve_statement (&head, "call-web", GG_KEYRESPONSEHEADERS, 0, 1);
                        carve_statement (&rhead, "call-web", GG_KEYREQUESTHEADERS, 0, 1);
                        carve_statement (&resp_code, "call-web", GG_KEYRESPONSECODE, 0, 1);
                        carve_statement (&len, "call-web", GG_KEYSTATUS, 0, 1);
                        carve_statement (&err, "call-web", GG_KEYERROR, 0, 1);
                        carve_statement (&cert, "call-web", GG_KEYCERT, 0, 1);
                        carve_statement (&nocert, "call-web", GG_KEYNOCERT, 0, 0);
                        carve_statement (&cookiejar, "call-web", GG_KEYCOOKIEJAR, 0, 1);
                        carve_stmt_obj (&mtext, true, true);
                        make_mem(&cert);    
                        make_mem(&method);    
                        make_mem(&cookiejar);    
                        //
                        check_var (&mtext, GG_DEFSTRING);
                        check_var (&cert, GG_DEFSTRING);
                        check_var (&method, GG_DEFSTRING);
                        check_var (&cookiejar, GG_DEFSTRING);
                        check_var (&timeout, GG_DEFNUMBER);

                        // process body clause after ward because it is done within a copy of carved out 'body' above
                        char *files = NULL;
                        char *fields = NULL;
                        char *content = NULL;
                        if (body != NULL)
                        {
                            char *mbody = gg_strdup (body);
                            content = find_keyword (mbody, GG_KEYCONTENT, 1);
                            files = find_keyword (mbody, GG_KEYFILES, 1);
                            fields = find_keyword (mbody, GG_KEYFIELDS, 1);
                            carve_statement (&content, "call-web", GG_KEYCONTENT, 0, 1);
                            carve_statement (&files, "call-web", GG_KEYFILES, 0, 1);
                            carve_statement (&fields, "call-web", GG_KEYFIELDS, 0, 1);
                            make_mem(&content);    
                            //
                            check_var (&content, GG_DEFSTRING);
                        }

                        //defines
                        define_statement (&len, GG_DEFNUMBER, false);
                        define_statement (&resp, GG_DEFSTRING, false); // exact length  set in gg_post_url_with_response
                        define_statement (&head, GG_DEFSTRING, false); // exact length  set in gg_post_url_with_response
                        define_statement (&resp_code, GG_DEFNUMBER, false);
                        define_statement (&err, GG_DEFSTRING, false); // exact length set in gg_post_url_with_response

                        char *req_header = NULL;
                        char req_header_ptr[200];
                        char* clen = NULL;
                        char* ctype = NULL;
                        if (rhead != NULL)
                        {
                            // GG_HEADER_FILE/PAGE doesn't matter, cache is not used with request header
                            req_header = make_default_header(GG_HEADER_FILE, http_header_count, 1);
                            snprintf (req_header_ptr, sizeof(req_header_ptr), "&%s", req_header);
                            process_http_header ("call-web", rhead, req_header, http_header_count, 1, &clen, &ctype); // GG_KEYCONTENTTYPE GG_KEYCUSTOM GG_KEYCONTENTLENGTH
                            http_header_count++;
                        }
                        if (content != NULL && ctype == NULL) 
                        {
                            gg_report_error( "'content-type' must be specified with 'content' subclause in 'request-body' in call-web statement");
                        }
                        if (content == NULL && clen != NULL && body != NULL) 
                        {
                            gg_report_error( "'content-length' cannot be specified without 'content' subclause in 'request-body' in call-web statement");
                        }
                        if (content == NULL && ctype != NULL && body != NULL) 
                        {
                            gg_report_error( "'content-type' cannot be specified without 'content' subclause in 'request-body' in call-web statement");
                        }
                        if ((files == NULL && fields == NULL && content == NULL ) && body != NULL) 
                        {
                            gg_report_error( "either 'content' or 'files'/'fields' must be specified with 'body' clause in  call-web statement");
                        }
                        if ((files != NULL || fields != NULL)) 
                        {
                            if (ctype == NULL) ctype = "multipart/form-data"; // when files or forms used, always this
                        }
                        if ((files != NULL || fields != NULL) && (content != NULL)) 
                        {
                            gg_report_error( "you can specify either 'content' or 'files'/'fields', but not both in  call-web statement");
                        }
                        if (cert != NULL && nocert != NULL)
                        {
                            gg_report_error( "cert and no-cert cannot coexist in the same call-web statement");
                        }

                        // cert cannot be defined, must exist and be filled with the location of .ca file!

                        // store fields and files (with body) arrays
                        char fields_var[100];
                        char files_var[100];
                        if (fields != NULL)
                        {
                            snprintf (fields_var, sizeof (fields_var), "_gg_fields_arr%ld", total_body);
                            outargs(fields, fields_var, "char *", 0, 1, GG_DEFSTRING, GG_DEFSTRING);
                        }
                        if (files != NULL)
                        {
                            snprintf (files_var, sizeof (files_var), "_gg_files_arr%ld", total_body);
                            outargs(files, files_var, "char *", 0, 1, GG_DEFSTRING, GG_DEFSTRING);
                        }

                        total_body++;

                        oprintf ("_gg_st=gg_post_url_with_response(%s, &(%s), %s%s%s, %s%s%s, %s, %s, %s%s%s, %s, %s, %s, %s, %s, %s, %s, %s);\n", mtext, resp, head==NULL ? "":"&(",head==NULL ? "NULL":head, head==NULL ? "":")", err==NULL ? "":"&(",err==NULL ? "NULL":err, err==NULL ? "":")", nocert != NULL ? "NULL" : (cert != NULL ? cert : "\"\""), cookiejar == NULL ? "NULL":cookiejar, resp_code==NULL ? "":"&(",resp_code==NULL ? "NULL":resp_code, resp_code==NULL ? "":")", timeout==NULL ? "120":timeout, body == NULL ? "0":"1", fields == NULL ? "NULL":fields_var, files == NULL ? "NULL":files_var, req_header == NULL ? "NULL":req_header_ptr, method == NULL ? "NULL" :method, content==NULL?"NULL":content, clen==NULL?"-1":clen);
                        if (len != NULL) oprintf("%s=_gg_st;\n",len); else oprintf("if (_gg_st < 0) gg_report_error (\"%s\", \"%s\", (gg_num)%ld, _gg_st);\n", GG_STMT_FAIL_CHECK, undecorate (src_file_name+GG_BLD_DIR_OFFSET), lnum);

                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "run-remote", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;
                        run_service (mtext);
                        // list of keywords for vim, picked up automatically
                        // GG_KEYLOCATION GG_KEYMETHOD GG_KEYAPPPATH GG_KEYREQUESTPATH GG_KEYREQUESTBODY GG_KEYURLPARAMS GG_KEYTIMEOUT GG_KEYENVIRONMENT  GG_KEYLOCAL  GG_KEYURLPATH GG_KEYCONTENTLENGTH GG_KEYCONTENT GG_KEYCONTENTTYPE GG_KEYCONTENTLENGTH GG_KEYCONTENTTYPE
                        // GG_KEYDATA  GG_KEYERROR  GG_KEYSTATUSTEXT GG_KEYEXITSTATUS GG_KEYSTATUS
                        // GG_KEYFINISHEDOKAY GG_KEYSTARTED GG_KEYSTATUS
                        // end list of keywords
                        //
                        //
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "new-remote", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;
                        new_service (mtext);
                        // list of keywords for vim, picked up automatically
                        // GG_KEYLOCATION GG_KEYMETHOD GG_KEYAPPPATH GG_KEYREQUESTPATH GG_KEYREQUESTBODY GG_KEYURLPARAMS GG_KEYTIMEOUT GG_KEYENVIRONMENT  GG_KEYLOCAL  GG_KEYURLPATH GG_KEYCONTENTLENGTH GG_KEYCONTENT GG_KEYCONTENTTYPE GG_KEYCONTENTLENGTH GG_KEYCONTENTTYPE
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "read-remote", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;
                        read_service (mtext);
                        // list of keywords for vim, picked up automatically
                        // GG_KEYDATA  GG_KEYERROR GG_KEYSTATUSTEXT GG_KEYEXITSTATUS GG_KEYSTATUS
                        //
                        //
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "call-extended", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;
                        if (gg_mode != GG_MODE_EXTENDED) gg_report_error ("call-extended statement cannot be used in safe mode, see *-mode statements");
                        carve_stmt_obj (&mtext, true, false);
                        // list of keywords for vim, picked up automatically
                        check_c (mtext);
                        oprintf("%s;\n", mtext);
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "call-remote", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;
                        call_service (mtext);
                        // list of keywords for vim, picked up automatically
                       // GG_KEYFINISHEDOKAY GG_KEYSTARTED GG_KEYSTATUS
                        //
                        // this commented out is just so getvim will color output - DO NOT REMOVE
                        //char *fok = find_keyword (mtext, GG_KEYFINISHEDOKAY, 1);
                        //char *start = find_keyword (mtext, GG_KEYSTARTED, 1);
                        //char *st = find_keyword (mtext, GG_KEYSTATUS, 1);
                        //
                        //this define_statement MUST stay for coloring
                        //define_statement (&st, GG_DEFNUMBER, false);
                        //define_statement (&start, GG_DEFNUMBER, false);
                        //define_statement (&fok, GG_DEFNUMBER, false);
                        //
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "call-handler", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;
                        char *rval = find_keyword (mtext, GG_KEYRETURNVALUE, 1);
                        carve_statement (&rval, "call-handler", GG_KEYRETURNVALUE, 0, 1);
                        define_statement (&rval, GG_DEFNUMBER, false); 

                        //
                        // must check if mtext clause is string constant *before* other variable checks below, because
                        // they may create a temp variable for it
                        bool is_constant = false;
                        if (is_constant_string(mtext) == GG_CONST_OK) is_constant = true;
                        //
                        //
                        //if sub name is a constant, check there's actually a source file to run it
                        check_sub (mtext); // this must be BEFORE carve_stmt_obj as it may create temp variable which is just some generated var name
                                           // it trims mtext, but we're about to do that anyway
                        carve_stmt_obj (&mtext, true, true);
                        check_var (&mtext, GG_DEFSTRING);
                        static gg_num call_handler_c = 0;
                        if (is_constant)
                        {
                            // if this is a constant call, cache the pointer to function, and just call it as such without
                            // involving hash search
                            oprintf ("static void *_gg_call_h_%ld = NULL;\n", call_handler_c);
                            oprintf ("gg_subs(%s, &_gg_call_h_%ld);\n", mtext, call_handler_c);
                            call_handler_c++;
                        } else oprintf ("gg_subs(%s, NULL);\n", mtext);
                        if (rval) oprintf("%s=gg_get_config ()->ctx.req->return_val;\n", rval);
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "uniq-file", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;
                        char *temp = find_keyword (mtext, GG_KEYTEMPORARY, 1);

                        //
                        // After all options positions have been found, we must get the options 
                        // for ALL of them
                        //
                        carve_statement (&temp, "uniq-file", GG_KEYTEMPORARY, 0, 0);
                        carve_stmt_obj (&mtext, true, true);

                        //defines
                        define_statement (&mtext, GG_DEFSTRING, false); // exact length set in gg_make_document

                        //generate code
                        oprintf ("{FILE *f = gg_make_document (&(%s), %d); fclose (f);}\n", mtext, temp!=NULL?1:0);

                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "get-time", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;
                        // Example:
                        // get-time to [define] res timezone "GMT" [year +/-3] [month +/-3] [day +/-3] [hour +/-3] [min +/-3] [sec +/-3] [format "%a..."]
                        // get system time that's current, unless one of the options are used which go into the future/past as given

                        //
                        // Look for each option and collect relevant info
                        // First we MUST get each options position
                        //
                        char *res = find_keyword (mtext, GG_KEYTO, 1);
                        char *epoch = find_keyword (mtext, GG_KEYSINCEEPOCH, 1);
                        char *fepoch = find_keyword (mtext, GG_KEYFROMEPOCH, 1);
                        char *timezone = find_keyword (mtext, GG_KEYTIMEZONE, 1);
                        char *year = find_keyword (mtext, GG_KEYYEAR, 1);
                        char *month = find_keyword (mtext, GG_KEYMONTH, 1);
                        char *day = find_keyword (mtext, GG_KEYDAY, 1);
                        char *hour = find_keyword (mtext, GG_KEYHOUR, 1);
                        char *min = find_keyword (mtext, GG_KEYMIN, 1);
                        char *sec = find_keyword (mtext, GG_KEYSEC, 1);
                        char *format = find_keyword (mtext, GG_KEYFORMAT, 1);

                        //
                        // After all options positions have been found, we must get the options 
                        // for ALL of them
                        //
                        carve_statement (&res, "get-time", GG_KEYTO, 1, 1);
                        carve_statement (&epoch, "get-time", GG_KEYSINCEEPOCH, 0, 0);
                        carve_statement (&fepoch, "get-time", GG_KEYFROMEPOCH, 0, 1);
                        carve_statement (&timezone, "get-time", GG_KEYTIMEZONE, 0, 1);
                        carve_statement (&year, "get-time", GG_KEYYEAR, 0, 1);
                        carve_statement (&month, "get-time", GG_KEYMONTH, 0, 1);
                        carve_statement (&day, "get-time", GG_KEYDAY, 0, 1);
                        carve_statement (&hour, "get-time", GG_KEYHOUR, 0, 1);
                        carve_statement (&min, "get-time", GG_KEYMIN, 0, 1);
                        carve_statement (&sec, "get-time", GG_KEYSEC, 0, 1);
                        carve_statement (&format, "get-time", GG_KEYFORMAT, 0, 1);
                        carve_stmt_obj (&mtext, false, false);

                        if (epoch != NULL && (timezone != NULL || year != NULL || month != NULL || day != NULL || hour != NULL || min != NULL || sec != NULL || format != NULL || fepoch != NULL))
                        {
                            gg_report_error ("Cannot use timezone/year/month/day/hour/min/sec/format/from-epoch clause(s) if since-epoch clause is used");
                        }

                        if (epoch == NULL)
                        {
                            make_mem(&timezone);    
                            make_mem(&format);    
                        }

                        // handle any possibly created variables 
                        // res is mandatory, but even if not, define_statement will skip if NULL
                        if (epoch != NULL) define_statement (&res, GG_DEFNUMBER, false); // seconds since epoch
                        else define_statement (&res, GG_DEFSTRING, false); // exact length set in gg_time
                        if (epoch == NULL)
                        {
                            check_var (&timezone, GG_DEFSTRING);
                            check_var (&year, GG_DEFNUMBER);
                            check_var (&month, GG_DEFNUMBER);
                            check_var (&day, GG_DEFNUMBER);
                            check_var (&hour, GG_DEFNUMBER);
                            check_var (&min, GG_DEFNUMBER);
                            check_var (&sec, GG_DEFNUMBER);
                            check_var (&format, GG_DEFSTRING);
                            check_var (&fepoch, GG_DEFNUMBER);
                        }

                        // No need to check if res!=NULL (i.e. it's mandatory), the above carve_statement checks that

                        //
                        // If there is data right after statement (i.e. 'get-time') and it has no option (such as call-web https://...)
                        // then mtext is this option (in this case https://...). In this particular case, we don't have such an option -
                        // every option has a keyword preceding it, including the first one.
                        //

                        if (epoch == NULL)
                        {
                            oprintf ("%s=gg_time(%s, %s, %s, %s, %s, %s, %s, %s, %s);\n", res, fepoch==NULL?"-1":fepoch, timezone==NULL ? "\"GMT\"":timezone, format==NULL ? "NULL":format, year==NULL ? "0":year, month==NULL ? "0":month, 
                                day==NULL ? "0":day, hour==NULL ? "0":hour, min==NULL ? "0":min, sec==NULL ? "0":sec);
                        }
                        else
                        {
                            oprintf ("%s=time(NULL); if ((time_t)(%s)==(time_t)-1) gg_report_error (\"Cannot obtain time, error [%%d]\",errno);\n", res, res);
                        }


                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "match-regex", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;
                        char *cache = find_keyword (mtext, GG_KEYCACHE, 1);
                        char *ccache = find_keyword (mtext, GG_KEYCLEARCACHE, 1);
                        char *in = find_keyword (mtext, GG_KEYIN, 1);
                        char *replace_with = find_keyword (mtext, GG_KEYREPLACEWITH, 1);
                        char *result = find_keyword (mtext, GG_KEYRESULT, 1);
                        char *status = find_keyword (mtext, GG_KEYSTATUS, 1);
                        char *case_insensitive = find_keyword (mtext, GG_KEYCASEINSENSITIVE, 1);
                        char *utf = find_keyword (mtext, GG_KEYUTF, 1);
                        char *single_match = find_keyword (mtext, GG_KEYSINGLEMATCH, 1);

                        //
                        // After all options positions have been found, we must get the options 
                        // for ALL of them
                        //
                        carve_statement (&cache, "match-regex", GG_KEYCACHE, 0, 0);
                        carve_statement (&ccache, "match-regex", GG_KEYCLEARCACHE, 0, 1);
                        carve_statement (&in, "match-regex", GG_KEYIN, 1, 1);
                        carve_statement (&replace_with, "match-regex", GG_KEYREPLACEWITH, 0, 1);
                        carve_statement (&result, "match-regex", GG_KEYRESULT, 0, 1);
                        carve_statement (&status, "match-regex", GG_KEYSTATUS, 0, 1);
                        carve_statement (&case_insensitive, "match-regex", GG_KEYCASEINSENSITIVE, 0, 2);
                        carve_statement (&utf, "match-regex", GG_KEYUTF, 0, 2);
                        carve_statement (&single_match, "match-regex", GG_KEYSINGLEMATCH, 0, 2);
                        carve_stmt_obj (&mtext, true, true);
                        char *pattern = mtext;

                        make_mem(&in);
                        make_mem(&replace_with);

                        char *case_insensitivec = opt_clause(case_insensitive, "1", "0", GG_DEFNUMBER);
                        char *utfc = opt_clause(utf, "1", "0", GG_DEFNUMBER);
                        char *single_matchc = opt_clause(single_match, "1", "0", GG_DEFNUMBER);

                        if (result != NULL && replace_with == NULL)
                        {
                            gg_report_error( "replace-with must be specified if result is given in match-regex statement");
                        }
                        if (replace_with != NULL && result == NULL)
                        {
                            gg_report_error( "result must be specified if replace-with is given in match-regex statement");
                        }
                        if (result == NULL && status == NULL)
                        {
                            gg_report_error( "status must be specified in this match-regex statement");
                        }

                        // result can be defined
                        define_statement (&result, GG_DEFSTRING, false); // exact length is set in gg_regex
                        define_statement (&status, GG_DEFNUMBER, false);
                        check_var (&replace_with, GG_DEFSTRING);
                        check_var (&mtext, GG_DEFSTRING);
                        check_var (&case_insensitivec, GG_DEFNUMBER);
                        check_var (&ccache, GG_DEFBOOL);
                        check_var (&utfc, GG_DEFNUMBER);
                        check_var (&single_matchc, GG_DEFNUMBER);
                        check_var (&in, GG_DEFSTRING);

                        //
                        // If there is data right after statement (i.e. 'match') and it has no option (such as call-web https://...)
                        // then mtext is this option (in this case https://...). 
                        //

                        // if no cache, then we pass NULL pointer, if there is cache we pass address of (originally) NULL pointer
                        char regname[100];
                        snprintf (regname, sizeof(regname), "_gg_regex%ld", regex_cache);
                        oprintf ("static regex_t *%s = NULL;\nGG_UNUSED(%s);\n", regname, regname);
                        if (ccache != NULL) 
                        {
                            // if clear-cache is true, then first check if there's existing regex context, if so, clear it
                            // then set context to NULL so it's created again.
                            oprintf ("if ((%s)) {if (%s != NULL) gg_regfree(%s); %s = NULL;}\n", ccache, regname, regname, regname);
                        }

                        if (replace_with == NULL) oprintf ("%s=gg_regex(%s, %s, %s, %s, %s, %s, %s, %s%s);\n", status==NULL?"_gg_st":status, in, pattern, "NULL", "NULL", utfc, case_insensitivec, single_matchc, cache==NULL?"":"&",cache==NULL?"NULL":regname);
                        else oprintf ("%s=gg_regex(%s, %s, %s, &(%s), %s, %s, %s, %s%s);\n", status==NULL?"_gg_st":status, in, pattern, replace_with, result, utfc, case_insensitivec, single_matchc, cache==NULL?"":"&",cache==NULL?"NULL":regname);
                        //We don't check here, because not replacing anything doesn't mean it's an error
                        //if (status == NULL) oprintf("if (_gg_st == 0) gg_report_error (\"%s\", \"%s\", (gg_num)%ld, _gg_st);\n", GG_STMT_FAIL_CHECK, undecorate (src_file_name+GG_BLD_DIR_OFFSET), lnum);
                        gg_free(case_insensitivec);
                        gg_free(single_matchc);
                        gg_free(utfc);

                        regex_cache++;
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "get-sys", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;

                        //
                        // Look for each option and collect relevant info
                        // First we MUST get each options position
                        //
                        char *env = find_keyword (mtext, GG_KEYENVIRONMENT, 1);
                        char *osname = find_keyword (mtext, GG_KEYOSNAME, 1);
                        char *osver = find_keyword (mtext, GG_KEYOSVERSION, 1);
                        char *to = find_keyword (mtext, GG_KEYTO, 1);
                        char *dir = find_keyword (mtext, GG_KEYDIRECTORY, 1);

                        //
                        // After all options positions have been found, we must get the options 
                        // for ALL of them
                        //
                        carve_statement (&env, "get-sys", GG_KEYENVIRONMENT, 0, 1);
                        carve_statement (&osname, "get-sys", GG_KEYOSNAME, 0, 0);
                        carve_statement (&osver, "get-sys", GG_KEYOSVERSION, 0, 0);
                        carve_statement (&to, "get-sys", GG_KEYTO, 1, 1);
                        carve_statement (&dir, "get-sys", GG_KEYDIRECTORY, 0, 0);
                        carve_stmt_obj (&mtext, false, false);
                        make_mem(&env);

                        // result can be defined
                        define_statement (&to, GG_DEFSTRING, false); // exact length is set below with strdup
                        check_var (&env, GG_DEFSTRING);

                        gg_num tot_opt = (env!=NULL?1:0)+(osname!=NULL?1:0)+(osver!=NULL?1:0)+(dir!=NULL?1:0);
                        if (tot_opt != 1)
                        {
                            gg_report_error( "Exactly one option must be in get-sys statement");
                        }

                        if (env !=NULL) oprintf ("%s=gg_strdup(gg_getenv(%s));\n", to,env);
                        if (osname !=NULL) oprintf ("%s=gg_strdup(gg_os_name());\n", to);
                        if (osver !=NULL) oprintf ("%s=gg_strdup(gg_os_version());\n", to);
                        if (dir !=NULL) oprintf ("%s=gg_strdup(gg_get_config()->app.run_dir);\n", to);


                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "get-app", &mtext, &msize, 0, &gg_is_inline)) != 0)
                    {
                        GG_GUARD
                        i = newI;
                        // Example:
                        // get-app name | version | directory  to [define] var

                        //
                        // Look for each option and collect relevant info
                        // First we MUST get each options position
                        //
                        char *appdir = find_keyword (mtext, GG_KEYDIRECTORY, 1);
                        char *name = find_keyword (mtext, GG_KEYNAME0, 1);
                        char *tracedir = find_keyword (mtext, GG_KEYTRACEDIRECTORY, 1);
                        char *rootdir = find_keyword (mtext, GG_KEYROOTDIRECTORY, 1);
                        char *filedir = find_keyword (mtext, GG_KEYFILEDIRECTORY, 1);
                        char *uploadsize = find_keyword (mtext, GG_KEYUPLOADSIZE, 1);
                        char *dbconfname = find_keyword (mtext, GG_KEYDBVENDOR, 1);
                        char *serv = find_keyword (mtext, GG_KEYISSERVICE, 1);
                        char *apath = find_keyword (mtext, GG_KEYPATH, 1);

                        char *to = find_keyword (mtext, GG_KEYTO, 1);

                        //
                        // After all options positions have been found, we must get the options
                        // for ALL of them
                        //
                        carve_statement (&name, "get-app", GG_KEYNAME0, 0, 0);
                        carve_statement (&appdir, "get-app", GG_KEYDIRECTORY, 0, 0);
                        carve_statement (&tracedir, "get-app", GG_KEYTRACEDIRECTORY, 0, 0);
                        carve_statement (&rootdir, "get-app", GG_KEYROOTDIRECTORY, 0, 0);
                        carve_statement (&filedir, "get-app", GG_KEYFILEDIRECTORY, 0, 0);
                        carve_statement (&uploadsize, "get-app", GG_KEYUPLOADSIZE, 0, 0);
                        carve_statement (&dbconfname, "get-app", GG_KEYDBVENDOR, 0, 1);
                        carve_statement (&serv, "get-app", GG_KEYISSERVICE, 0, 0);
                        carve_statement (&apath, "get-app", GG_KEYPATH, 0, 0);
                        carve_statement (&to, "get-app", GG_KEYTO, 1, 1);
                        carve_stmt_obj (&mtext, false, false);

                        // result can be defined
                        if (uploadsize !=NULL) define_statement (&to, GG_DEFNUMBER, false);
                        else if (serv !=NULL) define_statement (&to, GG_DEFBOOL, false);
                        else define_statement (&to, GG_DEFSTRING, false); // exact length set with strdup below

                        gg_num tot_opt = (serv!=NULL?1:0)+(appdir!=NULL?1:0)+(rootdir!=NULL?1:0)+(tracedir!=NULL?1:0)+(filedir!=NULL?1:0)+(apath!=NULL?1:0)+(uploadsize!=NULL?1:0)+(name!=NULL?1:0)+(dbconfname!=NULL?1:0);
                        if (tot_opt != 1)
                        {
                            gg_report_error( "Exactly one option must be in get-app statement");
                        }

                        if (appdir !=NULL) oprintf ("%s=gg_strdup(gg_get_config()->app.home_dir);\n", to); 
                        if (tracedir !=NULL) oprintf ("%s=gg_strdup(gg_get_config()->app.trace_dir);\n", to); 
                        if (rootdir !=NULL) oprintf ("%s=gg_strdup(gg_root);\n", to); 
                        if (filedir !=NULL) oprintf ("%s=gg_strdup(gg_get_config()->app.file_dir);\n", to); 
                        if (uploadsize !=NULL) oprintf ("%s=gg_get_config()->app.max_upload_size;\n", to); 
                        if (name !=NULL) oprintf ("%s=gg_strdup(gg_app_name);\n", to); 
                        if (serv !=NULL) oprintf ("%s=gg_is_service();\n", to);                        
                        if (dbconfname !=NULL) 
                        {
                            gg_num dbi = find_connection (dbconfname);
                            oprintf ("%s=gg_strdup(\"%s\");\n", to, gg_db_vendor(dbi));  // not alloced
                        }
                        if (apath !=NULL) oprintf ("%s=gg_strdup(gg_app_path);\n", to); // not alloced, const

                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "get-req", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;
                        //
                        // Look for each option and collect relevant info
                        // First we MUST get each options position
                        //
                        char *sfile = find_keyword (mtext, GG_KEYSOURCEFILE, 1);
                        char *ext = find_keyword (mtext, GG_KEYEXTERNALCALL, 1);
                        char *dir = find_keyword (mtext, GG_KEYDIRECTORY, 1);
                        char *name = find_keyword (mtext, GG_KEYNAME0, 1);
                        char *method = find_keyword (mtext, GG_KEYMETHOD, 1);
                        char *ctype = find_keyword (mtext, GG_KEYCONTENTTYPE, 1);
                        char *eno = find_keyword (mtext, GG_KEYERRNO, 1);
                        char *err = find_keyword (mtext, GG_KEYERROR, 1);
                        char *process_id = find_keyword (mtext, GG_KEYPROCESSID, 1);
                        char *tracefile = find_keyword (mtext, GG_KEYTRACEFILE, 1);
                        char *argnum = find_keyword (mtext, GG_KEYARGCOUNT, 1);
                        char *argval = find_keyword (mtext, GG_KEYARGVALUE, 1);
                        char *ref = find_keyword (mtext, GG_KEYREFERRINGURL, 1);
                        char *numcookie = find_keyword (mtext, GG_KEYCOOKIECOUNT, 1);
                        char *cookie = find_keyword (mtext, GG_KEYCOOKIE, 1);
                        char *header = find_keyword (mtext, GG_KEYHEADER, 1);
                        char *to = find_keyword (mtext, GG_KEYTO, 1);

                        //
                        // After all options positions have been found, we must get the options 
                        // for ALL of them
                        //
                        carve_statement (&sfile, "get-req", GG_KEYSOURCEFILE, 0, 0);
                        carve_statement (&ext, "get-req", GG_KEYEXTERNALCALL, 0, 0);
                        carve_statement (&name, "get-req", GG_KEYNAME0, 0, 0);
                        carve_statement (&method, "get-req", GG_KEYMETHOD, 0, 0);
                        carve_statement (&dir, "get-req", GG_KEYDIRECTORY, 0, 0);
                        carve_statement (&ctype, "get-req", GG_KEYCONTENTTYPE, 0, 0);
                        carve_statement (&tracefile, "get-req", GG_KEYTRACEFILE, 0, 0);
                        carve_statement (&argnum, "get-req", GG_KEYARGCOUNT, 0, 0);
                        carve_statement (&argval, "get-req", GG_KEYARGVALUE, 0, 1);
                        carve_statement (&ref, "get-req", GG_KEYREFERRINGURL, 0, 0);
                        carve_statement (&numcookie, "get-req", GG_KEYCOOKIECOUNT, 0, 0);
                        carve_statement (&cookie, "get-req", GG_KEYCOOKIE, 0, 1);
                        carve_statement (&header, "get-req", GG_KEYHEADER, 0, 1);
                        carve_statement (&process_id, "get-req", GG_KEYPROCESSID, 0, 0);
                        carve_statement (&eno, "get-req", GG_KEYERRNO, 0, 0);
                        carve_statement (&err, "get-req", GG_KEYERROR, 0, 0);
                        carve_statement (&to, "get-req", GG_KEYTO, 1, 1);
                        carve_stmt_obj (&mtext, false, false);

                        make_mem(&header);

                        // result can be defined
                        if (ext != NULL ) define_statement (&to, GG_DEFBOOL, false);
                        else if (numcookie != NULL || argnum != NULL || method != NULL ) define_statement (&to, GG_DEFNUMBER, false);
                        else if (ctype != NULL || argval != NULL || cookie != NULL || header != NULL || name != NULL || dir != NULL || sfile != NULL) 
                        {
                            define_statement (&to, GG_DEFSTRING, false); // exact length set with strdup below
                        }
                        else if (eno != NULL) define_statement (&to, GG_DEFNUMBER, false);
                        else if (process_id != NULL) define_statement (&to, GG_DEFNUMBER, false);
                        else 
                        {
                            define_statement (&to, GG_DEFSTRING, false); // exact length set with strdup below
                        }
                        check_var (&argval, GG_DEFNUMBER);
                        check_var (&cookie, GG_DEFNUMBER);
                        check_var (&header, GG_DEFSTRING);

                        gg_num tot_opt = (tracefile!=NULL?1:0)+(argval!=NULL?1:0)+(ext!=NULL?1:0)+(argnum!=NULL?1:0)+(ref!=NULL?1:0)+(numcookie!=NULL?1:0)+(cookie!=NULL?1:0)+(header!=NULL?1:0)+(process_id!=NULL?1:0)+(eno!=NULL?1:0)+(err!=NULL?1:0)+(method!=NULL?1:0)+(dir!=NULL?1:0)+(ctype!=NULL?1:0)+(name!=NULL?1:0)+(sfile!=NULL?1:0);
                        if (tot_opt != 1)
                        {
                            gg_report_error( "Exactly one option must be in get-req statement (%ld)", tot_opt);
                        }

                        if (tracefile !=NULL) oprintf ("%s=gg_strdup(gg_get_config()->trace.fname);\n", to); 
                        if (argval !=NULL) oprintf ("%s=gg_strdup(gg_get_config()->ctx.req->args.args[%s-1]);\n", to, argval); 
                        if (argnum !=NULL) oprintf ("%s=gg_get_config()->ctx.req->args.num_of_args;\n", to); // not alloced
                        if (ref !=NULL) oprintf ("%s=gg_strdup(gg_get_config()->ctx.req->referring_url);\n", to); 
                        if (numcookie !=NULL) oprintf ("%s=gg_get_config()->ctx.req->num_of_cookies;\n", to); 
                        if (cookie !=NULL) oprintf ("%s=gg_strdup(gg_get_config()->ctx.req->cookies[%s-1].data);\n", to, cookie); 
                        if (process_id !=NULL) oprintf ("%s=getpid();\n", to); 
                        if (eno !=NULL) oprintf ("%s=gg_errno;\n", to); 
                        if (ext !=NULL) oprintf ("%s=!(gg_get_config()->ctx.req->sub);\n", to); 
                        if (err !=NULL) oprintf ("%s=gg_strdup(strerror(gg_errno));\n", to); 
                        if (header !=NULL) oprintf ("%s=gg_strdup(gg_getheader(%s));\n", to, header); 
                        if (method !=NULL) oprintf ("%s=gg_get_config()->ctx.req->method;\n", to); 
                        // get current directory will return empty string if it can't get it, and you can get errno from get-req (this is GG_ERR below)
                        // we size the variable to be just what's needed, even though we get a lot more to begin with so getcwd() can succeed. That way
                        // there's no wasted memory and no fragmentation.
                        // Unfortunately we have to do strlen on the path, as there doesn't seem to be a way to get it. At least such memory is safe to handle.
                        if (dir !=NULL) oprintf ("{char *path=gg_malloc(GG_MAX_PATH_LEN); char *rpath=getcwd(path, GG_MAX_PATH_LEN);GG_ERR;if (rpath==NULL) {%s=GG_EMPTY_STRING;gg_free(path);} else {path = gg_realloc (gg_mem_get_id(path), strlen(path)+1); %s=path;}}\n", to, to); 
                        if (name !=NULL) oprintf ("%s=gg_strdup(gg_get_config()->ctx.req->name);\n", to); 
                        if (sfile !=NULL) 
                        {
                            char *osrc = undecorate (src_file_name+GG_BLD_DIR_OFFSET);
                            oprintf ("%s=gg_strdup(\"%s\");\n",to, osrc); 
                            gg_free (osrc);
                        }
                        if (ctype !=NULL) oprintf ("%s=gg_strdup(gg_getenv(\"CONTENT_TYPE\"));\n", to); 


                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "abs-number", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;

                        char *to = find_keyword (mtext, GG_KEYTO, 1);

                        carve_statement (&to, "string-number", GG_KEYTO, 1, 1); 
                        
                        carve_stmt_obj (&mtext, true, true);
                        define_statement (&to, GG_DEFNUMBER, false);
                        //
                        check_var (&mtext, GG_DEFNUMBER);

                        oprintf ("%s=llabs((long long)(%s));\n", to, mtext);

                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "string-number", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;

                        char *to = find_keyword (mtext, GG_KEYTO, 1);
                        char *base = find_keyword (mtext, GG_KEYBASE, 1);
                        char *st = find_keyword (mtext, GG_KEYSTATUS, 1);

                        carve_statement (&to, "string-number", GG_KEYTO, 1, 1); 
                        carve_statement (&base, "string-number", GG_KEYBASE, 0, 1); 
                        carve_statement (&st, "string-number", GG_KEYSTATUS, 0, 1);
                        
                        carve_stmt_obj (&mtext, true, true);
                        define_statement (&to, GG_DEFNUMBER, false);
                        define_statement (&st, GG_DEFNUMBER, false);
                        //
                        check_var (&mtext, GG_DEFSTRING);

                        oprintf ("%s=gg_str2num (%s, %s, &_gg_st);\n", to, mtext, base!=NULL?base:"0");
                        if (st != NULL) oprintf("%s=_gg_st;\n",st); else oprintf("if (_gg_st != GG_OKAY) gg_report_error (\"%s\", \"%s\", (gg_num)%ld, _gg_st);\n", GG_STMT_FAIL_CHECK, undecorate (src_file_name+GG_BLD_DIR_OFFSET), lnum);


                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "number-string", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;

                        char *to = find_keyword (mtext, GG_KEYTO, 1);
                        char *base = find_keyword (mtext, GG_KEYBASE, 1);
                        char *st = find_keyword (mtext, GG_KEYSTATUS, 1);

                        carve_statement (&to, "number-string", GG_KEYTO, 0, 1);  
                        carve_statement (&base, "number-string", GG_KEYBASE, 0, 1); 
                        carve_statement (&st, "number-string", GG_KEYSTATUS, 0, 1); 
                        carve_stmt_obj (&mtext, true, false);
                        define_statement (&to, GG_DEFSTRING, false); // exact length set in gg_num2str
                        define_statement (&st, GG_DEFNUMBER, false);
                        check_var (&mtext, GG_DEFNUMBER);
                        check_var (&base, GG_DEFNUMBER);

                        //Example for deprecation: length is old, buffer-size new

                        char *num0 = mtext; // number to convert, cannot use 'num' for var name as it is a type (solved now with gg_num)


                        do_numstr (to, num0, base); // _gg_st is produced inside do_numstr(), it's 0 if failed
                        if (st!=NULL) oprintf("if (_gg_st==0) (%s)=GG_ERR_FAILED; else (%s)=GG_OKAY;\n", st, st);
                        else oprintf("if (_gg_st == 0) gg_report_error (\"%s\", \"%s\", (gg_num)%ld, (gg_num)GG_ERR_FAILED);\n", GG_STMT_FAIL_CHECK, undecorate (src_file_name+GG_BLD_DIR_OFFSET), lnum);

                        continue;
                    }
                    else if ((newI1=recog_statement(line, i, "extended-mode", &mtext, &msize, 1, &gg_is_inline)) != 0)
                    {
                        //No checking if this is within a block, since this is outside any code and doesn't generate code
                        //GG_GUARD
                        i = newI1;

                        carve_stmt_obj (&mtext, false, false);
                        //
                        //
                        if (newI1 != 0) gg_mode = GG_MODE_EXTENDED;

                        if (num_recog > 1) gg_report_error ("extended-mode must be the very first statement in source file");

                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "flush-output", &mtext, &msize, 1, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;

                        carve_stmt_obj (&mtext, false, false);
                        oprintf ("gg_flush_out();\n");

                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "replace-string", &mtext, &msize, 0, &gg_is_inline)) != 0)
                    {
                        GG_GUARD
                        i = newI;

                        char *copy = find_keyword (mtext, GG_KEYCOPY, 1);
                        char *copyend = find_keyword (mtext, GG_KEYCOPYEND, 1);
                        char *swith = find_keyword (mtext, GG_KEYSTARTWITH, 1);
                        char *len = find_keyword (mtext, GG_KEYLENGTH, 1);
                        carve_statement (&copy, "replace-string", GG_KEYCOPY, 0, 1);
                        carve_statement (&copyend, "replace-string", GG_KEYCOPYEND, 0, 1);
                        carve_statement (&swith, "replace-string", GG_KEYSTARTWITH, 0, 1);
                        carve_statement (&len, "replace-string", GG_KEYLENGTH, 0, 1);
                        carve_stmt_obj (&mtext, true, true); // this has make_mem built in
                        //
                        make_mem(&copy);
                        make_mem(&copyend);
                        //
                        check_var (&mtext, GG_DEFSTRING);
                        check_var (&copy, GG_DEFSTRING);
                        check_var (&copyend, GG_DEFSTRING);
                        check_var (&swith, GG_DEFNUMBER);
                        check_var (&len, GG_DEFNUMBER);

                        if ((copy == NULL && copyend == NULL) || (copy != NULL && copyend != NULL)) gg_report_error ("there must be either 'copy' or copy-end clause present");

                        oprintf ("gg_alter_string (%s, %s, %s, %s, %s);\n", mtext, copy==NULL?copyend:copy, swith==NULL?"-1":swith, len==NULL?"-1":len, copy==NULL?"false":"true");

                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "copy-string", &mtext, &msize, 0, &gg_is_inline)) != 0)
                    {
                        GG_GUARD
                        i = newI;

                        char *to = find_keyword (mtext, GG_KEYTO, 1);
                        char *swith = find_keyword (mtext, GG_KEYSTARTWITH, 1);
                        char *len = find_keyword (mtext, GG_KEYLENGTH, 1);
                        carve_statement (&to, "copy-string", GG_KEYTO, 1, 1);
                        carve_statement (&swith, "copy-string", GG_KEYSTARTWITH, 0, 1);
                        carve_statement (&len, "copy-string", GG_KEYLENGTH, 0, 1);
                        carve_stmt_obj (&mtext, true, true);
                        define_statement (&to, GG_DEFSTRING, false); // exact length set in gg_copy_string
                        //
                        check_var (&mtext, GG_DEFSTRING);
                        check_var (&swith, GG_DEFNUMBER);
                        check_var (&len, GG_DEFNUMBER);

                        // must assign NULL, or it would assume cval is gg_string, which it may not be
                        if (len == NULL) oprintf ("%s = gg_strdupl (%s, %s, gg_mem_get_len(gg_mem_get_id(%s)));\n", to, mtext, swith==NULL?"0":swith, mtext); // exact length copied, so works for binary exactly
                        else oprintf ("gg_copy_string (%s, %s, &(%s), %s);\n", mtext, swith==NULL?"0":swith, to, len);

                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "delete-cookie", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;
                        char *path = find_keyword (mtext, GG_KEYPATH, 1);
                        char *st = find_keyword (mtext, GG_KEYSTATUS, 1);
                        char *secure = find_keyword (mtext, GG_KEYSECURE, 1);

                        carve_statement (&secure, "delete-cookie", GG_KEYSECURE, 0, 2);
                        carve_statement (&path, "delete-cookie", GG_KEYPATH, 0, 1);
                        carve_statement (&st, "delete-cookie", GG_KEYSTATUS, 0, 1);
                        carve_stmt_obj (&mtext, true, true);
                        make_mem(&path);
                        define_statement (&st, GG_DEFNUMBER, false);
                        char *secc = opt_clause(secure, "\"Secure; \"", "\"\"", GG_DEFSTRING);
                        check_var (&mtext, GG_DEFSTRING);
                        check_var (&path, GG_DEFSTRING);
                        check_var (&secc, GG_DEFSTRING);


                        oprintf("_gg_st=(gg_delete_cookie (gg_get_config()->ctx.req, %s, %s, %s)<0?GG_ERR_EXIST:GG_OKAY);\n", mtext, path==NULL?"NULL":path, secc ); 
                        if (st != NULL) oprintf("%s=_gg_st;\n",st); else oprintf("if (_gg_st != GG_OKAY) gg_report_error (\"%s\", \"%s\", (gg_num)%ld, _gg_st);\n", GG_STMT_FAIL_CHECK, undecorate (src_file_name+GG_BLD_DIR_OFFSET), lnum);
                        gg_free(secc);

                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "request-body", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;
                        carve_stmt_obj (&mtext, true, false);
                        if (gg_is_valid_param_name(mtext, false, false) != 1) gg_report_error(GG_NAME_INVALID, mtext);
                        define_statement (&mtext, GG_DEFSTRING, true);

                        oprintf ("%s = gg_get_config()->ctx.req->body;\n\n", mtext);
                        oprintf ("gg_mem_add_ref(%s);\n", mtext);

                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "set-cookie", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;

                        while (1) 
                        {
                            char *comma = find_keyword (mtext, GG_KEYCOMMA, 0);
                            carve_statement (&comma, "set-cookie", GG_KEYCOMMA, 0, 1); // separate entries with comma first, so
                                                                                      // this find/carve MUST come before the rest below!
                                                                                      //
                            char *exp = find_keyword (mtext, GG_KEYEXPIRES, 1);
                            char *path = find_keyword (mtext, GG_KEYPATH, 1);
                            char *samesite = find_keyword (mtext, GG_KEYSAMESITE, 1);
                            char *nohttponly = find_keyword (mtext, GG_KEYNOHTTPONLY, 1);
                            char *secure = find_keyword (mtext, GG_KEYSECURE, 1);
                            char *eq = find_keyword (mtext, GG_KEYEQUALSHORT, 0);

                            carve_statement (&exp, "set-cookie", GG_KEYEXPIRES, 0, 1);
                            carve_statement (&path, "set-cookie", GG_KEYPATH, 0, 1);
                            carve_statement (&samesite, "set-cookie", GG_KEYSAMESITE, 0, 1);
                            carve_statement (&nohttponly, "set-cookie", GG_KEYNOHTTPONLY, 0, 2);
                            carve_statement (&secure, "set-cookie", GG_KEYSECURE, 0, 2); // may have data
                            carve_statement (&eq, "set-cookie", GG_KEYEQUALSHORT, 1, 1);
                            carve_stmt_obj (&mtext, true, true);
                            make_mem(&exp);
                            make_mem(&path);
                            make_mem(&eq);
                            make_mem(&samesite);
                            char *secc = opt_clause(secure, "\"Secure; \"", "\"\"", GG_DEFSTRING);
                            char *httpc = opt_clause(nohttponly, "\"\"", "\"HttpOnly; \"", GG_DEFSTRING);
                            //
                            check_var (&mtext, GG_DEFSTRING);
                            check_var (&eq, GG_DEFSTRING);
                            check_var (&exp, GG_DEFSTRING);
                            check_var (&path, GG_DEFSTRING);
                            check_var (&samesite, GG_DEFSTRING);
                            check_var (&httpc, GG_DEFSTRING);
                            check_var (&secc, GG_DEFSTRING);


                            // enforce that Strict is the default for SameSite and HttpOnly is the default
                            // set-cookie creates its own memory, so no need to increase refcount of mtext
                            oprintf("gg_set_cookie (gg_get_config()->ctx.req, %s, %s, %s, %s, %s, %s, %s);\n", mtext, eq,
                                path == NULL ? "NULL" : path, exp == NULL ? "NULL" : exp, samesite == NULL ? "\"Strict\"" : samesite,
                                httpc, secc);

                            gg_free(secc);
                            gg_free(httpc);
                            if (comma == NULL) break; else mtext = comma;
                        }

                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "get-cookie", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;
                        while (1) 
                        {
                            char *comma = find_keyword (mtext, GG_KEYCOMMA, 0);
                            carve_statement (&comma, "get-cookie", GG_KEYCOMMA, 0, 1); // separate entries with comma first, so
                                                                                      // this find/carve MUST come before the rest below!
                                                                                      //
                            char *eq = find_keyword (mtext, GG_KEYEQUALSHORT, 0);
                            if (eq != NULL) carve_statement (&eq, "get-cookie", GG_KEYEQUALSHORT, 1, 1);
                            carve_stmt_obj (&mtext, true, false);
                            make_mem(&eq);

                            define_statement (&mtext, GG_DEFSTRING, false); // exact length set via strdup in gg_find_cookie
                            check_var (&eq, GG_DEFSTRING);

                            // memory assigned is created in gg_find_cookie, so it has refcount of 0 to begin with, just like
                            // any other statement
                            oprintf("%s = gg_find_cookie (gg_get_config()->ctx.req, %s, NULL, NULL, NULL);\n", mtext, eq); 
                            if (comma == NULL) break; else mtext = comma;
                        }

                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "get-param", &mtext, &msize, 0, &gg_is_inline)) != 0)
                    {
                        GG_GUARD
                        i = newI;

                        while (1) 
                        {
                            char *comma = find_keyword (mtext, GG_KEYCOMMA, 0);
                            carve_statement (&comma, "get-param", GG_KEYCOMMA, 0, 1); // separate entries with comma first, so
                                                                                      // this find/carve MUST come before the rest below!
                                                                                      //
                            char *type = find_keyword (mtext, GG_KEYTYPE, 1);
                            carve_statement (&type, "get-param", GG_KEYTYPE, 0, 1);
                            if (type != NULL) trimit (type); // type has to match
                            carve_stmt_obj (&mtext, true, false);
                            save_param (mtext);

                            gg_num t;
                            if (type == NULL) define_statement (&mtext, (t=GG_DEFSTRING), false);
                            else define_statement (&mtext, (t=typeid(type)), false);
                            //
                            // No need to check_var when define_statement is with "true" as the second argument, since it
                            // means the variable is created of that type for sure, and it can't have been or will be created before or after.
                            //
                            //
                            {
                                oprintf("{ typeof(%s) _gg_param = " , mtext);
                                if (t == GG_DEFNUMBER) oprintf ("*(gg_num*)");
                                else if (t == GG_DEFBOOL) oprintf ("*(bool*)");
                                oprintf("gg_get_input_param (_gg_aprm_%s, %ld);\n", mtext, t);
                                //
                                oprintf ("%s = _gg_param;}\n", mtext);
                                if (cmp_type(t,GG_DEFSTRING)) oprintf ("gg_mem_add_ref (%s);\n", mtext);
                            }
                            //
                            // Make vim recognize type names
                            //
                            // GG_KEY_T_STRING GG_KEY_T_BOOL GG_KEY_T_NUMBER GG_KEY_T_MESSAGE GG_KEY_T_SPLITSTRING GG_KEY_T_HASH GG_KEY_T_ARRAYSTRING GG_KEY_T_ARRAYNUMBER GG_KEY_T_ARRAYBOOL GG_KEY_T_TREE GG_KEY_T_TREECURSOR GG_KEY_T_FIFO GG_KEY_T_LIFO GG_KEY_T_LIST GG_KEY_T_ENCRYPT GG_KEY_T_FILE GG_KEY_T_SERVICE 

                            // we used to forbid non-string/bool/number in non-call-handler. But we should be able to create say index in call-handler and pass it back
                            // to caller. In golfrt.c, we error out if such param with such type is not found.
                            // if ((t != GG_DEFNUMBER && t != GG_DEFBOOL && t != GG_DEFSTRING) && !gg_is_sub) gg_report_error ("get-param type [%s] can only be used within call-handler", typename(t));

                            if (comma == NULL) break; else mtext = comma;
                        }

                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "set-param", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;

                        while (1) 
                        {
                            char *comma = find_keyword (mtext, GG_KEYCOMMA, 0);
                            carve_statement (&comma, "get-param", GG_KEYCOMMA, 0, 1); // separate entries with comma first, so
                                                                                      // this find/carve MUST come before the rest below!
                                                                                      //
                            char *eq = find_keyword (mtext, GG_KEYEQUALSHORT, 0);

                            carve_statement (&eq, "set-param", GG_KEYEQUALSHORT, 0, 1);
                            carve_stmt_obj (&mtext, true, false);
                            if (gg_is_valid_param_name(mtext, false, false) != 1) gg_report_error( "parameter name [%s] is not valid, it can have only alphanumeric characters and underscores, and must start with an alphabet character", mtext);
                            save_param (mtext);

                            if (eq == NULL) eq = gg_strdup (mtext); // if no =, assume it's the variable with the same name
                            make_mem(&eq);
                            gg_num type = check_var (&eq, GG_DEFUNKN);
                            if (type == GG_DEFUNKN) gg_report_error (GG_VAR_NOT_EXIST, eq);


     
                            oprintf("gg_set_input (_gg_aprm_%s, ", mtext);
                            if (cmp_type(type, GG_DEFNUMBER)) oprintf ("&(%s)", eq);
                            else if (cmp_type(type, GG_DEFBOOL)) oprintf ("((%s)?&gg_true:&gg_false)", eq);
                            else oprintf ("%s", eq);
                            oprintf (", %ld);\n", type);

                            if (comma == NULL) break; else mtext = comma;
                        }


                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "end-handler", &mtext, &msize, 1, &gg_is_inline)) != 0 ||
                        (newI1=recog_statement(line, i, "%%", &mtext, &msize, 1, &gg_is_inline)) != 0)  
                    {
                        //No checking if this is within a block, since this is function ending
                        //GG_GUARD
                        i = newI + newI1;
                        carve_stmt_obj (&mtext, false, false);

                        if (!done_handler)
                        {
                            gg_report_error( "end-handler without begin-handler found");
                        }
                        if (gg_single_file && done_end_handler)
                        {
                            gg_report_error( "end-handler found the second time");
                        }
                        done_end_handler = true;
                        oprintf("}\n");
                        if (open_curly != 0) gg_report_error ("Imbalance in blocks opening and closing {}");
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "after-handler", &mtext, &msize, 1, &gg_is_inline)) != 0)
                        {
                        //GG_GUARD
                        i = newI;
                        carve_stmt_obj (&mtext, false, false);
                        match_file (file_name, "after_handler");
                        if (after_ended || after_started) gg_report_error ("after-handler already found");
                        after_started = true;
                        done_handler = true; // avoid no request handler implemented error
                        oprintf ("void after_handler () {\n");
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "before-handler", &mtext, &msize, 1, &gg_is_inline)) != 0)
                        {
                        //GG_GUARD
                        i = newI;
                        carve_stmt_obj (&mtext, false, false);
                        match_file (file_name, "before_handler");
                        if (before_ended || before_started) gg_report_error ("before-handler already found");
                        before_started = true;
                        done_handler = true; // avoid no request handler implemented error
                        oprintf ("void before_handler () {\n");
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "end-after-handler", &mtext, &msize, 1, &gg_is_inline)) != 0)
                        {
                        //GG_GUARD
                        i = newI;
                        carve_stmt_obj (&mtext, false, false);
                        if (!after_started || after_ended) gg_report_error ("found end-after-handler without after-handler to begin with, or already found prior to here");
                        after_ended = true;
                        oprintf ("}\n");
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "end-before-handler", &mtext, &msize, 1, &gg_is_inline)) != 0)
                        {
                        //GG_GUARD
                        i = newI;
                        carve_stmt_obj (&mtext, false, false);
                        if (!before_started || before_ended) gg_report_error ("found end-before-handler without before-handler to begin with, or already found prior to here");
                        before_ended = true;
                        oprintf ("}\n");
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "begin-handler", &mtext, &msize, 0, &gg_is_inline)) != 0 || 
                        (newI1=recog_statement(line, i, "%%", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        //No checking if this is within a block, since this is function ending
                        //GG_GUARD
                        i = newI+newI1;

                        open_curly = 0;

                        // do not allow splitting of begin-handler, because this allows us to quickly get .h file which contains
                        // all prototypes of handlers in gg (in gen_src) - without this we'd have to make a mess and detect \ and concatenate lines in bash
                        if (line_split) gg_report_error ("begin-handler (or %%) statement cannot be split on multiple lines");

                        if (gg_single_file && done_handler)
                        {
                            gg_report_error( ".golf file can have only a single begin-handler");
                        }
                        char *priv = find_keyword (mtext, GG_KEYPRIVATE, 0);
                        char *pub = find_keyword (mtext, GG_KEYPUBLIC, 0);
                        carve_statement (&priv, "begin-handler", GG_KEYPRIVATE, 0, 0);
                        carve_statement (&pub, "begin-handler", GG_KEYPUBLIC, 0, 0);


                        done_handler = true;
                        gg_num mlen = carve_stmt_obj (&mtext, true, false);
                        static char reqname[GG_MAX_REQ_NAME_LEN];

                        if (mtext[0] != '/') gg_report_error ("request path must start with forward slash ('/')");
                        if (priv != NULL && pub != NULL) gg_report_error("Service handler [%s] cannot be both public and private", mtext);
                        // this handles what if neither public nor private specified
                        if (priv == NULL && pub == NULL)
                        {
                            if (gg_public == false) priv="private"; // private handler is by default
                            else pub = "public";  // this is if gg_public is true, which is --public in gg, so then it's public
                        }

                        char *p = mtext; // because gg_decorate_path() will set it to NULL if there's /
                        char decres = gg_decorate_path (reqname, sizeof(reqname), &p, mlen);
                        // 1 means good hierarchical path, reqname is it; or 3 means no /, so reqname is a copy of mtext
                        if (decres != 1) gg_report_error( "request path in begin-handler is not valid");
                        gg_num plen = strlen (reqname); // otherwise whitespace at the end would cause mismatch
                        gg_trim (reqname, &plen, false);

                        if (gg_is_valid_param_name(reqname, false, false) != 1) gg_report_error( "request path in begin-handler is not valid, it can have only alphanumeric characters, hyphens, underscores and forward slashes, and must start with an alphabet character");

                        if (is_reserved(reqname)) gg_report_error ("begin-handler name cannot be a C reserved word [%s]", reqname);

                        match_file (file_name, reqname);

                        oprintf ("void %s () {\n", reqname);
                        if (priv != NULL)
                        {
                            oprintf ("if (!gg_get_config()->ctx.req->sub) gg_report_error(\"This service [%s] can only be called as call-handler. If you wish to call this handler from an outside caller, either use 'public' in begin-handler, or to make all handlers public, use '--public' with '-q' when making the application.\");\n", mtext);
                            gg_is_sub = true;
                        }

                        continue;
                    }
                    else if ((newI=recog_statement (line, i, "set-bool", &mtext, &msize, 0, &gg_is_inline)) != 0)
                    {
                        GG_GUARD
                        i = newI;
                        char *eq = find_keyword (mtext, GG_KEYEQUALSHORT, 0);
                        char *ps = find_keyword (mtext, GG_KEYPROCESSSCOPE, 0);
                        char *fcond = find_keyword (mtext, GG_KEYFROMCONDITION, 0);
                        carve_statement (&eq, "set-bool", GG_KEYEQUALSHORT, 0, 1);
                        carve_statement (&ps, "set-bool", GG_KEYPROCESSSCOPE, 0, 0);
                        carve_statement (&fcond, "set-bool", GG_KEYFROMCONDITION, 0, 1);
                        carve_stmt_obj (&mtext, true, false);
                        gg_num is_def;
                        // GG_KEYAND0 GG_KEYOR0 GG_KEYNOT0 
                        // if defined, it's always false
                        if (ps != NULL) is_def = define_statement (&mtext, GG_DEFBOOLSTATIC, false);
                        else is_def = define_statement (&mtext, GG_DEFBOOL, false);

                        // this is if not defined, and no equal, set to false
                        static char empty[] = "false"; //[] so trim will work on it in check_var
                        if (eq == NULL && fcond == NULL) eq = empty;

                        if (ps && is_def != 1) gg_report_error ("process-scope can only be used when variable is created");

                        // check correctness
                        if (fcond == NULL)
                        {
                            trimit (eq);
                            check_var (&eq, GG_DEFBOOL);
                        }

                        //
                        static gg_num gg_pscope = 0;
                        if (ps)
                        {
                            // with process scope, initialization happens only once, like with static, except we do assignment
                            // to be able to "initialize" with a variable and also conditionally with if-true
                            oprintf("static bool _gg_nstat%ld = true;\n", gg_pscope);
                            oprintf("if (_gg_nstat%ld) {\n_gg_nstat%ld = false;\n", gg_pscope, gg_pscope);
                        }
                        if (eq != NULL) oprintf ("%s = %s;\n", mtext, eq);
                        else if (fcond != NULL) 
                        {
                            // GG_KEYEVERY GG_KEYNOTEVERY GG_KEYEQUAL GG_KEYNOTEQUAL GG_KEYOR GG_KEYAND GG_KEYLESSERTHAN GG_KEYLESSEREQUAL GG_KEYGREATERTHAN GG_KEYGREATEREQUAL GG_KEYCONTAIN GG_KEYNOTCONTAIN GG_KEYCASEINSENSITIVE GG_KEYLENGTH
                            char ifs[300];
                            snprintf(ifs, sizeof(ifs), "%s=(", mtext);
                            parse_cond (fcond, "", ifs, ");\n");
                        }
                        if (ps)
                        {
                            // with process scope, initialization happens only once, like with static, except we do assignment
                            // to be able to "initialize" with a variable and also conditionally with if-true
                            if (eq) {oprintf("}\n"); gg_pscope++;}
                        }
                        continue;
                    }
                    else if ((newI=recog_statement (line, i, "set-number", &mtext, &msize, 0, &gg_is_inline)) != 0)
                    {
                        GG_GUARD
                        i = newI;
                        char *eq = find_keyword (mtext, GG_KEYEQUALSHORT, 0);
                        char *ps = find_keyword (mtext, GG_KEYPROCESSSCOPE, 0);
                        carve_statement (&eq, "set-number", GG_KEYEQUALSHORT, 0, 1); // not mandatory because when add/multiply-by used, there's no =
                        carve_statement (&ps, "set-number", GG_KEYPROCESSSCOPE, 0, 0);
                        carve_stmt_obj (&mtext, true, false);
                        gg_num is_def;
                        // by default always 0
                        if (ps != NULL) is_def = define_statement (&mtext, GG_DEFNUMBERSTATIC, false);
                        else is_def = define_statement (&mtext, GG_DEFNUMBER, false);

                        // if not defined, default is 0
                        static char empty[] = "0"; //[] so trim will work on it in check_var
                        if (eq == NULL) eq = empty;

                        //
                        check_var (&eq, GG_DEFNUMBER);


                        if (ps && is_def != 1) gg_report_error ("process-scope can only be used when variable is created");


                        //
                        // Rules:
                        // process-scope must be when variable is created


                        static gg_num gg_pscope = 0;

                        // always init to zero when created
                        // init process-scope with guardian bool only if there's equal, otherwise it's set to 0
                        if (ps)
                        {
                            // with process scope, initialization happens only once, like with static, except we do assignment
                            // to be able to "initialize" with a variable and also conditionally with if-true
                            oprintf("static bool _gg_nstat%ld = true;\n", gg_pscope);
                            oprintf("if (_gg_nstat%ld) {\n_gg_nstat%ld = false;\n", gg_pscope, gg_pscope);
                        }
                        // based on above exclusions, only one will fire
                        // this is in any case, with or without variable creation or if-true
                        // no assignment if process scope (static)
                        oprintf ("%s = %s;\n", mtext, eq);

                        if (ps)
                        {
                            // with process scope, initialization happens only once, like with static, except we do assignment
                            // to be able to "initialize" with a variable and also conditionally with if-true
                            if (eq) {oprintf("}\n"); gg_pscope++;}
                        }
                        continue;
                    }
                    else if ((newI=recog_statement (line, i, "new-string", &mtext, &msize, 0, &gg_is_inline)) != 0)
                    {
                        GG_GUARD
                        i = newI;
                        char *len = find_keyword (mtext, GG_KEYLENGTH, 0);
                        carve_statement (&len, "new-string", GG_KEYLENGTH, 1, 1);
                        carve_stmt_obj (&mtext, true, false);
                        define_statement (&mtext, GG_DEFSTRING, false); // correct as it uses GG_EMPTY_STRING for empty init
                        gg_num st;
                        gg_num size=gg_str2num (len, 0, &st);
                        if (st == GG_OKAY && size <= 0) gg_report_error ("Length of a new string cannot be lesser than or equal to 0");
                        if (st == GG_OKAY && size < 1024) 
                        {
                            char *cs = gg_malloc (size+3); // +1 for end null, +2 for 2 quotes, plus 1 charm
                            cs[0] = '"';
                            gg_num i;
                            for (i = 1; i < size+1; i++) cs[i] = ' ';
                            cs[i] = '"';
                            cs[i+1] = 0;
                            make_mem (&cs);
                            // no need to add reference, this is new memory
                            oprintf ("%s=%s;\n%s[0]=0;\n", mtext,cs,mtext); // string is empty each time execution passes through new-string
                        }
                        else
                        {
                            // check_var is here and above, since it produces a variable, so len would be _gg_d...
                            check_var (&len, GG_DEFNUMBER);
                            oprintf ("%s = (char*)gg_malloc ((%s)+1);\n%s[0] = 0;\n", mtext, len, mtext); // again, string is empty with each passing through
                        }
                        continue;
                    }
                    else if ((newI=recog_statement (line, i, "set-string", &mtext, &msize, 0, &gg_is_inline)) != 0)
                    {
                        // TODO: set-unquoted-string which is the same as set-string but there's not surrounding quotes
                        // and all quotes and any escape chars are not honored. This will NOT have "x" "y" capability for
                        // concatenation of literals for obvious reason that there is no notion of a literal boundary other than
                        // the end of the line.
                        GG_GUARD
                        i = newI;
                        char *unq = find_keyword (mtext, GG_KEYUNQUOTED, 0);
                        char *eq = find_keyword (mtext, GG_KEYEQUALSHORT, 0);
                        char *ps = find_keyword (mtext, GG_KEYPROCESSSCOPE, 0);
                        char *len = find_keyword (mtext, GG_KEYLENGTH, 0);
                        carve_statement (&eq, "set-string", GG_KEYEQUALSHORT, 0, 1);
                        carve_statement (&ps, "set-string", GG_KEYPROCESSSCOPE, 0, 0);
                        carve_statement (&unq, "set-string", GG_KEYUNQUOTED, 0, 0);
                        carve_statement (&len, "set-string", GG_KEYLENGTH, 0, 1);
                        carve_stmt_obj (&mtext, true, false);
                        gg_num is_def;

                        if (len != NULL)
                        {
                            if (eq != NULL || ps != NULL || unq != NULL) gg_report_error ("Other clauses cannot be used with 'length' clause'");
                            check_var (&len, GG_DEFNUMBER);
                            if (gg_is_valid_param_name(mtext, false, false) != 1) gg_report_error( "Improper string variable name [%s]", mtext);
                            check_var (&mtext, GG_DEFSTRING);
                            oprintf ("{ gg_num _gg_s_id = gg_mem_get_id (%s); if ((%s)>=0 || (%s)<=gg_mem_get_len(_gg_s_id)) {gg_mem_set_len (_gg_s_id, (%s)+1);(%s)[%s] = 0;} else gg_report_error (\"String length [%%ld] is outside the string range [0-%%ld]\", %s, gg_mem_get_len(_gg_s_id));}\n", mtext, len, len, len, mtext, len, len);
                            continue;
                        }

                        // check for str[]
                        bool num_assign = false;
                        char *ind;
                        char *open_p;
                        if ((open_p = strchr (mtext, '[')) != NULL)
                        {
                            num_assign = true;
                            // this is str[index]=number, so this expression needs
                            // to evaluate to number
                            bool sub_found;
                            bool sub_err;
                            check_exp (mtext, &sub_found, &sub_err);
                            if (sub_err || !sub_found) gg_report_error ("Error in syntax (%s)", mtext); 
                            *open_p = 0; // now mtext has lost [], so it can be checked below for type
                            ind = open_p + 1; // ind starts after leading [
                            // now remove last ] so that ind (index of string) is correct for code generation
                            gg_num il = strlen (ind);
                            while (il > 0 && ind[il-1] != ']') il--;
                            if (il > 0) ind[il-1] = 0;
                        }

                        if (ps != NULL) is_def = define_statement (&mtext, GG_DEFSTRINGSTATIC, false); 
                        else is_def = define_statement (&mtext, GG_DEFSTRING, false); // correct as it uses GG_EMPTY_STRING for empty init,
                                                                              // otherwise just assigns pointer, so acts as an alias

                        if (is_def == 1 && num_assign) gg_report_error ("Cannot use [] assignment in new variable");
                        if (eq == NULL && num_assign) gg_report_error ("Must use equal ('=') with [] assignment");
                        if (ps && num_assign) gg_report_error ("Cannot use process-scope with [] assignment");

                        if (num_assign)
                        {
                            check_var (&eq, GG_DEFNUMBER);
                            oprintf ("if (gg_mem_get_len(gg_mem_get_id(%s))-1 < (%s) || (%s) < 0) gg_report_error (\"Cannot access byte [%%ld] in string\", (long)(%s));\n", mtext, ind, ind, ind);
                            oprintf ("%s[%s] = (%s);\n", mtext, ind, eq);
                            continue;
                        }

                        
                        // Rules:
                        // process-scope can only go with equal and if variable is created here
                        // equal can only go with process scope and if-true
                        
                        if (ps && is_def != 1) gg_report_error ("process-scope can only be used when variable is created");
                        static char empty[] = "GG_EMPTY_STRING"; //[] so trim will work on it in check_var
                        if (eq == NULL) eq = empty;


                        static gg_num gg_pscope = 0;
                        //
                        // First, create variable out of constant if it is a constant
                        // Also, check assigned value is a string in general
                        // Do not quote GG_EMPTY_STRING
                        //
                        if (unq && eq != empty) make_unq (&eq); 
                        else 
                        {
                            make_mem (&eq);
                        }
                        check_var (&eq, GG_DEFSTRING);

                        //
                        // start if process-scope and equal used together
                        //
                        if (ps) // assigning value, this goes under implied do-once
                        {
                            // with process scope, initialization happens only once, like with static, except we do assignment
                            // to be able to "initialize" with a variable and also conditionally with if-true
                            oprintf("static bool _gg_nstat%ld = true;\n", gg_pscope);
                            oprintf("if (_gg_nstat%ld) {\n_gg_nstat%ld = false;\n", gg_pscope, gg_pscope);
                        }

                        // if string is assigned to process-scope string, it becomes process-scoped too

                        // X(process) = Y(process), X is now process
                        // X(process) = Y(not process), X is now process
                        // X(not process) = Y(process), X is now process
                        // X(not process) = Y(not process), X is NOT process
                        // Basically if either one is process, the lvalue is process.
                        gg_num mtext_p = check_var (&mtext, GG_DEFUNKN);
                        if (mtext_p == GG_DEFSTRINGSTATIC) 
                        {
                            oprintf ("gg_mem_set_process(%s, %s, true, false);\n", mtext, eq);
                            oprintf ("gg_mem_replace_and_return (%s, %s);\n", mtext, eq);
                        }
                        oprintf ("%s = %s;\n", mtext, eq);
                        oprintf ("gg_mem_add_ref (%s);\n", eq); // must be after gg_mem_set_process, because it becomes process-scoped
                                                                // there's nothing to do

                        gg_num eq_p = check_var (&eq, GG_DEFUNKN);
                        if (eq_p == GG_DEFSTRINGSTATIC || mtext_p == GG_DEFSTRINGSTATIC)
                        {
                            //
                            //Set process flag for variable, so it's not automatically deleted at } or return-handler or such
                            //
                            bool is_process = true;
                            find_var (mtext, NULL, NULL, NULL, &is_process); // set 'mtext' to process in symbol table so we don't auto delete out of scope
                            //
                            //
                        }

                    
                        // finish if process-scope
                        if (ps)
                        {
                            // with process scope, initialization happens only once, like with static, except we do assignment
                            // to be able to "initialize" with a variable and also conditionally with if-true
                            oprintf("}\n"); 
                            gg_pscope++;
                        }
                        continue;
                    }
                    else if (((newI=recog_statement (line, i, "if-true", &mtext, &msize, 0, &gg_is_inline)) != 0)
                        || ((newI1=recog_statement (line, i, "else-if", &mtext, &msize, 1, &gg_is_inline)) != 0)
                        || ((newI2=recog_statement (line, i, "else-if", &mtext, &msize, 0, &gg_is_inline)) != 0))
                    {
                         GG_GUARD
                        i = newI+newI1+newI2;
                        bool else_if = false;
                        bool else_alone = false;
                        bool is_if = false;
                        if (newI1+newI2 != 0 ) else_if = true; else is_if = true;
                        if (newI1 != 0) else_alone = true;
                        // NOTE:the following comment must not be deleted, it provides coloring information for vim plugin!!!!
                        // GG_KEYEQUAL GG_KEYNOTEQUAL GG_KEYLESSERTHAN  GG_KEYLESSEREQUAL GG_KEYGREATERTHAN GG_KEYEVERY GG_KEYNOTEVERY GG_KEYGREATEREQUAL GG_KEYCONTAIN GG_KEYNOTCONTAIN GG_KEYCASEINSENSITIVE GG_KEYLENGTH

                        // if if-string, then open_ifs is a +1 from previous if-string (or 0 if the first), so open_ifs is correct
                        if (is_if) { if_nest[open_ifs].else_done = false; }


                        if (!else_alone)
                        {
                            if (is_if) 
                            {
                                if (open_ifs==0) last_line_string_closed = lnum;
                                open_ifs++; // this is the only place where open_ifs increases
                                if (open_ifs >= GG_MAX_OTHER) gg_report_error( "too many sub-ifs nested with if-true");
                            }
                            else
                            {
                                carve_stmt_obj (&mtext, true, true); // for || && ( or ) there is no mtext (cm), only for not-equal or equal 
                                                             // we check for this kind of correctness down here
                                // if else-string, then open_ifs is +1 from when if-string was done, so open_ifs-1 is correct
                                if (open_ifs <= 0) gg_report_error( "else-if found without an open if-true");
                                // open_ifs gets decreased by 1 (closing prior to else) and then increases by one (in opening after else) so stays the same
                                if (if_nest[open_ifs-1].else_done) gg_report_error( "else-if without a condition must be the last one after if-true statement");
                                if_nest[open_ifs-1].num_elses++; // one more "} else {"
                            }
                            char *cm = mtext;
                            char before_ifs[200];
                            snprintf(before_ifs, sizeof(before_ifs), "%s", else_if == 1 ? "} else { ":"");
                            // this comment needed for getvim to pick up these keywords
                            // GG_KEYEVERY GG_KEYNOTEVERY GG_KEYEQUAL GG_KEYNOTEQUAL GG_KEYOR GG_KEYAND GG_KEYLESSERTHAN GG_KEYLESSEREQUAL GG_KEYGREATERTHAN GG_KEYGREATEREQUAL GG_KEYCONTAIN GG_KEYNOTCONTAIN GG_KEYCASEINSENSITIVE GG_KEYLENGTH
                            parse_cond (cm, before_ifs, "if (", ") {\n");
                        }
                        else
                        {
                            carve_stmt_obj (&mtext, false, false); // do not expect anything for else-if without condition
                            // if else-string, then open_ifs is +1 from when if-string was done, so open_ifs-1 is correct
                            if (open_ifs <= 0) gg_report_error( "else-if found without an open if-true");
                            // open_ifs gets decreased by 1 (closing prior to else) and then increases by one (in opening after else) so stays the same
                            //
                            // At this point, open_ifs is 1 greater than the level, so on level 0 (original if-string), open_ifs is 1,
                            // on level 1 (one-nested if-string), open_ifs is 2 etc. That's because if-string will do +1, and for else-string
                            // it remains so. By the time code reaches here, it's +1
                            oprintf("} else {\n");
                            // here checking is done with open_ifs-1, because open_ifs is now +1 of what the level really is
                            if (if_nest[open_ifs-1].else_done) gg_report_error( "else-if without a condition can be used only once");
                            if_nest[open_ifs-1].else_done = true;
                        }

                        continue;
                    }
                    else if ((newI=recog_statement (line, i, "end-if", &mtext, &msize, 1, &gg_is_inline)) != 0)
                    {
                        GG_GUARD
                        i = newI;
                        if (open_ifs <= 0) gg_report_error( "end-if found without an open if statement");
                        open_ifs--; // this is the only place where open_ifs decreases
                        // check done on real open_ifs, after --
                        if_nest[open_ifs].else_done = false;
                        oprintf("}\n");
                        // close "} else {"s 
                        gg_num k;
                        for (k = 0; k < if_nest[open_ifs].num_elses; k++) oprintf("}\n");
                        if_nest[open_ifs].num_elses = 0;
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "delete-string", &mtext, &msize, 0, &gg_is_inline)) != 0)
                    {
                        GG_GUARD
                        i = newI;
                        carve_stmt_obj (&mtext, true, false);
                        //
                        check_var (&mtext, GG_DEFSTRING);
                        //
                        // Only string actually defined as process-scope can be freed as such (or rather it's 
                        // ref-count decreased, and then if 0, freed). Even references to it cannot be freed like this.
                        // Ordinary memory can be freed ONLY if never referenced.
                        //
                        gg_num mtext_p = check_var (&mtext, GG_DEFUNKN);
                        if (mtext_p == GG_DEFSTRINGSTATIC) oprintf("_gg_free ((void*)(%s),0);\n", mtext);
                        else oprintf("gg_free_int ((void*)(%s));\n", mtext);
                        oprintf("%s=GG_EMPTY_STRING;\n", mtext);
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "write-message", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;
                        char *key = find_keyword (mtext, GG_KEYKEY, 1);
                        char *value = find_keyword (mtext, GG_KEYVALUE, 1);
                        carve_statement (&key, "write-message", GG_KEYKEY, 1, 1);
                        carve_statement (&value, "write-message", GG_KEYVALUE, 1, 1);
                        carve_stmt_obj (&mtext, true, false);
                        make_mem(&key);
                        make_mem(&value);
                        //
                        check_var (&key, GG_DEFSTRING);
                        check_var (&value, GG_DEFSTRING);
                        check_var (&mtext, GG_DEFMSG);

                        // key and value are copied to message, so no ref increment
                        oprintf ("gg_write_msg(%s, %s, %s);\n",mtext, key, value); 

                        continue;

                    }
                    else if ((newI=recog_statement(line, i, "read-message", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;
                        char *key = find_keyword (mtext, GG_KEYKEY, 1);
                        char *value = find_keyword (mtext, GG_KEYVALUE, 1);
                        char *st = find_keyword (mtext, GG_KEYSTATUS, 1);

                        carve_statement (&key, "read-message", GG_KEYKEY, 0, 1);
                        carve_statement (&value, "read-message", GG_KEYVALUE, 0, 1);
                        carve_statement (&st, "read-message", GG_KEYSTATUS, 0, 1);
                        carve_stmt_obj (&mtext, true, false);
                        define_statement (&key, GG_DEFSTRING, false);
                        define_statement (&value, GG_DEFSTRING, false);
                        define_statement (&st, GG_DEFNUMBER, false);
                        //
                        check_var (&mtext, GG_DEFMSG);

                        oprintf ("_gg_st=gg_read_msg(%s, %s%s%s, %s%s%s);\n", mtext, key!=NULL?"&(":"",key!=NULL?key:"NULL",key!=NULL?")":"",value!=NULL?"&(":"",value!=NULL?value:"NULL",value!=NULL?")":"");
                        if (st != NULL) oprintf("%s=_gg_st;\n",st); else oprintf("if (_gg_st != GG_OKAY) gg_report_error (\"%s\", \"%s\", (gg_num)%ld, _gg_st);\n", GG_STMT_FAIL_CHECK, undecorate (src_file_name+GG_BLD_DIR_OFFSET), lnum);
                        continue;

                    }
                    else if ((newI=recog_statement(line, i, "get-message", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;

                        char *to = find_keyword (mtext, GG_KEYTO, 1);
                        carve_statement (&to, "get-message", GG_KEYTO, 1, 1);
                        carve_stmt_obj (&mtext, true, false);
                        define_statement (&to, GG_DEFSTRING, false); // exact length set below with gg_get_msg
                        //
                        check_var (&mtext, GG_DEFMSG);

                        oprintf ("{ char *_gg_gmsg =gg_get_msg(%s);\n",mtext); 
                        oprintf ("%s = _gg_gmsg;\n", to);
                        oprintf ("gg_mem_add_ref (%s);\n", to);
                        oprintf("}\n");


                        continue;

                    }
                    else if ((newI=recog_statement(line, i, "new-message", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;

                        char *from = find_keyword (mtext, GG_KEYFROM, 1);
                        carve_statement (&from, "new-message", GG_KEYFROM, 0, 1);
                        carve_stmt_obj (&mtext, true, false);
                        make_mem(&from);
                        define_statement (&mtext, GG_DEFMSG, true);
                        //
                        check_var (&from, GG_DEFSTRING);

                        oprintf ("%s=gg_new_msg(%s);\n",mtext, from==NULL?"NULL":from); 

                        continue;

                    }
                    else if ((newI=recog_statement(line, i, "read-split", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;
                        char *from = find_keyword (mtext, GG_KEYFROM, 1);
                        char *to = find_keyword (mtext, GG_KEYTO, 1);
                        char *st = find_keyword (mtext, GG_KEYSTATUS, 1);

                        carve_statement (&to, "read-split", GG_KEYTO, 1, 1);
                        carve_statement (&from, "read-split", GG_KEYFROM, 1, 1);
                        carve_statement (&st, "read-split", GG_KEYSTATUS, 0, 1);
                        carve_stmt_obj (&mtext, true, false);

                        define_statement (&to, GG_DEFSTRING, false); // exact length set below with strdup/GG_EMPTY_STRING
                        define_statement (&st, GG_DEFNUMBER, false);
                        //
                        check_var (&mtext, GG_DEFNUMBER);
                        check_var (&from, GG_DEFBROKEN);

                        // check split-string isn't NULL
                        oprintf ("if ((%s) != NULL && %s >= 1 && %s <= (%s)->num_pieces) {\n", from, mtext, mtext, from);
                        if (st != NULL) oprintf("%s=GG_OKAY;\n", st);
                        oprintf ("%s = (%s)->pieces[(%s)-1];gg_mem_add_ref(%s);\n", to, from, mtext, to);
                        oprintf("} else {\n");
                        if (st != NULL) oprintf("%s=GG_ERR_OVERFLOW;\n", st);
                        else oprintf("gg_report_error (\"%s\", \"%s\", (gg_num)%ld, (gg_num)GG_ERR_OVERFLOW);\n", GG_STMT_FAIL_CHECK, undecorate (src_file_name+GG_BLD_DIR_OFFSET), lnum);
                        oprintf ("%s=GG_EMPTY_STRING;}\n",to); 

                        continue;

                    }
                    else if ((newI=recog_statement(line, i, "split-string", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;
                        char *to = find_keyword (mtext, GG_KEYTO, 1);
                        char *with = find_keyword (mtext, GG_KEYWITH, 1);
                        char *del = find_keyword (mtext, GG_KEYDELETE, 1);
                        char *count = find_keyword (mtext, GG_KEYCOUNT, 1);

                        if (del != NULL && (to != NULL || with != NULL)) gg_report_error ("Only one of 'to'/'with' or 'delete' can be used in split-string");
                        if (del == NULL && to == NULL) gg_report_error ("At least one of 'to'/'with' or 'delete' must be used in split-string");
                        if (count != NULL && to == NULL) gg_report_error ("'count' can only be used with 'to' in split-string");

                        if (to != NULL) 
                        {
                            carve_statement (&to, "split-string", GG_KEYTO, 1, 1);
                            carve_statement (&with, "split-string", GG_KEYWITH, 1, 1);
                            carve_statement (&count, "split-string", GG_KEYCOUNT, 0, 1);
                            make_mem(&with);
                        }
                        if (del != NULL) 
                        {
                            carve_statement (&del, "split-string", GG_KEYDELETE, 0, 1);
                            carve_stmt_obj (&mtext, false, false);
                        }
                        else carve_stmt_obj (&mtext, true, true);

                        if (to != NULL) define_statement (&to, GG_DEFBROKEN, true);
                        if (count != NULL) define_statement (&count, GG_DEFNUMBER, false);
                        //
                        if (del == NULL) check_var (&mtext, GG_DEFSTRING);
                        check_var (&with, GG_DEFSTRING);
                        check_var (&del, GG_DEFBROKEN);

                        if (to != NULL) oprintf("gg_break_down (%s, %s, &(%s));\n", mtext, with, to);
                        if (count != NULL) oprintf ("%s= (%s)->num_pieces;\n", count, to); // must be after gg_break_down to fill in the result

                        if (del != NULL) oprintf ("gg_delete_break_down (&(%s));\n", del);

                        continue;

                    }
                    else if ((newI=recog_statement(line, i, "unused-var", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        i = newI;


                        carve_stmt_obj (&mtext, true, false);
                        while (1) 
                        {
                            char *comma = find_keyword (mtext, GG_KEYCOMMA, 0);
                            carve_statement (&comma, "unused-var", GG_KEYCOMMA, 0, 1); // separate entries with comma first, so
                                                                                      // this find/carve MUST come before the rest below!
                                                                                      //

                            check_var (&mtext, GG_DEFUNKN); // just check variable is known, we don't care about type
                            oprintf("GG_UNUSED (%s);\n", mtext);
                            if (comma == NULL) break; else mtext = comma;
                        }

                        continue;

                    }
                    else if ((newI=recog_statement(line, i, "new-list", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;

                        char *process = find_keyword (mtext, GG_KEYPROCESSSCOPE, 1);

                        carve_statement (&process, "new-list", GG_KEYPROCESSSCOPE, 0, 0);

                        carve_stmt_obj (&mtext, true, false);
                        if (process) define_statement (&mtext, GG_DEFLISTSTATIC, true); else define_statement (&mtext, GG_DEFLIST, true);
                        oprintf("gg_mem_process=%s;\n", process?"true":"false");
                        oprintf ("gg_list_init (&(%s), %s);\n", mtext, process?"true":"false");
                        oprintf("gg_mem_process=false;\n");

                        continue;

                    }
                    else if ((newI=recog_statement(line, i, "write-list", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;

                        char *key = find_keyword (mtext, GG_KEYKEY, 1);
                        char *value = find_keyword (mtext, GG_KEYVALUE, 1);
                        char *append = find_keyword (mtext, GG_KEYAPPEND, 1);


                        carve_statement (&key, "write-list", GG_KEYKEY, 1, 1);
                        carve_statement (&value, "write-list", GG_KEYVALUE, 1, 1);
                        carve_statement (&append, "write-list", GG_KEYAPPEND, 0, 2);
                        carve_stmt_obj (&mtext, true, false);
                        make_mem (&key);
                        make_mem (&value);

                        char *appendc = opt_clause(append, "true", "false", GG_DEFBOOL);
                        //
                        check_var (&mtext, GG_DEFLIST);
                        check_var (&key, GG_DEFSTRING);
                        check_var (&value, GG_DEFSTRING);
                        check_var (&appendc, GG_DEFBOOL);

                        oprintf("gg_mem_process=(%s)->process;\n", mtext);
                        oprintf ("gg_list_store (%s, %s, (void*)%s, %s);\n", mtext, key, value, appendc);
                        oprintf("gg_mem_process=false;\n");
                        continue;

                    }
                    else if ((newI=recog_statement(line, i, "read-list", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;

                        char *key = find_keyword (mtext, GG_KEYKEY, 1);
                        char *value = find_keyword (mtext, GG_KEYVALUE, 1);
                        char *upd = find_keyword (mtext, GG_KEYUPDATEVALUE, 1);
                        char *updkey = find_keyword (mtext, GG_KEYUPDATEKEY, 1);
                        char *st = find_keyword (mtext, GG_KEYSTATUS, 1);

                        carve_statement (&key, "read-list", GG_KEYKEY, 1, 1);
                        carve_statement (&value, "read-list", GG_KEYVALUE, 1, 1);
                        carve_statement (&upd, "read-list", GG_KEYUPDATEVALUE, 0, 1);
                        carve_statement (&updkey, "read-list", GG_KEYUPDATEKEY, 0, 1);
                        carve_statement (&st, "read-list", GG_KEYSTATUS, 0, 1);

                        carve_stmt_obj (&mtext, true, false);
                        make_mem(&upd);
                        make_mem(&updkey);

                        define_statement (&key, GG_DEFSTRING, false); // exact length okay this is just pointer aliasing
                        define_statement (&value, GG_DEFSTRING, false); // exact length okay this is just pointer aliasing

                        define_statement (&st, GG_DEFNUMBER, false);
                        //
                        check_var (&mtext, GG_DEFLIST);
                        check_var (&upd, GG_DEFSTRING);
                        check_var (&updkey, GG_DEFSTRING);

                        oprintf("gg_mem_process=(%s)->process;\n", mtext);
                        oprintf ("{ char *_gg_list_k; char *_gg_list_v; gg_list_retrieve (%s, &(_gg_list_k), (void**)&(_gg_list_v));\n", mtext);
                        if (upd || updkey || st) oprintf("if ((%s)->curr != NULL) {\n", mtext);
                        //
                        oprintf ("%s = _gg_list_k; gg_mem_add_ref (%s);\n", key, key);
                        oprintf ("%s = _gg_list_v; gg_mem_add_ref (%s);\n", value, value);
                    
                        // name,data surely existed, so 0 in add_ref
                        // 0 is fine in gg_mem_add_ref because in read-list an element can never be NULL
                        if (updkey) 
                        {
                            oprintf("gg_mem_set_process ((%s)->curr->name, %s, false, true); \n", mtext, updkey);
                            oprintf ("gg_mem_replace_and_return ((%s)->curr->name, %s);\n", mtext, updkey);
                            oprintf("(%s)->curr->name = (%s);\n", mtext, updkey); 
                        }
                        //
                        // 0 is fine in gg_mem_add_ref because in read-list an element can never be NULL
                        if (upd) 
                        {
                            oprintf("gg_mem_set_process ((%s)->curr->data, %s, false, true);\n", mtext, upd);
                            oprintf ("gg_mem_replace_and_return ((%s)->curr->data, %s);\n", mtext, upd);
                            oprintf("(%s)->curr->data = (%s) ;\n", mtext, upd); 
                        }
                        //
                        if (st) oprintf("     (%s) = GG_OKAY;\n", st); 
                        if (upd || updkey || st) oprintf("}\n");

                        if (upd || updkey || st)
                        {
                            if (st) oprintf ("else { (%s) = GG_ERR_EXIST; }\n", st); else oprintf("else { gg_report_error (\"%s\", \"%s\", (gg_num)%ld, (gg_num)GG_ERR_EXIST);}\n", GG_STMT_FAIL_CHECK, undecorate (src_file_name+GG_BLD_DIR_OFFSET), lnum);
                        }
                        oprintf("}\n");
                        oprintf("gg_mem_process=false;\n");

                        continue;

                    }
                    else if ((newI=recog_statement(line, i, "position-list", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;
                        char *first = find_keyword (mtext, GG_KEYFIRST, 1);
                        char *last = find_keyword (mtext, GG_KEYLAST, 1);
                        char *end = find_keyword (mtext, GG_KEYEND, 1);
                        char *prev = find_keyword (mtext, GG_KEYPREVIOUS, 1);
                        char *next = find_keyword (mtext, GG_KEYNEXT, 1);
                        char *st = find_keyword (mtext, GG_KEYSTATUS, 1);

                        carve_statement (&first, "position-list", GG_KEYFIRST, 0, 0);
                        carve_statement (&last, "position-list", GG_KEYLAST, 0, 0);
                        carve_statement (&end, "position-list", GG_KEYEND, 0, 0);
                        carve_statement (&prev, "position-list", GG_KEYPREVIOUS, 0, 0);
                        carve_statement (&next, "position-list", GG_KEYNEXT, 0, 0);
                        carve_statement (&st, "position-list", GG_KEYSTATUS, 0, 1);
                        carve_stmt_obj (&mtext, true, false);

                        define_statement (&st, GG_DEFNUMBER, false);
                        //
                        check_var (&mtext, GG_DEFLIST);

                        int where;
                        if (first != NULL) where = GG_LIST_FIRST;
                        else if (last != NULL) where = GG_LIST_LAST;
                        else if (end != NULL) where = GG_LIST_END;
                        else if (prev != NULL) where = GG_LIST_PREV;
                        else if (next != NULL) where = GG_LIST_NEXT;
                        else gg_report_error ("Must specify either first, last, end, previous or next clause");
                        int opt = (first!=NULL?1:0)+(last!=NULL?1:0)+(end!=NULL?1:0)+(prev!=NULL?1:0)+(next!=NULL?1:0);
                        if (opt > 1) gg_report_error ("Must specify only one of first, last, end, previous or next clauses");

                        static gg_num pos_st = 0;
                        //
                        // Position linked list to an element. If 'first' is true, then position to first one,
                        // otherwise to last one. If 'end' is true, then the end (which is NULL, beyond last one). 
                        // Inserting is always done by pushing current forward and inserting into its place. Deleting is done
                        // by deleting the current. Reading reads current. If 'item' is not NULL, position to this (it must have been
                        // saved previously from somewhere in the list). If 'prev' is true, then position to previous element, otherwise next.
                        // Returns GG_OKAY if okay, GG_ERR_EMPTY if list is empty
                        //
                        oprintf("gg_mem_process=(%s)->process;\n", mtext);
                        oprintf ("gg_num _gg_list_pos_st%ld;\n", pos_st);
                        oprintf ("if ((%s)->num_of == 0) _gg_list_pos_st%ld = GG_ERR_EXIST; else {\n", mtext, pos_st);
                        if (where == GG_LIST_FIRST) oprintf ("(%s)->curr = (%s)->first; _gg_list_pos_st%ld = GG_OKAY;\n", mtext, mtext, pos_st);
                        else if (where == GG_LIST_LAST) oprintf ("(%s)->curr = (%s)->last;_gg_list_pos_st%ld = GG_OKAY;\n", mtext, mtext, pos_st);
                        else if (where == GG_LIST_END) oprintf ("(%s)->curr = NULL; _gg_list_pos_st%ld = GG_OKAY;\n", mtext, pos_st);
                        else if (where == GG_LIST_PREV)
                        {
                            oprintf ("if ((%s)->curr == (%s)->first) _gg_list_pos_st%ld = GG_ERR_EXIST; else { (%s)->curr = ((%s)->curr ? (%s)->curr->prev:(%s)->last); _gg_list_pos_st%ld = GG_OKAY; }\n", mtext, mtext, pos_st, mtext,mtext,mtext,mtext, pos_st);
                        }
                        else if (where == GG_LIST_NEXT)
                        {
                            oprintf ("if ((%s)->curr == NULL) _gg_list_pos_st%ld = GG_ERR_EXIST; else { (%s)->curr = (%s)->curr->next; _gg_list_pos_st%ld = GG_OKAY; }\n", mtext, pos_st, mtext, mtext, pos_st);
                        }
                        oprintf ("}\n");
                        if (st) oprintf ("(%s) = _gg_list_pos_st%ld;\n", st, pos_st);
                        else oprintf("if (_gg_list_pos_st%ld != GG_OKAY) gg_report_error (\"%s\", \"%s\", (gg_num)%ld, _gg_list_pos_st%ld);\n", pos_st, GG_STMT_FAIL_CHECK, undecorate (src_file_name+GG_BLD_DIR_OFFSET), lnum, pos_st);
                        
                        pos_st++;
                        oprintf("gg_mem_process=false;\n");


                        continue;

                    }
                    else if ((newI=recog_statement(line, i, "purge-list", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;

                        carve_stmt_obj (&mtext, true, false);
                        //
                        check_var (&mtext, GG_DEFLIST);
                        oprintf("gg_mem_process=(%s)->process;\n", mtext);
                        oprintf ("gg_list_purge (&(%s));\n", mtext);
                        oprintf("gg_mem_process=false;\n");

                        continue;

                    }
                    else if ((newI=recog_statement(line, i, "delete-list", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;
                        char *st = find_keyword (mtext, GG_KEYSTATUS, 1);
                        carve_statement (&st, "delete-list", GG_KEYSTATUS, 0, 1);
                        carve_stmt_obj (&mtext, true, false);
                        define_statement (&st, GG_DEFNUMBER, false);
                        //
                        check_var (&mtext, GG_DEFLIST);
                        oprintf("gg_mem_process=(%s)->process;\n", mtext);
                        oprintf ("_gg_st=gg_list_delete (%s);\n", mtext);
                        if (st != NULL) oprintf("%s=_gg_st;\n", st); else oprintf("if (_gg_st != GG_OKAY) gg_report_error (\"%s\", \"%s\", (gg_num)%ld, _gg_st);\n", GG_STMT_FAIL_CHECK, undecorate (src_file_name+GG_BLD_DIR_OFFSET), lnum);
                        oprintf("gg_mem_process=false;\n");

                        continue;

                    }
                    else if ((newI=recog_statement(line, i, "new-fifo", &mtext, &msize, 0, &gg_is_inline)) != 0  
                     || (newI1=recog_statement(line, i, "new-lifo", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI+newI1;
                        bool lifo = (newI1 != 0);

                        carve_stmt_obj (&mtext, true, false);
                        define_statement (&mtext, lifo?GG_DEFLIFO:GG_DEFFIFO, true);
                        oprintf ("gg_store_init (&(%s));\n", mtext);

                        continue;

                    }
                    else if ((newI=recog_statement(line, i, "write-fifo", &mtext, &msize, 0, &gg_is_inline)) != 0
                     || (newI1=recog_statement(line, i, "write-lifo", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI+newI1;
                        bool lifo = (newI1 != 0);

                        char *key = find_keyword (mtext, GG_KEYKEY, 1);
                        char *value = find_keyword (mtext, GG_KEYVALUE, 1);

                        carve_statement (&key, "write-fifo", GG_KEYKEY, 1, 1);
                        carve_statement (&value, "write-fifo", GG_KEYVALUE, 1, 1);
                        carve_stmt_obj (&mtext, true, false);
                        make_mem(&key);
                        make_mem(&value);
                        //
                        check_var (&mtext, lifo?GG_DEFLIFO:GG_DEFFIFO);
                        check_var (&key, GG_DEFSTRING);
                        check_var (&value, GG_DEFSTRING);


                        oprintf ("gg_store%s (%s, %s, (void*)%s);\n", lifo?"_l":"", mtext, key, value);

                        continue;

                    }
                    else if ((newI=recog_statement(line, i, "rewind-fifo", &mtext, &msize, 0, &gg_is_inline)) != 0
                     || (newI1=recog_statement(line, i, "rewind-lifo", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI+newI1;
                        bool lifo = (newI1 != 0);

                        carve_stmt_obj (&mtext, true, false);
                        //
                        check_var (&mtext, lifo?GG_DEFLIFO:GG_DEFFIFO);
                        oprintf ("gg_rewind (%s);\n", mtext);

                        continue;

                    }
                    else if ((newI=recog_statement(line, i, "delete-fifo", &mtext, &msize, 0, &gg_is_inline)) != 0
                     || (newI1=recog_statement(line, i, "delete-lifo", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI+newI1;
                        bool lifo = (newI1 != 0);

                        carve_stmt_obj (&mtext, true, false);
                        //
                        check_var (&mtext, lifo?GG_DEFLIFO:GG_DEFFIFO);
                        oprintf ("gg_fifo_delete (%s);\n", mtext);

                        continue;

                    }
                    else if ((newI=recog_statement(line, i, "purge-fifo", &mtext, &msize, 0, &gg_is_inline)) != 0
                     || (newI1=recog_statement(line, i, "purge-lifo", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI+newI1;
                        bool lifo = (newI1 != 0);

                        carve_stmt_obj (&mtext, true, false);
                        //
                        check_var (&mtext, lifo?GG_DEFLIFO:GG_DEFFIFO);
                        oprintf ("gg_purge (&(%s));\n", mtext);

                        continue;

                    }
                    else if ((newI=recog_statement(line, i, "read-fifo", &mtext, &msize, 0, &gg_is_inline)) != 0
                     || (newI1=recog_statement(line, i, "read-lifo", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI+newI1;
                        bool lifo = (newI1 != 0);

                        char *key = find_keyword (mtext, GG_KEYKEY, 1);
                        char *value = find_keyword (mtext, GG_KEYVALUE, 1);
                        char *st = find_keyword (mtext, GG_KEYSTATUS, 1);

                        carve_statement (&key, "read-fifo", GG_KEYKEY, 1, 1);
                        carve_statement (&value, "read-fifo", GG_KEYVALUE, 1, 1);
                        carve_statement (&st, "read-fifo", GG_KEYSTATUS, 0, 1);
                        carve_stmt_obj (&mtext, true, false);

                        define_statement (&key, GG_DEFSTRING, false); // exact length okay this is just pointer aliasing in gg_retrieve
                        define_statement (&value, GG_DEFSTRING, false); // exact length okay this is just pointer aliasing in gg_retrieve
                        define_statement (&st, GG_DEFNUMBER, false);
                        //
                        check_var (&mtext, lifo?GG_DEFLIFO:GG_DEFFIFO);

                        oprintf ("_gg_st=gg_retrieve (%s, &(%s), (void**)&(%s));\n", mtext, key, value);
                        if (st != NULL) oprintf("%s=_gg_st;\n",st); else oprintf("if (_gg_st != GG_OKAY) gg_report_error (\"%s\", \"%s\", (gg_num)%ld, _gg_st);\n", GG_STMT_FAIL_CHECK, undecorate (src_file_name+GG_BLD_DIR_OFFSET), lnum);
                        continue;

                    }
                    else if ((newI=recog_statement(line, i, "write-file", &mtext, &msize, 0, &gg_is_inline)) != 0)
                    {
                        GG_GUARD
                        i = newI;
                        char *from = find_keyword (mtext, GG_KEYFROM, 1);
                        char *append = find_keyword (mtext, GG_KEYAPPEND, 1);
                        char *length = find_keyword (mtext, GG_KEYLENGTH, 1);
                        char *status = find_keyword (mtext, GG_KEYSTATUS, 1);
                        char *pos = find_keyword (mtext, GG_KEYPOSITION, 1);
                        char *fileid = find_keyword (mtext, GG_KEYFILEID, 1);

                        carve_statement (&from, "write-file", GG_KEYFROM, 1, 1);
                        carve_statement (&append, "write-file", GG_KEYAPPEND, 0, 2);
                        carve_statement (&length, "write-file", GG_KEYLENGTH, 0, 1);
                        carve_statement (&status, "write-file", GG_KEYSTATUS, 0, 1);
                        carve_statement (&pos, "write-file", GG_KEYPOSITION, 0, 1);
                        carve_statement (&fileid, "write-file", GG_KEYFILEID, 0, 1);
                        if (fileid == NULL) carve_stmt_obj (&mtext, true, true); else carve_stmt_obj (&mtext, false, false); 
                        make_mem(&from);

                        define_statement (&status, GG_DEFNUMBER, false);
                        char *appendc = opt_clause(append, "1", "0", GG_DEFNUMBER);
                        //
                        if (fileid == NULL) check_var (&mtext, GG_DEFSTRING);
                        check_var (&from, GG_DEFSTRING);
                        check_var (&appendc, GG_DEFNUMBER);
                        check_var (&length, GG_DEFNUMBER);
                        check_var (&pos, GG_DEFNUMBER);
                        if (fileid != NULL) check_var (&fileid, GG_DEFFILE);


                        if (mtext[0] != 0 && fileid!=NULL) gg_report_error( "you can specify either file name or file-id but not both in write-file statement");

                        if (append != NULL && pos!=NULL) gg_report_error( "'append' and 'position' cannot both be in write-file statement");

                        if (length==NULL) length="0";

                        if (fileid != NULL) oprintf("_gg_st=gg_write_file_id (*((%s)->f), %s, %s, %s, %s, %s);\n", fileid, from, length, appendc, pos == NULL ? "0":pos, pos==NULL?"0":"1");
                        else oprintf("_gg_st=gg_write_file (%s, %s, %s, %s, %s, %s);\n",mtext, from, length, appendc, pos == NULL ? "0":pos, pos==NULL?"0":"1");
                        // _gg_st here is the number of bytes written, so 0 or more is okay, less than 0 is error
                        if (status != NULL) oprintf("%s=_gg_st;\n",status); else oprintf("if (_gg_st < 0) gg_report_error (\"%s\", \"%s\", (gg_num)%ld, _gg_st);\n", GG_STMT_FAIL_CHECK, undecorate (src_file_name+GG_BLD_DIR_OFFSET), lnum);
                        gg_free(appendc);
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "read-file", &mtext, &msize, 0, &gg_is_inline)) != 0)
                    {
                        GG_GUARD
                        i = newI;
                        char *read_from = mtext;
                        char *read_to = find_keyword (mtext, GG_KEYTO, 1);
                        char *status = find_keyword (mtext, GG_KEYSTATUS, 1);
                        char *eof = find_keyword (mtext, GG_KEYENDOFFILE, 1);
                        char *pos = find_keyword (mtext, GG_KEYPOSITION, 1);
                        char *length = find_keyword (mtext, GG_KEYLENGTH, 1);
                        char *fileid = find_keyword (mtext, GG_KEYFILEID, 1);

                        carve_statement (&read_to, "read-file", GG_KEYTO, 1, 1);
                        carve_statement (&status, "read-file", GG_KEYSTATUS, 0, 1);
                        carve_statement (&eof, "read-file", GG_KEYENDOFFILE, 0, 1);
                        carve_statement (&pos, "read-file", GG_KEYPOSITION, 0, 1);
                        carve_statement (&length, "read-file", GG_KEYLENGTH, 0, 1);
                        carve_statement (&fileid, "read-file", GG_KEYFILEID, 0, 1);
                        if (fileid == NULL) carve_stmt_obj (&read_from, true, true); else carve_stmt_obj (&read_from, false, false);

                        define_statement (&eof, GG_DEFBOOL, false);
                        define_statement (&status, GG_DEFNUMBER, false);
                        define_statement (&read_to, GG_DEFSTRING, false); // exact length set in gg_read_file
                        //
                        if (fileid == NULL) check_var (&read_from, GG_DEFSTRING);
                        check_var (&pos, GG_DEFNUMBER);
                        check_var (&length, GG_DEFNUMBER);
                        if (fileid != NULL) check_var (&fileid, GG_DEFFILE);

                        if (length == NULL) length="0";

                        if (fileid != NULL) oprintf("_gg_st=gg_read_file_id (*((%s)->f), &(%s), %s, %s, %s, &_gg_st_bool);\n", fileid, read_to, pos==NULL?"0":pos, length, pos!=NULL?"true":"false");
                        else oprintf("_gg_st=gg_read_file (%s, &(%s), %s, %s, &_gg_st_bool);\n", read_from, read_to, pos==NULL?"0":pos, length);
                        if (eof != NULL) oprintf ("%s=_gg_st_bool;\n", eof);
                        // _gg_st is negative if error, otherwise size of data read which can be zero
                        if (status != NULL) oprintf("%s=_gg_st;\n",status); else oprintf("if (_gg_st < 0) gg_report_error (\"%s\", \"%s\", (gg_num)%ld, _gg_st);\n", GG_STMT_FAIL_CHECK, undecorate (src_file_name+GG_BLD_DIR_OFFSET), lnum);
                        continue;

                    }
                    else if ((newI=recog_statement(line, i, "get-hash", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;

                        char *len = find_keyword (mtext, GG_KEYLENGTH, 1);
                        char *size = find_keyword (mtext, GG_KEYHASHSIZE, 1);
                        char *reads = find_keyword (mtext, GG_KEYAVERAGEREADS, 1);


                        carve_statement (&size, "get-hash", GG_KEYHASHSIZE, 0, 1);
                        carve_statement (&len, "get-hash", GG_KEYLENGTH, 0, 1);
                        carve_statement (&reads, "get-hash", GG_KEYAVERAGEREADS, 0, 1);
                        carve_stmt_obj (&mtext, true, false);
                        define_statement (&len, GG_DEFNUMBER, false);
                        define_statement (&size, GG_DEFNUMBER, false);
                        define_statement (&reads, GG_DEFNUMBER, false);
                        //
                        check_var (&mtext, GG_DEFHASH);

                        if (size == NULL && len == NULL && reads == NULL) gg_report_error( "one of 'length', 'size' or 'average-reads' must be in get-hash statement");

                        oprintf("gg_mem_process=(%s)->process;\n", mtext);
                        if (len !=NULL) oprintf("%s=gg_total_hash (%s);\n", len, mtext);
                        if (size !=NULL) oprintf("%s=gg_hash_size (%s);\n", size, mtext);
                        if (reads !=NULL) oprintf("%s=gg_hash_reads (%s);\n", reads, mtext);
                        oprintf("gg_mem_process=false;\n");
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "purge-hash", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;

                        carve_stmt_obj (&mtext, true, false);
                        //
                        check_var (&mtext, GG_DEFHASH);
                        oprintf("gg_mem_process=(%s)->process;\n", mtext);
                        oprintf("gg_delete_hash (&(%s), true);\n", mtext);
                        oprintf("gg_mem_process=false;\n");
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "resize-hash", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;
                        char *size = find_keyword (mtext, GG_KEYHASHSIZE, 1);

                        carve_statement (&size, "resize-hash", GG_KEYHASHSIZE, 1, 1);
                        carve_stmt_obj (&mtext, true, false);
                        //
                        check_var (&mtext, GG_DEFHASH);
                        check_var (&size, GG_DEFNUMBER);

                        oprintf("gg_mem_process=(%s)->process;\n", mtext);
                        oprintf("gg_resize_hash (&(%s), %s);\n", mtext, size);
                        oprintf("gg_mem_process=false;\n");
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "new-array", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;
                        char *maxsize = find_keyword (mtext, GG_KEYMAXSIZE, 1);
                        char *process = find_keyword (mtext, GG_KEYPROCESSSCOPE, 1);
                        char *type = find_keyword (mtext, GG_KEYTYPE, 1);

                        carve_statement (&maxsize, "new-array", GG_KEYMAXSIZE, 0, 1);
                        carve_statement (&process, "new-array", GG_KEYPROCESSSCOPE, 0, 0);
                        carve_statement (&type, "new-array", GG_KEYTYPE, 0, 1);
                        if (type != NULL) trimit (type); // type has to match
                        else type ="string";
                        gg_num at = typeid(type);
                        if (!cmp_type (at, GG_DEFSTRING) && !cmp_type(at, GG_DEFNUMBER) && !cmp_type(at, GG_DEFBOOL)) gg_report_error("Unknown type [%s] in new-array", type);
                        // GG_KEY_T_STRING GG_KEY_T_BOOL GG_KEY_T_NUMBER  // see get-param if ever adding other types as arrays
                        carve_stmt_obj (&mtext, true, false);
                        // check 'type' clause
                        if (cmp_type(at, GG_DEFSTRING)) 
                        {
                            if (process) define_statement (&mtext, GG_DEFARRAYSTRINGSTATIC, true); else define_statement (&mtext, GG_DEFARRAYSTRING, true);
                        }
                        else if (cmp_type(at, GG_DEFNUMBER)) 
                        {
                            if (process) define_statement (&mtext, GG_DEFARRAYNUMBERSTATIC, true); else define_statement (&mtext, GG_DEFARRAYNUMBER, true);
                        }
                        if (cmp_type(at, GG_DEFBOOL)) 
                        {
                            if (process) define_statement (&mtext, GG_DEFARRAYBOOLSTATIC, true); else define_statement (&mtext, GG_DEFARRAYBOOL, true);
                        }
                        //
                        check_var (&maxsize, GG_DEFNUMBER);

                        oprintf("gg_mem_process=%s;\n", process?"true":"false");
                        if (cmp_type(at, GG_DEFSTRING)) oprintf("%s = gg_new_arraystring (%s, %s);\n", mtext, maxsize?maxsize:"0", process?"true":"false");
                        else if (cmp_type(at, GG_DEFNUMBER)) oprintf("%s = gg_new_arraynumber (%s, %s);\n", mtext, maxsize?maxsize:"0", process?"true":"false");
                        else if (cmp_type(at, GG_DEFBOOL)) oprintf("%s = gg_new_arraybool (%s, %s);\n", mtext, maxsize?maxsize:"0", process?"true":"false");
                        oprintf("gg_mem_process=false;\n");
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "new-hash", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;
                        char *size = find_keyword (mtext, GG_KEYHASHSIZE, 1);
                        char *process = find_keyword (mtext, GG_KEYPROCESSSCOPE, 1);

                        carve_statement (&size, "new-hash", GG_KEYHASHSIZE, 0, 1);
                        carve_statement (&process, "new-hash", GG_KEYPROCESSSCOPE, 0, 0);
                        carve_stmt_obj (&mtext, true, false);
                        if (process) define_statement (&mtext, GG_DEFHASHSTATIC, true); else define_statement (&mtext, GG_DEFHASH, true);
                        //
                        check_var (&size, GG_DEFNUMBER);

                        oprintf("gg_mem_process=%s;\n", process?"true":"false");
                        oprintf("gg_create_hash (&(%s), %s, NULL, %s);\n", mtext, size?size:"10", process?"true":"false");
                        oprintf("gg_mem_process=false;\n");
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "write-array", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;
                        char *key = find_keyword (mtext, GG_KEYKEY, 1);
                        char *val = find_keyword (mtext, GG_KEYVALUE, 1);
                        char *oldd = find_keyword (mtext, GG_KEYOLDVALUE, 1);

                        carve_statement (&key, "write-array", GG_KEYKEY, 1, 1);
                        carve_statement (&val, "write-array", GG_KEYVALUE, 1, 1);
                        carve_statement (&oldd, "write-array", GG_KEYOLDVALUE, 0, 1);
                        //
                        carve_stmt_obj (&mtext, true, false);
                        gg_num at = check_var (&mtext, GG_DEFUNKN);
                        if (!cmp_type (at, GG_DEFARRAYSTRING) && !cmp_type(at, GG_DEFARRAYNUMBER) && !cmp_type(at, GG_DEFARRAYBOOL)) gg_report_error("Unknown type [%s] in new-array", typename(at));
                        //
                        if (cmp_type (at, GG_DEFARRAYSTRING)) define_statement (&oldd, GG_DEFSTRING, false); // exact length okay this is just pointer aliasing in gg_add_hash with GG_EMPTY_STRING
                        else if (cmp_type (at, GG_DEFARRAYNUMBER)) define_statement (&oldd, GG_DEFNUMBER, false);
                        else if (cmp_type (at, GG_DEFARRAYBOOL)) define_statement (&oldd, GG_DEFBOOL, false);
                        //
                        //
                        check_var (&key, GG_DEFNUMBER);

                        oprintf("gg_mem_process=(%s)->process;\n", mtext);

                        //
                        // In all of deferred assignments (where old value can be used to assign to new value), gg_write_array... will report an error
                        // if key isn't correct. So setting afterwards is okay, because it means the key is correct memory-wise.
                        //
                        if (cmp_type (at, GG_DEFARRAYSTRING)) 
                        {
                            oprintf("gg_write_arraystring (%s, %s, %s%s%s);\n", mtext, key, oldd==NULL?"":"&(", oldd==NULL?"NULL":oldd, oldd==NULL?"":")");
                            make_mem (&val);
                            check_var (&val, GG_DEFSTRING);// here old_val can be used in val expression
                            oprintf ("gg_mem_set_process ((%s)->str[%s], %s, (%s)->process, true);\n", mtext, key, val, mtext);
                            oprintf ("(%s)->str[%s] = (%s)==NULL?GG_EMPTY_STRING:(%s);\n", mtext, key, val, val);
                            oprintf ("gg_mem_replace_and_return (%s, %s);\n", oldd==NULL?"NULL":oldd, val); // val is here just to check if equal to old value, in which case nothing is done; otherwise old-value's is made normal if it were process mem (in which case it's given ref of 1 if it were to go down to 0 so it stays available through the rest of request)
                        }
                        else if (cmp_type (at, GG_DEFARRAYNUMBER)) 
                        {
                            oprintf("gg_write_arraynumber (%s, %s, %s%s%s);\n", mtext, key, oldd==NULL?"":"&(", oldd==NULL?"NULL":oldd, oldd==NULL?"":")");
                            check_var (&val, GG_DEFNUMBER); // here old_val can be used in val expression
                            oprintf ("(%s)->num[%s] = (%s);\n", mtext, key, val);
                        }
                        else if (cmp_type (at, GG_DEFARRAYBOOL)) 
                        {
                            oprintf("gg_write_arraybool (%s, %s, %s%s%s);\n", mtext, key, oldd==NULL?"":"&(", oldd==NULL?"NULL":oldd, oldd==NULL?"":")");
                            check_var (&val, GG_DEFBOOL);
                            oprintf ("(%s)->logic[%s] = %s;\n", mtext, key, val);
                        }
                        oprintf("gg_mem_process=false;\n");
                        // both key and value are now referenced by the index as well as the original variables that put them here
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "purge-array", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;

                        carve_stmt_obj (&mtext, true, false);
                        //
                        gg_num at = check_var (&mtext, GG_DEFUNKN);
                        if (!cmp_type (at, GG_DEFARRAYSTRING) && !cmp_type(at, GG_DEFARRAYNUMBER) && !cmp_type(at, GG_DEFARRAYBOOL)) gg_report_error("Unknown type [%s] in new-array", typename(at));
                        oprintf("gg_mem_process=(%s)->process;\n", mtext);
                        if (cmp_type (at, GG_DEFARRAYSTRING)) oprintf("gg_purge_arraystring (%s);\n", mtext);
                        else if (cmp_type (at, GG_DEFARRAYNUMBER)) oprintf("gg_purge_arraynumber (%s);\n", mtext);
                        else if (cmp_type (at, GG_DEFARRAYBOOL)) oprintf("gg_purge_arraybool (%s);\n", mtext);
                        oprintf("gg_mem_process=false;\n");
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "read-array", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;
                        char *key = find_keyword (mtext, GG_KEYKEY, 1);
                        char *val = find_keyword (mtext, GG_KEYVALUE, 1);
                        char *del = find_keyword (mtext, GG_KEYDELETE0, 1);


                        carve_statement (&key, "read-array", GG_KEYKEY, 1, 1);
                        carve_statement (&val, "read-array", GG_KEYVALUE, 1, 1);
                        carve_statement (&del, "read-array", GG_KEYDELETE0, 0, 2);
                        carve_stmt_obj (&mtext, true, false);

                        gg_num at = check_var (&mtext, GG_DEFUNKN);
                        if (!cmp_type (at, GG_DEFARRAYSTRING) && !cmp_type(at, GG_DEFARRAYNUMBER) && !cmp_type(at, GG_DEFARRAYBOOL)) gg_report_error("Unknown type [%s] in new-array", typename(at));

                        char *delc = opt_clause(del, "true", "false", GG_DEFBOOL);

                        if (cmp_type (at, GG_DEFARRAYSTRING)) define_statement (&val, GG_DEFSTRING, false); // exact length via pointer assignment as a return from gg_find_hash and value param from gg_next_hash
                        else if (cmp_type (at, GG_DEFARRAYNUMBER)) define_statement (&val, GG_DEFNUMBER, false); // exact length via pointer assignment as a return from gg_find_hash and value param from gg_next_hash
                        else if (cmp_type (at, GG_DEFARRAYBOOL)) define_statement (&val, GG_DEFBOOL, false); // exact length via pointer assignment as a return from gg_find_hash and value param from gg_next_hash


                        //
                        check_var (&delc, GG_DEFBOOL);
                        check_var (&key, GG_DEFNUMBER);


                        //
                        // For read-array here, the output value is empty string when not found; for that reason we always get value (_gg_hfind), even when status
                        // is not GG_OKAY
                        //
                        oprintf("{\n");
                        if (cmp_type (at, GG_DEFARRAYSTRING)) oprintf("char *_gg_afind = gg_read_arraystring (%s, %s, %s);\n", mtext, key, delc);
                        else if (cmp_type (at, GG_DEFARRAYNUMBER)) oprintf("gg_num _gg_afind = gg_read_arraynumber (%s, %s, %s);\n", mtext, key, delc);
                        else if (cmp_type (at, GG_DEFARRAYBOOL)) oprintf("bool _gg_afind = gg_read_arraybool (%s, %s, %s);\n", mtext, key, delc);
                        oprintf ("%s = _gg_afind; ", val);
                        if (cmp_type (at, GG_DEFARRAYSTRING)) oprintf ("gg_mem_add_ref(%s);", val); 
                        oprintf("}\n");

                        gg_free(delc);
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "write-hash", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;
                        char *key = find_keyword (mtext, GG_KEYKEY, 1);
                        char *val = find_keyword (mtext, GG_KEYVALUE, 1);
                        char *oldd = find_keyword (mtext, GG_KEYOLDVALUE, 1);
                        char *st = find_keyword (mtext, GG_KEYSTATUS, 1);

                        carve_statement (&key, "write-hash", GG_KEYKEY, 1, 1);
                        carve_statement (&val, "write-hash", GG_KEYVALUE, 1, 1);
                        carve_statement (&oldd, "write-hash", GG_KEYOLDVALUE, 0, 1);
                        carve_statement (&st, "write-hash", GG_KEYSTATUS, 0, 1);
                        carve_stmt_obj (&mtext, true, false);
                        define_statement (&oldd, GG_DEFSTRING, false); // exact length okay this is just pointer aliasing in gg_add_hash with GG_EMPTY_STRING
                        define_statement (&st, GG_DEFNUMBER, false);
                        make_mem (&key);
                        make_mem (&val);
                        //
                        check_var (&mtext, GG_DEFHASH);
                        check_var (&key, GG_DEFSTRING);
                        check_var (&val, GG_DEFSTRING);

                        oprintf("gg_mem_process=(%s)->process;\n", mtext);

                        oprintf("gg_add_hash (%s, %s, NULL, %s, %s%s%s, %s%s%s);\n", mtext, key, val, oldd==NULL?"":"(void**)&(", oldd==NULL?"NULL":oldd, oldd==NULL?"":")", st==NULL?"":"&(", st==NULL?"NULL":st, st==NULL?"":")");
                        oprintf("gg_mem_process=false;\n");
                        // both key and value are now referenced by the index as well as the original variables that put them here
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "read-hash", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;
                        char *key = find_keyword (mtext, GG_KEYKEY, 1);
                        char *val = find_keyword (mtext, GG_KEYVALUE, 1);
                        char *del = find_keyword (mtext, GG_KEYDELETE0, 1);
                        char *st = find_keyword (mtext, GG_KEYSTATUS, 1);
                        char *trav = find_keyword (mtext, GG_KEYTRAVERSE, 1);
                        char *beg = NULL;

                        if (trav != NULL)
                        {
                            beg = find_keyword (mtext, GG_KEYBEGIN, 1);
                        }


                        //
                        // Carving must be after find_keyword
                        //
                        if (trav != NULL)
                        {
                            carve_statement (&beg, "read-hash", GG_KEYBEGIN, 0, 0);
                            carve_statement (&key, "read-hash", GG_KEYKEY, 0, 1);
                            carve_statement (&val, "read-hash", GG_KEYVALUE, 0, 1);
                            carve_statement (&trav, "read-hash", GG_KEYTRAVERSE, 0, 0);
                        }
                        else
                        {
                            carve_statement (&key, "read-hash", GG_KEYKEY, 1, 1);
                            carve_statement (&val, "read-hash", GG_KEYVALUE, 1, 1);
                        }
                        carve_statement (&st, "read-hash", GG_KEYSTATUS, 0, 1);
                        carve_statement (&del, "read-hash", GG_KEYDELETE0, 0, 2);
                        carve_stmt_obj (&mtext, true, false);
                        if (trav == NULL)
                        {
                            make_mem(&key);
                        }

                        char *delc = opt_clause(del, "true", "false", GG_DEFBOOL);

                        define_statement (&val, GG_DEFSTRING, false); // exact length via pointer assignment as a return from gg_find_hash and value param from gg_next_hash
                        define_statement (&st, GG_DEFNUMBER, false);

                        if (trav != NULL)
                        {
                            define_statement (&key, GG_DEFSTRING, false); // exact length okay this is just pointer aliasing in gg_next_hash with GG_EMPTY_STRING
                        }
                        else
                        {
                            // check_var is for INPUT vars only
                            check_var (&key, GG_DEFSTRING);
                        }
                        //
                        check_var (&delc, GG_DEFBOOL);
                        check_var (&mtext, GG_DEFHASH);


                        //
                        // For read-hash here, the output value is empty string when not found; for that reason we always get value (_gg_hfind), even when status
                        // is not GG_OKAY
                        //
                        if (trav == NULL)
                        {
                            oprintf("{ ");
                            oprintf("char *_gg_hfind = gg_find_hash (%s, %s, NULL, %s, &_gg_st);\n", mtext, key, delc);
                            oprintf ("if (_gg_st == GG_OKAY) { %s = _gg_hfind; gg_mem_add_ref(%s);}\n", val, val);
                            if (st==NULL) oprintf("else gg_report_error (\"%s\", \"%s\", (gg_num)%ld, _gg_st);\n", GG_STMT_FAIL_CHECK, undecorate (src_file_name+GG_BLD_DIR_OFFSET), lnum);
                            else oprintf ("%s=_gg_st;\n", st);
                            oprintf("}\n");

                        }
                        else
                        {
                            if ((key == NULL && val != NULL) || (key != NULL && val == NULL)) gg_report_error( "'key' and 'value' must both be in read-hash statement");
                            if (beg != NULL) oprintf ("gg_rewind_hash(%s);\n", mtext);
                            if (key != NULL) 
                            {
                                oprintf("{ ");
                                oprintf("char *_gg_hfind_v; char *_gg_hfind_k=gg_next_hash (%s, (void*)&(_gg_hfind_v), &_gg_st, %s);\n", mtext, delc);
                                //
                                oprintf ("if (_gg_st == GG_OKAY) \n{%s = _gg_hfind_v; gg_mem_add_ref(%s);\n", val, val);
                                oprintf ("%s = _gg_hfind_k; gg_mem_add_ref(%s);\n}\n", key, key);
                                //
                                //
                                // The following two are together: if status is not okay, AND if user didn't use 'status' clause,
                                // then report error
                                //
                                oprintf ("}\n");
                                if (st == NULL) oprintf(" else { gg_report_error (\"%s\", \"%s\", (gg_num)%ld, _gg_st);}\n", GG_STMT_FAIL_CHECK, undecorate (src_file_name+GG_BLD_DIR_OFFSET), lnum); else oprintf ("%s=_gg_st;\n", st);
                                //
                                //
                                //
                            }
                        }
                        gg_free(delc);
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "read-json", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;
                        char *key = find_keyword (mtext, GG_KEYKEY, 1);
                        char *val = find_keyword (mtext, GG_KEYVALUE, 1);
                        char *type = find_keyword (mtext, GG_KEYTYPE, 1);
                        char *next = find_keyword (mtext, GG_KEYNEXT, 1);


                        carve_statement (&key, "read-json", GG_KEYKEY, 0, 1);
                        carve_statement (&val, "read-json", GG_KEYVALUE, 0, 1);
                        carve_statement (&type, "read-json", GG_KEYTYPE, 0, 1);
                        carve_statement (&next, "read-json", GG_KEYNEXT, 0, 0);
                        carve_stmt_obj (&mtext, true, false);

                        define_statement (&key, GG_DEFSTRING, false); 
                        define_statement (&val, GG_DEFSTRING, false); 
                        define_statement (&type, GG_DEFNUMBER, false);

                        check_var (&mtext, GG_DEFJSON);
                        oprintf ("if ((%s) != NULL && ((%s)->node_r < (%s)->node_c)) { \n;\n", mtext, mtext, mtext);
                        if (key != NULL) 
                        {
                            oprintf ("(%s) = (%s)->nodes[(%s)->node_r].name;\n", key, mtext, mtext);
                            oprintf ("gg_mem_add_ref(%s);\n", key);
                        }
                        // if value is asked for, we copy value (lazy approch, faster) and we mark this value as 'alloced' 
                        // so when deleting json, we know to free it
                        // actual allocation is done only once of course, so we don't allocate the same string 10 times for accessing it 10 times
                        if (val != NULL) 
                        {
                            oprintf ("{gg_num _gg_json_node = (%s)->node_r; char *_gg_json_str = (%s)->nodes[_gg_json_node].str = ((%s)->nodes[_gg_json_node].alloced ? (%s)->nodes[_gg_json_node].str : gg_strdup((%s)->nodes[_gg_json_node].str)); (%s)->nodes[_gg_json_node].alloced=true;\n", mtext, mtext, mtext, mtext, mtext, mtext);
                            oprintf ("%s = _gg_json_str; \n", val);
                            oprintf ("gg_mem_add_ref(%s);}\n", val);
                        }
                        if (type != NULL) oprintf ("(%s) = (%s)->nodes[(%s)->node_r].type;\n", type, mtext, mtext);
                        if (next) oprintf ("(%s)->node_r++;\n", mtext);
                        oprintf ("} else { ;\n");
                        if (key != NULL) oprintf ("(%s) = GG_EMPTY_STRING;\n", key);
                        if (val != NULL) oprintf ("(%s) = GG_EMPTY_STRING;\n", val);
                        if (type != NULL) oprintf ("(%s) = GG_JSON_TYPE_NONE;\n", type);
                        oprintf ("}\n");

                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "read-xml", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;
                        char *key = find_keyword (mtext, GG_KEYKEY, 1);
                        char *val = find_keyword (mtext, GG_KEYVALUE, 1);
                        char *next = find_keyword (mtext, GG_KEYNEXT, 1);
                        char *st = find_keyword (mtext, GG_KEYSTATUS, 1);


                        carve_statement (&key, "read-xml", GG_KEYKEY, 0, 1);
                        carve_statement (&val, "read-xml", GG_KEYVALUE, 0, 1);
                        carve_statement (&next, "read-xml", GG_KEYNEXT, 0, 0);
                        carve_statement (&st, "read-xml", GG_KEYSTATUS, 0, 1);
                        carve_stmt_obj (&mtext, true, false);

                        define_statement (&key, GG_DEFSTRING, false); 
                        define_statement (&val, GG_DEFSTRING, false); 
                        define_statement (&st, GG_DEFNUMBER, false); 

                        check_var (&mtext, GG_DEFXML);
                        oprintf ("if ((%s) != NULL && ((%s)->node_r < (%s)->node_c)) { \n;\n", mtext, mtext, mtext);
                        if (key != NULL) 
                        {
                            oprintf ("(%s) = (%s)->nodes[(%s)->node_r].name;\n", key, mtext, mtext);
                            oprintf ("gg_mem_add_ref(%s);\n", key);
                        }
                        if (val != NULL) 
                        {
                            oprintf ("%s = (%s)->nodes[(%s)->node_r].str; \n", val, mtext,mtext);
                            oprintf ("gg_mem_add_ref(%s);\n", val);
                        }
                        if (next) oprintf ("(%s)->node_r++;\n", mtext);
                        if (st != NULL) oprintf ("%s=GG_OKAY;\n", st);
                        oprintf ("} else { ;\n");
                        if (key != NULL) oprintf ("(%s) = GG_EMPTY_STRING;\n", key);
                        if (val != NULL) oprintf ("(%s) = GG_EMPTY_STRING;\n", val);
                        if (st) oprintf ("%s=GG_ERR_EXIST;\n", st);
                        else oprintf("gg_report_error (\"%s\", \"%s\", (gg_num)%ld, (gg_num)GG_ERR_EXIST);\n", GG_STMT_FAIL_CHECK, undecorate (src_file_name+GG_BLD_DIR_OFFSET), lnum);
                        oprintf ("}\n");

                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "json-doc", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;

                        char *noenum = find_keyword (mtext, GG_KEYNOENUM, 1);
                        char *nodec = find_keyword (mtext, GG_KEYNOENCODE, 1);
                        char *to = find_keyword (mtext, GG_KEYTO, 1);
                        char *errp = find_keyword (mtext, GG_KEYERRORPOSITION, 1);
                        char *errl = find_keyword (mtext, GG_KEYERRORLINE, 1);
                        char *errc = find_keyword (mtext, GG_KEYERRORCHAR, 1);
                        char *status = find_keyword (mtext, GG_KEYSTATUS, 1);
                        char *errt = find_keyword (mtext, GG_KEYERRORTEXT, 1);
                        char *len = find_keyword (mtext, GG_KEYLENGTH, 1);
                        char *del = find_keyword (mtext, GG_KEYDELETE, 1);

                        if (del != NULL && to != NULL) gg_report_error ("Only one of 'to' or 'delete' can be used");
                        if (del == NULL && to == NULL) gg_report_error ("At least one of 'to' or 'delete' must be used");
                        if ((nodec != NULL || noenum != NULL || errp != NULL || errl != NULL || errc != NULL || status != NULL || errt != NULL || len != NULL) && to == NULL) gg_report_error ("no other options can be used with 'delete'");

                        if (to != NULL) 
                        {
                            carve_statement (&errt, "json-doc", GG_KEYERRORTEXT, 0, 1);
                            carve_statement (&status, "json-doc", GG_KEYSTATUS, 0, 1);
                            carve_statement (&errp, "json-doc", GG_KEYERRORPOSITION, 0, 1);
                            carve_statement (&errc, "json-doc", GG_KEYERRORCHAR, 0, 1);
                            carve_statement (&errl, "json-doc", GG_KEYERRORLINE, 0, 1);
                            carve_statement (&nodec, "json-doc", GG_KEYNOENCODE, 0, 0);
                            carve_statement (&noenum, "json-doc", GG_KEYNOENUM, 0, 0);
                            carve_statement (&len, "json-doc", GG_KEYLENGTH, 0, 1);
                            carve_statement (&to, "json-doc", GG_KEYTO, 0, 1);
                            carve_stmt_obj (&mtext, true, true);
                            define_statement (&errt, GG_DEFSTRING, false);
                            define_statement (&status, GG_DEFNUMBER, false);
                            define_statement (&errp, GG_DEFNUMBER, false);
                            define_statement (&errl, GG_DEFNUMBER, false);
                            define_statement (&errc, GG_DEFNUMBER, false);
                            define_statement (&to, GG_DEFJSON, true);
                            check_var (&len, GG_DEFNUMBER);
                            check_var (&mtext, GG_DEFSTRING);
                        }
                        if (del != NULL) 
                        {
                            carve_statement (&del, "json-doc", GG_KEYDELETE, 0, 1);
                            carve_stmt_obj (&mtext, false, false);
                            check_var (&del, GG_DEFJSON);
                        }

                        if (del != NULL) oprintf ("gg_del_json (&(%s));\n", del);
                        //


                        //
                        // Look for each option and collect relevant info
                        //
                        if (to != NULL)
                        {
                            oprintf ("gg_set_json (&(%s), %s, %s);\n", to, noenum == NULL?"false":"true", mtext);
                            oprintf ("gg_num gg_json_status_%ld = %s%sgg_json_new ((%s)->data, NULL, (%s), %s, %s%s%s, %s%s%s);\n", json_id, errp == NULL ? "":errp, errp == NULL ? "":"=", to, len == NULL ? "-1" : len, nodec == NULL?"1":"0", errc==NULL?"":"&(",errc==NULL?"NULL":errc,errc==NULL?"":")",  errl==NULL?"":"&(",errl==NULL?"NULL":errl,errl==NULL?"":")");
                            if (status != NULL)
                            {
                                oprintf ("GG_ERR0; %s = (gg_json_status_%ld == -1 ? GG_OKAY : GG_ERR_JSON);\n", status, json_id);
                            }
                            else
                            {
                                oprintf("if (gg_json_status_%ld != -1) gg_report_error (\"%s\", \"%s\", (gg_num)%ld, (gg_num)GG_ERR_JSON);\n", json_id, GG_STMT_FAIL_CHECK, undecorate (src_file_name+GG_BLD_DIR_OFFSET), lnum);
                            }
                            if (errt != NULL)
                            {
                                oprintf ("%s = gg_json_err();\n", errt);
                            }
                            json_id++;
                        }

                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "xml-doc", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;

                        char *to = find_keyword (mtext, GG_KEYTO, 1);
                        char *errl = find_keyword (mtext, GG_KEYERRORLINE, 1);
                        char *errc = find_keyword (mtext, GG_KEYERRORCHAR, 1);
                        char *status = find_keyword (mtext, GG_KEYSTATUS, 1);
                        char *errt = find_keyword (mtext, GG_KEYERRORTEXT, 1);
                        char *len = find_keyword (mtext, GG_KEYLENGTH, 1);
                        char *del = find_keyword (mtext, GG_KEYDELETE, 1);

                        if (del != NULL && to != NULL) gg_report_error ("Only one of 'to' or 'delete' can be used");
                        if (del == NULL && to == NULL) gg_report_error ("At least one of 'to' or 'delete' must be used");
                        if ((errl != NULL || errc != NULL || status != NULL || errt != NULL || len != NULL) && to == NULL) gg_report_error ("no other options can be used with 'delete'");

                        if (to != NULL) 
                        {
                            carve_statement (&errt, "xml-doc", GG_KEYERRORTEXT, 0, 1);
                            carve_statement (&status, "xml-doc", GG_KEYSTATUS, 0, 1);
                            carve_statement (&errc, "xml-doc", GG_KEYERRORCHAR, 0, 1);
                            carve_statement (&errl, "xml-doc", GG_KEYERRORLINE, 0, 1);
                            carve_statement (&len, "xml-doc", GG_KEYLENGTH, 0, 1);
                            carve_statement (&to, "xml-doc", GG_KEYTO, 0, 1);
                            carve_stmt_obj (&mtext, true, true);
                            define_statement (&errt, GG_DEFSTRING, false);
                            define_statement (&status, GG_DEFNUMBER, false);
                            define_statement (&errl, GG_DEFNUMBER, false);
                            define_statement (&errc, GG_DEFNUMBER, false);
                            define_statement (&to, GG_DEFXML, true);
                            check_var (&len, GG_DEFNUMBER);
                            check_var (&mtext, GG_DEFSTRING);
                        }
                        if (del != NULL) 
                        {
                            carve_statement (&del, "xml-doc", GG_KEYDELETE, 0, 1);
                            carve_stmt_obj (&mtext, false, false);
                            check_var (&del, GG_DEFXML);
                        }

                        if (del != NULL) oprintf ("gg_del_xml (&(%s));\n", del);
                        //


                        //
                        // Look for each option and collect relevant info
                        //
                        if (to != NULL)
                        {
                            oprintf ("gg_set_xml (&(%s));\n", to);
                            oprintf ("gg_num gg_xml_status_%ld = gg_xml_new ((%s), (%s), %s%s%s, %s%s%s);\n", xml_id, mtext, len == NULL ? "-1" : len,    errc==NULL?"":"&(",        errc==NULL?"NULL":errc,          errc==NULL?"":")",  errl==NULL?"":"&(",errl==NULL?"NULL":errl,errl==NULL?"":")");
                            if (status != NULL)
                            {
                                oprintf ("GG_ERR0; %s = (gg_xml_status_%ld == -1 ? GG_OKAY : GG_ERR_XML);\n", status, xml_id);
                            }
                            else
                            {
                                oprintf("if (gg_xml_status_%ld != -1) gg_report_error (\"%s\", \"%s\", (gg_num)%ld, (gg_num)GG_ERR_XML);\n", xml_id, GG_STMT_FAIL_CHECK, undecorate (src_file_name+GG_BLD_DIR_OFFSET), lnum);
                            }
                            if (errt != NULL)
                            {
                                oprintf ("%s = gg_xml_err();\n", errt);
                            }
                            xml_id++;
                        }

                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "new-tree", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;

                        //
                        // General principle of memory allocation for objects: when there's a new-xxxx then it can be done only once per request
                        // and reused many times with read/write/purge and similar. Others that don't have new-xxxx, such as split-string, need delete.
                        //

                        char *keyas = find_keyword (mtext, GG_KEYKEYAS, 1);
                        char *unsorted = find_keyword (mtext, GG_KEYUNSORTED, 1);
                        char *process = find_keyword (mtext, GG_KEYPROCESSSCOPE, 1);

                        carve_statement (&keyas, "new-tree", GG_KEYKEYAS, 0, 1);
                        carve_statement (&unsorted, "new-tree", GG_KEYUNSORTED, 0, 0);
                        carve_statement (&process, "new-tree", GG_KEYPROCESSSCOPE, 0, 0);
                        carve_stmt_obj (&mtext, true, false);

                        if (process) define_statement (&mtext, GG_DEFTREESTATIC, true); else define_statement (&mtext, GG_DEFTREE, true);
                        //

                        if (keyas != NULL)
                        {
                            gg_num lm = strlen (keyas);
                            keyas = gg_trim_ptr(keyas,  &lm);
                            if (!strcmp (keyas, "\"positive integer\""))
                            {
                                keyas = "GG_TREE_TYPE_NUM";
                            }
                            else gg_report_error ("Unknown key type [%s]", keyas);
                        } else keyas = "GG_TREE_TYPE_STR";

                        oprintf("gg_mem_process=%s;\n", process?"true":"false");
                        oprintf("%s = gg_tree_create(%s, %s, %s);\n", mtext,keyas, unsorted==NULL?"true":"false", process?"true":"false");
                        oprintf("gg_mem_process=false;\n");
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "get-lifo", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;

                        char *count = find_keyword (mtext, GG_KEYCOUNT, 1);

                        carve_statement (&count, "get-lifo", GG_KEYCOUNT, 0, 1);
                        carve_stmt_obj (&mtext, true, false);

                        if (count != NULL) define_statement (&count, GG_DEFNUMBER, false);
                        //

                        gg_num opt = (count != NULL?1:0);
                        if (opt != 1) gg_report_error ("Must specify a single option");

                        if (count != NULL) oprintf("%s = (%s)->num_of;\n", count, mtext);
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "get-list", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;

                        char *count = find_keyword (mtext, GG_KEYCOUNT, 1);

                        carve_statement (&count, "get-list", GG_KEYCOUNT, 0, 1);
                        carve_stmt_obj (&mtext, true, false);

                        if (count != NULL) define_statement (&count, GG_DEFNUMBER, false);
                        //

                        gg_num opt = (count != NULL?1:0);
                        if (opt != 1) gg_report_error ("Must specify a single option");

                        if (count != NULL) oprintf("%s = (%s)->num_of;\n", count, mtext);
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "get-tree", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;

                        char *count = find_keyword (mtext, GG_KEYCOUNT, 1);
                        char *hops = find_keyword (mtext, GG_KEYHOPS, 1);

                        carve_statement (&count, "get-tree", GG_KEYCOUNT, 0, 1);
                        carve_statement (&hops, "get-tree", GG_KEYHOPS, 0, 1);
                        carve_stmt_obj (&mtext, true, false);

                        if (count != NULL) define_statement (&count, GG_DEFNUMBER, false);
                        if (hops != NULL) define_statement (&hops, GG_DEFNUMBER, false);
                        //

                        gg_num opt = (count != NULL?1:0)+(hops != NULL?1:0);
                        if (opt != 1) gg_report_error ("Must specify a single option");

                        if (count != NULL) oprintf("%s = (%s)->count;\n", count, mtext);
                        if (hops != NULL) oprintf("%s = (%s)->hops;\n", hops, mtext);
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "purge-tree", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;

                        oprintf("gg_mem_process=(%s)->process;\n", mtext);
                        oprintf("gg_tree_purge_f (%s);\n", mtext);
                        oprintf("gg_mem_process=false;\n");
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "write-tree", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;

                        // TODO should add key-length to speed up write if length is known
                        char *key = find_keyword (mtext, GG_KEYKEY, 1);
                        char *value = find_keyword (mtext, GG_KEYVALUE, 1);
                        char *status = find_keyword (mtext, GG_KEYSTATUS, 1);
                        char *ccursor = find_keyword (mtext, GG_KEYNEWCURSOR, 1);

                        carve_statement (&key, "write-tree", GG_KEYKEY, 1, 1);
                        carve_statement (&value, "write-tree", GG_KEYVALUE, 1, 1);
                        carve_statement (&status, "write-tree", GG_KEYSTATUS, 0, 1);
                        carve_statement (&ccursor, "write-tree", GG_KEYNEWCURSOR, 0, 1);
                        carve_stmt_obj (&mtext, true, false);

                        define_statement (&status, GG_DEFNUMBER, false);
                        define_statement (&ccursor, GG_DEFTREECURSOR, true);

                        make_mem (&key);
                        make_mem (&value);
                        //
                        check_var (&mtext, GG_DEFTREE);
                        check_var (&key, GG_DEFSTRING);
                        check_var (&value, GG_DEFSTRING);

                        char cname[100];
                        oprintf ("gg_tree_cursor gg_tree_curs%ld;\n", wcurs);
                        if (ccursor == NULL)
                        {
                            snprintf (cname, sizeof (cname), "_gg_tree_curs%ld", wcurs);
                            oprintf ("gg_tree_cursor *_gg_tree_curs%ld;\n", wcurs);
                            ccursor = cname;
                        }
                        oprintf ("%s = &gg_tree_curs%ld;\n", ccursor, wcurs);
                        oprintf("gg_mem_process=(%s)->process;\n", mtext);
                        oprintf ("gg_tree_insert_f (%s, %s, %s, -1, %s);\n", ccursor, mtext, key, value);
                        oprintf("gg_mem_process=false;\n");
                        if (status != NULL) oprintf ("%s = (%s)->status;\n", status, ccursor);
                        else oprintf("if ((%s)->status != GG_OKAY) gg_report_error (\"%s\", \"%s\", (gg_num)%ld, (%s)->status);\n", ccursor, GG_STMT_FAIL_CHECK, undecorate (src_file_name+GG_BLD_DIR_OFFSET), lnum, ccursor);

                        wcurs++;
                        continue;

                    }
                    else if ((newI=recog_statement(line, i, "read-tree", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;

                        char *key = find_keyword (mtext, GG_KEYEQUAL, 1);
                        char *mink = find_keyword (mtext, GG_KEYMINKEY, 1);
                        char *maxk = find_keyword (mtext, GG_KEYMAXKEY, 1);
                        char *lkey = find_keyword (mtext, GG_KEYLESSER, 1);
                        char *lekey = find_keyword (mtext, GG_KEYLESSEREQUAL, 1);
                        char *gkey = find_keyword (mtext, GG_KEYGREATER, 1);
                        char *gekey = find_keyword (mtext, GG_KEYGREATEREQUAL, 1);
                        char *status = find_keyword (mtext, GG_KEYSTATUS, 1);
                        char *ccursor = find_keyword (mtext, GG_KEYNEWCURSOR, 1);
                        // Begin read-tree 
                        char *getkey = find_keyword (mtext, GG_KEYKEY, 1);
                        char *value = find_keyword (mtext, GG_KEYVALUE, 1);
                        char *upd = find_keyword (mtext, GG_KEYUPDATEVALUE, 1);
                        // End read-tree 

                        carve_statement (&key, "read-tree", GG_KEYEQUAL, 0, 1);
                        carve_statement (&lekey, "read-tree", GG_KEYLESSEREQUAL, 0, 1);
                        carve_statement (&gekey, "read-tree", GG_KEYGREATEREQUAL, 0, 1);
                        carve_statement (&lkey, "read-tree", GG_KEYLESSER, 0, 1);
                        carve_statement (&gkey, "read-tree", GG_KEYGREATER, 0, 1);
                        carve_statement (&mink, "read-tree", GG_KEYMINKEY, 0, 0);
                        carve_statement (&maxk, "read-tree", GG_KEYMAXKEY, 0, 0);
                        carve_statement (&status, "read-tree", GG_KEYSTATUS, 0, 1);
                        carve_statement (&ccursor, "read-tree", GG_KEYNEWCURSOR, 0, 1);
                        // Begin read-tree
                        carve_statement (&value, "read-tree", GG_KEYVALUE, 0, 1);
                        carve_statement (&upd, "read-tree", GG_KEYUPDATEVALUE, 0, 1);
                        carve_statement (&getkey, "read-tree", GG_KEYKEY, 0, 1);
                        // End read-tree
                        carve_stmt_obj (&mtext, true, false);

                        define_statement (&ccursor, GG_DEFTREECURSOR, true);
                        define_statement (&status, GG_DEFNUMBER, false);
                        // Begin read-tree
                        define_statement (&value, GG_DEFSTRING, false); // exact length okay this is just pointer aliasing from found tree node
                        define_statement (&getkey, GG_DEFSTRING, false); // exact length okay this is just pointer aliasing from found tree node
                        make_mem (&key);
                        make_mem (&lkey);
                        make_mem (&lekey);
                        make_mem (&gekey);
                        make_mem (&gkey);
                        make_mem (&upd);
                        // End read-tree
                        //
                        check_var (&mtext, GG_DEFTREE);
                        check_var (&key, GG_DEFSTRING);
                        check_var (&lekey, GG_DEFSTRING);
                        check_var (&gekey, GG_DEFSTRING);
                        check_var (&lkey, GG_DEFSTRING);
                        check_var (&gkey, GG_DEFSTRING);
                        check_var (&upd, GG_DEFSTRING);

                        // for key accessing, only one of the following can be used
                        gg_num is_one = (key!=NULL?1:0)+(lekey!=NULL?1:0)+(gekey!=NULL?1:0)+(lkey!=NULL?1:0)+(gkey!=NULL?1:0)+(mink!=NULL?1:0)+(maxk!=NULL?1:0);
                        if (is_one > 1) gg_report_error ("Can use only one of key, lesser, greater, lesser-equal, greater-equal, min-key or max-key clauses");
                        if (is_one != 1) gg_report_error ("Must specify one of key, lesser, greater, lesser-equal, greater-equal, min-key or max-key clauses");


                        char cname[100];
                        if (ccursor == NULL) 
                        {
                            snprintf (cname, sizeof (cname), "_gg_tree_curs%ld", wcurs);
                            oprintf ("gg_tree_cursor *_gg_tree_curs%ld;\n", wcurs);
                            ccursor = cname;
                        }
                        oprintf ("gg_tree_cursor gg_tree_curs%ld;\n", wcurs);
                        oprintf ("%s = &gg_tree_curs%ld;\n", ccursor, wcurs);
                        // These each set ->current alone (no res or rkey)
                        if (mink != NULL)
                        {
                            oprintf ("gg_tree_min_f (%s, %s);\n", ccursor, mtext);
                        }
                        else if (maxk != NULL)
                        {
                            oprintf ("gg_tree_max_f (%s, %s);\n", ccursor, mtext);
                        }
                        else if (lkey != NULL)
                        {
                            oprintf ("gg_tree_search_lesser_equal_f (%s, %s, false, %s, -1);\n",  ccursor, mtext, lkey);
                        }
                        else if (gkey != NULL)
                        {
                            oprintf ("gg_tree_search_greater_equal_f (%s, %s, false, %s, -1);\n",  ccursor, mtext, gkey);
                        }
                        else if (lekey != NULL)
                        {
                            oprintf ("gg_tree_search_lesser_equal_f (%s, %s, true, %s, -1);\n",  ccursor, mtext, lekey);
                        }
                        else if (gekey != NULL)
                        {
                            oprintf ("gg_tree_search_greater_equal_f (%s, %s, true, %s, -1);\n",  ccursor, mtext, gekey);
                        }
                        else if (key != NULL)
                        {
                            oprintf ("gg_tree_search_f (%s, %s, %s, -1);\n", ccursor , mtext, key);
                        }
                        else gg_report_error ("Unknown read-tree key");

                        // after searching or deleting, get the result and the key affected, as well as status
                        if (status != NULL) oprintf ("%s = (%s)->status;\n", status, ccursor);

                        // Begin read-tree 
                        oprintf ("if ((%s)->status == GG_OKAY) {\n", ccursor);
                        if (value != NULL) 
                        {
                            oprintf ("%s = (%s)->current->data; gg_mem_add_ref(%s);\n", value, ccursor, value);
                        }
                        if (getkey != NULL) 
                        {
                            oprintf ("%s = (%s)->current->key; gg_mem_add_ref(%s);\n", getkey, ccursor, getkey);
                        }
                        if (upd != NULL) 
                        {
                            oprintf("gg_mem_process=(%s)->process;\n", mtext);
                            // 0 is fine in gg_mem_add_ref because in read-tree an element can never be NULL
                            oprintf ("gg_mem_set_process ((%s)->current->data, %s, false, true); gg_mem_replace_and_return ((%s)->current->data, %s); (%s)->current->data = %s; \n", ccursor, upd, ccursor, upd, ccursor, upd);
                            oprintf("gg_mem_process=false;\n");
                        }
                        //
                        // The following two are together: if status is not okay, AND if user didn't use 'status' clause,
                        // then report error
                        //
                        oprintf ("}\n");
                        if (status == NULL) oprintf(" else { gg_report_error (\"%s\", \"%s\", (gg_num)%ld, (%s)->status);}\n", GG_STMT_FAIL_CHECK, undecorate (src_file_name+GG_BLD_DIR_OFFSET), lnum, ccursor);
                        //
                        //
                        //
                        // End read-tree 
                        wcurs++;
                        continue;

                    }
                    else if ((newI=recog_statement(line, i, "delete-tree", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;

                        char *key = find_keyword (mtext, GG_KEYKEY, 1);
                        char *value = find_keyword (mtext, GG_KEYVALUE, 1);
                        char *status = find_keyword (mtext, GG_KEYSTATUS, 1);

                        carve_statement (&key, "delete-tree", GG_KEYKEY, 1, 1);
                        carve_statement (&status, "delete-tree", GG_KEYSTATUS, 0, 1);
                        carve_statement (&value, "delete-tree", GG_KEYVALUE, 0, 1);
                        carve_stmt_obj (&mtext, true, false);
                        make_mem(&key);

                        define_statement (&status, GG_DEFNUMBER, false);
                        define_statement (&value, GG_DEFSTRING, false); // exact length okay this is just pointer aliasing from found tree node
                        //
                        check_var (&mtext, GG_DEFTREE);
                        check_var (&key, GG_DEFSTRING);

                        oprintf ("gg_tree_cursor gg_tree_curs%ld;\n", wcurs);
                        oprintf ("gg_tree_cursor *_gg_tree_curs%ld = &gg_tree_curs%ld;\n", wcurs, wcurs);
                        oprintf("gg_mem_process=(%s)->process;\n", mtext);
                        oprintf ("gg_tree_delete_f (_gg_tree_curs%ld, %s, %s);\n", wcurs, mtext, key);
                        oprintf("gg_mem_process=false;\n");

                        // after searching or deleting, get the result and the key affected, as well as status
                        if (status != NULL) oprintf ("%s = _gg_tree_curs%ld->status;\n", status, wcurs);
                        // if value isn't requested, delete it. 
                        // note that key is automatically deleted in gg_tree_delete_f() above
                        oprintf ("if (_gg_tree_curs%ld->status == GG_OKAY) { ", wcurs);
                        // no change in reference count, because tree node no longer points to value (-1), and new variable points to it now (+1), so 0 total change.
                        if (value != NULL) oprintf ("%s = _gg_tree_curs%ld->res;\n", value, wcurs);
                        else oprintf ("gg_free(_gg_tree_curs%ld->res);\n", wcurs);
                        //
                        // The following two are together: if status is not okay, AND if user didn't use 'status' clause,
                        // then report error
                        //
                        oprintf ("}\n");
                        //If delete-tree doesn't delete, that's not considered application error!
                        //if (status == NULL) oprintf(" else { gg_report_error (\"%s\", \"%s\", (gg_num)%ld, _gg_tree_curs%ld->status);}\n", GG_STMT_FAIL_CHECK, undecorate (src_file_name+GG_BLD_DIR_OFFSET), lnum, wcurs);
                        //
                        //
                        //
                        wcurs++;
                        continue;

                    }
                    else if ((newI=recog_statement(line, i, "use-cursor", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;

                        char *cur = find_keyword (mtext, GG_KEYCURRENT, 1);
                        char *lkey = find_keyword (mtext, GG_KEYGETLESSER, 1);
                        char *gkey = find_keyword (mtext, GG_KEYGETGREATER, 1);
                        char *status = find_keyword (mtext, GG_KEYSTATUS, 1);
                        // Begin read-tree 
                        char *getkey = find_keyword (mtext, GG_KEYKEY, 1);
                        char *value = find_keyword (mtext, GG_KEYVALUE, 1);
                        char *upd = find_keyword (mtext, GG_KEYUPDATEVALUE, 1);
                        // End read-tree 

                        carve_statement (&cur, "use-cursor", GG_KEYCURRENT, 0, 0);
                        carve_statement (&lkey, "use-cursor", GG_KEYGETLESSER, 0, 0);
                        carve_statement (&gkey, "use-cursor", GG_KEYGETGREATER, 0, 0);
                        carve_statement (&status, "use-cursor", GG_KEYSTATUS, 0, 1);
                        // Begin read-tree
                        carve_statement (&value, "use-cursor", GG_KEYVALUE, 0, 1);
                        carve_statement (&upd, "use-cursor", GG_KEYUPDATEVALUE, 0, 1);
                        carve_statement (&getkey, "use-cursor", GG_KEYKEY, 0, 1);
                        // End read-tree
                        carve_stmt_obj (&mtext, true, false);

                        define_statement (&status, GG_DEFNUMBER, false);
                        // Begin read-tree
                        define_statement (&value, GG_DEFSTRING, false); // exact length okay this is just pointer aliasing from found tree node
                        define_statement (&getkey, GG_DEFSTRING, false); // exact length okay this is just pointer aliasing from found tree node
                        make_mem (&upd);
                        // End read-tree
                        //
                        check_var (&mtext, GG_DEFTREECURSOR);
                        check_var (&upd, GG_DEFSTRING);

                        char *ucursor = mtext;

                        if (lkey != NULL)
                        {
                            oprintf ("if ((%s)->current != NULL) { if ((%s)->root->sorted) { if ((%s)->current->dlist[GG_TREE_LESSER_LIST]) { (%s)->current = (%s)->current->dlist[GG_TREE_LESSER_LIST]; (%s)->status = GG_OKAY; } else {(%s)->status = GG_ERR_EXIST;} } else { gg_tree_search_lesser_equal_f (%s, (%s)->root, false, (%s)->current->key, -1); } }\n", ucursor, ucursor,ucursor, ucursor, ucursor, ucursor, ucursor, ucursor, ucursor, ucursor);
                        }
                        else if (gkey != NULL)
                        {
                            oprintf ("if ((%s)->current != NULL) { if ((%s)->root->sorted) { if ((%s)->current->dlist[GG_TREE_GREATER_LIST]) { (%s)->current = (%s)->current->dlist[GG_TREE_GREATER_LIST]; (%s)->status = GG_OKAY; } else {(%s)->status = GG_ERR_EXIST;} } else { gg_tree_search_greater_equal_f (%s, (%s)->root, false, (%s)->current->key, -1); } }\n", ucursor, ucursor,ucursor, ucursor, ucursor, ucursor, ucursor, ucursor, ucursor, ucursor);
                        }
                        else if (cur != NULL)
                        {
                            ; // nothing
                        }
                        else gg_report_error ("Must use one of current, get-lesser or get-greater clauses");
                        if (status != NULL) oprintf ("%s = (%s)->status;\n", status, ucursor);
                        // Begin read-tree 
                        oprintf ("if ((%s)->status == GG_OKAY) {\n", ucursor);
                        //
                        if (value != NULL) 
                        {
                            // delete reference to where value/key was pointing to, assuming it existed
                            oprintf ("%s = (%s)->current->data; gg_mem_add_ref(%s);\n", value, ucursor, value);
                        }
                        if (getkey != NULL) 
                        {
                            // delete reference to where value/key was pointing to, assuming it existed
                            oprintf ("%s = (%s)->current->key; gg_mem_add_ref(%s);\n", getkey, ucursor, getkey);
                        }
                        if (upd != NULL) 
                        {
                            oprintf("gg_mem_process=(%s)->root->process;\n", ucursor);
                            // 0 is fine in gg_mem_add_ref because in use-cursor an element can never be NULL
                            oprintf("gg_mem_set_process ((%s)->current->data, %s, false, true); gg_mem_replace_and_return ((%s)->current->data, %s); (%s)->current->data = %s; \n", ucursor, upd,ucursor, upd, ucursor, upd);
                            oprintf("gg_mem_process=false;\n");
                        }
                        //
                        //
                        // The following two are together: if status is not okay, AND if user didn't use 'status' clause,
                        // then report error
                        //
                        oprintf ("}\n");
                        if (status == NULL) oprintf(" else { gg_report_error (\"%s\", \"%s\", (gg_num)%ld, (%s)->status);}\n", GG_STMT_FAIL_CHECK, undecorate (src_file_name+GG_BLD_DIR_OFFSET), lnum, ucursor);
                        //
                        //
                        // End read-tree 
                        continue;

                    }
                    else if ((newI=recog_statement(line, i, "do-once", &mtext, &msize, 1, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;

                        carve_stmt_obj (&mtext, false, false);
                        if (do_once_open) gg_report_error ("Cannot use do-once inside another do-once statement");
                        do_once_open = true;
                        do_once_line = lnum;
                        do_once_level = gg_level; // this is the level that process-scoped variables will have, assuming 
                                                  // they are created at do-once level. Any proces-scoped variable created within
                                                  // any {} within do-scope will NOT have this level, but rather whatever leve it is on!
                                                  // So the process-scoped variables that will be affected are those with gg_level + 1 and
                                                  // none other.

                        oprintf ("static bool _gg_do_once_%ld = false;\n", do_once);
                        oprintf ("if (_gg_do_once_%ld) goto _gg_end_do_once_%ld;\n", do_once, do_once);
                        oprintf ("_gg_do_once_%ld = true;\n", do_once);

                        check_level ("{"); // start new scope for do-once

                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "end-do-once", &mtext, &msize, 1, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;
                        carve_stmt_obj (&mtext, false, false);
                        do_once_open = false;

                        check_level ("}"); // end new scope for do-once
                                           
                        oprintf ("_gg_end_do_once_%ld: ;\n",do_once); // must have empty ; statement
                                                                       // to avoid syntax errors on older compilers

                        do_once++;

                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "copy-file", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;
                        char *copy_to = find_keyword (mtext, GG_KEYTO, 1);
                        char *status = find_keyword (mtext, GG_KEYSTATUS, 1);
                        carve_statement (&copy_to, "copy-file", GG_KEYTO, 1, 1);
                        carve_statement (&status, "copy-file", GG_KEYSTATUS, 0, 1);
                        carve_stmt_obj (&mtext, true, true);
                        make_mem(&copy_to);
                        define_statement (&status, GG_DEFNUMBER, false);
                        //
                        check_var (&mtext, GG_DEFSTRING);
                        check_var (&copy_to, GG_DEFSTRING);
                        char *copy_from = mtext;
                        oprintf("_gg_st=gg_copy_file (%s, %s);\n", copy_from, copy_to);
                        if (status != NULL) oprintf ("%s=_gg_st;\n", status);
                        else oprintf("if (_gg_st < 0) gg_report_error (\"%s\", \"%s\", (gg_num)%ld, _gg_st);\n", GG_STMT_FAIL_CHECK, undecorate (src_file_name+GG_BLD_DIR_OFFSET), lnum);
                        continue;

                    }
                    else if ((newI=recog_statement(line, i, "finish-output", &mtext, &msize, 1, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;

                        carve_stmt_obj (&mtext, false, false);
                        oprintf("gg_SERVICE_Finish();\n");
                        oprintf("gg_disable_output();\n");
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "quit-process", &mtext, &msize, 1, &gg_is_inline)) != 0)
                    {
                        GG_GUARD
                        i = newI;

                        // quit this process as if mgrg sent stop command
                        oprintf("raise(SIGTERM); return;\n");
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "return-handler", &mtext, &msize, 1, &gg_is_inline)) != 0 ||
                          (newI1=recog_statement(line, i, "return-handler", &mtext, &msize, 0, &gg_is_inline)) != 0)
                    {
                        GG_GUARD
                        i = newI+newI1;
                        
                        if (newI1 != 0)  // returned value
                        {
                            carve_stmt_obj (&mtext, true, false);
                            check_var (&mtext, GG_DEFNUMBER);
                            oprintf("gg_get_config ()->ctx.req->return_val=(%s);\n", mtext);
                            // if this is top level handler, it's also the same as exit-handler
                            oprintf("if (!(gg_get_config ()->ctx.req->sub)) gg_get_config ()->ctx.req->ec =gg_get_config ()->ctx.req->return_val;\n");
                        } else oprintf("gg_get_config ()->ctx.req->return_val=0;\n");

                        // return from handler, no return status, use set/get-param
                        oprintf("return;\n");
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "exit-handler", &mtext, &msize, 1, &gg_is_inline)) != 0  
                        || (newI1=recog_statement(line, i, "exit-handler", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI+newI1;

                        if (newI1 != 0) 
                        {
                            carve_stmt_obj (&mtext, true, false);
                            oprintf("gg_get_config ()->ctx.req->ec=(%s);\n", mtext);
                        }

                        // return 1 to sigsetjmp so it differs from the first time around
                        oprintf("gg_exit_request(1);\n");
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "file-position", &mtext, &msize, 0, &gg_is_inline)) != 0)
                    {
                        GG_GUARD
                        i = newI;
                        char *fileid = find_keyword (mtext, GG_KEYFILEID, 1);
                        char *get = find_keyword (mtext, GG_KEYGET, 1);
                        char *set = find_keyword (mtext, GG_KEYSET, 1);
                        char *status = find_keyword (mtext, GG_KEYSTATUS, 1);

                        carve_statement (&fileid, "file-position", GG_KEYFILEID, 1, 1);
                        carve_statement (&get, "file-position", GG_KEYGET, 0, 1);
                        carve_statement (&set, "file-position", GG_KEYSET, 0, 1);
                        carve_statement (&status, "file-position", GG_KEYSTATUS, 0, 1);
                        carve_stmt_obj (&mtext, false, false);

                        define_statement (&get, GG_DEFNUMBER, false);
                        define_statement (&status, GG_DEFNUMBER, false);
                        //
                        check_var (&set, GG_DEFNUMBER);
                        check_var (&fileid, GG_DEFFILE);

                        if (get == NULL && set == NULL)  gg_report_error( "either 'get' or 'set' must be used in file-position statement");
                        if (get != NULL && set != NULL)  gg_report_error( "cannot specify both 'get' and 'set' in file-position statement");
                        if (get != NULL) oprintf("_gg_st=gg_get_file_pos (*((%s)->f), &(%s));\n", fileid, get);
                        else if (set != NULL) oprintf("_gg_st=gg_set_file_pos (*((%s)->f), %s);\n", fileid, set);
                        if (status != NULL) oprintf ("%s=_gg_st;\n", status);
                        else oprintf("if (_gg_st != GG_OKAY) gg_report_error (\"%s\", \"%s\", (gg_num)%ld, _gg_st);\n", GG_STMT_FAIL_CHECK, undecorate (src_file_name+GG_BLD_DIR_OFFSET), lnum);
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "close-file", &mtext, &msize, 0, &gg_is_inline)) != 0)
                    {
                        GG_GUARD
                        i = newI;
                        char *fileid = find_keyword (mtext, GG_KEYFILEID, 1);
                        char *st = find_keyword (mtext, GG_KEYSTATUS, 1);

                        carve_statement (&fileid, "close-file", GG_KEYFILEID, 1, 1);
                        carve_statement (&st, "close-file", GG_KEYSTATUS, 0, 1);
                        carve_stmt_obj (&mtext, false, false);
                        define_statement (&st, GG_DEFNUMBER, false);
                        //
                        check_var (&fileid, GG_DEFFILE);

                        oprintf("_gg_st = gg_fclose (*((%s)->f));\n", fileid);
                       // we do not check for status not being checked here
                        if (st != NULL) oprintf("%s = _gg_st;\n", st);
                        // set file pointer to NULL, because that's checked in gg_done()
                        // this is needed beyond gg_done(), any file op would check if NULL so avoids crashes
                        oprintf ("*((%s)->f) = NULL;\n", fileid);
                        oprintf("gg_mem_release ((%s)->memind);\n", fileid); // also remove vm[] entry
                        oprintf("gg_free (%s);\n", fileid); 
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "open-file", &mtext, &msize, 0, &gg_is_inline)) != 0)
                    {
                        GG_GUARD
                        i = newI;
                        char *fileid = find_keyword (mtext, GG_KEYFILEID, 1);
                        char *newt = find_keyword (mtext, GG_KEYNEWTRUNCATE, 1);
                        char *st = find_keyword (mtext, GG_KEYSTATUS, 1);

                        carve_statement (&newt, "open-file", GG_KEYNEWTRUNCATE, 0, 0);
                        carve_statement (&fileid, "open-file", GG_KEYFILEID, 1, 1);
                        carve_statement (&st, "open-file", GG_KEYSTATUS, 0, 1);
                        carve_stmt_obj (&mtext, true, true); 

                        define_statement (&fileid, GG_DEFFILE, true);
                        define_statement (&st, GG_DEFNUMBER, false);
                        //
                        check_var (&mtext, GG_DEFSTRING);

                        //GG_ERR is set in gg_fopen
                        oprintf("%s = gg_calloc (1, sizeof(gg_file));\n", fileid); // this sets file pointer f to NULL
                        // need static FILE * so that its address points to valid memory anytime, including in gg_done()
                        // where any open file descriptors are at least checked, if not closed
                        oprintf("static FILE *_gg_fileid_%ld;\n", file_count); 
                        oprintf("_gg_fileid_%ld = gg_fopen (%s, \"%s\");\n", file_count, mtext, newt!=NULL?"w+":"r+");
                        if (st == NULL) oprintf("if (_gg_fileid_%ld == NULL) gg_report_error (\"%s\", \"%s\", (gg_num)%ld, (gg_num)GG_ERR_OPEN);\n", file_count,GG_STMT_FAIL_CHECK, undecorate (src_file_name+GG_BLD_DIR_OFFSET), lnum);
                        else oprintf("if (_gg_fileid_%ld == NULL) %s=GG_ERR_OPEN; else %s=GG_OKAY;\n", file_count, st, st);
                        // set FILE*, and also the index into gg[] (the memind member), so that when closing, it can 
                        // clear the gg[]->ptr and make gg_done() faster
                        // we set ->f to file descriptor even if NULL, because ->f is FILE** and should NEVER be NULL, rather *(of->f) should be null
                        oprintf ("(%s)->f = &_gg_fileid_%ld; if (_gg_fileid_%ld != NULL) {(%s)->memind = gg_reg_file(&_gg_fileid_%ld);}\n", fileid,  file_count, file_count, fileid, file_count);
                        file_count++;
                        continue;
                    }
                    //
                    // This is a method to have all optional params - check for both with no params (isLast==1) and with params
                    // (isLast==0). Based on return value we may look for optional params.
                    // Checking for non-mandatory (6th param is 1) MUST be done first.
                    //
                    else if ((newI=recog_statement(line, i, "end-write-string", &mtext, &msize, 1, &gg_is_inline)) != 0 ||
                        (newI1=recog_statement(line, i, "end-write-string", &mtext, &msize, 0, &gg_is_inline)) != 0  ||
                        (newI2=recog_statement(line, i, "))", &mtext, &msize, 1, &gg_is_inline)) != 0 ||
                        (newI3=recog_statement(line, i, "))", &mtext, &msize, 0, &gg_is_inline)) != 0)
                     {
                        GG_GUARD
                        i = newI+newI1+newI2+newI3;
                        // end-write-string, being string writing within string writing, can NOT be within <<..>>
                        if (within_inline == 1)
                        {
                            gg_report_error( "Cannot use write-string as inline code");
                        }
                        // checking for notrim must be before gg_write_to_string (NULL) below because it's used in it
                        // find_keyword must be done before any carve_statement, even if it's used later
                        char *notrim = NULL;
                        if (newI1 != 0 || newI3 != 0)
                        {
                            notrim = find_keyword (mtext, GG_KEYNOTRIM, 1);
                            carve_statement (&notrim, "end-write-string", GG_KEYNOTRIM, 0, 0);
                        }
                        carve_stmt_obj (&mtext, false, false); 
                        if (notrim != NULL)
                        {
                            oprintf("gg_write_to_string_notrim();\n");
                        }
                        oprintf("gg_write_to_string (NULL);\n"); 
                        // this has string length, but no longer needed as we keep track internally
                        //oprintf("%s%sgg_write_to_string_length();\n", bytes==NULL?"":bytes,bytes==NULL?"":"=");

                        gen_ctx->total_write_string--;

                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "write-string", &mtext, &msize, 0, &gg_is_inline)) != 0 ||
    -                    (newI1=recog_statement(line, i, "((", &mtext, &msize, 0, &gg_is_inline)) != 0)
                    {
                        GG_GUARD
                        i = newI+newI1;
                        // write-string, being string writing within string writing, can NOT be within <<..>>
                        if (within_inline == 1)
                        {
                            gg_report_error( "Cannot use write-string as inline code");
                        }
                        carve_stmt_obj (&mtext, true, false); 

                        define_statement (&mtext, GG_DEFSTRING, false); // exact length set in gg_write_to_string

                        oprintf("gg_write_to_string (&(%s));\n", mtext); 
                        gen_ctx->total_write_string++;


                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "read-line", &mtext, &msize, 0, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;

                        char *to = find_keyword (mtext, GG_KEYTO, 1);
                        char *del = find_keyword (mtext, GG_KEYDELIMITER, 1);
                        char *len = find_keyword (mtext, GG_KEYSTATUS, 1);

                        carve_statement (&to, "read-line", GG_KEYTO, 1, 1);
                        carve_statement (&del, "read-line", GG_KEYDELIMITER, 0, 1);
                        carve_statement (&len, "read-line", GG_KEYSTATUS, 0, 1);
                        carve_stmt_obj (&mtext, true, true); 

                        define_statement (&to, GG_DEFSTRING, false);
                        define_statement (&len, GG_DEFNUMBER, false);
                        if (del == NULL) {del = "\"\\n\"";}
                        make_mem (&del);
                        //
                        check_var (&mtext, GG_DEFSTRING);
                        check_var (&del, GG_DEFSTRING);


                        oprintf("{\n");
                        if (len != NULL) oprintf("gg_num *gg_rl_read_%ld = &(%s);\n", open_readline, len);
                        else 
                        {
                            oprintf("gg_num _gg_rl_read_%ld;\n", open_readline);
                            oprintf("gg_num *gg_rl_read_%ld = &(_gg_rl_read_%ld);\n", open_readline, open_readline);
                        }
                        oprintf("size_t gg_rl_local_len_%ld = 2;\n", open_readline);
                        oprintf("size_t gg_rl_len_%ld = 0;\n", open_readline);
                        oprintf("char *gg_rl_local_memptr_%ld = GG_EMPTY_STRING;\n", open_readline);
                        oprintf("char **gg_to_local_memptr_%ld = &(%s);\n", open_readline, to);
                        //
                        oprintf("char *gg_rl_memptr_%ld = NULL;\n", open_readline);
                        oprintf("FILE *gg_rl_%ld = fopen (%s, \"r\");\n", open_readline, mtext); 
                        oprintf("if (gg_rl_%ld != NULL) {\n", open_readline);
                        oprintf("while (1) {\n");
                        oprintf("*gg_rl_read_%ld = (gg_num)getdelim (&gg_rl_memptr_%ld, &gg_rl_len_%ld, (int)(%s[0]), gg_rl_%ld);\n", open_readline, open_readline, open_readline, del, open_readline);
                        oprintf("if (*gg_rl_read_%ld == -1) { break;}\n",  open_readline);
                        // result of getdelim is the real length of string, minus 1. gg_rl_len_%ld is generally greater than that, but impossible to say what it is!
                        oprintf("gg_rl_local_memptr_%ld = gg_malloc (gg_rl_local_len_%ld=*gg_rl_read_%ld+1);\n", open_readline,open_readline,open_readline);
                        // Copy to buffer
                        oprintf ("memcpy(gg_rl_local_memptr_%ld, gg_rl_memptr_%ld, *gg_rl_read_%ld+1);\n", open_readline,open_readline,open_readline);
                        //
                        oprintf ("%s = gg_rl_local_memptr_%ld;\n", to, open_readline);
                        oprintf ("free(gg_rl_memptr_%ld); gg_rl_memptr_%ld = NULL; gg_rl_len_%ld=0;\n", open_readline,open_readline,open_readline); // open_readline was ++ after generating the code, so this is it

                        check_next_readline
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "end-read-line", &mtext, &msize, 1, &gg_is_inline)) != 0)  
                    {
                        GG_GUARD
                        i = newI;
                        carve_stmt_obj (&mtext, false, false); 
                        // must assign "" in the loop, checking after loop for this pointer in case there was a break from the loop, but by setting to ""
                        // here, we won't free again
                        oprintf ("if (gg_rl_local_memptr_%ld!=GG_EMPTY_STRING) {gg_free_int (gg_rl_local_memptr_%ld); gg_rl_local_memptr_%ld = GG_EMPTY_STRING;}\n", open_readline-1, open_readline-1, open_readline-1);
                        oprintf("}\n");
                        oprintf ("if (!feof_unlocked(gg_rl_%ld)) {GG_ERR;*gg_rl_read_%ld = (gg_num)GG_ERR_READ;} else {*gg_rl_read_%ld = (gg_num)GG_OKAY;}\n", open_readline-1, open_readline-1, open_readline-1); // this is if there was an error, not end of file
                        oprintf ("fclose (gg_rl_%ld);\n", open_readline-1); // open_readline was ++ after generating the code, so this is it
                        oprintf("} else { GG_ERR;*gg_rl_read_%ld = (gg_num)GG_ERR_OPEN; }\n", open_readline-1);
                        // set "to" to empty, otherwise it's pointing to a freed pointer and Golf will try to free it again in optimized-memory mode, and crash
                        // so 'to' pointer is set to empty either way - we do equivalent of delete-string on it (which is free+set to empty)
                        // this is different because freeing setting to empty are not in the same place here
                        oprintf ("if (gg_rl_local_memptr_%ld!=GG_EMPTY_STRING) gg_free_int (gg_rl_local_memptr_%ld);\n*gg_to_local_memptr_%ld = GG_EMPTY_STRING;\n", open_readline-1, open_readline-1, open_readline-1);
                        oprintf ("if (gg_rl_memptr_%ld != NULL) free(gg_rl_memptr_%ld);\n",  open_readline-1, open_readline-1);
                        oprintf("}\n");
                        open_readline--;

                        continue;
                    }
                    // =recog_statement(line,i,"no-statement",...) - this is to cap the end_read_line in getvim script - DO NOT REMOVE
                    else if (print_mode != 1)
                    {
                        // anything unrecognized is C code
                        check_golf(line+i);
                        oprintf("%s\n", line+i);
                        i += strlen (line+i);
                        gg_report_error( "Unrecognized statement");
                        continue;
                    }
                    else
                    {
                        if (within_inline == 1) 
                        {
                            // anything unrecognized is C code
                            check_golf(line+i);
                            oprintf("%s", line+i);
                            i += strlen (line+i);
                            continue;
                        }
                    }
                    // we come here if it is print mode and all others are unrecognized 
                }
            }
            first_on_line = 0;

            if (line[i] == '"') 
            {
                oprintf("\\\"");
            }
            else if (line[i] == '\\') 
            {
                oprintf("\\\\");
            }
            else if (line[i] == '\n' || line[i] == 0 || i>=len) 
            {
                // should not get here because we gg_trim line, and never reach 0 char
                // and should never go beyond the length of the line
                gg_report_error ("Parsing error, extra text found, [%s]", line);
            }
            else if (line[i] == '%')
            {
                oprintf("%%");
            }
            else 
            {
                oprintf("%c", line[i]);
            }
        
        }

        line_split = false;

        // check if clause that should have no data, indeed has no data
        gg_num e;
        for (e = 0; e < emptyc_c; e++)
        {
            trimit (emptyc[e]);
            if (emptyc[e][0] != 0) gg_report_error ("Unrecognized text starting with [%s], please check syntax for statement", emptyc[e]);
        }

        //
        if (within_inline == 1)
        {
            gg_report_error( "Inline code was open with << but not closed with >>");
        }
        //See comment above for this. In addition, this is when >> is the very last on line, so the above does not
        //execute. We have to start this, even if it's gonna be empty, because we need to print this new line below
        //to finish the @ line. This is only if we're in @, i.e. printing.
        GG_OPEN_PRINT

        if (print_mode == 1)
        {
            // Print new line at the end
            oprintf("\\n\", GG_EMPTY_LONG_PLAIN_ZERO, false);\n");
        }
    

        if (feof_unlocked (f)) break; // something read in, then EOF
    }

    // check if variable unused
    gg_rewind_hash(gg_varh);
    gg_num ost;
    gg_var *ovar;
    while (1)
    {
        gg_next_hash(gg_varh,  (void**)&ovar, &ost, false);
        if (ost == GG_ERR_EXIST) break;
        char *n = strchr (ovar->name, '+');
        if (n != NULL) 
        {
            n++;
            if (!ovar->used) 
            {
                // _gg_report_error (*note* underscore) will NOT exit, so this is to be used for errors where
                // v1 *can* continue. This is the first, as so far code has been written to exit right away.
                // Of course it's possible to continue, but the code must be built for it.
                // In _gg_report_error, gg_curr_errors is set to >0, and at the end of main() this is checked and 
                // main exits with code 1, which is needed for v1 calls in vmakefile, so v1 doesn't keep going and 
                // trying to compile C code which we know is bad.
                _gg_report_error ("Variable [%s] defined on line [%ld] is not used", n, ovar->lnum);
            }
        }
    }


    if (do_once_open)
    {
        gg_report_error( "do-once statement found open on line [%ld], but it was never closed", do_once_line);
    }

    if (ccomm_open)
    {
        gg_report_error( "Opening C comment /* found on line [%ld], but it was never closed", ccomm_line);
    }

    if (gen_ctx->total_write_string != 0)
    {
        gg_report_error( "Imbalance in write-string/end-write-string statements, too many open or not closed");
    }

    if (open_loops != 0)
    {
        gg_report_error( "'start-loop' code block imbalance at line %ld, %ld %s open than closed", last_line_loop_closed, labs(open_loops), open_loops > 0 ? "more" : "less" );
    }
    if (open_queries != 0)
    {
        gg_report_error( "'query' code block imbalance at line %ld, %ld %s open than closed", last_line_query_closed, labs(open_queries), open_queries > 0 ? "more" : "less" );
    }
    if (open_readline != 0)
    {
        gg_report_error( "'read-line' code block imbalance at line check line %ld, %ld %s open than closed", last_line_readline_closed, labs(open_readline), open_readline > 0 ? "more" : "less");
    }
    if (open_ifs != 0)
    {
        gg_report_error( "'if-true' code block imbalance at line %ld, %ld %s open than closed", last_line_number_closed, labs(open_ifs), open_ifs > 0 ? "more" : "less" );
    }
    if (gen_ctx->curr_qry_ptr !=0)
    {
        gg_report_error( "Query imbalance (too many queries opened, too few closed)");
    }

    if (!done_handler) gg_report_error( "No begin-handler found");

}

#undef GG_FILE
#undef GG_LINE
#define GG_FILE "[no file opened yet]"
#define GG_LINE 0


//
// Main code, generates the source C file (as a __<file name>.c) based on input file
// and input parameters
//

int main (int argc, char* argv[])
{

    // environment var subst, does not use malloc
    if (argc == 2 && !strcmp (argv[1], "-envsub"))
    {
        envsub();
        exit(0); // no return from environment substitution
    }

    // create memory, no gg_malloc and such prior to here
    gg_pc = gg_alloc_config ();
    gg_memory_init();

    gg_gen_ctx *gen_ctx = (gg_gen_ctx*)gg_malloc(sizeof (gg_gen_ctx));
    init_gg_gen_ctx (gen_ctx);

    //
    // The following two options allow URL and WEB encoding of strings
    // from the command line. Typically used to construct URLs for command line
    // execution.
    //
    if (argc == 3 && !strcmp (argv[1], "-urlencode"))
    {
        // display URL encoded version of a string
        char *res = NULL;
        gg_encode (GG_URL, argv[2], -1, &res, false);
        fprintf (stdout, "%s", res);
        exit(0);
    }
    if (argc == 3 && !strcmp (argv[1], "-webencode"))
    {
        // display URL encoded version of a string
        char *res = NULL;
        gg_encode (GG_WEB, argv[2], -1, &res, false);
        fprintf (stdout, "%s", res);
        exit(0);
    }

    //
    // Get help from command line
    //
    if (argc == 1 || (argc == 2 && !strcmp (argv[1], "-version")))
    {
        fprintf (stdout, "%s %s on %s (%s)\n",argv[0], GG_VERSION, GG_OS_NAME, GG_OS_VERSION);
        exit(0);
    }

    //
    // parse input parameters
    //
    gg_num i;
    char *out_file_name = NULL;
    char *_item = NULL;
    gg_num _main = 0;
    for (i = 1; i < argc; i ++)
    {
        if (!strcmp (argv[i], "-main"))
        {
            _main = 1;
            continue;
        }
        else if (!strcmp (argv[i], "-trace"))
        {
            if (i + 1 >= argc)
            {
                gg_report_error ( "Trace (0 or 1) not specified after -trace option");
                exit (1);
            }
            gg_is_trace = atoi (argv[i+1]);
            i++; // skip db location now
            continue;
        }
        else if (!strcmp (argv[i], "-app-path"))
        {
            if (i + 1 >= argc)
            {
                gg_report_error ( "URL application prefix not specified after -app-path option");
                exit (1);
            }

            app_path = gg_strdup (argv[i+1]);
            i++; // skip app path
            continue;
        }
        else if (!strcmp (argv[i], "-plain-diag"))
        {
            if (i + 1 >= argc)
            {
                gg_report_error ( "Type of diagnostics not specified after -plain-diag option");
                exit (1);
            }
            if (!strcmp (argv[i+1], "1")) gg_plain_diag = 1;
            i++; // skip diag option
            continue;
        }
        else if (!strcmp (argv[i], "-max-upload"))
        {
            if (i + 1 >= argc)
            {
                gg_report_error ( "Maximum upload length not specified after -max-upload option");
                exit (1);
            }
            gg_max_upload = atol (argv[i+1]);
            i++; // skip db location now
            continue;
        }
        else if (!strcmp (argv[i], "-name"))
        {
            if (i + 1 >= argc)
            {
                gg_report_error ( "Application name not specified after -name option");
                exit (1);
            }
            GG_STRDUP (gg_app_name, argv[i+1]);
            if (gg_is_valid_param_name(gg_app_name, true, false) != 1)
            {
                gg_report_error( "Application name must have only alphanumeric characters, hyphens or underscores, found [%s]", gg_app_name);
            }
            i++; // skip db location now
            continue;
        }
        else if (!strcmp (argv[i], "-out"))
        {
            if (i + 1 >= argc)
            {
                gg_report_error ( "Output file not specified after -out option");
                exit (1);
            }
            GG_STRDUP (out_file_name, argv[i+1]);
            i++; // skip db location now
            continue;
        }
        else if (!strcmp (argv[i], "-ignore-warn"))
        {
            gg_ignore_warn = true;
        }
        else if (!strcmp (argv[i], "-max-errors"))
        {
            if (i + 1 >= argc)
            {
                gg_report_error ( "Maximum number of errors not specified");
                exit (1);
            }
            gg_max_errors = atol (argv[i+1]);
            i++;
            continue;
        }
        else if (!strcmp (argv[i], "-v"))
        {
            verbose = 1;
        }
        else if (!strcmp (argv[i], "-x"))
        {
            no_gg_line = 1;
        }
        else if (!strcmp (argv[i], "-single-file"))
        {
            gg_single_file = true;
        }
        else if (!strcmp (argv[i], "-public"))
        {
            gg_public = true;
        }
        else
        {
            _item = argv[i];
            if (src_file_name != NULL)
            {
                gg_report_error ( "Only one file name can be specified for processing, already specified [%s]", src_file_name);
                exit (1);
            }
            if (src_file_name != NULL && _main == 1)
            {
                gg_report_error ( "Cannot specify file name to process [%s], and the -main option to generate program main code. Use one or the other.", src_file_name);
                exit (1);
            }
            src_file_name = _item;
            char *src_bn = gg_basename (src_file_name); // this is alloc'd mem
            char *dot = strstr (src_bn, ".golf");
            if (dot == NULL || dot[sizeof(".golf")-1] != 0) gg_report_error ("Golf source file names must have a .golf extension, file [%s]", src_bn);
            *dot = 0;
            if (gg_is_valid_param_name (src_bn, false, false) != 1) gg_report_error (GG_MSG_SRC_NAME, src_bn);
            gg_free (src_bn);
        }
    }

    if (gg_is_trace != 0 && gg_is_trace != 1) 
    {
        gg_report_error ( "Tracing (-trace option) must be 0 or 1");
        exit (1);
    }
    gg_num upper_limit = (gg_num)1024*1024*1024*1024;
    if (gg_max_upload <1024 || gg_max_upload > upper_limit)
    {
        gg_report_error ( "Max_upload_size (-max-upload) must be a number between 1024 and %ld", upper_limit);
        exit (1);
    }
    if (gg_app_name == NULL || gg_app_name[0] == 0) 
    {
        gg_report_error ( "Application name is missing (-name option)");
        exit (1);
    }
    // app path is by default /<app name>
    if (app_path[0] == 0) 
    {
        gg_num namel = (gg_num)strlen (gg_app_name);
        app_path = (char*)gg_malloc (namel + 2); // 1 for ending-null and 1 for /
        snprintf (app_path, namel+2, "/%.*s", (int)namel, gg_app_name);
    }
    gg_is_valid_app_path(app_path);

    gg_bld_dir = (char*)gg_malloc (GG_FILE_NAME_LEN);
    gg_bld_dir_len = snprintf (gg_bld_dir, GG_FILE_NAME_LEN, GG_ROOT "/var/lib/gg/bld/%s", gg_app_name) + 1;
    gg_dbconf_dir = (char*)gg_malloc (GG_FILE_NAME_LEN);
    snprintf (gg_dbconf_dir, GG_FILE_NAME_LEN, GG_ROOT "/var/lib/gg/%s/app/db", gg_app_name);


    //
    //
    // We must set various variables used in GOLF shared library, for example, global connection data (description, transaction marker, 
    // connection marker). Encryption function is set too, as well as other variable.
    // This is the SAME code as below generated for application. This is because for GOLF, we need it in order for db connection etc. to work.
    // And we need it for application too. Here in golf, we don't need it to be static since it's only one run of this process.
    // We can't really put this code in a common place because it spans two projects. Perhaps something could be done with generated code
    // but the effort doesn't seem worth it.
    //
    memset (&gg_dbs, 0, sizeof(gg_dbs));
    gg_get_config()->ctx.db=&gg_dbs;
    totconn = 0;
    gg_get_config()->app.dbconf_dir=gg_dbconf_dir;

    // For any .golf file, we need a list of connections available (in /.../bld/.dbvendors file). Only connections available
    // there can be used in .golf files
    char gg_db_file[300];
    snprintf (gg_db_file, sizeof(gg_db_file), "%s/.dbvendors", gg_bld_dir);

    FILE *f = fopen (gg_db_file, "r");
    // it is okay for f to be NULL - this is if app has no connections, i.e. it does not use databases
    if (f != NULL)
    {
        gg_dbs.conn = NULL; // gg_malloc in add_connection
        char line[200];
        gg_num dbtype;
        while (1)
        {
            if (fgets_unlocked (line, sizeof (line) - 1, f) != NULL)
            {
                gg_num len = strlen (line);
                gg_trim (line, &len, false);
                char *eq = strchr (line, '=');
                if (eq == NULL) 
                {
                    gg_report_error ( "dbvendors file format is incorrect");
                    exit (1);
                }
                *(eq++) = 0;
                if (!strcmp (eq, "mariadb")) dbtype = GG_DB_MARIADB;
                else if (!strcmp (eq, "sqlite")) dbtype = GG_DB_SQLITE;
                else if (!strcmp (eq, "postgres")) dbtype = GG_DB_POSTGRES;
                else 
                {
                    gg_report_error ( "Unsupported database type [%s]", eq);
                    exit (1);
                }
                add_connection (line, dbtype); // add connection one by one, none is actually found since the list was empty
            } else break;
        }
        fclose (f);
    }


    if (out_file_name != NULL)
    {
        outf = gg_fopen (out_file_name, "w");
        if (outf == NULL)
        {
            gg_report_error ( "Cannot open output file [%s] for writing, error [%s]", out_file_name, strerror(errno));
            exit (1);
        }
    }


    if (_main == 0 && src_file_name == NULL)
    {
        gg_report_error ( "Neither -main option or the file name to process is specified.");
        exit (1);
    }

    // create hash for variables
    // first check the size of the source file, make hash size comparable to code length
    gg_num sz;
    gg_file_stat (src_file_name, NULL, &sz, NULL);
    sz = sz/10;
    if (sz < 100) sz = 100;
    gg_create_hash (&gg_varh, sz, NULL, false);
    add_var ("true", GG_DEFBOOL, NULL, false);
    add_var ("false", GG_DEFBOOL, NULL, false);
    //
    // These are user-interfacing variables from golf.h and vcfgi.h
    // When they are added/removed there, they must be here too
    //
    add_var ("GG_CLI_ERR_SOCK_READ", GG_DEFNUMBER, NULL, false);
    add_var ("GG_CLI_ERR_PROT_ERR", GG_DEFNUMBER, NULL, false);
    add_var ("GG_CLI_ERR_BAD_VER", GG_DEFNUMBER, NULL, false);
    add_var ("GG_CLI_ERR_SRV", GG_DEFNUMBER, NULL, false);
    add_var ("GG_CLI_ERR_UNK", GG_DEFNUMBER, NULL, false);
    add_var ("GG_CLI_ERR_OUT_MEM", GG_DEFNUMBER, NULL, false);
    add_var ("GG_CLI_ERR_RESOLVE_ADDR", GG_DEFNUMBER, NULL, false);
    add_var ("GG_CLI_ERR_PATH_TOO_LONG", GG_DEFNUMBER, NULL, false);
    add_var ("GG_CLI_ERR_CONNECT", GG_DEFNUMBER, NULL, false);
    add_var ("GG_CLI_ERR_TIMEOUT", GG_DEFNUMBER, NULL, false);
    add_var ("GG_CLI_ERR_SOCK_WRITE", GG_DEFNUMBER, NULL, false);
    add_var ("GG_CLI_ERR_INTERNAL", GG_DEFNUMBER, NULL, false);
    add_var ("GG_CLI_ERR_ENV_TOO_LONG", GG_DEFNUMBER, NULL, false);
    add_var ("GG_CLI_ERR_BAD_TIMEOUT", GG_DEFNUMBER, NULL, false);
    add_var ("GG_CLI_ERR_ENV_ODD", GG_DEFNUMBER, NULL, false);
    add_var ("GG_CLI_ERR_SOCKET", GG_DEFNUMBER, NULL, false);
    add_var ("GG_CLI_ERR_TOTAL", GG_DEFNUMBER, NULL, false);
    add_var ("GG_GET", GG_DEFNUMBER, NULL, false);
    add_var ("GG_PUT", GG_DEFNUMBER, NULL, false);
    add_var ("GG_POST", GG_DEFNUMBER, NULL, false);
    add_var ("GG_PATCH", GG_DEFNUMBER, NULL, false);
    add_var ("GG_DELETE", GG_DEFNUMBER, NULL, false);
    add_var ("GG_OTHER", GG_DEFNUMBER, NULL, false);
    add_var ("GG_FILE", GG_DEFNUMBER, NULL, false);
    add_var ("GG_DIR", GG_DEFNUMBER, NULL, false);
    add_var ("GG_OKAY", GG_DEFNUMBER, NULL, false);
    add_var ("GG_ERR_OPEN", GG_DEFNUMBER, NULL, false);
    add_var ("GG_ERR_OPEN_TARGET", GG_DEFNUMBER, NULL, false);
    add_var ("GG_ERR_READ", GG_DEFNUMBER, NULL, false);
    add_var ("GG_ERR_WRITE", GG_DEFNUMBER, NULL, false);
    add_var ("GG_ERR_POSITION", GG_DEFNUMBER, NULL, false);
    add_var ("GG_ERR_TOO_MANY", GG_DEFNUMBER, NULL, false);
    add_var ("GG_ERR_DELETE", GG_DEFNUMBER, NULL, false);
    add_var ("GG_ERR_REFERENCE", GG_DEFNUMBER, NULL, false);
    add_var ("GG_ERR_FAILED", GG_DEFNUMBER, NULL, false);
    add_var ("GG_ERR_WEB_CALL", GG_DEFNUMBER, NULL, false);
    add_var ("GG_ERR_CREATE", GG_DEFNUMBER, NULL, false);
    add_var ("GG_ERR_EXIST", GG_DEFNUMBER, NULL, false);
    add_var ("GG_ERR_INVALID", GG_DEFNUMBER, NULL, false);
    add_var ("GG_ERR_RENAME", GG_DEFNUMBER, NULL, false);
    add_var ("GG_ERR_MEMORY", GG_DEFNUMBER, NULL, false);
    add_var ("GG_ERR_UTF", GG_DEFNUMBER, NULL, false);
    add_var ("GG_ERR_FORMAT", GG_DEFNUMBER, NULL, false);
    add_var ("GG_ERR_CLOSE", GG_DEFNUMBER, NULL, false);
    add_var ("GG_ERR_OVERFLOW", GG_DEFNUMBER, NULL, false);
    add_var ("GG_ERR_LENGTH", GG_DEFNUMBER, NULL, false);
    add_var ("GG_ERR_JSON", GG_DEFNUMBER, NULL, false);
    add_var ("GG_ERR_XML", GG_DEFNUMBER, NULL, false);
    add_var ("GG_ERR_UNKNOWN", GG_DEFNUMBER, NULL, false);
    add_var ("GG_DB_MARIADB", GG_DEFNUMBER, NULL, false);
    add_var ("GG_DB_POSTGRES", GG_DEFNUMBER, NULL, false);
    add_var ("GG_DB_SQLITE", GG_DEFNUMBER, NULL, false);
    add_var ("GG_INFO_EXIST", GG_DEFNUMBER, NULL, false);
    add_var ("GG_JSON_TYPE_STRING", GG_DEFNUMBER, NULL, false);
    add_var ("GG_JSON_TYPE_NUMBER", GG_DEFNUMBER, NULL, false);
    add_var ("GG_JSON_TYPE_REAL", GG_DEFNUMBER, NULL, false);
    add_var ("GG_JSON_TYPE_BOOL", GG_DEFNUMBER, NULL, false);
    add_var ("GG_JSON_TYPE_NULL", GG_DEFNUMBER, NULL, false);
    add_var ("GG_JSON_TYPE_NONE", GG_DEFNUMBER, NULL, false);
    add_var ("GG_SOURCE_LINE", GG_DEFNUMBER, NULL, false);
    add_var ("GG_EMPTY_STRING", GG_DEFSTRING, NULL, false);
    add_var ("NULL", GG_DEFSTRING, NULL, false);
    //
    // C errno constant
    //
    add_var ("E2BIG", GG_DEFNUMBER, NULL, false);
    add_var ("EACCES", GG_DEFNUMBER, NULL, false);
    add_var ("EADDRINUSE", GG_DEFNUMBER, NULL, false);
    add_var ("EADDRNOTAVAIL", GG_DEFNUMBER, NULL, false);
    add_var ("EAFNOSUPPORT", GG_DEFNUMBER, NULL, false);
    add_var ("EAGAIN", GG_DEFNUMBER, NULL, false);
    add_var ("EWOULDBLOCK)", GG_DEFNUMBER, NULL, false);
    add_var ("EALREADY", GG_DEFNUMBER, NULL, false);
    add_var ("EBADE", GG_DEFNUMBER, NULL, false);
    add_var ("EBADF", GG_DEFNUMBER, NULL, false);
    add_var ("EBADFD", GG_DEFNUMBER, NULL, false);
    add_var ("EBADMSG", GG_DEFNUMBER, NULL, false);
    add_var ("EBADR", GG_DEFNUMBER, NULL, false);
    add_var ("EBADRQC", GG_DEFNUMBER, NULL, false);
    add_var ("EBADSLT", GG_DEFNUMBER, NULL, false);
    add_var ("EBUSY", GG_DEFNUMBER, NULL, false);
    add_var ("ECANCELED", GG_DEFNUMBER, NULL, false);
    add_var ("ECHILD", GG_DEFNUMBER, NULL, false);
    add_var ("ECHRNG", GG_DEFNUMBER, NULL, false);
    add_var ("ECOMM", GG_DEFNUMBER, NULL, false);
    add_var ("ECONNABORTED", GG_DEFNUMBER, NULL, false);
    add_var ("ECONNREFUSED", GG_DEFNUMBER, NULL, false);
    add_var ("ECONNRESET", GG_DEFNUMBER, NULL, false);
    add_var ("EDEADLK", GG_DEFNUMBER, NULL, false);
    add_var ("EDEADLOCK", GG_DEFNUMBER, NULL, false);
    add_var ("EDESTADDRREQ", GG_DEFNUMBER, NULL, false);
    add_var ("EDOM", GG_DEFNUMBER, NULL, false);
    add_var ("EDQUOT", GG_DEFNUMBER, NULL, false);
    add_var ("EEXIST", GG_DEFNUMBER, NULL, false);
    add_var ("EFAULT", GG_DEFNUMBER, NULL, false);
    add_var ("EFBIG", GG_DEFNUMBER, NULL, false);
    add_var ("EHOSTDOWN", GG_DEFNUMBER, NULL, false);
    add_var ("EHOSTUNREACH", GG_DEFNUMBER, NULL, false);
    add_var ("EHWPOISON", GG_DEFNUMBER, NULL, false);
    add_var ("EIDRM", GG_DEFNUMBER, NULL, false);
    add_var ("EILSEQ", GG_DEFNUMBER, NULL, false);
    add_var ("EINPROGRESS", GG_DEFNUMBER, NULL, false);
    add_var ("EINTR", GG_DEFNUMBER, NULL, false);
    add_var ("EINVAL", GG_DEFNUMBER, NULL, false);
    add_var ("EIO", GG_DEFNUMBER, NULL, false);
    add_var ("EISCONN", GG_DEFNUMBER, NULL, false);
    add_var ("EISDIR", GG_DEFNUMBER, NULL, false);
    add_var ("EISNAM", GG_DEFNUMBER, NULL, false);
    add_var ("EKEYEXPIRED", GG_DEFNUMBER, NULL, false);
    add_var ("EKEYREJECTED", GG_DEFNUMBER, NULL, false);
    add_var ("EKEYREVOKED", GG_DEFNUMBER, NULL, false);
    add_var ("EL2HLT", GG_DEFNUMBER, NULL, false);
    add_var ("EL2NSYNC", GG_DEFNUMBER, NULL, false);
    add_var ("EL3HLT", GG_DEFNUMBER, NULL, false);
    add_var ("EL3RST", GG_DEFNUMBER, NULL, false);
    add_var ("ELIBACC", GG_DEFNUMBER, NULL, false);
    add_var ("ELIBBAD", GG_DEFNUMBER, NULL, false);
    add_var ("ELIBMAX", GG_DEFNUMBER, NULL, false);
    add_var ("ELIBSCN", GG_DEFNUMBER, NULL, false);
    add_var ("ELIBEXEC", GG_DEFNUMBER, NULL, false);
    add_var ("ELNRNG", GG_DEFNUMBER, NULL, false);
    add_var ("ELOOP", GG_DEFNUMBER, NULL, false);
    add_var ("EMEDIUMTYPE", GG_DEFNUMBER, NULL, false);
    add_var ("EMFILE", GG_DEFNUMBER, NULL, false);
    add_var ("EMLINK", GG_DEFNUMBER, NULL, false);
    add_var ("EMSGSIZE", GG_DEFNUMBER, NULL, false);
    add_var ("EMULTIHOP", GG_DEFNUMBER, NULL, false);
    add_var ("ENAMETOOLONG", GG_DEFNUMBER, NULL, false);
    add_var ("ENETDOWN", GG_DEFNUMBER, NULL, false);
    add_var ("ENETRESET", GG_DEFNUMBER, NULL, false);
    add_var ("ENETUNREACH", GG_DEFNUMBER, NULL, false);
    add_var ("ENFILE", GG_DEFNUMBER, NULL, false);
    add_var ("ENOANO", GG_DEFNUMBER, NULL, false);
    add_var ("ENOBUFS", GG_DEFNUMBER, NULL, false);
    add_var ("ENODATA", GG_DEFNUMBER, NULL, false);
    add_var ("ENODEV", GG_DEFNUMBER, NULL, false);
    add_var ("ENOENT", GG_DEFNUMBER, NULL, false);
    add_var ("ENOEXEC", GG_DEFNUMBER, NULL, false);
    add_var ("Exec", GG_DEFNUMBER, NULL, false);
    add_var ("ENOKEY", GG_DEFNUMBER, NULL, false);
    add_var ("ENOLCK", GG_DEFNUMBER, NULL, false);
    add_var ("ENOLINK", GG_DEFNUMBER, NULL, false);
    add_var ("ENOMEDIUM", GG_DEFNUMBER, NULL, false);
    add_var ("ENOMEM", GG_DEFNUMBER, NULL, false);
    add_var ("ENOMSG", GG_DEFNUMBER, NULL, false);
    add_var ("ENONET", GG_DEFNUMBER, NULL, false);
    add_var ("ENOPKG", GG_DEFNUMBER, NULL, false);
    add_var ("ENOPROTOOPT", GG_DEFNUMBER, NULL, false);
    add_var ("ENOSPC", GG_DEFNUMBER, NULL, false);
    add_var ("ENOSR", GG_DEFNUMBER, NULL, false);
    add_var ("ENOSTR", GG_DEFNUMBER, NULL, false);
    add_var ("ENOSYS", GG_DEFNUMBER, NULL, false);
    add_var ("ENOTBLK", GG_DEFNUMBER, NULL, false);
    add_var ("ENOTCONN", GG_DEFNUMBER, NULL, false);
    add_var ("ENOTDIR", GG_DEFNUMBER, NULL, false);
    add_var ("ENOTEMPTY", GG_DEFNUMBER, NULL, false);
    add_var ("ENOTRECOVERABLE", GG_DEFNUMBER, NULL, false);
    add_var ("ENOTSOCK", GG_DEFNUMBER, NULL, false);
    add_var ("ENOTSUP", GG_DEFNUMBER, NULL, false);
    add_var ("ENOTTY", GG_DEFNUMBER, NULL, false);
    add_var ("ENOTUNIQ", GG_DEFNUMBER, NULL, false);
    add_var ("ENXIO", GG_DEFNUMBER, NULL, false);
    add_var ("EOPNOTSUPP", GG_DEFNUMBER, NULL, false);
    add_var ("EOVERFLOW", GG_DEFNUMBER, NULL, false);
    add_var ("EOWNERDEAD", GG_DEFNUMBER, NULL, false);
    add_var ("EPERM", GG_DEFNUMBER, NULL, false);
    add_var ("EPFNOSUPPORT", GG_DEFNUMBER, NULL, false);
    add_var ("EPIPE", GG_DEFNUMBER, NULL, false);
    add_var ("EPROTO", GG_DEFNUMBER, NULL, false);
    add_var ("EPROTONOSUPPORT", GG_DEFNUMBER, NULL, false);
    add_var ("EPROTOTYPE", GG_DEFNUMBER, NULL, false);
    add_var ("ERANGE", GG_DEFNUMBER, NULL, false);
    add_var ("EREMCHG", GG_DEFNUMBER, NULL, false);
    add_var ("EREMOTE", GG_DEFNUMBER, NULL, false);
    add_var ("EREMOTEIO", GG_DEFNUMBER, NULL, false);
    add_var ("ERESTART", GG_DEFNUMBER, NULL, false);
    add_var ("ERFKILL", GG_DEFNUMBER, NULL, false);
    add_var ("EROFS", GG_DEFNUMBER, NULL, false);
    add_var ("ESHUTDOWN", GG_DEFNUMBER, NULL, false);
    add_var ("ESPIPE", GG_DEFNUMBER, NULL, false);
    add_var ("ESOCKTNOSUPPORT", GG_DEFNUMBER, NULL, false);
    add_var ("ESRCH", GG_DEFNUMBER, NULL, false);
    add_var ("ESTALE", GG_DEFNUMBER, NULL, false);
    add_var ("ESTRPIPE", GG_DEFNUMBER, NULL, false);
    add_var ("ETIME", GG_DEFNUMBER, NULL, false);
    add_var ("ETIMEDOUT", GG_DEFNUMBER, NULL, false);
    add_var ("ETOOMANYREFS", GG_DEFNUMBER, NULL, false);
    add_var ("ETXTBSY", GG_DEFNUMBER, NULL, false);
    add_var ("EUCLEAN", GG_DEFNUMBER, NULL, false);
    add_var ("EUNATCH", GG_DEFNUMBER, NULL, false);
    add_var ("EUSERS", GG_DEFNUMBER, NULL, false);
    add_var ("EWOULDBLOCK", GG_DEFNUMBER, NULL, false);
    add_var ("EXDEV", GG_DEFNUMBER, NULL, false);
    add_var ("EXFULL", GG_DEFNUMBER, NULL, false);
    //
    // End of user interfacing variables
    //


    for (i = 0; i < GG_MAX_CODE_LEVEL; i++) gg_resurr[i] = 0; // resurrection count for each code block level set to 0
                                                              // each time level N closes, this count increases
                                                              // so new code block of level N will not see old variables of
                                                              // the same level that are now out of scope 


    if (_main == 1)
    {

        // Generate C code
        // 
        // NOTE: NOTHING IN THIS MAIN CODE CAN HAVE GG_MALLOC OR SUCH - that memory is release at the end of the request, thus anything that needs to be 
        // global must be C's malloc etc.
        //
        GG_VERBOSE(0,"Generating main code");

        oprintf("#include \"golf.h\"\n");
        oprintf("#include \"gg_sparam.h\"\n");
        oprintf("extern gg_num gg_end_program;\n");

        // 
        // code can be generated for standalone (program) version that can be executed from command line
        // or a web-server plug-in for the web
        //

        //
        // ANY variables used in main MUST be outside, or longjmp will destroy them (they cannot be automatic within the function)
        //
        oprintf("volatile gg_num gg_in_request=0;\n");
        oprintf("volatile gg_num gg_done_setjmp=0;\n");
        oprintf("volatile gg_num gg_done_err_setjmp=0;\n");
        oprintf("volatile gg_num gg_in_fatal_exit=0;\n");
        oprintf("sigjmp_buf gg_jmp_buffer;\n");
        oprintf("sigjmp_buf gg_err_jmp_buffer;\n");
        oprintf("char *gg_app_name=\"%s\";\n", gg_app_name);
        // default application path is /<app name>
        oprintf("char * gg_app_path=\"%s\";\n", app_path); // app-path cannot have quotes
        oprintf("unsigned long gg_app_path_len=%lu;\n", strlen(app_path)); 
        oprintf("gg_num gg_is_trace=%ld;\n", gg_is_trace);
        oprintf("gg_num _gg_st=GG_OKAY;\n"); // status used when not specified in statement
        oprintf("char *_gg_st_str;\n"); // status string used when not specified in statement
        oprintf("bool _gg_st_bool = true;\n"); // status boolean used when not specified in statement
        oprintf("gg_num _gg_run_version=0;\n");
        oprintf ("gg_hash gg_dispatch;\n");
        oprintf ("gg_hash gg_paramhash;\n");
        oprintf ("bool gg_true = true;\n");
        oprintf ("bool gg_false = false;\n");
        oprintf ("bool gg_path_changed = false;\n");
        oprintf("gg_num gg_max_upload=%ld;\n", gg_max_upload);
        oprintf("char *gg_root;\n");
        // The following are static variables, i.e. those that need not be seen outside this module
        oprintf ("static gg_db_connections gg_dbs;\n");
        oprintf("static gg_num gg_done_init=0;\n");
        oprintf("static gg_input_req gg_req;\n");
        oprintf("static gg_config *gg_s_pc;\n");

        oprintf("int main (int argc, char *argv[])\n");
        oprintf("{\n");
        // gg_done_init and allocation of config structure are done once per program run
        // this also reads config file
        oprintf("_gg_st_str=GG_EMPTY_STRING;\n"); 
        oprintf("gg_s_pc = gg_alloc_config ();\n");
        oprintf("gg_get_runtime_options();\n");

        //
        // trace_golf must be the very first one, or tracing won't work
        //
        oprintf("if (gg_s_pc->debug.trace_level != 0) gg_open_trace();\n");

        setup_internal_hash(".reqlist","gg_dispatch", true); // setup request hash for near-instant request routing
        setup_internal_hash(".gg_sparam","gg_paramhash", false); // setup hash for list of set-params

        oprintf("gg_memory_init();\n");
        //
        // Set gg_root, this is a constant that will only be available as a copy, just like all other system/app/request information
        // This way such information is stable and cannot be changed by the user (part of memory safety)
        //
        oprintf ("gg_root = GG_ROOT;\n");

        oprintf("umask (077);\n");
        // user-only permissions is used for new dir/files (i.e. mask of 077 which is 700)
        oprintf("gg_get_tz ();\n");
        // first memset base struct, then connections (not the other way around as conn would be wiped out)
        oprintf("   memset (&gg_dbs, 0, sizeof(gg_dbs));\n");
        if (totconn == 0) oprintf("   gg_dbs.conn = NULL;\n"); else oprintf("   gg_dbs.conn = calloc (%ld, sizeof(one_db));\n    if (gg_dbs.conn ==  NULL) GG_FATAL (\"Cannot allocate database array memory\");\n", totconn);
        oprintf("   gg_s_pc->ctx.db = &gg_dbs;\n");
        oprintf("   gg_s_pc->ctx.db->ind_current_db=-1;\n"); // no db used yet
        // Generate name of database connections
        for (i = 0; i < totconn; i++)
        {
            oprintf ("   gg_s_pc->ctx.db->conn[%ld].db_name = \"%s\";\n", i, gg_get_config()->ctx.db->conn[i].db_name);
            oprintf ("   gg_s_pc->ctx.db->conn[%ld].db_type = %ld;\n", i, gg_get_config()->ctx.db->conn[i].db_type);
            oprintf ("   gg_s_pc->ctx.db->conn[%ld].dbc = NULL;\n", i); // not connected yet
            oprintf ("   gg_s_pc->ctx.db->conn[%ld].exit_on_error = 1;\n", i); // all db errors fatal
        }
        oprintf ("  gg_s_pc->ctx.tot_dbs = %ld;\n", totconn); // all db errors fatal
        oprintf("GG_UNUSED (argc);\n");
        oprintf("GG_UNUSED (argv);\n");
        //
        // Setup crash handler. Also print out all shared libraries loaded and their start/end addresses.
        //
        oprintf("if (!gg_done_init) gg_set_crash_handler (gg_s_pc->app.trace_dir);\n");
        //
        // Initialize curl
        //
        oprintf("#ifdef GG_CURL_USED\nif (!gg_done_init) curl_global_init(CURL_GLOBAL_ALL);\n#endif\n");
        //
        // Initialize crypto
        //
        oprintf("#ifdef GG_CRYPTO_USED\nif (!gg_done_init) gg_sec_load_algos();\n#endif\n");


        oprintf ("while(gg_SERVICE_Accept() >= 0) {\n");
        oprintf ("gg_in_request = 1;\n");

        // start request anew
        oprintf("gg_reset_config (gg_s_pc);\n");

        // need to startprint, it is automatic with first gg_printf

        oprintf("gg_init_input_req(&gg_req);\n");

        oprintf("gg_s_pc->ctx.req = &gg_req;\n"); 
        oprintf("gg_req.app = &(gg_s_pc->app);\n");


        oprintf("GG_TRACE (\"STARTING REQUEST [%%s]\", gg_s_pc->app.trace_dir);\n");

        // input parameter(s) from the SERVICE process manager (mgrg),  but present for standalone too
        oprintf ("gg_s_pc->ctx.req->args.num_of_args = argc - 1;\ngg_s_pc->ctx.req->args.args = argv + 1;\n");

        // This is if there was an error caught in the request, go to handler after all requests
        oprintf("gg_num ret_val = sigsetjmp(gg_err_jmp_buffer, 1);\n");
        oprintf("if (ret_val != 0) goto end_point;\n");
        oprintf("gg_done_err_setjmp = 1;\n");

        // this must be AFTER the above sigsetjmp() in order for error handling to work in case chdir cannot be done
        // which would be for instance if home directory was delete between change directory and here
        oprintf ("if (gg_path_changed) {if (chdir (gg_s_pc->app.home_dir) != 0) gg_report_error(\"Cannot find Golf home directory [%%s]\", gg_s_pc->app.home_dir);}\n");

        // this gets input from either web server or from the environment
        // We check if return is 1 or 0. If not 1, this means there was a problem, but it was
        // handled (such as Forbidden reply) (otherwise there would be an erorring out). If return
        // value is 0, we just go directly to gg_shut() to flush the response out.
        //
        oprintf("if (gg_get_input(&gg_req, NULL, NULL) == 1)\n");
        oprintf("{\n");
        // main function that handles everything - programmer must implement this or use generated
        oprintf("gg_dispatch_request();\n");

        // if there is a transaction that wasn't explicitly committed or rollbacked
        // rollback it and error out, as this is a programming bug.
        oprintf("gg_check_transaction (0);\n");
        oprintf("}\n");

        // we can jump here and gg_req will be valid because long jump to above (where we goto end_point) is valid only between above sigsetjmp and below
        // setting of gg_done_err_setjmp to 0; outside of this block of code we just _Exit()
        oprintf("end_point:\n");


        //
        // gg_shut MUST ALWAYS be called at the end - no request can bypass it
        //
        oprintf("gg_shut(&gg_req);\n");
        // now we now at this point gg_req is valid; if it was NULL, gg_shut would exit

        // by setting gg_done_err_setjmp to 0 here, we make sure that report-error will jump to 
        // end-point only between above setting gg_done_err_setjump to 1 
        // and here where we set it to 0. Because gg_req is valid in this case, we can do gg_shut after end_point:
        oprintf("gg_done_err_setjmp = 0;\n");

        // All data must be flushed at the end of the request, no none is outstanding somehow later
        oprintf("gg_SERVICE_Finish();\n"); 
        // if gg_end_program set to 1:
        // All db connections (gg_end_all_db) will be closed and program exists
        // For web program, we exit here when program catches SIGTERM, and the exit code is always 0.
        // For command line, we use exit-code value, if set.
        oprintf("if (gg_end_program == 1) { gg_end_all_db(); gg_exit ();}\n"); // if SIGHUP or SIGTERM happened
        oprintf ("gg_in_request = 0;\n");
        //
        // gg_memory_init() MUST be at the very end befoe the loop goes back to the next request
        // if it were anywhere before (it used to be before gg_SERVICE_Finish()), the memory gets released
        // and all references fail with random values or sigsegv (for instance req->ec would be like that in gg_SERVICE_Finish).
        // DO NOT PLACE ANY CODE BETWEEN gg_memory_init() and <curlybrace> that ends the request processing
        //
        oprintf("gg_memory_init();\n");
        oprintf("}\n");
        oprintf("return 0;\n");

        oprintf("}\n");

        GG_VERBOSE(0,"End generating main code");
    }
    else
    {
        GG_VERBOSE(0,"Generating code for [%s]", src_file_name);
        gg_gen_c_code (gen_ctx, src_file_name);
    }

    oprintf("// END OF GENERATED CODE\n");
    oprintf (NULL); //  flush output
    if (outf != NULL) fclose (outf);

    // release of memory done automatically by the OS and likely faster, so no
    // gg_done();
    // check if errors raised, if so exit with code 1
    if (gg_curr_errors>0) return 1; else return 0;
}


