// SPDX-License-Identifier: Apache-2.0
// Copyright 2018-2025 Gliim LLC. 
// Licensed under Apache License v2. See LICENSE file.
// On the web http://rimstone-lang.com/ - this file is part of RimStone framework. 

// 
// Main RIM processor. Takes input and output parameters from the command line
// and generates C file ready for compilation.
//

#include "rim.h"


// 
//
// Defines (most of them)
//
//

// Maximum number of SQL queries in a single file, otherwise unlimited in application
// Queries's code is generated based on the name of the query, and not the index into an array.
// This limit is for internal code generation purposes.
#define RIM_MAX_QUERY 300
// level of queries to allow for nesting (i.e. query inside a query)
#define RIM_MAX_QUERY_NESTED 10
// maximum # of input parameters, '%s' in a query, per query
#define RIM_MAX_QUERY_INPUTS 200
// maximum length of input parameter, be it C code, or a string. This is the length of what goes in the 
// SQL query input, per input.
#define RIM_MAX_QUERY_INPUT_LEN 250
// maximum length of input line in a  source code file.
#define RIM_FILE_LINE_LEN 65536
// maximum length of file name
#define RIM_FILE_NAME_LEN 200
// maximum space to write out all output columns of a query
#define RIM_TOT_COLNAMES_LEN (RIM_MAX_COLNAME_LEN * RIM_MAX_QUERY_OUTPUTS)
// maximym length of query name, which is the name in run-query...=
#define RIM_MAX_QUERYNAME_LEN 200
// keep track up to 10 levels in if-true statement
#define RIM_MAX_OTHER 31 
// maximum length of any error this utility may produce
#define RIM_MAX_ERR_LEN 12000
// various keywords used. What's recognized is a keyword. Only keywords that have something following them
// have a space afterwards. No whitespace other than space can be used in commands!
#define RIM_KEYMAXSIZE "max-size "
#define RIM_KEYHASHSIZE "hash-size "
#define RIM_KEYWITHVALUE "with-value "
#define RIM_KEYREPLACE "replace"
#define RIM_KEYTOERROR "to-error"
#define RIM_KEYPARAM "param "
#define RIM_KEYEXIT "exit"
#define RIM_KEYHOME "home"
#define RIM_KEYOPTIONS "options "
#define RIM_KEYNEWLINE "new-line"
#define RIM_KEYMODE "mode "
#define RIM_KEYCREATE "create "
#define RIM_KEYTYPE "type "
#define RIM_KEYSOFTLINK "soft-link "
#define RIM_KEYHARDLINK "hard-link "
#define RIM_KEYONERRORCONTINUE "on-error-continue"
#define RIM_KEYONERROREXIT "on-error-exit"
#define RIM_KEYADD "add "
#define RIM_KEYON "on"
#define RIM_KEYOFF "off"
#define RIM_KEYERRNO "errno"
#define RIM_KEYWIDTH "width "
#define RIM_KEYFORMATFLOATING "format-floating"
#define RIM_KEYFORMATSCIENTIFIC "format-scientific"
#define RIM_KEYFORMATCOMPACT "format-compact"
#define RIM_KEYPRECISION "precision "
#define RIM_KEYPREFIX "prefix "
#define RIM_KEYNAME "name "
#define RIM_KEYNAME0 "name"
#define RIM_KEYSIZE "size "
#define RIM_KEYEXTERNALCALL "external-call"
#define RIM_KEYPUT "put"
#define RIM_KEYGET "get "
#define RIM_KEYSET "set "
#define RIM_KEYBASE "base "
#define RIM_KEYFIRST "first"
#define RIM_KEYLAST "last"
#define RIM_KEYEND "end"
#define RIM_KEYPREVIOUS "previous"
#define RIM_KEYNEXT "next"
#define RIM_KEYKEY "key "
#define RIM_KEYRETURNVALUE "return-value "
#define RIM_KEYRETURNCODE "return-code "
#define RIM_KEYSETPARAM "set-param "
#define RIM_KEYGETPARAM "get-param "
#define RIM_KEYGETLESSER "get-lesser"
#define RIM_KEYLESSER "lesser "
#define RIM_KEYLESSERTHAN "lesser-than "
#define RIM_KEYLESSEREQUAL "lesser-equal "
#define RIM_KEYGETGREATER "get-greater"
#define RIM_KEYOSUSER "os-user"
#define RIM_KEYCURRENT "current"
#define RIM_KEYGREATER "greater "
#define RIM_KEYCONTAIN "contain "
#define RIM_KEYNOTCONTAIN "not-contain "
#define RIM_KEYGREATERTHAN "greater-than "
#define RIM_KEYEVERY "every "
#define RIM_KEYSOURCEFILE "source-file"
#define RIM_KEYSOURCELINE "source-line"
#define RIM_KEYNOTEVERY "not-every "
#define RIM_KEYGREATEREQUAL "greater-equal "
#define RIM_KEYUPDATEKEY "update-key "
#define RIM_KEYUPDATEVALUE "update-value "
#define RIM_KEYKEYAS "key-as "
#define RIM_KEYRUNDIR "run-dir"
#define RIM_KEYUNSORTED "unsorted"
#define RIM_KEYNOENUM "no-enum"
#define RIM_KEYMAXKEY "max-key"
#define RIM_KEYMINKEY "min-key"
#define RIM_KEYKEYLIST "key-list "
#define RIM_KEYVALUE "value "
#define RIM_KEYNEWCURSOR "new-cursor "
#define RIM_KEYIFVAR "if-true "
#define RIM_KEYCOPY "copy "
#define RIM_KEYCOPYEND "copy-end "
#define RIM_KEYREPEAT "repeat "
#define RIM_KEYREWIND "rewind "
#define RIM_KEYPASSWORD "password "
#define RIM_KEYPRIVATE "private"
#define RIM_KEYPUBLIC "public"
#define RIM_KEYINPUTLENGTH "input-length "
#define RIM_KEYITERATIONS "iterations "
#define RIM_KEYSALT "salt "
#define RIM_KEYSALTLENGTH "salt-length "
#define RIM_KEYINITVECTOR "init-vector "
#define RIM_KEYBINARY "binary"
#define RIM_KEYHTML "html "
#define RIM_KEYPOSITION "position "
#define RIM_KEYUSE "use "
#define RIM_KEYWITH "with "
#define RIM_KEYDELIMITER "delimiter "
#define RIM_KEYDELETE0 "delete"
#define RIM_KEYDELETE "delete "
#define RIM_KEYNOLOOP "no-loop"
#define RIM_KEYFORJSON "for-json"
#define RIM_KEYLOCALFILE "local-file "
#define RIM_KEYCLIENTFILE "client-file "
#define RIM_KEYEXTENSION "extension "
#define RIM_KEYOLDVALUE "old-value "
#define RIM_KEYCIPHER "cipher "
#define RIM_KEYSTACKDEPTH "stack-depth "
#define RIM_KEYDIGEST "digest "
#define RIM_KEYSKIPDATA "skip-data"
#define RIM_KEYCRYPTO "crypto"
#define RIM_KEYPROCESSSCOPE "process-scope"
#define RIM_KEYSTARTWITH "start-with "
#define RIM_KEYIN "in "
#define RIM_KEYID "id "
#define RIM_KEYTO "to "
#define RIM_KEYISSERVICE "is-service"
#define RIM_KEYSTATUS "status "
#define RIM_KEYENDOFFILE "end-of-file "
#define RIM_KEYFROMCONDITION "from-condition "
#define RIM_KEYFROM "from "
#define RIM_KEYFROMLENGTH "from-length "
#define RIM_KEYTRAVERSE "traverse"
#define RIM_KEYUNQUOTED "unquoted"
#define RIM_KEYBEGIN "begin"
#define RIM_KEYAPPEND "append"
#define RIM_KEYLENGTH "length "
#define RIM_KEYRESPONSE "response "
#define RIM_KEYRESPONSECODE "response-code "
#define RIM_KEYCERT "cert "
#define RIM_KEYCOOKIEJAR "cookie-jar "
#define RIM_KEYNUMBER "number"
// no-cert has NO space afterwards because it doesn't need anything to follow it
#define RIM_KEYNOCERT "no-cert"
#define RIM_KEYBUFFERED "buffered"
#define RIM_KEYDEFAULT "default"
#define RIM_KEYDEFAULTVALUE "default-value "
#define RIM_KEYERROR "error "
#define RIM_KEYERRORFILE "error-file "
#define RIM_KEYERRORTEXT "error-text "
#define RIM_KEYERRORPOSITION "error-position "
#define RIM_KEYERRORLINE "error-line "
#define RIM_KEYERRORCHAR  "error-char "
#define RIM_KEYROWCOUNT "row-count "
#define RIM_KEYAFFECTEDROWS "affected-rows "
#define RIM_KEYNEWTRUNCATE "new-truncate"
#define RIM_KEYSECURE "secure"
#define RIM_KEYEXPIRES "expires "
#define RIM_KEYAPPPATH "app-path "
#define RIM_KEYPATH "path "
#define RIM_KEYFILEID "file-id "
#define RIM_KEYDIRID "dir-id "
#define RIM_KEYSAMESITE "same-site "
#define RIM_KEYDIRECTORY "directory"
#define RIM_KEYUSER "user "
#define RIM_KEYAPPLICATION "application "
#define RIM_KEYUSERDIRECTORY "user-directory"
#define RIM_KEYSOCKETFILE "socket-file"
#define RIM_KEYFILEDIRECTORY "file-directory"
#define RIM_KEYUPLOADSIZE "upload-size"
#define RIM_KEYDBVENDOR "db-vendor "
#define RIM_KEYNOHTTPONLY "no-http-only"
#define RIM_KEYOUTPUT "output "
#define RIM_KEYMETHOD "method "
#define RIM_KEYLOCATION "location "
#define RIM_KEYLOCAL "local "
#define RIM_KEYALGORITHM "algorithm "
#define RIM_KEYDESCENDING "descending"
#define RIM_KEYARGS "args "
#define RIM_KEYAVERAGEREADS "average-reads "
#define RIM_KEYOUTPUTFILE "output-file "
#define RIM_KEYINPUTFILE "input-file "
#define RIM_KEYSUBJECT "subject "
#define RIM_KEYHEADER "header "
#define RIM_KEYHEADERS "headers "
#define RIM_KEYRESPONSEHEADERS "response-headers "
#define RIM_KEYREQUESTHEADERS "request-headers "
#define RIM_KEYREQUESTPATH "request-path "
#define RIM_KEYFILES "files "
#define RIM_KEYFIELDS "fields "
#define RIM_KEYREQUESTBODY "request-body "
#define RIM_KEYCONTINUE "continue"
#define RIM_KEYCONTENT "content "
#define RIM_KEYCONTENTTYPE "content-type "
#define RIM_KEYCONTENTLENGTH "content-length "
#define RIM_KEYURLPARAMS "url-params "
#define RIM_KEYTHREADID "thread-id "
#define RIM_KEYETAG "etag"
#define RIM_KEYFILENAME "file-name "
#define RIM_KEYNOCACHE "no-cache"
#define RIM_KEYCACHECONTROL "cache-control "
#define RIM_KEYCACHE "cache"
#define RIM_KEYCLEARCACHE "clear-cache "
#define RIM_KEYSTATUSID "status-id "
#define RIM_KEYSTATUSTEXT "status-text "
#define RIM_KEYCUSTOM "custom "
#define RIM_KEYDOWNLOAD "download"
#define RIM_KEYEQUAL "equal "
#define RIM_KEYEQUALSHORT "="
#define RIM_KEYCOMMA ","
#define RIM_KEYCOMMENT "//"
#define RIM_KEYCURLYLEFT "{"
#define RIM_KEYCURLYRIGHT "}"
#define RIM_KEYPARRIGHT ")"
#define RIM_KEYPARLEFT "("
#define RIM_KEYPLUS "+"
#define RIM_KEYAT "@"
#define RIM_KEYCOLON ":"
#define RIM_KEYNOTEQUAL "not-equal "
#define RIM_KEYAND0 "&&"
#define RIM_KEYOR0 "||"
#define RIM_KEYNOT0 "!"
#define RIM_KEYAND "and "
#define RIM_KEYOR "or "
#define RIM_KEYNOT "not"
#define RIM_KEYNOENCODE "noencode"
#define RIM_KEYWEBENCODE "web-encode"
#define RIM_KEYURLENCODE "url-encode"
#define RIM_KEYURLPATH "url-path "
#define RIM_KEYDATA "data "
#define RIM_KEYEXITSTATUS "exit-status "
#define RIM_KEYYEAR "year "
#define RIM_KEYMONTH "month "
#define RIM_KEYDAY "day "
#define RIM_KEYHOUR "hour "
#define RIM_KEYMIN "minute "
#define RIM_KEYSEC "second "
#define RIM_KEYTIMEOUT "timeout "
#define RIM_KEYTIMEZONE "timezone "
#define RIM_KEYFORMAT "format "
#define RIM_KEYFOR "for "
#define RIM_KEYREPLACEWITH "replace-with "
#define RIM_KEYRESULT "result "
#define RIM_KEYNOTRIM "notrim"
#define RIM_KEYCASEINSENSITIVE "case-insensitive"
#define RIM_KEYERRORMARGIN "error-margin "
#define RIM_KEYUTF "utf"
#define RIM_KEYTEMPORARY "temporary"
#define RIM_KEYSINGLEMATCH "single-match"
#define RIM_KEYENVIRONMENT "environment "
#define RIM_KEYOSNAME "os-name"
#define RIM_KEYOSDIR "os-dir"
#define RIM_KEYOSVERSION "os-version"
#define RIM_KEYARGCOUNT "arg-count"
#define RIM_KEYCOUNT "count "
#define RIM_KEYOPENANGLEBRACKET "["
#define RIM_KEYPOUND "#"
#define RIM_KEYDOLLAR "$"
#define RIM_KEYHOPS "hops "
#define RIM_KEYARGVALUE "arg-value "
#define RIM_KEYREFERRINGURL "referring-url"
#define RIM_KEYPROCESSID "process-id"
#define RIM_KEYCOOKIECOUNT "cookie-count"
#define RIM_KEYCOOKIE "cookie "
#define RIM_KEYINPUT "input "
#define RIM_KEYFINISHEDOKAY "finished-okay "
#define RIM_KEYSTARTED "started "
#define RIM_KEYSINCEEPOCH "since-epoch"
#define RIM_KEYFROMEPOCH "from-epoch "
// Input or output variable
#define RIM_OUT 0
#define RIM_IN 1
#define RIM_INOUT 2
// maximum length of generated code line (in .c file, final line)
#define RIM_MAX_CODE_LINE RIM_FILE_LINE_LEN
// error messages
#define RIM_STMT_FAIL_CHECK "File [%s], line [%ld], Statement failed without status being obtained and/or checked [%ld]"
#define RIM_NAME_INVALID "Name [%s] is not valid, must be a valid identifier because a variable is required in this context, or this clause has already been specified"
#define RIM_VAR_NOT_EXIST "Variable [%s] does not exist"
#define RIM_MSG_NESTED_QRY "Qry ID [%ld] is nested too deep, maximum nesting of [%d]"
#define RIM_MSG_SRC_NAME "Source file name [%s] is not valid. Use underscore and double underscore in source file name, and hyphen and forward slash in request name, respectively"
#define RIM_MSG_BAD_TYPE "Unknown expression type or bad syntax for an expression"
// maximum length of a column name in db select query result 
#define RIM_MAX_COLNAME_LEN 64
// pre-processing status of a qry, describes the way it is used in the code
#define RIM_QRY_UNUSED 0
#define RIM_QRY_USED 1
#define RIM_QRY_INACTIVE 0
#define RIM_QRY_ACTIVE 1
// used as a silly placeholder to replace with actual length in rim_puts to increase output performance
#define RIM_EMPTY_LONG_PLAIN_ZERO 0

//guard against if (..) rim_statement, causes if (..) char ...; which is illegal
#define RIM_GUARD   oprintf("char _rim_statement_must_be_within_code_block_here_%ld; RIM_UNUSED (_rim_statement_must_be_within_code_block_here_%ld);\n", vnone, vnone), vnone++;
// check if query returns a tuple, if not, cannot use row-count on it
#define RIM_CHECK_TUPLE(k) if (gen_ctx->qry[k].returns_tuple == 0) rim_report_error( "row-count cannot be used on a query because it does not output any columns")
// current query id - 0 for the first, 1 for nested in it, 2 for nested further, then 0 again for the one next to the first etc.
#define query_id (gen_ctx->curr_qry_ptr-1)

// cipher+digest for encryption (default)
#define RIM_DEF_DIGEST "\"sha256\""
#define RIM_DEF_CIPHER "\"aes-256-cbc\""

// modes of compilation
#define RIM_MODE_SAFE 0
#define RIM_MODE_EXTENDED 1
// string constant, is it, and if so, what kind, see is_constant_string()
#define RIM_CONST_OK 0
#define RIM_CONST_NOT 2
//
// Variables
// Decorated variable name max length
#define RIM_MAX_DECNAME 200
// Max number of levels for code blocks
#define RIM_MAX_CODE_LEVEL 30 
// Max clauses in a statement, that have no data. Used to check there's no extraneous data after them
#define RIM_MAX_CLAUSE 50
// Backward-compatibility print functions (since 600.3.28)
#define RIM_P_SOURCELINE 2
#define RIM_P_SOURCEFILE 3
#define RIM_P_NUM 4
#define RIM_P_URL 5
#define RIM_P_WEB 6
//
//when using undecorate , get just the part passed the flat source directory
#define RIM_BLD_DIR_OFFSET rim_bld_dir_len + strlen (".flatsrc") + 1
//
#define RIM_IS_OCTAL(a) ((a)>='0' && (a)<='7')
//


//
//
// Type definitions
//
//

// Variable name and type and line number where defined
typedef struct s_rim_var
{
    char *name; // generated name
    rim_num type;
    rim_num lnum;
    char *rname; // real variable name
    bool used; // true if variable asked for (i.e. used)
} rim_var;


// 
// Query structure, describes run-query and all that it entails
//
typedef struct rim_qry_info_t
{
    char *text; // sql text of query
    char name[RIM_MAX_QUERYNAME_LEN + 1]; // name of query, generated by RimStone and used for naming variables and such

    rim_num returns_tuple; // 0 if this doesn't return a tuple, like INSERT, but not INSERT..RETURNING(), 1 otherwise

    // 1 if unknown outputs
    char unknown_output;

    // number of, and qry outputs 
    rim_num qry_total_outputs; 
    char *qry_outputs[RIM_MAX_QUERY_OUTPUTS + 1];
    
    // input parameters from URL bound to each query
    char qry_inputs[RIM_MAX_QUERY_INPUTS + 1][RIM_MAX_QUERY_INPUT_LEN + 1];

    // number of query inputs actually found as opposed to the number of '%s'
    rim_num qry_found_total_inputs; 

    // the database on which this index operates, used to switch back when end-query is encountered
    // note this is not spanning the code, but only set when needed. In other words, *every* db operation
    // has this set.
    rim_num ind_current_db;
} qry_info;
// 
// Context data for RIM preprocessor. Used to hold information
// during the preprocessing.
//
typedef struct rim_gen_ctx_s
{
    // list of db queries - actual SQL with %s for input params
    // from URL
    qry_info qry[RIM_MAX_QUERY + 1]; // used for generation code phase only

    rim_num qry_used[RIM_MAX_QUERY + 1]; // whether the query was used in the past or not
    rim_num qry_active[RIM_MAX_QUERY + 1]; // whether the query is active now or not
    rim_num total_queries; // total number of queries that we have. 
    rim_num total_write_string; // used to detect unclosed write-strings

    // when nesting, query IDs are indexes into qry_info.
    // curr_qry_ptr pointing to one just above the deepest. 
    rim_num curr_qry_ptr;


} rim_gen_ctx;

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
// on_error_action is RIM_ON_ERROR_CONTINUE/EXIT for statement-specific on-error continue/exit or RIM_OKAY if db-level on-error is in effect.
//
typedef struct s_rim_db_parse {
    char on_error_action;
    char *on_error_exit;
    char *on_error_cont;
    char *with_output;
    char *colon;
    char *query_result;
    rim_num cur_query_result;
    char *eq;
    char *at;
    char *noloop;
    char *err;
    char *errtext;
    char *arows;
    char *rcount;
} rim_db_parse;

typedef struct s_rim_if {
    bool else_done; // true if else with no condition done, false otherwise
    rim_num num_elses; // how many elses in this if, need that many } closings, since else-if is "} else { if ()... }"
} rim_if;



// 
//
// Global variables (and a few related macros)
//
//

bool rim_check_lib = true; // check if libraries are present; do not check when in debug mode
char *out_file_name = NULL; // file name that's output for .rim file
// what libs we use
bool rim_mod_curl = false;
bool rim_mod_xml = false;
bool rim_mod_pcre2 = false;
bool rim_mod_crypto = false;
bool rim_mod_mariadb = false;
bool rim_mod_sqlite = false;
bool rim_mod_postgres = false;
bool rim_mod_array = false;
bool rim_mod_tree = false;
bool rim_mod_service = false;
//
char rim_bld_dir[RIM_MAX_OS_UDIR_LEN]; // build directory
int rim_bld_dir_len =0;
char rim_user_dir[RIM_MAX_OS_UDIR_LEN]; // user directory
rim_num rim_pscope = 0; // counter of process-scoped "do once" sections of initialization
rim_num rim_sres_id = 0; // id for strings vars in expressions; must be global because string expressions can be recursive too
bool rim_no_output_line_num = false; // true if oprintf writes #line, in which case we don't
                                    // want to write #line inside oprintf again
rim_num rim_last_line_file_pos=0; // file position after writing last #line
rim_num rim_last_line=0; // last #line written (line number)
char *rim_valid_rim_name=""; // proper relative path from source directory plus file name for rim source files
char *rim_valid_rim_dir=""; // source directory path (together with rim_valid_rim_name, it's the full path of a source file)
rim_num rim_var_internal = 0; // counter for internal variables (such as used in expressions for arrays), goes up to approx 46,000
rim_num rim_curr_errors = 0; // current number of errors
rim_num rim_max_errors = 5; // max number of errors before giving up
bool rim_ignore_warn = false; // if true, do not display warnings
bool rim_single_file = false; // by default request handlers can be grouped in same-prefix files, with this being true, each must be in own file
bool rim_public = false; // by default all request handlers are private unless made public, with --public this flag will be switched to true
bool do_once_open = false; // true if do-once open
rim_num do_once_level; // the scope level at the beginning of do-once
char *emptyc[RIM_MAX_CLAUSE]; // empty statement
rim_num emptyc_c = 0; // counter for finding empty clauses (0 in carve_statement)
char *oline = NULL;
rim_num oline_len = 0; // length of buffer
rim_num oline_prev_line_beg = 0; // beginning of prev line
rim_num oline_size = 0; // size in bytes written 
rim_num oline_written = 0; // current # of bytes written 
bool rim_is_sub = false; // true if this is request handler with call-handler clause
// Resurrection count for a level: each time code block level drops, resurr count increases
// this is to make sure "old" level N does not interfere with "new" level N
rim_num rim_resurr[RIM_MAX_CODE_LEVEL+1];
rim_hash *rim_varh = NULL; // hash of variable names/types etc
rim_num rim_level = 0; // level of code blocks, used to decorate variable names
rim_num num_recog = 0; // number of recognized statements
bool after_started = false; // true if passed after-handler
bool before_started = false; // true if passed before-handler
bool after_ended = false; // true if passed end-after-handler
bool before_ended = false; // true if passed end-before-handler
rim_num rim_mode = RIM_MODE_SAFE; // mode of compilation, depends which features are enfored
rim_num wcurs = 0; // id of a tree cursor
bool done_handler = false; // true if begin-handler is found
bool done_end_handler = false; // true if end-handler is found
rim_if if_nest[RIM_MAX_OTHER+1] = {{false,0}}; // set all to false and 0
rim_num vnone = 0; // used in detecting if () rim_statement
char line[RIM_FILE_LINE_LEN + 1]; // buffer for each line from HTML file
rim_num open_loops = 0; // number of start-loop currently open
rim_num open_queries = 0; // number of queries currently open
rim_num rim_max_upload = 25000000; // max upload default
rim_num rim_client_timeout = 5; // client time out default (how many seconds will rimstone server wait before disconnecting client if no
                              // data sent after connecting
char *app_path = ""; // app prefix not defined
char *rim_app_name=NULL;
char rim_dbconf_dir[RIM_MAX_OS_UDIR_LEN];
rim_num lnum = 0; // current line worked on 
char *src_file_name = NULL; // file_name of the source
rim_num print_mode = 0; // 1 if @  to print out
rim_num die_now = 0; // used in rimstone run-time only for now to gracefully exit program, not used here (not yet,
                // and maybe not ever).
FILE *outf = NULL; // rimstone output file (.c generated file)
rim_num usedRIM = 0; // 1 if rimstone statement is used on the line
rim_num rim_is_inline = 0; // if 1, the last statement just processed was inline
rim_num within_inline = 0; // if statement is within <<...>>
rim_num last_line_readline_closed = 0; // line number of the last read-line that has closed
rim_num last_line_string_closed = 0; // line number of the last IF that has closed
rim_num last_line_number_closed = 0; // line number of the last IF that has closed
// setup variables to be able to report the location of unclosed readline
#define check_next_readline {if (open_readline==0) last_line_readline_closed = lnum; open_readline++;}
// setup variables to be able to report the location of unclosed IF
rim_num last_line_loop_closed = 0; // line number of where the last loop closed
rim_num last_line_query_closed = 0; // line number of where the last query closed
// setup variables to report the location of unclosed query
#define check_next_query {if (open_queries==0) last_line_query_closed = lnum; open_queries++;}
// setup variables to report the location of unclosed loop
#define check_next_loop {if (open_loops==0) last_line_loop_closed = lnum; open_loops++;}
rim_num no_rim_line = 0; // if 1, no #line generated
rim_num total_exec_programs=0; // enumerates instance of exec-program so generated argument array variables are unique
rim_num total_body=0; // enumerates instance of call-web for body fields and files
rim_num total_fcgi_arr=0; // enumerates instance of new-remote for env
// Application name - we must know it in order to figure out database config file
// Database connections
rim_db_connections rim_dbs;
rim_num totconn = 0;
//
// satisfy gcc, this is for signal handling for SERVICE/standalone programs, no purpose in gg
rim_num rim_in_request=0;
rim_num volatile rim_done_err_setjmp=0;
rim_num volatile rim_in_fatal_exit=0;
rim_num volatile rim_done_setjmp=0;
char rim_query_id_str[RIM_MAX_QUERYNAME_LEN + 1]; // query ID being worked on

//
//
// Function/macro declarations
//
//
void init_rim_gen_ctx (rim_gen_ctx *gen_ctx);
void rim_gen_c_code (rim_gen_ctx *gen_ctx, char *file_name);
rim_num recog_statement (char *cinp, rim_num pos, char *opt, char **mtext, rim_num *msize, rim_num isLast, rim_num *is_inline);
rim_num find_query (rim_gen_ctx *gen_ctx);
void new_query (rim_gen_ctx *gen_ctx, rim_db_parse *vp);
void oprintf (char *format, ...)  __attribute__ ((format (printf, 1, 2)));
void get_passed_whitespace (char **s);
void get_until_comma (char **s);
void get_until_whitespace (char **s);
void end_query (rim_gen_ctx *gen_ctx, rim_num close_block);
void get_all_input_param (rim_gen_ctx *gen_ctx, char *iparams);
rim_num terminal_width();
void add_input_param (rim_gen_ctx *gen_ctx, char *inp_par);
void carve_statement (char **statement, char *statement_name, char *keyword, rim_num is_mandatory, rim_num has_data);
rim_num define_statement (char **statement, rim_num type, bool always);
// true if to emit line number in source file, do not do it if gg -x used, lnum>1 is so that the very first line
// isn't emitting #line twice
#define RIM_EMIT_LINE (no_rim_line == 0 && lnum>0)
void parse_param_list (char *parse_list, rim_fifo **params, rim_num *tot);
char *find_keyword(char *str, char *find, rim_num has_spaces);
rim_num find_connection(char *conn);
rim_num add_connection(char *conn, rim_num dbtype);
void get_db_config (char *dbname);
void get_query_output (rim_gen_ctx *gen_ctx, rim_db_parse *vp);
void statement_SQL (rim_gen_ctx *gen_ctx, char is_prep);
void start_loop_query (rim_gen_ctx *gen_ctx);
void generate_sql_code (rim_gen_ctx *gen_ctx, char is_prep);
void generate_query_result (rim_gen_ctx *gen_ctx, rim_db_parse *vp);
void query_execution (rim_gen_ctx *gen_ctx,const rim_num run_query, const rim_num query_result, char is_prep, rim_db_parse *vp);
void prepare_query (rim_gen_ctx *gen_ctx, rim_db_parse *vp);
void process_query (rim_gen_ctx *gen_ctx, const rim_num query_result, const rim_num run_query, char is_prep, rim_db_parse *vp);
char *rim_db_vendor(rim_num dbconn);
rim_num trimit(char *var);
void check_rim (char *c);
char *id_from_file (char *file);
rim_num outargs(char *args, char *outname, char *type, rim_num startwith, char pair, rim_num check_type_val, rim_num check_type_name);
void process_http_header (char *statement, char *header, char *temp_header, rim_num http_header_count, char request, char **content_len, char **content_type);
char *make_default_header(int inittype, rim_num http_header_count, char request);
void on_error_act (char *on_error_cont, char *on_error_exit, char *act);
void envsub ();
void rim_is_valid_app_path (char *name);
void query_result (rim_gen_ctx *gen_ctx, char *mtext, rim_num cur_query_result);
char *opt_clause(char *clause, char *param, char *antiparam, rim_num type);
void name_query (rim_gen_ctx *gen_ctx);
void free_query (char *qryname, bool skip_data);
void convert_puts(char *pline);
void do_numstr (char *to, char *num0, char *base, bool isd, rim_num width, rim_num prec, char *format);
void setup_internal_hash(char *fname, char *hash, bool is_req);
void rim_report_warning (char *format, ...)  __attribute__ ((format (printf, 1, 2)));
rim_num carve_stmt_obj (char **mtext, bool has_value, bool is_constant);
void match_file (char *file_name, char *reqname);
void parse_cond(char *cm, char *bifs, char *ifs, char *ife);
bool make_mem (char **clause);
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
rim_num check_var (char **v, rim_num type);
void check_format(char *mtext, char *comma, char **list);
rim_num is_constant_string (char *s, rim_num *size);
rim_num is_len_format (char *s, char *f, rim_num flen, bool isd);
void check_level(char *prline);
rim_num typeid (char *type);
bool add_var (char *var, rim_num type, char *realname, bool watched_for_unused);
bool find_var (char *name, rim_num *type, rim_num *line, char **realname);
void make_var (char **v, rim_num type, bool internal);
void check_c (char *mtext);
char *check_bool (char *e, bool *found);
char *check_exp (char *e, bool *found, bool isd, bool comma);
char *check_str (char *e, rim_num *type);
void save_param (char *pname, rim_num type);
bool check_sub (char *sub);
char *undecorate (char *src);
void do_print (char *mtext, rim_num backmode);
void do_printf (char *mtext, bool rep_error, rim_gen_ctx *gen_ctx, rim_num backmode);
bool is_reserved (char *word);
bool var_exists (char *name, rim_num type, rim_num *real_type, rim_num *lnum);
char *process_bool_exp (char *v);
char *process_string_exp (char *v, rim_num *type);
char *process_number_exp (char *v, bool isd);
rim_num check_type(rim_num type, rim_num check_against);
bool process_array (char **cur_exp, rim_num *type_exp);
void find_cond (char *cm, char **eq, char **neq, char **less, char **lesseq, char **gr, char **greq, char **mod, char **notmod, char **cont, char **notcont);
char *upcoming_name (char **cur, rim_num *len);
void replace_var (char *cur, rim_num curlen, char *var);
rim_num is_single_quoted_char (char *icur);
char *check_input(char *inp);
char *rec_str (char *cur, rim_num *type, char **s_str, char **s_beg, char **s_len);
bool can_write_line_number(rim_num linepos, rim_num linenum);
void written_line_number(rim_num linepos, rim_num linenum);
char *make_empty_const (rim_num sz);
void mark_lib (char *lib);
bool math_fun_n (char *name, rim_num *params);
bool math_fun (char *name, rim_num *params);
bool str_fun (char *name);
bool c_name (char *name);
rim_num check_num_or_double (char *v, rim_num j);
char *promote_double (char *v);
void process_param_set (char *mtext);
void process_param_get (char *mtext);

//
//
// Implementation of functions used in RIM alone
//
//

//
// Process get-param. mtext is the source of the statement
//
void process_param_get (char *mtext)
{
    while (1) 
    {
        char *comma = find_keyword (mtext, RIM_KEYCOMMA, 0);
        carve_statement (&comma, "get-param", RIM_KEYCOMMA, 0, 1); // separate entries with comma first, so
                                                                  // this find/carve MUST come before the rest below!
                                                                  //
        char *defval = find_keyword (mtext, RIM_KEYDEFAULTVALUE, 1);
        char *type = find_keyword (mtext, RIM_KEYTYPE, 1);
        carve_statement (&type, "get-param", RIM_KEYTYPE, 0, 1);
        carve_statement (&defval, "get-param", RIM_KEYDEFAULTVALUE, 0, 1);
        if (type != NULL) trimit (type); // type has to match
        carve_stmt_obj (&mtext, true, false);

        rim_num t;
        if (type == NULL) define_statement (&mtext, (t=RIM_DEFSTRING), false);
        else define_statement (&mtext, (t=typeid(type)), false);
        //
        save_param (mtext, typeid(typename(t))); // t here *cannot* be process-scope, params are only non-process types
                                         // though you can pass along process-scope at run time of course.
                                         // this normalizes, so RIM_DEFSTRINGSTATIC shows up as RIM_DEFSTRING
        if (defval != NULL)
        {
            if (cmp_type (t, RIM_DEFSTRING))
            {
                make_mem(&defval);
                check_var (&defval, RIM_DEFSTRING);
            }
            else if (cmp_type (t, RIM_DEFDOUBLE))
            {
                check_var (&defval, RIM_DEFDOUBLE);
            }
            else if (cmp_type (t, RIM_DEFNUMBER))
            {
                check_var (&defval, RIM_DEFNUMBER);
            }
            else if (cmp_type (t, RIM_DEFBOOL))
            {
                check_var (&defval, RIM_DEFBOOL);
            }
            else rim_report_error ("Cannot specify default value for this type in get-param, parameter [%s]", mtext);
        }
        //
        // No need to check_var when define_statement is with "true" as the second argument, since it
        // means the variable is created of that type for sure, and it can't have been or will be created before or after.
        //
        //
        {
            if (cmp_type (t, RIM_DEFNUMBER))
            {
                oprintf ("%s=_rim_sprm_par[_rim_aprm_%s].set ? _rim_sprm_par[_rim_aprm_%s].tval.numval:", mtext,mtext,mtext);
                if (defval != NULL) oprintf ("(%s);\n", defval);
                else oprintf ("(rim_report_error (\"Trying to obtain value of parameter [%%s] that was not set\", _rim_sprm_par[_rim_aprm_%s].name),0);\n", mtext);
            }
            else if (cmp_type (t, RIM_DEFDOUBLE))
            {
                oprintf ("%s=_rim_sprm_par[_rim_aprm_%s].set ? _rim_sprm_par[_rim_aprm_%s].tval.dblval:", mtext,mtext,mtext);
                if (defval != NULL) oprintf ("(%s);\n", defval);
                else oprintf ("(rim_report_error (\"Trying to obtain value of parameter [%%s] that was not set\", _rim_sprm_par[_rim_aprm_%s].name),0);\n", mtext);
            }
            else if (cmp_type (t, RIM_DEFBOOL)) 
            {
                oprintf ("%s=_rim_sprm_par[_rim_aprm_%s].set ? _rim_sprm_par[_rim_aprm_%s].tval.logic:", mtext,mtext,mtext);
                if (defval != NULL) oprintf ("(%s);\n", defval);
                else oprintf ("(rim_report_error (\"Trying to obtain value of parameter [%%s] that was not set\", _rim_sprm_par[_rim_aprm_%s].name),0);\n", mtext);
            }
            else 
            {
                oprintf("{ typeof(%s) _rim_param = " , mtext);
                oprintf("rim_get_input_param (_rim_aprm_%s, %ld, %s);\n", mtext, t, defval==NULL?"NULL":defval);
                //
                if (cmp_type(t,RIM_DEFSTRING)) oprintf ("rim_mem_add_ref (_rim_param, 0);\n");
                oprintf ("%s = _rim_param;}\n", mtext);
            }
        }

        // we used to forbid non-string/bool/number in non-call-handler. But we should be able to create say index in call-handler and pass it back
        // to caller. In rimrt.c, we error out if such param with such type is not found.
        // if ((t != RIM_DEFNUMBER && t != RIM_DEFBOOL && t != RIM_DEFSTRING) && !rim_is_sub) rim_report_error ("get-param type [%s] can only be used within call-handler", typename(t));

        if (comma == NULL) break; else mtext = comma;
    }
}

//
// Process set-param. mtext is the source of the statement
//
void process_param_set (char *mtext)
{
    while (1) 
    {
        char *comma = find_keyword (mtext, RIM_KEYCOMMA, 0);
        carve_statement (&comma, "set-param", RIM_KEYCOMMA, 0, 1); // separate entries with comma first, so
                                                                  // this find/carve MUST come before the rest below!
                                                                  //
        char *eq = find_keyword (mtext, RIM_KEYEQUALSHORT, 0);

        carve_statement (&eq, "set-param", RIM_KEYEQUALSHORT, 0, 1);
        carve_stmt_obj (&mtext, true, false);
        if (rim_is_valid_param_name(mtext, false, false) != 1) rim_report_error( "parameter name [%s] is not valid, it can have only alphanumeric characters and underscores, and must start with an alphabet character", mtext);

        if (eq == NULL) eq = rim_strdup (mtext); // if no =, assume it's the variable with the same name
        make_mem(&eq);
        rim_num type = check_var (&eq, RIM_DEFUNKN);
        if (type == RIM_DEFUNKN) rim_report_error (RIM_VAR_NOT_EXIST, eq);
        save_param (mtext, typeid(typename(type))); // t here *cannot* be process-scope, params are only non-process types
                                         // though you can pass along process-scope at run time of course.
                                         // this normalizes, so RIM_DEFSTRINGSTATIC shows up as RIM_DEFSTRING



        //
        if (cmp_type(type, RIM_DEFNUMBER)) 
        {
            oprintf ("_rim_sprm_par[_rim_aprm_%s].tval.numval = %s;\n", mtext, eq);
            oprintf ("_rim_sprm_par[_rim_aprm_%s].set = true;\n", mtext);
        }
        else if (cmp_type(type, RIM_DEFDOUBLE)) 
        {
            oprintf ("_rim_sprm_par[_rim_aprm_%s].tval.dblval = %s;\n", mtext, eq);
            oprintf ("_rim_sprm_par[_rim_aprm_%s].set = true;\n", mtext);
        }
        else if (cmp_type(type, RIM_DEFBOOL)) 
        {
            oprintf ("_rim_sprm_par[_rim_aprm_%s].tval.logic = %s;\n", mtext, eq);
            oprintf ("_rim_sprm_par[_rim_aprm_%s].set = true;\n", mtext);
        }
        else
        {
            oprintf("rim_set_input (_rim_aprm_%s, %s, %ld); ", mtext, eq, type);
        }

        if (comma == NULL) break; else mtext = comma;
    }
}

//
// Replace / with /(double) in a double expression; this avoids integer divisions that round the result.
// v is the expression; returns the substitute.
// This is the only promotion needed to avoiding result rounding in number->double arithmetic. The rest are
// automatically promoted. It of course will promote double variables and numbers; there is no run-time penalty
// for that, and the compile time penaly is negligible.
// The promotion avoids converting strings and characters, such as "/" or '/' of course.
//
char *promote_double (char *v)
{
    // for doubles, we promote anything that's on the right of / to (double), thus avoiding integer division catastrophy where
    // 5/2 is 2 and not 2.5 in a double expression.
    rim_num tot = rim_count_substring (v, "/", 1, 0); // how many of /? Count even quoted, not a problem.
    if (tot != 0)
    {
        rim_num vlen = strlen (v);
        rim_num rlen; // what's left to subst in
        char *newv = rim_malloc (rlen = vlen + tot *(strlen("/(double)"))); // make string big enough
        memcpy (newv, v, vlen+1); // now we work on newv
        rim_num p = 0;
        while (1)
        {
            char *div = rim_find_keyword0 (newv+p, "/", 0, 0); // skip any / that's quoted
            if (div == NULL) break;
            rim_replace_string (div, rlen-p, "/", "/(double)", 0, NULL, 0); // replace just that one (3rd 0 from the right)
                                                                                  // we know this will succeed since we alloc'd enough
            p = div - newv + 1;
        }
        v = newv;
    }
    return v;
}


// 
// For a string v at byte j, check if this is a number or a double and then process this whole expression that starts with v
// Returns type found
//
rim_num check_num_or_double (char *v, rim_num j)
{
    while (isdigit (v[j])) j++;
    if (v[j] == '.' || v[j] == 'E' || v[j] == 'e')
    {
        return RIM_DEFDOUBLE;
    }
    else
    {
        return RIM_DEFNUMBER;
    }
}

//
// Returns true if name is a reserved word in C, cannot be a variable name
//
bool c_name (char *name)
{
    char *kwd[] = {
        "auto", "break", "case", "char", "const", "continue", "default", "do", "double", "else", "enum", "extern", "float", "for", "goto", "if", "int", "long", "register", "return", "short", "signed", "sizeof", "static", "struct", "switch", "typedef", "union", "unsigned", "void", "volatile", "while"
    };

    size_t i;
    for (i = 0; i < sizeof(kwd)/sizeof(kwd[0]); i++) 
    {
        if (strcmp(name, kwd[i]) == 0) {
            return true;
        }
    }
    return false;
}

//
// Return true if name is a string function, false otherwise
//
bool str_fun (char *name)
{
    if (!strcmp (name, "strlen"))
    {
        return true;
    }
    else return false;
}

//
// Return true if name is a math function. params is 1 or 2 (the number of input params). 
// This is for numbers, params can be NULL
// Note: pow() is a double function! Cannot have a function that's ambiguous as to what type it returns because
// then we could not determine the type of the expression reliably.
//
bool math_fun_n (char *name, rim_num *params)
{
    if (!strcmp (name, "abn") || !strcmp (name, "num") || !strcmp (name, "isinf") || !strcmp (name, "isnan"))
    {
        if (params != NULL) *params = 1;
        return true;
    }
    else return false;
}

//
// Return true if name is a math function. params is 1 or 2 (the number of input params), 
// This is for doubles; params can be NULL
//
bool math_fun (char *name, rim_num *params)
{
    if (!strcmp (name, "dbl") || !strcmp (name, "sin") || !strcmp (name, "cos") || !strcmp (name, "tan") || !strcmp (name, "asin") || !strcmp (name, "acos") || !strcmp (name, "atan") || !strcmp (name, "sinh") || !strcmp (name, "cosh") || !strcmp (name, "tanh") || !strcmp (name, "asinh") || !strcmp (name, "acosh") || !strcmp (name, "atanh") || !strcmp (name, "exp") || !strcmp (name, "exp2") || !strcmp (name, "log") || !strcmp (name, "log10") || !strcmp (name, "log2") || !strcmp (name, "sqrt") || !strcmp (name, "cbrt") || !strcmp (name, "abd") || !strcmp (name, "ceil") || !strcmp (name, "floor") || !strcmp (name, "round") || !strcmp (name, "trunc") || !strcmp (name, "erf") || !strcmp (name, "erfc") || !strcmp (name, "lgamma"))
    {
        if (params != NULL) *params = 1;
        return true;
    }
    else if (!strcmp (name, "atan2") || !strcmp (name, "pow") || !strcmp (name, "hypot") || !strcmp (name, "fmod") || !strcmp (name, "copysign") || !strcmp (name, "ldexp") || !strcmp (name, "nextafter") )
    {
        if (params != NULL) *params = 2;
        return true;
    }
    else return false;
}

//
// Mark lib as used. Each file is appended to, guaranteeing data is there, and the order doesn't matter
// Used by gg/vmakefile to link with minimum libs only. These files are removed prior to make.
// lib is the name of library
// This also marks include file by using out_file_name[.inc]
//
void mark_lib (char *lib)
{
    // lib
    char rim_inc_file[300];
    snprintf (rim_inc_file, sizeof(rim_inc_file), "%s.mod.%s", out_file_name, lib);
    FILE *f = fopen (rim_inc_file, "a");
    if (f == NULL) rim_report_error ("Cannot open internal mod file [%s]", rim_inc_file);
    fputs ("1", f);
    fclose (f);
    // include
    snprintf (rim_inc_file, sizeof(rim_inc_file), "%s.inc.%s", out_file_name, lib);
    f = fopen (rim_inc_file, "w+");
    if (f == NULL) rim_report_error ("Cannot open internal mod file [%s]", rim_inc_file);
    fprintf (f, " -DRIM_%s_INCLUDE ", lib);
    fclose (f);
}


//
// Make an empty (whitespaces) constant of size sz. This won't be in vm[] and can be used for cross-request purposes.
// Returns run-time variable name that is defined statically.
//
char *make_empty_const (rim_num sz)
{
    char *c = rim_malloc (1+sz+1+1);
    // first \"
    c[0] = '"';
    // then empty
    rim_num k;
    for (k = 1; k < sz+1; k++) c[k] = ' ';
    // then \"
    c[sz+1] = '"';
    // finish string
    c[sz+2] = 0; 
    char *full_c = c;
    make_mem (&c); // define and get name 
    rim_free (full_c);
    return c;
}


//
// Used in printing out message if v1 crashes, from chandle.c
// iserr is true if to stderr, false otherwise. s is the string, len is length to print.
// because for output to a crash, we just printout as is
// The prototype is in rim.h, as the same is used from rimrt.c (also in chandle.c)
//
rim_num rim_gen_write(bool iserr, char *s, rim_num len)
{
    rim_num res = write (iserr?fileno(stderr):fileno(stdout), s, len);
    return res;
}

//
// Record that line number linenum was written and at what offset (linepos) in the buffer
// This must be called *after* #line is written, before anything else is placed in the buffer
//
void written_line_number(rim_num linepos, rim_num linenum)
{
    rim_last_line_file_pos = linepos;
    rim_last_line = linenum;
}

//
// Decides if #line should be written (true), or if it's redundandant (false returned)
// linenum is the line intended to write, linepos is pos in the buffer where to be written
// This is called just *before* writing to a buffer
//
bool can_write_line_number(rim_num linepos, rim_num linenum)
{
   if (rim_no_output_line_num) return false;
   if (linepos == rim_last_line_file_pos && rim_last_line ==linenum) return false;
   return true;
}

//
// Recognize if *icur points to '...' character (either simple as 'x' or maybe '\n') and return number of bytes to skip if so; 
// otherwise return 0
//
rim_num is_single_quoted_char (char *cur)
{
    char *ocur = cur;
    if (*cur == '\'') 
    {
        cur++;
        if (*cur == 0) rim_report_error ("Syntax error in character constant");
        if (*cur == '\\') cur++; // account for '\'' for instance, skip the \'
        cur++;
        if (*cur != '\'') rim_report_error ("Syntax error in character constant");
        cur++;
        return cur-ocur;
    }
    return 0;
}


//
// Expand string that has # or $ or [ to have more room for those to expand into _000 variables (see make_var() with internal)
// inp is expanded, copied back into and spaces expanded after the above, and retuned
//
char *check_input(char *inp)
{
    // we count all of them, even if quoted, as it'd too long to count them for sure; this is an overestimate, which is fine
    rim_num totp = rim_count_substring (inp, RIM_KEYPOUND, 1, 0);
    rim_num totd = rim_count_substring (inp, RIM_KEYDOLLAR, 1, 0);
    rim_num totb = rim_count_substring (inp, RIM_KEYOPENANGLEBRACKET, 1, 0);
    if (totp + totd + totb > 0)
    {
        // estimate will need extra 5 chars per each, we'll give one extra for each
        rim_num ext = (totp+totd+totb)*6;
        rim_num len = strlen (inp);
        char *new_inp_b = rim_malloc (len + ext + 1); // new string
        char *new_inp = new_inp_b;
        while (1)
        {
            // find each keyword, account for "" and '', start from wherever we're currently
            // we search within parenthesis too, so #($x) would work
            char *d = rim_find_keyword0 (inp, RIM_KEYDOLLAR, 0, 0);
            char *p = rim_find_keyword0 (inp, RIM_KEYPOUND, 0, 0);
            char *b = rim_find_keyword0 (inp, RIM_KEYOPENANGLEBRACKET, 0, 0);
            // then conclude which comes first
            // dist_xxx is the position of the first char, or max integer if not found
            rim_num dist_d;
            rim_num dist_b;
            rim_num dist_p;
            if (d != NULL) dist_d = d-inp; else dist_d =LLONG_MAX;
            if (b != NULL) dist_b = b-inp; else dist_b =LLONG_MAX;
            if (p != NULL) dist_p = p-inp; else dist_p =LLONG_MAX;
            rim_num first = MIN (dist_d, MIN(dist_b, dist_p));
            if (first != LLONG_MAX)
            {
                // check if $$ for double, if so, ignore first $ and use just the second
                if (first == dist_d && *(d+1) == '$') first++; 
                // check if ## for double, if so, ignore first # and use just the second
                if (first == dist_p && *(p+1) == '#') first++; 
                //
                rim_num lcopy = first+1;
                memcpy (new_inp, inp,lcopy);
                inp+=lcopy;
                new_inp+=lcopy;
                char empty[] = "     ";
                memcpy (new_inp, empty, sizeof(empty)); // length of string is 5, copy zero afterward in case this is the last one
                new_inp += 5;
            } else 
            {
                strcpy (new_inp, inp);
                break; // nothing found at some point
            }
        }
        return new_inp_b;
    } else return inp; // there are no special chars
}

//
// Replace string at cur with var, and fill the rest up to curlen with space.
// This is used to replace variable with generated _000 (internal in make_var()) variables
//
void replace_var (char *cur, rim_num curlen, char *var)
{
    if (curlen < 4) rim_report_error ("Expression too short");
    // byte at which to start to put blanks, after we place _000 variable in it
    rim_num beg_empty = strlen (var);
    // replace whole expression with a new variable we created that is array element
    memcpy (cur, var, beg_empty);
    // the rest of expression is then just blanks
    while (beg_empty != curlen) cur[beg_empty++] = ' ';
    // not restoring [..] because the whole thing was replaced with _00..
}

//
// If *icur is a start to a name, find all alphanum or underscore characters as far
// as we can (for instance up to plus sign or end of string etc), extract the name as a new string
// and return it. icur is advanced to one character past the last of the name.
// The caller may free the return value
// We will clear any leading spaces (which may be placed there by us for #, $ and [ uses)
// len is the length of the whole name, including leading spaces, it can be NULL
// If NULL is returned, it means upcoming name is (...) subexpression, to be handled by the caller, in which case len is the length to the "(" and icur points to "("
//
char *upcoming_name (char **icur, rim_num *len)
{
    char *orig_cur = *icur;
    char *cur = *icur;
    while (isspace(*cur)) cur++;
    if (*cur == '(') 
    {
        if (len != NULL) *len = cur - orig_cur;
        *icur = cur;
        return NULL; 
    }
    char *vname = cur;
    while (isalnum(*cur) || *cur == '_') cur++;
    // get var name
    char *name = rim_strdupl (vname, 0, cur-vname);
    name[cur-vname] = 0; // end string
    *icur = cur;
    if (len != NULL) *len = cur - orig_cur;
    return name;
}

//
// Check if variable 'name' exists and is of type 'type', returns true if it does, false if it doesn't exist or not of right type
// If it doesn't exist real_type is RIM_DEFUNKN; it can be NULL.
// If it exists, real_type is its real type, and lnum is where it's defined; both can be NULL.
// lnum is the line number where it was defined, if it exists
//
bool var_exists (char *name, rim_num type, rim_num *real_type, rim_num *lnum)
{
    rim_num vtype;
    rim_num l;
    char *rname;
    bool found = find_var (name, &vtype, &l, &rname);
    if (found)
    {
        if (!cmp_type (type, vtype)) 
        {
            if (real_type) *real_type = vtype;
            if (lnum) *lnum = l;
            return false; 
        } else 
        {
            if (real_type) *real_type = vtype; // if we're looking for RIM_DEFSTRING but get RIM_DEFSTRINGSTATIC for instance!!!
                                               // so we know which one we got!
            return true;
        }
    }
    else 
    {
        if (real_type) *real_type = RIM_DEFUNKN;
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

    rim_num i;
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
// rep_error is true if this is report-error,
// gen_ctx is the compiler context.
//
//
void do_printf (char *mtext, bool rep_error, rim_gen_ctx *gen_ctx, rim_num backmode)
{


    // Example:
    // printf "%s %ld", str, int
    // this is no-enc printout

    //printf statements must have a comma, so instead of printf-url "xxx" it must be printf-url "%s",xxx
    //to avoid hard to track bugs
    //look also for format (starting with unquoted quote), to know where the printf part starts
    char *comma = find_keyword (mtext, RIM_KEYCOMMA, 0);
    char *toerr = find_keyword (mtext, RIM_KEYTOERROR, 1);
    char *to = find_keyword (mtext, RIM_KEYTO, 1);
    char *webe = find_keyword (mtext, RIM_KEYWEBENCODE, 1);
    char *urle = find_keyword (mtext, RIM_KEYURLENCODE, 1);


    carve_statement (&toerr, "print-format", RIM_KEYTOERROR, 0, 0);
    carve_statement (&to, "print-format", RIM_KEYTO, 0, 1);
    carve_statement (&comma, "print-format", RIM_KEYCOMMA, 0, 1);
    carve_statement (&webe, "print-format", RIM_KEYWEBENCODE, 0, 0);
    carve_statement (&urle, "print-format", RIM_KEYURLENCODE, 0, 0);
    //
    // Simulate various parts of statements, if they are not there but we need them
    if (backmode != 0)
    {
        if (backmode == RIM_P_URL) urle=RIM_KEYURLENCODE;
        else if (backmode == RIM_P_WEB) webe=RIM_KEYWEBENCODE;
        else rim_report_error ("Unknown backward compatibility mode for print-out (%ld)", backmode);
    }
    //
    // printf to stdout for standalone app, or to web/string otherwise 
    // Simulate various parts of statements, if they are not there but we need them
    rim_num c_printf_web = (webe != NULL ? 1:0);
    rim_num c_printf_url = (urle != NULL ? 1:0);
    rim_num c_printf = ((c_printf_web == 0 && c_printf_url == 0) ? 1:0);

    if (rep_error && (to != NULL || toerr != NULL)) rim_report_error ("Cannot use to-error or 'to' clause with report-error statement");


    // check it's a string constant
    rim_num flen = trimit (mtext);
    if ((mtext[0] != '"' || mtext[flen-1] != '"')) rim_report_error ("format must be a string literal, found [%s]", mtext);

    if (comma == NULL ) {comma = mtext; mtext=rim_strdup("\"%s\"");} // make up format for gcc, format must be dynamic as it will be carved
    char *format = mtext; // since mtext will become a generated variable below in carve_stmt_obj
    carve_stmt_obj (&mtext,true, true); // mtext is format


    if (toerr != NULL && to != NULL) rim_report_error( "cannot use both to-error and 'to' clause; use one or the other");
    define_statement (&to, RIM_DEFSTRING, false); // exact length is set by rim_write_to_string
    //
    check_var (&mtext, RIM_DEFSTRING);

    //Check only %s or %ld

    char *list;

    //
    // Look for each option and collect relevant info
    // First we MUST get each options position
    if (rep_error)
    {
        check_format(format, comma, &list);
        //
        //
        //
        // if this report-error happens anywhere outside a request, we will just start a new one
        // Otherwise, close this request, release resources and process the next one (for SERVICE).
        //
        // Explanation for how and why report-error works like this, as implemented below:
        // This is the same as _rim_report_error, except that nothing but what's output in format/args is actually output to stderr
        // Presumably, we don't want to print out anything else, since the user is in control. Before issuing report-error (which is what this does),
        // user should output whatever needs to be output to the user. The point is, this isn't like an error in programming that's kind of
        // panicky. User knows they're issuing an error (which goes to error stream), but before doing that the user should output a message to
        // the client!
        // No header is output.
        //
        // IMPORTANT: go to rimrt.c and check out _rim_report_error, see 1) and 2). If anything changes here, it must there too.
        //
        //
        oprintf("rim_printf(true, RIM_NOENC, %s", mtext);
        oprintf("%s", list);
        oprintf(");\n");
        oprintf ("rim_gen_write (true, \"\\n\", 1);\n"); // there's a new line implied at the end
        // Rollback any open transactions. Error can happen in there too, but if it dies, we exit the process (see above)
        // rather than risk complications
        oprintf ("rim_check_transaction (1);\n");
        oprintf ("if (rim_s_pc->ctx.req->ec == 0) rim_s_pc->ctx.req->ec=RIM_DISTRESS_STATUS;\n"); // go to process the next request, but if exit-status not set<>0
                                                                                // set it now to RimStone's distress code
        oprintf ("rim_error_request(1);\n"); // go to process the next request
        rim_free (list);
    }
    else if (to == NULL) 
    {
        check_format(format, comma, &list);
        oprintf ("rim_printf(%s, %s, %s", toerr!=NULL?"true":"false", rep_error?"RIM_NOENC":(c_printf!=0?"RIM_NOENC":(c_printf_web!=0?"RIM_WEB":"RIM_URL")),   mtext);
        oprintf("%s", list);
        oprintf(");\n");
        rim_free (list);
    }
    else
    {
        // this is ... to [ define ] <str>
        //
        oprintf("rim_write_to_string (&(%s));\n", to);
        gen_ctx->total_write_string++;
        //toerr is guaranteed to be NULL here, see above
        check_format(format, comma, &list);
        oprintf ("rim_printf(true, %s, %s", c_printf!=0?"RIM_NOENC":(c_printf_web!=0?"RIM_WEB":"RIM_URL"),mtext);
        oprintf("%s", list);
        oprintf(");\n");
        rim_free (list);
        oprintf("rim_write_to_string_notrim();\n"); 
        oprintf("rim_write_to_string (NULL);\n"); 
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
void do_print (char *mtext, rim_num backmode)
{
    char *length = find_keyword (mtext, RIM_KEYLENGTH, 1);
    char *nl = find_keyword (mtext, RIM_KEYNEWLINE, 1); // can be used with any other clause
    char *sfile = find_keyword (mtext, RIM_KEYSOURCEFILE, 1);
    char *sline = find_keyword (mtext, RIM_KEYSOURCELINE, 1);
    char *webe = find_keyword (mtext, RIM_KEYWEBENCODE, 1);
    char *urle = find_keyword (mtext, RIM_KEYURLENCODE, 1);

    carve_statement (&nl, "print-out", RIM_KEYNEWLINE, 0, 0);
    carve_statement (&sfile, "print-out", RIM_KEYSOURCEFILE, 0, 0);
    carve_statement (&sline, "print-out", RIM_KEYSOURCELINE, 0, 0);
    carve_statement (&webe, "print-out", RIM_KEYWEBENCODE, 0, 0);
    carve_statement (&urle, "print-out", RIM_KEYURLENCODE, 0, 0);
    carve_statement (&length, "print-out", RIM_KEYLENGTH, 0, 1);
    //
    // Simulate various parts of statements, if they are not there but we need them
    if (backmode != 0)
    {
        if (backmode == RIM_P_SOURCELINE) sline=RIM_KEYSOURCELINE;
        else if (backmode == RIM_P_SOURCEFILE) sfile=RIM_KEYSOURCEFILE;
        else if (backmode == RIM_P_NUM) ;
        else if (backmode == RIM_P_URL) urle=RIM_KEYURLENCODE;
        else if (backmode == RIM_P_WEB) webe=RIM_KEYWEBENCODE;
        else rim_report_error ("Unknown backward compatibility mode for print-out (%ld)", backmode);
    }

    if (webe != NULL && (sfile != NULL || sline != NULL || urle != NULL)) rim_report_error ("Cannot use web-encode clause with other clauses");
    if (urle != NULL && (sfile != NULL || sline != NULL || webe != NULL)) rim_report_error ("Cannot use url-encode clause with other clauses");
    if (length != NULL && (sfile != NULL || sline != NULL)) rim_report_error ("Cannot use length clause with other clauses, except url-encode and web-encode");

    if (sfile != NULL)
    {
        if (sline != NULL || webe != NULL || urle != NULL) rim_report_error ("Cannot use source-file clause with other clauses");
        carve_stmt_obj (&mtext, false, false);
        oprintf("rim_puts (RIM_NOENC, \"%s\", %ld, false);\n", rim_valid_rim_name, strlen(rim_valid_rim_name)); // optimized to compute strlen at compile time
        if (nl != NULL) oprintf("rim_puts (RIM_NOENC, \"\\n\", 1, false);\n"); // output 1 byte non-alloc'd
    }
    else if (sline != NULL)
    {
        if (sfile != NULL || webe != NULL || urle != NULL) rim_report_error ("Cannot use source-line clause with other clauses");
        carve_stmt_obj (&mtext, false, false);
        do_numstr (NULL, "__LINE__",  NULL, false, -1, -1, NULL);
        if (nl != NULL) oprintf("rim_puts (RIM_NOENC, \"\\n\", 1, false);\n"); // output 1 byte non-alloc'd
    }
    else
    {
        // this must be before or inside first checkvar so make_mem() doesn't substitute constant with gen var
        char *clen = NULL;
        if (is_constant_string (mtext, NULL) == RIM_CONST_OK)
        {
            rim_num clen_size = strlen(mtext)+30; // enough for sizeof()-1
            clen = rim_malloc (clen_size); 
            snprintf (clen, clen_size, "sizeof(%s)-1", mtext);
        }
        rim_num t = check_var (&mtext, RIM_DEFUNKN);
        if (cmp_type (t, RIM_DEFNUMBER))
        {
            if (webe != NULL || sfile != NULL || sline != NULL || urle != NULL || length != NULL) rim_report_error ("Cannot use other clauses (except new-line) when outputting a number");
            carve_stmt_obj (&mtext, true, true);
            do_numstr (NULL, mtext, NULL, false, -1, -1, NULL);
            if (nl != NULL) oprintf("rim_puts (RIM_NOENC, \"\\n\", 1, false);\n"); // output 1 byte non-alloc'd
        }
        else if (cmp_type (t, RIM_DEFDOUBLE))
        {
            if (webe != NULL || sfile != NULL || sline != NULL || urle != NULL || length != NULL) rim_report_error ("Cannot use other clauses (except new-line) when outputting a double");
            carve_stmt_obj (&mtext, true, true);
            do_numstr (NULL, mtext, NULL, true, -1, -1, "f");
            if (nl != NULL) oprintf("rim_puts (RIM_NOENC, \"\\n\", 1, false);\n"); // output 1 byte non-alloc'd
        }
        else if (cmp_type (t, RIM_DEFBOOL))
        {
            if (webe != NULL || sfile != NULL || sline != NULL || urle != NULL || length != NULL) rim_report_error ("Cannot use other clauses (except new-line) when outputting a boolean");
            carve_stmt_obj (&mtext, true, true); // boolean outputs 1 for true, 0 for false
            oprintf("rim_puts (RIM_NOENC, (%s)?\"1\":\"0\", 1, false);\n",mtext); 
            if (nl != NULL) oprintf("rim_puts (RIM_NOENC, \"\\n\", 1, false);\n"); 
        }
        // print-out assumes it's a string if internal mode!
        else if (cmp_type (t, RIM_DEFSTRING))
        {
            if (sfile != NULL || sline != NULL) rim_report_error ("Cannot use other clauses (except new-line, length, web-encode and url-encode) when outputting a string");
            if (webe != NULL && urle != NULL) rim_report_error ("Cannot use both url-encode and web-encode");
            carve_stmt_obj (&mtext, true, true);
            check_var (&mtext, RIM_DEFSTRING);
            check_var (&length, RIM_DEFNUMBER);
            bool constlen=false;
            if (length == NULL)
            {
                if (clen != NULL) 
                {
                    constlen = true;
                    length = clen;
                } else length = "0";
            } 

            if (urle != NULL) oprintf("rim_puts (RIM_URL, %s, %s, %s);\n", mtext, length, constlen?"false":"true"); 
            else if (webe != NULL) oprintf("rim_puts (RIM_WEB, %s, %s, %s);\n", mtext, length, constlen?"false":"true"); 
            else oprintf("rim_puts (RIM_NOENC, %s, %s, %s);\n", mtext, length, constlen?"false":"true"); 
            if (nl != NULL) oprintf("rim_puts (RIM_NOENC, \"\\n\", 1, false);\n"); // output 1 byte non-alloc'd
        }
        else 
        {
            rim_report_error( "Unsupported type in print-out");
        }
    }
}


//
//
//
// Replace __ with / in string, result should be freed by caller
//
//
char *undecorate (char *src)
{
    char *r_src = rim_strdup(src);
    rim_replace_string (r_src, strlen(r_src)+1, "__", "/", 1, NULL, 1);
    return r_src;
}

//
//
// Check if this sub is a constant and if it is, check if it exists
// sub is the name of request handler
// Returns true if sub is a string constant, false otherwise.
//
//
bool check_sub (char *sub)
{
    // trim request path 
    rim_num ml = strlen (sub);
    sub = rim_trim_ptr(sub,  &ml);
    // check if string - TODO: this won't take "xx" "yy", must be single constant!
    if (is_constant_string (sub, NULL) == RIM_CONST_OK)
    {
        // .reqlist file contains the list of all requests and we check to see if this request is there
        char rn[RIM_MAX_REQ_NAME_LEN];
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
        // get file name without .rim
        char *q = p; // p may be NULL after decorating below
        if (rim_decorate_path (rn, sizeof(rn), &q, len) != 1) rim_report_error( "request path [%s] is not valid", p);
        //
        // restore "
        p[zero] = '"'; // restore last "" because we don't want sub changed after this function
        //
        char rim_req_file[300];
        snprintf (rim_req_file, sizeof(rim_req_file), "%s/.reqlist", rim_bld_dir);
        FILE *f = fopen (rim_req_file, "r");
        if (f == NULL) rim_report_error( "Error [%s] opening file [%s]", strerror (errno), rim_req_file);
    
        char line[RIM_MAX_REQ_NAME_LEN]; // line to read from fname
        bool done_count = false; // true when first line (count of requests) read
        bool exist = false;
        while (1)
        {
            char *req = fgets_unlocked (line, sizeof (line), f);
            if (req == NULL) // either end or error
            {
                rim_num err = ferror_unlocked (f);
                if (err) rim_report_error( "Error [%s] reading file [%s]", strerror (errno), rim_req_file);
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
        if (!exist) rim_report_error ("Request [%s] does not exist", sub);
        return true;
    } else return false;
}

//
//
// Save the name of a parameter for an application-wide list of parameters, which then will be indexed in a fixed fashion,
// so that there's no "searching" for a parameter, rather we compile it as a direct memory access, making it very very fast.
// The index list will be generated in gg
// pname is the name of a parameter. A parameter with the same name can be used unrelated in different requests, but a process
// does only one request at a time, so each such parameter can have just one index in the list of params.
// type is the type of param used; once used as say number, it can't be anything else
//
//
void save_param (char *pname, rim_num type)
{
    rim_num plen = strlen (pname);
    if (pname[0] == '(') *pname = ' ';
    if (pname[plen-1] == ')') pname[plen-1] = ' ';
    rim_trim (pname, &plen, false);
    char rim_sparam_file[300];
    char *base = rim_basename (src_file_name);
    snprintf (rim_sparam_file, sizeof(rim_sparam_file), "%s/.setparam/%s.sp", rim_bld_dir, base);
    FILE *f = fopen (rim_sparam_file, "a");
    if (f == NULL) rim_report_error ("Cannot open file [%s]", rim_sparam_file);
    fprintf (f, "%s+%ld\n", pname, type);
    fclose(f);
    rim_free (base);
}

//
// Check cur_exp to see if it's an array/hash that's expected to yield type type_exp. If so, return true, otherwise false.
// If returns true, then proper variable is generated and substituted inside the expression (the _000 variable, see make_var() )
// type_exp on return is the actual type (important for strings as it can be RIM_DEFSTRINGSTATIC)
// This is called when the first character of what's checked here is alpha or _
// char, so that's why we skip the first char of **cur_exp. Caller makes sure of that.
// cur_exp is just passed "]" (for non-string/hash) and unchanged for string/hash
//
bool process_array (char **cur_exp, rim_num *type_exp)
{
    char *cur = *cur_exp;
    rim_num vtype;
    rim_num l;
    char *rname;
    // allow string[] notation for an integer expression
    // if it is, then remove [] and verify that index is integer, and allow this as a valid C expression (that evaluates to int)
    char *ind;
    char *open_p = cur+1; // +1 since we know *cur is alpha or _
    while (isalnum (*open_p) || *open_p == '_') open_p++; // search for anything other than name, if found '[', look for index
                                        // if not, move to the next possibility
                                        // here we use isalnum, since variable name cannot start with a digit but can contain it
    while (isspace (*open_p)) open_p++; // skip spaces, as in x [3]
    if (*open_p == '[')
    {
        // this is something = something[index]
        ind = open_p + 1;
        *open_p = 0;
        char *close_p = ind;
        // this now looks for str[str[str[a]]] and such and returns position of only the last ]
        rim_num p_open = 1; // current [
        while (*close_p != 0) 
        {
            // skip any 'X', which could be '[' or ']' that can mess up this calculation
            rim_num skip = is_single_quoted_char (close_p);
            if (skip > 0) {close_p += skip; continue;}
            if (*close_p == '[') p_open ++;
            if (*close_p == ']') 
            {
                p_open --;
                if (p_open == 0) break;
            }
            close_p++;
        }
        if (*close_p == 0) rim_report_error ("Syntax error in index, expected ']'");
        *close_p = 0;
        // check that variable is a string or an array or hash (in which case index is a string)
        while (isspace (*cur)) cur++; // skip spaces, as in x[     3], which happens when spaces is added for $,# or [
        bool str_found = find_var (cur, &vtype, &l, &rname);
        if (!str_found) 
        {
            rim_report_error ("Variable [%s] is not found", rname);
        }
        bool arr_str = false;
        bool arr_num = false;
        bool arr_dbl = false;
        bool arr_bool = false;
        bool hash = false;
        bool is_eligible = ((arr_num = cmp_type (vtype, RIM_DEFARRAYNUMBER)) || (arr_dbl = cmp_type (vtype, RIM_DEFARRAYDOUBLE)) || (arr_str = cmp_type (vtype, RIM_DEFARRAYSTRING)) || (arr_bool = cmp_type (vtype, RIM_DEFARRAYBOOL)) || (hash = cmp_type (vtype, RIM_DEFHASH)));
        //
        // check that index is a number or string (for hash)
        //
        bool sub_found; // subexpression within []
        char *hash_key = "RIM_EMPTY_STRING";
        if (!hash)
        { // if not hash, index is a number (not a double)
            check_exp (ind, &sub_found, false, false); // no check_input (), since it's not a top level call
        }
        else
        { // if hash, index is a string
            sub_found = true; // check_str will fail if there's no string expression
            rim_num type;
            while (isspace(*ind)) ind++;
            hash_key = check_str (ind, &type); // subtype isn't relevant here, it doesn't matter
                                                        // if string is normal or process for an index
                                                        // no check_input (), since it's not a top level cal
        }
        if (is_eligible)
        {
            if (arr_num && !cmp_type (*type_exp, RIM_DEFNUMBER)) 
            {
                rim_report_error ("Expected number array element");
            }
            else if (arr_dbl && !cmp_type (*type_exp, RIM_DEFDOUBLE)) 
            {
                rim_report_error ("Expected double array element");
            }
            else if (arr_str && !cmp_type (*type_exp, RIM_DEFSTRING)) 
            {
                rim_report_error ("Expected string array element");
            }
            else if (arr_bool && !cmp_type (*type_exp, RIM_DEFBOOL)) 
            {
                rim_report_error ("Expected boolean array element");
            }
            else if (hash && !cmp_type (*type_exp, RIM_DEFSTRING)) 
            {
                rim_report_error ("Expected hash element");
            }
            char arrv[300];
            // make expression to get a number/string/bool
            // NOTE: there is no add_ref for string here, because this is not assignment, just use in expression, if this is a 
            // lone set-string x = y[z], then set-string will do adding reference.
            if (hash) snprintf (arrv, sizeof(arrv),"rim_hash_get (%s, %s);\n", cur, hash_key);
            else snprintf (arrv, sizeof(arrv),"%s (%s, %s);\n", arr_str?"rim_read_arraystring":(arr_num?"rim_read_arraynumber":(arr_dbl?"rim_read_arraydouble":"rim_read_arraybool")), cur, hash?hash_key:ind);
            // get internal _000 variable
            char *arrv_p = arrv;
            // this is the *only* instance of internal being true for make_var() - because we're creating regular variable that must remain internal,
            // i.e. never again be processed by RimStone, or otherwise, since it's not in the format _rim_<type>... (see make_var()), then it wouldn't work
            // What's made here is next processed by gcc.
            make_var (&arrv_p, *type_exp = arr_str?(vtype==RIM_DEFARRAYSTRINGSTATIC?RIM_DEFSTRINGSTATIC:RIM_DEFSTRING):(arr_num?RIM_DEFNUMBER:(arr_bool?RIM_DEFBOOL:(arr_dbl?RIM_DEFDOUBLE:(vtype==RIM_DEFHASHSTATIC?RIM_DEFSTRINGSTATIC:RIM_DEFSTRING)))), true);
            // get total length of x[y] expression here (as a second param below in replace_var())
            replace_var (cur, close_p-cur+1, arrv_p);
        }
        else if (cmp_type (vtype, RIM_DEFSTRING)) // if it's not an array, the only primitive type is for a string to have subscription!!
        {
            if (!cmp_type (*type_exp, RIM_DEFNUMBER)) rim_report_error ("Expected string variable with [] number subscription");
            // check at run time accessing this byte is valid memory, this is in order
            // so the deepest expressions are checked first, as it should be
            oprintf ("if (rim_mem_get_len(%s)-1 < (%s) || (%s) < 0) rim_report_error (\"Cannot access byte [%%ld] in string\", (long)(%s));\n", cur, ind, ind, ind);
            // restore [] and clean up, success
            *open_p = '[';
            *close_p = ']';
        }
        else 
        {
            rim_report_error ("Variable [%s] is not an array, hash or a string", rname);
        }
        // for when strings are returned, we don't have recursive 
        // expressions for strings, only +; so in check_str() we don't
        // advance the expression text forward
        if (!arr_str && !hash) *cur_exp = close_p + 1; // move passed []
        return true;
    } else return false;
    return true; // never gets, just for gcc
}

//
// Recursively parse number expression for validity, and check if number variables in it are proper
// Emits messages if errors. 
// cur is the expression, found is used internally (to make sure expressions are not empty)
// level is 0 at the top call, and >0 and deeper calls
// isd is true if this is a double expression, false if number. In RimStone, you can't mix double with integer numbers, but you can do
// comma is true if expression can legally end with a comma (,). This is useful when parsing functions with parameters that are separated by a comma.
// do opposite, since a number is losslessly promoted to a double (which is 16 bytes on 64 bit linux).
// Returns current point in parsing, not useful to caller, only err is useful.
// Imagine $(#(something)), so getting a string from a number from a string. Since the bottom string cannot be known at compile time,
// so we can't know if #(something) is a number or double at compile time. So # is always expecting a string that's a number, and $ is always expecting a
// number. We'll use ## for a string that will return double and $$ for a double. To not differentiate can still be done, but it would involve run-time
// type determination and we don't do that as it leads to performance issues.
//
char *check_exp (char *cur, bool *found, bool isd, bool comma)
{
    *found = false;
    while (isspace(*cur)) cur++;
    if (*cur == '(') 
    {
        // First find enclosed expressions
        cur = check_exp (cur+1, found, isd, comma);
        if (!*found) rim_report_error ("Empty number expression");
        if (*cur != ')') rim_report_error ("Right parenthesis missing in number expression, found [%c]", *cur);
        cur++;
    }
    else
    {
        // Find begin token of an expression
        rim_num llen = strlen("strlen");
        if (!strncmp (cur, "strlen", llen) && (cur[llen] == ' ' || cur[llen] == '('))
        {
            *found = true;
            // it is strlen () function, or at least it should be
            cur = cur+llen;
            // process string inside and calculate its length
            rim_num lname;
            char *name = upcoming_name (&cur, &lname);
            if (name != NULL) rim_report_error ("Left parenthesis not found after strlen function call");
            char *inner_cur = cur;
            rim_num type;
            char *end_par = rim_find_keyword0 (cur+1, RIM_KEYPARRIGHT, 0, 1);
            if (end_par == NULL) rim_report_error ("Missing right parenthesis in strlen function call");
            *end_par = 0;
            rim_num inner_len = strlen (cur+1);
            char *res = check_str (cur+1, &type);
            //
            char s2nvar[300];
            char *s2n = s2nvar;
            snprintf (s2n, sizeof (s2nvar), "rim_mem_get_len(%s)", res);
            make_var (&s2n, isd?RIM_DEFDOUBLE:RIM_DEFNUMBER, true);
            //
            // subst expression with var _000
            // extra +1 is for trailing ")"
            replace_var (inner_cur-lname-llen, inner_len+lname+1+llen, s2n);
            *end_par = ' '; // make space where ")" was
            cur = end_par + 1; // continue right after ")"
        }
        else if (*cur == '#') 
        {
            // this is conversion from string to number
            *found = true;
            cur++;
            if (*cur == '#') // this is ## for double
            {
                if (!isd) rim_report_error ("Cannot use ## for a number expression, use #");
                cur++;
            } else
            {
                if (isd) rim_report_error ("Cannot use # for a double expression, use ##");
            }
            rim_num lname;
            char *name = upcoming_name (&cur, &lname);
            rim_num adj_double = (isd?1:0); // if used $$ or ##, when substituting with _000 variable, below in replace_var we need extra byte for that
            if (name == NULL)
            {
                char *inner_cur = cur;
                rim_num type;
                char *end_par = rim_find_keyword0 (cur+1, RIM_KEYPARRIGHT, 0, 1);
                if (end_par == NULL) rim_report_error ("Missing right parenthesis in expression");
                *end_par = 0;
                rim_num inner_len = strlen (cur+1);
                char *res = check_str (cur+1, &type);
                //
                char s2nvar[300];
                char *s2n = s2nvar;
                if (isd) snprintf (s2n, sizeof (s2nvar), "rim_str2dbl(%s, NULL)", res);
                else snprintf (s2n, sizeof (s2nvar), "rim_str2num (%s, 0, NULL)", res);
                make_var (&s2n, isd?RIM_DEFDOUBLE:RIM_DEFNUMBER, true);
                //
                // subst expression with var _000
                // extra +1 is for trailing ")"
                replace_var (inner_cur-lname-1-adj_double, inner_len+lname+1+1+adj_double, s2n);
                *end_par = ' '; // make space where ")" was
                cur = end_par + 1; // continue right after ")"
            }
            else
            {
                if (!var_exists (name, RIM_DEFSTRING, NULL, NULL)) rim_report_error ("Variable [%s] is not found or is not a number", name);
                // replace cur (as name) with _000 variable and at convert string to number
                char s2nvar[300];
                char *s2n = s2nvar;
                if (isd) snprintf (s2n, sizeof (s2nvar), "rim_str2dbl (%s, NULL)", name);
                else snprintf (s2n, sizeof (s2nvar), "rim_str2num (%s,0, NULL)", name);
                make_var (&s2n, isd?RIM_DEFDOUBLE:RIM_DEFNUMBER, true);
                // get total length of x[y] expression here (as a second param below in replace_var())
                // in this case, we're substituting the actual name, and we need to move one back to include #
                replace_var (cur-lname-1-adj_double, lname+1+adj_double, s2n);
            }
        }
        else if (*cur == '+') // unary +
        {
            cur = check_exp (cur+1, found, isd, comma);
            if (!*found) rim_report_error ("Empty number expression found");
        }
        else if (*cur == '-')  // unary -
        {
            cur = check_exp (cur+1, found, isd, comma);
            if (!*found) rim_report_error ("Empty number expression found");
        }
        else if (!isd && *cur == '~') // bitwise not 
        {
            cur = check_exp (cur+1, found, isd, comma);
            if (!*found) rim_report_error ("Empty number expression found");
        }
        else if (isalpha(*cur) || *cur == '_') 
        {
            *found = true;
            rim_num type_exp = isd?RIM_DEFDOUBLE:RIM_DEFNUMBER;
            if (!process_array (&cur, &type_exp))
            {
                char *name = upcoming_name (&cur, NULL);
                rim_num rtype;
                if (!var_exists (name, RIM_DEFNUMBER, &rtype, NULL)) 
                {
                    // first check if it's a function name, since those are reserved, and no variable name can be those
                    rim_num params; // number of params expected for this function, we support up to 2 for now

                    bool is_double_math;
                    if ((is_double_math=math_fun (name, &params)) && !isd) 
                    {
                        // this is where function is present and it's not double. But check if this function is okay with number exp
                        if (!math_fun_n (name, &params)) rim_report_error ("Function [%s] can only be used in a double expression. For instance use dbl() or num() function to promote variables, constants or expressions", name);
                    }
                    if (is_double_math || math_fun_n (name, &params)) // number functions can be used in either number or double exp
                    {
                        bool sub_isd = isd;
                        // if dbl() or num(), then we change what we parse inside!!
                        if (!strcmp (name, "dbl")) sub_isd = false; // so if converting to dbl, the subexpression must be number
                        else if (!strcmp (name, "num") || !strcmp (name, "isinf") || !strcmp (name, "isnan")) sub_isd = true; // and vice versa
                        //
                        // this is a function, process it, we do it for doubles and numbers, up to 2 params, so far haven't seen one with 3
                        // we can add our functions, and just implement it!
                        while (isspace(*cur)) cur++;
                        if (*cur == '(') 
                        {
                            // First find enclosed expressions
                            cur = check_exp (cur+1, found, sub_isd, params == 2 ? true:false);
                            if (!*found) rim_report_error ("Empty number expression");
                            if (params == 2) // find second param if warranted
                            {
                                if (*cur != ',') rim_report_error ("Missing comma for a second parameter in function [%s]", name);
                                cur = check_exp (cur+1, found, sub_isd, false);
                                if (!*found) rim_report_error ("Empty number expression");
                            }
                            if (*cur != ')') rim_report_error ("Right parenthesis missing in function [%s]", name);
                            cur++;
                        } else rim_report_error ("Opening parenthesis '(' missing after function [%s]", name);
                    }
                    else
                    {
                        if (cmp_type (rtype , RIM_DEFDOUBLE))
                        {
                            if (!isd) rim_report_error ("Variable [%s] is a double, but number expected. Use num(), dbl(), round(), floor(), ceil() etc.", name);
                        }
                        else rim_report_error ("Variable [%s] is not found or is not a %s", name, isd?"double":"number");
                    }
                }
                else
                {
                    //We're okay with doubles mixed with numbers to produce double value, but not the other way around
                    //(to produce number value with doubles mixed in)
                    //if (isd) rim_report_error ("Variable [%s] is a number, but double expected", name);
                }
            }
        }
        else if (isdigit (*cur) || (isd && *cur == '.')) // for double, it can start with a dot
        {
            *found = true;
            bool dot_first = (*cur == '.'); // is dot the first one in a double?
            cur++;
            if (!isd)
            {
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
            else
            {
                // HERE NEED TO CHECK FOR DOT, for e or E and for exponent which is <number> or -<number>
                if (!dot_first)
                {
                    while (isdigit(*cur)) cur++; // first the leading part, we know we have one digit already since dot_first is false here
                    if (*cur == '.') // can have some afterward, but it can just be 2.
                    {
                        cur++;
                        while (isdigit(*cur)) cur++; // could be some digit or none (so 2.0 and 2. are both valid)
                    }
                }
                else
                {
                    bool found_dig = false;
                    while (isdigit(*cur)) { found_dig = true; cur++;} // after the first dot, we must have at least one digit
                    if (!found_dig) rim_report_error ("No digits after a dot for a double");
                }
                if (*cur == 'e' || *cur == 'E')
                {
                    // this is exponent
                    cur++;
                    char *exp_beg;
                    if (*cur == '-' || *cur == '+') exp_beg = ++cur; else exp_beg = cur;  // skip plus or minus
                    while (isdigit(*cur)) cur++; // must be some digit, we enforce that below
                    if (cur == exp_beg) rim_report_error ("Missing exponent in a double");
                }
            }
        } 
        else if (!isd && *cur == '\'')
        {
            cur++;
            if (*cur == 0) rim_report_error ("Incomplete character after single quote"); 
            if (*cur == '\\')  // this is say '\n'
            {
                cur++;
                if (!(*cur == 'a' || *cur == 'b' || *cur == 'f' || *cur == 'n' || *cur == 'r' || *cur == 't' || *cur == 'v' || *cur == '\\' || *cur == '\'')) rim_report_error ("Invalid escape sequence (\\%c)", *cur); 
                cur++;
            } else cur++;
            if (*cur != '\'') rim_report_error ("Expected single quote");
            cur++;
            *found = true;
        }
        else rim_report_error ("Unrecognized expression starting with [%c]", *cur);
    }
    while (isspace(*cur)) cur++;

    // Once token found, find if it's connected to another token
    if (*cur == '+' || *cur == '-' || *cur == '*' || *cur == '/' || (!isd && *cur == '%') || (!isd && *cur == '&') || (!isd && *cur == '|') || (!isd && *cur == '^') ) 
    {
        if (!*found) rim_report_error ("No %s expression found", isd?"double":"number"); // something must have been found prior to operators
        cur = check_exp(cur+1, found, isd, comma);
        if (!*found) rim_report_error ("No %s expression found", isd?"double":"number");
    }

    // allow comma if asked for in function processing for a 2nd argument in fun(a,b)
    if (*found && *cur != ')' && *cur != 0 && !(*cur == ',' && comma)) rim_report_error ("Unexpected character in %s expression [%c]", isd?"double":"number", *cur); 
    // if there's something and it's not followed by an operator; what follows must be either ) or nothing or , (if comma is true)

    return cur;
    
}


//
// Parse boolean expression using !, && and || (as well as parenthesis)
// Returns current point in parsing, not useful to caller
//
char *check_bool (char *cur, bool *found)
{
    *found = false;
    while (isspace(*cur)) cur++;

    if (*cur == '(') 
    {
        // First find enclosed expressions
        // no check_input (), since it's not a top level call
        cur = check_bool (cur+1, found); 
        if (!*found) rim_report_error ("Empty boolean expression found");
        if (*cur != ')') rim_report_error ("Expected right parenthesis in boolean expression, found [%c]", *cur);
        cur++;
        while (isspace(*cur)) cur++;
    }
    else
    {
        // Find begin token of an expression
        if (cur[0] == '!') 
        {
            // no check_input (), since it's not a top level call
            cur = check_bool (cur+1, found); 
            if (!*found) rim_report_error ("Empty boolean expression found");
        }
        else if (isalpha(*cur) || *cur == '_') 
        {
            *found = true;
            rim_num type_exp = RIM_DEFBOOL;
            if (!process_array (&cur, &type_exp))
            {
                char *name = upcoming_name (&cur, NULL);
                if (!var_exists (name, RIM_DEFBOOL, NULL, NULL)) rim_report_error ("Variable [%s] is not found or is not a boolean", name);
            }
        }
    }
    while (isspace(*cur)) cur++;

    // Once token found, find if it's connected to another token (first char has been advanced to non-space)
    bool is_and = (cur[0] == '&' && cur[1] == '&');
    bool is_or = (cur[0] == '|' && cur[1] == '|');
    if (is_and || is_or) 
    {
        if (!*found) rim_report_error ("Empty boolean expression found"); // something must have been found prior to operators
        // no check_input (), since it's not a top level call
        cur = check_bool(cur+2, found); 
        if (!*found) rim_report_error ("Empty expression found");
    }

    if (*found && *cur != ')' && *cur != 0) rim_report_error("Unexpected character in boolean expression [%c]", *cur); // if there's something and it's not followed by an operator 
                                                         // what follows must be either ) or nothing

    return cur;
    
}


//
// Recognize single string cur, can be a literal, $x (number to string), an array/hash element or just a variable
// Returns value that can be used in upper expression
// type is returned as the actual string type (static or not)
// s_str, s_beg and s_len are string, beginning and length of a substring (or NULL if not provided), s_str and s_beg must be provided if s_str is.
//
char *rec_str (char *cur, rim_num *type, char **s_str, char **s_beg, char **s_len)
{
    *s_str = NULL;
    *s_beg = NULL;
    *s_len = NULL;
    if (cur[0] == '"') // check if starts with double quote, and interpret as string expression
    {
        if (is_constant_string(cur, NULL) == RIM_CONST_OK) 
        {
            make_mem (&cur);
            return cur;
        }
        else rim_report_error ("Syntax error in a string literal [%s]", cur);
    }
    else if (cur[0] == '@') // substring
    {
        cur++;
        while (isspace(*cur)) cur++;
        // first must come "("
        if (*cur != RIM_KEYPARLEFT[0]) rim_report_error ("Left parenthesis missing in substring");
        char *sub_str = cur+1;
        // it must be closed with ")" (other parenthesis can be in betwen if balanced)
        char *rpar = rim_find_keyword0 (sub_str, RIM_KEYPARRIGHT, 0, 1);
        if (rpar == NULL) rim_report_error ("Right parenthesis missing in substring");
        *rpar = 0;
        // find comma where beginning of string is
        char *fcomma = rim_find_keyword0 (sub_str, RIM_KEYCOMMA, 0, 1);
        if (fcomma == NULL) rim_report_error ("No comma found in substring");
        *fcomma = 0;
        char *sub_beg = fcomma+1;
        // find comma where length of string is
        fcomma = rim_find_keyword0 (sub_beg+1, RIM_KEYCOMMA, 0, 1);
        char *sub_len;
        if (fcomma == NULL) sub_len = "-1"; else
        {
            *fcomma = 0;
            sub_len = fcomma + 1;
        }
        // Now check out the string expression
        rim_num type;
        sub_str = check_str (sub_str, &type);
        // check out number expression for beg
        bool found;
        check_exp (sub_beg, &found, false, false); 
        // check out number expression for length
        check_exp (sub_len, &found, false, false); 
        make_var (&sub_beg, RIM_DEFNUMBER, true);
        make_var (&sub_len, RIM_DEFNUMBER, true);
        *s_str = sub_str;
        *s_beg = sub_beg;
        *s_len = sub_len;
        return *s_str;
    }
    else if (cur[0] == '$') // convert number to string
    {
        cur++;
        bool isd = false;
        if (cur[0] == '$') { isd = true; cur++; } // $$ is for string to double
        rim_num lname;
        char *fcur = cur; // don't update curr, we don't do that for string
        char *name = upcoming_name (&fcur, &lname);
        if (name == NULL)
        {
            bool found;
            char *inner_cur = fcur;
            char *end_par = rim_find_keyword0 (fcur+1, RIM_KEYPARRIGHT, 0, 1);
            if (end_par == NULL) rim_report_error ("Missing right parenthesis in expression");
            *end_par = 0;
            rim_num inner_len = strlen (fcur+1);
            check_exp (fcur+1, &found, isd?true:false, false); 
            //
            char s2nvar[300];
            char *s2n = s2nvar;
            if (!isd) snprintf (s2n, sizeof (s2nvar), "rim_num2str (%s, NULL, 10)", fcur+1);
            else snprintf (s2n, sizeof (s2nvar), "rim_dbl2str (%s, 'f', -1, -1, NULL)", promote_double(fcur+1)); // for double, promote any integer divisions
                                                                                                                // to double, avoiding unexpected rounding
            make_var (&s2n, RIM_DEFSTRING, true);
            //
            // subst expression with var _000
            // extra +1 is for trailing ")"
            replace_var (inner_cur-lname-1, inner_len+lname+1+1, s2n);
            *end_par = ' '; // make space where ")" was
            return cur-1;
        }
        else
        {
            rim_num rtype;
            bool isd = false;
            // check the variable to see if it's a number or double and convert accordingly
            if (!var_exists (name, RIM_DEFNUMBER, &rtype, NULL)) 
            {
                if (rtype != RIM_DEFDOUBLE) rim_report_error ("Variable [%s] is not found or is not a number or double", name);
                else isd = true;
            }
            // replace fcur (as name) with _000 variable and at convert string to number
            char n2svar[300];
            char *n2s = n2svar;
            if (!isd) snprintf (n2s, sizeof (n2svar), "rim_num2str (%s, NULL, 10)", name);
            else snprintf (n2s, sizeof (n2svar), "rim_dbl2str (%s, 'f', -1, -1, NULL)", name);
            make_var (&n2s, RIM_DEFSTRING, true);
            // get total length of x[y] expression here (as a second param below in replace_var())
            // in this case, we're substituting the actual name, and we need to move one back to include #
            replace_var (fcur-lname-1, lname+1, n2s);
            return cur-1; // this is where $... starts and where it'll be replaced with _000
        }
    }
    else 
    {
        if (process_array (&cur, type)) 
        {
            return cur;
        }
        else 
        {
            if (!var_exists (cur, RIM_DEFSTRING, type, NULL)) rim_report_error ("Variable [%s] is not found or is not a string", cur);
            return cur;
        }
    }
}


//
// Parse string expression for validity, and check if string variables in it are proper. This does solely concatenation of strings with plus sign.
// Emits messages if errors. 
// cur is the expression
// type is either RIM_DEFSTRING or RIM_DEFSTRINGSTATIC, since for strings, it matters which one it is (as they are
// copied by reference), while it doesn't matter for numbers/booleans since they copy by value.
// Returns actual code that computes the string expression,(so code that does computation or cur itself if it's just a variable), or RIM_EMPTY_STRING if error.
// String expressions are a+b+c... i.e. concatenation only
//
char *check_str (char *cur, rim_num *type)
{
#define _RIM_STRING_VAR_LEN 300
    char *sres =rim_malloc (_RIM_STRING_VAR_LEN);
    char *c_elem;
    bool first = true;
    char *s_str;
    char *s_beg;
    char *s_len;
    *type = RIM_DEFSTRING;
    rim_fifo *vars;
    rim_store_init(&vars);
    while (1)
    {
        char *lp = rim_find_keyword0 (cur, RIM_KEYPLUS, 0, 1); // skip plus within parenthesis as it would be $(x+y..)!!!
        if (first) 
        { 
            if (lp == NULL) 
            {
                while (isspace(*cur)) cur++;
                char *res = rec_str (cur, type, &s_str, &s_beg, &s_len);

                if (s_str != NULL)
                {
                    // create a variable that will hold the result of just one @()
                    snprintf (sres, _RIM_STRING_VAR_LEN, "_rim_sres_%ld", rim_sres_id++);
                    oprintf ("char *%s = RIM_EMPTY_STRING;\n", sres);
                    oprintf ("if (%s == -1) %s = rim_strdupl (%s, %s, rim_mem_get_len(%s)); else rim_copy_string (%s, %s, &(%s), %s);\n", s_len, sres, s_str, s_beg, s_str, s_str, s_beg, sres, s_len); // exact length copied, so works for binary exactly

                    return sres;
                } else return res;
            }
            // create a variable when there are plus signs
            snprintf (sres, _RIM_STRING_VAR_LEN, "_rim_sres_%ld", rim_sres_id++);
        }

        // produce curr trimmed, ready for variable check
        if (lp != NULL) *lp = 0;
        rim_num lcur = strlen (cur);
        cur = rim_trim_ptr(cur,  &lcur); // must use this form of trim, because trimit() would alter cur, and thus stored
                                        // pointers below in rim_store, causing memory failure

        if (lp == NULL) 
        {
            if (cur[0] == 0) rim_report_error ("String expression missing");
        }

        c_elem = rec_str (cur, type, &s_str, &s_beg, &s_len);
        char substr[200];
        if (s_str != NULL)
        {
            snprintf (substr, sizeof(substr), "%s+%s+%s", s_str, s_beg, s_len);
            c_elem = substr;
        }
        rim_store(vars, rim_strdup(sres), rim_strdup(c_elem));
        //
        first = false;
        if (lp == NULL) break; else *lp = '+'; // restore plus sign sign we stored the variable into fifo with rim_store
        cur = lp + 1; // skip the plus
    }
    oprintf ("char *%s = RIM_EMPTY_STRING;\n", sres);
    //
    // we get here is the expression parsed is in indeed in the form of a+b+... were each is a string
    //
    rim_rewind(vars);
    while (1) 
    {
        char *res = RIM_EMPTY_STRING;
        c_elem = RIM_EMPTY_STRING;
        rim_num st = rim_retrieve (vars, &res, (void**)&c_elem);
        if (st != RIM_OKAY) break;
        // no need for make_mem here, since it's made up when we went through + expressions
        char *p;
        char *beg;
        char *len;
        if ((p = strchr (c_elem, '+')) != NULL)
        {
            *p = 0;
            beg = p+1;
            p = strchr (beg, '+');
            if (p == NULL) rim_report_error ("Internal error in substring");
            *p = 0;
            len = p+1;
            oprintf ("%s = rim_add_string_part (%s, %s, %s, %s);\n", res, res, c_elem, beg, len);
        }
        else
        {
            oprintf ("%s = rim_add_string (%s, %s);\n", res, res, c_elem);
        }
    }
    // make memory occupied to be as much as needed. 
    oprintf ("{rim_num tmp_id = rim_mem_get_id(%s); %s=rim_realloc(tmp_id, rim_mem_get_len(%s)+1);}\n", sres, sres,sres);
    return sres;
}



//
// Check C call (a function/macro) is with valid parameters and of proper syntax
//
void check_c (char *m)
{
    char *mtext = rim_strdup (m);
    char *lp = rim_find_keyword0 (mtext, RIM_KEYPARLEFT, 0, 0);
    char *rp = rim_find_keyword0 (mtext, RIM_KEYPARRIGHT, 0, 0);
    if (!lp) rim_report_error ("Left parenthesis missing in C function/macro call");
    if (!rp) rim_report_error ("Right parenthesis missing in C function/macro call");
    carve_statement (&lp, "call-extended", RIM_KEYPARLEFT, 1, 1);
    carve_statement (&rp, "call-extended", RIM_KEYPARRIGHT, 1, 0);
    trimit(mtext);
    if (rim_is_valid_param_name (mtext, false, false) != 1) rim_report_error ("Function/macro name in call-extended is not valid [%s]", mtext);
    char *nc;
    while (1)
    {
        nc = find_keyword (lp, RIM_KEYCOMMA, 0);
        carve_statement (&nc, "format parameters", RIM_KEYCOMMA, 0, 1);
        trimit (lp);
        if (lp[0] == '&') lp++;
        trimit (lp);
        if (lp[0] != 0) // make sure it works for fun() meaning no arguments
        {
            // if input variable, check it
            check_var (&lp, RIM_DEFUNKN);
            oprintf("RIM_UNUSED (%s);\n", lp);
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
// type, line, rname can be NULL if we don't need some of them
//
bool find_var (char *name, rim_num *type, rim_num *line, char **rname)
{
    rim_num lev = rim_level;
    rim_num lname = strlen (name);
    name = rim_trim_ptr(name,  &lname);
    while (lev >= 0)
    {
        char decname[RIM_MAX_DECNAME+1];
        snprintf (decname, sizeof(decname), "%ld_%ld+%s", lev, rim_resurr[lev], name);
        rim_num st;
        rim_var *var = rim_find_hash (rim_varh, decname, 0, &st, false);
        if (st == RIM_OKAY) 
        { 
            if (type) *type = var->type;
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
bool add_var (char *var, rim_num type, char *realname, bool watched_for_unused)
{
    // Check if it exists
    rim_num st;
    rim_num ot;
    rim_num ol;
    char *rname;
    if (find_var (var, &ot, &ol, &rname)) 
    {
        // if variable of type string is wanted as the output of a statement and found to exist (which is what add_var is called for)
        // but there's a static string already there, we can't use it; static string is a special string that can be assigned
        // **only** in set-string and *nowhere* else
        // The same is true in reverse.
        if (!cmp_type (type,ot)) rim_report_error ("Variable [%s] of type [%s] is already defined on line [%ld], cannot create a variable with the same name but of type [%s]", rname, typename(ot), ol, typename(type));
        if (((type == RIM_DEFSTRING && ot == RIM_DEFSTRINGSTATIC) || (type == RIM_DEFSTRINGSTATIC && ot == RIM_DEFSTRING))) rim_report_error ("Process-scope string variable [%s] can only be assigned with set-string, and can only be created with process-scope clause if it doesn't already exist", rname);
        return true;  
    }
    // check if variable name reserved
    if (math_fun (var, NULL) || math_fun_n (var, NULL) || str_fun (var) || c_name (var)) rim_report_error ("Cannot use name [%s] for a variable because it is reserved", var);
    // Add if not found, this is hash for exact variable scope
    char *decname=rim_malloc (RIM_MAX_DECNAME+1);
    snprintf (decname, RIM_MAX_DECNAME, "%ld_%ld+%s", rim_level, rim_resurr[rim_level], var);
    rim_var *v = rim_malloc (sizeof(rim_var));
    v->name = decname;
    v->rname = (realname == NULL ? "":rim_strdup (realname));
    v->type = type;
    v->lnum = lnum;
    v->used = watched_for_unused?false:true; // if watched, we start with false, otherwise declared it "used", meaning
                                                   // we won't generate error messages;
    rim_add_hash (rim_varh, v->name, (void*)v, NULL, &st);
    return false;
}



//
// Return type id based on type name
//
rim_num typeid (char *type)
{
    if (!strcmp (type, RIM_KEY_T_STRING)) return RIM_DEFSTRING; 
    else if (!strcmp (type, RIM_KEY_T_NUMBER)) return RIM_DEFNUMBER;
    else if (!strcmp (type, RIM_KEY_T_DOUBLE)) return RIM_DEFDOUBLE;
    else if (!strcmp (type, RIM_KEY_T_MESSAGE)) return RIM_DEFMSG;
    else if (!strcmp (type, RIM_KEY_T_SPLITSTRING)) return RIM_DEFBROKEN;
    else if (!strcmp (type, RIM_KEY_T_HASH)) return RIM_DEFHASH;
    else if (!strcmp (type, RIM_KEY_T_ARRAYSTRING)) return RIM_DEFARRAYSTRING;
    else if (!strcmp (type, RIM_KEY_T_ARRAYNUMBER)) return RIM_DEFARRAYNUMBER;
    else if (!strcmp (type, RIM_KEY_T_ARRAYDOUBLE)) return RIM_DEFARRAYDOUBLE;
    else if (!strcmp (type, RIM_KEY_T_ARRAYBOOL)) return RIM_DEFARRAYBOOL;
    else if (!strcmp (type, RIM_KEY_T_JSON)) return RIM_DEFJSON;
    else if (!strcmp (type, RIM_KEY_T_XML)) return RIM_DEFXML;
    else if (!strcmp (type, RIM_KEY_T_TREE)) return RIM_DEFTREE;
    else if (!strcmp (type, RIM_KEY_T_TREECURSOR)) return RIM_DEFTREECURSOR;
    else if (!strcmp (type, RIM_KEY_T_FIFO))  return RIM_DEFFIFO;
    else if (!strcmp (type, RIM_KEY_T_LIFO))  return RIM_DEFLIFO;
    else if (!strcmp (type, RIM_KEY_T_LIST))  return RIM_DEFLIST;
    else if (!strcmp (type, RIM_KEY_T_ENCRYPT)) return RIM_DEFENCRYPT;
    else if (!strcmp (type, RIM_KEY_T_FILE)) return RIM_DEFFILE;
    else if (!strcmp (type, RIM_KEY_T_DIR)) return RIM_DEFDIR;
    else if (!strcmp (type, RIM_KEY_T_SERVICE)) return RIM_DEFSERVICE;
    else if (!strcmp (type, RIM_KEY_T_BOOL)) return RIM_DEFBOOL;
    else rim_report_error ("Unknown type [%s]", type);
}

//
// Check for {} in prline and adjust rim_level
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
    comm =  rim_find_keyword0 (prline, RIM_KEYCOMMENT, 0, 0);
    if (comm != NULL ) *comm = 0; // cap the line, restore later
    while (1)
    {
        // 0 means don't need a space after keyword, 1 means ignore () and account only for ""
        cbl = rim_find_keyword0 (prline, RIM_KEYCURLYLEFT, 0, 0);
        cbr = rim_find_keyword0 (prline, RIM_KEYCURLYRIGHT, 0, 0);
        // cbls is { and cbr is }. We adjust scope level
        // based on the order these appear, which the below does
        if (cbl != NULL)
        {
            if (cbr != NULL)
            {
                if (cbl-prline < cbr - prline)
                {
                    rim_level++;
                    cb = cbl;
                }
                else 
                {
                    rim_resurr[rim_level]++;
                    rim_level--;
                    cb = cbr;
                }
            }
            else
            {
                rim_level++;
                cb = cbl;
            }
        }
        else
        {
            if (cbr != NULL)
            {
                rim_resurr[rim_level]++;
                rim_level--;
                cb = cbr;
            } else break;
        }
        if (rim_level >= RIM_MAX_CODE_LEVEL) rim_report_warning ("Code levels too deep, restructure code for less code blocks in depth");
        if (rim_level < 0) { rim_level = 0; rim_report_warning ("Code block opening/closing imbalance"); }
        if (cb == NULL) break;
        prline = cb + 1;
        cb = NULL;
        if (prline[0] == 0) break;
    }
    if (comm != NULL) *comm = '/';
}

//
// Returns >0 if s is in the form of [0-9]*<f>, as in %40s where <f> is "s", or just <f>; flen is the length of f (so for "e" it's 1)
// Returned number is such that s+<return> is the first byte after <f>, it returns 'true' in this case
// If not in this form, returns 0 (i.e. false)
// isd is true if looking for format for double, which can have . (dot)
//
rim_num is_len_format (char *s, char *f, rim_num flen, bool isd)
{
    char *os = s;
    while (*s && isdigit(*s)) s++;
    if (isd && s[0] == '.')
    {
        s++;
        // this is <some>.<some> or <some>. or .<some> or just . which are all valid for double type, but just for double
        while (*s && isdigit(*s)) s++;
    }
    if (!memcmp(s,f,flen)) {s+=flen; return (rim_num)(s-os);} else return 0;
}


//
// Returns RIM_CONST_OK if 's' is "something" valid string, with backslash (\) escaping whatever accounted for
// It also accounts if there's a + sign in there, which would be an expression of concatenating strings
// Otherwise errors out if it starts with \" but isn't a valid string, or RIM_CONST_NOT if it doesn't start with \" or  it does but is + concatenation.
// A string can also be a concatenation of "xxx" strings, such as "xxx""yyy"...
//
rim_num is_constant_string (char *s, rim_num *size)
{
    rim_num clen = 0;
    while (*s && isspace(*s)) s++;
    if (*s != '"') return RIM_CONST_NOT; // not string
    if (rim_find_keyword0 (s, RIM_KEYPLUS, 0, 0) != NULL) return  RIM_CONST_NOT; // this will be checked out later
                                                                                // to see if it's a correct expression
                                                                                // in check_str() from check_var()
    s++;
    bool found = false;
    while (*s != 0)  
    {
        if (*s == '\\') 
        { 
            if (*(s+1) != 0) 
            { 
                char *cur = s+1;
                // check if escape is one of valid simple C escape sequences
                if (*cur == '"')
                {
                    // this is escaped double quote
                    clen++; s+=2; continue;
                }
                else if (!(*cur == 'a' || *cur == 'b' || *cur == 'f' || *cur == 'n' || *cur == 'r' || *cur == 't' || *cur == 'v' || *cur == '\\' || *cur == '\'')) 
                {
                    // now check if octal number
                    if (RIM_IS_OCTAL(*cur))
                    { // this is octal followed by one, two or three 0-7 digits, max being \377
                        if (!RIM_IS_OCTAL (*(cur+1))) { clen++; s+=2; continue; } // one digit
                        else if (!RIM_IS_OCTAL (*(cur+2))) { clen++; s+=3; continue; } // two digits
                        else 
                        { //three digits
                            if ((*cur-'0')*8*8+(*(cur+1)-'0')*8+(*(cur+2)-'0') > 255) rim_report_error ("Octal escape sequence out of range [%s]", cur);
                            clen++; 
                            s+=4; 
                            continue; 
                        } 
                    }
                    else if (*cur == 'x')
                    { // this is hex followed by two 0-F digits
                        if (isxdigit(*(cur+1)))
                        {
                            if (!isxdigit(*(cur+2))) { clen++; s+=3; continue; }
                            else { clen++; s+=4; continue; }
                        }
                        else rim_report_error ("Invalid hexadecimal escape sequence [%s]", cur);
                    }
                    else if (*cur == 'U')
                    {
                        if (*(cur+1) != 0 && isxdigit(*(cur+1)) && *(cur+2)!=0 && isxdigit(*(cur+2)) && *(cur+3)!=0 && isxdigit(*(cur+3)) && *(cur+4)!=0 && isxdigit(*(cur+4)) && *(cur+5)!=0 && isxdigit(*(cur+5)) && *(cur+6)!=0 && isxdigit(*(cur+6)) && *(cur+7)!=0 && isxdigit(*(cur+7)) && *(cur+8)!=0 && isxdigit(*(cur+8))) 
                        {
                            rim_num rtot; // unicode
                            rim_num totjv; // # of bytes used up in source, will be 10 here
                            rim_num i = 0;
                            char **o_errm;
                            o_errm = &RIM_EMPTY_STRING;
                            if ((rtot = rim_utf_get_code8 (cur, &i, &totjv, o_errm)) == -1) rim_report_error ("Cannot interpret Unicode character, error [%s]", *o_errm);
                            // turn unicode to binary
                            unsigned char utf_res[30];
                            rim_num32 bytes = rim_decode_utf (rtot, utf_res, o_errm);
                            if ((*o_errm)[0] != 0) rim_report_error ("Cannot interpret Unicode character, error [%s]", *o_errm);
                            clen+=bytes; // this is how many bytes are actually taken for this Unicode character
                            s+=totjv; 
                            continue;
                        }
                        else rim_report_error ("Invalid hexadecimal escape sequence (\\%c)", *cur);
                    }
                    else if (*cur == 'u')
                    {
                        if (*(cur+1) != 0 && isxdigit(*(cur+1)) && *(cur+2)!=0 && isxdigit(*(cur+2)) && *(cur+3)!=0 && isxdigit(*(cur+3)) && *(cur+4)!=0 && isxdigit(*(cur+4))) 
                        {
                            rim_num rtot; // unicode
                            rim_num totjv; // # of bytes used up in source (i.e. not binary, so it's 6 for \uABCD or 12 for  \uABCD\uABCD when it's surrogate)
                            rim_num i = 0;
                            char **o_errm;
                            o_errm = &RIM_EMPTY_STRING;
                            if ((rtot = rim_utf_get_code (cur, &i, &totjv, o_errm)) == -1) rim_report_error ("Cannot interpret Unicode character, error [%s]", *o_errm);
                            // turn unicode to binary
                            unsigned char utf_res[30];
                            rim_num32 bytes = rim_decode_utf (rtot, utf_res, o_errm);
                            if ((*o_errm)[0] != 0) rim_report_error ("Cannot interpret Unicode character, error [%s]", *o_errm);
                            // if Unicode is surrogate, write it as \xXX... bytes. It will either take all 12 bytes
                            // from \uXXXX\uXXXX (12 bytes), or there will be less and will insert "" or " " to make up for it
                            if (totjv == 12)
                            {
                                // bytes is the number of actual bytes in binary UTF, could be 3 or 4
                                // 10 will be zero byte
                                // cur-1 because that's where the first \u starts
                                snprintf(cur-1, 11, "\\U%0*X", 8, (int)rtot);
                                cur[9]='"';
                                cur[10]='"';
                            }
                            clen+=bytes; // this is how many bytes are actually taken for this Unicode character
                            s+=totjv; 
                            continue;
                        }
                        else rim_report_error ("Invalid hexadecimal escape sequence (\\%c)", *cur);
                    }
                    else rim_report_error ("Invalid escape sequence (\\%c)", *cur);
                }
                else { clen++; s+=2; continue; }
            } else rim_report_error ("Invalid escape sequence at the end");
        }  
        else if (*s == '"')
        { //end of string see if has another one afterwards
            s++;
            while (*s && isspace(*s)) s++;
            if (*s == '"') { s++; continue;} // this is "xxx" "yyy" etc
            found =true; 
            break;
        } else clen++;
        s++;
    }
    while (*s && isspace(*s)) s++;
    if (*s != 0) rim_report_error ("Invalid extra character sequence at the end"); 
    else 
    {
        if (found) 
        {
            if (size != NULL) *size = clen;
            return RIM_CONST_OK;
        } else rim_report_error ("No valid string found, check syntax");
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
    rim_num mlist;
    *list = rim_malloc (mlist=200+strlen(mtext)*15); // 15 is max length of _rim_dexp%ld, so worst case scenario
    rim_num clist = 0;
    char *p = rim_strdup(mtext);
    char *next = p;
    char *c;
    if (comma != NULL) c = rim_strdup(comma); else c=NULL;
    char *par = c;
    rim_num pos = 0;
    char *nc = NULL;
    bool perc = false;
    rim_num fmt_len;
    while (1)
    {
        pos++;
        // find next comma, if any, but only if previously wasn't just a percent sign, there's no advancement of parameter in that case
        if (!perc && par != NULL)
        {
            nc = find_keyword (par, RIM_KEYCOMMA, 0);
            carve_statement (&nc, "format parameters", RIM_KEYCOMMA, 0, 1);
        }
        // find a percent sign
        next = strchr (next, '%');
        if (next == NULL) 
        {
            if (par != NULL) rim_report_error ("There is more output parameters than percent-placeholders in format [%s]", mtext);
            break;
        }
        else
        {
            if (par == NULL) rim_report_error ("There are more percent-placeholders than output parameters, format [%s]", mtext);
        }
        // check for valid percent-placeholders
        perc = false;
        fmt_len = 0; // since assignment of fmt_len below is in ||, meaning it may not execute
        if (*(next+1) == '%') { next+=2; perc = true; continue;} // this is printing out %, do not advance nc since %% doesn't use up parameter
        else if (*(next+1) == 's' || (fmt_len = is_len_format(next+1, "s", 1, false)))
        {
            // should be RIM_DEFSTRING
            if (is_constant_string(par, NULL) != RIM_CONST_OK &&  !cmp_type (check_var (&par, RIM_DEFUNKN), RIM_DEFSTRING)) rim_report_error ("Parameter #%ld in format should be of string type", pos);
            if (fmt_len == 0) next+=2; else next+=1+fmt_len;
            clist += snprintf (*list+clist, mlist-clist, ", %s ",par);
        }
        // this supports f, e, g, %20f, %14e, %8g etc.
        else if ((*(next+1) == 'f' || *(next+1) == 'e' || *(next+1) == 'g') || ((fmt_len = is_len_format(next+1, "f", 1, true)) || (fmt_len = is_len_format(next+1, "e", 1, true)) || (fmt_len = is_len_format(next+1, "g", 1, true))))
        {
            // should be RIM_DEFDOUBLE
            rim_num t = check_var (&par, RIM_DEFUNKN);
            if ((!cmp_type (t, RIM_DEFDOUBLE))) rim_report_error ("Parameter #%ld in format should be of double type", pos);
            if (fmt_len == 0) next+=2; else next+=1+fmt_len;
            clist += snprintf (*list+clist, mlist-clist, ", %s ",par);
        }
        // this supports ld, lo, lx, %20ld, %14lo, %8lx etc.
        else if ((*(next+1) == 'l' && (*(next+2) == 'd' || *(next+2) == 'o' || *(next+2) == 'x')) || ((fmt_len = is_len_format(next+1, "ld", 2, false)) || (fmt_len = is_len_format(next+1, "lo", 2, false)) || (fmt_len = is_len_format(next+1, "lx", 2, false))))
        {
            // should be RIM_DEFNUMBER
            rim_num t = check_var (&par, RIM_DEFUNKN);
            if ((!cmp_type (t, RIM_DEFNUMBER) && !cmp_type (t, RIM_DEFBOOL))) rim_report_error ("Parameter #%ld in format should be of number type", pos);
            if (fmt_len == 0) next+=3; else next+=1+fmt_len;
            clist += snprintf (*list+clist, mlist-clist, ", %s ",par);
        }
        // support for h, meaning hd hu ho hx
        else if ((*(next+1) == 'h' && (*(next+2) == 'd' || *(next+2) == 'u' || *(next+2) == 'o' || *(next+2) == 'x')) || ((fmt_len = is_len_format(next+1, "hd", 2, false)) || (fmt_len = is_len_format(next+1, "hu", 2, false)) || (fmt_len = is_len_format(next+1, "ho", 2, false)) || (fmt_len = is_len_format(next+1, "hx", 2, false))))
        {
            // should be RIM_DEFNUMBER
            rim_num t = check_var (&par, RIM_DEFUNKN);
            if ((!cmp_type (t, RIM_DEFNUMBER) && !cmp_type (t, RIM_DEFBOOL))) rim_report_error ("Parameter #%ld in format should be of number type", pos);
            if (fmt_len == 0) next+=3; else next+=1+fmt_len;
            clist += snprintf (*list+clist, mlist-clist, ", %s ",par);
        }
        // support for hh, meaning hhd hho hhu hhx
        else if ((*(next+1) == 'h' && *(next+2) == 'h' && (*(next+3) == 'd' || *(next+3) == 'u' || *(next+3) == 'o' || *(next+3) == 'x')) || ((fmt_len = is_len_format(next+1, "hhd", 3, false)) || (fmt_len = is_len_format(next+1, "hhu", 3, false)) || (fmt_len = is_len_format(next+1, "hho", 3, false)) || (fmt_len = is_len_format(next+1, "hhx", 3, false))))
        {
            // should be RIM_DEFNUMBER
            rim_num t = check_var (&par, RIM_DEFUNKN);
            if ((!cmp_type (t, RIM_DEFNUMBER) && !cmp_type (t, RIM_DEFBOOL))) rim_report_error ("Parameter #%ld in format should be of number type", pos);
            if (fmt_len == 0) next+=4; else next+=1+fmt_len;
            clist += snprintf (*list+clist, mlist-clist, ", %s ",par);
        }
        else if ((*(next+1) == 'c'))
        {
            // should be RIM_DEFNUMBER (this is character from string)
            rim_num t = check_var (&par, RIM_DEFUNKN);
            if ((!cmp_type (t, RIM_DEFNUMBER))) rim_report_error ("Parameter #%ld in format should be of number type", pos);
            next+=2;
            clist += snprintf (*list+clist, mlist-clist, ", %s ",par);
        }
        else rim_report_error ("Only %%s, %%<length>s, %%l[dox], %%<length>l[dox], %%<length>.<prec><f|g|e> and %%c are valid placeholders in format string, along with %%%% to output the percent sign, found [%s]", mtext);
        par = nc;
    }
}

//
//call-remote from statement text (mtext)
//
void call_service_find (char *mtext, char **finok, char **started)
{
    *finok = find_keyword (mtext, RIM_KEYFINISHEDOKAY, 1);
    *started = find_keyword (mtext, RIM_KEYSTARTED, 1);
}
void call_service_carve (char **finok, char **started)
{
    carve_statement (finok, "call-remote", RIM_KEYFINISHEDOKAY, 0, 1);
    carve_statement (started, "call-remote", RIM_KEYSTARTED, 0, 1);
}
void call_service_define(char **finok, char **started)
{
    define_statement (finok, RIM_DEFNUMBER, false);
    define_statement (started, RIM_DEFNUMBER, false);
}

void call_service (char *mtext)
{
    char *st, *finok, *started;
    call_service_find (mtext, &finok, &started);
    st = find_keyword (mtext, RIM_KEYSTATUS, 1);

    call_service_carve (&finok, &started);
    carve_statement (&st, "call-remote", RIM_KEYSTATUS, 0, 1);

    carve_stmt_obj (&mtext, true, false);
    call_service_define (&finok, &started);
    define_statement (&st, RIM_DEFNUMBER, false);

    call_service_impl (st, finok, started, mtext);
}

//
// Implementation of call-service. st is status (can be NULL), finok
// is finished-okay, started is 'started' clause, mtext is the service
// NOTE: _rim_st is the status regardless of 'status' clause used or not, so that's only checked in run-service
// This works the same as standalone or part of run-service. Note that checking
// if status is present is fine for either case, because it doesn't matter at
// what stage it errors out (if there's no status and there's an error)
//
void call_service_impl (char *st, char *finok, char *started, char *mtext)
{

    char req_var[100];
    char totreq_s[30];
    // this is a list of fcgi calls, otherwise var is mtext, count is the length of array
    snprintf (req_var, sizeof (req_var), "_rim_cli_req_arr%ld", total_fcgi_arr);
    rim_num totreq = outargs(mtext, req_var, "rim_cli *", 0, 0, RIM_DEFSERVICE, RIM_DEFUNKN);
    snprintf (totreq_s, sizeof(totreq_s), "%ld", totreq);
    oprintf ("{_rim_st=rim_call_fcgi (%s, %s, %s%s%s, %s%s%s);}\n", req_var, totreq_s, finok==NULL?"":"&(", finok==NULL?"NULL":finok, finok==NULL?"":")", started==NULL?"":"&(", started==NULL?"NULL":started, started==NULL?"":")" );
    if (st != NULL) oprintf("%s=_rim_st;\n",st);
    else oprintf("if (_rim_st != RIM_OKAY) rim_report_error (\"%s\", \"%s\", (rim_num)%ld, _rim_st);\n", RIM_STMT_FAIL_CHECK, rim_valid_rim_name, lnum);
    total_fcgi_arr++;
}


void read_service_find(char *mtext, char **data, char **error, char **rmsg, char **rstatus)
{
    *data = find_keyword (mtext, RIM_KEYDATA, 1);
    *error = find_keyword (mtext, RIM_KEYERROR, 1);
    *rmsg = find_keyword (mtext, RIM_KEYSTATUSTEXT, 1);
    *rstatus = find_keyword (mtext, RIM_KEYEXITSTATUS, 1);
}
void read_service_carve(char **data, char **error, char **rmsg, char **rstatus)
{
    carve_statement (data, "read-remote", RIM_KEYDATA, 0, 1);
    carve_statement (error, "read-remote", RIM_KEYERROR, 0, 1);
    carve_statement (rmsg, "read-remote", RIM_KEYSTATUSTEXT, 0, 1);
    carve_statement (rstatus, "read-remote", RIM_KEYEXITSTATUS, 0, 1);
}
void read_service_define(char **data, char **error, char **rmsg, char **rstatus)
{
    define_statement (data, RIM_DEFSTRING, false); // exact length set in rim_cli_data
    define_statement (error, RIM_DEFSTRING, false); // exact length set in rim_cli_error
    define_statement (rstatus, RIM_DEFNUMBER, false);
    define_statement (rmsg, RIM_DEFSTRING, false); // exact length set in strdup below
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
// Note _rim_st is NOT used here, as this is the end of execution of run-service
//
void read_service_impl(char *mtext, char *data, char *error, char *st, char *rmsg, char *rstatus)
{
    check_var (&mtext, RIM_DEFSERVICE);
    //
    if (data == NULL && error == NULL && st == NULL && rstatus == NULL) rim_report_error( "at least one clause must be specified in new-remote statement");
    if (data != NULL) oprintf ("%s = rim_cli_data(%s);\n", data, mtext);
    if (error != NULL) oprintf ("%s = rim_cli_error(%s);\n", error, mtext);
    if (st != NULL) oprintf ("%s = (%s)->return_code;\n", st, mtext);
    else oprintf("if ((%s)->return_code != RIM_OKAY) rim_report_error (\"%s\", \"%s\", (rim_num)%ld, (rim_num)((%s)->return_code));\n", mtext, RIM_STMT_FAIL_CHECK, rim_valid_rim_name, lnum, mtext);
    if (rmsg != NULL) oprintf ("%s = rim_strdup((%s)->errm);\n", rmsg, mtext);
    if (rstatus != NULL) oprintf ("%s = (%s)->req_status;\n", rstatus, mtext);
}

//
//read-remote from statement text (mtext)
//
void read_service (char *mtext)
{
    char *data, *error, *st, *rmsg, *rstatus;
    read_service_find(mtext, &data, &error, &rmsg, &rstatus);
    st = find_keyword (mtext, RIM_KEYSTATUS, 1);
    read_service_carve(&data, &error, &rmsg, &rstatus);
    carve_statement (&st, "read-remote", RIM_KEYSTATUS, 0, 1);

    carve_stmt_obj (&mtext, true, false);
    read_service_define(&data, &error, &rmsg, &rstatus);
    define_statement (&st, RIM_DEFNUMBER, false);
    
    read_service_impl(mtext, data, error, st, rmsg, rstatus);
}

void new_service_find(char *mtext, char **server, char **method, char **apath, char **req, char **body, char **upay, char **timeout, char **env, char **loc, char **url)
{
    *server = find_keyword (mtext, RIM_KEYLOCATION, 1);
    *method = find_keyword (mtext, RIM_KEYMETHOD, 1);
    *apath = find_keyword (mtext, RIM_KEYAPPPATH, 1);
    *req = find_keyword (mtext, RIM_KEYREQUESTPATH, 1);
    *body = find_keyword (mtext, RIM_KEYREQUESTBODY, 1);
    *upay = find_keyword (mtext, RIM_KEYURLPARAMS, 1);
    *timeout = find_keyword (mtext, RIM_KEYTIMEOUT, 1);
    *env = find_keyword (mtext, RIM_KEYENVIRONMENT, 1);
    *loc = find_keyword (mtext, RIM_KEYLOCAL, 1);
    *url = find_keyword (mtext, RIM_KEYURLPATH, 1);
}
void new_service_carve(char **server, char **method, char **apath, char **req, char **body, char **upay, char **timeout, char **env, char **loc, char **url)
{
    carve_statement (server, "new-remote", RIM_KEYLOCATION, 0, 1);
    carve_statement (method, "new-remote", RIM_KEYMETHOD, 0, 1);
    carve_statement (apath, "new-remote", RIM_KEYAPPPATH, 0, 1);
    carve_statement (req, "new-remote", RIM_KEYREQUESTPATH, 0, 1);
    carve_statement (body, "new-remote", RIM_KEYREQUESTBODY, 0, 1);
    carve_statement (upay, "new-remote", RIM_KEYURLPARAMS, 0, 1);
    carve_statement (timeout, "new-remote", RIM_KEYTIMEOUT, 0, 1);
    carve_statement (env, "new-remote", RIM_KEYENVIRONMENT, 0, 1);
    carve_statement (loc, "new-remote", RIM_KEYLOCAL, 0, 1);
    carve_statement (url, "new-remote", RIM_KEYURLPATH, 0, 1);
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
    check_var (&server, RIM_DEFSTRING);
    check_var (&method, RIM_DEFSTRING);
    check_var (&apath, RIM_DEFSTRING);
    check_var (&req, RIM_DEFSTRING);
    check_var (&upay, RIM_DEFSTRING);
    check_var (&loc, RIM_DEFSTRING);
    check_var (&timeout, RIM_DEFNUMBER);
    check_var (&url, RIM_DEFSTRING);
    //
    if (server != NULL && loc != NULL) rim_report_error ("Only one of 'local' or 'location' can be used");
    if (server == NULL && loc == NULL) rim_report_error ("Either 'local' or 'location' must be specified");
    if (url != NULL && (apath != NULL || req != NULL || upay != NULL)) rim_report_error ("Use either 'url-path' clause, or app-path/request-path/url-payload clauses");
    if (url == NULL && (apath == NULL || req == NULL)) rim_report_error ("If 'url-path' clause is not used, then you must specify app-path and request-path clauses");

    // process body clause after ward because it is done within a copy of carved out 'body' above
    char *ctype = NULL;
    char *content = NULL;
    char *clen = NULL;
    char *mbody = NULL;
    if (body != NULL)
    {
        mbody = rim_strdup (body);
        content = find_keyword (mbody, RIM_KEYCONTENT, 1);
        clen = find_keyword (mbody, RIM_KEYCONTENTLENGTH, 1);
        ctype = find_keyword (mbody, RIM_KEYCONTENTTYPE, 1);
        carve_statement (&content, "new-remote", RIM_KEYCONTENT, 0, 1);
        carve_statement (&clen, "new-remote", RIM_KEYCONTENTLENGTH, 0, 1);
        carve_statement (&ctype, "new-remote", RIM_KEYCONTENTTYPE, 0, 1);
        // can place them here because there's no more carve_statement, make_mem is ALWAYS after
        // ALL carve_statement ARE DONE!
        make_mem (&content);
        make_mem (&ctype);
        //
        check_var (&content, RIM_DEFSTRING);
        check_var (&ctype, RIM_DEFSTRING);
        check_var (&clen, RIM_DEFNUMBER);
    }

    if (method == NULL) { method = "\"GET\""; make_mem(&method);} // by default method is get 
    if (body == NULL && ctype != NULL) rim_report_error( "'content-type' cannot be specified without 'request-body' in new-remote statement");
    if (body == NULL && clen != NULL) rim_report_error( "'content-length' cannot be specified without 'request-body' in new-remote statement");
    if (body != NULL && ctype == NULL) rim_report_error( "'content-type' must be specified for 'request-body' in new-remote statement");
    if (body != NULL && content == NULL) rim_report_error( "'content' must be specified for 'request-body' in new-remote statement");


    char env_var[100];
    snprintf (env_var, sizeof (env_var), "_rim_cli_env_arr%ld", total_fcgi_arr);
    if (env != NULL) outargs(env, env_var, "char *", 0, 1, RIM_DEFSTRING, RIM_DEFSTRING);

    // define for fcgi thread
    define_statement (&mtext, RIM_DEFSERVICE, true);
    oprintf ("rim_set_fcgi (&(%s), %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s);\n", mtext, server==NULL?loc:server, method, apath==NULL?"RIM_EMPTY_STRING":apath, req==NULL?"RIM_EMPTY_STRING":req, upay==NULL?url:upay,ctype==NULL?"NULL":ctype, body==NULL?"NULL":content, clen==NULL?"0":clen, timeout==NULL?"0":timeout, env==NULL?"NULL":env_var, loc!=NULL?"true":"false", url!=NULL?"true":"false");

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
    st = find_keyword (mtext, RIM_KEYSTATUS, 1); // common to read-/call-remote

    // carve keywords
    new_service_carve(&server, &method, &apath, &req, &body, &upay, &timeout, &env, &loc, &url);
    call_service_carve (&finok, &started);
    read_service_carve(&data, &error, &rmsg, &rstatus);
    carve_statement (&st, "run-remote", RIM_KEYSTATUS, 0, 1); // carve common to read/call service
    carve_stmt_obj (&mtext, true, false);

    //define statements
    read_service_define(&data, &error, &rmsg, &rstatus);
    define_statement (&st, RIM_DEFNUMBER, false);
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
    // we do want to read-remote if RIM_OKAY or RIM_ERR_FAILED, because we get the actual error messsage/code there
    // only for RIM_ERR_TOO_MANY or RIM_ERR_MEMORY we don't want to.
    oprintf ("if (_rim_st == RIM_OKAY || _rim_st == RIM_ERR_FAILED) {\n");
    read_service_impl(mtext, data, error, st, rmsg, rstatus);
    oprintf ("} else {\n");
    // this is if err_too_many (which can't happen since it's only one, or out of memory, so say it
    if (rmsg != NULL) oprintf ("if (_rim_st == RIM_ERR_MEMORY) %s = rim_strdup(\"Out of memory\"); else %s = rim_strdup (\"Unknown error\");\n", rmsg,rmsg);
    else oprintf(";"); // for compiler peace of mind
    if (st==NULL) oprintf("rim_report_error (\"%s\", \"%s\", (rim_num)%ld, _rim_st);\n", RIM_STMT_FAIL_CHECK, rim_valid_rim_name, lnum); // this really should never happen since call_service_impl() should catch this
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
    rim_num tot;
    rim_num len;
    char *q = rim_malloc (tot=2*(len=strlen(*clause))+3+1); // 2x for each char escaped, 2 quotes plus null, plus 1 for luck
    char *curr = q;
    *(curr++) = '"';
    rim_num i;
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
// Make rimstone memory out of string literal clause. 
// We use this system instead of a structure that has a pointer/length etc. because with that system we would use
// a pointer to this structure, which has a pointer to memory. That's double redirection which slows down everything
// a lot. Making a bit of memory in a few special cases is a much better performing system.
// *clause is the memory to convert, and is the result, which holds the name of variable created.
// Returns true if constant was actually made (its name is *clause).
//
bool make_mem (char **clause)
{
    if (*clause == NULL) return false;
    char *s = *clause;
    while (isspace(*s)) s++;
    rim_num size;
    rim_num cs = is_constant_string(s, &size);
    if (cs != RIM_CONST_OK && cs != RIM_CONST_NOT) rim_report_error ("String [%s] is not correctly formed", s);
    if (cs != RIM_CONST_OK) return false; // this captures variables and anything with _ in front
                                            // which we assume is a define-d literal (like __FILE__)

    static rim_num gmem = 0;
#define RIM_MAKE_MEM_LEN 35
    char *makem = rim_malloc(RIM_MAKE_MEM_LEN);
    // if we're converting literal into string variable, this is where we give it name which propogates to caller
    snprintf (makem, RIM_MAKE_MEM_LEN, "_rim__" RIM_STRINGIZE(RIM_DEFSTRING)  "__ret%ld", gmem);
    //
    // create new value, copy string literal to it (once for the life of process), set it process-scoped
    // then return the name of that variable instead of constant
    // variable is made only once per process
    // it is "literal", because we declare it as a char array really!
    // source file name is immutable, or at least is supposed to 
    // static makes sure the pointer value remains outside this scope
    // and constant initi ensures it remains exactly the same pointer

    // Get the \x00 representation of the header. String constant header uses id of -1, meaning it's NOT in vm[] (reserved for future use, as just any negative)
    // The rest is the length. We construct the header, then  imprint it into a constant string, in the same byte order as the target architecture
    // This means you can't move binaries from one arch to the other, but then again, you can't do that anyway.
    rim_head head;
    head.id = -1;
    head.len = size+1;
    // cast to block of memory
    unsigned char *hstr = (unsigned char*)&head;
    // memory for the actual code \x00...
    char shead[4*RIM_ALIGN+1];
    rim_num i;
    rim_num ptr = 0;
    // go through binary bytes
    for (i = 0; i < (rim_num)RIM_ALIGN; i++)
    {
        // construct \x
        shead[ptr++] = '\\';
        shead[ptr++] = 'x';
        // then put in two hexadecimal digits
        char *chead = shead + ptr;
        RIM_HEX_FROM_BYTE(chead,hstr[i]);
        ptr += 2;
    }
    shead[ptr] = 0; // finish the string containing id and lenght
    //
    //
    oprintf ("static char _rim_gstr%ld[] = \"%s\" %s;\n",gmem,shead,*clause); // 16 bytes is for RIM_ALIGN
                                                                            // so whatever number that is, that's how many spaces are in front of *clause
    //
    //
    // NOTE: internal variable that starts with _rim__<string def code (see rim.h)>__ is **always** a string!! check_var **depends** on this
    //
    //
    oprintf ("static char *%s =  _rim_gstr%ld+RIM_ALIGN;\n", makem, gmem); 
    //
    // Adding variable must be done at the level when it's generated since we count levels by {} in code generated
    // So add it before the next { down here
    //
    add_var (makem, RIM_DEFSTRING, *clause, false);
    *clause = makem; // must be AFTER add_var or otherwise realname in add_var would be generated name!
    // Faster than a branch, better for gcc to optimize
    //
    gmem++;
    return true;
}

//
//
// Find if there are simple comparisons in cm. eq is equal, neq is not-equal, less is lesser-than
// lesseq is lesser-equal, gr is greater-than, greq is greater-equal, mod is every, notmod is not-every,
// cont is contains, notcont is not-contains. 
// The caller will determine which one is to be used, which will be the first one; another use of this 
// is to determine if cm is the comparison at all.
//
//
void find_cond (char *cm, char **eq, char **neq, char **less, char **lesseq, char **gr, char **greq, char **mod, char **notmod, char **cont, char **notcont)
{
    *eq = find_keyword (cm, RIM_KEYEQUAL, 1);
    *neq = find_keyword (cm, RIM_KEYNOTEQUAL, 1);
    *less = find_keyword (cm, RIM_KEYLESSERTHAN, 1);
    *lesseq = find_keyword (cm, RIM_KEYLESSEREQUAL, 1);
    *gr = find_keyword (cm, RIM_KEYGREATERTHAN, 1);
    *greq = find_keyword (cm, RIM_KEYGREATEREQUAL, 1);
    *mod = find_keyword (cm, RIM_KEYEVERY, 1);
    *notmod = find_keyword (cm, RIM_KEYNOTEVERY, 1);
    *cont = find_keyword (cm, RIM_KEYCONTAIN, 1);
    *notcont = find_keyword (cm, RIM_KEYNOTCONTAIN, 1);
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
    char *errorm = NULL;
    char *len = NULL;
    char *or = NULL;
    char *and = NULL;

    static rim_num gvar_stmt = 0; // used for the entire condition
    static rim_num gvar = 0; // used for variables for each condition in a service
    bool done_stmt = false;
    rim_num tot_open = 0; // number of open { which we need to close with }
    bool is_or = false;
    bool is_and =false;
    oprintf ("%s", bifs);
    while (1) 
    {
        // Look for any and/or first
        or = find_keyword (cm, RIM_KEYOR, 1);
        and = find_keyword (cm, RIM_KEYAND, 1);
        if (or != NULL && and != NULL) rim_report_error ("Can use 'or' or 'and' clauses, but not both");
        rim_num or_pos = INT_MAX;
        rim_num and_pos = INT_MAX;
        // Cut off first and/or segment, if there is one (if not, it's just a simple condition)
        if (or) or_pos = or - cm;
        if (and) and_pos = and - cm;
        rim_num op = MIN (or_pos, and_pos);
        if (op != INT_MAX)
        {
            if (or_pos == op) { is_or = true; carve_statement (&or, "if-true", RIM_KEYOR, 0, 1); }
            else if (and_pos == op) { is_and = true; carve_statement (&and, "if-true", RIM_KEYAND, 0, 1); }
        }
        // else both is_or and is_true remain false
        //
        if (!done_stmt)
        {
            done_stmt = true;
            // statement boolean (the one we actually check in if) is initially false for OR and true for AND
            // that's because of the nature of or/and operators, so that initial value doesn't affect boolean computation outcome
            if (is_or )oprintf ("bool _rim_if_stmt%ld = false;\n", gvar_stmt);
            else if (is_and )oprintf ("bool _rim_if_stmt%ld = true;\n", gvar_stmt);
            else oprintf ("bool _rim_if_stmt%ld;\n", gvar_stmt);
        }

        // then find any relevant keywords in the first segment (which may be the only (remaining) one)
        find_cond (cm, &eq, &neq, &less, &lesseq, &gr, &greq, &mod, &notmod, &cont, &notcont);
        caseins = find_keyword (cm, RIM_KEYCASEINSENSITIVE, 1);
        errorm = find_keyword (cm, RIM_KEYERRORMARGIN, 1);
        len = find_keyword (cm, RIM_KEYLENGTH, 1);

        // do not use caseins, errorm and len here, those are *only* as a subclause to one of these!
        if (eq == NULL && neq == NULL && less == NULL && lesseq == NULL && gr == NULL && mod == NULL && notmod == NULL && greq == NULL && cont == NULL && notcont == NULL ) rim_report_error ("conditional clause is missing or empty");

        rim_num eq_pos = INT_MAX;
        rim_num neq_pos = INT_MAX;
        rim_num less_pos = INT_MAX;
        rim_num lesseq_pos = INT_MAX;
        rim_num gr_pos = INT_MAX;
        rim_num mod_pos = INT_MAX;
        rim_num notmod_pos = INT_MAX;
        rim_num greq_pos = INT_MAX;
        rim_num cont_pos = INT_MAX;
        rim_num notcont_pos = INT_MAX;
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
        if (caseins) carve_statement (&caseins, "if-true", RIM_KEYCASEINSENSITIVE, 0, 0);  // carve it out, so it doesn't mess up other clauses
        if (errorm) carve_statement (&errorm, "if-true", RIM_KEYERRORMARGIN, 0, 1);  // carve it out, so it doesn't mess up other clauses
        if (len) carve_statement (&len, "if-true", RIM_KEYLENGTH, 0, 1);  // carve it out, so it doesn't mess up other clauses

        check_var (&len, RIM_DEFNUMBER);

        rim_num m = MIN(MIN (MIN (MIN (MIN (MIN(eq_pos, neq_pos), MIN(less_pos,lesseq_pos)), MIN(gr_pos,greq_pos)), MIN(notcont_pos,cont_pos)), mod_pos), notmod_pos);
        if (m == INT_MAX) break;
        // must be after break or _rim_ifcond.. would be duplicated or unused
        oprintf ("bool _rim_ifcond%ld;\n", gvar);

        if (eq_pos == m) 
        { 
            carve_statement (&eq, "if-true", RIM_KEYEQUAL, 1, 1); 
            // n_cm is so that cm is preserved for the next loop of search for the next or/and
            // make_mem for n_cm must be repeated for each condition b/c it must be after carve_statements
            char *n_cm = cm;
            make_mem (&n_cm);
            rim_num leq = strlen (eq);
            eq = rim_trim_ptr(eq,  &leq);
            //comparison with NULL is undocumented and unsuported! Only for testing where C is allowed.
            if (!strcmp (eq, "NULL")) oprintf("_rim_ifcond%ld = ((%s) == NULL ? true:false);\n", gvar, n_cm);  else 
            {
                make_mem (&eq);
                //
                rim_num t1 = check_var (&n_cm, RIM_DEFUNKN);
                check_var (&eq, t1); 
                if (!cmp_type (t1,RIM_DEFSTRING) && len != NULL) rim_report_error ("length can be used with strings only");
                if (!cmp_type (t1,RIM_DEFSTRING) && caseins != NULL) rim_report_error ("case-insensitive can be used with strings only");
                if (!cmp_type (t1,RIM_DEFDOUBLE) && errorm != NULL) rim_report_error ("error-margin can be used with doubles only");
                // Here now can call specific function based on type and there's no need for Generic()
                if (cmp_type (t1,RIM_DEFSTRING))
                {
                    if (!len) oprintf("_rim_ifcond%ld = rim_cm_str%s((%s), rim_mem_get_len(%s),(%s), rim_mem_get_len(%s));\n", gvar, caseins?"_c":"", n_cm, n_cm, eq, eq);  
                    else oprintf("_rim_ifcond%ld = rim_cm_str%s_l((%s), rim_mem_get_len(%s), (%s), rim_mem_get_len(%s), (%s));\n",gvar, caseins?"_c":"", n_cm, n_cm, eq, eq, len);  
                }
                else if (cmp_type (t1, RIM_DEFDOUBLE))
                {
                    check_var (&errorm, RIM_DEFDOUBLE); // epsilon must be double 
                    if (errorm != NULL) oprintf("_rim_ifcond%ld = rim_cm_dbl_eps((%s), (%s), (%s));\n", gvar, n_cm, eq, errorm);  
                    else oprintf("_rim_ifcond%ld = rim_cm_dbl((%s), (%s));\n", gvar, n_cm, eq);  
                }
                else if (cmp_type (t1, RIM_DEFNUMBER))
                {
                    oprintf("_rim_ifcond%ld = rim_cm_num((%s), (%s));\n", gvar, n_cm, eq);  
                }
                else if (cmp_type (t1, RIM_DEFBOOL))
                {
                    oprintf("_rim_ifcond%ld = rim_cm_bool((%s), (%s));\n", gvar, n_cm, eq);  
                }
            }
        }
        else if (neq_pos == m) 
        { 
            carve_statement (&neq, "if-true", RIM_KEYNOTEQUAL, 1, 1); 
            // n_cm is so that cm is preserved for the next loop of search for the next or/and
            // make_mem for n_cm must be repeated for each condition b/c it must be after carve_statements
            char *n_cm = cm;
            make_mem (&n_cm);
            rim_num lneq = strlen (neq);
            neq = rim_trim_ptr(neq,  &lneq);
            //comparison with NULL is undocumented and unsuported! Only for testing where C is allowed.
            if (!strcmp (neq, "NULL")) oprintf("_rim_ifcond%ld = ((%s) != NULL ? true:false);\n",gvar, n_cm);  else 
            {
                make_mem (&neq);
                //
                rim_num t1 = check_var (&n_cm, RIM_DEFUNKN);
                check_var (&neq, t1);
                if (cmp_type (t1,RIM_DEFSTRING))
                {
                    if (!len) oprintf("_rim_ifcond%ld = !rim_cm_str%s((%s), rim_mem_get_len(%s),(%s), rim_mem_get_len(%s));\n", gvar, caseins?"_c":"", n_cm, n_cm, neq, neq);  
                    else oprintf("_rim_ifcond%ld = !rim_cm_str%s_l((%s), rim_mem_get_len(%s), (%s), rim_mem_get_len(%s), (%s));\n",gvar, caseins?"_c":"", n_cm, n_cm, neq, neq, len);  
                }
                else if (cmp_type (t1, RIM_DEFNUMBER))
                {
                    oprintf("_rim_ifcond%ld = !rim_cm_num((%s), (%s));\n", gvar, n_cm, neq);  
                }
                else if (cmp_type (t1, RIM_DEFDOUBLE))
                {
                    check_var (&errorm, RIM_DEFDOUBLE); // epsilon must be double 
                    if (errorm != NULL) oprintf("_rim_ifcond%ld = !rim_cm_dbl_eps((%s), (%s), (%s));\n", gvar, n_cm, neq, errorm);  
                    else oprintf("_rim_ifcond%ld = !rim_cm_dbl((%s), (%s));\n", gvar, n_cm, neq);  
                }
                else if (cmp_type (t1, RIM_DEFBOOL))
                {
                    oprintf("_rim_ifcond%ld = !rim_cm_bool((%s), (%s));\n", gvar, n_cm, neq);  
                }
            }
        }
        else if (lesseq_pos == m) 
        { 
            carve_statement (&lesseq, "if-true", RIM_KEYLESSEREQUAL, 1, 1); 
            // n_cm is so that cm is preserved for the next loop of search for the next or/and
            // make_mem for n_cm must be repeated for each condition b/c it must be after carve_statements
            char *n_cm = cm;
            make_mem (&n_cm);
            make_mem (&lesseq);
            //
            rim_num t1 = check_var (&n_cm, RIM_DEFUNKN);
            check_var (&lesseq, t1);
            if (cmp_type (t1,RIM_DEFSTRING))
            {
                if (!len) oprintf("_rim_ifcond%ld = rim_cm_str_lesseq%s((%s),rim_mem_get_len(%s), (%s), rim_mem_get_len(%s));\n", gvar, caseins?"_c":"", n_cm, n_cm, lesseq, lesseq); 
                else oprintf("_rim_ifcond%ld = rim_cm_str_lesseq%s_l((%s), rim_mem_get_len(%s), (%s),rim_mem_get_len(%s), (%s));\n", gvar, caseins?"_c":"", n_cm, n_cm, lesseq, lesseq, len); 
            }
            else if (cmp_type (t1, RIM_DEFDOUBLE))
            {
                oprintf("_rim_ifcond%ld = rim_cm_dbl_lesseq((%s), (%s));\n", gvar, n_cm, lesseq);  
            }
            else if (cmp_type (t1, RIM_DEFNUMBER))
            {
                oprintf("_rim_ifcond%ld = rim_cm_num_lesseq((%s), (%s));\n", gvar, n_cm, lesseq);  
            }
        }
        else if (greq_pos == m) 
        { 
            carve_statement (&greq, "if-true", RIM_KEYGREATEREQUAL, 1, 1); 
            // n_cm is so that cm is preserved for the next loop of search for the next or/and
            // make_mem for n_cm must be repeated for each condition b/c it must be after carve_statements
            char *n_cm = cm;
            make_mem (&n_cm);
            make_mem (&greq);
            //
            rim_num t1 = check_var (&n_cm, RIM_DEFUNKN);
            check_var (&greq, t1);
            if (cmp_type (t1,RIM_DEFSTRING))
            {
                if (!len) oprintf("_rim_ifcond%ld = rim_cm_str_greq%s((%s), rim_mem_get_len(%s), (%s), rim_mem_get_len(%s) );\n", gvar, caseins?"_c":"", n_cm, n_cm, greq, greq); 
                else oprintf("_rim_ifcond%ld = rim_cm_str_greq%s_l((%s), rim_mem_get_len(%s), (%s), rim_mem_get_len(%s), (%s));\n", gvar, caseins?"_c":"", n_cm, n_cm, greq, greq, len); 
            }
            else if (cmp_type (t1, RIM_DEFDOUBLE))
            {
                oprintf("_rim_ifcond%ld = rim_cm_dbl_greq((%s), (%s));\n", gvar, n_cm, greq);  
            }
            else if (cmp_type (t1, RIM_DEFNUMBER))
            {
                oprintf("_rim_ifcond%ld = rim_cm_num_greq((%s), (%s));\n", gvar, n_cm, greq);  
            }
        }
        else if (less_pos == m) 
        {
            carve_statement (&less, "if-true", RIM_KEYLESSERTHAN, 1, 1); 
            // n_cm is so that cm is preserved for the next loop of search for the next or/and
            // make_mem for n_cm must be repeated for each condition b/c it must be after carve_statements
            char *n_cm = cm;
            make_mem (&n_cm);
            make_mem (&less);
            //
            rim_num t1 = check_var (&n_cm, RIM_DEFUNKN);
            check_var (&less, t1);
            if (cmp_type (t1,RIM_DEFSTRING))
            {
                if (!len) oprintf("_rim_ifcond%ld = rim_cm_str_less%s((%s), rim_mem_get_len(%s), (%s), rim_mem_get_len(%s));\n", gvar, caseins?"_c":"", n_cm, n_cm, less, less); 
                else oprintf("_rim_ifcond%ld = rim_cm_str_less%s_l((%s), rim_mem_get_len(%s), (%s), rim_mem_get_len(%s), (%s));\n", gvar, caseins?"_c":"", n_cm, n_cm, less, less, len); 
            }
            else if (cmp_type (t1, RIM_DEFDOUBLE))
            {
                oprintf("_rim_ifcond%ld = rim_cm_dbl_less((%s), (%s));\n", gvar, n_cm, less);  
            }
            else if (cmp_type (t1, RIM_DEFNUMBER))
            {
                oprintf("_rim_ifcond%ld = rim_cm_num_less((%s), (%s));\n", gvar, n_cm, less);  
            }
        }
        else if (notmod_pos == m) 
        { 
            carve_statement (&notmod, "if-true", RIM_KEYNOTEVERY, 1, 1); 
            // n_cm is so that cm is preserved for the next loop of search for the next or/and
            // make_mem for n_cm must be repeated for each condition b/c it must be after carve_statements
            char *n_cm = cm;
            // no need for make_me since these must be numbers
            //
            rim_num t1 = check_var (&n_cm, RIM_DEFNUMBER);
            check_var (&notmod, t1);
            oprintf("_rim_ifcond%ld = ((%s)%%(%s));\n", gvar, n_cm, notmod); 
        }
        else if (mod_pos == m) 
        { 
            carve_statement (&mod, "if-true", RIM_KEYEVERY, 1, 1); 
            // n_cm is so that cm is preserved for the next loop of search for the next or/and
            // make_mem for n_cm must be repeated for each condition b/c it must be after carve_statements
            char *n_cm = cm;
            // no need for make_me since these must be numbers
            //
            rim_num t1 = check_var (&n_cm, RIM_DEFNUMBER);
            check_var (&mod, t1);
            oprintf("_rim_ifcond%ld = !((%s)%%(%s));\n", gvar, n_cm, mod); 
        }
        else if (gr_pos == m) 
        { 
            carve_statement (&gr, "if-true", RIM_KEYGREATERTHAN, 1, 1); 
            // n_cm is so that cm is preserved for the next loop of search for the next or/and
            // make_mem for n_cm must be repeated for each condition b/c it must be after carve_statements
            char *n_cm = cm;
            make_mem (&n_cm);
            make_mem (&gr);
            //
            rim_num t1 = check_var (&n_cm, RIM_DEFUNKN);
            check_var (&gr, t1);
            if (cmp_type (t1,RIM_DEFSTRING))
            {
                if (!len) oprintf("_rim_ifcond%ld = rim_cm_str_gr%s((%s),rim_mem_get_len(%s), (%s), rim_mem_get_len(%s));\n", gvar, caseins?"_c":"", n_cm, n_cm, gr, gr); 
                else oprintf("_rim_ifcond%ld = rim_cm_str_gr%s_l((%s), rim_mem_get_len(%s), (%s), rim_mem_get_len(%s), (%s));\n", gvar, caseins?"_c":"", n_cm, n_cm, gr, gr, len); 
            }
            else if (cmp_type (t1, RIM_DEFDOUBLE))
            {
                oprintf("_rim_ifcond%ld = rim_cm_dbl_gr((%s), (%s));\n", gvar, n_cm, gr);  
            }
            else if (cmp_type (t1, RIM_DEFNUMBER))
            {
                oprintf("_rim_ifcond%ld = rim_cm_num_gr((%s), (%s));\n", gvar, n_cm, gr);  
            }
        }
        else if (cont_pos == m) 
        { 
            carve_statement (&cont, "if-true", RIM_KEYCONTAIN, 1, 1); 
            // n_cm is so that cm is preserved for the next loop of search for the next or/and
            // make_mem for n_cm must be repeated for each condition b/c it must be after carve_statements
            char *n_cm = cm;
            make_mem (&n_cm);
            make_mem (&cont);
            //
            check_var (&n_cm, RIM_DEFSTRING);
            rim_num t_cont = check_var (&cont, RIM_DEFUNKN);
            if (cmp_type (t_cont,RIM_DEFSTRING)) 
            {
                if (caseins) oprintf("_rim_ifcond%ld = (rim_contain_str_c((%s), rim_mem_get_len(%s), (%s))!=NULL);\n", gvar, n_cm, n_cm, cont);
                else oprintf("_rim_ifcond%ld = (rim_contain_str((%s), rim_mem_get_len(%s), (%s), rim_mem_get_len (%s))!=NULL);\n", gvar, n_cm, n_cm, cont, cont);
            }
            else if (cmp_type (t_cont, RIM_DEFNUMBER)) 
            {
                oprintf("_rim_ifcond%ld = (rim_contain_int%s((%s), rim_mem_get_len(%s), (%s))!=NULL);\n", gvar, caseins?"_c":"", n_cm, n_cm, cont);
            }
        }
        else if (notcont_pos == m) 
        { 
            carve_statement (&notcont, "if-true", RIM_KEYNOTCONTAIN, 1, 1); 
            // n_cm is so that cm is preserved for the next loop of search for the next or/and
            // make_mem for n_cm must be repeated for each condition b/c it must be after carve_statements
            char *n_cm = cm;
            make_mem (&n_cm);
            make_mem (&notcont);
            //
            check_var (&n_cm, RIM_DEFSTRING);
            rim_num t_notcont=check_var (&notcont, RIM_DEFUNKN);
            if (cmp_type (t_notcont,RIM_DEFSTRING)) 
            {
                if (caseins) oprintf("_rim_ifcond%ld = (rim_contain_str_c((%s), rim_mem_get_len(%s), (%s))==NULL);\n", gvar, n_cm, n_cm, notcont);
                else oprintf("_rim_ifcond%ld = (rim_contain_str((%s), rim_mem_get_len(%s), (%s), rim_mem_get_len (%s))==NULL);\n", gvar, n_cm, n_cm, notcont, notcont);
            }
            else if (cmp_type (t_notcont, RIM_DEFNUMBER)) 
            {
                oprintf("_rim_ifcond%ld = (rim_contain_int%s((%s), rim_mem_get_len(%s), (%s))==NULL);\n", gvar, caseins?"_c":"", n_cm, n_cm, notcont);
            }
        }
        else rim_report_error ("Internal error in logic condition");

        if (op != INT_MAX) 
        {
            if (is_or) { oprintf ("if (!_rim_ifcond%ld) {\n",  gvar); cm = or; }
            else if (is_and) { oprintf ("if (_rim_ifcond%ld) {\n", gvar); cm = and; }
            else rim_report_error ("Internal error in logic condition");
            tot_open++;
        }
        else
        {
            // last and/or which determines the whole condition, or there was no and/or(just a single condition)
            oprintf ("_rim_if_stmt%ld = _rim_ifcond%ld;\n", gvar_stmt, gvar);

        }
        gvar++;
        if (op == INT_MAX) break;
    }
    rim_num i;
    for (i = 0; i < tot_open; i++) 
    {
        if (is_or) oprintf("} else  _rim_if_stmt%ld = true;\n", gvar_stmt);
        else if (is_and) oprintf("} else  _rim_if_stmt%ld = false;\n", gvar_stmt);
    }
    oprintf("\n%s _rim_if_stmt%ld %s\n", ifs, gvar_stmt, ife); // final result
    gvar_stmt++;
}


//
// Check if currently processed file name (file_name) matches request name (reqname). Error out if not.
//
void match_file (char *file_name, char *reqname)
{
    // file_name and reqname are decorated, so no /, they are just plain flattened names, no need for basename. So in most cases just a copy of file_name would suffice. HOWEVER, in some cases, like for dispatcher, the actual /var/lib/gg/<user>/lib/... file name is passed as file_name - this is JUST for generated files - and reqname is just the name, so getting basename works for that. But for regular source files, it just makes a copy
    char *base = rim_basename (file_name);
    char *dot = strchr (base, '.');
    if (dot == NULL)  rim_report_error( "Source file name [%s] must have .rim extension", file_name);
    *dot = 0; // cut file_name short for easy comparison
    // do not use source file name b/c rim_report_error will print it properly
    char *r = undecorate (reqname);
    char *ubase = undecorate (base);
    rim_replace_string (base, strlen(base)+1, "-", "_", 1, NULL, 1); // this is okay since boundaries of allocated 'base' aren't crossed
    if (rim_single_file) // must be a full match for --single-file in gg
    {
        if (strcmp (reqname, base)) rim_report_error( "Source file name does not match request handler name [%s]", r);
    }
    else
    {
        rim_num base_len = strlen(base);
        // request name has to match the base in the beginning and either end there, or have __ afterwards for / (since these are undecorated names)
        if (strncmp (reqname, base, base_len) || (!strncmp (reqname, base, base_len) && !(reqname[base_len] == 0 || (reqname[base_len] == '_' && reqname[base_len+1] == '_')))) rim_report_error( "Request handler name [%s] does not begin with source file path [%s]", r, ubase);
    }
    rim_free (r);
    rim_free (base);
    rim_free (ubase);
}

//
// Check the result of parsing (after last carve_statement()), so that mtext is trimmed,
// and if has_value is true, check that it's not empty
// is_constant is true if this can be a string constant, false otherwise
// Returns length of mtext
// This MUST ALWAYS be after ALL other carve_statement calls!!!!! Or otherwise junk may just pass parsing as okay (even if it doesn't do anything)!
//
rim_num carve_stmt_obj (char **mtext, bool has_value, bool is_constant)
{
    // Make sure mtext is not empty, there has to be something
    rim_num ml = strlen (*mtext);
    *mtext = rim_trim_ptr(*mtext,  &ml);
    if (has_value && (*mtext)[0] == 0) rim_report_error("Object argument following the statement name is missing");
    if (!has_value && (*mtext)[0] != 0) rim_report_error("This statement should have no object argument");
    if (is_constant) make_mem (mtext);
    return ml;
}


//
// Make variable out of expression *v of type 'type'. *v contains the name of newly generated variable name.
// This prevents double evaluation, as whatever is used is assigned once (and only if it's an expression).
// If internal is true, the variable name generated is _xxx, yielding close to 46,000 names - these are used
// as internal variables only and never referenced by user
// This is for input params only.
//
void make_var (char **v, rim_num type, bool internal)
{
    char *beg = *v;
    while (isspace(*beg)) beg++;
    //
    // First make sure this isn't just a variable name, if so, no need to do anything
    //
    char *begv = beg;
    while (isalpha(*begv) || *begv == '_') begv++;
    while (isspace(*begv)) begv++;
    if (*begv == 0) return;
    //
    //
    char *var;
#define RIM_MAX_VEXP_LEN 64
    var = rim_malloc(RIM_MAX_VEXP_LEN);
    //
    // Make sure this isn't just a number, if so, all we need is to cast them as rim_num
    // otherwise they would be int, causing errors
    //
    if (cmp_type (type , RIM_DEFNUMBER) || cmp_type (type , RIM_DEFDOUBLE))
    {
        rim_num st;
        if (cmp_type (type , RIM_DEFNUMBER)) rim_str2num (beg, 0, &st); else rim_str2dbl (beg, &st);
        if (st == RIM_OKAY) 
        {
            snprintf (var, RIM_MAX_VEXP_LEN, "((%s)%s)",type==RIM_DEFNUMBER?"rim_num":"rim_dbl", beg);
            *v = var;
            return;
        }
    }
    //
    //
    static rim_num rim_dexp = 0; // counter to create variables to prevent double evaluation problem
    //
    //
    // The name of this variable has its type encoded in the name. We use this in check_var
    // so *do not* change this naming covention (_rim__<code>__<some counter>) without a **very** good reason!!
    //
    //
    if (internal) 
    {
        char *b36;
        snprintf (var, RIM_MAX_VEXP_LEN, "_%s", b36 = rim_num2str (rim_var_internal, NULL, 36));
        rim_free (b36);
    }
    else snprintf (var, RIM_MAX_VEXP_LEN, "_rim__%ld__dexp%ld", type, rim_dexp); 
    char rtype[100];
    bool simple_type = true;
    if (cmp_type(RIM_DEFSTRING, type)) strcpy (rtype, "char*");
    else if (cmp_type(RIM_DEFNUMBER, type)) strcpy (rtype, "rim_num");
    else if (cmp_type(RIM_DEFDOUBLE, type)) strcpy (rtype, "rim_dbl");
    else if (cmp_type(RIM_DEFBOOL, type)) strcpy (rtype, "bool");
    else { simple_type = false; snprintf(rtype, sizeof(rtype), "typeof (%s)", *v); }
    oprintf( "%s %s = %s;\n", rtype,  var, *v);
    char *realname = *v; // MUST be before *v=var below or otherwise realname would be rim_dexp...
    *v = var;
    // add new variable rim_dexp... to the hash of variables only if simple type, because we can't specify constants for other types in code anyway
    if (simple_type)
    {
        add_var (*v, type, realname, false);
        oprintf( "RIM_UNUSED(%s);\n", *v);
    }
    if (internal) 
    {
        rim_var_internal++;
        // limit is 36*36*36, which is 46656, so that's the limit on the number of internal variables used in expression
        // Given they are currently used only for arrays, it would be quite something to exhaust that
        if (rim_var_internal>46650) rim_report_error ("Too many variables in expressions"); 
    } else rim_dexp++;
}


//
// When we're sure v is boolean, we check it and either return or error out
//
char *process_bool_exp (char *v)
{
    if (!strcmp (v, "true") || !strcmp (v, "false")) return v;
    bool found;
    check_bool (v, &found);
    if (!found) rim_report_error ("Expression is not a boolean");
    make_var (&v, RIM_DEFBOOL, false); // create new variable to avoid double evaluation (say if *var is 'rim_strdup(..)'!)
    return v; // new variable instead of expression
}

//
// When we're sure v is string, we check it and either return string var to be used or error out
// type is either RIM_DEFSTRING or RIM_DEFSTRINGSTATIC
//
char *process_string_exp (char *v, rim_num *type)
{
    // check if string expression
    char *res = check_str (v, type);
    return res; 
}

//
// When we're sure v is string, we check it and either return string var to be used or error out
// isd is true if double, 0 if number
//
char *process_number_exp (char *v, bool isd)
{
    bool found;
    check_exp (v, &found, isd, false);
    if (isd) v = promote_double (v);
    make_var (&v, isd?RIM_DEFDOUBLE:RIM_DEFNUMBER, false); // create new variable to avoid double evaluation (say if *var is 'rim_strdup(..)'!)
    return v;
}

//
//
// Check if type is equivalent with check_against. If it is, return type.
// If it's not, error out.
// This is used to make sure that the type of a variable is the correct, or emit an error otherwise.
//
//
rim_num check_type(rim_num type, rim_num check_against)
{
    if (check_against != RIM_DEFUNKN)
    {
        if (!cmp_type(type, check_against)) // if types not equivalent, error out
        {
            rim_report_error (RIM_MSG_BAD_TYPE);
        } else return type; // type is okay (checks out), report back
    } else return type; // if unknown type passed to here, report back what type this is
}

//
// Make sure every object/clause argument is an expression of some type. String constants have been converted to variables at this point.
// However, numbers are still possible here.
// type if RIM_DEFNUMBER, RIM_DEFBOOL, RIM_DEFDOUBLE, RIM_DEFSTRING etc. in which case variable v is checked to be of this type and errors out if not
// returns type of v
// If type if RIM_DEFUNKN, then return type found is returned as found. We always either find the type or error out.
// If type is other than RIM_DEFUNKN, then this variable *must* be of that type (or similar, such as STATIC version),
// and if not, error is thrown.
// check_var must be called after any define_statement, and it must be after any make_mem as it checks variables
// and not constants.
// Note: internal variables MUST start with '_gg' to be taken as correct!
// IMPORTANT: check_var is ONLY for input variables; NEVER for output, which is handled with define statement!!!!! The only exception is use with type of RIM_DEFUNKN to get the type.
//
rim_num check_var (char **var, rim_num type)
{
    
    char *v = *var;
    if (v == NULL) return type; // when a clause is not found (so find_keyword returns NULL) we still check variable
                                // and so if NULL (which it would be if not found), just rubber stamp it

    // trim expression and check if it's missing
    rim_num lv = strlen (v);
    v = rim_trim_ptr(v,  &lv);
    if (v[0] == 0) return type; // in some cases like with opt_clause(), the variable is not present (like case-insensitive in count-substring)
                                // and it's actuall "", and not NULL , so just rubberstamp it
                               

    // if the var has () around it, remove it. There's no need for it, other than to use the same name
    // for a variable or function as is a keyword. If really needed, use (()), so the inner ones remain
    // This is for variable name only, because what of (a)+(b), the result is a)+(b ????; thus only for a string
    // So we check this is just (<var name>) and if so, remove ()
    if (lv >= 3 && v[0] == '(' && v[lv-1] == ')')
    {
        rim_num j = 1;
        bool is_just_name = true;
        while (isspace (v[j])) j++; // clear any space after (
        if (isalpha (v[j]))
        {
            j++;
            for (; j < lv-1; j++) { if (!(isalnum(v[j]) || v[j] == '_')) {is_just_name = false; break; }}
        } else is_just_name = false;
        if (is_just_name)
        {
            // remove trailing )
            v[lv - 1] = 0;
            // We check what's in (..) because it could be (rim_num)strlen(x), which starts with ( and ends with ) but isn't a variable!
            // remove leading ( 
            v++;
            *var = v; // we altered the location of var, make it correct
        }
    }


    if (v[0] == '_' && v[1] == 'r' && v[2] == 'i' && v[3] == 'm')  
    {
        if (!strncmp (v, "_rim__", 6)) // this is internal variable with type encoded in name
                                      // and it's always by itself, i.e. there's no expression here
        {
            char *tcode = v+6;
            char *ecode = strstr (tcode, "__");
            if (ecode != NULL)
            {
                *ecode = 0;
                rim_num found_type = atol (tcode);
                *ecode = '_';
                return check_type (found_type, type); 
            }
        }
        return type; // internal vars are always of the right type (aka _gg...)
                     // but not "rim_..." vars, those are synonyms for user vars
    }

    //
    //
    //
    v = check_input (v); // expand for top-level call
    //
    //
    //

    //
    // we don't do check_type in these because we already do cmp_type() to assure the type returned is compatible with 'type'
    //
    if (cmp_type (type, RIM_DEFSTRING)) // we check this for internal too. We however, detect if the expression is in the 
                                       // form a+b+... where each is a string var, or just a string, or...
    {
        rim_num ftype;
        *var = process_string_exp (v, &ftype);
        return ftype;
    }
    if (cmp_type (type, RIM_DEFNUMBER))
    {
        // check if number expression
        *var = process_number_exp (v, false);
        return RIM_DEFNUMBER;
    }
    if (cmp_type (type, RIM_DEFDOUBLE))
    {
        // check if number expression
        *var = process_number_exp (v, true);
        return RIM_DEFDOUBLE;
    }
    if (cmp_type(type, RIM_DEFBOOL))
    {
        // or true false
        *var = process_bool_exp (v);
        return RIM_DEFBOOL;
    }

    //
    // Here, we now determine the type of expression if the caller doesn't know what kind of expression this is
    //

    // Get passed any left parenthesis
    rim_num i = 0;
    while (v[i] == '(') i++;
    rim_num ibeg = i; // beginning of something to check

    // we have a loop currently just when the expression begins with - or +, and we aren't sure if it's number or double.
    // It can be -(-(-(... until we reach the definitive answer and there are multiple combos that yield an answer after
    // that, hence we'll peel it until we find the type.
    while (1)
    {

        if (v[ibeg] == '"') // check if starts with double quote, and interpret as string expression
        {
            if (is_constant_string(v, NULL) == RIM_CONST_OK) { *var = v; return RIM_DEFSTRING; } // must be string
            else 
            {
                // could be "a"+"b", so a string expression
                rim_num ftype;
                *var = process_string_exp (v, &ftype);
                return check_type (ftype, type); 
            }
        }
        else if (v[ibeg] == '!') // this is partial recognition of boolean expression, just for negation
        {
            *var = process_bool_exp (v+ibeg);
            return check_type (RIM_DEFBOOL, type); 
        }
        else if (v[ibeg] == '.' || (v[ibeg] == '#' && v[ibeg+1] == '#')) // double ONLY
        {
            *var = process_number_exp (v, true);
            return check_type (RIM_DEFDOUBLE, type); 
        }
        else if (v[ibeg] == '\'' || v[ibeg] == '~' || (v[ibeg] == '#' && v[ibeg+1] != '#')) // check if starts with digit, #, + or -, and interpret as number expression, or if it start with numeric char literal (like 'a') - these point to number ONLY
        {
            *var = process_number_exp (v, false);
            return check_type (RIM_DEFNUMBER, type); 
        }
        else if (isdigit(v[ibeg])) // this could be either number or a double
        {
            rim_num j = ibeg+1;
            rim_num tf = check_num_or_double (v, j);
            if (tf == RIM_DEFNUMBER)
            {
                *var = process_number_exp (v, false);
                return check_type (RIM_DEFNUMBER, type); 
            }
            else
            {
                *var = process_number_exp (v, true);
                return check_type (RIM_DEFDOUBLE, type); 
            }
        }
        else if (v[ibeg] == '-' || v[ibeg] == '+' ) // this could be either number or a double expression (not just a number)
        {
            rim_num j = ibeg+1;
            while (isspace (v[j])) j++; // clear any space afterwards
            if (isdigit (v[j])) 
            {
                rim_num tf = check_num_or_double (v, j);
                if (tf == RIM_DEFNUMBER)
                {
                    *var = process_number_exp (v, false);
                    return check_type (RIM_DEFNUMBER, type); 
                }
                else
                {
                    *var = process_number_exp (v, true);
                    return check_type (RIM_DEFDOUBLE, type); 
                }
            }
            else
            {
                ibeg = j;
                continue; // we don't know enough to determine the type, start again until we do;
                          // note: this does NOT parse, so it doesn't matter that ibeg changes, as long as "v" remains unchanged,
                          // which is what's parsed. We're only looking for a first clue as to the type and then we parse v.
            }
        }
        else if (v[ibeg] == '$' || v[ibeg] == '@') // check if starts with $ to convert number to string, or a substring
        {
            rim_num ftype;
            *var = process_string_exp (v, &ftype);
            return check_type (ftype, type); 
        }
        else if (isalpha(v[ibeg]))
        {
            i = ibeg;
            i++; // we already know v[ibeg] is alpha
            while (isalnum(v[i]) || v[i] == '_') i++; // advance to catch the name of variable, stop when anything else found
                                                        // (such as [ or + or ) or whatever

            char char_end = v[i]; // remember byte we temporarily nullify
            v[i] = 0; // put the zero in ending to cap var name
            char *firstn = rim_strdup (v+ibeg); // remember first variable name, so we don't have to save+nullify+restore a byte!
            v[i] = char_end; // restore byte right after name
            while (isspace(v[i])) i++; // advance character after the name to skip spaces
            rim_num var_end = i; // and now var_end is position of whatever comes after name, after spaces skipped; and we have the name of variable
                         // it may be the same as passed_end, or maybe a bit after it if there were spaces passed variable name
            if (rim_is_valid_param_name (firstn, false, false) != 1) 
            {
                rim_report_error ("Unknown variable or expression [%s]", firstn);
            }
            else
            {
                rim_num tp;
                rim_num lnum;
                // Start checking with string, it could be any type really, but string is probably the most common
                if (var_exists (firstn, RIM_DEFSTRING, &tp, &lnum)) 
                {
                    // check if what follows is [, in which case it's a number
                    if (v[var_end] == '[')
                    {
                        *var = process_number_exp (v, false);
                        return check_type (RIM_DEFNUMBER, type);
                    }
                    else
                    {
                        rim_num ftype;
                        *var = process_string_exp (v, &ftype);
                        return check_type (ftype, type); 
                    }
                }
                else if (cmp_type (tp, RIM_DEFBOOL))
                {
                    *var = process_bool_exp (v);
                    return check_type (tp, type); 
                }
                else if (cmp_type (tp, RIM_DEFNUMBER))
                {
                    *var = process_number_exp (v, false);
                    return check_type (tp, type); 
                }
                else if (cmp_type (tp, RIM_DEFDOUBLE))
                {
                    *var = process_number_exp (v, true);
                    return check_type (tp, type); 
                }
                else if (cmp_type (tp, RIM_DEFARRAYNUMBER) && v[var_end] == '[')
                {
                    // this is number array[]
                    *var = process_number_exp (v, false);
                    tp = RIM_DEFNUMBER; // if we're here it's correct array[] expression
                    return check_type (tp, type); 
                }
                else if (cmp_type (tp, RIM_DEFARRAYDOUBLE) && v[var_end] == '[')
                {
                    // this is number array[]
                    *var = process_number_exp (v, true);
                    tp = RIM_DEFDOUBLE; // if we're here it's correct array[] expression
                    return check_type (tp, type); 
                }
                else if (cmp_type (tp, RIM_DEFHASH) && v[var_end] == '[')
                {
                    // this is number array[]
                    rim_num ftype;
                    *var = process_string_exp (v, &ftype);
                    return check_type (ftype, type); 
                }
                else if (cmp_type (tp, RIM_DEFARRAYSTRING) && v[var_end] == '[')
                {
                    // this is number array[]
                    rim_num ftype;
                    *var = process_string_exp (v, &ftype);
                    return check_type (ftype, type); 
                }
                else if (cmp_type (tp, RIM_DEFARRAYBOOL) && v[var_end] == '[')
                {
                    // this is number array[]
                    *var = process_bool_exp (v);
                    tp = RIM_DEFBOOL; // if we're here it's correct array[] expression
                    return check_type (tp, type); 
                }
                else 
                {
                    // since math function names cannot be used as variables, if anything starts with such a name, then it has to be 
                    // an expression of that type
                    if (math_fun (firstn, NULL))
                    {
                        *var = process_number_exp (v, true);
                        return check_type (RIM_DEFDOUBLE, type); 
                    }
                    if (math_fun_n (firstn, NULL)) 
                    {
                        *var = process_number_exp (v, false);
                        return check_type (RIM_DEFNUMBER, type); 
                    }
                    if (str_fun (firstn)) 
                    {
                        *var = process_number_exp (v, false);
                        return check_type (RIM_DEFNUMBER, type); 
                    }
                    if (tp == RIM_DEFUNKN) 
                    {
                        // after looking at variable, not found
                        rim_report_error (RIM_MSG_BAD_TYPE);
                    } 
                    else 
                    {
                        return check_type (tp, type); // just return type found, other types do NOT have expressions (like trees, hashes etc)
                    }
                }
            }
        } else 
        {
             rim_report_error (RIM_MSG_BAD_TYPE);
        }
        rim_report_error (RIM_MSG_BAD_TYPE); // never to be reached, just in case
    }

    return RIM_DEFUNKN; //  will never get here, just for compiler sanity
}



//
// Generate code that loads pre-computed hash to memory. This hash allows near-instantaneous
// request routing in RimStone's dispatcher. The entire hash table is then in a data segment of the program,
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
    char rim_req_file[300];
    snprintf (rim_req_file, sizeof(rim_req_file), "%s/%s", rim_bld_dir, fname);

    FILE *f = fopen (rim_req_file, "r");
    if (f == NULL) rim_report_error( "Error [%s] opening file [%s]", strerror (errno), rim_req_file);

    char line[RIM_MAX_REQ_NAME_LEN]; // line to read from fname
    rim_hash *rim_hardhash; // hash we will build at compile time, and then make a memory representation of it
    bool done_count = false; // true when first line (count of requests) read
                             //
    while (1)
    {
        char *req = fgets_unlocked (line, sizeof (line), f);
        if (req == NULL) // either end or error
        {
            rim_num err = ferror_unlocked (f);
            if (err) // if error, quit
            {
                rim_report_error( "Error [%s] reading file [%s]", strerror (errno), rim_req_file);
            }
            break; // nothing read in, done with hash building
        }
        trimit (req);     
        if (!done_count) 
        {
            done_count = true;
            rim_create_hash (&rim_hardhash, atol (req), NULL, false); // create hash, first line is the
                                                          // number of elements in the hash
        }
        else
        {
            // check if file name is proper, as it's converted into request name
            if (is_req && rim_is_valid_param_name (req, false, false) != 1) rim_report_error (RIM_MSG_SRC_NAME, req);
            // must use rim_strdup() so that each new element is new memory, otherwise
            // new req would override all previous additions
            if (is_req) rim_add_hash (rim_hardhash, rim_strdup(req), RIM_EMPTY_STRING, NULL, NULL);
            else 
            {
                // this is the address of index into param array added as value associated with hash key (which is the name of param), _rim_aprm_xxxx is generated in vmakefile
                char hdata[300];
                snprintf(hdata, sizeof(hdata), "(void*)&_rim_aprm_%s", req);
                rim_add_hash (rim_hardhash, rim_strdup(req), rim_strdup(hdata), NULL, NULL);
            }
        }
    }
    // Now that we have the hash, we need to generate the code that unwinds this process into a 
    // list of static buckets that get loaded in data text of the program.
    // hash is deterministic; the buckets we get here is what gets computed at run time
    // we're just saving time precomputing them at compile time, i.e. here.

    // rim_hardhash is a global hash ptr variable, we declare a pointer to it as hash ops expect a pointer
    oprintf ("rim_hash *%s_ptr = &(%s);\n", hash, hash);
    rim_num i;
    // Calculate how many hash buckets are empty. We still need to have a representation for them, as they
    // are part of the hash image we want to recreate in a running RimStone program.
    rim_num empty = 0;
    for (i = 0; i < rim_hardhash->num_buckets; i++)
    {
        if (rim_hardhash->table[i] == NULL) empty++;
    }

    // harr is the total number of entries we must declare. There's ->tot (total number of elements stored
    // in the hash) plus the empty buckets, plus one for the ending.
    rim_num harr = rim_hardhash->tot+1+empty;

    // Declare internal hash table. Normally, this is allocated at run-time for a hash, but RimStone API allows
    // for it to be provided as a fixed array. We generate the code for it with the initializator (not an assignment)
    // so that it can be linked as a data segment and copied extremely fast into a running program for use.
    oprintf ("rim_hash_table _%s_arr[%ld] = { ", hash,harr);
    // We do not allocate memory. Instead, any overflow bucket elements are stored as the trailing element in the 
    // array, and we manipulate the ->next to point to them. So the hash structure is NOT exactly the same as it would
    // have been with the normal hash. This is faster, since there's no allocation, and memory is in a single contiguous block.
    rim_num bottom = rim_hardhash->num_buckets; 
    rim_num nx;
    // Go through all buckets
    for (i = 0; i < rim_hardhash->num_buckets; i++)
    {
        char *next;
        char next_s[300];
        // Start with each leading bucket element
        rim_hash_table *t = rim_hardhash->table[i];
        rim_num curr_i = i;
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
            // NOTE: the key and data are NOT RimStone memory; thus they can only be searched with strcmp(); rim_mem_get_len() will NOT work on them
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
    oprintf(" };\nRIM_UNUSED(_%s_arr);\n", hash); // finish the list
    // What we created so far is the entire hash bucket structure, including overflow elements
    // What hash API expects is a list of pointers that point to buckets.
    // That's what we initialize here, an array of pointers to buckets, and that's what we use
    // to create a request hash, which then can be queried
    oprintf ("rim_hash_table *_%s_arr_ptr[%ld] = { ", hash,harr-1);
    for (i = 0; i < rim_hardhash->num_buckets; i++) 
    {
        // for those buckets that are NULL, we must have NULL too in runtime hash table, or otherwise
        // the key in such a non-NULL bucket would be NULL, causing RimStone to crash in hash.c(142) when comparing a key
        if (rim_hardhash->table[i] == NULL) oprintf ("%s NULL \n", i!=0?",":""); else oprintf ("%s &(_%s_arr[%ld]) \n", i!=0?",":"", hash, i);
    }
    oprintf ("};\n");
    // generate create hash
    oprintf ("rim_create_hash (&%s_ptr, %ld, _%s_arr_ptr, false);\n", hash, rim_hardhash->tot, hash); 
    

    // hash is typically a fairly small structure. We do not delete it here, as it would just slow down processing.
    fclose (f);
}

//
// Print out a number or write it to a string. 'to' is a string where to write, can be NULL for printing out.
// 'to' is allocated (unless it's NULL, then it's printed out).
// num0 is the number to convert to string. base is
// the base, 2-36, default 10.
// _rim_st is set to be the status of conversion on return
// isd is true if this is double, otherwise number
// width is the width of double (and also a number in the future), prec is the precision, as in %10.3f where 10 is width and 3 is prec
// if width is -1, then it's not used; if prec is -1, then it's not used
// format is NULL if none used, or for double it's 'e', 'f' or 'g'
//
void do_numstr (char *to, char *num0, char *base, bool isd, rim_num width, rim_num prec, char *format)
{
    // to == NULL means print it out. 
    bool go_out = false;
    if (to == NULL)
    {
        go_out = true;
        oprintf ("{\nchar *_rim_numstr;\n");
        to = "_rim_numstr";
    }

    if (!isd) oprintf ("%s=rim_num2str (%s, &_rim_st, %s);\n", to,num0, base!=NULL?base:"10");
    else oprintf ("%s=rim_dbl2str (%s, '%s', %ld, %ld, &_rim_st);\n", to,num0, format, width, prec);
    if (go_out) 
    {
        //_rim_st (i.e. the length of string) is never <0
        oprintf ("rim_puts (RIM_NOENC, %s, _rim_st, true);\n", to);
        oprintf ("rim_free_int (%s);\n", to); // for printing, release memory right away

        oprintf ("}\n");
    }
}

//
// Convert rim_puts (...) with only a string into rim_puts(..., length) for better output performance
// Basically, all literals are strlen-ed at compile time so there's no strlen() at run-time
// pline is the line to convert. This is done by replacing RIM_EMPTY_LONG_PLAIN_ZERO with the actual 
// number (plus some spaces), so there is never a question if there's enough memory (there is since this constant
// is bigger than any number). Also, this constant is used here ONLY with "..." (string constants), so we know just before
// this constant is a comma, and before that a string. This makes for easy replacement with constant's length.
// The nice thing here is even if we miss some, it still works, though a bit slower.
//
void convert_puts(char *pline)
{
    char *zero;
    static int elen = strlen ("RIM_EMPTY_LONG_PLAIN_ZERO");

    char *search = pline;
    while (1) 
    {
        // look for next rim_puts placeholder for length
        char *ozero = strstr (search, "RIM_EMPTY_LONG_PLAIN_ZERO");
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
        rim_num totstr = 0;
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
        // leading quote, we find rim_puts (XXX, 
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
char *opt_clause(char *clause, char *param, char *antiparam, rim_num type)
{
    char *res = NULL;
    if (clause != NULL)
    {
        if (clause[0] != 0) // there is true|false
        {
            res = rim_malloc(strlen(clause)+strlen(param)+strlen(antiparam) + 30);
            check_var (&clause, RIM_DEFBOOL); // make sure it's a boolean variable
            sprintf (res, "((%s)?(%s):(%s))", clause, param, antiparam);
            make_var (&res, type, false);
        }
        else // clause is present but no true|false
        {
            res = rim_strdup(param);
        }
    }
    else // clause is not present (NULL)
    {
        res = rim_strdup(antiparam);
    }
    return res;
}

//
// This processes query-result statement, see documentation
//
void query_result (rim_gen_ctx *gen_ctx, char *mtext, rim_num cur_query_result)
{
    rim_db_parse vp;
    memset  (&vp, 0, sizeof(rim_db_parse)); // all are NULL now

    vp.query_result = rim_strdup (mtext);
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
void rim_is_valid_app_path (char *name)
{

    rim_num i = 0;
    while (name[i] != 0)
    {
        if (!isalnum(name[i]) && name[i] != '_' && name[i] != '-' && name[i] != '/') rim_report_error ("Application path is not valid; it can contain only alphanumeric characters, underscores, hyphens and forward slashes");
        i++;
    }
    rim_num l = (rim_num)strlen(rim_app_name);
    // app patch must end with /<app name>
    if (i < l+1 || strcmp (name+i-l, rim_app_name) || name[i-l-1] != '/') rim_report_error ("Application path [%s] must end with /%s", name, rim_app_name);
}


//
// Reads stdin, and substs $<curlybrace>XYZ<curlybrace> to the stdout. Max var len 250. Non-existent is empty. 
// Anything bad with $<curlybrace>XYZ<curlybrace> just
// copied to the stdout. XYZ can be alphanumeric or underscore.
// This does NOT and MUST NOT use rim_malloc. If it does, the calling of envsub in main() must be after the mem init.
//
void envsub () 
{
    char varname[256];
    rim_num varlen = 0;
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
                    if (c == EOF || !(isalnum(c) || c == '_') || varlen >= (rim_num)sizeof(varname)-2)
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
// on_error_cont/exit is carved out on-error-cont/exit value, act is either RIM_ON_ERROR_CONTINUE/EXIT or RIM_OKAY
//
void on_error_act (char *on_error_cont, char *on_error_exit, char *act)
{
    if (on_error_cont != NULL) *act = RIM_ON_ERROR_CONTINUE;
    else if (on_error_exit != NULL) *act = RIM_ON_ERROR_EXIT;
    else *act = RIM_OKAY;

    // check for sanity
    if (on_error_cont != NULL && on_error_exit != NULL) rim_report_error ("on-error can be either 'continue' or 'exit', but not both");
}

//
// Generate code to create and init default rim_header. 'inittype' is RIM_HEADER_FILE for file serving, i.e.
// when there's caching; RIM_HEADER_PAGE for page serving, i.e when there's no caching.
// http_header_count is the ID of the temp rim_header-type.
// Returns the name of the rim_header variable for which the code is generated.
// request is 0 if this is reply header, or 1 if request. Some http headers are just for reply and vice versa.
//
char *make_default_header(int inittype, rim_num http_header_count, char request)
{
    static char temp_header[200];
    if (inittype != RIM_HEADER_FILE && inittype != RIM_HEADER_PAGE) rim_report_error( "unsupported header type [%d] (internal)", inittype);
    snprintf (temp_header, sizeof(temp_header), "_rim_tmp_header_%ld", http_header_count); 
    oprintf ("rim_header %s;\n", temp_header);
    // by default it is to show the file (disposition is empty)
    // use disposition as "attachment" or "download" for download
    oprintf ("rim_init_header (&(%s), %d, %d) ;\n", temp_header, inittype, request);
    return temp_header;
}

//
// Generates code that constructs rim_header variable. User can specify content-type, download, etag and others.
// Also allows for "custom" header in the form of expr=expr,... making it easy to create custom headers.
// "statement" is the name of the statement (such as "send-file"). "header" is the actual text of headers clause in the
// statement. temp_header is the name of the rim_header var generated via make_default_header().
// http_header_count is the ID of the temp rim_header-type, the same as used in make_default_header().
// request is 0 if this is reply header, or 1 if request. Some http headers are just for reply and vice versa.
// content_type is content-type if set, NULL otherwise,
// content_len is content-length if set, NULL otherwise (originally rim_header was to be used to set the values, but
// there is an issue of types: here all types are char*, while at run-time, they can be different); can be NULL.
//
void process_http_header (char *statement, char *header, char *temp_header, rim_num http_header_count, char request, char **content_len, char **content_type)
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
        char *mheader = rim_strdup (header);
        if (request == 0)
        {
            download = find_keyword (mheader, RIM_KEYDOWNLOAD, 1);
            etag = find_keyword (mheader, RIM_KEYETAG, 1);
            filename = find_keyword (mheader, RIM_KEYFILENAME, 1);
            cachecontrol = find_keyword (mheader, RIM_KEYCACHECONTROL, 1);
            nocache = find_keyword (mheader, RIM_KEYNOCACHE, 1);
            statusid = find_keyword (mheader, RIM_KEYSTATUSID, 1);
            statustext = find_keyword (mheader, RIM_KEYSTATUSTEXT, 1);
        }
        if (request == 1) 
        {
            clen = find_keyword (mheader, RIM_KEYCONTENTLENGTH, 1);
        }
        ctype = find_keyword (mheader, RIM_KEYCONTENTTYPE, 1);
        custom = find_keyword (mheader, RIM_KEYCUSTOM, 1);
        if (request == 0)
        {
            carve_statement (&download, statement, RIM_KEYDOWNLOAD, 0, 2);
            carve_statement (&etag, statement, RIM_KEYETAG, 0, 2);
            carve_statement (&filename, statement, RIM_KEYFILENAME, 0, 1);
            carve_statement (&cachecontrol, statement, RIM_KEYCACHECONTROL, 0, 1);
            carve_statement (&nocache, statement, RIM_KEYNOCACHE, 0, 0);
            carve_statement (&statusid, statement, RIM_KEYSTATUSID, 0, 1);
            carve_statement (&statustext, statement, RIM_KEYSTATUSTEXT, 0, 1);
        }
        if (request == 1) 
        {
            carve_statement (&clen, statement, RIM_KEYCONTENTLENGTH, 0, 1);
        }
        carve_statement (&ctype, statement, RIM_KEYCONTENTTYPE, 0, 1);
        carve_statement (&custom, statement, RIM_KEYCUSTOM, 0, 1);

        // check_var MUST be after ALL carve_statements
        if (request == 1) 
        {
            check_var (&clen, RIM_DEFNUMBER);
        }

        make_mem (&ctype);
        char *downloadc = opt_clause(download, "\"attachment\"", "NULL", RIM_DEFSTRING);
        char *etagc = opt_clause(etag, "1", "0", RIM_DEFNUMBER);
        if (request == 0)
        {
            make_mem (&filename);
            make_mem (&cachecontrol);
            make_mem (&statustext);
            //
            check_var (&filename, RIM_DEFSTRING);
            check_var (&cachecontrol, RIM_DEFSTRING);
            check_var (&statusid, RIM_DEFNUMBER);
            check_var (&statustext, RIM_DEFSTRING);
            check_var (&etagc, RIM_DEFNUMBER);
            check_var (&downloadc, RIM_DEFSTRING);
        }


        if (nocache != NULL && cachecontrol != NULL) rim_report_error( "both no-cache and cache-control specified, only one can be used");
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
        rim_free(downloadc);
        rim_free(etagc);
        // get custom fields
        if (custom != NULL)
        {
            char *mcustom = rim_strdup (custom);
            // store custom headers
            char chead_var[100];
            snprintf (chead_var, sizeof (chead_var), "_rim_chead_arr%ld", http_header_count);
            rim_num tothead = outargs(mcustom, chead_var, "char *", 0, 1, RIM_DEFSTRING, RIM_DEFSTRING);
            rim_num i;
            rim_num j = 0;
            // This pointer copying, first to _rim_chead_arr, then to control/value will
            // be optimized away by gcc; no effect on performance
            for (i = 0; i < tothead; i+=2)
            {
                oprintf ("(%s).control[%ld]=_rim_chead_arr%ld[%ld];\n", temp_header, j, http_header_count, i);
                oprintf ("(%s).value[%ld]=_rim_chead_arr%ld[%ld+1];\n", temp_header, j, http_header_count, i);
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
rim_num outargs(char *args, char *outname, char *type, rim_num startwith, char pair, rim_num check_type_val, rim_num check_type_name)
{


    rim_num exec_inputs=startwith; // we start with index 1 in order to fill in program name as arg[0] at run-time
                        // args[0] is filled with program name at run-time

    rim_num tote;
    if (args != NULL)
    {
        rim_num tot;
        // must duplicate string because we CHANGE curr_start in parse_param_list (because it removes
        // escaped quotes) and thus we change the 'line' (the current parsing line). As a result, we will
        // probably error out at the end of line because we will see null characters since the line has
        // shifted to the left during removal of escaped quotes).
        char *curr_start = rim_strdup(args);

        // parse program arguments
        rim_fifo *params = NULL;
        parse_param_list (curr_start, &params, &tot);
        // define run-time list of program arguments, account for the last NULL
        // also account double when there's x=y pair, since that'll take 2 entries
        // add startwith if not zero to account for pre-filled elements
        oprintf("%s%s[%ld];\n", type,outname, tote=((pair == 1 ? 2:1)*tot+1+startwith));

        char *value;
        while (1)
        {
            rim_num st = rim_retrieve (params, NULL, (void*)&value);
            if (st != RIM_OKAY) break;

            if (pair == 1)
            {
                // find equal sign outside the quotes or parenthesis
                char *eq = find_keyword(value, "=", 0);
                if (eq == NULL)
                {
                    rim_report_error( "Name/value pair missing equal sign ('=') in [%s]", value);
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
    if (exec_inputs >= tote) rim_report_error( "Internal error in building a list [%ld, %ld]", exec_inputs, tote);

    return exec_inputs;
}

//
// Check if (maybe) command c looks like rimstone, and error out if it does
// (because it's likely misspelled rimstone command)
// Check for 'ab..-cd..<space|eol>' or 'ab..-cd..#'
//
void check_rim (char *c)
{
    // first find the space or #, or end-of-line
    rim_num i = 0;
    while (!isspace(c[i]) && c[i] != 0) i++;
    rim_num j;
    rim_num isrim = 1;
    rim_num hasdash = 0;
    for (j = 0; j < i; j++)
    {
        if (c[j] == '-') hasdash = 1;
        if (!isalpha(c[j]) && c[j] != '-') {isrim = 0; break;}
    }
    if (hasdash == 1 && isrim == 1) 
    {
        rim_report_error ("Unrecognized RimStone statement [%s]", c);
    }
}

//
// Trim string var in-place. Returns its length.
//
rim_num trimit(char *var)
{
    rim_num var_len = strlen (var);
    rim_trim (var, &var_len, false);
    return var_len;
}

//
// Return name of database based on its index in the list of used dbs (dbconn)
// Return empty if not found or not recognized
//
char *rim_db_vendor(rim_num dbconn)
{
    if (dbconn < 0 || dbconn >= totconn ) return "";
    if (rim_get_config()->ctx.db->conn[dbconn].db_type == RIM_DB_SQLITE) return "sqlite";
    else if (rim_get_config()->ctx.db->conn[dbconn].db_type == RIM_DB_MARIADB) return "mariadb";
    else if (rim_get_config()->ctx.db->conn[dbconn].db_type == RIM_DB_POSTGRES) return "postgres";
    return "";
}


//
// Create and return C variable name from file name 'file'
//
char *id_from_file (char *file)
{
    char *id = rim_strdup(file);
    rim_num i;
    if (!isalpha(id[0])) id[0] = '_';
    for (i = 1; i < (rim_num)strlen(id); i++)
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
void process_query (rim_gen_ctx *gen_ctx, const rim_num query_result, const rim_num run_query, char is_prep, rim_db_parse *vp)
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
            rim_report_error( "Query usage attempted, but no query present and active");
        }
        query_execution (gen_ctx, run_query, query_result, is_prep, vp);
        return;
    }
}


//
// Name a query. A query is numbered (as in 0,1,2..)
// gen_ctx is a query context, vp is the context of a parsed run-query statement
//
void name_query (rim_gen_ctx *gen_ctx)
{
    snprintf (gen_ctx->qry[query_id].name, sizeof (gen_ctx->qry[query_id].name), "_rim_qryname_%ld", gen_ctx->total_queries);
}

//
// Examine proposed query statement, perform checks to its validity, and generate initial defining code for it.
// gen_ctx is execution context, vp has all clauses parsed out
//
void prepare_query (rim_gen_ctx *gen_ctx, rim_db_parse *vp)
{

    // new_query either adds a query or uses existing one
    new_query (gen_ctx, vp);

    // get query output columns (or unknown)
    get_query_output (gen_ctx, vp); 

    //
    // Generate query variables etc
    //
    // generate variables used by query
    oprintf("rim_num _rim_qry_on_error_%s = %d;\n", gen_ctx->qry[query_id].name, vp->on_error_action);
    oprintf("RIM_UNUSED (_rim_qry_on_error_%s);\n", gen_ctx->qry[query_id].name);
    oprintf("rim_num _rim_qry_executed_%s = 0;\n", gen_ctx->qry[query_id].name);
    oprintf("RIM_UNUSED (_rim_qry_executed_%s);\n", gen_ctx->qry[query_id].name);
    oprintf("char *%s = NULL;\n", gen_ctx->qry[query_id].name); 
    //
    // Generate the C code to allocate a query. gen_ctx is the context, and query_id is the query id.
    // Depending on what kind of code we generate later, some of these may not be used, and we mark them
    // as unused to avoid compiler warning.
    //
    // affected rows, and then number of rows and columns retrieved
    // set to zero in case run-query doesn't run so that they don't use random values
    oprintf("rim_num _rim_arow_%s = 0;\n", gen_ctx->qry[query_id].name);
    oprintf("rim_num _rim_nrow_%s = 0;\n", gen_ctx->qry[query_id].name);
    oprintf("rim_num _rim_ncol_%s = 0;\n", gen_ctx->qry[query_id].name);
    oprintf("RIM_UNUSED(_rim_arow_%s);\n", gen_ctx->qry[query_id].name);
    oprintf("RIM_UNUSED(_rim_nrow_%s);\n", gen_ctx->qry[query_id].name);
    oprintf("RIM_UNUSED(_rim_ncol_%s);\n", gen_ctx->qry[query_id].name);
    // when we do 'run-query' with empty text followed by start-query, we don't
    // know if query is DML or not, so we have to specify all variables
    oprintf("char * _rim_errm_%s = RIM_EMPTY_STRING;\n", gen_ctx->qry[query_id].name);
    oprintf("RIM_UNUSED (_rim_errm_%s);\n", gen_ctx->qry[query_id].name);
    oprintf("char * _rim_err_%s = RIM_EMPTY_STRING;\n", gen_ctx->qry[query_id].name);
    oprintf("RIM_UNUSED (_rim_err_%s);\n", gen_ctx->qry[query_id].name);
    oprintf("char **_rim_data_%s = NULL;\n", gen_ctx->qry[query_id].name);
    oprintf("RIM_UNUSED (_rim_data_%s);\n", gen_ctx->qry[query_id].name);
    oprintf("char **_rim_col_names_%s = NULL;\n", gen_ctx->qry[query_id].name);
    oprintf("RIM_UNUSED (_rim_col_names_%s);\n", gen_ctx->qry[query_id].name);
    // allocate SQL buffer
    oprintf("static char *_rim_sql_buf_%s;\n", gen_ctx->qry[query_id].name);
    oprintf("char **_rim_sql_params_%s = NULL;\n", gen_ctx->qry[query_id].name);
    oprintf("rim_num _rim_sql_paramcount_%s = 0;\n", gen_ctx->qry[query_id].name);
    // _rim_is_input_used specifies if there's an input parameter. 
    // There may be some zeroes in it, but normally it is a block of 1's corresponding to input parameters
    // see comments on rim_make_SQL() function for more on this.
    oprintf ("rim_num _rim_is_input_used_%s[%d];\n", gen_ctx->qry[query_id].name, RIM_MAX_QUERY_INPUTS + 1);
    oprintf ("memset (_rim_is_input_used_%s, 0, sizeof (rim_num)*%d);\n", gen_ctx->qry[query_id].name, RIM_MAX_QUERY_INPUTS + 1);
    //_dlen is length of data fields
    oprintf ("rim_num *_rim_dlen_%s = NULL;\n", gen_ctx->qry[query_id].name);
    oprintf("RIM_UNUSED (_rim_dlen_%s);\n", gen_ctx->qry[query_id].name);
    // _iter is the iterator through output data loop (incrementing by 1 for each row)
    oprintf("rim_num _rim_iter_%s;\n", gen_ctx->qry[query_id].name);
    oprintf("RIM_UNUSED (_rim_iter_%s);\n", gen_ctx->qry[query_id].name);
    // set if prepared statement, if so, make the unique variable used for it
    // unique void * for prepared stmt, important to be NULL. It is only non-NULL when it has been prepared. 
    // It cannot be deallocated (when connection goes down) if we cannot distinguish whether it has been prepared or not
    oprintf("static void *_rim_sql_prep_%s = NULL;\nRIM_UNUSED(_rim_sql_prep_%s);\n", gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].name);
}


//
// Free resources used by query
// gen_ctx is execution context, qryname is the name of the query
// skip_data is true if not to free data (but rather free everything else)
//
void free_query (char *qryname, bool skip_data)
{
    char qname[RIM_MAX_QUERYNAME_LEN+1];
    snprintf (qname, sizeof (qname), "_rim_qryname_%s", qryname);
    // qname itself is just sql text
    // the error msg/cde are either RIM_EMPTY_STRING or allocated, so they can be freed
    oprintf("rim_free(_rim_errm_%s);\n", qname);
    oprintf("rim_free(_rim_err_%s);\n", qname);
    // free data if there, also free each individual chunk
    oprintf("if (_rim_data_%s != NULL) { if (!%s) {rim_num i; for (i=0;i<_rim_ncol_%s*_rim_nrow_%s;i++) rim_free(_rim_data_%s[i]);} rim_free(_rim_data_%s);}\n", qname, skip_data?"true":"false", qname, qname, qname, qname);
    // free names if there, also free each individual chunk
    oprintf("if (_rim_col_names_%s != NULL) {rim_num i; for (i=0;i<_rim_ncol_%s;i++) rim_free(_rim_col_names_%s[i]); rim_free (_rim_col_names_%s);}\n", qname, qname, qname, qname);
    // free length of data if there
    oprintf ("if (_rim_dlen_%s != NULL) rim_free (_rim_dlen_%s);\n", qname, qname);
    // _rim_sql_buf_... is freed if it was allocated, it is a dynamic buffer for SQL text to be executed, search on it
    // _rim_sql_params_... is freed if it was allocated, it's dynamic params when statement is prepared
    // _rim_sql_prep is allocated once with strdup() and never freed unless re-prepared, just a unique pointer, search on it
}


//
// Do most of query code generation and processing of query result. Do all of query stack processing.
// query_result is 1 if this is query-result. run_query is 1 if this is run-query 
//
void query_execution (rim_gen_ctx *gen_ctx, const rim_num run_query, const rim_num query_result, char is_prep, rim_db_parse *vp)
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
            oprintf("%s = _rim_err_%s;\n", vp->err, gen_ctx->qry[query_id].name);
        } 
        if (vp->errtext != NULL)
        {
            oprintf("%s = _rim_errm_%s;\n", vp->errtext, gen_ctx->qry[query_id].name);
        }
        if (vp->arows != NULL)
        {
            oprintf("%s = _rim_arow_%s;\n", vp->arows, gen_ctx->qry[query_id].name);
        }
        if (vp->rcount != NULL)
        {
            RIM_CHECK_TUPLE(query_id);
            oprintf("%s = _rim_nrow_%s;\n", vp->rcount, gen_ctx->qry[query_id].name);
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
       rim_report_error ("Syntax error in query");
    }
}

//
//  Processing of query-result.  vp->query_result is the name of column being asked to provide value for.
//
void generate_query_result (rim_gen_ctx *gen_ctx, rim_db_parse *vp)
{
    char *col_out = vp->query_result;

    // get column encoding when retrieving query column
    rim_num no_encode = 0;
    rim_num url_encode = 0;
    rim_num web_encode = 0;
    char *noenc = find_keyword (col_out, RIM_KEYNOENCODE, 1);
    if (noenc != NULL) no_encode = 1;
    char *webenc = find_keyword (col_out, RIM_KEYWEBENCODE, 1);
    if (webenc != NULL) web_encode = 1;
    char *urlenc = find_keyword (col_out, RIM_KEYURLENCODE, 1);
    if (urlenc != NULL) url_encode = 1;
        
    carve_statement (&noenc, "query-result", RIM_KEYNOENCODE, 0, 0);
    carve_statement (&webenc, "query-result", RIM_KEYWEBENCODE, 0, 0);
    carve_statement (&urlenc, "query-result", RIM_KEYURLENCODE, 0, 0);

    rim_num len_name = carve_stmt_obj (&col_out, true, false);
    if (len_name > RIM_MAX_COLNAME_LEN) rim_report_error ("Query name in [%s] is too long", col_out);

    define_statement (&col_out, RIM_DEFSTRING, false); 
    //
    // no check_var needed above as col_out (name of column) is defined, and the other have no clause values

    // we can change the mtext all we want, but we can't memmove, which is fine, that's why we use col_out here
    if (urlenc != NULL) *urlenc = 0;
    if (webenc != NULL) *webenc = 0;
    if (noenc != NULL) *noenc = 0;

    if (no_encode+web_encode+url_encode > 1)
    {
        rim_report_error ("Query output can be either noencode, webencode (default) or urlencode, but not any combination of these");
    }
    if (no_encode+web_encode+url_encode == 0) web_encode = 1; // default is web-encode


    //
    // to 'var' is always used, run-query ALWAYS stores data into defined variables
    //

    // Check if query ID is active. If it is used, but not active, it means
    // we're using a query column outside the action block for that query
    if (gen_ctx->qry_active[query_id] != RIM_QRY_ACTIVE)
    {
        rim_report_error( "Query [%s] is not active", gen_ctx->qry[query_id].name);
    }

    //
    //
    // column_id is the id of the query column
    //
    //
    rim_num column_id =  vp->cur_query_result;

    // we now have numerical positional ID for a column in the result set
    // which is column_id

    // we cannot have error, affected_rows here, because this is result set and it may be 0 rows,
    // so without rows, query-result is never reached, making it impossible to get the aforementioned fields.

    // if we added too many output columns, but query has not enough of them, the data will be undefined; prevent that
    oprintf ("if (_rim_ncol_%s <= %ld) rim_report_error (\"Column #%ld does not exist in the database result set\");\n", gen_ctx->qry[query_id].name,column_id, column_id);

    
    if (no_encode == 1) 
    {
        oprintf("%s = (_rim_data_%s[_rim_iter_%s*_rim_ncol_%s+%ld]);\n", col_out, gen_ctx->qry[query_id].name,gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].name, column_id);
    }
    else 
    {
        oprintf ("rim_encode (%s, _rim_data_%s[_rim_iter_%s*_rim_ncol_%s+%ld], -1, &(%s), true);\n", web_encode==1?"RIM_WEB":"RIM_URL", gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].name, column_id , col_out);
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
void generate_sql_code (rim_gen_ctx *gen_ctx, char is_prep)
{

    oprintf("%s = %s;\n", gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].text);

    oprintf("char *fname_loc_%s = \"%s\";\n",gen_ctx->qry[query_id].name, src_file_name); // to overcome constness
    oprintf("rim_num lnum_%s = %ld;\n",gen_ctx->qry[query_id].name,lnum); 
    oprintf("rim_location (&fname_loc_%s, &lnum_%s, 1);\n",gen_ctx->qry[query_id].name,gen_ctx->qry[query_id].name);


    statement_SQL (gen_ctx, is_prep);

    if (gen_ctx->qry[query_id].returns_tuple == 1)
    {
        // generate select call for any SQL that returns tuple
        oprintf("rim_select_table (_rim_sql_buf_%s, &_rim_arow_%s, &_rim_nrow_%s, &_rim_ncol_%s, %s%s, &_rim_data_%s, &_rim_dlen_%s, (char**)&_rim_err_%s, (char**)&_rim_errm_%s, %d, &_rim_sql_prep_%s, _rim_sql_paramcount_%s, _rim_sql_params_%s, _rim_qry_on_error_%s);\n",
        gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].name,gen_ctx->qry[query_id].name, 
        // get query columns names from db only if unknown output is used, otherwise not
        gen_ctx->qry[query_id].unknown_output == 1 ? "&_rim_col_names_": "NULL", 
        gen_ctx->qry[query_id].unknown_output == 1 ? gen_ctx->qry[query_id].name:"",
        gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].name, is_prep, gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].name);
        oprintf("rim_db_free_result (%d);\n", (int)is_prep);


    }
    else
    {
        // this is no-tuple call

        oprintf("rim_execute_SQL (_rim_sql_buf_%s, &_rim_arow_%s, (char**)&_rim_err_%s, (char**)&_rim_errm_%s, %ld, 1, %d, &_rim_sql_prep_%s, _rim_sql_paramcount_%s, _rim_sql_params_%s, _rim_qry_on_error_%s);\n",
            gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].returns_tuple, (int)is_prep, gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].name);
        // set nrow to 0
        oprintf("_rim_nrow_%s=0;\n", gen_ctx->qry[query_id].name);
        oprintf("rim_db_free_result (%d);\n", (int)is_prep);
    }

    // deallocate _rim_sql_buf_<query name> if not prepared and no input params (which is the condition for it to be allocated)
    if (gen_ctx->qry[query_id].qry_found_total_inputs > 0 && is_prep == 0) oprintf("rim_free_int (_rim_sql_buf_%s);\n", gen_ctx->qry[query_id].name);
    // deallocate SQL params used for prepared query
    oprintf("if (_rim_sql_params_%s != NULL) rim_free_int (_rim_sql_params_%s);\n", gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].name);
}

//
// open the loop query in generated code. query_id is the index number of query.
//
void start_loop_query (rim_gen_ctx *gen_ctx)
{

    oprintf("for (_rim_iter_%s = 0; _rim_iter_%s < _rim_nrow_%s; _rim_iter_%s++)\n",gen_ctx->qry[query_id].name, 
        gen_ctx->qry[query_id].name,gen_ctx->qry[query_id].name,gen_ctx->qry[query_id].name);
    oprintf("{\n");
    
    // if outputs defined with "col1, col2..." then define them and fill values
    rim_num i;
    for (i = 0; i < gen_ctx->qry[query_id].qry_total_outputs; i++) query_result (gen_ctx, gen_ctx->qry[query_id].qry_outputs[i], i);


    // done with loop query
}



//
// Statement SQL to be executed for run-query etc. This is after the analysis of the statement,
// so we have filled out structures based on query_id. The statement SQL is output to generated source file
//
void statement_SQL (rim_gen_ctx *gen_ctx, char is_prep)
{
    if (is_prep == 1)
    {
        // 
        // statement is evaluated only the first time for prepared queries, so we save a copy
        // of the query in case we need to reconnect. TODO: if query is a string literal, we can
        // avoid strdup below.
        //
        oprintf("static char _rim_sql_buf_set_%s=0;\n", gen_ctx->qry[query_id].name);
        //
        // _rim_sql_buf_<query name> is a copy of query text. It should stay and not be freed for the life of the process,
        // because if the db server goes down, it will be needed to prepare statement again.
        // So no rim_ alloc functions, as they are deallocated at the end of the request
        // Also set void** as prepared statement pointer, so it can be cleared when connection is lost (rim_db_prep() call)
        oprintf("if (_rim_sql_buf_set_%s==0) {if ((_rim_sql_buf_%s=strdup(%s))==NULL) rim_report_error(\"Cannot allocate query [%s] for a prepared statement\");_rim_sql_buf_set_%s=1; rim_db_prep(&_rim_sql_prep_%s); }\n", gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].name);
        oprintf("if (_rim_qry_executed_%s == 1) {rim_report_error(\"Query [%s] has executed the second time; if you want to execute the same query twice in a row without a run-query, use different queries with the same query text (or variable) if that is your intention, file [%s], line number [%ld] \");}\n", gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].name, src_file_name, lnum);
    }

    // with dynamic queries, we cannot count how many '%s' in SQL text (i.e. inputs) there are. 
    // For dynamic, the number of inputs is known  only by
    // the number of actual input parameters in start-query (this is qry_found_total_inputs). Because
    // SQL statement (_rim_sql_buf...) is only known at run-time, we need to pass this number of input params to it
    // and verify we are not using bad memory or missing arguments.
    if (gen_ctx->qry[query_id].qry_found_total_inputs == 0)
    {
        //
        // If there are no parameters, this would look like:
        // rim_make_SQL (_rim_sql_buf_something, 32000, 0, something);
        // and that would cause gcc error 'format not a string literal and no format arguments [-Werror=format_security]
        // because gcc doesn't know that generated code will NOT make a mistake of including %s in 'something'
        // So make it a simple copy, as it's all that's needed.
        // is_prep can be 0 or 1 here, if 1, this is prepared statement without any input params
        //
        // for non-prepared statements without any input params, just use the query text
        if (is_prep == 0) oprintf("_rim_sql_buf_%s=%s;\n", gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].name);
    }
    else
    {
        if (is_prep == 0)
        {
            // _rim_sql_buf_%s may be NULL here, in general it is undefined. And it will be allocated in rim_make_SQL at run time
            // and it will be deallocated once the query executes.
            oprintf("rim_make_SQL (&_rim_sql_buf_%s, %ld, %s ",
                gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].qry_found_total_inputs,  gen_ctx->qry[query_id].name); 
            rim_num z;
            for (z = 0; z < gen_ctx->qry[query_id].qry_found_total_inputs; z++)
            {
                oprintf(", _rim_is_input_used_%s[%ld]==1 ?  (%s) : NULL ", gen_ctx->qry[query_id].name, z, gen_ctx->qry[query_id].qry_inputs[z]);
            }
            oprintf(");\n");
        }
        else
        {
            // 
            // set params count and params themselves for prepared statement
            //
            oprintf("_rim_sql_paramcount_%s = %ld;\n", gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].qry_found_total_inputs);
            oprintf("_rim_sql_params_%s = (char**)rim_calloc (%ld, sizeof(char*));\n", gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].qry_found_total_inputs);
            rim_num z;
            for (z = 0; z < gen_ctx->qry[query_id].qry_found_total_inputs; z++)
            {
                oprintf("_rim_sql_params_%s[%ld]=(_rim_is_input_used_%s[%ld]==1 ?  (%s) : NULL);\n", gen_ctx->qry[query_id].name, z, gen_ctx->qry[query_id].name, z, gen_ctx->qry[query_id].qry_inputs[z]);
            }
        }
    }

    // We execute the actual db query right at the beginning of the action block
    // We use 'query ID' decorated variables, so all our results are separate
    // file/line must be here because this is generated code and file name/line number are NOT available then unless 
    // we plug it in now
    oprintf("if (_rim_qry_executed_%s == 1) {rim_report_error(\"Query [%s] has executed the second time; if you want to execute the same query twice in a row without a run-query, use different queries with the same query text (or variable) if that is your intention, file [%s], line number [%ld] \");}\n", gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].name, src_file_name, lnum);
    oprintf("_rim_qry_executed_%s = 1;\n", gen_ctx->qry[query_id].name);
}


// 
// Process text of dynamic query. gen_ctx is execution context, vp is parsed statement clauses.
// Get output query, the columns. 
//
void get_query_output (rim_gen_ctx *gen_ctx,  rim_db_parse *vp)
{
    char *with_output = vp->with_output;

    if (with_output != NULL)
    {
        while (isspace(*with_output)) with_output++;
        // get all the column names
        while (1)
        {
            rim_num end_of_list = 0;
            char *collist = with_output;
            // now finding column list after output
            char *colend = collist;
            get_until_comma (&colend);
            if (*colend == 0) end_of_list = 1; // no more in comma-separated list, last item
            *colend = 0;
            rim_num colLen = strlen (collist);
            rim_trim (collist, &colLen, false);
            // these column names are checked to be valid names in start_loop_query
            gen_ctx->qry[query_id].qry_outputs[gen_ctx->qry[query_id].qry_total_outputs] = rim_strdup (collist);
            gen_ctx->qry[query_id].qry_total_outputs++;
            if (gen_ctx->qry[query_id].qry_total_outputs >= RIM_MAX_QUERY_OUTPUTS)
            {
                rim_report_error( "Too many query outputs [%ld]", gen_ctx->qry[query_id].qry_total_outputs);
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
        if (totconn == 1) dbname = rim_get_config()->ctx.db->conn[0].db_name;
        else if (totconn == 0) {rim_report_error ("No database is configured. Configure a database before attempting to use it");}
        else {rim_report_error ("There is more than one database configured. Use '@' to specify the database used here");}
    }

    rim_num l = strlen (dbname);
    rim_trim (dbname, &l, false);

    // dbname is now pure db conn name
    if (rim_is_valid_param_name(dbname, false, false) != 1)
    {
        rim_report_error( "Database name must have only alphanumeric characters and underscore");
    }

    // generate database switch to this database
    // no need for runtime to have dbtype info, each library already knows what it is
    rim_num dbconn;
    oprintf ("rim_get_config()->ctx.db->ind_current_db=%ld;\n", dbconn=find_connection (dbname));
    if (rim_get_config()->ctx.db->conn[dbconn].db_type == RIM_DB_SQLITE) rim_mod_sqlite = true;
    else if (rim_get_config()->ctx.db->conn[dbconn].db_type == RIM_DB_MARIADB) rim_mod_mariadb = true;
    else if (rim_get_config()->ctx.db->conn[dbconn].db_type == RIM_DB_POSTGRES) rim_mod_postgres = true;

}

//
// Build a list of connections, and return a fixed index that identifies the
// database connection we're adding. 'conn' is the connection name. It is assume no duplicates arrive here - this is
// fed from .dbvendors file generated by rimstone utility.
// dbtype is db vendor (such as RIM_DB_MARIADB)
// There is no limit on number of db connections
//
rim_num add_connection(char *conn, rim_num dbtype)
{
    rim_num i;
    static rim_num conn_num = 0;
    if (rim_get_config()->ctx.db->conn == NULL)
    {
        conn_num = 1; // start with 1 db connection since that's what most programs will do.
        // calloc initializes to zero here - we rely on that to make sure members like g_con are NULL, etc.
        rim_get_config()->ctx.db->conn = rim_calloc (conn_num, sizeof(one_db));
    }
    for (i = 0; i < totconn; i++)
    {
        if (!strcmp (conn, rim_get_config()->ctx.db->conn[i].db_name))
        {
            rim_report_error ("Database connection [%s] duplicated in the list of connections", conn);
            return i;
        }
    }
    totconn++;
    if (totconn > conn_num)
    {
        rim_num old_conn_num = conn_num;
        conn_num+=3; // add three connections at a time
        rim_get_config()->ctx.db->conn = rim_realloc (rim_mem_get_id(rim_get_config()->ctx.db->conn), conn_num*sizeof(one_db));
        // set to zero so g_con is NULL etc.
        rim_num j;
        for (j = old_conn_num; j < conn_num; j++) 
        {
            memset (&(rim_get_config()->ctx.db->conn[j]), 0, sizeof(one_db));
        }
    }
    rim_get_config()->ctx.db->conn[totconn-1].db_name=rim_strdup (conn);
    rim_get_config()->ctx.db->conn[totconn-1].db_type=dbtype; // the default (empty) is mariadb
    return totconn - 1;
}

//
// Build a list of connections, and return a fixed number between 0 and RIM_MAX_DB_CONNECTIONS that identifies the
// database connection we're dealing with. 'conn' is the connection name.
//
rim_num find_connection(char *conn)
{
    rim_num i;
    // make sure no spaces around the db name, there usually are from parsing
    char *db = rim_strdup (conn);
    rim_num dlen = strlen (db);
    rim_trim (db, &dlen, false);

    for (i = 0; i < totconn; i++)
    {
        if (!strcmp (db, rim_get_config()->ctx.db->conn[i].db_name))
        {
            return i;
        }
    }
    rim_report_error ("Database connection [%s] not found. There must be file [%s] in current directory describing the database, and you must use --db option to specify what kind of database it is", db, db);
    return -1;
}

//
// See rim_find_keyword0
//
char *find_keyword(char *str, char *find, rim_num has_spaces)
{
    return  rim_find_keyword0 (str, find, has_spaces, 1);
}


//
// Get value of option in statement. For example, if statement is 
// send-mail from "x@y.com" to "z@w.com" ...
// then you can get "x@y.com" by doing this:
// 
// char *from = find_keyword (...);
// carve_statement (&from, "send-mail", RIM_KEYFROM, 1, 1);
//
// where variable 'from' will be "x@y.com", '1' as 'is_mandatory' parameter means this parameter MUST be present,
// indicate where we're at in the parsing process (file name and line number).
// 'send-mail' is the name of top statement, and RIM_KEYFROM is "from", and we're parsing out the data after it.
// NOTE that 'from' MUST point to actual "x@y.com" within original send-mail string. This means ALL options must be first
// found with find_keyword() before calling this function for any of them.
// This function MUST be called for ALL statement options - if not then some statement values will contain other statement and will be 
// INCORRECT. 
// 'has_data' is 0 if the option is alone without data, for example 'no-cert'. Typically it's 1. It can also be 2, which is
// when there may be data after it, but it may also be missing.
// *statement is the value of what's carved out.
//
void carve_statement (char **statement, char *statement_name, char *keyword, rim_num is_mandatory, rim_num has_data)
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
            if (has_data != 2) rim_report_error( "Internal: keyword [%s] is missing trailing space", keyword);
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
        rim_report_error( "%s keyword is missing in %s", keyword, statement_name);
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
// 'type is RIM_DEFSTRING('char *') or RIM_DEFNUMBER or... (see below) - this is what variable should be (if it exists) or to be created
// If 'always' is true, then create new variable regardless, thus error out if same variable is used in the same scope - this is for
// indexes, lists, arrays etc.
// Returns 0 if variable existed and was not created, 1 if it didn't exist and was created.
//
// if the '*statement' has () around it, those are removed (see comments below) - this is to allow keywords to be used as variable names
// check that variable doesn't start with gg, rim, _gg or _rim
//
rim_num define_statement (char **statement, rim_num type, bool always)
{
    if (*statement == NULL) return 0;

    // if the result has () around it, remove it. There's no need for it, other than to use the same name
    // for a variable or function as is a keyword. If really needed, use (()), so the inner ones remain
    rim_num ls = strlen (*statement);
    // make sure *statement is not empty - this happens when data to a clause is not supplied
    *statement = rim_trim_ptr(*statement,  &ls);

    if ((*statement)[0] == 0) rim_report_error("Parameter missing in the statement");

    if (ls >= 3 && (*statement)[0] == '(' && (*statement)[ls-1] == ')')
    {
        // remove leading ( and trailing )
        (*statement)[ls - 1] = 0;
        (*statement)++;
        ls -= 2; // update length
    }


    // output is always a variable
    if (rim_is_valid_param_name(*statement, false, false) != 1) rim_report_error(RIM_NAME_INVALID, *statement);

    if (!strncmp (*statement, "rim_", sizeof("rim_")-1) ||
        !strncmp (*statement, "rim_", sizeof("rim_")-1) ||
        !strncmp (*statement, "_rim_", sizeof("_rim_")-1) ||
        !strncmp (*statement, "_rim_", sizeof("_rim_")-1)) rim_report_error ("Variable name starting with _rim_, _rim_, rim_ or rim_ are reserved and cannot be used");

    bool add_var_res;
    //
    // For static types within do-once, lower their rim_level, but only if current level is do_once_level+1 - see "do-once" handler elsewhere here in this file
    // Note that do-once CANNOT be nested, so it makes this simpler here.
    //
    if (do_once_open && rim_level == do_once_level+1 && (type == RIM_DEFSTRINGSTATIC || type == RIM_DEFNUMBERSTATIC || type == RIM_DEFDOUBLESTATIC || type == RIM_DEFHASHSTATIC || type == RIM_DEFARRAYSTRINGSTATIC || type == RIM_DEFARRAYNUMBERSTATIC || type == RIM_DEFARRAYDOUBLESTATIC || type == RIM_DEFARRAYBOOLSTATIC || type == RIM_DEFTREESTATIC || type == RIM_DEFLISTSTATIC || type == RIM_DEFBOOLSTATIC))
    {
        // make this variable in the same scope as do-once. All other vars are in +1 level. 
        // Note that generated C code is all the same level as do-once. We simulate higher level. The other variables
        // within do-once (other than process-scoped) could theoretically be accessed outside do-once block, but RimStone will
        // consider them non-existant, so they cannot be used - this prevents gcc from generating uninitialized warnings/errors.
        rim_level--; // this is the level C compiler see
        add_var_res = add_var (*statement, type, NULL, true);
        rim_level++;
    }
    else
    {
        // when do-once is in effect, rim_level is +1 (even though C compiler's level is one less), which prevents user
        // from using variables made in do-once outside it (technically in C they could, but in RimStone they can't, see above comment)
        add_var_res = add_var (*statement, type, NULL, true);
    }

    // Create var name  and store it to hash
    // Do not create if already exisiting
    if (!add_var_res)
    {
        // string is initialized to NULL (and not to RIM_EMPTY_STRING), since NULL is treated the same in rim_realloc/free, and 
        // because when static, gcc won't initialize to variable, even if it's a constant assigned (per C)
        if (type == RIM_DEFSTRING) oprintf ("char *%s = RIM_EMPTY_STRING;\n", *statement); // RIM_EMPTY_STRING cannot  be NULL in user execution
        else if (type == RIM_DEFSTRINGSTATIC) oprintf ("static char *%s;\n", *statement); // cannot use RIM_EMPTY_STRING b/c it's a variable
        else if (type == RIM_DEFNUMBER) oprintf ("rim_num %s = 0;\n", *statement);
        else if (type == RIM_DEFDOUBLE) oprintf ("rim_dbl %s = 0;\n", *statement);
        else if (type == RIM_DEFNUMBERSTATIC) oprintf ("static rim_num %s = 0;\n", *statement);
        else if (type == RIM_DEFDOUBLESTATIC) oprintf ("static rim_dbl %s = 0;\n", *statement);
        else if (type == RIM_DEFMSG) oprintf ("rim_msg *%s = NULL;\n",*statement);
        else if (type == RIM_DEFBROKEN) oprintf ("rim_split_str *%s = NULL;\n",*statement);
        else if (type == RIM_DEFHASH) oprintf ("rim_hash *%s = NULL;\n", *statement);
        else if (type == RIM_DEFHASHSTATIC) oprintf ("static rim_hash *%s = NULL;\n", *statement);
        else if (type == RIM_DEFARRAYSTRING) oprintf ("rim_arraystring *%s = NULL;\n", *statement);
        else if (type == RIM_DEFARRAYSTRINGSTATIC) oprintf ("static rim_arraystring *%s = NULL;\n", *statement);
        else if (type == RIM_DEFARRAYNUMBER) oprintf ("rim_arraynumber *%s = NULL;\n", *statement);
        else if (type == RIM_DEFARRAYNUMBERSTATIC) oprintf ("static rim_arraynumber *%s = NULL;\n", *statement);
        else if (type == RIM_DEFARRAYDOUBLE) oprintf ("rim_arraydouble *%s = NULL;\n", *statement);
        else if (type == RIM_DEFARRAYDOUBLESTATIC) oprintf ("static rim_arraydouble *%s = NULL;\n", *statement);
        else if (type == RIM_DEFARRAYBOOL) oprintf ("rim_arraybool *%s = NULL;\n", *statement);
        else if (type == RIM_DEFARRAYBOOLSTATIC) oprintf ("static rim_arraybool *%s = NULL;\n", *statement);
        else if (type == RIM_DEFJSON) oprintf ("rim_json *%s = NULL;\n", *statement);
        else if (type == RIM_DEFXML) oprintf ("rim_xml *%s = NULL;\n", *statement);
        else if (type == RIM_DEFTREE) oprintf ("rim_tree *%s = NULL;\n", *statement);
        else if (type == RIM_DEFTREESTATIC) oprintf ("static rim_tree *%s = NULL;\n", *statement);
        else if (type == RIM_DEFTREECURSOR) oprintf ("rim_tree_cursor *%s = NULL;\n", *statement);
        else if (type == RIM_DEFFIFO) oprintf ("rim_fifo *%s = NULL;\n", *statement);
        else if (type == RIM_DEFLIFO) oprintf ("rim_fifo *%s = NULL;\n", *statement);
        else if (type == RIM_DEFLIST) oprintf ("rim_list *%s = NULL;\n", *statement);
        else if (type == RIM_DEFLISTSTATIC) oprintf ("static rim_list *%s = NULL;\n", *statement);
        else if (type == RIM_DEFENCRYPT) oprintf ("EVP_CIPHER_CTX *%s = NULL;\n", *statement);
        else if (type == RIM_DEFFILE) oprintf ("rim_file *%s = NULL;\n", *statement);
        else if (type == RIM_DEFDIR) oprintf ("rim_directory *%s = NULL;\n", *statement);
        else if (type == RIM_DEFSERVICE) oprintf ("rim_cli *%s = NULL;\n", *statement);
        else if (type == RIM_DEFBOOL) oprintf ("bool %s = false;\n", *statement);
        else if (type == RIM_DEFBOOLSTATIC) oprintf ("static bool %s = false;\n", *statement);
        else rim_report_error( "Unknown type (%ld)", type);
        return 1;
    } else 
    {

        if (always) rim_report_error ("Variable [%s] cannot be created again", *statement);
        return 0;
    }
} 



// 
// Find terminal width for help display.
// Returns width in characters.
//
rim_num terminal_width()
{
    static rim_num twidth = 0;
    if (twidth == 0)
    {
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

        twidth = (rim_num)(w.ws_col);
    }
    return twidth;
}

//
// Parse a list of parameters. Parameters are a list of strings or non-string parameters separated
// by commas. Parameter can have a comma if it is a string, or within ()
//
// String representing list of parameters is parse_list. RIM list structure is 'params', which will
// hold the parsed parameters. 
// parse_list is advanced to its end during processing.
// tot is the number of elements (single or x=y pairs encountered); it can be NULL.
//
// This is for a LIST of parameters separated by comma ONLY (i.e. exec-program).
// Each param is trimmed.
// On return, parse_list is one byte passed the end of last character in the list, which is null character.
//
void parse_param_list (char *parse_list, rim_fifo **params, rim_num *tot)
{
    rim_store_init(params);
    if (tot != NULL) *tot = 0;

    while (1)
    {
        // input param can be anything separate by comma, which must be outside quotes
        // or parenthesis, i.e. free-comma.
        // so find free comma, or if none found returns pointer
        // to the ending null char of *parse_list string.
        char *milestone = find_keyword(parse_list, ",", 0);
        char *end_of_parse_list; // this is where parse_list should be after looking for parameter
        char _rim_inp_par[RIM_MAX_QUERY_INPUT_LEN + 1];
        char *inp_par=_rim_inp_par; // because we will ++ this pointer, can't work otherwise

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
        rim_num par_len = milestone - (parse_list);

        if (par_len > RIM_MAX_QUERY_INPUT_LEN - 1)
        {
            rim_report_error( "Parameter too long [%ld], parameter [%.100s]", par_len, parse_list);
        }

        memcpy (inp_par, parse_list, par_len);
        inp_par[par_len] = 0;

        rim_trim (inp_par, &par_len, false);
        rim_store(*params, RIM_EMPTY_STRING, (void*)rim_strdup(inp_par));

        parse_list = end_of_parse_list; // positioned to next parameter
                                         // this needs to be before break below
                                         // because caller relies on parse_list being exhausted
        if (tot != NULL) (*tot)++;

        // break if at the end
        if (*end_of_parse_list==0) break;

    } 
    rim_rewind(*params);
}

//
// Find next input parameter in SQL text. gen_ctx is the context of parsing, and 
// This parses part of statement such as query: a,b,c... i.e. it finds a, b, c etc. In this case query text uses '%s' 
// The parameter is a C expression or a quoted string (technically also a C expression, but treated differently).
// A parameter must be either a string, or not have comma, or be within ()
// So for example a parameter like func_call("x",2) would be (func_call("x",2))
//
// gen_ctx is the context. query_id is index into gen_ctx->qry[] array. iparams is the list of parameters to be parsed
//
void get_all_input_param (rim_gen_ctx *gen_ctx, char *iparams)
{

    // parse params. 
    rim_fifo *params = NULL;
    parse_param_list (iparams, &params, NULL);


    char *value;
    while (1)
    {
        rim_num st = rim_retrieve (params, NULL, (void**)&value);
        if (st != RIM_OKAY) break;

        make_mem (&value);
        check_var (&value, RIM_DEFSTRING);
        add_input_param (gen_ctx, value);
    } 
}

// 
// Add input parameter to a query.
// gen_ctx is the context. query_id is the index into gen_ctx->qry[] (i.e. queries). inp_par is the string
// representing input parameter 
//
void add_input_param (rim_gen_ctx *gen_ctx, char *inp_par)
{

    rim_strncpy (gen_ctx->qry[query_id].qry_inputs[gen_ctx->qry[query_id].qry_found_total_inputs], inp_par, 
       RIM_MAX_QUERY_INPUT_LEN - 1);

    oprintf("_rim_is_input_used_%s[%ld]=1;\n",  gen_ctx->qry[query_id].name, gen_ctx->qry[query_id].qry_found_total_inputs);
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
void end_query (rim_gen_ctx *gen_ctx, rim_num close_block)
{
    // end of action block found , but there was not beginning
    if (query_id == -1)
    {
        rim_report_error( "query ending found, but no matching beginning");
    }
    // Ending of query with ID of leaving ID
    rim_num leaving_id = gen_ctx->curr_qry_ptr;
    // must be (leaving_id != -1);

    // no longer active, and its variables are defined, in case we
    // need to use them again - but they won't be defined again 
    gen_ctx->qry_active[leaving_id] = RIM_QRY_INACTIVE;

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

    static rim_num oline_prev_line = 0; // has previous line ended?

    if (format == NULL)
    {
        if (oline != NULL)
        {
            // remove empty printouts
            rim_num len = strlen (oline);
            rim_replace_string (oline, len+1, "rim_puts (RIM_NOENC, \"\", RIM_EMPTY_LONG_PLAIN_ZERO, false);\n", "", 1, NULL, 1); // remove idempotent printouts
                                                    // will always succeed because output is shorter 
            rim_replace_string (oline, len+1, "rim_puts (RIM_NOENC, \"\", RIM_EMPTY_LONG_PLAIN_ZERO, false);", "", 1, NULL, 1); // replace with or without new line

            //
            // make sure consecutive new lines are gone as they confuse gdb: when stepping through rimstone code, gdb sometimes goes to the next line, meaning
            // showing like it goes there, but it doesn't execute it. Bogus step. This is because of consecuitve new lines.
            rim_num lsub;
            lsub = rim_replace_string (oline, len+1, "\n\n", "\n", 1, NULL, 1); 
            // subst new lines until nothing done
            while (1)
            {
                rim_num nsub = rim_replace_string (oline, len+1, "\n\n", "\n", 1, NULL, 1); 
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
            rim_free (oline);
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

    if (oline == NULL) oline = rim_malloc (oline_size += 2*RIM_MAX_CODE_LINE + 1);

    // add line number so all the code generated is the same line number
    // but only if the previous line had a new line (meaning this is the beginning of the new line of source code)
    // for example a single code line may be output by several oprintfs (this is oline_prev_line)
    if (oline_prev_line == 1 && RIM_EMIT_LINE)
    {
        // make reported line as line*10000+generated_line, so each generated line is tied to original
        // line (final_line/10000), but it also has information about which generated line the diagnostic is about
        // this is used in error reporting
        static rim_num old_lnum = -1;
        static rim_num in_lnum = 0;
        if (old_lnum != lnum) { old_lnum = lnum; in_lnum = 0; } else in_lnum++;
        rim_num lwritten = lnum;
        if (can_write_line_number (oline_len, lwritten))
        {
            rim_num tot_written = snprintf (oline + oline_len, oline_size - oline_len - 1, "#line %ld \"%s/%s\"\n", lwritten, rim_valid_rim_dir, rim_valid_rim_name);
            written_line_number(oline_len, lwritten);
            if (tot_written >= oline_size - oline_len - 1)
            {
                rim_report_error ("Source code line too long, exiting");
                exit (1);
            }
            oline_len += tot_written;
        }
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
        rim_num l = strlen (format);
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
        rim_report_error ("Source code line too long, exiting");
        exit (1);
    }
    //
    bool nl;
    if ((nl = (strchr (oline + oline_len, '\n') != NULL))) 
    {
        oline_prev_line = 1;
        // check for { and } as keyword (i.e. not within strings) and adjust rim_level accordingly
    }
    oline_len += oline_written;
    if (nl)
    {
        check_level(oline + oline_prev_line_beg);
        oline_prev_line_beg = oline_len;
    }

    // expand if getting long, meaning what's left is less than RIM_MAX_CODE_LINE
    if (oline_size-oline_len <= RIM_MAX_CODE_LINE)
    {
        oline = rim_realloc (rim_mem_get_id(oline), oline_size += 2*RIM_MAX_CODE_LINE + 1);
    }
    va_end (args);
}

// 
// Output warning to stdout. During the preprocessing with RIM.
// There's a maximum length for it, and if it's more than that, ignore the rest.
// If there is no dot at the end, put it there.
// After that, continue program.
//
void rim_report_warning (char *format, ...)
{
    if (rim_ignore_warn) return;
    char wrntext[RIM_MAX_ERR_LEN + 1];

    va_list args;
    va_start (args, format);
    vsnprintf (wrntext, sizeof(wrntext) - 1, format, args);
    va_end (args);
    fprintf (stderr, "Warning: %s", wrntext);
    if (src_file_name != NULL)
    {
        // make sure path is displayed
        fprintf (stderr, ", reading file [%s], line [%ld].\n", rim_valid_rim_name, lnum);
    }
    else
    {
        if (wrntext[0] != 0 && wrntext[strlen (wrntext) - 1] != '.')  fprintf(stderr, ".\n");
    }
}

// 
// Output error to stderr. The error means error during the preprocessing with RIM.
// There's a maximum length for it, and if it's more than that, ignore the rest.
// If there is no dot at the end, put it there.
// When rim_report_error is called, exit(1) is appended. However,
// _rim_report_error can be called multiple times before it exits (see below)
//
void _rim_report_error (char *format, ...)
{
    char errtext[RIM_MAX_ERR_LEN + 1];

    oprintf (NULL); //  flush output

    va_list args;
    va_start (args, format);
    vsnprintf (errtext, sizeof(errtext) - 1, format, args);
    va_end (args);
    fprintf (stderr, "%s", errtext);
    if (src_file_name != NULL)
    {
        // make sure path is displayed
        fprintf (stderr, ", reading file [%s], line [%ld].\n", rim_valid_rim_name, lnum);
    }
    else
    {
        if (errtext[0] != 0 && errtext[strlen (errtext) - 1] != '.')  fprintf(stderr, ".\n");
    }
    fflush (stderr);
    rim_curr_errors++;
    if (rim_curr_errors >= rim_max_errors) exit (1);
}


// 
// Add new query. gen_ctx is the context, vp is the query parsing context.
// Error messages are emitted if there are syntax errors.
//
void new_query (rim_gen_ctx *gen_ctx, rim_db_parse *vp)
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
    if (gen_ctx->curr_qry_ptr >= RIM_MAX_QUERY_NESTED) rim_report_error( RIM_MSG_NESTED_QRY, query_id, RIM_MAX_QUERY_NESTED);
    // now this query ID is active. We use it to prohibit nesting queries with the same ID
    gen_ctx->qry_active[query_id] = RIM_QRY_ACTIVE;
    // this is start-query, so the query has now officially been used
    gen_ctx->qry_used[query_id] = RIM_QRY_USED;
    //
    // End new query creation
    //

    char *qry=rim_strdup(orig_qry);
    rim_num ql = strlen(qry);
    // 
    // We do not touch % signs, because we do NOT use snprintf like function at run time in rim_make_SQL
    // '%s' will be replaced with input parameters, others won't be touched
    //
    // Sanity check and error
    if (qry[0] != 0 && qry[ql - 1] == ';')
    {
        rim_report_error( "Query cannot end with a semicolon");
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
    gen_ctx->qry_used[query_id] = RIM_QRY_UNUSED;
    gen_ctx->total_queries++;
    if (gen_ctx->total_queries >= RIM_MAX_QUERY)
    {
        rim_report_error("Too many queries specified");
    }
}

// 
// Find current query ID - all references to queries are implicit and refer to the current one. gen_ctx is the context.
// Returns query ID or -1 if none found..
//
rim_num find_query (rim_gen_ctx *gen_ctx)
{
    return gen_ctx->curr_qry_ptr - 1;
}


// 
// Change pointer to string pointer s, so it advances until either 1) a comma is encountered or 2) the end of the string
// The pointer is now at either one
//
void get_until_comma (char **s)
{
    rim_num i = 0;
    while (((*s)[i])!=',' && (*s)[i]!=0) i++;
    *s = *s + i;
}

// 
// Change pointer to string pointer s, so it advances until either 1) a whitespace is encountered or 2) the end of the string
// The pointer is now at whitespace
//
void get_until_whitespace (char **s)
{
    rim_num i = 0;
    while (!isspace ((*s)[i])) i++;
    *s = *s + i;
}

// 
// Change pointer to string pointer s, so it advances until either 1) a non-whitespace is encountered or 2) the end of the string
// The pointer is now non-whitespace (presumably after skipping bunch (or zero) of whitespaces)
//
void get_passed_whitespace (char **s)
{
    rim_num i = 0;
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
// mtext is null-terminated so it can be rim_trim()-ed.
// is_inline is 1 if this was the end (>>) of <<..>> inline statement
// Errors out if there is an unterminated string after statement, or if arguments aren't there (either end of string or >>)
//
rim_num recog_statement (char *cinp, rim_num pos, char *opt, char **mtext, rim_num *msize, rim_num isLast, rim_num *is_inline)
{


    *is_inline = 0; // by default this isn't closed by >>

    // do not assign mtext in case of not-found, because it could override the found one when
    // we check for multiple recog_statement() in the same if(), same for msize
    rim_num opt_len = strlen (opt);
    rim_num orig_position = pos;
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
            rim_num is_end_of_inline = !memcmp (cinp+pos, ">>", 2) ? 1: 0;
            if (is_end_of_inline == 1)
            {
                *is_inline = 1;
                if (within_inline != 1)  rim_report_error ("End of inline detected (>>), but no beginning (<<)");
                within_inline = 0;
            }
            if (cinp[pos] == 0 || is_end_of_inline == 1)
            {
                usedRIM = 0;
                *msize = (cinp+pos)-*mtext;
                (*mtext)[*msize] = 0; // zero the >>
                // rimstone line never has new line in it, so this should be fine
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
                if (within_inline != 1)  rim_report_error ("End of inline detected (>>), but no beginning (<<)");
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
                rim_report_error( "Statement '%s' cannot end with semicolon", opt);
            }
            // if opt is empty, the whole rimstone marker is c code, just pass it back
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
                    rim_report_error( "Statement '%s' has no arguments, but it should", opt);
                }
                return 0; // must have space afterward
            }
            usedRIM = 0;
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
void init_rim_gen_ctx (rim_gen_ctx *gen_ctx)
{

    rim_num i;
    rim_num j;
    for (j = 0; j < RIM_MAX_QUERY; j++) 
    {
        gen_ctx->qry[j].returns_tuple = 1;
        for (i = 0; i < RIM_MAX_QUERY_INPUTS; i++)  
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
void rim_gen_c_code (rim_gen_ctx *gen_ctx, char *file_name)
{
    FILE* f = rim_fopen1 (file_name, "r");
    if (f == NULL)
    {
        rim_report_error( "Error opening file [%s]", file_name);
    }


    //
    // Start reading line by line from the source file
    //
    char *res;


    rim_num json_id = 0;
    rim_num xml_id = 0;
    rim_num open_readline = 0;
    rim_num open_ifs = 0;
    rim_num open_curly=0;
    rim_num line_len = 0; // unlike 'len' below, used only for concatenation of lines

    rim_num http_header_count = 0; // used for _rim_tmp_header_%ld 
    rim_num encrypt_count=0; // used for encryption length var
    rim_num file_count=0; // used for static file descriptors
    rim_num trim_count=0; // used for trim length var
    rim_num code_text = 0; // counter for status/errt vars
    rim_num regex_cache = 0; // used to cache regex compilation


    rim_num do_once = 0;
    rim_num do_once_line = 0;
    bool ccomm_open = false; // true if C comment opened in some line prior
    rim_num ccomm_line = -1; // line where /* C comment opened


    // Include rim.h so it's not necessary, doubles are prevented with ifdefs inside
    oprintf("#include \"rim.h\"\n");
    oprintf("#include \"rim_sparam_%s.h\"\n",rim_basename(file_name));

    bool line_split = false;

    // 
    // Main loop in which lines are read from the source file
    //
    while (1)
    {
        res = fgets_unlocked (line + line_len, sizeof (line) - line_len- 1, f);
        if (res == NULL) // either end or error
        {
            rim_num err = ferror_unlocked (f);
            if (err) // if error, quit
            {
                rim_report_error( "Error [%s] reading file [%s]", strerror (errno), file_name);
            }
            break; // nothing read in, exit normally
        }
        lnum++;


        rim_num len;
        // 
        // if this is continuation, trim the continuation to avoid big gaps in between pieces
        // Lines can be concatenated
        //
        if (line_len > 0)
        {
            rim_num cont_len = strlen (line+line_len);
            // check if continuation starts with space. If it doesn't, we cannot trim.
            if (cont_len > 0)
            {
                // Concatenation means just that - concatenation.
                // each line in concatenation is trimmed both ends
                rim_trim (line+line_len, &cont_len, false);
            }
            len = line_len + cont_len;
        }
        else
        {
            // if line_len is zero then we trim line (meaning also on the left). If line_line > 0, then the above will trim on the right,
            // no need to trim on the left too again
            len = strlen (line);
            rim_trim (line, &len, false);
        }

        // 
        // Setup line number for error reporting. Print new line in front
        // so if we forget to finish a line with new line, this would ruin
        // C syntax.
        // For internal debugging, may be useful not to have it.
        //
        if (RIM_EMIT_LINE && len > 0)
        {
            rim_num lwritten = lnum;
            if (can_write_line_number (oline_len, lwritten))
            {
                rim_no_output_line_num = true;
                oprintf("\n#line %ld \"%s/%s\"\n", lwritten, rim_valid_rim_dir, rim_valid_rim_name);
                rim_no_output_line_num = false;
                written_line_number(oline_len, lwritten);
            }
        }

        if (len >= (rim_num)sizeof (line) - 16) // don't allow too big of a line
        {
            rim_report_error( "Line too long");
        }

        rim_num i;

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


        char *mtext = NULL;
        rim_num msize = 0;
        rim_num first_on_line = 1;
        rim_num is_verbatim = 0; // 1 if #  to print out
        within_inline = 0; // if statement is within <<...>>
        usedRIM = 0;
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
        rim_num op_i = 0;
        bool line_comm = false; // true if line starts with /**/ comment, we disallow those before beginning of statement as it's hard to read
                                    // and also detecting statements include/libs in gg become way more difficult
        rim_num ccomm_pos = -1; // position of /* that starts multiline comment
        while (isspace (line[op_i])) op_i++; // find beginning of line (ignore whitespace)
        bool linebeg = op_i; // this is where non-space of line begins
        // if this is @ or !, all comments are ignored, this is whole-line output statement
        if (ccomm_open || (memcmp(line+op_i, "@",1) && memcmp(line+op_i, "!",1))) 
        {
            // Check for /**/ comment
            // There can be multiple /**/ comments on a line, so this must be a loop
            bool skip_line=false;
            rim_num lcheck = 0;
            bool closing_ccomm_found = false;// true if */ potentially found without matching /* on the same line
                                             // may be a false alarm if */ is after //
            rim_num closing_ccomm_pos = -1;// position of closing */ per above bool
            while (1)
            {
                char *cl_comment = rim_find_keyword0 (line+lcheck, "/*", 0, 0);
                char *cr_comment = rim_find_keyword0 (line+lcheck, "*/", 0, 0);
                // calculate where /* and */ are, meaning the first ones found
                rim_num l_pos = -1;
                rim_num r_pos = -1;
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
            char *cpp_comment = rim_find_keyword0 (line, "//", 0, 0);
            if (cpp_comment != NULL)
            {
                *cpp_comment = 0;
                len = cpp_comment - line; // after removing the comment, length is smaller
                // if /* for multiline is after //, then ignore it
                if (ccomm_pos !=-1 && ccomm_pos > len) ccomm_open = false;
                // if */ is found after //, then ignore it, unless it closes a multi-line /**/. If it doesn't
                // then it's ignored 
                if (closing_ccomm_found && closing_ccomm_pos < len) rim_report_error( "Closing */ C comment found, but there was no opening /* comment");
            }
            if (skip_line) continue; // this line is a line where /* started before it, but no */ found yet, so ignore
        }
        //
        // END comment handling
        //

        // If in @, this is if >> was just processed (end of inline), so start printing again
        // as we have temporarily suspended printing when << was found (by doing rim_puts(<end of string>...)
        // reset rim_is_inline afterwards so any new checks (aside from recog_statement() which sets rim_is_inline)
        // do not get wrong value
#define RIM_OPEN_PRINT  if (rim_is_inline == 1 && print_mode == 1) oprintf("rim_puts (RIM_NOENC, \""); \
        rim_is_inline = 0; 


        // check if line is empty, if not, and if there was /**/ prior to it, that's not allowed (see above comments)
        op_i = 0;
        while (isspace (line[op_i])) op_i++; // find beginning of line (ignore whitespace)
        if (line[op_i] != 0 && line_comm) rim_report_error ("Statement cannot start with /**/ comment");

        //
        // In this loop, a line is examined char by char. However, once certain patterns are recognized, parsing
        // progresses more rapidly. Basically, recog_statement() is called to recognized the beginning of the RIM statement,
        // and from there the rest is parsed in a more rapid fashion. Each statement is checked, and once recognized and
        // processed, the next line is read in the outer loop.
        // Skip whitespaces
        //
        for (i = op_i; i < len; i++)
        {
            rim_num newI = 0;
            rim_num newI1 = 0;
            rim_num newI2 = 0;
            rim_num newI3 = 0;
            rim_num newI4 = 0;
            rim_num newI5 = 0;
            rim_num newI6 = 0;
            rim_num newI7 = 0;
            rim_num newI8 = 0;

            // since every statement, after it's recognized and consumed, does continue, it comes back here.
            // so if statement was inline, and within @, then we must start printing. This is first thing right 
            // after >>. We stopped printing at <<. So this continues printing; see the RIM_OPEN_PRINT after this for loop
            RIM_OPEN_PRINT

            if (is_verbatim == 0) 
            {

                if (!memcmp (line + i, "<<", 2)) // short directive
                {
                    i = i + 2; // past "<<"
                    within_inline = 1;
                    usedRIM = 1;
                    if (print_mode == 1)
                    {
                        // Finish current portion of @ line, so that <<>> can do its thing
                        // if not in @, then just execute
                        oprintf("\", RIM_EMPTY_LONG_PLAIN_ZERO, false);\n");
                    }
                }
                else
                {
                    ; // else? nothing. We consider new line start within RIM_BEGIN/RIM_END block to be the start
                    // of the <<rimstone statement>>, for simplicity of coding.
                }


                // checking for RIM statement without <<..>> can only work in those statements where they are the first on the 
                // line, OR if this is actual statement << ...>>
                if (first_on_line == 1 || usedRIM == 1)
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
                    if ((newI=recog_statement(line, i, "end-query", &mtext, &msize, 1, &rim_is_inline)) != 0) // is it end of query action block
                    {
                        RIM_GUARD
                        i = newI;
                        carve_stmt_obj (&mtext, false, false);

                        end_query (gen_ctx,  1);

                        continue; // skip the statement and continue analyzing 
                    }
                    else if ((newI=recog_statement(line, i, "start-loop", &mtext, &msize, 1, &rim_is_inline)) != 0 
                        || (newI1=recog_statement(line, i, "start-loop", &mtext, &msize, 0, &rim_is_inline)) != 0) 
                    {
                        RIM_GUARD
                        i = newI+newI1;
                        check_next_loop
                        if (newI != 0)
                        {
                            oprintf ("while (1) {\n");
                        }
                        else
                        {
                            char *repeat = find_keyword (mtext, RIM_KEYREPEAT, 0);
                            char *use = find_keyword (mtext, RIM_KEYUSE, 1);
                            char *swith = find_keyword (mtext, RIM_KEYSTARTWITH, 1);
                            char *add = find_keyword (mtext, RIM_KEYADD, 1);
                            carve_statement (&repeat, "start-loop", RIM_KEYREPEAT, 0, 1);
                            carve_statement (&use, "start-loop", RIM_KEYUSE, 0, 1);
                            carve_statement (&swith, "start-loop", RIM_KEYSTARTWITH, 0, 1);
                            carve_statement (&add, "start-loop", RIM_KEYADD, 0, 1);
                            carve_stmt_obj (&mtext, false, false);
                            define_statement (&use, RIM_DEFNUMBER, false); 
                            //
                            check_var (&repeat, RIM_DEFNUMBER);
                            check_var (&swith, RIM_DEFNUMBER);
                            check_var (&add, RIM_DEFNUMBER);

                            static rim_num _rim_loop = 0;
                            //if (repeat == NULL  && use != NULL) rim_report_error("Must specify 'repeat' clause with 'use'");
                            if ((swith != NULL || add != NULL) && use == NULL) rim_report_error("Must specify 'use' clause with 'start-with' or 'add' clause");
                            if (use != NULL && rim_is_valid_param_name(use, false, false) != 1) rim_report_error(RIM_NAME_INVALID, use);
                            if (use != NULL) oprintf("%s=%s-%s;\n", use, swith!=NULL?swith:"1", add==NULL?"1":add); // -<add> as we do ++/+= at the beginning of the loop
                            // allow for loop where we have repeat and don't have it
                            // if we don't have repeat, then just keep incrementing 'use' and we must use continue or break to get out
                            if (repeat != NULL) 
                            {
                                // in a common scenario when add is 1 and using a variable, just use this variable and avoid two counters (better performance)
                                if ((add == NULL || !strcmp (add, "1")) && use != NULL) 
                                {
                                    oprintf ("rim_num _rim_loop_end%ld=(%s)+(%s);\n", _rim_loop, swith==NULL?"1":swith, repeat);
                                    oprintf ("for (%s=(%s); %s < _rim_loop_end%ld; (%s)++) {\n", use, swith==NULL?"1":swith, use, _rim_loop, use);
                                }
                                else 
                                {
                                    if (use != NULL)
                                    {
                                        // in this case add!=NULL (we checked above add=NULL && use != NULL)
                                        oprintf ("rim_num _rim_loop_end%ld=(%s)+1;\n", _rim_loop, repeat);
                                        oprintf ("rim_num _rim_loop%ld;\nfor (_rim_loop%ld=1, (%s)=(%s); _rim_loop%ld < _rim_loop_end%ld; _rim_loop%ld++,(%s)+=(%s)) {\n", _rim_loop, _rim_loop, use, swith==NULL?"1":swith, _rim_loop, _rim_loop, _rim_loop, use, add); // +1 needed with 'repeat' because we want the 'use' to come out at number over after the loop
                                    }
                                    else
                                    {
                                        oprintf ("rim_num _rim_loop%ld;\nfor (_rim_loop%ld=1; _rim_loop%ld < (%s)+1; _rim_loop%ld++) {\n", _rim_loop, _rim_loop, _rim_loop, repeat, _rim_loop); // +1 needed with 'repeat' because we want the 'use' to come out at number over after the loop
                                    }
                                }
                            }
                            else oprintf ("rim_num _rim_loop%ld;\nfor (_rim_loop%ld=1; ; _rim_loop%ld++) {\n", _rim_loop, _rim_loop,  _rim_loop);

                            _rim_loop++;
                        }
                        continue; // skip the statement and continue analyzing 
                    }
                    else if ((newI=recog_statement(line, i, "end-defined", &mtext, &msize, 1, &rim_is_inline)))
                    {
                        //not part of code block
                        //RIM_GUARD
                        i = newI;

                        oprintf ("#endif\n");

                        continue; // skip the statement and continue analyzing 
                    }
                    else if ((newI=recog_statement(line, i, "if-defined", &mtext, &msize, 0, &rim_is_inline)) != 0
                     || (newI1=recog_statement(line, i, "if-not-defined", &mtext, &msize, 0, &rim_is_inline)) != 0) 
                    {
                        //not part of code block
                        //RIM_GUARD
                        i = newI+newI1;

                        char *eq = find_keyword (mtext, RIM_KEYEQUAL, 1);
                        char *neq = find_keyword (mtext, RIM_KEYNOTEQUAL, 1);
                        char *less = find_keyword (mtext, RIM_KEYLESSERTHAN, 1);
                        char *lesseq = find_keyword (mtext, RIM_KEYLESSEREQUAL, 1);
                        char *gr = find_keyword (mtext, RIM_KEYGREATERTHAN, 1);
                        char *greq = find_keyword (mtext, RIM_KEYGREATEREQUAL, 1);

                        carve_statement (&eq, "if-defined", RIM_KEYEQUAL, 0, 1); 
                        carve_statement (&neq, "if-defined", RIM_KEYNOTEQUAL, 0, 1); 
                        carve_statement (&less, "if-defined", RIM_KEYLESSERTHAN, 0, 1); 
                        carve_statement (&lesseq, "if-defined", RIM_KEYLESSEREQUAL, 0, 1); 
                        carve_statement (&gr, "if-defined", RIM_KEYGREATERTHAN, 0, 1); 
                        carve_statement (&greq, "if-defined", RIM_KEYGREATEREQUAL, 0, 1); 
                        carve_stmt_obj (&mtext, true, false);
                        // the name that's defined from either gg compilation (--cflag) or specified by the platform (such as
                        // __x86_64__), can start with "_"
                        if (!(mtext[0] == '_' || isalpha (mtext[0])) && rim_is_valid_param_name(mtext+1, false, false) != 1) rim_report_error(RIM_NAME_INVALID, mtext);

                        if (newI1 != 0 && (eq != NULL || neq != NULL || less != NULL || lesseq != NULL || gr != NULL || greq != NULL)) rim_report_error ("Cannot use comparison clauses with if-not-defined");
                        rim_num tot = (eq != NULL ?1:0)+(neq != NULL ?1:0)+(less != NULL ?1:0)+(lesseq != NULL ?1:0)+(gr != NULL ?1:0)+(greq != NULL ?1:0);
                        if (tot > 1) rim_report_error ("Only a single comparison operator can be used with if-defined");
                        char *op = RIM_EMPTY_STRING;
                        if (tot == 1) 
                        {
                            op = (eq!=NULL?eq:(neq!=NULL?neq:(less!=NULL?less:(lesseq!=NULL?lesseq:(gr!=NULL?gr:(greq!=NULL?greq:NULL))))));
                            rim_num st;
                            rim_str2num (op, 0, &st);
                            if (st != RIM_OKAY)  rim_report_error ("Invalid number in if-defined comparison");
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
                    else if ((newI=recog_statement(line, i, "{", &mtext, &msize, 1, &rim_is_inline)) != 0) 
                    {
                        //not part of code block
                        //RIM_GUARD
                        i = newI;

                        carve_stmt_obj (&mtext, false, false);
                        oprintf ("{\n");
                        open_curly++;
                        continue; // skip the statement and continue analyzing 
                    }
                    else if ((newI=recog_statement(line, i, "}", &mtext, &msize, 1, &rim_is_inline)) != 0) 
                    {
                        //not part of code block
                        //RIM_GUARD
                        i = newI;

                        carve_stmt_obj (&mtext, false, false);
                        oprintf ("}\n");
                        open_curly--;

                        continue; // skip the statement and continue analyzing 
                    }
                    else if ((newI=recog_statement(line, i, "pause-program", &mtext, &msize, 0, &rim_is_inline)) != 0) 
                    {
                        RIM_GUARD
                        i = newI;

                        carve_stmt_obj (&mtext, true, false);
                        check_var (&mtext, RIM_DEFNUMBER);
                        oprintf ("rim_sleepabit (%s);\n", mtext);

                        continue; // skip the statement and continue analyzing 
                    }
                    else if ((newI=recog_statement(line, i, "continue-loop", &mtext, &msize, 1, &rim_is_inline)) != 0) 
                    {
                        RIM_GUARD
                        i = newI;

                        carve_stmt_obj (&mtext, false, false);
                        if (open_loops <= 0 && query_id == -1 && open_readline <= 0) rim_report_error ("continue-loop must be between start-loop and end-loop statements, or within a run-query/end-query statements, or within read-line and end-read-line");
                        oprintf ("continue;\n");

                        continue; // skip the statement and continue analyzing 
                    }
                    else if ((newI=recog_statement(line, i, "break-loop", &mtext, &msize, 1, &rim_is_inline)) != 0) // is it end of query action block
                    {
                        RIM_GUARD
                        i = newI;

                        carve_stmt_obj (&mtext, false, false);
                        if (open_loops <= 0 && query_id == -1 && open_readline <= 0) rim_report_error ("break-loop must be between start-loop and end-loop statements, or within a run-query/end-query statements, or within read-line and end-read-line");
                        oprintf ("break;\n");

                        continue; // skip the statement and continue analyzing 
                    }
                    else if ((newI=recog_statement(line, i, "end-loop", &mtext, &msize, 1, &rim_is_inline)) != 0) // is it end of query action block
                    {
                        RIM_GUARD
                        i = newI;

                        carve_stmt_obj (&mtext, false, false);
                        oprintf ("}\n");
                        open_loops--;

                        continue; // skip the statement and continue analyzing 
                    }
                    // Checking for non-mandatory (6th param is 1) MUST be done first.
                    else if (((newI=recog_statement(line, i, "rollback-transaction", &mtext, &msize, 1, &rim_is_inline)) != 0) 
                     || ((newI1=recog_statement(line, i, "rollback-transaction", &mtext, &msize, 0, &rim_is_inline)) != 0)) 
                    {
                        RIM_GUARD
                        i = newI+newI1;
                        char *opt = find_keyword (mtext, RIM_KEYOPTIONS, 1);
                        char *database = find_keyword (mtext, RIM_KEYAT, 0);
                        char *oncont = find_keyword (mtext, RIM_KEYONERRORCONTINUE, 1);
                        char *onexit = find_keyword (mtext, RIM_KEYONERROREXIT, 1);
                        char *errt = find_keyword (mtext, RIM_KEYERRORTEXT, 1);
                        char *err = find_keyword (mtext, RIM_KEYERROR, 1);
                        carve_statement (&opt, "rollback-transaction", RIM_KEYOPTIONS, 0, 1);
                        carve_statement (&database, "rollback-transaction", RIM_KEYAT, 0, 1);
                        carve_statement (&oncont, "rollback-transaction", RIM_KEYONERRORCONTINUE, 0, 0);
                        carve_statement (&onexit, "rollback-transaction", RIM_KEYONERROREXIT, 0, 0);
                        carve_statement (&errt, "rollback-transaction", RIM_KEYERRORTEXT, 0, 1);
                        carve_statement (&err, "rollback-transaction", RIM_KEYERROR, 0, 1);
                        carve_stmt_obj (&mtext, false, false);
                        make_mem(&opt);
                        // database is not a string constant or variable, though it's stringified internally, so no make_mem
                        define_statement (&errt, RIM_DEFSTRING, false); // allocated with strdup in db.c, exact length
                        define_statement (&err, RIM_DEFSTRING, false); // allocated with strdup in db.c, exact length
                        //
                        check_var (&opt, RIM_DEFSTRING);

                        get_db_config (database);
                        //erract is RIM_ON_ERROR_CONTINUE/EXIT for statement-specific on-error continue/exit or RIM_OKAY if db-level on-error is in effect
                        oprintf("rim_rollback (\"%s\", %s, %s%s%s, %s%s%s);\n", opt==NULL?"":opt, oncont!=NULL?"RIM_ON_ERROR_CONTINUE":(onexit!=NULL?"RIM_ON_ERROR_EXIT":"RIM_OKAY"),
                            err!=NULL?"&(":"", err !=NULL?err:"NULL", err!=NULL?")":"", 
                            errt!=NULL?"&(":"", errt !=NULL?errt:"NULL", errt!=NULL?")":""
                            );
                        continue;
                    }
                    else if (((newI=recog_statement(line, i, "db-error", &mtext, &msize, 0, &rim_is_inline)) != 0)) // commit transaction
                    {
                        RIM_GUARD
                        i = newI;
                        char *database = find_keyword (mtext, RIM_KEYAT, 0);
                        char *ex = find_keyword (mtext, RIM_KEYEXIT, 1);
                        char *cont = find_keyword (mtext, RIM_KEYCONTINUE, 1);
                        carve_statement (&database, "db-error", RIM_KEYAT, 0, 1);
                        carve_statement (&ex, "db-error", RIM_KEYEXIT, 0, 0);
                        carve_statement (&cont, "db-error", RIM_KEYCONTINUE, 0, 0);
                        carve_stmt_obj (&mtext, false, false);

                        make_mem (&database);
                        //

                        if (ex == NULL && cont == NULL) rim_report_error( "Missing 'exit' or 'continue'");
                        if (ex != NULL && cont != NULL) rim_report_error( "Cannot have both 'exit' or 'continue'");

                        get_db_config (database);
                        oprintf ("rim_get_config()->ctx.db->conn[rim_get_config()->ctx.db->ind_current_db].exit_on_error=%d;\n", ex != NULL ? 1 : 0);
                        continue;
                    }
                    // Checking for non-mandatory (6th param is 1) MUST be done first.
                    else if (((newI=recog_statement(line, i, "commit-transaction", &mtext, &msize, 1, &rim_is_inline)) != 0)
                     || ((newI1=recog_statement(line, i, "commit-transaction", &mtext, &msize, 0, &rim_is_inline)) != 0)) 
                    {
                        RIM_GUARD
                        i = newI+newI1;
                        char *opt = find_keyword (mtext, RIM_KEYOPTIONS, 1);
                        char *database = find_keyword (mtext, RIM_KEYAT, 0);
                        char *oncont = find_keyword (mtext, RIM_KEYONERRORCONTINUE, 1);
                        char *onexit = find_keyword (mtext, RIM_KEYONERROREXIT, 1);
                        char *errt = find_keyword (mtext, RIM_KEYERRORTEXT, 1);
                        char *err = find_keyword (mtext, RIM_KEYERROR, 1);
                        carve_statement (&opt, "commit-transaction", RIM_KEYOPTIONS, 0, 1);
                        carve_statement (&database, "commit-transaction", RIM_KEYAT, 0, 1);
                        carve_statement (&oncont, "commit-transaction", RIM_KEYONERRORCONTINUE, 0, 0);
                        carve_statement (&onexit, "commit-transaction", RIM_KEYONERROREXIT, 0, 0);
                        carve_statement (&errt, "commit-transaction", RIM_KEYERRORTEXT, 0, 1);
                        carve_statement (&err, "commit-transaction", RIM_KEYERROR, 0, 1);
                        carve_stmt_obj (&mtext, false, false);
                        make_mem (&opt);
                        // database is not a string constant or variable, though it's stringified internally, so no make_mem
                        define_statement (&errt, RIM_DEFSTRING, false); // allocated with strdup in db.c, exact length
                        define_statement (&err, RIM_DEFSTRING, false); // allocated with strdup in db.c, exact length
                        //
                        check_var (&opt, RIM_DEFSTRING);
                        //
                        get_db_config (database);
                        //erract is RIM_ON_ERROR_CONTINUE/EXIT for statement-specific on-error continue/exit or RIM_OKAY if db-level on-error is in effect
                        oprintf("rim_commit (\"%s\", %s, %s%s%s, %s%s%s);\n", opt==NULL?"":opt, oncont!=NULL?"RIM_ON_ERROR_CONTINUE":(onexit!=NULL?"RIM_ON_ERROR_EXIT":"RIM_OKAY"),
                            err!=NULL?"&(":"", err !=NULL?err:"NULL", err!=NULL?")":"", 
                            errt!=NULL?"&(":"", errt !=NULL?errt:"NULL", errt!=NULL?")":""
                            );
                        continue;
                    }
                    // Checking for non-mandatory (6th param is 1) MUST be done first.
                    else if (((newI=recog_statement(line, i, "begin-transaction", &mtext, &msize, 1, &rim_is_inline)) != 0) 
                        || ((newI1=recog_statement(line, i, "begin-transaction", &mtext, &msize, 0, &rim_is_inline)) != 0))
                    {
                        RIM_GUARD
                        i = newI+newI1;
                        char *database = find_keyword (mtext, RIM_KEYAT, 0);
                        char *opt = find_keyword (mtext, RIM_KEYOPTIONS, 1);
                        char *oncont = find_keyword (mtext, RIM_KEYONERRORCONTINUE, 1);
                        char *onexit = find_keyword (mtext, RIM_KEYONERROREXIT, 1);
                        char *errt = find_keyword (mtext, RIM_KEYERRORTEXT, 1);
                        char *err = find_keyword (mtext, RIM_KEYERROR, 1);
                        carve_statement (&opt, "begin-transaction", RIM_KEYOPTIONS, 0, 1);
                        carve_statement (&database, "begin-transaction", RIM_KEYAT, 0, 1);
                        carve_statement (&oncont, "begin-transaction", RIM_KEYONERRORCONTINUE, 0, 0);
                        carve_statement (&onexit, "begin-transaction", RIM_KEYONERROREXIT, 0, 0);
                        carve_statement (&errt, "begin-transaction", RIM_KEYERRORTEXT, 0, 1);
                        carve_statement (&err, "begin-transaction", RIM_KEYERROR, 0, 1);
                        carve_stmt_obj (&mtext, false, false);
                        make_mem (&opt);
                        // database is not a string constant or variable, though it's stringified internally, so no make_mem
                        define_statement (&errt, RIM_DEFSTRING, false); // allocated with strdup in db.c, exact length
                        define_statement (&err, RIM_DEFSTRING, false); // allocated with strdup in db.c, exact length
                        //
                        check_var (&opt, RIM_DEFSTRING);
                        //
                        get_db_config (database);
                        //erract is RIM_ON_ERROR_CONTINUE/EXIT for statement-specific on-error continue/exit or RIM_OKAY if db-level on-error is in effect
                        oprintf("rim_begin_transaction (\"%s\", %s, %s%s%s, %s%s%s);\n", opt==NULL?"":opt, oncont!=NULL?"RIM_ON_ERROR_CONTINUE":(onexit!=NULL?"RIM_ON_ERROR_EXIT":"RIM_OKAY"),
                            err!=NULL?"&(":"", err !=NULL?err:"NULL", err!=NULL?")":"", 
                            errt!=NULL?"&(":"", errt !=NULL?errt:"NULL", errt!=NULL?")":""
                            );
                        continue;
                    }
                    // Checking for non-mandatory (6th param is 1) MUST be done first.
                    else if (((newI1=recog_statement(line, i, "current-row", &mtext, &msize, 1, &rim_is_inline)) != 0)  // this is query row (number)
                         || (newI=recog_statement(line, i, "current-row", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI+newI1;
                        if (query_id == -1) rim_report_error( "Query usage attempted, but no query present and active");

                        if (gen_ctx->qry[query_id].returns_tuple == 0)
                        {
                            rim_report_error( "current-row cannot be used on a query because it does not output any columns");
                        }
                        char *to = NULL;
                        if (newI != 0)
                        {
                            to = find_keyword (mtext, RIM_KEYTO, 1);
                            carve_statement (&to, "current-row", RIM_KEYTO, 1, 1);
                            define_statement (&to, RIM_DEFNUMBER, false);
                        }
                        carve_stmt_obj (&mtext, false, false);

                        if (to != NULL)
                        {
                            // this happens right in the run-time, as rim_printf_noenc is going on
                            oprintf("%s = _rim_iter_%s+1;\n", to, gen_ctx->qry[query_id].name);
                        }
                        else
                        {
                            oprintf("rim_printf (false, RIM_NOENC, \"%%ld\", _rim_iter_%s+1);\n", gen_ctx->qry[query_id].name); 
                        }
                        
                        continue;
                    }
                    else if (((newI4=recog_statement(line, i, "run-prepared-query", &mtext, &msize, 0, &rim_is_inline)) != 0)
                        || ((newI8=recog_statement(line, i, "run-query", &mtext, &msize, 0, &rim_is_inline)) != 0))
                    {
                        RIM_GUARD
                        i = newI8+newI4;

                        //
                        // these are keywords needed to perform syntax highlighting, do NOT remove this comment!
                        // RIM_KEYNOLOOP RIM_KEYAFFECTEDROWS RIM_KEYROWCOUNT RIM_KEYERROR RIM_KEYERRORTEXT RIM_KEYONERRORCONTINUE RIM_KEYONERROREXIT RIM_KEYURLENCODE RIM_KEYWEBENCODE RIM_KEYNOENCODE
                        //

                        char is_prep = ((newI4 !=0) ? 1:0);

                        rim_db_parse vp;
                        memset  (&vp, 0, sizeof(rim_db_parse)); // all are NULL now
                        bool is_colon_subst = false;

                        //
                        // find keyword must be first
                        //

                        // run-query 
                        vp.on_error_cont = find_keyword (mtext, RIM_KEYONERRORCONTINUE, 1);
                        vp.on_error_exit = find_keyword (mtext, RIM_KEYONERROREXIT, 1);
                        vp.with_output = find_keyword (mtext, RIM_KEYOUTPUT, 1);
                        vp.colon = find_keyword (mtext, RIM_KEYCOLON, 0);
                        char *colon_subst = find_keyword (mtext, RIM_KEYINPUT, 1);
                        if (vp.colon != NULL)
                        {
                            if (colon_subst != NULL) rim_report_error("Cannot use both ':' and input clause, use one or the other");
                        }
                        else
                        {
                            if (colon_subst != NULL) 
                            {
                                vp.colon = colon_subst;
                                is_colon_subst = true;
                            }
                        }
                        vp.eq = find_keyword (mtext, RIM_KEYEQUALSHORT, 0);
                        vp.at = find_keyword (mtext, RIM_KEYAT, 0);
                        vp.noloop = find_keyword (mtext, RIM_KEYNOLOOP, 0);

                        // Stated as char* (the 4 below) so getvim for coloring will pick them up, expects char *....
                        char *err = find_keyword (mtext, RIM_KEYERROR, 1);
                        char *errtext = find_keyword (mtext, RIM_KEYERRORTEXT, 1);
                        char *arows = find_keyword (mtext, RIM_KEYAFFECTEDROWS, 1);
                        char *rcount = find_keyword (mtext, RIM_KEYROWCOUNT, 1);
                        // this commented out is just so getvim will color output - DO NOT REMOVE
                        //char *out = find_keyword (mtext, RIM_KEYOUTPUT, 1);
                        //


                        //
                        // Carving must be after find_keyword
                        //

                        // carve for run-query, match above defines
                        carve_statement (&(vp.on_error_cont), "run-query", RIM_KEYONERRORCONTINUE, 0, 0);
                        carve_statement (&(vp.on_error_exit), "run-query", RIM_KEYONERROREXIT, 0, 0);
                        carve_statement (&(vp.with_output), "run-query", RIM_KEYOUTPUT, 0, 1);
                        carve_statement (&(vp.colon), "run-query", is_colon_subst ? RIM_KEYINPUT : RIM_KEYCOLON, 0, 1);
                        carve_statement (&(vp.eq), "run-query", RIM_KEYEQUALSHORT, 1, 1);
                        carve_statement (&(vp.at), "run-query", RIM_KEYAT, 0, 1);
                        carve_statement (&(vp.noloop), "run-query", RIM_KEYNOLOOP, 0, 0);
                        carve_statement (&err, "run-query", RIM_KEYERROR, 0, 1);
                        carve_statement (&errtext, "run-query", RIM_KEYERRORTEXT, 0, 1);
                        carve_statement (&arows, "run-query", RIM_KEYAFFECTEDROWS, 0, 1);
                        carve_statement (&rcount, "run-query", RIM_KEYROWCOUNT, 0, 1);
                        carve_stmt_obj (&mtext, false, false);

                        make_mem (&(vp.eq));
                        //vp.at is not a string (we stringify it, but never subject to string processing), so no make_mem

                        //
                        // Define must be after carving
                        //

                        // run-query has output vars that can be defined
                        define_statement (&err, RIM_DEFSTRING, false); // allocated with strdup in db.c, exact length
                        define_statement (&errtext, RIM_DEFSTRING, false); // allocated with strdup in db.c, exact length
                        define_statement (&arows, RIM_DEFNUMBER, false);
                        define_statement (&rcount, RIM_DEFNUMBER, false);
                        //this define_statement MUST stay for coloring
                        //define_statement (&out, RIM_DEFSTRING, false);

                        // assign back to struct members
                        vp.err = err;
                        vp.errtext = errtext;
                        vp.arows = arows;
                        vp.rcount = rcount;

                        //
                        check_var (&(vp.eq), RIM_DEFSTRING);

                        // check on-error: can be only continue or exit
                        on_error_act (vp.on_error_cont, vp.on_error_exit, &(vp.on_error_action));

                        get_db_config (vp.at);

                        // now @db has been cutoff with 0 char  and
                        // also rim_get_config()->ctx.db->conn[rim_get_config()->ctx.db->ind_current_db].db_name is now correct

                        process_query (gen_ctx, 0, 1, is_prep, &vp);

                        continue;
                    }

                    // web-output line
                    else if (!memcmp(line+i, "@",1) || !memcmp(line+i, "!",1))
                    {
                        if (within_inline == 1)
                        {
                            rim_report_error("Cannot use output statements (such as @ or !) within an inline statement (<< ... >>)");
                        }
                        oprintf("//%s\n", line+i); // there can never be \n in line, so this should work
                        // @, !  is a synonym for an output line.  ! is verbatim, @ respects << ... >>
                        usedRIM = 0; // reset usedRIM because "@/!" isn't actual directive, it only
                                    // says what follows is just printed out
                        if (line[i+1] == 0)
                        {
                            // if empty, print empty line, and continue
                            oprintf("\n");

                        }
                        oprintf("rim_puts (RIM_NOENC, \""); // open the text line for free-text unencoded output
                        print_mode = 1;
                        if (line[i] == '!') is_verbatim = 1;
                        continue;
                    }
                    // Note: if 1 in 5th param in recog_statement, it means nothing afterwards, it must always be first!
                    else if ((newI2=recog_statement(line, i, "p-source-line", &mtext, &msize, 1, &rim_is_inline)) != 0 ||
                        (newI4=recog_statement(line, i, "p-source-file", &mtext, &msize, 1, &rim_is_inline)) != 0 ||
                        (newI3=recog_statement(line, i, "p-source-line", &mtext, &msize, 0, &rim_is_inline)) != 0 ||
                        (newI=recog_statement(line, i, "p-out", &mtext, &msize, 0, &rim_is_inline)) != 0 ||
                        (newI5=recog_statement(line, i, "p-source-file", &mtext, &msize, 0, &rim_is_inline)) != 0 ||
                        (newI6=recog_statement(line, i, "p-num", &mtext, &msize, 0, &rim_is_inline)) != 0 ||
                        (newI7=recog_statement(line, i, "p-url", &mtext, &msize, 0, &rim_is_inline)) != 0 ||
                        (newI8=recog_statement(line, i, "p-web", &mtext, &msize, 0, &rim_is_inline)) != 0)
                    {
                        RIM_GUARD
                        i = newI+newI2+newI3+newI4+newI5+newI6+newI7+newI8;
                        rim_report_warning ("p-* family of statements has been deprecated. Use print-out instead");
                        // RIM_KEYLENGTH RIM_KEYNEWLINE RIM_KEYSOURCEFILE RIM_KEYSOURCELINE RIM_KEYWEBENCODE RIM_KEYURLENCODE
                        if (newI != 0) do_print (mtext, 0);
                        else if (newI2 != 0 || newI3 != 0) do_print (mtext, RIM_P_SOURCELINE);
                        else if (newI4 != 0 || newI5 != 0) do_print (mtext, RIM_P_SOURCEFILE);
                        else if (newI6 != 0) do_print (mtext, RIM_P_NUM);
                        else if (newI7 != 0) do_print (mtext, RIM_P_URL);
                        else if (newI8 != 0) do_print (mtext, RIM_P_WEB);
                        continue;
                    }
                    else if ((newI1=recog_statement(line, i, "pf-web", &mtext, &msize, 0, &rim_is_inline)) != 0 ||
                        (newI2=recog_statement(line, i, "pf-url", &mtext, &msize, 0, &rim_is_inline)) != 0 ||
                        (newI=recog_statement(line, i, "pf-out", &mtext, &msize, 0, &rim_is_inline)) != 0)
                    {
                        RIM_GUARD
                        i = newI+newI2+newI1;
                        // RIM_KEYCOMMA RIM_KEYTOERROR RIM_KEYTO RIM_KEYWEBENCODE RIM_KEYURLENCODE
                        rim_report_warning ("pf-* family of statements has been deprecated. Use print-format instead");
                        if (newI1 != 0) do_printf (mtext, false, gen_ctx, RIM_P_WEB);
                        else if (newI2 != 0) do_printf (mtext, false, gen_ctx, RIM_P_URL);
                        else if (newI != 0) do_printf (mtext, false, gen_ctx, 0);
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "print-path", &mtext, &msize, 0, &rim_is_inline)) != 0 ||
                        (newI1=recog_statement(line, i, "p-path", &mtext, &msize, 0, &rim_is_inline)))
                    {
                        RIM_GUARD
                        i = newI+newI1;
                        char *nl = find_keyword (mtext, RIM_KEYNEWLINE, 1); // can be used with any other clause
                        carve_statement (&nl, "print-path", RIM_KEYNEWLINE, 0, 0);
                        check_sub (mtext); // this must be BEFORE carve_stmt_obj as it may create temp variable which is just some generated var name
                                           // it trims mtext, but we're about to do that anyway (but this must be after all other carve-outs)
                        carve_stmt_obj (&mtext, true, true);

                        // mtext is mandatory request path
                        make_mem(&mtext);
                        check_var (&mtext, RIM_DEFSTRING);

                        oprintf("rim_puts (RIM_WEB, rim_app_path, rim_app_path_len, false);\n"); // optimized to compute strlen at compile time
                        oprintf("rim_puts (RIM_WEB, %s, rim_mem_get_len(%s), false);\n", mtext, mtext); // optimized to compute strlen at compile time
                        if (nl != NULL) oprintf("rim_puts (RIM_NOENC, \"\\n\", 1, false);\n"); // output 1 byte non-alloc'd

                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "print-out", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;
                        // RIM_KEYLENGTH RIM_KEYNEWLINE RIM_KEYSOURCELINE RIM_KEYSOURCEFILE RIM_KEYWEBENCODE RIM_KEYURLENCODE
                        while (1) 
                        {
                            char *comma = find_keyword (mtext, RIM_KEYCOMMA, 0);
                            carve_statement (&comma, "print-out", RIM_KEYCOMMA, 0, 1); // separate entries with comma first, so
                                                                                      // this find/carve MUST come before the rest below!
                                                                                      //
                            do_print (mtext, 0);

                            if (comma == NULL) break; else mtext = comma;
                        }
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "print-format", &mtext, &msize, 0, &rim_is_inline)) != 0 ||
                            (newI3=recog_statement(line, i, "report-error", &mtext, &msize, 0, &rim_is_inline)) != 0
                        )  
                    {
                        RIM_GUARD
                        i = newI+newI3;
                        bool rep_error;
                        // RIM_KEYCOMMA RIM_KEYTOERROR RIM_KEYTO RIM_KEYWEBENCODE RIM_KEYURLENCODE
                        if (newI3 != 0) rep_error = true; else rep_error = false;
                        do_printf (mtext, rep_error, gen_ctx, 0);
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "exit-status", &mtext, &msize, 0, &rim_is_inline)) != 0 )
                    {
                        RIM_GUARD
                        i = newI;
                        carve_stmt_obj (&mtext, true, true);
                        check_var (&mtext, RIM_DEFNUMBER);
                        oprintf("rim_req.ec=(%s);\n", mtext);

                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "silent-header", &mtext, &msize, 1, &rim_is_inline)) != 0 ) 
                    {
                        RIM_GUARD
                        i = newI;
                        oprintf("rim_req.silent=1;\n");

                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "out-header", &mtext, &msize, 0, &rim_is_inline)) != 0 ) 
                    {
                        RIM_GUARD
                        i = newI;
                        char *use_default = find_keyword (mtext, RIM_KEYDEFAULT, 1);
                        char *use = find_keyword (mtext, RIM_KEYUSE, 1);
                        carve_statement (&use, "out-header", RIM_KEYUSE, 0, 1);
                        carve_statement (&use_default, "out-header", RIM_KEYDEFAULT, 0, 0);
                        carve_stmt_obj (&mtext, false, false);

                        if (use == NULL && use_default == NULL) rim_report_error( "Must use either 'use' or 'default' to specify header in out-header");
                        if (use != NULL && use_default != NULL) rim_report_error( "Cannot use both 'use' or 'default' in out-header");
                        if (use != NULL) 
                        {
                            //RIM_HEADER_PAGE because output from a page isn't cached, it's dynamic
                            char *temp_header = make_default_header(RIM_HEADER_PAGE, http_header_count, 0);
                            // RIM_KEY* must be specified, even as a header, for getvim to pick up keywords for highlighting!
                            process_http_header ("out-header", use, temp_header, http_header_count, 0, NULL, NULL); // RIM_KEYCONTENTTYPE RIM_KEYDOWNLOAD RIM_KEYETAG RIM_KEYFILENAME RIM_KEYCACHECONTROL RIM_KEYNOCACHE RIM_KEYSTATUSID RIM_KEYSTATUSTEXT RIM_KEYCUSTOM
                            oprintf("rim_req.header=&(%s);\n", temp_header);
                            http_header_count++;
                        }
                        oprintf("rim_output_http_header(&rim_req);\n");

                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "exec-program", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;
                        

                        // Get keywords (if used)
                        char *env = find_keyword (mtext, RIM_KEYENVIRONMENT, 1);
                        char *program_args = find_keyword (mtext, RIM_KEYARGS, 1);
                        char *program_status = find_keyword (mtext, RIM_KEYSTATUS, 1);
                        char *program_error = find_keyword (mtext, RIM_KEYERROR, 1);
                        char *program_output = find_keyword (mtext, RIM_KEYOUTPUT, 1);
                        char *program_input = find_keyword (mtext, RIM_KEYINPUT, 1);
                        char *program_input_length = find_keyword (mtext, RIM_KEYINPUTLENGTH, 1);
                        char *out_file = find_keyword (mtext, RIM_KEYOUTPUTFILE, 1);
                        char *err_file = find_keyword (mtext, RIM_KEYERRORFILE, 1);
                        char *in_file = find_keyword (mtext, RIM_KEYINPUTFILE, 1);

                        carve_statement (&env, "exec-program", RIM_KEYENVIRONMENT, 0, 1);
                        carve_statement (&program_args, "exec-program", RIM_KEYARGS, 0, 1);
                        carve_statement (&program_status, "exec-program", RIM_KEYSTATUS, 0, 1);
                        carve_statement (&program_output, "exec-program", RIM_KEYOUTPUT, 0, 1);
                        carve_statement (&program_error, "exec-program", RIM_KEYERROR, 0, 1);
                        carve_statement (&program_input, "exec-program", RIM_KEYINPUT, 0, 1);
                        carve_statement (&program_input_length, "exec-program", RIM_KEYINPUTLENGTH, 0, 1);
                        carve_statement (&out_file, "exec-program", RIM_KEYOUTPUTFILE, 0, 1);
                        carve_statement (&err_file, "exec-program", RIM_KEYERRORFILE, 0, 1);
                        carve_statement (&in_file, "exec-program", RIM_KEYINPUTFILE, 0, 1);
                        carve_stmt_obj (&mtext, true, true);



                        define_statement (&program_output, RIM_DEFSTRING, false); // exact length set in read_child in rimrt.c
                        define_statement (&program_error, RIM_DEFSTRING, false); // exact length set in read_child in rimrt.c
                        define_statement (&program_status, RIM_DEFNUMBER, false);

                        make_mem(&program_input);
                        make_mem(&out_file);
                        make_mem(&err_file);
                        make_mem(&in_file);

                        //
                        check_var (&mtext, RIM_DEFSTRING);
                        check_var (&program_input, RIM_DEFSTRING);
                        check_var (&program_input_length, RIM_DEFNUMBER);
                        check_var (&out_file, RIM_DEFSTRING);
                        check_var (&err_file, RIM_DEFSTRING);
                        check_var (&in_file, RIM_DEFSTRING);

                        // cannot have output-* and output (to string) at the same time
                        if (out_file != NULL && program_output != NULL) rim_report_error( "Specify either output-file or output in exec-program statement");
                        if (err_file != NULL && program_error != NULL) rim_report_error( "Specify either error-file or error in exec-program statement");
                        if (in_file != NULL && program_input != NULL) rim_report_error( "Specify either input-file or input in exec-program statement");
                        if (program_input == NULL && program_input_length != NULL) rim_report_error( "Cannot use input-length without 'input' in exec-program statement");
                        // setup any additional environment variables
                        char env_var[100];
                        if (env != NULL) 
                        {
                            snprintf (env_var, sizeof (env_var), "_rim_exec_env%ld", total_exec_programs);
                            outargs(env, env_var, "char *", 0, 1, RIM_DEFSTRING, RIM_DEFSTRING);
                        }


                        oprintf ("rim_num _rim_prg_status%ld = 0;\n", total_exec_programs);
                        oprintf("RIM_UNUSED (_rim_prg_status%ld);\n", total_exec_programs); // if status not used by the developer
                        oprintf ("FILE *_rim_prg_out_file%ld = NULL;\nRIM_UNUSED(_rim_prg_out_file%ld);\n", total_exec_programs, total_exec_programs);
                        oprintf ("FILE *_rim_prg_err_file%ld = NULL;\nRIM_UNUSED(_rim_prg_err_file%ld);\n", total_exec_programs, total_exec_programs);
                        if (err_file != NULL)
                        {
                            // this is error-file. 
                            oprintf ("_rim_prg_err_file%ld = rim_fopen1 ((%s), \"w+\");\n", total_exec_programs, err_file);
                            // we set status to -8 if cannot open for writing
                            //RIM_ERR is set in rim_fopen
                            oprintf ("if (_rim_prg_err_file%ld == NULL) {_rim_prg_status%ld=RIM_ERR_WRITE;} else { \n", total_exec_programs, total_exec_programs);
                        }
                        if (out_file != NULL)
                        {
                            // this is output-file. We open file for writing, and we close it after we're done
                            oprintf ("_rim_prg_out_file%ld = rim_fopen1 ((%s), \"w+\");\n", total_exec_programs, out_file);
                            // we set status to -8 if cannot open for writing
                            //RIM_ERR is set in rim_fopen
                            oprintf ("if (_rim_prg_out_file%ld == NULL) {_rim_prg_status%ld=RIM_ERR_WRITE;} else { \n", total_exec_programs, total_exec_programs);
                        }

                        oprintf ("FILE *_rim_prg_in_file%ld = NULL;\nRIM_UNUSED(_rim_prg_in_file%ld);\n", total_exec_programs, total_exec_programs);
                        if (in_file != NULL)
                        {
                            // this is input-file. We open it for reading, and close after we're done
                            oprintf ("_rim_prg_in_file%ld = rim_fopen1 ((%s), \"r\");\n", total_exec_programs, in_file);
                            // for status, we set it to -9 if cannot read
                            //RIM_ERR is set in rim_fopen
                            oprintf ("if (_rim_prg_in_file%ld == NULL) {_rim_prg_status%ld=RIM_ERR_READ;} else {\n", total_exec_programs, total_exec_programs);
                        }



                        // store exec-program args arguments in an array
                        char prog_var[100];
                        snprintf (prog_var, sizeof(prog_var), "_rim_prg_arr%ld", total_exec_programs);
                        rim_num exec_inputs = outargs(program_args, prog_var, "char *", 1, 0, RIM_DEFSTRING, RIM_DEFSTRING);

                        // generate run-time call to execute program
                        // exec_inputs is always at least 1 (to account for args[0] being program name)
                        // this is output (i.e. string output)
                        oprintf ("_rim_st = rim_exec_program(%s, _rim_prg_arr%ld, %ld, _rim_prg_in_file%ld, &(_rim_prg_out_file%ld), &(_rim_prg_err_file%ld), %s, %s, %s%s%s, %s%s%s, %s);\n",
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
                        program_error==NULL ? "":")",
                        env==NULL?"NULL":env_var);
                        if (program_status!=NULL) oprintf("%s=_rim_st;\n", program_status);
                        else oprintf ("if (_rim_st != RIM_OKAY) rim_report_error (\"%s\", \"%s\", (rim_num)%ld, _rim_st);\n", RIM_STMT_FAIL_CHECK, rim_valid_rim_name, lnum);
                        if (err_file != NULL) oprintf ("}\n");
                        if (out_file != NULL) oprintf ("}\n");
                        if (in_file != NULL) oprintf ("}\n");

                        total_exec_programs++; // advance exec-program counter so generating specific variables is unique
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "send-file", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;

                        //
                        // Look for each option and collect relevant info
                        //
                        char *header = find_keyword (mtext, RIM_KEYHEADERS, 1);
                        carve_statement (&header, "send-file", RIM_KEYHEADERS, 0, 1);
                        carve_stmt_obj (&mtext, true, true);
                        //
                        check_var (&mtext, RIM_DEFSTRING);

                        // if there's no header, by default:
                        char *temp_header = make_default_header(RIM_HEADER_FILE, http_header_count, 0);
                        oprintf ("(%s).file_name = basename(%s);\n", temp_header, mtext);

                        // specify keywords used in function for syntax highlighting to work properly
                        process_http_header ("send-file", header, temp_header, http_header_count, 0, NULL, NULL); // RIM_KEYCONTENTTYPE RIM_KEYDOWNLOAD RIM_KEYETAG RIM_KEYFILENAME RIM_KEYCACHECONTROL RIM_KEYNOCACHE RIM_KEYSTATUSID RIM_KEYSTATUSTEXT RIM_KEYCUSTOM
                        http_header_count++;


                        oprintf ("rim_out_file(%s, &(%s));\n", mtext, temp_header);


                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "lock-file", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;
                        // Example:
                        // lock-file "/home/rim/files" id [define] x status [define] st
                        // later
                        // unlock-file x
                        char *id = find_keyword (mtext, RIM_KEYID, 1);
                        char *status = find_keyword (mtext, RIM_KEYSTATUS, 1);

                        carve_statement (&id, "lock-file", RIM_KEYID, 1, 1);
                        carve_statement (&status, "lock-file", RIM_KEYSTATUS, 0, 1);
                        carve_stmt_obj (&mtext, true, true);

                        define_statement (&id, RIM_DEFFILE, false);
                        define_statement (&status, RIM_DEFNUMBER, false);
                        //
                        check_var (&mtext, RIM_DEFSTRING);

                        oprintf("%s = rim_calloc (1, sizeof(rim_file)); (%s)->attr.id = -1; \n", id, id); // set to -1 (it's file id)
                                                                                                        // this memory released at the end of request
                        oprintf ("_rim_st=rim_lockfile (%s, &((%s)->attr.id));\n", mtext, id);
                        if (status != NULL) oprintf ("%s=_rim_st;\n", status);
                        else oprintf("if (_rim_st != RIM_OKAY) rim_report_error (\"%s\", \"%s\", (rim_num)%ld, _rim_st);}\n", RIM_STMT_FAIL_CHECK, rim_valid_rim_name, lnum);

                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "unlock-file", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;
                        // Example:
                        // unlock-file id x
                        char *id = find_keyword (mtext, RIM_KEYID, 1);
                        carve_statement (&id, "lock-file", RIM_KEYID, 1, 1);
                        carve_stmt_obj (&mtext, false, false);

                        check_var (&id, RIM_DEFFILE);

                        // closes and assigns -1 to ID, so it can't be closed again (or if not open somehow)
                        oprintf ("if ((%s)->attr.id != -1) {close((%s)->attr.id); (%s)->attr.id=-1;}\n", id, id, id);

                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "random-string", &mtext, &msize, 0, &rim_is_inline)) != 0)
                    {
                        RIM_GUARD
                        i = newI;
                        char *to = find_keyword (mtext, RIM_KEYTO, 1);
                        char *length = find_keyword (mtext, RIM_KEYLENGTH, 1);
                        char *number = find_keyword (mtext, RIM_KEYNUMBER, 1);
                        char *binary = find_keyword (mtext, RIM_KEYBINARY, 1);

                        carve_statement (&to, "random-string", RIM_KEYTO, 1, 1);
                        carve_statement (&length, "random-string", RIM_KEYLENGTH, 0, 1);
                        carve_statement (&number, "random-string", RIM_KEYNUMBER, 0, 0);
                        carve_statement (&binary, "random-string", RIM_KEYBINARY, 0, 0);
                        carve_stmt_obj (&mtext, false, false);

                        char type;
                        if (number != NULL) type = RIM_RANDOM_NUM;
                        else if (binary != NULL) type = RIM_RANDOM_BIN;
                        else type = RIM_RANDOM_STR;

                        if (number != NULL && binary != NULL) rim_report_error( "cannot use both 'number' and 'binary' in random-string statement");

                        define_statement (&to, RIM_DEFSTRING, false); // exact length set in rim_make_random
                        //
                        check_var (&length, RIM_DEFNUMBER);

                        if (length == NULL)
                        {
                            length="20";
                        }

                        oprintf ("rim_make_random (&(%s), (%s)+1, %d, false);\n", to, length, type);


                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "random-crypto", &mtext, &msize, 0, &rim_is_inline)) != 0)
                    {
                        RIM_GUARD
                        rim_mod_crypto = true;
                        i = newI;
                        char *to = find_keyword (mtext, RIM_KEYTO, 1);
                        char *length = find_keyword (mtext, RIM_KEYLENGTH, 1);

                        carve_statement (&to, "random-string", RIM_KEYTO, 1, 1);
                        carve_statement (&length, "random-string", RIM_KEYLENGTH, 0, 1);
                        carve_stmt_obj (&mtext, false, false);

                        define_statement (&to, RIM_DEFSTRING, false); // exact length set in rim_make_random
                        //
                        check_var (&length, RIM_DEFNUMBER);

                        if (length == NULL)
                        {
                            length="20";
                        }

                        oprintf ("rim_make_random (&(%s), (%s)+1, RIM_RANDOM_BIN, true);\n", to, length);


                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "upper-string", &mtext, &msize, 0, &rim_is_inline)) != 0)
                    {
                        RIM_GUARD
                        i = newI;
                        // Example:
                        // upper-string str

                        oprintf ("rim_upper (%s);\n", mtext);


                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "lower-string", &mtext, &msize, 0, &rim_is_inline)) != 0)
                    {
                        RIM_GUARD
                        i = newI;
                        // Example:
                        // lower-string str

                        oprintf ("rim_lower (%s);\n", mtext);


                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "string-length", &mtext, &msize, 0, &rim_is_inline)) != 0)
                    {
                        RIM_GUARD
                        i = newI;

                        char *to = find_keyword (mtext, RIM_KEYTO, 1);

                        carve_statement (&to, "string-length", RIM_KEYTO, 0, 1);
                        carve_stmt_obj (&mtext, true, true);

                        define_statement (&to, RIM_DEFNUMBER, false);
                        //
                        check_var (&mtext, RIM_DEFSTRING);

                        if (to != NULL) oprintf ("%s=(rim_num)rim_mem_get_len (%s);\n", to, mtext);
                        else 
                        {
                            oprintf("{rim_num _rim_slen=rim_mem_get_len(%s);\n", mtext);
                            do_numstr (NULL, "_rim_slen", NULL, false, -1, -1, NULL);
                            oprintf("}\n");
                        }

                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "count-substring", &mtext, &msize, 0, &rim_is_inline)) != 0)
                    {
                        RIM_GUARD
                        i = newI;

                        char *to = find_keyword (mtext, RIM_KEYTO, 1);
                        char *in = find_keyword (mtext, RIM_KEYIN, 1);
                        char *case_insensitive = find_keyword (mtext, RIM_KEYCASEINSENSITIVE, 1);

                        carve_statement (&to, "count-substring", RIM_KEYTO, 1, 1);
                        carve_statement (&in, "count-substring", RIM_KEYIN, 1, 1);
                        carve_statement (&case_insensitive, "count-substring", RIM_KEYCASEINSENSITIVE, 0, 2);
                        carve_stmt_obj (&mtext, true, true);
                        make_mem (&in);

                        char *case_insensitivec = opt_clause(case_insensitive, "0", "1", RIM_DEFNUMBER);

                        define_statement (&to, RIM_DEFNUMBER, false);
                        //
                        check_var (&mtext, RIM_DEFSTRING);
                        check_var (&in, RIM_DEFSTRING);
                        check_var (&case_insensitivec, RIM_DEFNUMBER);

                        oprintf ("%s=rim_count_substring (%s, %s, 0, %s);\n", to, in, mtext, case_insensitivec);
                        rim_free(case_insensitivec);


                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "derive-key", &mtext, &msize, 0, &rim_is_inline)) != 0)
                    {
                        RIM_GUARD
                        i = newI;

                        char *from = find_keyword (mtext, RIM_KEYFROM, 1);
                        char *fromlen = find_keyword (mtext, RIM_KEYFROMLENGTH, 1);
                        char *digest = find_keyword (mtext, RIM_KEYDIGEST, 1);
                        char *salt = find_keyword (mtext, RIM_KEYSALT, 1);
                        char *salt_len = find_keyword (mtext, RIM_KEYSALTLENGTH, 1);
                        char *iterations = find_keyword (mtext, RIM_KEYITERATIONS, 1);
                        char *length = find_keyword (mtext, RIM_KEYLENGTH, 1);
                        char *binary = find_keyword (mtext, RIM_KEYBINARY, 1);

                        carve_statement (&length, "derive-key", RIM_KEYLENGTH, 1, 1);
                        carve_statement (&from, "derive-key", RIM_KEYFROM, 1, 1);
                        carve_statement (&fromlen, "derive-key", RIM_KEYFROMLENGTH, 0, 1);
                        carve_statement (&digest, "derive-key", RIM_KEYDIGEST, 0, 1);
                        carve_statement (&salt, "derive-key", RIM_KEYSALT, 0, 1);
                        carve_statement (&salt_len, "derive-key" , RIM_KEYSALTLENGTH, 0, 1);
                        carve_statement (&iterations, "derive-key", RIM_KEYITERATIONS, 0, 1);
                        carve_statement (&binary, "derive-key", RIM_KEYBINARY, 0, 2);
                        carve_stmt_obj (&mtext, true, true);

                        make_mem(&from);
                        make_mem(&digest);
                        make_mem(&salt);

                        define_statement (&mtext, RIM_DEFSTRING, false); // exact length set in rim_derive_key
                        char *binaryc = opt_clause(binary, "true", "false", RIM_DEFBOOL);
                        //
                        check_var (&length, RIM_DEFNUMBER);
                        check_var (&from, RIM_DEFSTRING);
                        check_var (&fromlen, RIM_DEFNUMBER);
                        check_var (&digest, RIM_DEFSTRING);
                        check_var (&salt, RIM_DEFSTRING);
                        check_var (&salt_len, RIM_DEFNUMBER);
                        check_var (&iterations, RIM_DEFNUMBER);
                        check_var (&binaryc, RIM_DEFBOOL);

                        oprintf ("%s=rim_derive_key( %s, %s, %s, %s, %s, %s, %s, %s );\n", mtext, from, fromlen==NULL?"-1":fromlen, digest==NULL?RIM_DEF_DIGEST:digest, iterations==NULL?"-1":iterations, salt==NULL?"NULL":salt, salt_len==NULL?"0":salt_len, length, binaryc);
                        rim_free(binaryc);


                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "hmac-string", &mtext, &msize, 0, &rim_is_inline)) != 0)
                    {
                        RIM_GUARD
                        i = newI;
                        rim_mod_crypto = true;

                        char *key = find_keyword (mtext, RIM_KEYKEY, 1);
                        char *to = find_keyword (mtext, RIM_KEYTO, 1);
                        char *digest = find_keyword (mtext, RIM_KEYDIGEST, 1);
                        char *binary = find_keyword (mtext, RIM_KEYBINARY, 1);

                        carve_statement (&key, "hmac-string", RIM_KEYKEY, 1, 1);
                        carve_statement (&to, "hmac-string", RIM_KEYTO, 1, 1);
                        carve_statement (&digest, "hmac-string", RIM_KEYDIGEST, 0, 1);
                        carve_statement (&binary, "hmac-string", RIM_KEYBINARY, 0, 2);
                        carve_stmt_obj (&mtext, true, true);
                        make_mem (&digest);
                        make_mem (&key);

                        define_statement (&to, RIM_DEFSTRING, false); // exact length set in rim_hash_data
                        char *binaryc = opt_clause(binary, "true", "false", RIM_DEFBOOL);
                        //
                        check_var (&mtext, RIM_DEFSTRING);
                        check_var (&key, RIM_DEFSTRING);
                        check_var (&digest, RIM_DEFSTRING);
                        check_var (&binaryc, RIM_DEFBOOL);

                        oprintf ("%s=rim_hmac (%s, %s, %s, %s);\n", to, key, mtext, digest==NULL?RIM_DEF_DIGEST:digest, binaryc);
                        rim_free(binaryc);



                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "hash-string", &mtext, &msize, 0, &rim_is_inline)) != 0)
                    {
                        RIM_GUARD
                        i = newI;
                        rim_mod_crypto = true;

                        char *to = find_keyword (mtext, RIM_KEYTO, 1);
                        char *digest = find_keyword (mtext, RIM_KEYDIGEST, 1);
                        char *binary = find_keyword (mtext, RIM_KEYBINARY, 1);

                        carve_statement (&to, "hash-string", RIM_KEYTO, 1, 1);
                        carve_statement (&digest, "hash-string", RIM_KEYDIGEST, 0, 1);
                        carve_statement (&binary, "hash-string", RIM_KEYBINARY, 0, 2);
                        carve_stmt_obj (&mtext, true, true);
                        make_mem (&digest);

                        define_statement (&to, RIM_DEFSTRING, false); // exact length set in rim_hash_data
                        char *binaryc = opt_clause(binary, "true", "false", RIM_DEFBOOL);
                        //
                        check_var (&mtext, RIM_DEFSTRING);
                        check_var (&digest, RIM_DEFSTRING);
                        check_var (&binaryc, RIM_DEFBOOL);

                        oprintf ("%s=rim_hash_data (%s, %s, %s);\n", to, mtext, digest==NULL?RIM_DEF_DIGEST:digest, binaryc);
                        rim_free(binaryc);



                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "decode-base64", &mtext, &msize, 0, &rim_is_inline)) != 0)
                    {
                        RIM_GUARD
                        i = newI;
                        rim_mod_crypto = true;

                        char *ilen = find_keyword (mtext, RIM_KEYINPUTLENGTH, 1);
                        char *to = find_keyword (mtext, RIM_KEYTO, 1);

                        carve_statement (&ilen, "decode-base64", RIM_KEYINPUTLENGTH, 0, 1);
                        carve_statement (&to, "decode-base64", RIM_KEYTO, 1, 1);
                        carve_stmt_obj (&mtext, true, true);

                        define_statement (&to, RIM_DEFSTRING, false); // exact length set in rim_b64_decode
                        //
                        check_var (&mtext, RIM_DEFSTRING);
                        check_var (&ilen, RIM_DEFNUMBER);

                        oprintf ("rim_b64_decode ((char*)(%s), %s, &((%s)));\n", mtext, ilen==NULL?"-1":ilen,to);



                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "encode-base64", &mtext, &msize, 0, &rim_is_inline)) != 0)
                    {
                        RIM_GUARD
                        rim_mod_crypto = true;
                        i = newI;

                        char *ilen = find_keyword (mtext, RIM_KEYINPUTLENGTH, 1);
                        char *to = find_keyword (mtext, RIM_KEYTO, 1);

                        carve_statement (&ilen, "encode-base64", RIM_KEYINPUTLENGTH, 0, 1);
                        carve_statement (&to, "encode-base64", RIM_KEYTO, 1, 1);
                        carve_stmt_obj (&mtext, true, true);

                        define_statement (&to, RIM_DEFSTRING, false); // exact length set in rim_b64_encode
                        //
                        check_var (&mtext, RIM_DEFSTRING);
                        check_var (&ilen, RIM_DEFNUMBER);

                        oprintf ("rim_b64_encode ((char*)(%s), %s, &(%s));\n", mtext, ilen==NULL?"-1":ilen,to);



                        continue;
                    }

                    else if ((newI=recog_statement(line, i, "encode-hex", &mtext, &msize, 0, &rim_is_inline)) != 0)
                    {
                        RIM_GUARD
                        i = newI;

                        char *ilen = find_keyword (mtext, RIM_KEYINPUTLENGTH, 1);
                        char *pref = find_keyword (mtext, RIM_KEYPREFIX, 1);
                        char *to = find_keyword (mtext, RIM_KEYTO, 1);

                        carve_statement (&ilen, "encode-hex", RIM_KEYINPUTLENGTH, 0, 1);
                        carve_statement (&pref, "encode-hex", RIM_KEYPREFIX, 0, 1);
                        carve_statement (&to, "encode-hex", RIM_KEYTO, 1, 1);
                        carve_stmt_obj (&mtext, true, true);

                        make_mem(&pref);

                        define_statement (&to, RIM_DEFSTRING, false); // exact length set in rim_bin2hex
                        //
                        check_var (&mtext, RIM_DEFSTRING);
                        check_var (&ilen, RIM_DEFNUMBER);
                        check_var (&pref, RIM_DEFSTRING);

                        oprintf ("rim_bin2hex((char*)(%s), &(%s), %s, %s);\n", mtext, to, ilen==NULL?"-1":ilen, pref==NULL?"NULL":pref);

                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "decode-hex", &mtext, &msize, 0, &rim_is_inline)) != 0)
                    {
                        RIM_GUARD
                        i = newI;

                        char *ilen = find_keyword (mtext, RIM_KEYINPUTLENGTH, 1);
                        char *to = find_keyword (mtext, RIM_KEYTO, 1);

                        carve_statement (&ilen, "decode-hex", RIM_KEYINPUTLENGTH, 0, 1);
                        carve_statement (&to, "decode-hex", RIM_KEYTO, 1, 1);
                        carve_stmt_obj (&mtext, true, true);

                        define_statement (&to, RIM_DEFSTRING, false); // exact length set in rim_hex2bin
                        //
                        check_var (&mtext, RIM_DEFSTRING);
                        check_var (&ilen, RIM_DEFNUMBER);

                        oprintf ("rim_hex2bin ((char*)(%s), &(%s), %s);\n", mtext, to, ilen==NULL?"-1":ilen);

                        continue;
                    }

                    else if ((newI=recog_statement(line, i, "decode-url", &mtext, &msize, 0, &rim_is_inline)) != 0 ||
                     (newI1=recog_statement(line, i, "decode-web", &mtext, &msize, 0, &rim_is_inline)) != 0)
                    {
                        RIM_GUARD
                        i = newI+newI1;

                        rim_num is_web=(newI1 != 0 ? 1:0);
                        char *inlength = find_keyword (mtext, RIM_KEYINPUTLENGTH, 1);
                        char *status = NULL;
                        if (newI != 0) status = find_keyword (mtext, RIM_KEYSTATUS, 1); // only for decode-url

                        carve_statement (&inlength, is_web==1?"decode-web":"decode-url", RIM_KEYINPUTLENGTH, 0, 1);
                        if (newI != 0) carve_statement (&status, "decode-url", RIM_KEYSTATUS, 0, 1);
                        carve_stmt_obj (&mtext, true, true);
                        if (newI != 0) define_statement (&status, RIM_DEFNUMBER, false);

                        //
                        check_var (&mtext, RIM_DEFSTRING);
                        check_var (&inlength, RIM_DEFNUMBER);

                        if (inlength == NULL) inlength="-1";

                        oprintf ("rim_decode (%s, %s, %s, true, &_rim_st);\n",is_web==1?"RIM_WEB":"RIM_URL", mtext, inlength);
                        if (newI != 0)
                        {
                            if (status != NULL) oprintf("%s=_rim_st;\n", status);
                            else oprintf("if (_rim_st != RIM_OKAY) rim_report_error (\"%s\", \"%s\", (rim_num)%ld, _rim_st);\n", RIM_STMT_FAIL_CHECK, rim_valid_rim_name, lnum);
                        }


                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "encode-url", &mtext, &msize, 0, &rim_is_inline)) != 0 ||
                     (newI1=recog_statement(line, i, "encode-web", &mtext, &msize, 0, &rim_is_inline)) != 0)
                    {
                        RIM_GUARD
                        i = newI+newI1;

                        rim_num is_web=(newI1 != 0 ? 1:0);
                        char *inlength = find_keyword (mtext, RIM_KEYINPUTLENGTH, 1);
                        char *to = find_keyword (mtext, RIM_KEYTO, 1);

                        carve_statement (&inlength, is_web==1?"encode-web":"encode-url", RIM_KEYINPUTLENGTH, 0, 1);
                        carve_statement (&to, is_web==1?"encode-web":"encode-url", RIM_KEYTO, 1, 1);
                        carve_stmt_obj (&mtext, true, true);

                        define_statement (&to, RIM_DEFSTRING, false); // exact length set in rimstone encode
                        //
                        check_var (&mtext, RIM_DEFSTRING);
                        check_var (&inlength, RIM_DEFNUMBER);

                        if (inlength == NULL) inlength = "-1";

                        oprintf ("rim_encode (%s, %s, %s, &(%s), true);\n", is_web==1?"RIM_WEB":"RIM_URL", mtext, inlength, to);



                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "trim-string", &mtext, &msize, 0, &rim_is_inline)) != 0)
                    {
                        RIM_GUARD
                        i = newI;
                        // Example:
                        // trim-string str 

                        carve_stmt_obj (&mtext, true, true);

                        //
                        check_var (&mtext, RIM_DEFSTRING);

                        oprintf ("rim_num _rim_trim_len%ld=rim_mem_get_len (%s);\n", trim_count, mtext);
                        oprintf ("rim_trim (%s, &_rim_trim_len%ld, true);\n", mtext, trim_count);
                        trim_count++;


                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "encrypt-data", &mtext, &msize, 0, &rim_is_inline)) != 0 ||
                        (newI1=recog_statement(line, i, "decrypt-data", &mtext, &msize, 0, &rim_is_inline)) != 0)
                    {
                        RIM_GUARD
                        rim_mod_crypto = true;
                        i = newI+newI1;
                        bool enc = (newI!=0); // is it encrypt (otherwise decrypt)

                        // Example:
                        // encrypt-data data [input-length l] output-length [define] ol password p [salt s] to [define] res [binary]

                        char *password = find_keyword (mtext, RIM_KEYPASSWORD, 1);
                        char *inlength = find_keyword (mtext, RIM_KEYINPUTLENGTH, 1);
                        char *salt = find_keyword (mtext, RIM_KEYSALT, 1);
                        char *salt_len = find_keyword (mtext, RIM_KEYSALTLENGTH, 1);
                        char *iv = find_keyword (mtext, RIM_KEYINITVECTOR, 1);
                        char *iterations = find_keyword (mtext, RIM_KEYITERATIONS, 1);
                        char *to = find_keyword (mtext, RIM_KEYTO, 1);
                        char *binary = find_keyword (mtext, RIM_KEYBINARY, 1);
                        char *cipher = find_keyword (mtext, RIM_KEYCIPHER, 1);
                        char *digest = find_keyword (mtext, RIM_KEYDIGEST, 1);
                        char *cache = find_keyword (mtext, RIM_KEYCACHE, 1);
                        char *ccache = find_keyword (mtext, RIM_KEYCLEARCACHE, 1);

                        carve_statement (&password, enc?"encrypt-data":"decrypt-data", RIM_KEYPASSWORD, 1, 1);
                        carve_statement (&inlength, enc?"encrypt-data":"decrypt-data" , RIM_KEYINPUTLENGTH, 0, 1);
                        carve_statement (&iv, enc?"encrypt-data":"decrypt-data", RIM_KEYINITVECTOR, 0, 1);
                        carve_statement (&salt, enc?"encrypt-data":"decrypt-data", RIM_KEYSALT, 0, 1);
                        carve_statement (&salt_len, enc?"encrypt-data":"decrypt-data" , RIM_KEYSALTLENGTH, 0, 1);
                        carve_statement (&iterations, enc?"encrypt-data":"decrypt-data", RIM_KEYITERATIONS, 0, 1);
                        carve_statement (&to, enc?"encrypt-data":"decrypt-data" , RIM_KEYTO, 1, 1);
                        carve_statement (&binary, enc?"encrypt-data":"decrypt-data", RIM_KEYBINARY, 0, 2);
                        carve_statement (&cipher, enc?"encrypt-data":"decrypt-data", RIM_KEYCIPHER, 0, 1);
                        carve_statement (&digest, enc?"encrypt-data":"decrypt-data", RIM_KEYDIGEST, 0, 1);
                        carve_statement (&cache, enc?"encrypt-data":"decrypt-data", RIM_KEYCACHE, 0, 0);
                        carve_statement (&ccache, enc?"encrypt-data":"decrypt-data", RIM_KEYCLEARCACHE, 0, 1);
                        carve_stmt_obj (&mtext, true, true);

                        make_mem(&password);
                        make_mem(&salt);
                        make_mem(&iv);
                        make_mem(&cipher);
                        make_mem(&digest);

                        define_statement (&to, RIM_DEFSTRING, false); // rim_encrypt sets exact length
                        char *binaryc = opt_clause(binary, "1", "0", RIM_DEFNUMBER);
                        //
                        check_var (&mtext, RIM_DEFSTRING);
                        check_var (&inlength, RIM_DEFNUMBER);
                        check_var (&salt_len, RIM_DEFNUMBER);
                        check_var (&iterations, RIM_DEFNUMBER);
                        check_var (&iv, RIM_DEFSTRING);
                        check_var (&password, RIM_DEFSTRING);
                        check_var (&salt, RIM_DEFSTRING);
                        check_var (&cipher, RIM_DEFSTRING);
                        check_var (&digest, RIM_DEFSTRING);
                        check_var (&binaryc, RIM_DEFNUMBER);
                        check_var (&ccache, RIM_DEFBOOL);

                        if (ccache != NULL && cache == NULL) rim_report_error( "clear-cache cannot be used without cache in encrypt/decrypt-data statement");

                        if (iv == NULL)
                        {
                            iv="NULL";
                            if (cache != NULL) rim_report_error( "cache cannot be used if init-vector is omitted in encrypt/decrypt-data statement");
                        } 
                        if (salt == NULL)
                        {
                            if (salt_len != NULL) rim_report_error( "cannot specify salt-len without salt in encrypt/decrypt-data statement");
                            salt="NULL";
                        }

                        char *to_crypt = mtext;
                        
                        oprintf ("%sEVP_CIPHER_CTX *_rim_e_ctx%ld = NULL;\n", cache!=NULL?"static ":"", encrypt_count);
                        if (ccache != NULL) 
                        {
                            // if clear-cache is true, then first check if there's existing encryption context, if so, clear it
                            // then set context to NULL so it's created again.
                            oprintf ("if ((%s)) {if (_rim_e_ctx%ld != NULL) EVP_CIPHER_CTX_free(_rim_e_ctx%ld); _rim_e_ctx%ld = NULL;}\n", ccache, encrypt_count, encrypt_count, encrypt_count);
                        }
                        oprintf ("if (_rim_e_ctx%ld == NULL) { _rim_e_ctx%ld = EVP_CIPHER_CTX_new(); rim_get_enc_key(%s, %s, %s, %s, %s?_rim_e_ctx%ld:NULL, %s?_rim_e_ctx%ld:NULL, %s, %s);}\n", encrypt_count, encrypt_count, password, salt, salt_len == NULL ? "0":salt_len, iterations==NULL?"-1":iterations, enc?"true":"false", encrypt_count, (!enc)?"true":"false", encrypt_count, cipher==NULL?RIM_DEF_CIPHER:cipher, digest==NULL?RIM_DEF_DIGEST:digest);
                        // regardless of whether input-length is used or not, we have it set
                        if (inlength != NULL) oprintf ("rim_num _rim_encrypt_count%ld = %s;\n", encrypt_count, inlength);
                        else oprintf ("rim_num _rim_encrypt_count%ld = -1;\n", encrypt_count);
                        oprintf ("%s = %s(_rim_e_ctx%ld, (unsigned char*)(%s), &_rim_encrypt_count%ld, %s, (unsigned char*)(%s));\n", to, enc?"rim_encrypt":"rim_decrypt", encrypt_count, to_crypt, encrypt_count, binaryc, iv==NULL?"NULL":iv);
                        if (cache == NULL) oprintf ("EVP_CIPHER_CTX_free(_rim_e_ctx%ld);\n", encrypt_count);
                        encrypt_count++;
                        rim_free(binaryc);


                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "stat-file", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;
                        char *soft = find_keyword (mtext, RIM_KEYSOFTLINK, 1);
                        char *hard = find_keyword (mtext, RIM_KEYHARDLINK, 1);
                        char *path = find_keyword (mtext, RIM_KEYPATH, 1);
                        char *basename = find_keyword (mtext, RIM_KEYNAME, 1);
                        char *type = find_keyword (mtext, RIM_KEYTYPE, 1);
                        char *size = find_keyword (mtext, RIM_KEYSIZE, 1);
                        char *mode = find_keyword (mtext, RIM_KEYMODE, 1);

                        carve_statement (&soft, "stat-file", RIM_KEYSOFTLINK, 0, 1);
                        carve_statement (&hard, "stat-file", RIM_KEYHARDLINK, 0, 1);
                        carve_statement (&path, "stat-file", RIM_KEYPATH, 0, 1);
                        carve_statement (&basename, "stat-file", RIM_KEYNAME, 0, 1);
                        carve_statement (&type, "stat-file", RIM_KEYTYPE, 0, 1);
                        carve_statement (&size, "stat-file", RIM_KEYSIZE, 0, 1);
                        carve_statement (&mode, "stat-file", RIM_KEYMODE, 0, 1);
                        carve_stmt_obj (&mtext, true, true);


                        rim_num tot_opt = (path!=NULL?1:0)+(basename!=NULL?1:0)+(hard!=NULL?1:0)+(soft!=NULL?1:0)+(type!=NULL?1:0)+(size!=NULL?1:0)+(mode!=NULL?1:0);
                        if (tot_opt == 0)
                        {
                            rim_report_error( "At least one clause must be in stat-file statement");
                        }
                        define_statement (&size, RIM_DEFNUMBER, false);
                        define_statement (&type, RIM_DEFNUMBER, false);
                        define_statement (&mode, RIM_DEFNUMBER, false);
                        define_statement (&path, RIM_DEFSTRING, false);
                        define_statement (&soft, RIM_DEFBOOL, false);
                        define_statement (&hard, RIM_DEFBOOL, false);
                        define_statement (&basename, RIM_DEFSTRING, false);

                        check_var (&mtext, RIM_DEFSTRING);

                        if (size != NULL || type != NULL || mode != NULL) 
                        {
                            oprintf ("rim_file_stat (%s, %s%s%s, %s%s%s, %s%s%s, %s%s%s, %s%s%s);\n", mtext, type==NULL?"":"&(",type==NULL?"NULL":type,type==NULL?"":")",  soft==NULL?"":"&(",soft==NULL?"NULL":soft,soft==NULL?"":")", hard==NULL?"":"&(",hard==NULL?"NULL":hard,hard==NULL?"":")", size==NULL?"":"&(",size==NULL?"NULL":size,size==NULL?"":")",   mode==NULL?"":"&(",mode==NULL?"NULL":mode,mode==NULL?"":")");
                        }
                        // basename and path must use strdup on argument because it may be altered
                        // also strdup the result because it may be overwritten by subsequent calls
                        if (basename != NULL) 
                        {
                            oprintf ("%s=rim_basename (%s);\n", basename, mtext);
                        }
                        if (path != NULL) 
                        {
                            oprintf ("%s=rim_realpath (%s);\n", path, mtext);
                        }


                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "utf-text", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;

                        char *json = find_keyword (mtext, RIM_KEYFORJSON, 1);
                        char *len = find_keyword (mtext, RIM_KEYLENGTH, 1);
                        char *status = find_keyword (mtext, RIM_KEYSTATUS, 1);
                        char *errt = find_keyword (mtext, RIM_KEYERRORTEXT, 1);
                        char *to = find_keyword (mtext, RIM_KEYTO, 1);
                        carve_statement (&json, "utf-text", RIM_KEYFORJSON, 0, 0);
                        carve_statement (&len, "utf-text", RIM_KEYLENGTH, 0, 1);
                        carve_statement (&status, "utf-text", RIM_KEYSTATUS, 0, 1);
                        carve_statement (&errt, "utf-text", RIM_KEYERRORTEXT, 0, 1);
                        carve_statement (&to, "utf-text", RIM_KEYTO, 0, 1);
                        carve_stmt_obj (&mtext, true, true);
                        define_statement (&status, RIM_DEFNUMBER, false);
                        define_statement (&errt, RIM_DEFSTRING, false); // exact length set in glim_utf_to_text
                        define_statement (&to, RIM_DEFSTRING, false); // exact length set in glim_utf_to_text
                        //
                        check_var (&mtext, RIM_DEFSTRING);
                        check_var (&len, RIM_DEFNUMBER);

                        oprintf ("char *_rim_errt%ld = RIM_EMPTY_STRING;\n", code_text);
                        oprintf ("_rim_st = rim_utf_to_text (%s, %s, %s%s%s, &_rim_errt%ld, %s);\n", mtext, len == NULL ? "-1":len, to==NULL?"":"&(", to==NULL?"NULL":to, to==NULL?"":")", code_text, json!=NULL?"true":"false");
                        if (status != NULL) oprintf ("if (_rim_st == -1) {RIM_ERR0;%s=RIM_ERR_UTF;} else %s=_rim_st;\n", status, status); else oprintf("if (_rim_st == -1) rim_report_error (\"%s\", \"%s\", (rim_num)%ld, (rim_num)RIM_ERR_UTF);\n", RIM_STMT_FAIL_CHECK, rim_valid_rim_name, lnum);
                        if (errt != NULL) oprintf ("%s = _rim_errt%ld;\n", errt, code_text); 

                        code_text++;

                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "text-utf", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;

                        char *status = find_keyword (mtext, RIM_KEYSTATUS, 1);
                        char *errt = find_keyword (mtext, RIM_KEYERRORTEXT, 1);
                        carve_statement (&status, "text-utf", RIM_KEYSTATUS, 0, 1);
                        carve_statement (&errt, "text-utf", RIM_KEYERRORTEXT, 0, 1);
                        carve_stmt_obj (&mtext, true, true);
                        define_statement (&status, RIM_DEFNUMBER, false);
                        define_statement (&errt, RIM_DEFSTRING, false); // exact length set in glim_text_to_utf
                        //
                        check_var (&mtext, RIM_DEFSTRING);

                        oprintf ("char *_rim_errt%ld = NULL;\n", code_text);
                        oprintf ("_rim_st_str = rim_text_to_utf (%s, 0, &_rim_errt%ld, 1, true);\n", mtext, code_text);
                        if (status != NULL) oprintf ("if (_rim_st_str != NULL) %s=RIM_OKAY; else {RIM_ERR0;%s=RIM_ERR_UTF;}\n", status, status); else oprintf("if (_rim_st_str == NULL) rim_report_error (\"%s\", \"%s\", (rim_num)%ld, (rim_num)RIM_ERR_UTF);\n", RIM_STMT_FAIL_CHECK, rim_valid_rim_name, lnum);
                        if (errt != NULL) oprintf ("%s = _rim_errt%ld;\n", errt, code_text);


                        code_text++;

                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "rename-file", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;
                        // Example:
                        // rename-file "/home/rim/files/somefile.jpg" to "file..." status st
                        char *to = find_keyword (mtext, RIM_KEYTO, 1);
                        char *status = find_keyword (mtext, RIM_KEYSTATUS, 1);

                        carve_statement (&status, "rename-file", RIM_KEYSTATUS, 0, 1);
                        carve_statement (&to, "rename-file", RIM_KEYTO, 1, 1);
                        carve_stmt_obj (&mtext, true, true);
                        make_mem(&to);    

                        define_statement (&status, RIM_DEFNUMBER, false);
                        //
                        check_var (&mtext, RIM_DEFSTRING);
                        check_var (&to, RIM_DEFSTRING);

                        //
                        // Look for each option and collect relevant info
                        //
                        oprintf ("_rim_st = rename ((%s), (%s));RIM_ERR;\n", mtext, to);
                        if (status == NULL)
                        {
                            oprintf("if (_rim_st != RIM_OKAY) rim_report_error (\"%s\", \"%s\", (rim_num)%ld, (rim_num)RIM_ERR_RENAME);\n", RIM_STMT_FAIL_CHECK, rim_valid_rim_name, lnum);
                        }
                        else
                        {
                            oprintf ("if (_rim_st != 0) {%s=RIM_ERR_RENAME;} else %s=RIM_OKAY;\n",  status, status);
                        }


                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "delete-dir", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;
                        char *status = find_keyword (mtext, RIM_KEYSTATUS, 1);

                        carve_statement (&status, "delete-dir", RIM_KEYSTATUS, 0, 1);

                        carve_stmt_obj (&mtext, true, true);

                        define_statement (&status, RIM_DEFNUMBER, false);
                        //
                        check_var (&mtext, RIM_DEFSTRING);

                        //
                        // Look for each option and collect relevant info, namely OS errno
                        //
                        oprintf ("_rim_st = rmdir (%s);RIM_ERR;\n", mtext);
                        if (status == NULL)
                        {
                            oprintf("if (_rim_st != RIM_OKAY) rim_report_error (\"%s\", \"%s\", (rim_num)%ld, (rim_num)((errno == EEXIST||errno==ENOTEMPTY)?RIM_ERR_EXIST:RIM_ERR_FAILED));\n", RIM_STMT_FAIL_CHECK, rim_valid_rim_name, lnum);
                        }
                        else
                        {
                            // if directory exists, RIM_ERR_EXIST is the error
                            oprintf ("if (_rim_st != 0 ) {if (errno == EEXIST || errno == ENOTEMPTY) %s=RIM_ERR_EXIST; else {RIM_ERR;%s=RIM_ERR_FAILED;}} else %s=RIM_OKAY;\n", status, status, status);
                        }


                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "change-mode", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;
                        char *status = find_keyword (mtext, RIM_KEYSTATUS, 1);
                        char *mode = find_keyword (mtext, RIM_KEYMODE, 1);

                        carve_statement (&status, "change-mode", RIM_KEYSTATUS, 0, 1);
                        carve_statement (&mode, "change-mode", RIM_KEYMODE, 1, 1);

                        carve_stmt_obj (&mtext, true, true);

                        define_statement (&status, RIM_DEFNUMBER, false);
                        check_var (&mode, RIM_DEFNUMBER);
                        //
                        check_var (&mtext, RIM_DEFSTRING);

                        //
                        // Look for each option and collect relevant info, namely OS errno
                        //
                        oprintf ("_rim_st = chmod (%s, %s);RIM_ERR;\n", mtext, mode);
                        if (status == NULL)
                        {
                            oprintf("if (_rim_st != RIM_OKAY) rim_report_error (\"%s\", \"%s\", (rim_num)%ld, (rim_num)(errno==ENOENT?RIM_ERR_EXIST:RIM_ERR_FAILED));\n", RIM_STMT_FAIL_CHECK, rim_valid_rim_name, lnum);
                        }
                        else
                        {
                            // if directory exists, RIM_ERR_EXIST is the error
                            oprintf ("if (_rim_st != 0 ) {if (errno == ENOENT) %s=RIM_ERR_EXIST; else {RIM_ERR; %s=RIM_ERR_FAILED;}} else %s=RIM_OKAY;\n", status, status, status);
                        }
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "new-dir", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;
                        char *status = find_keyword (mtext, RIM_KEYSTATUS, 1);
                        char *mode = find_keyword (mtext, RIM_KEYMODE, 1);

                        carve_statement (&status, "new-dir", RIM_KEYSTATUS, 0, 1);
                        carve_statement (&mode, "new-dir", RIM_KEYMODE, 0, 1);

                        carve_stmt_obj (&mtext, true, true);

                        define_statement (&status, RIM_DEFNUMBER, false);
                        check_var (&mode, RIM_DEFNUMBER);
                        //
                        check_var (&mtext, RIM_DEFSTRING);
                        if (mode == NULL) mode = "0700";

                        //
                        // Look for each option and collect relevant info, namely OS errno
                        //
                        oprintf ("_rim_st = mkdir (%s, %s);RIM_ERR;\n", mtext, mode);
                        if (status == NULL)
                        {
                            oprintf("if (_rim_st != RIM_OKAY) rim_report_error (\"%s\", \"%s\", (rim_num)%ld, (rim_num)(errno==EEXIST?RIM_ERR_EXIST?RIM_ERR_FAILED));\n", RIM_STMT_FAIL_CHECK, rim_valid_rim_name, lnum);
                        }
                        else
                        {
                            // if directory exists, RIM_ERR_EXIST is the error
                            oprintf ("if (_rim_st != 0 ) {if (errno == EEXIST) %s=RIM_ERR_EXIST; else {RIM_ERR;%s=RIM_ERR_FAILED;}} else %s=RIM_OKAY;\n", status, status, status);
                        }
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "change-dir", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;
                        char *status = find_keyword (mtext, RIM_KEYSTATUS, 1);
                        char *home = find_keyword (mtext, RIM_KEYHOME, 1);
                        char *edir = find_keyword (mtext, RIM_KEYRUNDIR, 1);

                        carve_statement (&status, "change-dir", RIM_KEYSTATUS, 0, 1);
                        carve_statement (&home, "change-dir", RIM_KEYHOME, 0, 0);
                        carve_statement (&edir, "change-dir", RIM_KEYRUNDIR, 0, 0);
                        if (home == NULL && edir == NULL) carve_stmt_obj (&mtext, true, true); else carve_stmt_obj (&mtext, false, false);

                        define_statement (&status, RIM_DEFNUMBER, false);
                        //
                        check_var (&mtext, RIM_DEFSTRING);

                        //
                        // Look for each option and collect relevant info, namely OS errno
                        //
                        oprintf ("_rim_st = chdir (%s);RIM_ERR;\n", home == NULL ? (edir == NULL ? mtext:"rim_get_config()->app.run_dir"): "rim_get_config()->app.home_dir");
                        if (status == NULL)
                        {
                            oprintf("if (_rim_st != RIM_OKAY) rim_report_error (\"%s\", \"%s\", (rim_num)%ld, (rim_num)RIM_ERR_FAILED);\n", RIM_STMT_FAIL_CHECK, rim_valid_rim_name, lnum);
                        }
                        else
                        {
                            oprintf ("if (_rim_st != 0 ) {RIM_ERR;%s=RIM_ERR_FAILED;} else %s=RIM_OKAY;\n", status, status);
                        }
                        // we set this global boolean so we don't have to chdir at the beginning of each request
                        // note that in internal mode or in external, using directly C' chdir() will not affect this, and rim_path_changed should be manually set!!!
                        // nothing to do for edir, since home should for the next request either way
                        // Plus, this shouldn't be used for command-line programs
                        oprintf("rim_path_changed=%s;\n", home == NULL?"true":"false");


                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "delete-file", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;
                        // Example:
                        // delete-file "/home/rim/files/somefile.jpg" status st
                        char *status = find_keyword (mtext, RIM_KEYSTATUS, 1);

                        carve_statement (&status, "delete-file", RIM_KEYSTATUS, 0, 1);
                        carve_stmt_obj (&mtext, true, true);

                        define_statement (&status, RIM_DEFNUMBER, false);
                        //
                        check_var (&mtext, RIM_DEFSTRING);

                        //
                        // Look for each option and collect relevant info
                        //
                        oprintf ("_rim_st = unlink (%s);RIM_ERR;\n", mtext);
                        if (status == NULL)
                        {
                            oprintf("if (_rim_st != RIM_OKAY && errno != ENOENT) rim_report_error (\"%s\", \"%s\", (rim_num)%ld,  (rim_num)RIM_ERR_DELETE);\n", RIM_STMT_FAIL_CHECK, rim_valid_rim_name, lnum);
                        }
                        else
                        {
                            oprintf ("if (_rim_st != 0  && errno != ENOENT) {RIM_ERR; %s=RIM_ERR_DELETE;} else %s=RIM_OKAY;\n", status, status);
                        }


                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "call-web", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;

                        rim_mod_curl = true;

                        //
                        // Look for each option and collect relevant info
                        //
                        char *resp = find_keyword (mtext, RIM_KEYRESPONSE, 1);
                        char *head = find_keyword (mtext, RIM_KEYRESPONSEHEADERS, 1);
                        char *rhead = find_keyword (mtext, RIM_KEYREQUESTHEADERS, 1);
                        char *resp_code = find_keyword (mtext, RIM_KEYRESPONSECODE, 1);
                        char *len = find_keyword (mtext, RIM_KEYSTATUS, 1);
                        char *err = find_keyword (mtext, RIM_KEYERROR, 1);
                        char *cert = find_keyword (mtext, RIM_KEYCERT, 1);
                        char *nocert = find_keyword (mtext, RIM_KEYNOCERT, 1);
                        char *cookiejar = find_keyword (mtext, RIM_KEYCOOKIEJAR, 1);
                        char *timeout = find_keyword (mtext, RIM_KEYTIMEOUT, 1);
                        char *body = find_keyword (mtext, RIM_KEYREQUESTBODY, 1);
                        char *method = find_keyword (mtext, RIM_KEYMETHOD, 1);

                        carve_statement (&body, "call-web", RIM_KEYREQUESTBODY, 0, 1);
                        carve_statement (&timeout, "call-web", RIM_KEYTIMEOUT, 0, 1);
                        carve_statement (&method, "call-web", RIM_KEYMETHOD, 0, 1);
                        carve_statement (&resp, "call-web", RIM_KEYRESPONSE, 1, 1);
                        carve_statement (&head, "call-web", RIM_KEYRESPONSEHEADERS, 0, 1);
                        carve_statement (&rhead, "call-web", RIM_KEYREQUESTHEADERS, 0, 1);
                        carve_statement (&resp_code, "call-web", RIM_KEYRESPONSECODE, 0, 1);
                        carve_statement (&len, "call-web", RIM_KEYSTATUS, 0, 1);
                        carve_statement (&err, "call-web", RIM_KEYERROR, 0, 1);
                        carve_statement (&cert, "call-web", RIM_KEYCERT, 0, 1);
                        carve_statement (&nocert, "call-web", RIM_KEYNOCERT, 0, 0);
                        carve_statement (&cookiejar, "call-web", RIM_KEYCOOKIEJAR, 0, 1);
                        carve_stmt_obj (&mtext, true, true);
                        make_mem(&cert);    
                        make_mem(&method);    
                        make_mem(&cookiejar);    
                        //
                        check_var (&mtext, RIM_DEFSTRING);
                        check_var (&cert, RIM_DEFSTRING);
                        check_var (&method, RIM_DEFSTRING);
                        check_var (&cookiejar, RIM_DEFSTRING);
                        check_var (&timeout, RIM_DEFNUMBER);

                        // process body clause after ward because it is done within a copy of carved out 'body' above
                        char *files = NULL;
                        char *fields = NULL;
                        char *content = NULL;
                        if (body != NULL)
                        {
                            char *mbody = rim_strdup (body);
                            content = find_keyword (mbody, RIM_KEYCONTENT, 1);
                            files = find_keyword (mbody, RIM_KEYFILES, 1);
                            fields = find_keyword (mbody, RIM_KEYFIELDS, 1);
                            carve_statement (&content, "call-web", RIM_KEYCONTENT, 0, 1);
                            carve_statement (&files, "call-web", RIM_KEYFILES, 0, 1);
                            carve_statement (&fields, "call-web", RIM_KEYFIELDS, 0, 1);
                            make_mem(&content);    
                            //
                            check_var (&content, RIM_DEFSTRING);
                        }

                        //defines
                        define_statement (&len, RIM_DEFNUMBER, false);
                        define_statement (&resp, RIM_DEFSTRING, false); // exact length  set in rim_post_url_with_response
                        define_statement (&head, RIM_DEFSTRING, false); // exact length  set in rim_post_url_with_response
                        define_statement (&resp_code, RIM_DEFNUMBER, false);
                        define_statement (&err, RIM_DEFSTRING, false); // exact length set in rim_post_url_with_response

                        char *req_header = NULL;
                        char req_header_ptr[200];
                        char* clen = NULL;
                        char* ctype = NULL;
                        if (rhead != NULL)
                        {
                            // RIM_HEADER_FILE/PAGE doesn't matter, cache is not used with request header
                            req_header = make_default_header(RIM_HEADER_FILE, http_header_count, 1);
                            snprintf (req_header_ptr, sizeof(req_header_ptr), "&%s", req_header);
                            process_http_header ("call-web", rhead, req_header, http_header_count, 1, &clen, &ctype); // RIM_KEYCONTENTTYPE RIM_KEYCUSTOM RIM_KEYCONTENTLENGTH
                            http_header_count++;
                        }
                        if (content != NULL && ctype == NULL) 
                        {
                            rim_report_error( "'content-type' must be specified with 'content' subclause in 'request-body' in call-web statement");
                        }
                        if (content == NULL && clen != NULL && body != NULL) 
                        {
                            rim_report_error( "'content-length' cannot be specified without 'content' subclause in 'request-body' in call-web statement");
                        }
                        if (content == NULL && ctype != NULL && body != NULL) 
                        {
                            rim_report_error( "'content-type' cannot be specified without 'content' subclause in 'request-body' in call-web statement");
                        }
                        if ((files == NULL && fields == NULL && content == NULL ) && body != NULL) 
                        {
                            rim_report_error( "either 'content' or 'files'/'fields' must be specified with 'body' clause in  call-web statement");
                        }
                        if ((files != NULL || fields != NULL)) 
                        {
                            if (ctype == NULL) ctype = "multipart/form-data"; // when files or forms used, always this
                        }
                        if ((files != NULL || fields != NULL) && (content != NULL)) 
                        {
                            rim_report_error( "you can specify either 'content' or 'files'/'fields', but not both in  call-web statement");
                        }
                        if (cert != NULL && nocert != NULL)
                        {
                            rim_report_error( "cert and no-cert cannot coexist in the same call-web statement");
                        }

                        // cert cannot be defined, must exist and be filled with the location of .ca file!

                        // store fields and files (with body) arrays
                        char fields_var[100];
                        char files_var[100];
                        if (fields != NULL)
                        {
                            snprintf (fields_var, sizeof (fields_var), "_rim_fields_arr%ld", total_body);
                            outargs(fields, fields_var, "char *", 0, 1, RIM_DEFSTRING, RIM_DEFSTRING);
                        }
                        if (files != NULL)
                        {
                            snprintf (files_var, sizeof (files_var), "_rim_files_arr%ld", total_body);
                            outargs(files, files_var, "char *", 0, 1, RIM_DEFSTRING, RIM_DEFSTRING);
                        }

                        total_body++;

                        oprintf ("_rim_st=rim_post_url_with_response(%s, &(%s), %s%s%s, %s%s%s, %s, %s, %s%s%s, %s, %s, %s, %s, %s, %s, %s, %s);\n", mtext, resp, head==NULL ? "":"&(",head==NULL ? "NULL":head, head==NULL ? "":")", err==NULL ? "":"&(",err==NULL ? "NULL":err, err==NULL ? "":")", nocert != NULL ? "NULL" : (cert != NULL ? cert : "\"\""), cookiejar == NULL ? "NULL":cookiejar, resp_code==NULL ? "":"&(",resp_code==NULL ? "NULL":resp_code, resp_code==NULL ? "":")", timeout==NULL ? "120":timeout, body == NULL ? "0":"1", fields == NULL ? "NULL":fields_var, files == NULL ? "NULL":files_var, req_header == NULL ? "NULL":req_header_ptr, method == NULL ? "NULL" :method, content==NULL?"NULL":content, clen==NULL?"-1":clen);
                        if (len != NULL) oprintf("%s=_rim_st;\n",len); else oprintf("if (_rim_st < 0) rim_report_error (\"%s\", \"%s\", (rim_num)%ld, _rim_st);\n", RIM_STMT_FAIL_CHECK, rim_valid_rim_name, lnum);

                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "run-remote", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;
                        rim_mod_service = true;
                        run_service (mtext);
                        // list of keywords for vim, picked up automatically
                        // RIM_KEYLOCATION RIM_KEYMETHOD RIM_KEYAPPPATH RIM_KEYREQUESTPATH RIM_KEYREQUESTBODY RIM_KEYURLPARAMS RIM_KEYTIMEOUT RIM_KEYENVIRONMENT  RIM_KEYLOCAL  RIM_KEYURLPATH RIM_KEYCONTENTLENGTH RIM_KEYCONTENT RIM_KEYCONTENTTYPE RIM_KEYCONTENTLENGTH RIM_KEYCONTENTTYPE
                        // RIM_KEYDATA  RIM_KEYERROR  RIM_KEYSTATUSTEXT RIM_KEYEXITSTATUS RIM_KEYSTATUS
                        // RIM_KEYFINISHEDOKAY RIM_KEYSTARTED RIM_KEYSTATUS
                        // end list of keywords
                        //
                        //
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "new-remote", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;
                        rim_mod_service = true;
                        new_service (mtext);
                        // list of keywords for vim, picked up automatically
                        // RIM_KEYLOCATION RIM_KEYMETHOD RIM_KEYAPPPATH RIM_KEYREQUESTPATH RIM_KEYREQUESTBODY RIM_KEYURLPARAMS RIM_KEYTIMEOUT RIM_KEYENVIRONMENT  RIM_KEYLOCAL  RIM_KEYURLPATH RIM_KEYCONTENTLENGTH RIM_KEYCONTENT RIM_KEYCONTENTTYPE RIM_KEYCONTENTLENGTH RIM_KEYCONTENTTYPE
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "read-remote", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;
                        rim_mod_service = true;
                        read_service (mtext);
                        // list of keywords for vim, picked up automatically
                        // RIM_KEYDATA  RIM_KEYERROR RIM_KEYSTATUSTEXT RIM_KEYEXITSTATUS RIM_KEYSTATUS
                        //
                        //
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "call-extended", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;
                        if (rim_mode != RIM_MODE_EXTENDED) rim_report_error ("call-extended statement cannot be used in safe mode, see *-mode statements");
                        carve_stmt_obj (&mtext, true, false);
                        // list of keywords for vim, picked up automatically
                        check_c (mtext);
                        oprintf("%s;\n", mtext);
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "call-remote", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;
                        rim_mod_service = true;
                        call_service (mtext);
                        // list of keywords for vim, picked up automatically
                       // RIM_KEYFINISHEDOKAY RIM_KEYSTARTED RIM_KEYSTATUS
                        //
                        // this commented out is just so getvim will color output - DO NOT REMOVE
                        //char *fok = find_keyword (mtext, RIM_KEYFINISHEDOKAY, 1);
                        //char *start = find_keyword (mtext, RIM_KEYSTARTED, 1);
                        //char *st = find_keyword (mtext, RIM_KEYSTATUS, 1);
                        //
                        //this define_statement MUST stay for coloring
                        //define_statement (&st, RIM_DEFNUMBER, false);
                        //define_statement (&start, RIM_DEFNUMBER, false);
                        //define_statement (&fok, RIM_DEFNUMBER, false);
                        //
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "call-handler", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;
                        char *rval = find_keyword (mtext, RIM_KEYRETURNCODE, 1);

                        //
                        //This block delete when new major release
                        bool old_return = false;
                        if (rval == NULL) { rval = find_keyword (mtext, RIM_KEYRETURNVALUE, 1); old_return = true; }
                        //

                        char *spar = find_keyword (mtext, RIM_KEYSETPARAM, 1);
                        char *gpar = find_keyword (mtext, RIM_KEYGETPARAM, 1);
                        carve_statement (&rval, "call-handler", old_return?RIM_KEYRETURNVALUE:RIM_KEYRETURNCODE, 0, 1);
                        carve_statement (&spar, "call-handler", RIM_KEYSETPARAM, 0, 1);
                        carve_statement (&gpar, "call-handler", RIM_KEYGETPARAM, 0, 1);
                        define_statement (&rval, RIM_DEFNUMBER, false); 

                        if (spar != NULL) process_param_set (spar); // set parameters *before* calling the handler

                        //
                        // must check if mtext clause is string constant *before* other variable checks below, because
                        // they may create a temp variable for it
                        bool is_constant;
                        //
                        //
                        //if sub name is a constant, check there's actually a source file to run it
                        is_constant = check_sub (mtext); // this must be BEFORE carve_stmt_obj as it may create temp variable which is just some generated var name
                                           // it trims mtext, but we're about to do that anyway
                        //
                        rim_num mlen = carve_stmt_obj (&mtext, true, false); // we specify false for is_constant so that mtext is NOT converted to _rim_var... (see above)
                        if (!is_constant) check_var (&mtext, RIM_DEFSTRING); // we do not want to create a _rim_... variable for when callee is a constant
                                                                            // string, as we want to plug that string in directly in C function call name

                        // 
                        // Calling a handler is implemented as a direct call because performance is paramount
                        // We either call C function directly, or find it in hash (sized to provide 1-1.5 lookups at most))
                        // and then call it - the latter is only if handler is a variable and can't be mapped to a C function at compile time/
                        // We count stack depth and enforce limit for safety, and also use this information to know if we're a top call or not.
                        //
                        // NOTE: rim_req  is predefined at run time and can be used anywhere in v1.c!!
                        //
                        oprintf ("{\n");
                        oprintf ("if (__builtin_expect (++(rim_req.sub_depth) >= rim_sub_max_depth,0)) rim_report_error (\"Too many request handlers called without returning [%%ld], use set-app statement with stack-depth clause to increase the maximum recursion depth level if needed\", rim_req.sub_depth);\n");
                        if (is_constant) // callee known at compile time
                        {
                            char reqname[RIM_MAX_REQ_NAME_LEN];
                            char *scall = mtext;
                            char decres = rim_decorate_path (reqname, sizeof(reqname), &scall, mlen);
                            if (decres != 1) rim_report_error( "Request path [%s] is not a valid name",scall);
                            // we must get rid of double quotes around the callee, and the leading __ (coming from leading / when decorating)
                            reqname[strlen(reqname)-1] = 0; // last double quote gone
                            oprintf ("%s%s%s();\n", rval?rval:"", rval?"=":"",reqname+1+(reqname[1]=='_'?2:0)); // +3 skips "__ to get to the callee name as a C function, __ may not be present, hence the ?
                        }
                        else
                        { // callee must be computed at run-time
                            oprintf ("rim_request_handler _rim_req_handler;\n");
                            oprintf ("char reqname[RIM_MAX_REQ_NAME_LEN];\n");
                            oprintf ("char *_rim_call=%s;\n",mtext); // must use variable, as it gets changed at runtime from rim_decorate_path()
                            // 1 means good hierarchical path, reqname is it; or 3 means no /, so reqname is a copy of mtext
                            oprintf ("char decres = rim_decorate_path (reqname, sizeof(reqname), &_rim_call, rim_mem_get_len(_rim_call));\n");
                            oprintf ("if (decres != 1) rim_report_error( \"Request path [%%s] is not a valid name\",_rim_call);\n");
                            oprintf ("rim_num _rim_found;\n");
                            // reqname is NOT rimstone memory, so false on rimstone-comparison (last param)
                            oprintf ("_rim_req_handler = rim_find_hash (&rim_dispatch, reqname, 0, &_rim_found, false);\n");
                            oprintf ("if (_rim_found != RIM_OKAY) rim_report_error( \"Request handler not found [%%s]\", _rim_call);\n");
                            oprintf ("%s%s_rim_req_handler();\n", rval?rval:"", rval?"=":"");
                        }
                        oprintf ("rim_req.sub_depth--;\n");
                        oprintf ("}\n");

                        // get-param *after* the call-handler
                        if (gpar != NULL)
                        {
                            process_param_get (gpar);
                            //
                            // RIM_KEYCOMMA RIM_KEYDEFAULTVALUE RIM_KEYTYPE 
                            // Make vim recognize type names
                            //
                            // RIM_KEY_T_STRING RIM_KEY_T_BOOL RIM_KEY_T_NUMBER RIM_KEY_T_DOUBLE RIM_KEY_T_MESSAGE RIM_KEY_T_SPLITSTRING RIM_KEY_T_HASH RIM_KEY_T_ARRAYSTRING RIM_KEY_T_ARRAYNUMBER RIM_KEY_T_ARRAYBOOL RIM_KEY_T_ARRAYDOUBLE RIM_KEY_T_TREE RIM_KEY_T_TREECURSOR RIM_KEY_T_FIFO RIM_KEY_T_LIFO RIM_KEY_T_LIST RIM_KEY_T_ENCRYPT RIM_KEY_T_FILE RIM_KEY_T_SERVICE 
                        }

                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "uniq-file", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;
                        char *temp = find_keyword (mtext, RIM_KEYTEMPORARY, 1);

                        //
                        // After all options positions have been found, we must get the options 
                        // for ALL of them
                        //
                        carve_statement (&temp, "uniq-file", RIM_KEYTEMPORARY, 0, 0);
                        carve_stmt_obj (&mtext, true, true);

                        //defines
                        define_statement (&mtext, RIM_DEFSTRING, false); // exact length set in rim_make_document

                        //generate code
                        oprintf ("{FILE *f = rim_make_document (&(%s), %d); fclose (f);}\n", mtext, temp!=NULL?1:0);

                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "get-time", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;
                        // Example:
                        // get-time to [define] res timezone "GMT" [year +/-3] [month +/-3] [day +/-3] [hour +/-3] [min +/-3] [sec +/-3] [format "%a..."]
                        // get system time that's current, unless one of the options are used which go into the future/past as given

                        //
                        // Look for each option and collect relevant info
                        // First we MUST get each options position
                        //
                        char *res = find_keyword (mtext, RIM_KEYTO, 1);
                        char *epoch = find_keyword (mtext, RIM_KEYSINCEEPOCH, 1);
                        char *fepoch = find_keyword (mtext, RIM_KEYFROMEPOCH, 1);
                        char *timezone = find_keyword (mtext, RIM_KEYTIMEZONE, 1);
                        char *year = find_keyword (mtext, RIM_KEYYEAR, 1);
                        char *month = find_keyword (mtext, RIM_KEYMONTH, 1);
                        char *day = find_keyword (mtext, RIM_KEYDAY, 1);
                        char *hour = find_keyword (mtext, RIM_KEYHOUR, 1);
                        char *min = find_keyword (mtext, RIM_KEYMIN, 1);
                        char *sec = find_keyword (mtext, RIM_KEYSEC, 1);
                        char *format = find_keyword (mtext, RIM_KEYFORMAT, 1);

                        //
                        // After all options positions have been found, we must get the options 
                        // for ALL of them
                        //
                        carve_statement (&res, "get-time", RIM_KEYTO, 1, 1);
                        carve_statement (&epoch, "get-time", RIM_KEYSINCEEPOCH, 0, 0);
                        carve_statement (&fepoch, "get-time", RIM_KEYFROMEPOCH, 0, 1);
                        carve_statement (&timezone, "get-time", RIM_KEYTIMEZONE, 0, 1);
                        carve_statement (&year, "get-time", RIM_KEYYEAR, 0, 1);
                        carve_statement (&month, "get-time", RIM_KEYMONTH, 0, 1);
                        carve_statement (&day, "get-time", RIM_KEYDAY, 0, 1);
                        carve_statement (&hour, "get-time", RIM_KEYHOUR, 0, 1);
                        carve_statement (&min, "get-time", RIM_KEYMIN, 0, 1);
                        carve_statement (&sec, "get-time", RIM_KEYSEC, 0, 1);
                        carve_statement (&format, "get-time", RIM_KEYFORMAT, 0, 1);
                        carve_stmt_obj (&mtext, false, false);

                        if (epoch != NULL && (timezone != NULL || year != NULL || month != NULL || day != NULL || hour != NULL || min != NULL || sec != NULL || format != NULL || fepoch != NULL))
                        {
                            rim_report_error ("Cannot use timezone/year/month/day/hour/min/sec/format/from-epoch clause(s) if since-epoch clause is used");
                        }

                        if (epoch == NULL)
                        {
                            make_mem(&timezone);    
                            make_mem(&format);    
                        }

                        // handle any possibly created variables 
                        // res is mandatory, but even if not, define_statement will skip if NULL
                        if (epoch != NULL) define_statement (&res, RIM_DEFNUMBER, false); // seconds since epoch
                        else define_statement (&res, RIM_DEFSTRING, false); // exact length set in rim_time
                        if (epoch == NULL)
                        {
                            check_var (&timezone, RIM_DEFSTRING);
                            check_var (&year, RIM_DEFNUMBER);
                            check_var (&month, RIM_DEFNUMBER);
                            check_var (&day, RIM_DEFNUMBER);
                            check_var (&hour, RIM_DEFNUMBER);
                            check_var (&min, RIM_DEFNUMBER);
                            check_var (&sec, RIM_DEFNUMBER);
                            check_var (&format, RIM_DEFSTRING);
                            check_var (&fepoch, RIM_DEFNUMBER);
                        }

                        // No need to check if res!=NULL (i.e. it's mandatory), the above carve_statement checks that

                        //
                        // If there is data right after statement (i.e. 'get-time') and it has no option (such as call-web https://...)
                        // then mtext is this option (in this case https://...). In this particular case, we don't have such an option -
                        // every option has a keyword preceding it, including the first one.
                        //

                        if (epoch == NULL)
                        {
                            oprintf ("%s=rim_time(%s, %s, %s, %s, %s, %s, %s, %s, %s);\n", res, fepoch==NULL?"-1":fepoch, timezone==NULL ? "\"GMT\"":timezone, format==NULL ? "NULL":format, year==NULL ? "0":year, month==NULL ? "0":month, 
                                day==NULL ? "0":day, hour==NULL ? "0":hour, min==NULL ? "0":min, sec==NULL ? "0":sec);
                        }
                        else
                        {
                            oprintf ("%s=time(NULL); if ((time_t)(%s)==(time_t)-1) rim_report_error (\"Cannot obtain time, error [%%d]\",errno);\n", res, res);
                        }


                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "match-regex", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;
                        rim_mod_pcre2 = true;
                        char *cache = find_keyword (mtext, RIM_KEYCACHE, 1);
                        char *ccache = find_keyword (mtext, RIM_KEYCLEARCACHE, 1);
                        char *in = find_keyword (mtext, RIM_KEYIN, 1);
                        char *replace_with = find_keyword (mtext, RIM_KEYREPLACEWITH, 1);
                        char *result = find_keyword (mtext, RIM_KEYRESULT, 1);
                        char *status = find_keyword (mtext, RIM_KEYSTATUS, 1);
                        char *case_insensitive = find_keyword (mtext, RIM_KEYCASEINSENSITIVE, 1);
                        char *utf = find_keyword (mtext, RIM_KEYUTF, 1);
                        char *single_match = find_keyword (mtext, RIM_KEYSINGLEMATCH, 1);

                        //
                        // After all options positions have been found, we must get the options 
                        // for ALL of them
                        //
                        carve_statement (&cache, "match-regex", RIM_KEYCACHE, 0, 0);
                        carve_statement (&ccache, "match-regex", RIM_KEYCLEARCACHE, 0, 1);
                        carve_statement (&in, "match-regex", RIM_KEYIN, 1, 1);
                        carve_statement (&replace_with, "match-regex", RIM_KEYREPLACEWITH, 0, 1);
                        carve_statement (&result, "match-regex", RIM_KEYRESULT, 0, 1);
                        carve_statement (&status, "match-regex", RIM_KEYSTATUS, 0, 1);
                        carve_statement (&case_insensitive, "match-regex", RIM_KEYCASEINSENSITIVE, 0, 2);
                        carve_statement (&utf, "match-regex", RIM_KEYUTF, 0, 2);
                        carve_statement (&single_match, "match-regex", RIM_KEYSINGLEMATCH, 0, 2);
                        carve_stmt_obj (&mtext, true, true);
                        char *pattern = mtext;

                        make_mem(&in);
                        make_mem(&replace_with);

                        char *case_insensitivec = opt_clause(case_insensitive, "1", "0", RIM_DEFNUMBER);
                        char *utfc = opt_clause(utf, "1", "0", RIM_DEFNUMBER);
                        char *single_matchc = opt_clause(single_match, "1", "0", RIM_DEFNUMBER);

                        if (result != NULL && replace_with == NULL)
                        {
                            rim_report_error( "replace-with must be specified if result is given in match-regex statement");
                        }
                        if (replace_with != NULL && result == NULL)
                        {
                            rim_report_error( "result must be specified if replace-with is given in match-regex statement");
                        }
                        if (result == NULL && status == NULL)
                        {
                            rim_report_error( "status must be specified in this match-regex statement");
                        }

                        // result can be defined
                        define_statement (&result, RIM_DEFSTRING, false); // exact length is set in rim_regex
                        define_statement (&status, RIM_DEFNUMBER, false);
                        check_var (&replace_with, RIM_DEFSTRING);
                        check_var (&mtext, RIM_DEFSTRING);
                        check_var (&case_insensitivec, RIM_DEFNUMBER);
                        check_var (&ccache, RIM_DEFBOOL);
                        check_var (&utfc, RIM_DEFNUMBER);
                        check_var (&single_matchc, RIM_DEFNUMBER);
                        check_var (&in, RIM_DEFSTRING);

                        //
                        // If there is data right after statement (i.e. 'match') and it has no option (such as call-web https://...)
                        // then mtext is this option (in this case https://...). 
                        //

                        // if no cache, then we pass NULL pointer, if there is cache we pass address of (originally) NULL pointer
                        char regname[100];
                        snprintf (regname, sizeof(regname), "_rim_regex%ld", regex_cache);
                        oprintf ("static regex_t *%s = NULL;\nRIM_UNUSED(%s);\n", regname, regname);
                        if (ccache != NULL) 
                        {
                            // if clear-cache is true, then first check if there's existing regex context, if so, clear it
                            // then set context to NULL so it's created again.
                            oprintf ("if ((%s)) {if (%s != NULL) rim_regfree(%s); %s = NULL;}\n", ccache, regname, regname, regname);
                        }

                        if (replace_with == NULL) oprintf ("%s=rim_regex(%s, %s, %s, %s, %s, %s, %s, %s%s);\n", status==NULL?"_rim_st":status, in, pattern, "NULL", "NULL", utfc, case_insensitivec, single_matchc, cache==NULL?"":"&",cache==NULL?"NULL":regname);
                        else oprintf ("%s=rim_regex(%s, %s, %s, &(%s), %s, %s, %s, %s%s);\n", status==NULL?"_rim_st":status, in, pattern, replace_with, result, utfc, case_insensitivec, single_matchc, cache==NULL?"":"&",cache==NULL?"NULL":regname);
                        //We don't check here, because not replacing anything doesn't mean it's an error
                        //if (status == NULL) oprintf("if (_rim_st == 0) rim_report_error (\"%s\", \"%s\", (rim_num)%ld, _rim_st);\n", RIM_STMT_FAIL_CHECK, rim_valid_rim_name, lnum);
                        rim_free(case_insensitivec);
                        rim_free(single_matchc);
                        rim_free(utfc);

                        regex_cache++;
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "get-sys", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;

                        //
                        // Look for each option and collect relevant info
                        // First we MUST get each options position
                        //
                        char *env = find_keyword (mtext, RIM_KEYENVIRONMENT, 1);
                        char *osuser = find_keyword (mtext, RIM_KEYOSUSER, 1);
                        char *osdir = find_keyword (mtext, RIM_KEYOSDIR, 1);
                        char *osname = find_keyword (mtext, RIM_KEYOSNAME, 1);
                        char *osver = find_keyword (mtext, RIM_KEYOSVERSION, 1);
                        char *to = find_keyword (mtext, RIM_KEYTO, 1);
                        char *dir = find_keyword (mtext, RIM_KEYDIRECTORY, 1);

                        //
                        // After all options positions have been found, we must get the options 
                        // for ALL of them
                        //
                        carve_statement (&env, "get-sys", RIM_KEYENVIRONMENT, 0, 1);
                        carve_statement (&osname, "get-sys", RIM_KEYOSNAME, 0, 0);
                        carve_statement (&osdir, "get-sys", RIM_KEYOSDIR, 0, 0);
                        carve_statement (&osuser, "get-sys", RIM_KEYOSUSER, 0, 0);
                        carve_statement (&osver, "get-sys", RIM_KEYOSVERSION, 0, 0);
                        carve_statement (&to, "get-sys", RIM_KEYTO, 1, 1);
                        carve_statement (&dir, "get-sys", RIM_KEYDIRECTORY, 0, 0);
                        carve_stmt_obj (&mtext, false, false);
                        make_mem(&env);

                        // result can be defined
                        define_statement (&to, RIM_DEFSTRING, false); // exact length is set below with strdup
                        check_var (&env, RIM_DEFSTRING);

                        rim_num tot_opt = (env!=NULL?1:0)+(osname!=NULL?1:0)+(osdir!=NULL?1:0)+(osuser!=NULL?1:0)+(osver!=NULL?1:0)+(dir!=NULL?1:0);
                        if (tot_opt != 1)
                        {
                            rim_report_error( "Exactly one option must be in get-sys statement");
                        }

                        if (env !=NULL) oprintf ("%s=rim_strdup(rim_getenv(%s));\n", to,env);
                        if (osname !=NULL) oprintf ("%s=rim_strdup(rim_os_name());\n", to);
                        if (osuser !=NULL) 
                        {
                            oprintf ("{ struct passwd* pwd;\n");
                            oprintf ("if ((pwd = getpwuid (geteuid())) == NULL) rim_report_error(\"Cannot obtain current Operating System user name\");\n");
                            oprintf ("%s=rim_strdup (pwd->pw_name); }\n", to);
                        }
                        if (osdir !=NULL) 
                        {
                            oprintf ("{ struct passwd* pwd;\n");
                            oprintf ("if ((pwd = getpwuid (geteuid())) == NULL) rim_report_error(\"Cannot obtain current Operating System user name\");\n");
                            oprintf ("%s=rim_strdup (pwd->pw_dir); }\n", to);
                        }
                        if (osver !=NULL) oprintf ("%s=rim_strdup(rim_os_version());\n", to);
                        if (dir !=NULL) oprintf ("%s=rim_strdup(rim_get_config()->app.run_dir);\n", to);


                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "get-app", &mtext, &msize, 0, &rim_is_inline)) != 0)
                    {
                        RIM_GUARD
                        i = newI;
                        // Example:
                        // get-app name | version | directory  to [define] var

                        //
                        // Look for each option and collect relevant info
                        // First we MUST get each options position
                        //
                        char *user = find_keyword (mtext, RIM_KEYUSER, 1);
                        char *app = find_keyword (mtext, RIM_KEYAPPLICATION, 1);
                        char *udir = find_keyword (mtext, RIM_KEYUSERDIRECTORY, 1);
                        char *sock = find_keyword (mtext, RIM_KEYSOCKETFILE, 1);
                        char *appdir = find_keyword (mtext, RIM_KEYDIRECTORY, 1);
                        char *name = find_keyword (mtext, RIM_KEYNAME0, 1);
                        char *filedir = find_keyword (mtext, RIM_KEYFILEDIRECTORY, 1);
                        char *uploadsize = find_keyword (mtext, RIM_KEYUPLOADSIZE, 1);
                        char *dbconfname = find_keyword (mtext, RIM_KEYDBVENDOR, 1);
                        char *serv = find_keyword (mtext, RIM_KEYISSERVICE, 1);
                        char *apath = find_keyword (mtext, RIM_KEYPATH, 1);
                        char *sdep = find_keyword (mtext, RIM_KEYSTACKDEPTH, 1);

                        char *to = find_keyword (mtext, RIM_KEYTO, 1);

                        //
                        // After all options positions have been found, we must get the options
                        // for ALL of them
                        //
                        carve_statement (&name, "get-app", RIM_KEYNAME0, 0, 0);
                        carve_statement (&appdir, "get-app", RIM_KEYDIRECTORY, 0, 0);
                        carve_statement (&user, "get-app", RIM_KEYUSER, 0, 1);
                        carve_statement (&app, "get-app", RIM_KEYAPPLICATION, 0, 1);
                        carve_statement (&udir, "get-app", RIM_KEYUSERDIRECTORY, 0, 0);
                        carve_statement (&sock, "get-app", RIM_KEYSOCKETFILE, 0, 0);
                        carve_statement (&filedir, "get-app", RIM_KEYFILEDIRECTORY, 0, 0);
                        carve_statement (&uploadsize, "get-app", RIM_KEYUPLOADSIZE, 0, 0);
                        carve_statement (&dbconfname, "get-app", RIM_KEYDBVENDOR, 0, 1);
                        carve_statement (&serv, "get-app", RIM_KEYISSERVICE, 0, 0);
                        carve_statement (&apath, "get-app", RIM_KEYPATH, 0, 0);
                        carve_statement (&sdep, "get-app", RIM_KEYSTACKDEPTH, 0, 0);
                        carve_statement (&to, "get-app", RIM_KEYTO, 1, 1);
                        carve_stmt_obj (&mtext, false, false);

                        if ((user != NULL)  && udir == NULL && sock == NULL) rim_report_error ("user clause can only be used with user-directory or socket-file");
                        if (app != NULL  && sock == NULL) rim_report_error ("application clause can only be used with socket-file");

                        // result can be defined
                        if (uploadsize !=NULL || sdep != NULL) define_statement (&to, RIM_DEFNUMBER, false);
                        else if (serv !=NULL) define_statement (&to, RIM_DEFBOOL, false);
                        else define_statement (&to, RIM_DEFSTRING, false); // exact length set with strdup below

                        rim_num tot_opt = (serv!=NULL?1:0)+(appdir!=NULL?1:0)+(udir!=NULL?1:0)+(sock!=NULL?1:0)+(filedir!=NULL?1:0)+(apath!=NULL?1:0)+(uploadsize!=NULL?1:0)+(name!=NULL?1:0)+(dbconfname!=NULL?1:0)+(sdep!=NULL?1:0);
                        if (tot_opt != 1)
                        {
                            rim_report_error( "Exactly one option must be in get-app statement");
                        }

                        if (sock !=NULL) 
                        {
                            oprintf ("%s = rim_malloc(RIM_MAX_OS_UDIR_LEN);\n", to);
                            oprintf ("rim_dir (RIM_DIR_SOCKFILE, %s, RIM_MAX_OS_UDIR_LEN, %s, %s);\n", to, app==NULL?"NULL":app, user==NULL?"NULL":user); 
                        }
                        if (udir !=NULL) 
                        {
                            if (user == NULL && app == NULL) oprintf ("%s=rim_strdup(rim_get_config()->app.user_dir);\n", to);  // this user, this application
                            else 
                            {
                                oprintf ("%s = rim_malloc(RIM_MAX_OS_UDIR_LEN);\n", to);
                                oprintf ("rim_dir (RIM_DIR_USER, %s, RIM_MAX_OS_UDIR_LEN, NULL, %s);\n", to, user==NULL?"NULL":user); 
                            }
                        }
                        if (appdir !=NULL) oprintf ("%s=rim_strdup(rim_get_config()->app.home_dir);\n", to); 
                        if (filedir !=NULL) oprintf ("%s=rim_strdup(rim_get_config()->app.file_dir);\n", to); 
                        if (uploadsize !=NULL) oprintf ("%s=rim_get_config()->app.max_upload_size;\n", to); 
                        if (sdep !=NULL) oprintf ("%s=rim_sub_max_depth;\n", to); 
                        if (name !=NULL) oprintf ("%s=rim_strdup(rim_app_name);\n", to); 
                        if (serv !=NULL) oprintf ("%s=rim_is_service();\n", to);                        
                        if (dbconfname !=NULL) 
                        {
                            rim_num dbi = find_connection (dbconfname);
                            oprintf ("%s=rim_strdup(\"%s\");\n", to, rim_db_vendor(dbi));  // not alloced
                        }
                        if (apath !=NULL) oprintf ("%s=rim_strdup(rim_app_path);\n", to); // not alloced, const

                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "set-app", &mtext, &msize, 0, &rim_is_inline)) != 0)
                    {
                        RIM_GUARD
                        i = newI;
                        char *sdep = find_keyword (mtext, RIM_KEYSTACKDEPTH, 1);

                        carve_statement (&sdep, "set-app", RIM_KEYSTACKDEPTH, 0, 1);
                        carve_stmt_obj (&mtext, false, false);
                        // Check number 
                        rim_num st;
                        rim_num depth=rim_str2num (sdep, 0, &st);
                        if (st != RIM_OKAY) 
                        {
                            rim_report_error ("Stack depth must a positive number greater than %d", RIM_MIN_STACK_DEPTH);
                        }
                        else if (st == RIM_OKAY && depth < RIM_MIN_STACK_DEPTH) rim_report_error ("Stack depth cannot be lesser than %d", RIM_MIN_STACK_DEPTH);
                        oprintf ("rim_sub_max_depth = (%ld);\n", depth);


                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "get-req", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;
                        //
                        // Look for each option and collect relevant info
                        // First we MUST get each options position
                        //
                        char *sfile = find_keyword (mtext, RIM_KEYSOURCEFILE, 1);
                        char *ext = find_keyword (mtext, RIM_KEYEXTERNALCALL, 1);
                        char *dir = find_keyword (mtext, RIM_KEYDIRECTORY, 1);
                        char *name = find_keyword (mtext, RIM_KEYNAME0, 1);
                        char *method = find_keyword (mtext, RIM_KEYMETHOD, 1);
                        char *ctype = find_keyword (mtext, RIM_KEYCONTENTTYPE, 1);
                        char *eno = find_keyword (mtext, RIM_KEYERRNO, 1);
                        char *err = find_keyword (mtext, RIM_KEYERROR, 1);
                        char *process_id = find_keyword (mtext, RIM_KEYPROCESSID, 1);
                        char *argnum = find_keyword (mtext, RIM_KEYARGCOUNT, 1);
                        char *argval = find_keyword (mtext, RIM_KEYARGVALUE, 1);
                        char *ref = find_keyword (mtext, RIM_KEYREFERRINGURL, 1);
                        char *numcookie = find_keyword (mtext, RIM_KEYCOOKIECOUNT, 1);
                        char *cookie = find_keyword (mtext, RIM_KEYCOOKIE, 1);
                        char *header = find_keyword (mtext, RIM_KEYHEADER, 1);
                        char *to = find_keyword (mtext, RIM_KEYTO, 1);

                        //
                        // After all options positions have been found, we must get the options 
                        // for ALL of them
                        //
                        carve_statement (&sfile, "get-req", RIM_KEYSOURCEFILE, 0, 0);
                        carve_statement (&ext, "get-req", RIM_KEYEXTERNALCALL, 0, 0);
                        carve_statement (&name, "get-req", RIM_KEYNAME0, 0, 0);
                        carve_statement (&method, "get-req", RIM_KEYMETHOD, 0, 0);
                        carve_statement (&dir, "get-req", RIM_KEYDIRECTORY, 0, 0);
                        carve_statement (&ctype, "get-req", RIM_KEYCONTENTTYPE, 0, 0);
                        carve_statement (&argnum, "get-req", RIM_KEYARGCOUNT, 0, 0);
                        carve_statement (&argval, "get-req", RIM_KEYARGVALUE, 0, 1);
                        carve_statement (&ref, "get-req", RIM_KEYREFERRINGURL, 0, 0);
                        carve_statement (&numcookie, "get-req", RIM_KEYCOOKIECOUNT, 0, 0);
                        carve_statement (&cookie, "get-req", RIM_KEYCOOKIE, 0, 1);
                        carve_statement (&header, "get-req", RIM_KEYHEADER, 0, 1);
                        carve_statement (&process_id, "get-req", RIM_KEYPROCESSID, 0, 0);
                        carve_statement (&eno, "get-req", RIM_KEYERRNO, 0, 0);
                        carve_statement (&err, "get-req", RIM_KEYERROR, 0, 0);
                        carve_statement (&to, "get-req", RIM_KEYTO, 1, 1);
                        carve_stmt_obj (&mtext, false, false);

                        make_mem(&header);

                        // result can be defined
                        if (ext != NULL ) define_statement (&to, RIM_DEFBOOL, false);
                        else if (numcookie != NULL || argnum != NULL || method != NULL ) define_statement (&to, RIM_DEFNUMBER, false);
                        else if (ctype != NULL || argval != NULL || cookie != NULL || header != NULL || name != NULL || dir != NULL || sfile != NULL) 
                        {
                            define_statement (&to, RIM_DEFSTRING, false); // exact length set with strdup below
                        }
                        else if (eno != NULL) define_statement (&to, RIM_DEFNUMBER, false);
                        else if (process_id != NULL) define_statement (&to, RIM_DEFNUMBER, false);
                        else 
                        {
                            define_statement (&to, RIM_DEFSTRING, false); // exact length set with strdup below
                        }
                        check_var (&argval, RIM_DEFNUMBER);
                        check_var (&cookie, RIM_DEFNUMBER);
                        check_var (&header, RIM_DEFSTRING);

                        rim_num tot_opt = (argval!=NULL?1:0)+(ext!=NULL?1:0)+(argnum!=NULL?1:0)+(ref!=NULL?1:0)+(numcookie!=NULL?1:0)+(cookie!=NULL?1:0)+(header!=NULL?1:0)+(process_id!=NULL?1:0)+(eno!=NULL?1:0)+(err!=NULL?1:0)+(method!=NULL?1:0)+(dir!=NULL?1:0)+(ctype!=NULL?1:0)+(name!=NULL?1:0)+(sfile!=NULL?1:0);
                        if (tot_opt != 1)
                        {
                            rim_report_error( "Exactly one option must be in get-req statement (%ld)", tot_opt);
                        }

                        if (argval !=NULL) oprintf ("%s=rim_strdup(rim_req.args.args[%s-1]);\n", to, argval); 
                        if (argnum !=NULL) oprintf ("%s=rim_req.args.num_of_args;\n", to); // not alloced
                        if (ref !=NULL) oprintf ("%s=rim_strdup(rim_req.referring_url);\n", to); 
                        if (numcookie !=NULL) oprintf ("%s=rim_req.num_of_cookies;\n", to); 
                        if (cookie !=NULL) oprintf ("%s=rim_strdup(rim_req.cookies[%s-1].data);\n", to, cookie); 
                        if (process_id !=NULL) oprintf ("%s=getpid();\n", to); 
                        if (eno !=NULL) oprintf ("%s=rim_errno;\n", to); 
                        if (ext !=NULL) oprintf ("%s=(rim_req.sub_depth==1);\n", to); 
                        if (err !=NULL) oprintf ("%s=rim_strdup(strerror(rim_errno));\n", to); 
                        if (header !=NULL) oprintf ("%s=rim_strdup(rim_getheader(%s));\n", to, header); 
                        if (method !=NULL) oprintf ("%s=rim_req.method;\n", to); 
                        // get current directory will return empty string if it can't get it, and you can get errno from get-req (this is RIM_ERR below)
                        // we size the variable to be just what's needed, even though we get a lot more to begin with so getcwd() can succeed. That way
                        // there's no wasted memory and no fragmentation.
                        // Unfortunately we have to do strlen on the path, as there doesn't seem to be a way to get it. At least such memory is safe to handle.
                        if (dir !=NULL) oprintf ("{char *path=rim_malloc(RIM_MAX_PATH_LEN); char *rpath=getcwd(path, RIM_MAX_PATH_LEN);RIM_ERR;if (rpath==NULL) {%s=RIM_EMPTY_STRING;rim_free(path);} else {path = rim_realloc (rim_mem_get_id(path), strlen(path)+1); %s=path;}}\n", to, to); 
                        if (name !=NULL) oprintf ("%s=rim_strdup(rim_req.name);\n", to); 
                        if (sfile !=NULL) 
                        {
                            oprintf ("%s=rim_strdup(\"%s\");\n",to, rim_valid_rim_name); 
                        }
                        if (ctype !=NULL) oprintf ("%s=rim_strdup(rim_getenv(\"CONTENT_TYPE\"));\n", to); 


                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "abs-double", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;

                        char *to = find_keyword (mtext, RIM_KEYTO, 1);

                        carve_statement (&to, "abs-double", RIM_KEYTO, 1, 1); 
                        
                        carve_stmt_obj (&mtext, true, true);
                        define_statement (&to, RIM_DEFDOUBLE, false);
                        //
                        check_var (&mtext, RIM_DEFDOUBLE);

                        oprintf ("%s=abd((double)(%s));\n", to, mtext);

                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "abs-number", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;

                        char *to = find_keyword (mtext, RIM_KEYTO, 1);

                        carve_statement (&to, "abs-number", RIM_KEYTO, 1, 1); 
                        
                        carve_stmt_obj (&mtext, true, true);
                        define_statement (&to, RIM_DEFNUMBER, false);
                        //
                        check_var (&mtext, RIM_DEFNUMBER);

                        oprintf ("%s=llabs((long long)(%s));\n", to, mtext);

                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "string-double", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;

                        char *to = find_keyword (mtext, RIM_KEYTO, 1);
                        char *st = find_keyword (mtext, RIM_KEYSTATUS, 1);

                        carve_statement (&to, "string-double", RIM_KEYTO, 1, 1); 
                        carve_statement (&st, "string-double", RIM_KEYSTATUS, 0, 1);
                        
                        carve_stmt_obj (&mtext, true, true);
                        define_statement (&to, RIM_DEFDOUBLE, false);
                        define_statement (&st, RIM_DEFNUMBER, false);
                        //
                        check_var (&mtext, RIM_DEFSTRING);

                        oprintf ("%s=rim_str2dbl (%s, &_rim_st);\n", to, mtext);
                        if (st != NULL) oprintf("%s=_rim_st;\n",st); else oprintf("if (_rim_st != RIM_OKAY) rim_report_error (\"%s\", \"%s\", (rim_num)%ld, _rim_st);\n", RIM_STMT_FAIL_CHECK, rim_valid_rim_name, lnum);


                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "string-number", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;

                        char *to = find_keyword (mtext, RIM_KEYTO, 1);
                        char *base = find_keyword (mtext, RIM_KEYBASE, 1);
                        char *st = find_keyword (mtext, RIM_KEYSTATUS, 1);

                        carve_statement (&to, "string-number", RIM_KEYTO, 1, 1); 
                        carve_statement (&base, "string-number", RIM_KEYBASE, 0, 1); 
                        carve_statement (&st, "string-number", RIM_KEYSTATUS, 0, 1);
                        
                        carve_stmt_obj (&mtext, true, true);
                        define_statement (&to, RIM_DEFNUMBER, false);
                        define_statement (&st, RIM_DEFNUMBER, false);
                        //
                        check_var (&mtext, RIM_DEFSTRING);

                        oprintf ("%s=rim_str2num (%s, %s, &_rim_st);\n", to, mtext, base!=NULL?base:"0");
                        if (st != NULL) oprintf("%s=_rim_st;\n",st); else oprintf("if (_rim_st != RIM_OKAY) rim_report_error (\"%s\", \"%s\", (rim_num)%ld, _rim_st);\n", RIM_STMT_FAIL_CHECK, rim_valid_rim_name, lnum);


                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "double-string", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;

                        char *to = find_keyword (mtext, RIM_KEYTO, 1);
                        char *width = find_keyword (mtext, RIM_KEYWIDTH, 1);
                        char *prec = find_keyword (mtext, RIM_KEYPRECISION, 1);
                        char *ffloat = find_keyword (mtext, RIM_KEYFORMATFLOATING, 1);
                        char *fsci = find_keyword (mtext, RIM_KEYFORMATSCIENTIFIC, 1);
                        char *fcomp = find_keyword (mtext, RIM_KEYFORMATCOMPACT, 1);
                        carve_statement (&to, "double-string", RIM_KEYTO, 0, 1);  
                        carve_statement (&width, "double-string", RIM_KEYWIDTH, 0, 1);  
                        carve_statement (&prec, "double-string", RIM_KEYPRECISION, 0, 1);  
                        carve_statement (&ffloat, "double-string", RIM_KEYFORMATFLOATING, 0, 0);  
                        carve_statement (&fsci, "double-string", RIM_KEYFORMATSCIENTIFIC, 0, 0);  
                        carve_statement (&fcomp, "double-string", RIM_KEYFORMATCOMPACT, 0, 0);  
                        carve_stmt_obj (&mtext, true, false);
                        define_statement (&to, RIM_DEFSTRING, false); // exact length set in rim_num2str
                        check_var (&mtext, RIM_DEFDOUBLE);

                        rim_num w, p;
                        if (width != NULL) 
                        {
                            rim_num st;
                            w =rim_str2num (width, 0, &st);
                        } else w = -1;
                        if (prec != NULL) 
                        {
                            rim_num st;
                            p =rim_str2num (prec, 0, &st);
                        } else p = -1;

                        do_numstr (to, mtext, NULL, true, w, p, ffloat==NULL?(fsci==NULL?(fcomp==NULL?"f":"g"):"e"):"f"); // _rim_st is produced inside do_numstr(), it's 0 if failed

                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "number-string", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;

                        char *to = find_keyword (mtext, RIM_KEYTO, 1);
                        char *base = find_keyword (mtext, RIM_KEYBASE, 1);
                        char *st = find_keyword (mtext, RIM_KEYSTATUS, 1);

                        carve_statement (&to, "number-string", RIM_KEYTO, 0, 1);  
                        carve_statement (&base, "number-string", RIM_KEYBASE, 0, 1); 
                        carve_statement (&st, "number-string", RIM_KEYSTATUS, 0, 1); 
                        carve_stmt_obj (&mtext, true, false);
                        define_statement (&to, RIM_DEFSTRING, false); // exact length set in rim_num2str
                        define_statement (&st, RIM_DEFNUMBER, false);
                        check_var (&mtext, RIM_DEFNUMBER);
                        check_var (&base, RIM_DEFNUMBER);

                        //Example for deprecation: length is old, buffer-size new

                        char *num0 = mtext; // number to convert, cannot use 'num' for var name as it is a type (solved now with rim_num)


                        do_numstr (to, num0, base, false, -1, -1, NULL); // _rim_st is produced inside do_numstr(), it's 0 if failed
                        if (st!=NULL) oprintf("if (_rim_st==0) (%s)=RIM_ERR_FAILED; else (%s)=RIM_OKAY;\n", st, st);
                        else oprintf("if (_rim_st == 0) rim_report_error (\"%s\", \"%s\", (rim_num)%ld, (rim_num)RIM_ERR_FAILED);\n", RIM_STMT_FAIL_CHECK, rim_valid_rim_name, lnum);

                        continue;
                    }
                    else if ((newI1=recog_statement(line, i, "extended-mode", &mtext, &msize, 1, &rim_is_inline)) != 0)
                    {
                        //No checking if this is within a block, since this is outside any code and doesn't generate code
                        //RIM_GUARD
                        i = newI1;

                        carve_stmt_obj (&mtext, false, false);
                        //
                        //
                        if (newI1 != 0) rim_mode = RIM_MODE_EXTENDED;

                        if (num_recog > 1) rim_report_error ("extended-mode must be the very first statement in source file");

                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "flush-output", &mtext, &msize, 1, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;

                        carve_stmt_obj (&mtext, false, false);
                        oprintf ("rim_flush_out();\n");

                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "replace-string", &mtext, &msize, 0, &rim_is_inline)) != 0)
                    {
                        RIM_GUARD
                        i = newI;

                        char *copy = find_keyword (mtext, RIM_KEYCOPY, 1);
                        char *copyend = find_keyword (mtext, RIM_KEYCOPYEND, 1);
                        char *swith = find_keyword (mtext, RIM_KEYSTARTWITH, 1);
                        char *len = find_keyword (mtext, RIM_KEYLENGTH, 1);
                        carve_statement (&copy, "replace-string", RIM_KEYCOPY, 0, 1);
                        carve_statement (&copyend, "replace-string", RIM_KEYCOPYEND, 0, 1);
                        carve_statement (&swith, "replace-string", RIM_KEYSTARTWITH, 0, 1);
                        carve_statement (&len, "replace-string", RIM_KEYLENGTH, 0, 1);
                        carve_stmt_obj (&mtext, true, true); // this has make_mem built in
                        //
                        make_mem(&copy);
                        make_mem(&copyend);
                        //
                        check_var (&mtext, RIM_DEFSTRING);
                        check_var (&copy, RIM_DEFSTRING);
                        check_var (&copyend, RIM_DEFSTRING);
                        check_var (&swith, RIM_DEFNUMBER);
                        check_var (&len, RIM_DEFNUMBER);

                        if ((copy == NULL && copyend == NULL) || (copy != NULL && copyend != NULL)) rim_report_error ("there must be either 'copy' or copy-end clause present");

                        oprintf ("rim_alter_string (%s, %s, %s, %s, %s);\n", mtext, copy==NULL?copyend:copy, swith==NULL?"-1":swith, len==NULL?"-1":len, copy==NULL?"false":"true");

                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "scan-string", &mtext, &msize, 0, &rim_is_inline)) != 0)
                    {
                        RIM_GUARD
                        i = newI;

                        char *st = find_keyword (mtext, RIM_KEYSTATUS, 1);
                        char *fors = find_keyword (mtext, RIM_KEYFOR, 1);
                        char *swith = find_keyword (mtext, RIM_KEYSTARTWITH, 1);
                        char *len = find_keyword (mtext, RIM_KEYLENGTH, 1);
                        carve_statement (&fors, "scan-string", RIM_KEYFOR, 1, 1);
                        carve_statement (&swith, "scan-string", RIM_KEYSTARTWITH, 0, 1);
                        carve_statement (&len, "scan-string", RIM_KEYLENGTH, 0, 1);
                        carve_statement (&st, "scan-string", RIM_KEYSTATUS, 1, 1);
                        carve_stmt_obj (&mtext, true, true);
                        make_mem(&fors); // make_mem *must* be used right after carves for any input string object!!!!
                        define_statement (&st, RIM_DEFNUMBER, false); // exact length set in rim_copy_string
                        //
                        check_var (&mtext, RIM_DEFSTRING);
                        check_var (&swith, RIM_DEFNUMBER);
                        check_var (&len, RIM_DEFNUMBER);
                        rim_num ftype = check_var (&fors, RIM_DEFUNKN);

                        // check validity of start-with and length
                        if (swith != NULL) oprintf ("if ((%s)<0) rim_report_error (\"start-with is negative [%%ld]\", %s);\n ", swith, swith);
                        if (len != NULL) oprintf ("if ((%s)<0) rim_report_error (\"length is negative [%%ld]\", %s);\n ", len, len);
                        if (len != NULL) oprintf ("if ((%s)+(%s) > rim_mem_get_len(%s)) rim_report_error (\"Scan area in string extends to byte [%%ld] with string limit [%%ld]\", (%s)+(%s), rim_mem_get_len(%s));\n ", swith==NULL?"0":swith, len, mtext, swith==NULL?"0":swith, len, mtext);
                        if (cmp_type (ftype, RIM_DEFNUMBER)) // searching for 'a'
                        {
                            oprintf ("{ ");
                            if (len != NULL) oprintf ("char *_rim_res = memchr (%s+(%s), (char)(%s), (%s));", mtext, swith==NULL?"0":swith, fors, len);
                            else oprintf ("char *_rim_res = memchr (%s+(%s), (char)(%s), rim_mem_get_len(%s)-(%s));", mtext, swith==NULL?"0":swith, fors, mtext, swith==NULL?"0":swith);
                            oprintf ("if (_rim_res == NULL) %s=RIM_ERR_POSITION; else %s=_rim_res-(%s); }\n", st, st, mtext); // return pos from the beginning of the string
                        }
                        else if (cmp_type (ftype, RIM_DEFSTRING)) // searching for "string"
                        {
                            oprintf ("{ ");
                            if (len != NULL) oprintf ("char *_rim_res = memmem (%s+(%s), (%s), %s, rim_mem_get_len(%s));", mtext, swith==NULL?"0":swith, len, fors, fors);
                            else oprintf ("char *_rim_res = memmem (%s+(%s), rim_mem_get_len(%s)-(%s), %s, rim_mem_get_len(%s));", mtext, swith==NULL?"0":swith, mtext, swith==NULL?"0":swith, fors, fors);
                            oprintf ("if (_rim_res == NULL) %s=RIM_ERR_POSITION; else %s=_rim_res-(%s); }\n", st, st, mtext); // return pos from the beginning of the string
                        } else rim_report_error ("'for' clause must provide either a string or a character (i.e. a number), found [%s]", fors);

                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "copy-string", &mtext, &msize, 0, &rim_is_inline)) != 0)
                    {
                        RIM_GUARD
                        i = newI;

                        char *to = find_keyword (mtext, RIM_KEYTO, 1);
                        char *swith = find_keyword (mtext, RIM_KEYSTARTWITH, 1);
                        char *len = find_keyword (mtext, RIM_KEYLENGTH, 1);
                        carve_statement (&to, "copy-string", RIM_KEYTO, 1, 1);
                        carve_statement (&swith, "copy-string", RIM_KEYSTARTWITH, 0, 1);
                        carve_statement (&len, "copy-string", RIM_KEYLENGTH, 0, 1);
                        carve_stmt_obj (&mtext, true, true);
                        define_statement (&to, RIM_DEFSTRING, false); // exact length set in rim_copy_string
                        //
                        check_var (&mtext, RIM_DEFSTRING);
                        check_var (&swith, RIM_DEFNUMBER);
                        check_var (&len, RIM_DEFNUMBER);

                        // must assign NULL, or it would assume cval is rim_string, which it may not be
                        if (len == NULL) oprintf ("%s = rim_strdupl (%s, %s, rim_mem_get_len(%s));\n", to, mtext, swith==NULL?"0":swith, mtext); // exact length copied, so works for binary exactly
                        else oprintf ("rim_copy_string (%s, %s, &(%s), %s);\n", mtext, swith==NULL?"0":swith, to, len);

                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "delete-cookie", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;
                        char *path = find_keyword (mtext, RIM_KEYPATH, 1);
                        char *st = find_keyword (mtext, RIM_KEYSTATUS, 1);
                        char *secure = find_keyword (mtext, RIM_KEYSECURE, 1);

                        carve_statement (&secure, "delete-cookie", RIM_KEYSECURE, 0, 2);
                        carve_statement (&path, "delete-cookie", RIM_KEYPATH, 0, 1);
                        carve_statement (&st, "delete-cookie", RIM_KEYSTATUS, 0, 1);
                        carve_stmt_obj (&mtext, true, true);
                        make_mem(&path);
                        define_statement (&st, RIM_DEFNUMBER, false);
                        char *secc = opt_clause(secure, "\"Secure; \"", "\"\"", RIM_DEFSTRING);
                        check_var (&mtext, RIM_DEFSTRING);
                        check_var (&path, RIM_DEFSTRING);
                        check_var (&secc, RIM_DEFSTRING);


                        oprintf("_rim_st=(rim_delete_cookie (&rim_req, %s, %s, %s)<0?RIM_ERR_EXIST:RIM_OKAY);\n", mtext, path==NULL?"NULL":path, secc ); 
                        if (st != NULL) oprintf("%s=_rim_st;\n",st); else oprintf("if (_rim_st != RIM_OKAY) rim_report_error (\"%s\", \"%s\", (rim_num)%ld, _rim_st);\n", RIM_STMT_FAIL_CHECK, rim_valid_rim_name, lnum);
                        rim_free(secc);

                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "request-body", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;
                        carve_stmt_obj (&mtext, true, false);
                        if (rim_is_valid_param_name(mtext, false, false) != 1) rim_report_error(RIM_NAME_INVALID, mtext);
                        define_statement (&mtext, RIM_DEFSTRING, true);

                        oprintf ("rim_mem_add_ref(rim_req.body, 0);\n");
                        oprintf ("%s = rim_req.body;\n\n", mtext);

                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "set-cookie", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;

                        while (1) 
                        {
                            char *comma = find_keyword (mtext, RIM_KEYCOMMA, 0);
                            carve_statement (&comma, "set-cookie", RIM_KEYCOMMA, 0, 1); // separate entries with comma first, so
                                                                                      // this find/carve MUST come before the rest below!
                                                                                      //
                            char *exp = find_keyword (mtext, RIM_KEYEXPIRES, 1);
                            char *path = find_keyword (mtext, RIM_KEYPATH, 1);
                            char *samesite = find_keyword (mtext, RIM_KEYSAMESITE, 1);
                            char *nohttponly = find_keyword (mtext, RIM_KEYNOHTTPONLY, 1);
                            char *secure = find_keyword (mtext, RIM_KEYSECURE, 1);
                            char *eq = find_keyword (mtext, RIM_KEYEQUALSHORT, 0);

                            carve_statement (&exp, "set-cookie", RIM_KEYEXPIRES, 0, 1);
                            carve_statement (&path, "set-cookie", RIM_KEYPATH, 0, 1);
                            carve_statement (&samesite, "set-cookie", RIM_KEYSAMESITE, 0, 1);
                            carve_statement (&nohttponly, "set-cookie", RIM_KEYNOHTTPONLY, 0, 2);
                            carve_statement (&secure, "set-cookie", RIM_KEYSECURE, 0, 2); // may have data
                            carve_statement (&eq, "set-cookie", RIM_KEYEQUALSHORT, 1, 1);
                            carve_stmt_obj (&mtext, true, true);
                            make_mem(&exp);
                            make_mem(&path);
                            make_mem(&eq);
                            make_mem(&samesite);
                            char *secc = opt_clause(secure, "\"Secure; \"", "\"\"", RIM_DEFSTRING);
                            char *httpc = opt_clause(nohttponly, "\"\"", "\"HttpOnly; \"", RIM_DEFSTRING);
                            //
                            check_var (&mtext, RIM_DEFSTRING);
                            check_var (&eq, RIM_DEFSTRING);
                            check_var (&exp, RIM_DEFSTRING);
                            check_var (&path, RIM_DEFSTRING);
                            check_var (&samesite, RIM_DEFSTRING);
                            check_var (&httpc, RIM_DEFSTRING);
                            check_var (&secc, RIM_DEFSTRING);


                            // enforce that Strict is the default for SameSite and HttpOnly is the default
                            // set-cookie creates its own memory, so no need to increase refcount of mtext
                            oprintf("rim_set_cookie (&rim_req, %s, %s, %s, %s, %s, %s, %s);\n", mtext, eq,
                                path == NULL ? "NULL" : path, exp == NULL ? "NULL" : exp, samesite == NULL ? "\"Strict\"" : samesite,
                                httpc, secc);

                            rim_free(secc);
                            rim_free(httpc);
                            if (comma == NULL) break; else mtext = comma;
                        }

                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "get-cookie", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;
                        while (1) 
                        {
                            char *comma = find_keyword (mtext, RIM_KEYCOMMA, 0);
                            carve_statement (&comma, "get-cookie", RIM_KEYCOMMA, 0, 1); // separate entries with comma first, so
                                                                                      // this find/carve MUST come before the rest below!
                                                                                      //
                            char *eq = find_keyword (mtext, RIM_KEYEQUALSHORT, 0);
                            if (eq != NULL) carve_statement (&eq, "get-cookie", RIM_KEYEQUALSHORT, 1, 1);
                            carve_stmt_obj (&mtext, true, false);
                            make_mem(&eq);

                            define_statement (&mtext, RIM_DEFSTRING, false); // exact length set via strdup in rim_find_cookie
                            check_var (&eq, RIM_DEFSTRING);

                            // memory assigned is created in rim_find_cookie, so it has refcount of 0 to begin with, just like
                            // any other statement
                            oprintf("%s = rim_find_cookie (&rim_req, %s, NULL, NULL, NULL);\n", mtext, eq); 
                            if (comma == NULL) break; else mtext = comma;
                        }

                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "get-param", &mtext, &msize, 0, &rim_is_inline)) != 0)
                    {
                        RIM_GUARD
                        i = newI;

                        process_param_get (mtext);
                        //
                        // RIM_KEYCOMMA RIM_KEYDEFAULTVALUE RIM_KEYTYPE 
                        // Make vim recognize type names
                        //
                        // RIM_KEY_T_STRING RIM_KEY_T_BOOL RIM_KEY_T_NUMBER RIM_KEY_T_DOUBLE RIM_KEY_T_MESSAGE RIM_KEY_T_SPLITSTRING RIM_KEY_T_HASH RIM_KEY_T_ARRAYSTRING RIM_KEY_T_ARRAYNUMBER RIM_KEY_T_ARRAYBOOL RIM_KEY_T_ARRAYDOUBLE RIM_KEY_T_TREE RIM_KEY_T_TREECURSOR RIM_KEY_T_FIFO RIM_KEY_T_LIFO RIM_KEY_T_LIST RIM_KEY_T_ENCRYPT RIM_KEY_T_FILE RIM_KEY_T_SERVICE 

                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "set-param", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;

                        process_param_set (mtext);


                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "end-handler", &mtext, &msize, 1, &rim_is_inline)) != 0 ||
                        (newI1=recog_statement(line, i, "%%", &mtext, &msize, 1, &rim_is_inline)) != 0)  
                    {
                        //No checking if this is within a block, since this is function ending
                        //RIM_GUARD
                        i = newI + newI1;
                        carve_stmt_obj (&mtext, false, false);

                        if (!done_handler)
                        {
                            rim_report_error( "end-handler without begin-handler found");
                        }
                        if (rim_single_file && done_end_handler)
                        {
                            rim_report_error( "end-handler found the second time");
                        }
                        done_end_handler = true;
                        oprintf("return 0;\n}\n");
                        if (open_curly != 0) rim_report_error ("Imbalance in blocks opening and closing {}");
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "after-handler", &mtext, &msize, 1, &rim_is_inline)) != 0)
                        {
                        //RIM_GUARD
                        i = newI;
                        carve_stmt_obj (&mtext, false, false);
                        match_file (file_name, "after_handler");
                        if (after_ended || after_started) rim_report_error ("after-handler already found");
                        after_started = true;
                        done_handler = true; // avoid no request handler implemented error
                        oprintf ("void after_handler () {\n");
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "before-handler", &mtext, &msize, 1, &rim_is_inline)) != 0)
                        {
                        //RIM_GUARD
                        i = newI;
                        carve_stmt_obj (&mtext, false, false);
                        match_file (file_name, "before_handler");
                        if (before_ended || before_started) rim_report_error ("before-handler already found");
                        before_started = true;
                        done_handler = true; // avoid no request handler implemented error
                        oprintf ("void before_handler () {\n");
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "end-after-handler", &mtext, &msize, 1, &rim_is_inline)) != 0)
                        {
                        //RIM_GUARD
                        i = newI;
                        carve_stmt_obj (&mtext, false, false);
                        if (!after_started || after_ended) rim_report_error ("found end-after-handler without after-handler to begin with, or already found prior to here");
                        after_ended = true;
                        oprintf ("}\n");
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "end-before-handler", &mtext, &msize, 1, &rim_is_inline)) != 0)
                    {
                        //RIM_GUARD
                        i = newI;
                        carve_stmt_obj (&mtext, false, false);
                        if (!before_started || before_ended) rim_report_error ("found end-before-handler without before-handler to begin with, or already found prior to here");
                        before_ended = true;
                        oprintf ("}\n");
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "begin-handler", &mtext, &msize, 0, &rim_is_inline)) != 0 || 
                        (newI1=recog_statement(line, i, "%%", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        //No checking if this is within a block, since this is function ending
                        //RIM_GUARD
                        i = newI+newI1;

                        open_curly = 0;

                        // do not allow splitting of begin-handler, because this allows us to quickly get .h file which contains
                        // all prototypes of handlers in gg (in gen_src) - without this we'd have to make a mess and detect \ and concatenate lines in bash
                        if (line_split) rim_report_error ("begin-handler (or %%) statement cannot be split on multiple lines");

                        if (rim_single_file && done_handler)
                        {
                            rim_report_error( ".rim file can have only a single begin-handler");
                        }
                        char *priv = find_keyword (mtext, RIM_KEYPRIVATE, 0);
                        char *pub = find_keyword (mtext, RIM_KEYPUBLIC, 0);
                        char *gpar = find_keyword (mtext, RIM_KEYGETPARAM, 1);
                        carve_statement (&priv, "begin-handler", RIM_KEYPRIVATE, 0, 0);
                        carve_statement (&pub, "begin-handler", RIM_KEYPUBLIC, 0, 0);
                        carve_statement (&gpar, "call-handler", RIM_KEYGETPARAM, 0, 1);



                        done_handler = true;
                        rim_num mlen = carve_stmt_obj (&mtext, true, false);
                        static char reqname[RIM_MAX_REQ_NAME_LEN];

                        if (mtext[0] != '/') rim_report_error ("request path must start with forward slash ('/')");
                        if (priv != NULL && pub != NULL) rim_report_error("Service handler [%s] cannot be both public and private", mtext);
                        // this handles what if neither public nor private specified
                        if (priv == NULL && pub == NULL)
                        {
                            if (rim_public == false) priv="private"; // private handler is by default
                            else pub = "public";  // this is if rim_public is true, which is --public in gg, so then it's public
                        }

                        char *p = mtext; // because rim_decorate_path() will set it to NULL if there's /
                        char decres = rim_decorate_path (reqname, sizeof(reqname), &p, mlen);
                        // 1 means good hierarchical path, reqname is it; or 3 means no /, so reqname is a copy of mtext
                        if (decres != 1) rim_report_error( "request path in begin-handler is not valid");
                        rim_num plen = strlen (reqname); // otherwise whitespace at the end would cause mismatch
                        rim_trim (reqname, &plen, false);

                        if (rim_is_valid_param_name(reqname, false, false) != 1) rim_report_error( "request path in begin-handler is not valid, it can have only alphanumeric characters, hyphens, underscores and forward slashes, and must start with an alphabet character");

                        if (is_reserved(reqname)) rim_report_error ("begin-handler name cannot be a C reserved word [%s]", reqname);

                        match_file (file_name, reqname);

                        oprintf ("rim_num %s () {\n", reqname);
                        if (priv != NULL)
                        {
                            oprintf ("if (rim_req.sub_depth==1) rim_report_error(\"This service [%s] can only be called as call-handler. If you wish to call this handler from an outside caller, either use 'public' in begin-handler, or to make all handlers public, use '--public' with '-q' when making the application.\");\n", mtext);
                            rim_is_sub = true;
                        }
                        // get-param *before* the begin-handler begins but after the beginning of function impl
                        if (gpar != NULL)
                        {
                            process_param_get (gpar);
                            //
                            // RIM_KEYCOMMA RIM_KEYDEFAULTVALUE RIM_KEYTYPE 
                            // Make vim recognize type names
                            //
                            // RIM_KEY_T_STRING RIM_KEY_T_BOOL RIM_KEY_T_NUMBER RIM_KEY_T_DOUBLE RIM_KEY_T_MESSAGE RIM_KEY_T_SPLITSTRING RIM_KEY_T_HASH RIM_KEY_T_ARRAYSTRING RIM_KEY_T_ARRAYNUMBER RIM_KEY_T_ARRAYBOOL RIM_KEY_T_ARRAYDOUBLE RIM_KEY_T_TREE RIM_KEY_T_TREECURSOR RIM_KEY_T_FIFO RIM_KEY_T_LIFO RIM_KEY_T_LIST RIM_KEY_T_ENCRYPT RIM_KEY_T_FILE RIM_KEY_T_SERVICE 
                        }

                        continue;
                    }
                    else if ((newI=recog_statement (line, i, "set-bool", &mtext, &msize, 0, &rim_is_inline)) != 0)
                    {
                        RIM_GUARD
                        i = newI;
                        char *eq = find_keyword (mtext, RIM_KEYEQUALSHORT, 0);
                        char *ps = find_keyword (mtext, RIM_KEYPROCESSSCOPE, 0);
                        carve_statement (&eq, "set-bool", RIM_KEYEQUALSHORT, 0, 1);
                        carve_statement (&ps, "set-bool", RIM_KEYPROCESSSCOPE, 0, 0);
                        carve_stmt_obj (&mtext, true, false);
                        rim_num is_def;
                        // RIM_KEYAND0 RIM_KEYOR0 RIM_KEYNOT0 
                        // if defined, it's always false
                        if (ps != NULL) is_def = define_statement (&mtext, RIM_DEFBOOLSTATIC, false);
                        else is_def = define_statement (&mtext, RIM_DEFBOOL, false);

                        // this is if not defined, and no equal, set to false
                        static char empty[] = "false"; //[] so trim will work on it in check_var
                        if (eq == NULL) eq = empty;

                        if (ps && is_def != 1) rim_report_error ("process-scope can only be used when variable is created");

                        char *equal = NULL;
                        char *neq = NULL;
                        char *less = NULL;
                        char *lesseq = NULL;
                        char *gr = NULL;
                        char *mod = NULL;
                        char *notmod = NULL;
                        char *greq = NULL;
                        char *cont = NULL;
                        char *notcont = NULL;
                        find_cond (eq, &equal, &neq, &less, &lesseq, &gr, &greq, &mod, &notmod, &cont, &notcont);
                        bool is_cond = equal || neq || less || lesseq || gr || greq || mod || notmod || cont || notcont;
                        // check correctness
                        if (!is_cond)
                        {
                            trimit (eq);
                            check_var (&eq, RIM_DEFBOOL);
                        }

                        //
                        if (ps)
                        {
                            // with process scope, initialization happens only once, like with static, except we do assignment
                            // to be able to "initialize" with a variable and also conditionally with if-true
                            oprintf("static bool _rim_nstat%ld = true;\n", rim_pscope);
                            oprintf("if (_rim_nstat%ld) {\n_rim_nstat%ld = false;\n", rim_pscope, rim_pscope);
                        }
                        if (eq != NULL) 
                        {
                            if (!is_cond) oprintf ("%s = %s;\n", mtext, eq);
                            else 
                            {
                                // RIM_KEYEVERY RIM_KEYNOTEVERY RIM_KEYEQUAL RIM_KEYNOTEQUAL RIM_KEYOR RIM_KEYAND RIM_KEYLESSERTHAN RIM_KEYLESSEREQUAL RIM_KEYGREATERTHAN RIM_KEYGREATEREQUAL RIM_KEYCONTAIN RIM_KEYNOTCONTAIN RIM_KEYCASEINSENSITIVE KEYERRORMARGIN RIM_KEYLENGTH
                                char ifs[300];
                                snprintf(ifs, sizeof(ifs), "%s=(", mtext);
                                parse_cond (eq, "", ifs, ");\n");
                            }
                        }
                        if (ps)
                        {
                            // with process scope, initialization happens only once, like with static, except we do assignment
                            // to be able to "initialize" with a variable and also conditionally with if-true
                            if (eq) oprintf("}\n"); 
                            rim_pscope++;
                        }
                        continue;
                    }
                    else if ((newI=recog_statement (line, i, "set-number", &mtext, &msize, 0, &rim_is_inline)) != 0
                     || (newI1=recog_statement (line, i, "set-double", &mtext, &msize, 0, &rim_is_inline)) != 0)
                    {
                        RIM_GUARD
                        i = newI+newI1;
                        char *eq = find_keyword (mtext, RIM_KEYEQUALSHORT, 0);
                        char *ps = find_keyword (mtext, RIM_KEYPROCESSSCOPE, 0);
                        carve_statement (&eq, newI?"set-number":"set-double", RIM_KEYEQUALSHORT, 0, 1); // not mandatory because when add/multiply-by used, there's no =
                        carve_statement (&ps, newI?"set-number":"set-double", RIM_KEYPROCESSSCOPE, 0, 0);
                        carve_stmt_obj (&mtext, true, false);
                        rim_num is_def;
                        // by default always 0
                        if (ps != NULL) is_def = define_statement (&mtext, newI?RIM_DEFNUMBERSTATIC:RIM_DEFDOUBLESTATIC, false);
                        else is_def = define_statement (&mtext, newI?RIM_DEFNUMBER:RIM_DEFDOUBLE, false);


                        // if not defined, default is 0
                        static char empty[] = "0"; //[] so trim will work on it in check_var
                        if (eq == NULL) eq = empty;

                        //
                        check_var (&eq, newI?RIM_DEFNUMBER:RIM_DEFDOUBLE);


                        if (ps && is_def != 1) rim_report_error ("process-scope can only be used when variable is created");


                        //
                        // Rules:
                        // process-scope must be when variable is created



                        // always init to zero when created
                        // init process-scope with guardian bool only if there's equal, otherwise it's set to 0
                        if (ps)
                        {
                            // with process scope, initialization happens only once, like with static, except we do assignment
                            // to be able to "initialize" with a variable and also conditionally with if-true
                            oprintf("static bool _rim_nstat%ld = true;\n", rim_pscope);
                            oprintf("if (_rim_nstat%ld) {\n_rim_nstat%ld = false;\n", rim_pscope, rim_pscope);
                        }
                        // based on above exclusions, only one will fire
                        // this is in any case, with or without variable creation or if-true
                        // no assignment if process scope (static)
                        oprintf ("%s = %s%s;\n", mtext, newI?"":"(double)",eq);

                        if (ps)
                        {
                            // with process scope, initialization happens only once, like with static, except we do assignment
                            // to be able to "initialize" with a variable and also conditionally with if-true
                            if (eq) oprintf("}\n"); 
                            rim_pscope++;
                        }
                        continue;
                    }
                    else if ((newI=recog_statement (line, i, "new-string", &mtext, &msize, 0, &rim_is_inline)) != 0)
                    {
                        RIM_GUARD
                        i = newI;
                        char *len = find_keyword (mtext, RIM_KEYLENGTH, 0);
                        carve_statement (&len, "new-string", RIM_KEYLENGTH, 1, 1);
                        carve_stmt_obj (&mtext, true, false);
                        define_statement (&mtext, RIM_DEFSTRING, false); // correct as it uses RIM_EMPTY_STRING for empty init
                        rim_num st;
                        rim_num size=rim_str2num (len, 0, &st);
                        if (st == RIM_OKAY && size <= 0) rim_report_error ("Length of a new string cannot be lesser than or equal to 0");
                        if (st == RIM_OKAY && size < 1024) 
                        {
                            char *cs = rim_malloc (size+3); // +1 for end null, +2 for 2 quotes, plus 1 charm
                            cs[0] = '"';
                            rim_num i;
                            for (i = 1; i < size+1; i++) cs[i] = ' ';
                            cs[i] = '"';
                            cs[i+1] = 0;
                            make_mem (&cs);
                            // no need to add reference, this is new memory
                            oprintf ("%s=%s;\n%s[0]=0;\n", mtext,cs,mtext); // string is empty each time execution passes through new-string
                        }
                        else
                        {
                            // check_var is here and above, since it produces a variable, so len would be _rim_d...
                            check_var (&len, RIM_DEFNUMBER);
                            oprintf ("%s = (char*)rim_malloc ((%s)+1);\n%s[0] = 0;\n", mtext, len, mtext); // again, string is empty with each passing through
                        }
                        continue;
                    }
                    else if ((newI=recog_statement (line, i, "set-string", &mtext, &msize, 0, &rim_is_inline)) != 0)
                    {
                        // TODO: set-unquoted-string which is the same as set-string but there's not surrounding quotes
                        // and all quotes and any escape chars are not honored. This will NOT have "x" "y" capability for
                        // concatenation of literals for obvious reason that there is no notion of a literal boundary other than
                        // the end of the line.
                        RIM_GUARD
                        i = newI;
                        char *unq = find_keyword (mtext, RIM_KEYUNQUOTED, 0);
                        char *eq = find_keyword (mtext, RIM_KEYEQUALSHORT, 0);
                        char *ps = find_keyword (mtext, RIM_KEYPROCESSSCOPE, 0);
                        char *len = find_keyword (mtext, RIM_KEYLENGTH, 0);
                        carve_statement (&eq, "set-string", RIM_KEYEQUALSHORT, 0, 1);
                        carve_statement (&ps, "set-string", RIM_KEYPROCESSSCOPE, 0, 0);
                        carve_statement (&unq, "set-string", RIM_KEYUNQUOTED, 0, 0);
                        carve_statement (&len, "set-string", RIM_KEYLENGTH, 0, 1);
                        carve_stmt_obj (&mtext, true, false);
                        rim_num is_def;
                        //BEFORE make_mem or check_var, eq is checked if it's a constant; if so there's no need to add ref to it
                        //unquoted would of course fail is_constant_string(), so by checking first for it below we avoid going into that failure
                        bool eq_const = (unq != NULL || eq == NULL || is_constant_string (eq, NULL) == RIM_CONST_OK);
                        //
                        //

                        bool ps_cre = (ps != NULL);


                        if (len != NULL)
                        {
                            if (eq != NULL || ps_cre  || unq != NULL) rim_report_error ("Other clauses cannot be used with 'length' clause'");
                            check_var (&len, RIM_DEFNUMBER);
                            if (rim_is_valid_param_name(mtext, false, false) != 1) rim_report_error( "Improper string variable name [%s]", mtext);
                            check_var (&mtext, RIM_DEFSTRING);
                            oprintf ("{ rim_num _rim_s_id = rim_mem_get_len(%s); if ((%s)>=0 || (%s)<=_rim_s_id) {rim_mem_set_len (%s, (%s)+1);(%s)[%s] = 0;} else rim_report_error (\"String length [%%ld] is outside the string range [0-%%ld]\", %s, rim_mem_get_len(_rim_s_id));}\n", mtext, len, len, mtext, len, mtext, len, len);
                            continue;
                        }

                        // check for str[]
                        bool num_assign = false;
                        char *ind;
                        char *open_p;
                        mtext = check_input (mtext); // expand for top-level call
                        if ((open_p = strchr (mtext, '[')) != NULL)
                        {
                            num_assign = true;
                            // this is str[index]=number, so this expression needs
                            // to evaluate to number
                            bool sub_found;
                            check_exp (mtext, &sub_found, false, false);
                            *open_p = 0; // now mtext has lost [], so it can be checked below for type
                            ind = open_p + 1; // ind starts after leading [
                            // now remove last ] so that ind (index of string) is correct for code generation
                            rim_num il = strlen (ind);
                            while (il > 0 && ind[il-1] != ']') il--;
                            if (il > 0) ind[il-1] = 0;
                        }

                        //
                        // When process string is created, it couldn't have existed as string; add_var (from define_statement) won't allow
                        // When ordinary string is created, it couldn't have existed as process string for the same reason. 
                        // And since define_statement is used in *all* outputs from statement, no statement can write directly to process string
                        // We check if the string a in set-string a=b was previously defined as process-scoped, and we only define it again as 
                        // process scope (unnecessary really), because otherwise we would try to define it as ordinary string which would cause an error
                        // as per above
                        //
                        rim_num real_type;
                        bool is_ps = var_exists (mtext, RIM_DEFSTRINGSTATIC, &real_type, NULL) && real_type==RIM_DEFSTRINGSTATIC; // did it exist as process-scope var prior to this statement?
                        if (ps_cre || is_ps) is_def = define_statement (&mtext, RIM_DEFSTRINGSTATIC, false); 
                        else is_def = define_statement (&mtext, RIM_DEFSTRING, false); // correct as it uses RIM_EMPTY_STRING for empty init,
                                                                              // otherwise just assigns pointer, so acts as an alias

                        if (is_def == 1 && num_assign) rim_report_error ("Cannot use [] assignment in new variable");
                        if (eq == NULL && num_assign) rim_report_error ("Must use equal ('=') with [] assignment");
                        if (ps_cre && num_assign) rim_report_error ("Cannot use process-scope with [] assignment");

                        if (num_assign)
                        {
                            check_var (&eq, RIM_DEFNUMBER);
                            oprintf ("if (rim_mem_get_len(%s)-1 < (%s) || (%s) < 0) rim_report_error (\"Cannot access byte [%%ld] in string\", (long)(%s));\n", mtext, ind, ind, ind);
                            oprintf ("%s[%s] = (%s);\n", mtext, ind, eq);
                            continue;
                        }

                        
                        // Rules:
                        // process-scope can only go with equal and if variable is created here
                        // equal can only go with process scope and if-true
                        
                        if (ps_cre && is_def != 1) rim_report_error ("process-scope can only be used when variable is created");
                        static char empty[] = "RIM_EMPTY_STRING"; //[] so trim will work on it in check_var
                        if (eq == NULL) eq = empty;


                        //
                        // First, create variable out of constant if it is a constant
                        // Also, check assigned value is a string in general
                        // Do not quote RIM_EMPTY_STRING
                        //
                        if (unq && eq != empty) make_unq (&eq); 
                        else 
                        {
                            make_mem (&eq);
                        }
                        rim_num eq_p = check_var (&eq, RIM_DEFSTRING);
                        RIM_UNUSED(eq_p);
                        
                        //
                        // We check the type of to in to=src. If process, then src is given process status, except of course if it's 
                        // a constant, that's already process in a different way (id is -1). 
                        // Then if this is the actual definition of process string, the value is assigned to it only the first time and
                        // afterwards, nothing is assigned to process string in set-string a=b process-scope (both is_ps_cre and is_ps are true); 
                        // for process string to be assigned again it has to be set-string a=b (is_ps is true, ps_cre is false),
                        // every subsequent time (when ps_cre is false, but is_ps is true), the new value is assigned. 
                        //
                        if (ps_cre)
                        {
                            // with process scope, initialization happens only once, like with static, except we do assignment
                            // to be able to "initialize" with a variable and also conditionally with if-true
                            // if process string was created here, do not decrement process ref count; only do that if it's regular assignment below
                            // this is because process string (unlike tree, hash, list or array) has update and creation in the same statement
                            oprintf("static bool _rim_nstat%ld = true;\n", rim_pscope);
                            oprintf("if (_rim_nstat%ld) {\n_rim_nstat%ld = false;\n", rim_pscope, rim_pscope);
                            if (!eq_const) oprintf ("rim_mem_add_ref (%s,RIM_MEM_PROCESS);\n", eq); // do not needlessly call adding ref
                            oprintf ("%s = %s;\n", mtext, eq);
                            // with process scope, initialization happens only once, like with static, except we do assignment
                            // to be able to "initialize" with a variable and also conditionally with if-true
                            if (eq) oprintf("}\n"); 
                            rim_pscope++;
                        }
                        else
                        {
                            if (!eq_const) oprintf ("rim_mem_add_ref (%s,%s);\n", eq, is_ps?"RIM_MEM_PROCESS":"0"); // do not needlessly call adding ref
                            if (is_ps) oprintf ("rim_mem_dec_process (%s);\n", mtext);
                            oprintf ("%s = %s;\n", mtext, eq);
                        }

                        continue;
                    }
                    else if (((newI=recog_statement (line, i, "if-true", &mtext, &msize, 0, &rim_is_inline)) != 0)
                        || ((newI1=recog_statement (line, i, "else-if", &mtext, &msize, 1, &rim_is_inline)) != 0)
                        || ((newI2=recog_statement (line, i, "else-if", &mtext, &msize, 0, &rim_is_inline)) != 0))
                    {
                         RIM_GUARD
                        i = newI+newI1+newI2;
                        bool else_if = false;
                        bool else_alone = false;
                        bool is_if = false;
                        if (newI1+newI2 != 0 ) else_if = true; else is_if = true;
                        if (newI1 != 0) else_alone = true;
                        // NOTE:the following comment must not be deleted, it provides coloring information for vim plugin!!!!
                        // RIM_KEYEQUAL RIM_KEYNOTEQUAL RIM_KEYLESSERTHAN  RIM_KEYLESSEREQUAL RIM_KEYGREATERTHAN RIM_KEYEVERY RIM_KEYNOTEVERY RIM_KEYGREATEREQUAL RIM_KEYCONTAIN RIM_KEYNOTCONTAIN RIM_KEYCASEINSENSITIVE RIM_KEYERRORMARGIN RIM_KEYLENGTH

                        // if if-string, then open_ifs is a +1 from previous if-string (or 0 if the first), so open_ifs is correct
                        if (is_if) { if_nest[open_ifs].else_done = false; }


                        if (!else_alone)
                        {
                            if (is_if) 
                            {
                                if (open_ifs==0) last_line_string_closed = lnum;
                                open_ifs++; // this is the only place where open_ifs increases
                                if (open_ifs >= RIM_MAX_OTHER) rim_report_error( "too many sub-ifs nested with if-true");
                            }
                            else
                            {
                                carve_stmt_obj (&mtext, true, true); // for || && ( or ) there is no mtext (cm), only for not-equal or equal 
                                                             // we check for this kind of correctness down here
                                // if else-string, then open_ifs is +1 from when if-string was done, so open_ifs-1 is correct
                                if (open_ifs <= 0) rim_report_error( "else-if found without an open if-true");
                                // open_ifs gets decreased by 1 (closing prior to else) and then increases by one (in opening after else) so stays the same
                                if (if_nest[open_ifs-1].else_done) rim_report_error( "else-if without a condition must be the last one after if-true statement");
                                if_nest[open_ifs-1].num_elses++; // one more "} else {"
                            }
                            char *cm = mtext;
                            char before_ifs[200];
                            snprintf(before_ifs, sizeof(before_ifs), "%s", else_if == 1 ? "} else { ":"");
                            // this comment needed for getvim to pick up these keywords
                            // RIM_KEYEVERY RIM_KEYNOTEVERY RIM_KEYEQUAL RIM_KEYNOTEQUAL RIM_KEYOR RIM_KEYAND RIM_KEYLESSERTHAN RIM_KEYLESSEREQUAL RIM_KEYGREATERTHAN RIM_KEYGREATEREQUAL RIM_KEYCONTAIN RIM_KEYNOTCONTAIN RIM_KEYCASEINSENSITIVE KEYERRORMARGIN RIM_KEYLENGTH
                            parse_cond (cm, before_ifs, "if (", ") {\n");
                        }
                        else
                        {
                            carve_stmt_obj (&mtext, false, false); // do not expect anything for else-if without condition
                            // if else-string, then open_ifs is +1 from when if-string was done, so open_ifs-1 is correct
                            if (open_ifs <= 0) rim_report_error( "else-if found without an open if-true");
                            // open_ifs gets decreased by 1 (closing prior to else) and then increases by one (in opening after else) so stays the same
                            //
                            // At this point, open_ifs is 1 greater than the level, so on level 0 (original if-string), open_ifs is 1,
                            // on level 1 (one-nested if-string), open_ifs is 2 etc. That's because if-string will do +1, and for else-string
                            // it remains so. By the time code reaches here, it's +1
                            oprintf("} else {\n");
                            // here checking is done with open_ifs-1, because open_ifs is now +1 of what the level really is
                            if (if_nest[open_ifs-1].else_done) rim_report_error( "else-if without a condition can be used only once");
                            if_nest[open_ifs-1].else_done = true;
                        }

                        continue;
                    }
                    else if ((newI=recog_statement (line, i, "end-if", &mtext, &msize, 1, &rim_is_inline)) != 0)
                    {
                        RIM_GUARD
                        i = newI;
                        if (open_ifs <= 0) rim_report_error( "end-if found without an open if statement");
                        open_ifs--; // this is the only place where open_ifs decreases
                        // check done on real open_ifs, after --
                        if_nest[open_ifs].else_done = false;
                        oprintf("}\n");
                        // close "} else {"s 
                        rim_num k;
                        for (k = 0; k < if_nest[open_ifs].num_elses; k++) oprintf("}\n");
                        if_nest[open_ifs].num_elses = 0;
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "delete-string", &mtext, &msize, 0, &rim_is_inline)) != 0)
                    {
                        RIM_GUARD
                        i = newI;
                        carve_stmt_obj (&mtext, true, false);
                        //
                        check_var (&mtext, RIM_DEFSTRING);
                        //
                        // When process-scoped ref counter goes down to 0, it become ordinary memory with ref of 1
                        // Ordinary memory is only deleted immediately if ref is 0, otherwise at the end of request
                        //
                        check_var (&mtext, RIM_DEFSTRING);
                        oprintf("rim_free (%s);\n", mtext);
                        oprintf("%s=RIM_EMPTY_STRING;\n", mtext);
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "write-message", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;
                        char *key = find_keyword (mtext, RIM_KEYKEY, 1);
                        char *value = find_keyword (mtext, RIM_KEYVALUE, 1);
                        carve_statement (&key, "write-message", RIM_KEYKEY, 1, 1);
                        carve_statement (&value, "write-message", RIM_KEYVALUE, 1, 1);
                        carve_stmt_obj (&mtext, true, false);
                        make_mem(&key);
                        make_mem(&value);
                        //
                        check_var (&key, RIM_DEFSTRING);
                        check_var (&value, RIM_DEFSTRING);
                        check_var (&mtext, RIM_DEFMSG);

                        // key and value are copied to message, so no ref increment
                        oprintf ("rim_write_msg(%s, %s, %s);\n",mtext, key, value); 

                        continue;

                    }
                    else if ((newI=recog_statement(line, i, "read-message", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;
                        char *key = find_keyword (mtext, RIM_KEYKEY, 1);
                        char *value = find_keyword (mtext, RIM_KEYVALUE, 1);
                        char *st = find_keyword (mtext, RIM_KEYSTATUS, 1);

                        carve_statement (&key, "read-message", RIM_KEYKEY, 0, 1);
                        carve_statement (&value, "read-message", RIM_KEYVALUE, 0, 1);
                        carve_statement (&st, "read-message", RIM_KEYSTATUS, 0, 1);
                        carve_stmt_obj (&mtext, true, false);
                        define_statement (&key, RIM_DEFSTRING, false);
                        define_statement (&value, RIM_DEFSTRING, false);
                        define_statement (&st, RIM_DEFNUMBER, false);
                        //
                        check_var (&mtext, RIM_DEFMSG);

                        oprintf ("_rim_st=rim_read_msg(%s, %s%s%s, %s%s%s);\n", mtext, key!=NULL?"&(":"",key!=NULL?key:"NULL",key!=NULL?")":"",value!=NULL?"&(":"",value!=NULL?value:"NULL",value!=NULL?")":"");
                        if (st != NULL) oprintf("%s=_rim_st;\n",st); else oprintf("if (_rim_st != RIM_OKAY) rim_report_error (\"%s\", \"%s\", (rim_num)%ld, _rim_st);\n", RIM_STMT_FAIL_CHECK, rim_valid_rim_name, lnum);
                        continue;

                    }
                    else if ((newI=recog_statement(line, i, "get-message", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;

                        char *to = find_keyword (mtext, RIM_KEYTO, 1);
                        carve_statement (&to, "get-message", RIM_KEYTO, 1, 1);
                        carve_stmt_obj (&mtext, true, false);
                        define_statement (&to, RIM_DEFSTRING, false); // exact length set below with rim_get_msg
                        //
                        check_var (&mtext, RIM_DEFMSG);

                        oprintf ("{ char *_rim_gmsg =rim_get_msg(%s);\n",mtext); 
                        oprintf ("rim_mem_add_ref (_rim_gmsg, 0);\n");
                        oprintf ("%s = _rim_gmsg;\n", to);
                        oprintf("}\n");


                        continue;

                    }
                    else if ((newI=recog_statement(line, i, "new-message", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;

                        char *from = find_keyword (mtext, RIM_KEYFROM, 1);
                        carve_statement (&from, "new-message", RIM_KEYFROM, 0, 1);
                        carve_stmt_obj (&mtext, true, false);
                        make_mem(&from);
                        define_statement (&mtext, RIM_DEFMSG, true);
                        //
                        check_var (&from, RIM_DEFSTRING);

                        oprintf ("%s=rim_new_msg(%s);\n",mtext, from==NULL?"NULL":from); 

                        continue;

                    }
                    else if ((newI=recog_statement(line, i, "read-split", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;
                        char *from = find_keyword (mtext, RIM_KEYFROM, 1);
                        char *to = find_keyword (mtext, RIM_KEYTO, 1);
                        char *st = find_keyword (mtext, RIM_KEYSTATUS, 1);

                        carve_statement (&to, "read-split", RIM_KEYTO, 1, 1);
                        carve_statement (&from, "read-split", RIM_KEYFROM, 1, 1);
                        carve_statement (&st, "read-split", RIM_KEYSTATUS, 0, 1);
                        carve_stmt_obj (&mtext, true, false);

                        define_statement (&to, RIM_DEFSTRING, false); // exact length set below with strdup/RIM_EMPTY_STRING
                        define_statement (&st, RIM_DEFNUMBER, false);
                        //
                        check_var (&mtext, RIM_DEFNUMBER);
                        check_var (&from, RIM_DEFBROKEN);

                        // check split-string isn't NULL
                        oprintf ("if ((%s) != NULL && %s >= 1 && %s <= (%s)->num_pieces) {\n", from, mtext, mtext, from);
                        if (st != NULL) oprintf("%s=RIM_OKAY;\n", st);
                        // read-split is always non-process, hence using RIM_EMPTY_STRING
                        oprintf ("rim_mem_add_ref( (%s)->pieces[(%s)-1], 0); %s = (%s)->pieces[(%s)-1];\n", from,mtext, to, from, mtext);
                        oprintf("} else {\n");
                        if (st != NULL) oprintf("%s=RIM_ERR_OVERFLOW;\n", st);
                        else oprintf("rim_report_error (\"%s\", \"%s\", (rim_num)%ld, (rim_num)RIM_ERR_OVERFLOW);\n", RIM_STMT_FAIL_CHECK, rim_valid_rim_name, lnum);
                        oprintf ("%s=RIM_EMPTY_STRING;}\n",to); 

                        continue;

                    }
                    else if ((newI=recog_statement(line, i, "split-string", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;
                        char *to = find_keyword (mtext, RIM_KEYTO, 1);
                        char *with = find_keyword (mtext, RIM_KEYWITH, 1);
                        char *del = find_keyword (mtext, RIM_KEYDELETE, 1);
                        char *count = find_keyword (mtext, RIM_KEYCOUNT, 1);

                        if (del != NULL && (to != NULL || with != NULL)) rim_report_error ("Only one of 'to'/'with' or 'delete' can be used in split-string");
                        if (del == NULL && to == NULL) rim_report_error ("At least one of 'to'/'with' or 'delete' must be used in split-string");
                        if (count != NULL && to == NULL) rim_report_error ("'count' can only be used with 'to' in split-string");

                        if (to != NULL) 
                        {
                            carve_statement (&to, "split-string", RIM_KEYTO, 1, 1);
                            carve_statement (&with, "split-string", RIM_KEYWITH, 1, 1);
                            carve_statement (&count, "split-string", RIM_KEYCOUNT, 0, 1);
                            make_mem(&with);
                        }
                        if (del != NULL) 
                        {
                            carve_statement (&del, "split-string", RIM_KEYDELETE, 0, 1);
                            carve_stmt_obj (&mtext, false, false);
                        }
                        else carve_stmt_obj (&mtext, true, true);

                        if (to != NULL) define_statement (&to, RIM_DEFBROKEN, true);
                        if (count != NULL) define_statement (&count, RIM_DEFNUMBER, false);
                        //
                        if (del == NULL) check_var (&mtext, RIM_DEFSTRING);
                        check_var (&with, RIM_DEFSTRING);
                        check_var (&del, RIM_DEFBROKEN);

                        if (to != NULL) oprintf("rim_break_down (%s, %s, &(%s));\n", mtext, with, to);
                        if (count != NULL) oprintf ("%s= (%s)->num_pieces;\n", count, to); // must be after rim_break_down to fill in the result

                        if (del != NULL) oprintf ("rim_delete_break_down (&(%s));\n", del);

                        continue;

                    }
                    else if ((newI=recog_statement(line, i, "unused-var", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        i = newI;


                        carve_stmt_obj (&mtext, true, false);
                        while (1) 
                        {
                            char *comma = find_keyword (mtext, RIM_KEYCOMMA, 0);
                            carve_statement (&comma, "unused-var", RIM_KEYCOMMA, 0, 1); // separate entries with comma first, so
                                                                                      // this find/carve MUST come before the rest below!
                                                                                      //

                            check_var (&mtext, RIM_DEFUNKN); // just check variable is known, we don't care about type
                            oprintf("RIM_UNUSED (%s);\n", mtext);
                            if (comma == NULL) break; else mtext = comma;
                        }

                        continue;

                    }
                    else if ((newI=recog_statement(line, i, "new-list", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;

                        char *process = find_keyword (mtext, RIM_KEYPROCESSSCOPE, 1);

                        carve_statement (&process, "new-list", RIM_KEYPROCESSSCOPE, 0, 0);

                        carve_stmt_obj (&mtext, true, false);
                        if (process) define_statement (&mtext, RIM_DEFLISTSTATIC, true); else define_statement (&mtext, RIM_DEFLIST, true);
                        oprintf("rim_mem_process=%s;\n", process?"RIM_MEM_PROCESS":"0");
                        oprintf ("rim_list_init (&(%s), %s);\n", mtext, process?"RIM_MEM_PROCESS":"0");
                        oprintf("rim_mem_process=0;\n");

                        continue;

                    }
                    else if ((newI=recog_statement(line, i, "write-list", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;

                        char *key = find_keyword (mtext, RIM_KEYKEY, 1);
                        char *value = find_keyword (mtext, RIM_KEYVALUE, 1);
                        char *append = find_keyword (mtext, RIM_KEYAPPEND, 1);


                        carve_statement (&key, "write-list", RIM_KEYKEY, 1, 1);
                        carve_statement (&value, "write-list", RIM_KEYVALUE, 1, 1);
                        carve_statement (&append, "write-list", RIM_KEYAPPEND, 0, 2);
                        carve_stmt_obj (&mtext, true, false);
                        make_mem (&key);
                        make_mem (&value);

                        char *appendc = opt_clause(append, "true", "false", RIM_DEFBOOL);
                        //
                        check_var (&mtext, RIM_DEFLIST);
                        check_var (&key, RIM_DEFSTRING);
                        check_var (&value, RIM_DEFSTRING);
                        check_var (&appendc, RIM_DEFBOOL);

                        oprintf("rim_mem_process=(%s)->process;\n", mtext);
                        oprintf ("rim_list_store (%s, %s, (void*)%s, %s);\n", mtext, key, value, appendc);
                        oprintf("rim_mem_process=0;\n");
                        continue;

                    }
                    else if ((newI=recog_statement(line, i, "read-list", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;

                        char *key = find_keyword (mtext, RIM_KEYKEY, 1);
                        char *value = find_keyword (mtext, RIM_KEYVALUE, 1);
                        char *upd = find_keyword (mtext, RIM_KEYUPDATEVALUE, 1);
                        char *updkey = find_keyword (mtext, RIM_KEYUPDATEKEY, 1);
                        char *st = find_keyword (mtext, RIM_KEYSTATUS, 1);

                        carve_statement (&key, "read-list", RIM_KEYKEY, 1, 1);
                        carve_statement (&value, "read-list", RIM_KEYVALUE, 1, 1);
                        carve_statement (&upd, "read-list", RIM_KEYUPDATEVALUE, 0, 1);
                        carve_statement (&updkey, "read-list", RIM_KEYUPDATEKEY, 0, 1);
                        carve_statement (&st, "read-list", RIM_KEYSTATUS, 0, 1);

                        carve_stmt_obj (&mtext, true, false);
                        make_mem(&upd);
                        make_mem(&updkey);

                        define_statement (&key, RIM_DEFSTRING, false); // exact length okay this is just pointer aliasing
                        define_statement (&value, RIM_DEFSTRING, false); // exact length okay this is just pointer aliasing

                        define_statement (&st, RIM_DEFNUMBER, false);
                        //
                        check_var (&mtext, RIM_DEFLIST);

                        oprintf ("{ char *_rim_list_k; char *_rim_list_v; rim_list_retrieve (%s, &(_rim_list_k), (void**)&(_rim_list_v));\n", mtext);
                        if (upd || updkey || st) oprintf("if ((%s)->curr != NULL) {\n", mtext);
                        //
                        oprintf ("%s = _rim_list_k;\n",  key);
                        oprintf ("%s = _rim_list_v;\n", value);
                    
                        //
                        // Update key/value must be *AFTER* obtaining key and value from the list above, so that this works:
                        // read-list l key k value v update-key "1"+k update-value "2+v
                        //
                        check_var (&upd, RIM_DEFSTRING);
                        check_var (&updkey, RIM_DEFSTRING);

                        // name,data surely existed, so 0 in add_ref
                        if (updkey) 
                        {
                            oprintf ("rim_mem_add_ref (%s, (%s)->process);\n", updkey, mtext);
                            oprintf ("rim_mem_dec_process ((%s)->curr->name);\n", mtext);
                            oprintf("(%s)->curr->name = (%s);\n", mtext, updkey); 
                        }
                        //
                        if (upd) 
                        {
                            oprintf("rim_mem_add_ref((%s), (%s)->process) ;\n", upd, mtext); 
                            oprintf ("rim_mem_dec_process ((%s)->curr->data);\n", mtext);
                            oprintf("(%s)->curr->data = (%s) ;\n", mtext, upd); 
                        }
                        //
                        if (st) oprintf("     (%s) = RIM_OKAY;\n", st); 
                        if (upd || updkey || st) oprintf("}\n");

                        if (upd || updkey || st)
                        {
                            if (st) oprintf ("else { (%s) = RIM_ERR_EXIST; }\n", st); else oprintf("else { rim_report_error (\"%s\", \"%s\", (rim_num)%ld, (rim_num)RIM_ERR_EXIST);}\n", RIM_STMT_FAIL_CHECK, rim_valid_rim_name, lnum);
                        }
                        oprintf("}\n");

                        continue;

                    }
                    else if ((newI=recog_statement(line, i, "position-list", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;
                        char *first = find_keyword (mtext, RIM_KEYFIRST, 1);
                        char *last = find_keyword (mtext, RIM_KEYLAST, 1);
                        char *end = find_keyword (mtext, RIM_KEYEND, 1);
                        char *prev = find_keyword (mtext, RIM_KEYPREVIOUS, 1);
                        char *next = find_keyword (mtext, RIM_KEYNEXT, 1);
                        char *st = find_keyword (mtext, RIM_KEYSTATUS, 1);

                        carve_statement (&first, "position-list", RIM_KEYFIRST, 0, 0);
                        carve_statement (&last, "position-list", RIM_KEYLAST, 0, 0);
                        carve_statement (&end, "position-list", RIM_KEYEND, 0, 0);
                        carve_statement (&prev, "position-list", RIM_KEYPREVIOUS, 0, 0);
                        carve_statement (&next, "position-list", RIM_KEYNEXT, 0, 0);
                        carve_statement (&st, "position-list", RIM_KEYSTATUS, 0, 1);
                        carve_stmt_obj (&mtext, true, false);

                        define_statement (&st, RIM_DEFNUMBER, false);
                        //
                        check_var (&mtext, RIM_DEFLIST);

                        int where;
                        if (first != NULL) where = RIM_LIST_FIRST;
                        else if (last != NULL) where = RIM_LIST_LAST;
                        else if (end != NULL) where = RIM_LIST_END;
                        else if (prev != NULL) where = RIM_LIST_PREV;
                        else if (next != NULL) where = RIM_LIST_NEXT;
                        else rim_report_error ("Must specify either first, last, end, previous or next clause");
                        int opt = (first!=NULL?1:0)+(last!=NULL?1:0)+(end!=NULL?1:0)+(prev!=NULL?1:0)+(next!=NULL?1:0);
                        if (opt > 1) rim_report_error ("Must specify only one of first, last, end, previous or next clauses");

                        static rim_num pos_st = 0;
                        //
                        // Position linked list to an element. If 'first' is true, then position to first one,
                        // otherwise to last one. If 'end' is true, then the end (which is NULL, beyond last one). 
                        // Inserting is always done by pushing current forward and inserting into its place. Deleting is done
                        // by deleting the current. Reading reads current. If 'item' is not NULL, position to this (it must have been
                        // saved previously from somewhere in the list). If 'prev' is true, then position to previous element, otherwise next.
                        // Returns RIM_OKAY if okay, RIM_ERR_EMPTY if list is empty
                        //
                        oprintf("rim_mem_process=(%s)->process;\n", mtext);
                        oprintf ("rim_num _rim_list_pos_st%ld;\n", pos_st);
                        oprintf ("if ((%s)->num_of == 0) _rim_list_pos_st%ld = RIM_ERR_EXIST; else {\n", mtext, pos_st);
                        if (where == RIM_LIST_FIRST) oprintf ("(%s)->curr = (%s)->first; _rim_list_pos_st%ld = RIM_OKAY;\n", mtext, mtext, pos_st);
                        else if (where == RIM_LIST_LAST) oprintf ("(%s)->curr = (%s)->last;_rim_list_pos_st%ld = RIM_OKAY;\n", mtext, mtext, pos_st);
                        else if (where == RIM_LIST_END) oprintf ("(%s)->curr = NULL; _rim_list_pos_st%ld = RIM_OKAY;\n", mtext, pos_st);
                        else if (where == RIM_LIST_PREV)
                        {
                            oprintf ("if ((%s)->curr == (%s)->first) _rim_list_pos_st%ld = RIM_ERR_EXIST; else { (%s)->curr = ((%s)->curr ? (%s)->curr->prev:(%s)->last); _rim_list_pos_st%ld = RIM_OKAY; }\n", mtext, mtext, pos_st, mtext,mtext,mtext,mtext, pos_st);
                        }
                        else if (where == RIM_LIST_NEXT)
                        {
                            oprintf ("if ((%s)->curr == NULL) _rim_list_pos_st%ld = RIM_ERR_EXIST; else { (%s)->curr = (%s)->curr->next; _rim_list_pos_st%ld = RIM_OKAY; }\n", mtext, pos_st, mtext, mtext, pos_st);
                        }
                        oprintf ("}\n");
                        if (st) oprintf ("(%s) = _rim_list_pos_st%ld;\n", st, pos_st);
                        else oprintf("if (_rim_list_pos_st%ld != RIM_OKAY) rim_report_error (\"%s\", \"%s\", (rim_num)%ld, _rim_list_pos_st%ld);\n", pos_st, RIM_STMT_FAIL_CHECK, rim_valid_rim_name, lnum, pos_st);
                        
                        pos_st++;
                        oprintf("rim_mem_process=0;\n");


                        continue;

                    }
                    else if ((newI=recog_statement(line, i, "purge-list", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;

                        carve_stmt_obj (&mtext, true, false);
                        //
                        check_var (&mtext, RIM_DEFLIST);
                        oprintf("rim_mem_process=(%s)->process;\n", mtext);
                        oprintf ("rim_list_purge (&(%s));\n", mtext);
                        oprintf("rim_mem_process=0;\n");

                        continue;

                    }
                    else if ((newI=recog_statement(line, i, "delete-list", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;
                        char *st = find_keyword (mtext, RIM_KEYSTATUS, 1);
                        carve_statement (&st, "delete-list", RIM_KEYSTATUS, 0, 1);
                        carve_stmt_obj (&mtext, true, false);
                        define_statement (&st, RIM_DEFNUMBER, false);
                        //
                        check_var (&mtext, RIM_DEFLIST);
                        oprintf("rim_mem_process=(%s)->process;\n", mtext);
                        oprintf ("_rim_st=rim_list_delete (%s);\n", mtext);
                        if (st != NULL) oprintf("%s=_rim_st;\n", st); else oprintf("if (_rim_st != RIM_OKAY) rim_report_error (\"%s\", \"%s\", (rim_num)%ld, _rim_st);\n", RIM_STMT_FAIL_CHECK, rim_valid_rim_name, lnum);
                        oprintf("rim_mem_process=0;\n");

                        continue;

                    }
                    else if ((newI=recog_statement(line, i, "new-fifo", &mtext, &msize, 0, &rim_is_inline)) != 0  
                     || (newI1=recog_statement(line, i, "new-lifo", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI+newI1;
                        bool lifo = (newI1 != 0);

                        carve_stmt_obj (&mtext, true, false);
                        define_statement (&mtext, lifo?RIM_DEFLIFO:RIM_DEFFIFO, true);
                        oprintf ("rim_store_init (&(%s));\n", mtext);

                        continue;

                    }
                    else if ((newI=recog_statement(line, i, "write-fifo", &mtext, &msize, 0, &rim_is_inline)) != 0
                     || (newI1=recog_statement(line, i, "write-lifo", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI+newI1;
                        bool lifo = (newI1 != 0);

                        char *key = find_keyword (mtext, RIM_KEYKEY, 1);
                        char *value = find_keyword (mtext, RIM_KEYVALUE, 1);

                        carve_statement (&key, "write-fifo", RIM_KEYKEY, 1, 1);
                        carve_statement (&value, "write-fifo", RIM_KEYVALUE, 1, 1);
                        carve_stmt_obj (&mtext, true, false);
                        make_mem(&key);
                        make_mem(&value);
                        //
                        check_var (&mtext, lifo?RIM_DEFLIFO:RIM_DEFFIFO);
                        check_var (&key, RIM_DEFSTRING);
                        check_var (&value, RIM_DEFSTRING);


                        oprintf ("rim_store%s (%s, %s, (void*)%s);\n", lifo?"_l":"", mtext, key, value);

                        continue;

                    }
                    else if ((newI=recog_statement(line, i, "rewind-fifo", &mtext, &msize, 0, &rim_is_inline)) != 0
                     || (newI1=recog_statement(line, i, "rewind-lifo", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI+newI1;
                        bool lifo = (newI1 != 0);

                        carve_stmt_obj (&mtext, true, false);
                        //
                        check_var (&mtext, lifo?RIM_DEFLIFO:RIM_DEFFIFO);
                        oprintf ("rim_rewind (%s);\n", mtext);

                        continue;

                    }
                    else if ((newI=recog_statement(line, i, "delete-fifo", &mtext, &msize, 0, &rim_is_inline)) != 0
                     || (newI1=recog_statement(line, i, "delete-lifo", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI+newI1;
                        bool lifo = (newI1 != 0);

                        carve_stmt_obj (&mtext, true, false);
                        //
                        check_var (&mtext, lifo?RIM_DEFLIFO:RIM_DEFFIFO);
                        oprintf ("rim_fifo_delete (%s);\n", mtext);

                        continue;

                    }
                    else if ((newI=recog_statement(line, i, "purge-fifo", &mtext, &msize, 0, &rim_is_inline)) != 0
                     || (newI1=recog_statement(line, i, "purge-lifo", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI+newI1;
                        bool lifo = (newI1 != 0);

                        carve_stmt_obj (&mtext, true, false);
                        //
                        check_var (&mtext, lifo?RIM_DEFLIFO:RIM_DEFFIFO);
                        oprintf ("rim_purge (&(%s));\n", mtext);

                        continue;

                    }
                    else if ((newI=recog_statement(line, i, "read-fifo", &mtext, &msize, 0, &rim_is_inline)) != 0
                     || (newI1=recog_statement(line, i, "read-lifo", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI+newI1;
                        bool lifo = (newI1 != 0);

                        char *key = find_keyword (mtext, RIM_KEYKEY, 1);
                        char *value = find_keyword (mtext, RIM_KEYVALUE, 1);
                        char *st = find_keyword (mtext, RIM_KEYSTATUS, 1);

                        carve_statement (&key, "read-fifo", RIM_KEYKEY, 1, 1);
                        carve_statement (&value, "read-fifo", RIM_KEYVALUE, 1, 1);
                        carve_statement (&st, "read-fifo", RIM_KEYSTATUS, 0, 1);
                        carve_stmt_obj (&mtext, true, false);

                        define_statement (&key, RIM_DEFSTRING, false); // exact length okay this is just pointer aliasing in rim_retrieve
                        define_statement (&value, RIM_DEFSTRING, false); // exact length okay this is just pointer aliasing in rim_retrieve
                        define_statement (&st, RIM_DEFNUMBER, false);
                        //
                        check_var (&mtext, lifo?RIM_DEFLIFO:RIM_DEFFIFO);

                        oprintf ("_rim_st=rim_retrieve (%s, &(%s), (void**)&(%s));\n", mtext, key, value);
                        if (st != NULL) oprintf("%s=_rim_st;\n",st); else oprintf("if (_rim_st != RIM_OKAY) rim_report_error (\"%s\", \"%s\", (rim_num)%ld, _rim_st);\n", RIM_STMT_FAIL_CHECK, rim_valid_rim_name, lnum);
                        continue;

                    }
                    else if ((newI=recog_statement(line, i, "write-file", &mtext, &msize, 0, &rim_is_inline)) != 0)
                    {
                        RIM_GUARD
                        i = newI;
                        char *from = find_keyword (mtext, RIM_KEYFROM, 1);
                        char *append = find_keyword (mtext, RIM_KEYAPPEND, 1);
                        char *length = find_keyword (mtext, RIM_KEYLENGTH, 1);
                        char *status = find_keyword (mtext, RIM_KEYSTATUS, 1);
                        char *pos = find_keyword (mtext, RIM_KEYPOSITION, 1);
                        char *fileid = find_keyword (mtext, RIM_KEYFILEID, 1);

                        carve_statement (&from, "write-file", RIM_KEYFROM, 1, 1);
                        carve_statement (&append, "write-file", RIM_KEYAPPEND, 0, 2);
                        carve_statement (&length, "write-file", RIM_KEYLENGTH, 0, 1);
                        carve_statement (&status, "write-file", RIM_KEYSTATUS, 0, 1);
                        carve_statement (&pos, "write-file", RIM_KEYPOSITION, 0, 1);
                        carve_statement (&fileid, "write-file", RIM_KEYFILEID, 0, 1);
                        if (fileid == NULL) carve_stmt_obj (&mtext, true, true); else carve_stmt_obj (&mtext, false, false); 
                        make_mem(&from);

                        define_statement (&status, RIM_DEFNUMBER, false);
                        char *appendc = opt_clause(append, "1", "0", RIM_DEFNUMBER);
                        //
                        if (fileid == NULL) check_var (&mtext, RIM_DEFSTRING);
                        check_var (&from, RIM_DEFSTRING);
                        check_var (&appendc, RIM_DEFNUMBER);
                        check_var (&length, RIM_DEFNUMBER);
                        check_var (&pos, RIM_DEFNUMBER);
                        if (fileid != NULL) check_var (&fileid, RIM_DEFFILE);


                        if (mtext[0] != 0 && fileid!=NULL) rim_report_error( "you can specify either file name or file-id but not both in write-file statement");

                        if (append != NULL && pos!=NULL) rim_report_error( "'append' and 'position' cannot both be in write-file statement");

                        if (length==NULL) length="0";

                        // Check if file opened, pretty fatal
                        if (fileid != NULL) oprintf ("if ((%s) == NULL) rim_report_error (\"Cannot write to file that is not open\");\n", fileid);

                        if (fileid != NULL) oprintf("_rim_st=rim_write_file_id (*((%s)->f), %s, %s, %s, %s, %s);\n", fileid, from, length, appendc, pos == NULL ? "0":pos, pos==NULL?"0":"1");
                        else oprintf("_rim_st=rim_write_file (%s, %s, %s, %s, %s, %s);\n",mtext, from, length, appendc, pos == NULL ? "0":pos, pos==NULL?"0":"1");
                        // _rim_st here is the number of bytes written, so 0 or more is okay, less than 0 is error
                        if (status != NULL) oprintf("%s=_rim_st;\n",status); else oprintf("if (_rim_st < 0) rim_report_error (\"%s\", \"%s\", (rim_num)%ld, _rim_st);\n", RIM_STMT_FAIL_CHECK, rim_valid_rim_name, lnum);
                        rim_free(appendc);
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "read-file", &mtext, &msize, 0, &rim_is_inline)) != 0)
                    {
                        RIM_GUARD
                        i = newI;
                        char *read_from = mtext;
                        char *read_to = find_keyword (mtext, RIM_KEYTO, 1);
                        char *status = find_keyword (mtext, RIM_KEYSTATUS, 1);
                        char *eof = find_keyword (mtext, RIM_KEYENDOFFILE, 1);
                        char *pos = find_keyword (mtext, RIM_KEYPOSITION, 1);
                        char *length = find_keyword (mtext, RIM_KEYLENGTH, 1);
                        char *fileid = find_keyword (mtext, RIM_KEYFILEID, 1);

                        carve_statement (&read_to, "read-file", RIM_KEYTO, 1, 1);
                        carve_statement (&status, "read-file", RIM_KEYSTATUS, 0, 1);
                        carve_statement (&eof, "read-file", RIM_KEYENDOFFILE, 0, 1);
                        carve_statement (&pos, "read-file", RIM_KEYPOSITION, 0, 1);
                        carve_statement (&length, "read-file", RIM_KEYLENGTH, 0, 1);
                        carve_statement (&fileid, "read-file", RIM_KEYFILEID, 0, 1);
                        if (fileid == NULL) carve_stmt_obj (&read_from, true, true); else carve_stmt_obj (&read_from, false, false);

                        define_statement (&eof, RIM_DEFBOOL, false);
                        define_statement (&status, RIM_DEFNUMBER, false);
                        define_statement (&read_to, RIM_DEFSTRING, false); // exact length set in rim_read_file
                        //
                        if (fileid == NULL) check_var (&read_from, RIM_DEFSTRING);
                        check_var (&pos, RIM_DEFNUMBER);
                        check_var (&length, RIM_DEFNUMBER);
                        if (fileid != NULL) check_var (&fileid, RIM_DEFFILE);

                        if (length == NULL) length="0";

                        // Fatail if not open
                        if (fileid != NULL) oprintf ("if ((%s) == NULL) rim_report_error (\"Cannot read from file that is not open\");\n", fileid);

                        if (fileid != NULL) oprintf("_rim_st=rim_read_file_id (*((%s)->f), &(%s), %s, %s, %s, &_rim_st_bool);\n", fileid, read_to, pos==NULL?"0":pos, length, pos!=NULL?"true":"false");
                        else oprintf("_rim_st=rim_read_file (%s, &(%s), %s, %s, &_rim_st_bool);\n", read_from, read_to, pos==NULL?"0":pos, length);
                        if (eof != NULL) oprintf ("%s=_rim_st_bool;\n", eof);
                        // _rim_st is negative if error, otherwise size of data read which can be zero
                        if (status != NULL) oprintf("%s=_rim_st;\n",status); else oprintf("if (_rim_st < 0) rim_report_error (\"%s\", \"%s\", (rim_num)%ld, _rim_st);\n", RIM_STMT_FAIL_CHECK, rim_valid_rim_name, lnum);
                        continue;

                    }
                    else if ((newI=recog_statement(line, i, "get-hash", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;

                        char *len = find_keyword (mtext, RIM_KEYLENGTH, 1);
                        char *size = find_keyword (mtext, RIM_KEYHASHSIZE, 1);
                        char *reads = find_keyword (mtext, RIM_KEYAVERAGEREADS, 1);


                        carve_statement (&size, "get-hash", RIM_KEYHASHSIZE, 0, 1);
                        carve_statement (&len, "get-hash", RIM_KEYLENGTH, 0, 1);
                        carve_statement (&reads, "get-hash", RIM_KEYAVERAGEREADS, 0, 1);
                        carve_stmt_obj (&mtext, true, false);
                        define_statement (&len, RIM_DEFNUMBER, false);
                        define_statement (&size, RIM_DEFNUMBER, false);
                        define_statement (&reads, RIM_DEFNUMBER, false);
                        //
                        check_var (&mtext, RIM_DEFHASH);

                        if (size == NULL && len == NULL && reads == NULL) rim_report_error( "one of 'length', 'size' or 'average-reads' must be in get-hash statement");

                        oprintf("rim_mem_process=(%s)->process;\n", mtext);
                        if (len !=NULL) oprintf("%s=rim_total_hash (%s);\n", len, mtext);
                        if (size !=NULL) oprintf("%s=rim_hash_size (%s);\n", size, mtext);
                        if (reads !=NULL) oprintf("%s=rim_hash_reads (%s);\n", reads, mtext);
                        oprintf("rim_mem_process=0;\n");
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "purge-hash", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;

                        carve_stmt_obj (&mtext, true, false);
                        //
                        check_var (&mtext, RIM_DEFHASH);
                        oprintf("rim_mem_process=(%s)->process;\n", mtext);
                        oprintf("rim_delete_hash (&(%s), true);\n", mtext);
                        oprintf("rim_mem_process=0;\n");
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "resize-hash", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;
                        char *size = find_keyword (mtext, RIM_KEYHASHSIZE, 1);

                        carve_statement (&size, "resize-hash", RIM_KEYHASHSIZE, 1, 1);
                        carve_stmt_obj (&mtext, true, false);
                        //
                        check_var (&mtext, RIM_DEFHASH);
                        check_var (&size, RIM_DEFNUMBER);

                        oprintf("rim_mem_process=(%s)->process;\n", mtext);
                        oprintf("rim_resize_hash (&(%s), %s);\n", mtext, size);
                        oprintf("rim_mem_process=0;\n");
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "new-array", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;
                        rim_mod_array = true;
                        char *maxsize = find_keyword (mtext, RIM_KEYMAXSIZE, 1);
                        char *process = find_keyword (mtext, RIM_KEYPROCESSSCOPE, 1);
                        char *type = find_keyword (mtext, RIM_KEYTYPE, 1);

                        carve_statement (&maxsize, "new-array", RIM_KEYMAXSIZE, 0, 1);
                        carve_statement (&process, "new-array", RIM_KEYPROCESSSCOPE, 0, 0);
                        carve_statement (&type, "new-array", RIM_KEYTYPE, 0, 1);
                        if (type != NULL) trimit (type); // type has to match
                        else type ="string";
                        rim_num at = typeid(type);
                        if (!cmp_type (at, RIM_DEFSTRING) && !cmp_type(at, RIM_DEFNUMBER) && !cmp_type(at, RIM_DEFBOOL) && !cmp_type(at, RIM_DEFDOUBLE)) rim_report_error("Unknown type [%s] in new-array", type);
                        // RIM_KEY_T_STRING RIM_KEY_T_BOOL RIM_KEY_T_NUMBER RIM_KEY_T_DOUBLE  // see get-param if ever adding other types as arrays
                        carve_stmt_obj (&mtext, true, false);
                        // check 'type' clause
                        if (cmp_type(at, RIM_DEFSTRING)) 
                        {
                            if (process) define_statement (&mtext, RIM_DEFARRAYSTRINGSTATIC, true); else define_statement (&mtext, RIM_DEFARRAYSTRING, true);
                        }
                        else if (cmp_type(at, RIM_DEFDOUBLE)) 
                        {
                            if (process) define_statement (&mtext, RIM_DEFARRAYDOUBLESTATIC, true); else define_statement (&mtext, RIM_DEFARRAYDOUBLE, true);
                        }
                        else if (cmp_type(at, RIM_DEFNUMBER)) 
                        {
                            if (process) define_statement (&mtext, RIM_DEFARRAYNUMBERSTATIC, true); else define_statement (&mtext, RIM_DEFARRAYNUMBER, true);
                        }
                        if (cmp_type(at, RIM_DEFBOOL)) 
                        {
                            if (process) define_statement (&mtext, RIM_DEFARRAYBOOLSTATIC, true); else define_statement (&mtext, RIM_DEFARRAYBOOL, true);
                        }
                        //
                        check_var (&maxsize, RIM_DEFNUMBER);

                        oprintf("rim_mem_process=%s;\n", process?"RIM_MEM_PROCESS":"0");
                        if (cmp_type(at, RIM_DEFSTRING)) oprintf("%s = rim_new_arraystring (%s, %s);\n", mtext, maxsize?maxsize:"0", process?"RIM_MEM_PROCESS":"0");
                        else if (cmp_type(at, RIM_DEFNUMBER)) oprintf("%s = rim_new_arraynumber (%s, %s);\n", mtext, maxsize?maxsize:"0", process?"RIM_MEM_PROCESS":"0");
                        else if (cmp_type(at, RIM_DEFDOUBLE)) oprintf("%s = rim_new_arraydouble (%s, %s);\n", mtext, maxsize?maxsize:"0", process?"RIM_MEM_PROCESS":"0");
                        else if (cmp_type(at, RIM_DEFBOOL)) oprintf("%s = rim_new_arraybool (%s, %s);\n", mtext, maxsize?maxsize:"0", process?"RIM_MEM_PROCESS":"0");
                        oprintf("rim_mem_process=0;\n");
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "new-hash", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;
                        char *size = find_keyword (mtext, RIM_KEYHASHSIZE, 1);
                        char *process = find_keyword (mtext, RIM_KEYPROCESSSCOPE, 1);

                        carve_statement (&size, "new-hash", RIM_KEYHASHSIZE, 0, 1);
                        carve_statement (&process, "new-hash", RIM_KEYPROCESSSCOPE, 0, 0);
                        carve_stmt_obj (&mtext, true, false);
                        if (process) define_statement (&mtext, RIM_DEFHASHSTATIC, true); else define_statement (&mtext, RIM_DEFHASH, true);
                        //
                        check_var (&size, RIM_DEFNUMBER);

                        oprintf("rim_mem_process=%s;\n", process?"RIM_MEM_PROCESS":"0");
                        oprintf("rim_create_hash (&(%s), %s, NULL, %s);\n", mtext, size?size:"10", process?"RIM_MEM_PROCESS":"0");
                        oprintf("rim_mem_process=0;\n");
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "write-array", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        rim_mod_array = true;
                        i = newI;
                        char *key = find_keyword (mtext, RIM_KEYKEY, 1);
                        char *val = find_keyword (mtext, RIM_KEYVALUE, 1);
                        char *oldd = find_keyword (mtext, RIM_KEYOLDVALUE, 1);

                        carve_statement (&key, "write-array", RIM_KEYKEY, 1, 1);
                        carve_statement (&val, "write-array", RIM_KEYVALUE, 1, 1);
                        carve_statement (&oldd, "write-array", RIM_KEYOLDVALUE, 0, 1);
                        //
                        carve_stmt_obj (&mtext, true, false);
                        rim_num at = check_var (&mtext, RIM_DEFUNKN);
                        if (!cmp_type (at, RIM_DEFARRAYSTRING) && !cmp_type(at, RIM_DEFARRAYNUMBER) && !cmp_type(at, RIM_DEFARRAYBOOL) && !cmp_type(at, RIM_DEFARRAYDOUBLE)) rim_report_error("Unknown type [%s] in write-array", typename(at));
                        //
                        if (cmp_type (at, RIM_DEFARRAYSTRING)) define_statement (&oldd, RIM_DEFSTRING, false); // exact length okay this is just pointer aliasing in rim_add_hash with RIM_EMPTY_STRING
                        else if (cmp_type (at, RIM_DEFARRAYNUMBER)) define_statement (&oldd, RIM_DEFNUMBER, false);
                        else if (cmp_type (at, RIM_DEFARRAYDOUBLE)) define_statement (&oldd, RIM_DEFDOUBLE, false);
                        else if (cmp_type (at, RIM_DEFARRAYBOOL)) define_statement (&oldd, RIM_DEFBOOL, false);
                        //
                        //
                        check_var (&key, RIM_DEFNUMBER);

                        oprintf("{ rim_mem_process=(%s)->process;\n", mtext);

                        //
                        // In all of deferred assignments (where old value can be used to assign to new value), rim_write_array... will report an error
                        // if key isn't correct. So setting afterwards is okay, because it means the key is correct memory-wise.
                        //
                        if (cmp_type (at, RIM_DEFARRAYSTRING)) 
                        {
                            oprintf("char *_rim_str = rim_write_arraystring (%s, %s, %s%s%s);\n", mtext, key, oldd==NULL?"":"&(", oldd==NULL?"NULL":oldd, oldd==NULL?"":")");
                            make_mem (&val);
                            check_var (&val, RIM_DEFSTRING);// here old_val can be used in val expression
                            oprintf ("rim_mem_add_ref (%s, rim_mem_process);\n", val);

                            oprintf ("(%s)->str[%s] = (%s);\n", mtext, key, val);
                            oprintf ("rim_mem_dec_process(_rim_str);\n");
                        }
                        else if (cmp_type (at, RIM_DEFARRAYDOUBLE)) 
                        {
                            oprintf("rim_write_arraydouble (%s, %s, %s%s%s);\n", mtext, key, oldd==NULL?"":"&(", oldd==NULL?"NULL":oldd, oldd==NULL?"":")");
                            check_var (&val, RIM_DEFDOUBLE); // here old_val can be used in val expression
                            oprintf ("(%s)->dbl[%s] = (%s);\n", mtext, key, val);
                        }
                        else if (cmp_type (at, RIM_DEFARRAYNUMBER)) 
                        {
                            oprintf("rim_write_arraynumber (%s, %s, %s%s%s);\n", mtext, key, oldd==NULL?"":"&(", oldd==NULL?"NULL":oldd, oldd==NULL?"":")");
                            check_var (&val, RIM_DEFNUMBER); // here old_val can be used in val expression
                            oprintf ("(%s)->num[%s] = (%s);\n", mtext, key, val);
                        }
                        else if (cmp_type (at, RIM_DEFARRAYBOOL)) 
                        {
                            oprintf("rim_write_arraybool (%s, %s, %s%s%s);\n", mtext, key, oldd==NULL?"":"&(", oldd==NULL?"NULL":oldd, oldd==NULL?"":")");
                            check_var (&val, RIM_DEFBOOL);
                            oprintf ("(%s)->logic[%s] = %s;\n", mtext, key, val);
                        }
                        oprintf("rim_mem_process=0; }\n");
                        // both key and value are now referenced by the index as well as the original variables that put them here
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "sort-array", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;
                        char *sort = find_keyword (mtext, RIM_KEYALGORITHM, 1);
                        char *desc = find_keyword (mtext, RIM_KEYDESCENDING, 1);
                        carve_statement (&sort, "sort-array", RIM_KEYALGORITHM, 0, 1);
                        carve_statement (&desc, "sort-array", RIM_KEYDESCENDING, 0, 0);

                        rim_mod_array = true;
                        carve_stmt_obj (&mtext, true, false);

                        if (sort != NULL)
                        {
                            rim_num lm = strlen (sort);
                            sort = rim_trim_ptr(sort,  &lm);
                            if (strcmp (sort, "\"quick_sort\"") && strcmp (sort, "\"merge_sort\"") && strcmp (sort, "\"shell_sort\"") && strcmp (sort, "\"binary_insertion_sort\"") && strcmp (sort, "\"tim_sort\"") && strcmp (sort, "\"heap_sort\"") && strcmp (sort, "\"merge_sort_in_place\"") && strcmp (sort, "\"selection_sort\"") && strcmp (sort, "\"grail_sort\"") && strcmp (sort, "\"sqrt_sort\"") && strcmp (sort, "\"bubble_sort\"")) rim_report_error ("sort-algorithm can be quick_sort, merge_sort, shell_sort, binary_insertion_sort, tim_sort, heap_sort, merge_sort_in_place, selection_sort, grail_sort, sqrt_sort, bubble_sort, found [%s]", sort);
                            // make sort good for oprintf(), i.e. remove quotes
                            sort++;
                            sort[strlen (sort)-1] = 0;
                        } else sort="quick_sort";
                        if (desc != NULL) desc = "desc_"; else desc = "";
                        // 
                        // No need to check_var sort b/c we make sure above it's one of the constants!!!
                        //
                        rim_num at = check_var (&mtext, RIM_DEFUNKN);
                        if (!cmp_type (at, RIM_DEFARRAYSTRING) && !cmp_type(at, RIM_DEFARRAYNUMBER) && !cmp_type(at, RIM_DEFARRAYDOUBLE)) rim_report_error("Unknown type [%s] in sort-array", typename(at));
                        if (cmp_type (at, RIM_DEFARRAYSTRING)) oprintf("rim_string_%s%s ((%s)->str, (const size_t)((%s)->tot_elem));\n", desc, sort, mtext, mtext);
                        else if (cmp_type (at, RIM_DEFARRAYNUMBER)) oprintf("rim_number_%s%s ((%s)->num, (const size_t)((%s)->tot_elem));\n", desc, sort, mtext, mtext);
                        else if (cmp_type (at, RIM_DEFARRAYDOUBLE)) oprintf("rim_double_%s%s ((%s)->dbl, (const size_t)((%s)->tot_elem));\n", desc, sort, mtext, mtext);
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "purge-array", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;
                        rim_mod_array = true;
                        carve_stmt_obj (&mtext, true, false);
                        //
                        rim_num at = check_var (&mtext, RIM_DEFUNKN);
                        if (!cmp_type (at, RIM_DEFARRAYSTRING) && !cmp_type(at, RIM_DEFARRAYNUMBER) && !cmp_type(at, RIM_DEFARRAYBOOL) && !cmp_type(at, RIM_DEFARRAYDOUBLE)) rim_report_error("Unknown type [%s] in purge-array", typename(at));
                        oprintf("rim_mem_process=(%s)->process;\n", mtext);
                        if (cmp_type (at, RIM_DEFARRAYSTRING)) oprintf("rim_purge_arraystring (%s);\n", mtext);
                        else if (cmp_type (at, RIM_DEFARRAYNUMBER)) oprintf("rim_purge_arraynumber (%s);\n", mtext);
                        else if (cmp_type (at, RIM_DEFARRAYBOOL)) oprintf("rim_purge_arraybool (%s);\n", mtext);
                        else if (cmp_type (at, RIM_DEFARRAYDOUBLE)) oprintf("rim_purge_arraydouble (%s);\n", mtext);
                        oprintf("rim_mem_process=0;\n");
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "read-array", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        rim_mod_array = true;
                        i = newI;
                        char *key = find_keyword (mtext, RIM_KEYKEY, 1);
                        char *val = find_keyword (mtext, RIM_KEYVALUE, 1);
                        char *del = find_keyword (mtext, RIM_KEYDELETE0, 1);


                        carve_statement (&key, "read-array", RIM_KEYKEY, 1, 1);
                        carve_statement (&val, "read-array", RIM_KEYVALUE, 1, 1);
                        carve_statement (&del, "read-array", RIM_KEYDELETE0, 0, 2);
                        carve_stmt_obj (&mtext, true, false);


                        rim_num at = check_var (&mtext, RIM_DEFUNKN);
                        if (!cmp_type (at, RIM_DEFARRAYSTRING) && !cmp_type(at, RIM_DEFARRAYNUMBER) && !cmp_type(at, RIM_DEFARRAYBOOL) && !cmp_type(at, RIM_DEFARRAYDOUBLE)) rim_report_error("Unknown type [%s] in read-array", typename(at));

                        char *delc = opt_clause(del, "true", "false", RIM_DEFBOOL);

                        if (cmp_type (at, RIM_DEFARRAYSTRING)) define_statement (&val, RIM_DEFSTRING, false); // exact length via pointer assignment as a return from rim_find_hash and value param from rim_next_hash
                        else if (cmp_type (at, RIM_DEFARRAYDOUBLE)) define_statement (&val, RIM_DEFDOUBLE, false); // exact length via pointer assignment as a return from rim_find_hash and value param from rim_next_hash
                        else if (cmp_type (at, RIM_DEFARRAYNUMBER)) define_statement (&val, RIM_DEFNUMBER, false); // exact length via pointer assignment as a return from rim_find_hash and value param from rim_next_hash
                        else if (cmp_type (at, RIM_DEFARRAYBOOL)) define_statement (&val, RIM_DEFBOOL, false); // exact length via pointer assignment as a return from rim_find_hash and value param from rim_next_hash


                        //
                        check_var (&delc, RIM_DEFBOOL);
                        check_var (&key, RIM_DEFNUMBER);


                        //
                        // For read-array here, the output value is empty string when not found; for that reason we always get value (_rim_hfind), even when status
                        // is not RIM_OKAY
                        //
                        oprintf("{\n");
                        if (cmp_type (at, RIM_DEFARRAYSTRING)) oprintf("char *_rim_afind = rim_read_arraystring (%s, %s);\n", mtext, key);
                        else if (cmp_type (at, RIM_DEFARRAYNUMBER)) oprintf("rim_num _rim_afind = rim_read_arraynumber (%s, %s);\n", mtext, key);
                        else if (cmp_type (at, RIM_DEFARRAYDOUBLE)) oprintf("rim_dbl _rim_afind = rim_read_arraydouble (%s, %s);\n", mtext, key);
                        else if (cmp_type (at, RIM_DEFARRAYBOOL)) oprintf("bool _rim_afind = rim_read_arraybool (%s, %s);\n", mtext, key);
                        // if asked to delete, generate code to delete (check for false because opt_clause will create it)
                        if (delc && strcmp (delc, "false"))
                        {
                            if (cmp_type (at, RIM_DEFARRAYSTRING)) oprintf ("if (%s) {rim_mem_dec_process(_rim_afind); %s->str[%s] = RIM_EMPTY_STRING; }\n", delc, mtext, key);
                            else if (cmp_type (at, RIM_DEFARRAYNUMBER))oprintf ("if (%s) {%s->num[%s] = 0; }\n", delc, mtext, key);
                            else if (cmp_type (at, RIM_DEFARRAYDOUBLE))oprintf ("if (%s) {%s->dbl[%s] = 0.0; }\n", delc, mtext, key);
                            else if (cmp_type (at, RIM_DEFARRAYBOOL)) oprintf ("if (%s) {%s->logic[%s] = false; }\n", delc, mtext, key);

                        }
                        // assign result, add reference if string
                        oprintf ("%s = _rim_afind; ", val);
                        oprintf("}\n");

                        rim_free(delc);
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "write-hash", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;
                        char *key = find_keyword (mtext, RIM_KEYKEY, 1);
                        char *val = find_keyword (mtext, RIM_KEYVALUE, 1);
                        char *oldd = find_keyword (mtext, RIM_KEYOLDVALUE, 1);
                        char *st = find_keyword (mtext, RIM_KEYSTATUS, 1);

                        carve_statement (&key, "write-hash", RIM_KEYKEY, 1, 1);
                        carve_statement (&val, "write-hash", RIM_KEYVALUE, 1, 1);
                        carve_statement (&oldd, "write-hash", RIM_KEYOLDVALUE, 0, 1);
                        carve_statement (&st, "write-hash", RIM_KEYSTATUS, 0, 1);
                        carve_stmt_obj (&mtext, true, false);
                        define_statement (&oldd, RIM_DEFSTRING, false); // exact length okay this is just pointer aliasing in rim_add_hash with RIM_EMPTY_STRING
                        define_statement (&st, RIM_DEFNUMBER, false);
                        make_mem (&key);
                        make_mem (&val);
                        //
                        check_var (&mtext, RIM_DEFHASH);
                        check_var (&key, RIM_DEFSTRING);
                        check_var (&val, RIM_DEFSTRING);

                        oprintf("rim_mem_process=(%s)->process;\n", mtext);

                        oprintf("rim_add_hash (%s, %s, %s, %s%s%s, %s%s%s);\n", mtext, key, val, oldd==NULL?"":"(void**)&(", oldd==NULL?"NULL":oldd, oldd==NULL?"":")", st==NULL?"":"&(", st==NULL?"NULL":st, st==NULL?"":")");
                        oprintf("rim_mem_process=0;\n");
                        // both key and value are now referenced by the index as well as the original variables that put them here
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "read-hash", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;
                        char *key = find_keyword (mtext, RIM_KEYKEY, 1);
                        char *val = find_keyword (mtext, RIM_KEYVALUE, 1);
                        char *del = find_keyword (mtext, RIM_KEYDELETE0, 1);
                        char *st = find_keyword (mtext, RIM_KEYSTATUS, 1);
                        char *trav = find_keyword (mtext, RIM_KEYTRAVERSE, 1);
                        char *beg = NULL;

                        if (trav != NULL)
                        {
                            beg = find_keyword (mtext, RIM_KEYBEGIN, 1);
                        }


                        //
                        // Carving must be after find_keyword
                        //
                        if (trav != NULL)
                        {
                            carve_statement (&beg, "read-hash", RIM_KEYBEGIN, 0, 0);
                            carve_statement (&key, "read-hash", RIM_KEYKEY, 0, 1);
                            carve_statement (&val, "read-hash", RIM_KEYVALUE, 0, 1);
                            carve_statement (&trav, "read-hash", RIM_KEYTRAVERSE, 0, 0);
                        }
                        else
                        {
                            carve_statement (&key, "read-hash", RIM_KEYKEY, 1, 1);
                            carve_statement (&val, "read-hash", RIM_KEYVALUE, 1, 1);
                        }
                        carve_statement (&st, "read-hash", RIM_KEYSTATUS, 0, 1);
                        carve_statement (&del, "read-hash", RIM_KEYDELETE0, 0, 2);
                        carve_stmt_obj (&mtext, true, false);
                        if (trav == NULL)
                        {
                            make_mem(&key);
                        }

                        char *delc = opt_clause(del, "true", "false", RIM_DEFBOOL);

                        define_statement (&val, RIM_DEFSTRING, false); // exact length via pointer assignment as a return from rim_find_hash and value param from rim_next_hash
                        define_statement (&st, RIM_DEFNUMBER, false);

                        if (trav != NULL)
                        {
                            define_statement (&key, RIM_DEFSTRING, false); // exact length okay this is just pointer aliasing in rim_next_hash with RIM_EMPTY_STRING
                        }
                        else
                        {
                            // check_var is for INPUT vars only
                            check_var (&key, RIM_DEFSTRING);
                        }
                        //
                        check_var (&delc, RIM_DEFBOOL);
                        check_var (&mtext, RIM_DEFHASH);


                        //
                        // For read-hash here, the output value is empty string when not found; for that reason we always get value (_rim_hfind), even when status
                        // is not RIM_OKAY
                        //
                        if (trav == NULL)
                        {
                            oprintf("{ ");
                            oprintf("char *_rim_hfind = rim_find_hash (%s, %s, %s, &_rim_st, true);\n", mtext, key, delc);
                            oprintf ("if (_rim_st == RIM_OKAY) { %s = _rim_hfind; }\n",  val);
                            if (st==NULL) oprintf("else rim_report_error (\"%s\", \"%s\", (rim_num)%ld, _rim_st);\n", RIM_STMT_FAIL_CHECK, rim_valid_rim_name, lnum);
                            else oprintf ("%s=_rim_st;\n", st);
                            oprintf("}\n");

                        }
                        else
                        {
                            if ((key == NULL && val != NULL) || (key != NULL && val == NULL)) rim_report_error( "'key' and 'value' must both be in read-hash statement");
                            if (beg != NULL) oprintf ("rim_rewind_hash(%s);\n", mtext);
                            if (key != NULL) 
                            {
                                oprintf("{ ");
                                oprintf("char *_rim_hfind_v; char *_rim_hfind_k=rim_next_hash (%s, (void*)&(_rim_hfind_v), &_rim_st, %s);\n", mtext, delc);
                                //
                                oprintf ("if (_rim_st == RIM_OKAY) \n{ %s = _rim_hfind_v; \n", val);
                                oprintf (" %s = _rim_hfind_k; \n}\n", key);
                                //
                                //
                                // The following two are together: if status is not okay, AND if user didn't use 'status' clause,
                                // then report error
                                //
                                oprintf ("}\n");
                                if (st == NULL) oprintf(" else { rim_report_error (\"%s\", \"%s\", (rim_num)%ld, _rim_st);}\n", RIM_STMT_FAIL_CHECK, rim_valid_rim_name, lnum); else oprintf ("%s=_rim_st;\n", st);
                                //
                                //
                                //
                            }
                        }
                        rim_free(delc);
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "read-json", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;
                        char *key = find_keyword (mtext, RIM_KEYKEY, 1);
                        char *val = find_keyword (mtext, RIM_KEYVALUE, 1);
                        char *type = find_keyword (mtext, RIM_KEYTYPE, 1);
                        char *next = find_keyword (mtext, RIM_KEYNEXT, 1);


                        carve_statement (&key, "read-json", RIM_KEYKEY, 0, 1);
                        carve_statement (&val, "read-json", RIM_KEYVALUE, 0, 1);
                        carve_statement (&type, "read-json", RIM_KEYTYPE, 0, 1);
                        carve_statement (&next, "read-json", RIM_KEYNEXT, 0, 0);
                        carve_stmt_obj (&mtext, true, false);

                        define_statement (&key, RIM_DEFSTRING, false); 
                        define_statement (&val, RIM_DEFSTRING, false); 
                        define_statement (&type, RIM_DEFNUMBER, false);

                        check_var (&mtext, RIM_DEFJSON);
                        oprintf ("if ((%s) != NULL && ((%s)->node_r < (%s)->node_c)) { \n;\n", mtext, mtext, mtext);
                        if (key != NULL) 
                        {
                            oprintf ("(%s) = (%s)->nodes[(%s)->node_r].name;\n", key, mtext, mtext);
                        }
                        // if value is asked for, we copy value (lazy approch, faster) and we mark this value as 'alloced' 
                        // so when deleting json, we know to free it
                        // actual allocation is done only once of course, so we don't allocate the same string 10 times for accessing it 10 times
                        if (val != NULL) 
                        {
                            oprintf ("{rim_num _rim_json_node = (%s)->node_r; char *_rim_json_str = (%s)->nodes[_rim_json_node].str = ((%s)->nodes[_rim_json_node].alloced ? (%s)->nodes[_rim_json_node].str : rim_strdup((%s)->nodes[_rim_json_node].str)); (%s)->nodes[_rim_json_node].alloced=true;\n", mtext, mtext, mtext, mtext, mtext, mtext);
                            oprintf ("rim_mem_add_ref( _rim_json_str, 0);\n");
                            oprintf ("%s = _rim_json_str; } \n", val);
                        }
                        if (type != NULL) oprintf ("(%s) = (%s)->nodes[(%s)->node_r].type;\n", type, mtext, mtext);
                        if (next) oprintf ("(%s)->node_r++;\n", mtext);
                        oprintf ("} else { ;\n");
                        if (key != NULL) oprintf ("(%s) = RIM_EMPTY_STRING;\n", key);
                        if (val != NULL) oprintf ("(%s) = RIM_EMPTY_STRING;\n", val);
                        if (type != NULL) oprintf ("(%s) = RIM_JSON_TYPE_NONE;\n", type);
                        oprintf ("}\n");

                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "read-xml", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;
                        char *key = find_keyword (mtext, RIM_KEYKEY, 1);
                        char *val = find_keyword (mtext, RIM_KEYVALUE, 1);
                        char *next = find_keyword (mtext, RIM_KEYNEXT, 1);
                        char *st = find_keyword (mtext, RIM_KEYSTATUS, 1);


                        carve_statement (&key, "read-xml", RIM_KEYKEY, 0, 1);
                        carve_statement (&val, "read-xml", RIM_KEYVALUE, 0, 1);
                        carve_statement (&next, "read-xml", RIM_KEYNEXT, 0, 0);
                        carve_statement (&st, "read-xml", RIM_KEYSTATUS, 0, 1);
                        carve_stmt_obj (&mtext, true, false);

                        define_statement (&key, RIM_DEFSTRING, false); 
                        define_statement (&val, RIM_DEFSTRING, false); 
                        define_statement (&st, RIM_DEFNUMBER, false); 

                        check_var (&mtext, RIM_DEFXML);
                        oprintf ("if ((%s) != NULL && ((%s)->node_r < (%s)->node_c)) { \n;\n", mtext, mtext, mtext);
                        if (key != NULL) 
                        {
                            oprintf ("rim_mem_add_ref((%s)->nodes[(%s)->node_r].name, 0);\n", mtext, mtext);
                            oprintf ("(%s) = (%s)->nodes[(%s)->node_r].name;\n", key, mtext, mtext);
                        }
                        if (val != NULL) 
                        {
                            oprintf ("rim_mem_add_ref( (%s)->nodes[(%s)->node_r].str, 0);\n", mtext, mtext);
                            oprintf ("%s = (%s)->nodes[(%s)->node_r].str; \n", val, mtext,mtext);
                        }
                        if (next) oprintf ("(%s)->node_r++;\n", mtext);
                        if (st != NULL) oprintf ("%s=RIM_OKAY;\n", st);
                        oprintf ("} else { ;\n");
                        if (key != NULL) oprintf ("(%s) = RIM_EMPTY_STRING;\n", key);
                        if (val != NULL) oprintf ("(%s) = RIM_EMPTY_STRING;\n", val);
                        if (st) oprintf ("%s=RIM_ERR_EXIST;\n", st);
                        else oprintf("rim_report_error (\"%s\", \"%s\", (rim_num)%ld, (rim_num)RIM_ERR_EXIST);\n", RIM_STMT_FAIL_CHECK, rim_valid_rim_name, lnum);
                        oprintf ("}\n");

                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "json-doc", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;

                        char *noenum = find_keyword (mtext, RIM_KEYNOENUM, 1);
                        char *nodec = find_keyword (mtext, RIM_KEYNOENCODE, 1);
                        char *to = find_keyword (mtext, RIM_KEYTO, 1);
                        char *errp = find_keyword (mtext, RIM_KEYERRORPOSITION, 1);
                        char *errl = find_keyword (mtext, RIM_KEYERRORLINE, 1);
                        char *errc = find_keyword (mtext, RIM_KEYERRORCHAR, 1);
                        char *status = find_keyword (mtext, RIM_KEYSTATUS, 1);
                        char *errt = find_keyword (mtext, RIM_KEYERRORTEXT, 1);
                        char *len = find_keyword (mtext, RIM_KEYLENGTH, 1);
                        char *del = find_keyword (mtext, RIM_KEYDELETE, 1);

                        if (del != NULL && to != NULL) rim_report_error ("Only one of 'to' or 'delete' can be used");
                        if (del == NULL && to == NULL) rim_report_error ("At least one of 'to' or 'delete' must be used");
                        if ((nodec != NULL || noenum != NULL || errp != NULL || errl != NULL || errc != NULL || status != NULL || errt != NULL || len != NULL) && to == NULL) rim_report_error ("no other options can be used with 'delete'");

                        if (to != NULL) 
                        {
                            carve_statement (&errt, "json-doc", RIM_KEYERRORTEXT, 0, 1);
                            carve_statement (&status, "json-doc", RIM_KEYSTATUS, 0, 1);
                            carve_statement (&errp, "json-doc", RIM_KEYERRORPOSITION, 0, 1);
                            carve_statement (&errc, "json-doc", RIM_KEYERRORCHAR, 0, 1);
                            carve_statement (&errl, "json-doc", RIM_KEYERRORLINE, 0, 1);
                            carve_statement (&nodec, "json-doc", RIM_KEYNOENCODE, 0, 0);
                            carve_statement (&noenum, "json-doc", RIM_KEYNOENUM, 0, 0);
                            carve_statement (&len, "json-doc", RIM_KEYLENGTH, 0, 1);
                            carve_statement (&to, "json-doc", RIM_KEYTO, 0, 1);
                            carve_stmt_obj (&mtext, true, true);
                            define_statement (&errt, RIM_DEFSTRING, false);
                            define_statement (&status, RIM_DEFNUMBER, false);
                            define_statement (&errp, RIM_DEFNUMBER, false);
                            define_statement (&errl, RIM_DEFNUMBER, false);
                            define_statement (&errc, RIM_DEFNUMBER, false);
                            define_statement (&to, RIM_DEFJSON, true);
                            check_var (&len, RIM_DEFNUMBER);
                            check_var (&mtext, RIM_DEFSTRING);
                        }
                        if (del != NULL) 
                        {
                            carve_statement (&del, "json-doc", RIM_KEYDELETE, 0, 1);
                            carve_stmt_obj (&mtext, false, false);
                            check_var (&del, RIM_DEFJSON);
                        }

                        if (del != NULL) oprintf ("rim_del_json (&(%s));\n", del);
                        //


                        //
                        // Look for each option and collect relevant info
                        //
                        if (to != NULL)
                        {
                            oprintf ("rim_set_json (&(%s), %s, %s);\n", to, noenum == NULL?"false":"true", mtext);
                            oprintf ("rim_num rim_json_status_%ld = %s%srim_json_new ((%s)->data, NULL, (%s), %s, %s%s%s, %s%s%s);\n", json_id, errp == NULL ? "":errp, errp == NULL ? "":"=", to, len == NULL ? "-1" : len, nodec == NULL?"1":"0", errc==NULL?"":"&(",errc==NULL?"NULL":errc,errc==NULL?"":")",  errl==NULL?"":"&(",errl==NULL?"NULL":errl,errl==NULL?"":")");
                            if (status != NULL)
                            {
                                oprintf ("RIM_ERR0; %s = (rim_json_status_%ld == -1 ? RIM_OKAY : RIM_ERR_JSON);\n", status, json_id);
                            }
                            else
                            {
                                oprintf("if (rim_json_status_%ld != -1) rim_report_error (\"%s\", \"%s\", (rim_num)%ld, (rim_num)RIM_ERR_JSON);\n", json_id, RIM_STMT_FAIL_CHECK, rim_valid_rim_name, lnum);
                            }
                            if (errt != NULL)
                            {
                                oprintf ("%s = rim_json_err();\n", errt);
                            }
                            json_id++;
                        }

                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "xml-doc", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;
                        rim_mod_xml = true;

                        char *to = find_keyword (mtext, RIM_KEYTO, 1);
                        char *errl = find_keyword (mtext, RIM_KEYERRORLINE, 1);
                        char *errc = find_keyword (mtext, RIM_KEYERRORCHAR, 1);
                        char *status = find_keyword (mtext, RIM_KEYSTATUS, 1);
                        char *errt = find_keyword (mtext, RIM_KEYERRORTEXT, 1);
                        char *len = find_keyword (mtext, RIM_KEYLENGTH, 1);
                        char *del = find_keyword (mtext, RIM_KEYDELETE, 1);

                        if (del != NULL && to != NULL) rim_report_error ("Only one of 'to' or 'delete' can be used");
                        if (del == NULL && to == NULL) rim_report_error ("At least one of 'to' or 'delete' must be used");
                        if ((errl != NULL || errc != NULL || status != NULL || errt != NULL || len != NULL) && to == NULL) rim_report_error ("no other options can be used with 'delete'");

                        if (to != NULL) 
                        {
                            carve_statement (&errt, "xml-doc", RIM_KEYERRORTEXT, 0, 1);
                            carve_statement (&status, "xml-doc", RIM_KEYSTATUS, 0, 1);
                            carve_statement (&errc, "xml-doc", RIM_KEYERRORCHAR, 0, 1);
                            carve_statement (&errl, "xml-doc", RIM_KEYERRORLINE, 0, 1);
                            carve_statement (&len, "xml-doc", RIM_KEYLENGTH, 0, 1);
                            carve_statement (&to, "xml-doc", RIM_KEYTO, 0, 1);
                            carve_stmt_obj (&mtext, true, true);
                            define_statement (&errt, RIM_DEFSTRING, false);
                            define_statement (&status, RIM_DEFNUMBER, false);
                            define_statement (&errl, RIM_DEFNUMBER, false);
                            define_statement (&errc, RIM_DEFNUMBER, false);
                            define_statement (&to, RIM_DEFXML, true);
                            check_var (&len, RIM_DEFNUMBER);
                            check_var (&mtext, RIM_DEFSTRING);
                        }
                        if (del != NULL) 
                        {
                            carve_statement (&del, "xml-doc", RIM_KEYDELETE, 0, 1);
                            carve_stmt_obj (&mtext, false, false);
                            check_var (&del, RIM_DEFXML);
                        }

                        if (del != NULL) oprintf ("rim_del_xml (&(%s));\n", del);
                        //


                        //
                        // Look for each option and collect relevant info
                        //
                        if (to != NULL)
                        {
                            oprintf ("rim_set_xml (&(%s));\n", to);
                            oprintf ("rim_num rim_xml_status_%ld = rim_xml_new ((%s), (%s), %s%s%s, %s%s%s);\n", xml_id, mtext, len == NULL ? "-1" : len,    errc==NULL?"":"&(",        errc==NULL?"NULL":errc,          errc==NULL?"":")",  errl==NULL?"":"&(",errl==NULL?"NULL":errl,errl==NULL?"":")");
                            if (status != NULL)
                            {
                                oprintf ("RIM_ERR0; %s = (rim_xml_status_%ld == -1 ? RIM_OKAY : RIM_ERR_XML);\n", status, xml_id);
                            }
                            else
                            {
                                oprintf("if (rim_xml_status_%ld != -1) rim_report_error (\"%s\", \"%s\", (rim_num)%ld, (rim_num)RIM_ERR_XML);\n", xml_id, RIM_STMT_FAIL_CHECK, rim_valid_rim_name, lnum);
                            }
                            if (errt != NULL)
                            {
                                oprintf ("%s = rim_xml_err();\n", errt);
                            }
                            xml_id++;
                        }

                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "new-tree", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;
                        rim_mod_tree = true;

                        //
                        // General principle of memory allocation for objects: when there's a new-xxxx then it can be done only once per request
                        // and reused many times with read/write/purge and similar. Others that don't have new-xxxx, such as split-string, need delete.
                        //

                        char *keyas = find_keyword (mtext, RIM_KEYKEYAS, 1);
                        char *unsorted = find_keyword (mtext, RIM_KEYUNSORTED, 1);
                        char *process = find_keyword (mtext, RIM_KEYPROCESSSCOPE, 1);

                        carve_statement (&keyas, "new-tree", RIM_KEYKEYAS, 0, 1);
                        carve_statement (&unsorted, "new-tree", RIM_KEYUNSORTED, 0, 0);
                        carve_statement (&process, "new-tree", RIM_KEYPROCESSSCOPE, 0, 0);
                        carve_stmt_obj (&mtext, true, false);

                        if (process) define_statement (&mtext, RIM_DEFTREESTATIC, true); else define_statement (&mtext, RIM_DEFTREE, true);
                        //

                        if (keyas != NULL)
                        {
                            rim_num lm = strlen (keyas);
                            keyas = rim_trim_ptr(keyas,  &lm);
                            if (!strcmp (keyas, "\"positive integer\""))
                            {
                                keyas = "RIM_TREE_TYPE_NUM";
                            }
                            else rim_report_error ("Unknown key type [%s]", keyas);
                        } else keyas = "RIM_TREE_TYPE_STR";

                        oprintf("rim_mem_process=%s;\n", process?"RIM_MEM_PROCESS":"0");
                        oprintf("%s = rim_tree_create(%s, %s, %s);\n", mtext,keyas, unsorted==NULL?"true":"false", process?"RIM_MEM_PROCESS":"0");
                        oprintf("rim_mem_process=0;\n");
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "get-upload", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;

                        char *lfile = find_keyword (mtext, RIM_KEYLOCALFILE, 1);
                        char *cfile = find_keyword (mtext, RIM_KEYCLIENTFILE, 1);
                        char *size = find_keyword (mtext, RIM_KEYSIZE, 1);
                        char *ext = find_keyword (mtext, RIM_KEYEXTENSION, 1);
                        char *st = find_keyword (mtext, RIM_KEYSTATUS, 1);

                        carve_statement (&lfile, "get-upload", RIM_KEYLOCALFILE, 0, 1);
                        carve_statement (&cfile, "get-upload", RIM_KEYCLIENTFILE, 0, 1);
                        carve_statement (&size, "get-upload", RIM_KEYSIZE, 0, 1);
                        carve_statement (&ext, "get-upload", RIM_KEYEXTENSION, 0, 1);
                        carve_statement (&st, "get-upload", RIM_KEYSTATUS, 0, 1);
                        carve_stmt_obj (&mtext, true, false);
                        save_param (mtext, RIM_DEFSTRING);  // this way we don't even have to use get-param to get this one,
                                                           // file upload is a special case and shows up as a string param in URL 

                        if (lfile != NULL) define_statement (&lfile, RIM_DEFSTRING, false);
                        if (cfile != NULL) define_statement (&cfile, RIM_DEFSTRING, false);
                        if (ext != NULL) define_statement (&ext, RIM_DEFSTRING, false);
                        if (size != NULL) define_statement (&size, RIM_DEFNUMBER, false);
                        //

                        if (lfile == NULL) rim_report_error ("Must specify at least local-file clause in get-upload");
                        
                        // uploaded file name and its attributes *are* RimStone memory from rim_get_input(), HOWEVER, 
                        // what we're passing is not the variable but the NAME of the variable (as that's how it's stored in the hash),
                        // that's why we're quoting it here. Hence false for is_rim in rim_find_hash.
                        oprintf("{ rim_upload *_rim_hfind = (rim_upload*)rim_find_hash (rim_req.upload, \"%s\", false, &_rim_st, false);\n", mtext);
                        oprintf ("if (_rim_st == RIM_OKAY) {\n"); 
                        oprintf ("    rim_mem_add_ref(_rim_hfind->loc, 0);%s = _rim_hfind->loc; \n", lfile);
                        if (cfile != NULL) oprintf ("    rim_mem_add_ref(_rim_hfind->fname,0);%s = _rim_hfind->fname; \n", cfile);
                        if (ext != NULL) oprintf ("    rim_mem_add_ref(_rim_hfind->ext,0); %s = _rim_hfind->ext; \n", ext);
                        if (size != NULL) oprintf ("    %s = _rim_hfind->size; \n", size);
                        oprintf ("}\n");
                        if (st==NULL) oprintf("else rim_report_error (\"%s\", \"%s\", (rim_num)%ld, _rim_st);\n", RIM_STMT_FAIL_CHECK, rim_valid_rim_name, lnum);
                        else oprintf ("%s=_rim_st;\n", st);
                        oprintf("}\n");
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "get-lifo", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;

                        char *count = find_keyword (mtext, RIM_KEYCOUNT, 1);

                        carve_statement (&count, "get-lifo", RIM_KEYCOUNT, 0, 1);
                        carve_stmt_obj (&mtext, true, false);
                        check_var (&mtext, RIM_DEFLIFO);

                        if (count != NULL) define_statement (&count, RIM_DEFNUMBER, false);
                        //

                        rim_num opt = (count != NULL?1:0);
                        if (opt != 1) rim_report_error ("Must specify a single option");

                        if (count != NULL) oprintf("%s = (%s)->num_of;\n", count, mtext);
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "get-list", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;

                        char *count = find_keyword (mtext, RIM_KEYCOUNT, 1);

                        carve_statement (&count, "get-list", RIM_KEYCOUNT, 0, 1);
                        carve_stmt_obj (&mtext, true, false);

                        if (count != NULL) define_statement (&count, RIM_DEFNUMBER, false);
                        //

                        rim_num opt = (count != NULL?1:0);
                        if (opt != 1) rim_report_error ("Must specify a single option");

                        if (count != NULL) oprintf("%s = (%s)->num_of;\n", count, mtext);
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "get-tree", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;
                        rim_mod_tree = true;

                        char *count = find_keyword (mtext, RIM_KEYCOUNT, 1);
                        char *hops = find_keyword (mtext, RIM_KEYHOPS, 1);

                        carve_statement (&count, "get-tree", RIM_KEYCOUNT, 0, 1);
                        carve_statement (&hops, "get-tree", RIM_KEYHOPS, 0, 1);
                        carve_stmt_obj (&mtext, true, false);

                        if (count != NULL) define_statement (&count, RIM_DEFNUMBER, false);
                        if (hops != NULL) define_statement (&hops, RIM_DEFNUMBER, false);
                        //

                        rim_num opt = (count != NULL?1:0)+(hops != NULL?1:0);
                        if (opt != 1) rim_report_error ("Must specify a single option");

                        if (count != NULL) oprintf("%s = (%s)->count;\n", count, mtext);
                        if (hops != NULL) oprintf("%s = (%s)->hops;\n", hops, mtext);
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "purge-tree", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;
                        rim_mod_tree = true;

                        oprintf("rim_mem_process=(%s)->process;\n", mtext);
                        oprintf("rim_tree_purge_f (%s);\n", mtext);
                        oprintf("rim_mem_process=0;\n");
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "write-tree", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;
                        rim_mod_tree = true;

                        // TODO should add key-length to speed up write if length is known
                        char *key = find_keyword (mtext, RIM_KEYKEY, 1);
                        char *value = find_keyword (mtext, RIM_KEYVALUE, 1);
                        char *status = find_keyword (mtext, RIM_KEYSTATUS, 1);
                        char *ccursor = find_keyword (mtext, RIM_KEYNEWCURSOR, 1);

                        carve_statement (&key, "write-tree", RIM_KEYKEY, 1, 1);
                        carve_statement (&value, "write-tree", RIM_KEYVALUE, 1, 1);
                        carve_statement (&status, "write-tree", RIM_KEYSTATUS, 0, 1);
                        carve_statement (&ccursor, "write-tree", RIM_KEYNEWCURSOR, 0, 1);
                        carve_stmt_obj (&mtext, true, false);

                        define_statement (&status, RIM_DEFNUMBER, false);
                        define_statement (&ccursor, RIM_DEFTREECURSOR, true);

                        make_mem (&key);
                        make_mem (&value);
                        //
                        check_var (&mtext, RIM_DEFTREE);
                        check_var (&key, RIM_DEFSTRING);
                        check_var (&value, RIM_DEFSTRING);

                        char cname[100];
                        oprintf ("rim_tree_cursor rim_tree_curs%ld;\n", wcurs);
                        if (ccursor == NULL)
                        {
                            snprintf (cname, sizeof (cname), "_rim_tree_curs%ld", wcurs);
                            oprintf ("rim_tree_cursor *_rim_tree_curs%ld;\n", wcurs);
                            ccursor = cname;
                        }
                        oprintf ("%s = &rim_tree_curs%ld;\n", ccursor, wcurs);
                        oprintf("rim_mem_process=(%s)->process;\n", mtext);
                        oprintf ("rim_tree_insert_f (%s, %s, %s, -1, %s);\n", ccursor, mtext, key, value);
                        oprintf("rim_mem_process=0;\n");
                        if (status != NULL) oprintf ("%s = (%s)->status;\n", status, ccursor);
                        else oprintf("if ((%s)->status != RIM_OKAY) rim_report_error (\"%s\", \"%s\", (rim_num)%ld, (%s)->status);\n", ccursor, RIM_STMT_FAIL_CHECK, rim_valid_rim_name, lnum, ccursor);

                        wcurs++;
                        continue;

                    }
                    else if ((newI=recog_statement(line, i, "read-tree", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;
                        rim_mod_tree = true;

                        char *key = find_keyword (mtext, RIM_KEYEQUAL, 1);
                        char *mink = find_keyword (mtext, RIM_KEYMINKEY, 1);
                        char *maxk = find_keyword (mtext, RIM_KEYMAXKEY, 1);
                        char *lkey = find_keyword (mtext, RIM_KEYLESSER, 1);
                        char *lekey = find_keyword (mtext, RIM_KEYLESSEREQUAL, 1);
                        char *gkey = find_keyword (mtext, RIM_KEYGREATER, 1);
                        char *gekey = find_keyword (mtext, RIM_KEYGREATEREQUAL, 1);
                        char *status = find_keyword (mtext, RIM_KEYSTATUS, 1);
                        char *ccursor = find_keyword (mtext, RIM_KEYNEWCURSOR, 1);
                        // Begin read-tree 
                        char *getkey = find_keyword (mtext, RIM_KEYKEY, 1);
                        char *value = find_keyword (mtext, RIM_KEYVALUE, 1);
                        char *upd = find_keyword (mtext, RIM_KEYUPDATEVALUE, 1);
                        // End read-tree 

                        carve_statement (&key, "read-tree", RIM_KEYEQUAL, 0, 1);
                        carve_statement (&lekey, "read-tree", RIM_KEYLESSEREQUAL, 0, 1);
                        carve_statement (&gekey, "read-tree", RIM_KEYGREATEREQUAL, 0, 1);
                        carve_statement (&lkey, "read-tree", RIM_KEYLESSER, 0, 1);
                        carve_statement (&gkey, "read-tree", RIM_KEYGREATER, 0, 1);
                        carve_statement (&mink, "read-tree", RIM_KEYMINKEY, 0, 0);
                        carve_statement (&maxk, "read-tree", RIM_KEYMAXKEY, 0, 0);
                        carve_statement (&status, "read-tree", RIM_KEYSTATUS, 0, 1);
                        carve_statement (&ccursor, "read-tree", RIM_KEYNEWCURSOR, 0, 1);
                        // Begin read-tree
                        carve_statement (&value, "read-tree", RIM_KEYVALUE, 0, 1);
                        carve_statement (&upd, "read-tree", RIM_KEYUPDATEVALUE, 0, 1);
                        carve_statement (&getkey, "read-tree", RIM_KEYKEY, 0, 1);
                        // End read-tree
                        carve_stmt_obj (&mtext, true, false);

                        define_statement (&ccursor, RIM_DEFTREECURSOR, true);
                        define_statement (&status, RIM_DEFNUMBER, false);
                        // Begin read-tree
                        define_statement (&value, RIM_DEFSTRING, false); // exact length okay this is just pointer aliasing from found tree node
                        define_statement (&getkey, RIM_DEFSTRING, false); // exact length okay this is just pointer aliasing from found tree node
                        make_mem (&key);
                        make_mem (&lkey);
                        make_mem (&lekey);
                        make_mem (&gekey);
                        make_mem (&gkey);
                        make_mem (&upd);
                        // End read-tree
                        //
                        check_var (&mtext, RIM_DEFTREE);
                        check_var (&key, RIM_DEFSTRING);
                        check_var (&lekey, RIM_DEFSTRING);
                        check_var (&gekey, RIM_DEFSTRING);
                        check_var (&lkey, RIM_DEFSTRING);
                        check_var (&gkey, RIM_DEFSTRING);

                        // for key accessing, only one of the following can be used
                        rim_num is_one = (key!=NULL?1:0)+(lekey!=NULL?1:0)+(gekey!=NULL?1:0)+(lkey!=NULL?1:0)+(gkey!=NULL?1:0)+(mink!=NULL?1:0)+(maxk!=NULL?1:0);
                        if (is_one > 1) rim_report_error ("Can use only one of key, lesser, greater, lesser-equal, greater-equal, min-key or max-key clauses");
                        if (is_one != 1) rim_report_error ("Must specify one of key, lesser, greater, lesser-equal, greater-equal, min-key or max-key clauses");


                        char cname[100];
                        if (ccursor == NULL) 
                        {
                            snprintf (cname, sizeof (cname), "_rim_tree_curs%ld", wcurs);
                            oprintf ("rim_tree_cursor *_rim_tree_curs%ld;\n", wcurs);
                            ccursor = cname;
                        }
                        oprintf ("rim_tree_cursor rim_tree_curs%ld;\n", wcurs);
                        oprintf ("%s = &rim_tree_curs%ld;\n", ccursor, wcurs);
                        // These each set ->current alone (no res or rkey)
                        if (mink != NULL)
                        {
                            oprintf ("rim_tree_min_f (%s, %s);\n", ccursor, mtext);
                        }
                        else if (maxk != NULL)
                        {
                            oprintf ("rim_tree_max_f (%s, %s);\n", ccursor, mtext);
                        }
                        else if (lkey != NULL)
                        {
                            oprintf ("rim_tree_search_lesser_equal_f (%s, %s, false, %s, -1);\n",  ccursor, mtext, lkey);
                        }
                        else if (gkey != NULL)
                        {
                            oprintf ("rim_tree_search_greater_equal_f (%s, %s, false, %s, -1);\n",  ccursor, mtext, gkey);
                        }
                        else if (lekey != NULL)
                        {
                            oprintf ("rim_tree_search_lesser_equal_f (%s, %s, true, %s, -1);\n",  ccursor, mtext, lekey);
                        }
                        else if (gekey != NULL)
                        {
                            oprintf ("rim_tree_search_greater_equal_f (%s, %s, true, %s, -1);\n",  ccursor, mtext, gekey);
                        }
                        else if (key != NULL)
                        {
                            oprintf ("rim_tree_search_f (%s, %s, %s, -1);\n", ccursor , mtext, key);
                        }
                        else rim_report_error ("Unknown read-tree key");

                        // after searching or deleting, get the result and the key affected, as well as status
                        if (status != NULL) oprintf ("%s = (%s)->status;\n", status, ccursor);

                        // Begin read-tree 
                        oprintf ("if ((%s)->status == RIM_OKAY) {\n", ccursor);
                        if (value != NULL) 
                        {
                            oprintf ("%s = (%s)->current->data; \n", value, ccursor);
                        }
                        if (getkey != NULL) 
                        {
                            oprintf ("%s = (%s)->current->key; \n", getkey, ccursor);
                        }
                        //
                        // checking var (and building upd variable) *MUST* be done here after the value is obtained above
                        // so we canuse it in updating in the same read-tree, as in:
                        // read-tree t key k value v update-value "new+"v
                        //
                        check_var (&upd, RIM_DEFSTRING);
                        if (upd != NULL) 
                        {
                            oprintf ("rim_mem_add_ref (%s, (%s)->process);\n", upd, mtext);
                            oprintf ("rim_mem_dec_process ((%s)->current->data);\n", ccursor);
                            oprintf ("(%s)->current->data = %s;\n", ccursor, upd); 
                        }
                        //
                        // The following two are together: if status is not okay, AND if user didn't use 'status' clause,
                        // then report error
                        //
                        oprintf ("}\n");
                        if (status == NULL) oprintf(" else { rim_report_error (\"%s\", \"%s\", (rim_num)%ld, (%s)->status);}\n", RIM_STMT_FAIL_CHECK, rim_valid_rim_name, lnum, ccursor);
                        //
                        //
                        //
                        // End read-tree 
                        wcurs++;
                        continue;

                    }
                    else if ((newI=recog_statement(line, i, "delete-tree", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;
                        rim_mod_tree = true;

                        char *key = find_keyword (mtext, RIM_KEYKEY, 1);
                        char *value = find_keyword (mtext, RIM_KEYVALUE, 1);
                        char *status = find_keyword (mtext, RIM_KEYSTATUS, 1);

                        carve_statement (&key, "delete-tree", RIM_KEYKEY, 1, 1);
                        carve_statement (&status, "delete-tree", RIM_KEYSTATUS, 0, 1);
                        carve_statement (&value, "delete-tree", RIM_KEYVALUE, 0, 1);
                        carve_stmt_obj (&mtext, true, false);
                        make_mem(&key);

                        define_statement (&status, RIM_DEFNUMBER, false);
                        define_statement (&value, RIM_DEFSTRING, false); // exact length okay this is just pointer aliasing from found tree node
                        //
                        check_var (&mtext, RIM_DEFTREE);
                        check_var (&key, RIM_DEFSTRING);

                        oprintf ("rim_tree_cursor rim_tree_curs%ld;\n", wcurs);
                        oprintf ("rim_tree_cursor *_rim_tree_curs%ld = &rim_tree_curs%ld;\n", wcurs, wcurs);
                        oprintf("rim_mem_process=(%s)->process;\n", mtext);
                        oprintf ("rim_tree_delete_f (_rim_tree_curs%ld, %s, %s);\n", wcurs, mtext, key);
                        oprintf("rim_mem_process=0;\n");

                        // after searching or deleting, get the result and the key affected, as well as status
                        if (status != NULL) oprintf ("%s = _rim_tree_curs%ld->status;\n", status, wcurs);
                        // if value isn't requested, delete it. 
                        // note that key is automatically deleted in rim_tree_delete_f() above
                        oprintf ("if (_rim_tree_curs%ld->status == RIM_OKAY) { ", wcurs);
                        // no change in reference count, because tree node no longer points to value (-1), and new variable points to it now (+1), so 0 total change.
                        if (value != NULL) oprintf ("%s = _rim_tree_curs%ld->res;\n", value, wcurs);
                        else oprintf ("rim_mem_dec_process(_rim_tree_curs%ld->res);\n", wcurs);
                        //
                        // The following two are together: if status is not okay, AND if user didn't use 'status' clause,
                        // then report error
                        //
                        oprintf ("}\n");
                        //If delete-tree doesn't delete, that's not considered application error!
                        //if (status == NULL) oprintf(" else { rim_report_error (\"%s\", \"%s\", (rim_num)%ld, _rim_tree_curs%ld->status);}\n", RIM_STMT_FAIL_CHECK, rim_valid_rim_name, lnum, wcurs);
                        //
                        //
                        //
                        wcurs++;
                        continue;

                    }
                    else if ((newI=recog_statement(line, i, "use-cursor", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;
                        rim_mod_tree = true;

                        char *cur = find_keyword (mtext, RIM_KEYCURRENT, 1);
                        char *lkey = find_keyword (mtext, RIM_KEYGETLESSER, 1);
                        char *gkey = find_keyword (mtext, RIM_KEYGETGREATER, 1);
                        char *status = find_keyword (mtext, RIM_KEYSTATUS, 1);
                        // Begin read-tree 
                        char *getkey = find_keyword (mtext, RIM_KEYKEY, 1);
                        char *value = find_keyword (mtext, RIM_KEYVALUE, 1);
                        char *upd = find_keyword (mtext, RIM_KEYUPDATEVALUE, 1);
                        // End read-tree 

                        carve_statement (&cur, "use-cursor", RIM_KEYCURRENT, 0, 0);
                        carve_statement (&lkey, "use-cursor", RIM_KEYGETLESSER, 0, 0);
                        carve_statement (&gkey, "use-cursor", RIM_KEYGETGREATER, 0, 0);
                        carve_statement (&status, "use-cursor", RIM_KEYSTATUS, 0, 1);
                        // Begin read-tree
                        carve_statement (&value, "use-cursor", RIM_KEYVALUE, 0, 1);
                        carve_statement (&upd, "use-cursor", RIM_KEYUPDATEVALUE, 0, 1);
                        carve_statement (&getkey, "use-cursor", RIM_KEYKEY, 0, 1);
                        // End read-tree
                        carve_stmt_obj (&mtext, true, false);

                        define_statement (&status, RIM_DEFNUMBER, false);
                        // Begin read-tree
                        define_statement (&value, RIM_DEFSTRING, false); // exact length okay this is just pointer aliasing from found tree node
                        define_statement (&getkey, RIM_DEFSTRING, false); // exact length okay this is just pointer aliasing from found tree node
                        make_mem (&upd);
                        // End read-tree
                        //
                        check_var (&mtext, RIM_DEFTREECURSOR);
                        check_var (&upd, RIM_DEFSTRING);

                        char *ucursor = mtext;

                        if (lkey != NULL)
                        {
                            oprintf ("if ((%s)->current != NULL) { if ((%s)->root->sorted) { if ((%s)->current->dlist[RIM_TREE_LESSER_LIST]) { (%s)->current = (%s)->current->dlist[RIM_TREE_LESSER_LIST]; (%s)->status = RIM_OKAY; } else {(%s)->status = RIM_ERR_EXIST;} } else { rim_tree_search_lesser_equal_f (%s, (%s)->root, false, (%s)->current->key, -1); } }\n", ucursor, ucursor,ucursor, ucursor, ucursor, ucursor, ucursor, ucursor, ucursor, ucursor);
                        }
                        else if (gkey != NULL)
                        {
                            oprintf ("if ((%s)->current != NULL) { if ((%s)->root->sorted) { if ((%s)->current->dlist[RIM_TREE_GREATER_LIST]) { (%s)->current = (%s)->current->dlist[RIM_TREE_GREATER_LIST]; (%s)->status = RIM_OKAY; } else {(%s)->status = RIM_ERR_EXIST;} } else { rim_tree_search_greater_equal_f (%s, (%s)->root, false, (%s)->current->key, -1); } }\n", ucursor, ucursor,ucursor, ucursor, ucursor, ucursor, ucursor, ucursor, ucursor, ucursor);
                        }
                        else if (cur != NULL)
                        {
                            ; // nothing
                        }
                        else rim_report_error ("Must use one of current, get-lesser or get-greater clauses");
                        if (status != NULL) oprintf ("%s = (%s)->status;\n", status, ucursor);
                        // Begin read-tree 
                        oprintf ("if ((%s)->status == RIM_OKAY) {\n", ucursor);
                        //
                        if (value != NULL) 
                        {
                            // delete reference to where value/key was pointing to, assuming it existed
                            oprintf ("%s = (%s)->current->data; \n", value, ucursor);
                        }
                        if (getkey != NULL) 
                        {
                            // delete reference to where value/key was pointing to, assuming it existed
                            oprintf ("%s = (%s)->current->key; \n", getkey, ucursor);
                        }
                        if (upd != NULL) 
                        {
                            oprintf ("rim_mem_add_ref (%s, (%s)->root->process);\n", upd, ucursor);
                            oprintf ("rim_mem_dec_process ((%s)->current->data);\n", ucursor);
                            oprintf ("(%s)->current->data = %s;\n",  ucursor, upd);
                        }
                        //
                        //
                        // The following two are together: if status is not okay, AND if user didn't use 'status' clause,
                        // then report error
                        //
                        oprintf ("}\n");
                        if (status == NULL) oprintf(" else { rim_report_error (\"%s\", \"%s\", (rim_num)%ld, (%s)->status);}\n", RIM_STMT_FAIL_CHECK, rim_valid_rim_name, lnum, ucursor);
                        //
                        //
                        // End read-tree 
                        continue;

                    }
                    else if ((newI=recog_statement(line, i, "do-once", &mtext, &msize, 1, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;

                        carve_stmt_obj (&mtext, false, false);
                        if (do_once_open) rim_report_error ("Cannot use do-once inside another do-once statement");
                        do_once_open = true;
                        do_once_line = lnum;
                        do_once_level = rim_level; // this is the level that process-scoped variables will have, assuming 
                                                  // they are created at do-once level. Any proces-scoped variable created within
                                                  // any {} within do-scope will NOT have this level, but rather whatever leve it is on!
                                                  // So the process-scoped variables that will be affected are those with rim_level + 1 and
                                                  // none other.

                        oprintf ("static bool _rim_do_once_%ld = false;\n", do_once);
                        oprintf ("if (_rim_do_once_%ld) goto _rim_end_do_once_%ld;\n", do_once, do_once);
                        oprintf ("_rim_do_once_%ld = true;\n", do_once);

                        check_level ("{"); // start new scope for do-once

                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "end-do-once", &mtext, &msize, 1, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;
                        carve_stmt_obj (&mtext, false, false);
                        do_once_open = false;

                        check_level ("}"); // end new scope for do-once
                                           
                        oprintf ("_rim_end_do_once_%ld: ;\n",do_once); // must have empty ; statement
                                                                       // to avoid syntax errors on older compilers

                        do_once++;

                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "copy-file", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;
                        char *copy_to = find_keyword (mtext, RIM_KEYTO, 1);
                        char *status = find_keyword (mtext, RIM_KEYSTATUS, 1);
                        carve_statement (&copy_to, "copy-file", RIM_KEYTO, 1, 1);
                        carve_statement (&status, "copy-file", RIM_KEYSTATUS, 0, 1);
                        carve_stmt_obj (&mtext, true, true);
                        make_mem(&copy_to);
                        define_statement (&status, RIM_DEFNUMBER, false);
                        //
                        check_var (&mtext, RIM_DEFSTRING);
                        check_var (&copy_to, RIM_DEFSTRING);
                        char *copy_from = mtext;
                        oprintf("_rim_st=rim_copy_file (%s, %s);\n", copy_from, copy_to);
                        if (status != NULL) oprintf ("%s=_rim_st;\n", status);
                        else oprintf("if (_rim_st < 0) rim_report_error (\"%s\", \"%s\", (rim_num)%ld, _rim_st);\n", RIM_STMT_FAIL_CHECK, rim_valid_rim_name, lnum);
                        continue;

                    }
                    else if ((newI=recog_statement(line, i, "finish-output", &mtext, &msize, 1, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;

                        carve_stmt_obj (&mtext, false, false);
                        oprintf("rim_SERVICE_Finish();\n");
                        oprintf("rim_disable_output();\n");
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "quit-process", &mtext, &msize, 1, &rim_is_inline)) != 0)
                    {
                        RIM_GUARD
                        i = newI;

                        // quit this process as if mrim sent stop command
                        oprintf("raise(SIGTERM); return 0;\n");
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "return-handler", &mtext, &msize, 1, &rim_is_inline)) != 0 ||
                          (newI1=recog_statement(line, i, "return-handler", &mtext, &msize, 0, &rim_is_inline)) != 0)
                    {
                        RIM_GUARD
                        i = newI+newI1;
                        
                        if (newI1 != 0)  // returned value
                        {
                            carve_stmt_obj (&mtext, true, false);
                            check_var (&mtext, RIM_DEFNUMBER);
                            // if this is top level handler, it's also the same as exit-handler
                            oprintf("return rim_req.sub_depth==1 ? (rim_req.ec = (%s)): (%s); \n", mtext, mtext);
                        } 
                        else
                        {
                            // return from handler, no set return status (default 0), use set/get-param
                            oprintf("return 0;\n");
                        }
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "exit-handler", &mtext, &msize, 1, &rim_is_inline)) != 0  
                        || (newI1=recog_statement(line, i, "exit-handler", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI+newI1;

                        if (newI1 != 0) 
                        {
                            carve_stmt_obj (&mtext, true, false);
                            oprintf("rim_req.ec=(%s);\n", mtext);
                        }

                        // return 1 to sigsetjmp so it differs from the first time around
                        oprintf("rim_exit_request(1);\n");
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "file-position", &mtext, &msize, 0, &rim_is_inline)) != 0)
                    {
                        RIM_GUARD
                        i = newI;
                        char *fileid = find_keyword (mtext, RIM_KEYFILEID, 1);
                        char *get = find_keyword (mtext, RIM_KEYGET, 1);
                        char *set = find_keyword (mtext, RIM_KEYSET, 1);
                        char *status = find_keyword (mtext, RIM_KEYSTATUS, 1);

                        carve_statement (&fileid, "file-position", RIM_KEYFILEID, 1, 1);
                        carve_statement (&get, "file-position", RIM_KEYGET, 0, 1);
                        carve_statement (&set, "file-position", RIM_KEYSET, 0, 1);
                        carve_statement (&status, "file-position", RIM_KEYSTATUS, 0, 1);
                        carve_stmt_obj (&mtext, false, false);

                        define_statement (&get, RIM_DEFNUMBER, false);
                        define_statement (&status, RIM_DEFNUMBER, false);
                        //
                        check_var (&set, RIM_DEFNUMBER);
                        check_var (&fileid, RIM_DEFFILE);

                        // Fatal if not open
                        oprintf ("if ((%s) == NULL) rim_report_error (\"Cannot position in file that is not open\");\n", fileid);

                        if (get == NULL && set == NULL)  rim_report_error( "either 'get' or 'set' must be used in file-position statement");
                        if (get != NULL && set != NULL)  rim_report_error( "cannot specify both 'get' and 'set' in file-position statement");
                        if (get != NULL) oprintf("_rim_st=rim_get_file_pos (*((%s)->f), &(%s));\n", fileid, get);
                        else if (set != NULL) oprintf("_rim_st=rim_set_file_pos (*((%s)->f), %s);\n", fileid, set);
                        if (status != NULL) oprintf ("%s=_rim_st;\n", status);
                        else oprintf("if (_rim_st != RIM_OKAY) rim_report_error (\"%s\", \"%s\", (rim_num)%ld, _rim_st);\n", RIM_STMT_FAIL_CHECK, rim_valid_rim_name, lnum);
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "close-file", &mtext, &msize, 0, &rim_is_inline)) != 0)
                    {
                        RIM_GUARD
                        i = newI;
                        char *fileid = find_keyword (mtext, RIM_KEYFILEID, 1);
                        char *st = find_keyword (mtext, RIM_KEYSTATUS, 1);

                        carve_statement (&fileid, "close-file", RIM_KEYFILEID, 1, 1);
                        carve_statement (&st, "close-file", RIM_KEYSTATUS, 0, 1);
                        carve_stmt_obj (&mtext, false, false);
                        define_statement (&st, RIM_DEFNUMBER, false);
                        //
                        check_var (&fileid, RIM_DEFFILE);
                        // If already closed, do not mess with it again
                        oprintf ("if ((%s) != NULL ) { _rim_st = rim_fclose (*((%s)->f));\n", fileid, fileid);
                       // we do not check for status not being checked here
                        if (st != NULL) oprintf("%s = _rim_st;\n", st);
                        // set file pointer to NULL, because that's checked in rim_done()
                        // this is needed beyond rim_done(), any file op would check if NULL so avoids crashes
                        oprintf("rim_free_int ((%s)->f);\n", fileid); // see comment in close-dir
                        oprintf("rim_free_int (%s); \n", fileid); 
                        oprintf ("(%s) = NULL; }\n", fileid);
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "open-dir", &mtext, &msize, 0, &rim_is_inline)) != 0)
                    {
                        RIM_GUARD
                        i = newI;
                        char *dirid = find_keyword (mtext, RIM_KEYDIRID, 1);
                        char *st = find_keyword (mtext, RIM_KEYSTATUS, 1);

                        carve_statement (&dirid, "open-dir", RIM_KEYDIRID, 1, 1);
                        carve_statement (&st, "open-dir", RIM_KEYSTATUS, 0, 1);
                        carve_stmt_obj (&mtext, true, true); 

                        define_statement (&dirid, RIM_DEFDIR, true);
                        define_statement (&st, RIM_DEFNUMBER, false);
                        //
                        check_var (&mtext, RIM_DEFSTRING);

                        oprintf("%s = rim_calloc (1, sizeof(rim_directory));\n", dirid); // this sets dir pointer dir to NULL
                        // use the same count for internals as for files
                        // Dir/file descriptor cannot be static because when in a loop, each next open-dir overwrites the old.
                        // When freeing, all of them point to the last. Once it's freed the first time, it's set to NULL, and all others
                        // are NEVER freed, resulting in massive leak, which usually ends with an error that there's too many files open
                        // since we only release 1 out of N in a loop!
                        oprintf("DIR **_rim_dir_%ld = rim_malloc (sizeof(DIR*));\n", file_count);
                        oprintf("*_rim_dir_%ld = opendir(%s);\n", file_count, mtext); 
                        if (st == NULL) oprintf("if (*_rim_dir_%ld == NULL) rim_report_error (\"%s\", \"%s\", (rim_num)%ld, (rim_num)RIM_ERR_OPEN);\n", file_count,RIM_STMT_FAIL_CHECK, rim_valid_rim_name, lnum);
                        else oprintf("if (*_rim_dir_%ld == NULL) { RIM_ERR; %s=RIM_ERR_OPEN;} else %s=RIM_OKAY;\n", file_count, st, st);
                        // set DIR*, and also the index into gg[] (the memind member), so that when closing, it can 
                        // clear the gg[]->ptr and make rim_done() faster
                        // we set ->dir to be the pointer to
                        // dir descriptor even if NULL, because ->dir is DIR** and should NEVER be NULL, rather *(of->dir) should be null
                        oprintf ("(%s)->dir = _rim_dir_%ld; if (*_rim_dir_%ld != NULL) {rim_mem_set_status (rim_mem_get_id(_rim_dir_%ld), RIM_MEM_DIR);}\n", dirid,  file_count, file_count, file_count);
                        file_count++;
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "read-dir", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;
                        char *to = find_keyword (mtext, RIM_KEYTO, 1);
                        char *type = find_keyword (mtext, RIM_KEYTYPE, 1);
                        char *st = find_keyword (mtext, RIM_KEYSTATUS, 1);


                        carve_statement (&type, "read-dir", RIM_KEYTYPE, 0, 1);
                        carve_statement (&to, "read-dir", RIM_KEYTO, 1, 1);
                        carve_statement (&st, "read-dir", RIM_KEYSTATUS, 0, 1);
                        carve_stmt_obj (&mtext, true, false);

                        define_statement (&type, RIM_DEFNUMBER, false); 
                        define_statement (&to, RIM_DEFSTRING, false); 
                        define_statement (&st, RIM_DEFNUMBER, false); 

                        check_var (&mtext, RIM_DEFDIR);

                        // Fatal if not open
                        oprintf ("if ((%s) == NULL) rim_report_error (\"Cannot read from directory that is not open\");\n", mtext);

                        oprintf ("struct dirent *_rim_tmp_dir;\n");
                        oprintf ("if (*((%s)->dir) != NULL && (_rim_tmp_dir = readdir(*((%s)->dir))) != NULL ) { \n;\n", mtext, mtext);
                        oprintf ("rim_mem_add_ref((%s) = rim_strdup(_rim_tmp_dir->d_name),0);\n", to);
                        if (st != NULL) oprintf ("%s=RIM_OKAY;\n", st);
                        if (type != NULL) 
                        {
                            oprintf ("if (_rim_tmp_dir->d_type == DT_REG) (%s) = RIM_FILE;\n\
                                    else if (_rim_tmp_dir->d_type == DT_DIR) (%s) = RIM_DIR;\n\
                                    else if (_rim_tmp_dir->d_type == DT_LNK) (%s) = RIM_LINK;\n\
                                    else if (_rim_tmp_dir->d_type == DT_SOCK) (%s) = RIM_SOCK;\n\
                                    else if (_rim_tmp_dir->d_type == DT_FIFO) (%s) = RIM_FIFO;\n\
                                    else if (_rim_tmp_dir->d_type == DT_CHR) (%s) = RIM_CHAR;\n\
                                    else if (_rim_tmp_dir->d_type == DT_BLK) (%s) = RIM_BLOCK;\n\
                                    else if (_rim_tmp_dir->d_type == DT_UNKNOWN) (%s) = RIM_UNKNOWN;\n", type, type, type, type, type, type, type, type);
                        }
                        oprintf ("} else { ;\n");
                        oprintf ("(%s) = RIM_EMPTY_STRING;\n", to);
                        if (st) oprintf ("%s=RIM_ERR_EXIST;\n", st);
                        else oprintf("rim_report_error (\"%s\", \"%s\", (rim_num)%ld, (rim_num)RIM_ERR_EXIST);\n", RIM_STMT_FAIL_CHECK, rim_valid_rim_name, lnum);
                        oprintf ("}\n");

                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "close-dir", &mtext, &msize, 0, &rim_is_inline)) != 0)
                    {
                        RIM_GUARD
                        i = newI;
                        char *st = find_keyword (mtext, RIM_KEYSTATUS, 1);

                        carve_statement (&st, "close-dir", RIM_KEYSTATUS, 0, 1);
                        carve_stmt_obj (&mtext, true, false);
                        define_statement (&st, RIM_DEFNUMBER, false);
                        //
                        check_var (&mtext, RIM_DEFDIR);

                        // when error is RimStone one (in this case close-dir with NULL directory, then we use RIM_ERR0
                        // closing it though when there's nothing (i.e. already closed) is not considered an error
                        oprintf ("if ((%s) != NULL) { _rim_st = rim_dclose (*((%s)->dir));\n", mtext, mtext);
                       // we do not check for status not being checked here
                        if (st != NULL) oprintf("%s = _rim_st;\n", st);
                        // set dir pointer to NULL, because that's checked in rim_done()
                        // this is needed beyond rim_done(), any file op would check if NULL so avoids crashes
                        oprintf("rim_free_int ((%s)->dir);\n", mtext); // also remove DIR*, which is a separate alloc
                                                                          // because if close-dir not used, then it when freed
                                                                          // we also do the closedir()
                        oprintf("rim_free_int (%s);\n", mtext); 
                        oprintf("(%s) = NULL; }\n", mtext); 
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "open-file", &mtext, &msize, 0, &rim_is_inline)) != 0)
                    {
                        RIM_GUARD
                        i = newI;
                        char *fileid = find_keyword (mtext, RIM_KEYFILEID, 1);
                        char *newt = find_keyword (mtext, RIM_KEYNEWTRUNCATE, 1);
                        char *st = find_keyword (mtext, RIM_KEYSTATUS, 1);

                        carve_statement (&newt, "open-file", RIM_KEYNEWTRUNCATE, 0, 0);
                        carve_statement (&fileid, "open-file", RIM_KEYFILEID, 1, 1);
                        carve_statement (&st, "open-file", RIM_KEYSTATUS, 0, 1);
                        carve_stmt_obj (&mtext, true, true); 

                        define_statement (&fileid, RIM_DEFFILE, true);
                        define_statement (&st, RIM_DEFNUMBER, false);
                        //
                        check_var (&mtext, RIM_DEFSTRING);

                        //RIM_ERR is set in rim_fopen
                        oprintf("%s = rim_calloc (1, sizeof(rim_file));\n", fileid); // this sets file pointer f to NULL
                        // need static int * so that its address points to valid memory anytime, including in rim_done()
                        // where any open file descriptors are at least checked, if not closed
                        // see comment in open-dir as to why we malloc here
                        oprintf("int *_rim_fileid_%ld = rim_malloc (sizeof(int));\n", file_count);
                        oprintf("*_rim_fileid_%ld = rim_fopen (%s, %s);\n", file_count, mtext, newt!=NULL?"O_RDWR | O_CREAT | O_TRUNC":"O_RDWR");
                        if (st == NULL) oprintf("if (*_rim_fileid_%ld == -1) rim_report_error (\"%s\", \"%s\", (rim_num)%ld, (rim_num)RIM_ERR_OPEN);\n", file_count,RIM_STMT_FAIL_CHECK, rim_valid_rim_name, lnum);
                        else oprintf("if (*_rim_fileid_%ld == -1) %s=RIM_ERR_OPEN; else %s=RIM_OKAY;\n", file_count, st, st);
                        // set FILE*, and also the index into gg[] (the memind member), so that when closing, it can 
                        // clear the gg[]->ptr and make rim_done() faster
                        // we set ->f to file descriptor even if NULL, because ->f is int* and should NEVER be -1, rather *(of->f) should be -1
                        oprintf ("(%s)->f = _rim_fileid_%ld; if (*_rim_fileid_%ld != -1) {rim_mem_set_status (rim_mem_get_id(_rim_fileid_%ld), RIM_MEM_FILE);}\n", fileid,  file_count, file_count, file_count);
                        file_count++;
                        continue;
                    }
                    //
                    // This is a method to have all optional params - check for both with no params (isLast==1) and with params
                    // (isLast==0). Based on return value we may look for optional params.
                    // Checking for non-mandatory (6th param is 1) MUST be done first.
                    //
                    else if ((newI=recog_statement(line, i, "end-write-string", &mtext, &msize, 1, &rim_is_inline)) != 0 ||
                        (newI1=recog_statement(line, i, "end-write-string", &mtext, &msize, 0, &rim_is_inline)) != 0  ||
                        (newI2=recog_statement(line, i, "))", &mtext, &msize, 1, &rim_is_inline)) != 0 ||
                        (newI3=recog_statement(line, i, "))", &mtext, &msize, 0, &rim_is_inline)) != 0)
                     {
                        RIM_GUARD
                        i = newI+newI1+newI2+newI3;
                        // end-write-string, being string writing within string writing, can NOT be within <<..>>
                        if (within_inline == 1)
                        {
                            rim_report_error( "Cannot use write-string as inline code");
                        }
                        // checking for notrim must be before rim_write_to_string (NULL) below because it's used in it
                        // find_keyword must be done before any carve_statement, even if it's used later
                        char *notrim = NULL;
                        if (newI1 != 0 || newI3 != 0)
                        {
                            notrim = find_keyword (mtext, RIM_KEYNOTRIM, 1);
                            carve_statement (&notrim, "end-write-string", RIM_KEYNOTRIM, 0, 0);
                        }
                        carve_stmt_obj (&mtext, false, false); 
                        if (notrim != NULL)
                        {
                            oprintf("rim_write_to_string_notrim();\n");
                        }
                        oprintf("rim_write_to_string (NULL);\n"); 
                        // this has string length, but no longer needed as we keep track internally
                        //oprintf("%s%srim_write_to_string_length();\n", bytes==NULL?"":bytes,bytes==NULL?"":"=");

                        gen_ctx->total_write_string--;

                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "write-string", &mtext, &msize, 0, &rim_is_inline)) != 0 ||
    -                    (newI1=recog_statement(line, i, "((", &mtext, &msize, 0, &rim_is_inline)) != 0)
                    {
                        RIM_GUARD
                        i = newI+newI1;
                        // write-string, being string writing within string writing, can NOT be within <<..>>
                        if (within_inline == 1)
                        {
                            rim_report_error( "Cannot use write-string as inline code");
                        }
                        carve_stmt_obj (&mtext, true, false); 

                        define_statement (&mtext, RIM_DEFSTRING, false); // exact length set in rim_write_to_string

                        oprintf("rim_write_to_string (&(%s));\n", mtext); 
                        gen_ctx->total_write_string++;


                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "read-line", &mtext, &msize, 0, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;

                        char *to = find_keyword (mtext, RIM_KEYTO, 1);
                        char *del = find_keyword (mtext, RIM_KEYDELIMITER, 1);
                        char *len = find_keyword (mtext, RIM_KEYSTATUS, 1);

                        carve_statement (&to, "read-line", RIM_KEYTO, 1, 1);
                        carve_statement (&del, "read-line", RIM_KEYDELIMITER, 0, 1);
                        carve_statement (&len, "read-line", RIM_KEYSTATUS, 0, 1);
                        carve_stmt_obj (&mtext, true, true); 

                        define_statement (&to, RIM_DEFSTRING, false);
                        define_statement (&len, RIM_DEFNUMBER, false);
                        if (del == NULL) {del = "\"\\n\"";}
                        make_mem (&del);
                        //
                        check_var (&mtext, RIM_DEFSTRING);
                        check_var (&del, RIM_DEFSTRING);


                        oprintf("{\n");
                        if (len != NULL) oprintf("rim_num *rim_rl_read_%ld = &(%s);\n", open_readline, len);
                        else 
                        {
                            oprintf("rim_num _rim_rl_read_%ld;\n", open_readline);
                            oprintf("rim_num *rim_rl_read_%ld = &(_rim_rl_read_%ld);\n", open_readline, open_readline);
                        }
                        oprintf("size_t rim_rl_local_len_%ld = 2;\n", open_readline);
                        oprintf("size_t rim_rl_len_%ld = 0;\n", open_readline);
                        oprintf("char *rim_rl_local_memptr_%ld = RIM_EMPTY_STRING;\n", open_readline);
                        oprintf("char **rim_to_local_memptr_%ld = &(%s);\n", open_readline, to);
                        //
                        oprintf("char *rim_rl_memptr_%ld = NULL;\n", open_readline);
                        oprintf("FILE *rim_rl_%ld = fopen (%s, \"r\");\n", open_readline, mtext); 
                        oprintf("if (rim_rl_%ld != NULL) {\n", open_readline);
                        oprintf("while (1) {\n");
                        oprintf("*rim_rl_read_%ld = (rim_num)getdelim (&rim_rl_memptr_%ld, &rim_rl_len_%ld, (int)(%s[0]), rim_rl_%ld);\n", open_readline, open_readline, open_readline, del, open_readline);
                        oprintf("if (*rim_rl_read_%ld == -1) { break;}\n",  open_readline);
                        // result of getdelim is the real length of string, minus 1. rim_rl_len_%ld is generally greater than that, but impossible to say what it is!
                        oprintf("rim_rl_local_memptr_%ld = rim_malloc (rim_rl_local_len_%ld=*rim_rl_read_%ld+1);\n", open_readline,open_readline,open_readline);
                        // Copy to buffer
                        oprintf ("memcpy(rim_rl_local_memptr_%ld, rim_rl_memptr_%ld, *rim_rl_read_%ld+1);\n", open_readline,open_readline,open_readline);
                        //
                        oprintf ("%s = rim_rl_local_memptr_%ld;\n", to, open_readline);
                        oprintf ("free(rim_rl_memptr_%ld); rim_rl_memptr_%ld = NULL; rim_rl_len_%ld=0;\n", open_readline,open_readline,open_readline); // open_readline was ++ after generating the code, so this is it

                        check_next_readline
                        continue;
                    }
                    else if ((newI=recog_statement(line, i, "end-read-line", &mtext, &msize, 1, &rim_is_inline)) != 0)  
                    {
                        RIM_GUARD
                        i = newI;
                        carve_stmt_obj (&mtext, false, false); 
                        // must assign "" in the loop, checking after loop for this pointer in case there was a break from the loop, but by setting to ""
                        // here, we won't free again
                        oprintf ("if (rim_rl_local_memptr_%ld!=RIM_EMPTY_STRING) {rim_free_int (rim_rl_local_memptr_%ld); rim_rl_local_memptr_%ld = RIM_EMPTY_STRING;}\n", open_readline-1, open_readline-1, open_readline-1);
                        oprintf("}\n");
                        oprintf ("if (!feof_unlocked(rim_rl_%ld)) {RIM_ERR;*rim_rl_read_%ld = (rim_num)RIM_ERR_READ;} else {*rim_rl_read_%ld = (rim_num)RIM_OKAY;}\n", open_readline-1, open_readline-1, open_readline-1); // this is if there was an error, not end of file
                        oprintf ("fclose (rim_rl_%ld);\n", open_readline-1); // open_readline was ++ after generating the code, so this is it
                        oprintf("} else { RIM_ERR;*rim_rl_read_%ld = (rim_num)RIM_ERR_OPEN; }\n", open_readline-1);
                        // set "to" to empty, otherwise it's pointing to a freed pointer and RimStone will try to free it again in optimized-memory mode, and crash
                        // so 'to' pointer is set to empty either way - we do equivalent of delete-string on it (which is free+set to empty)
                        // this is different because freeing setting to empty are not in the same place here
                        oprintf ("if (rim_rl_local_memptr_%ld!=RIM_EMPTY_STRING) rim_free_int (rim_rl_local_memptr_%ld);\n*rim_to_local_memptr_%ld = RIM_EMPTY_STRING;\n", open_readline-1, open_readline-1, open_readline-1);
                        oprintf ("if (rim_rl_memptr_%ld != NULL) free(rim_rl_memptr_%ld);\n",  open_readline-1, open_readline-1);
                        oprintf("}\n");
                        open_readline--;

                        continue;
                    }
                    // =recog_statement(line,i,"no-statement",...) - this is to cap the end_read_line in getvim script - DO NOT REMOVE
                    else if (print_mode != 1)
                    {
                        rim_report_error( "Unrecognized statement");
                        continue;
                    }
                    else
                    {
                        rim_report_error( "Unrecognized statement");
                        continue;
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
                // should not get here because we rim_trim line, and never reach 0 char
                // and should never go beyond the length of the line
                rim_report_error ("Parsing error, extra text found, [%s]", line);
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
        rim_num e;
        for (e = 0; e < emptyc_c; e++)
        {
            trimit (emptyc[e]);
            if (emptyc[e][0] != 0) rim_report_error ("Unrecognized text starting with [%s], please check syntax for statement", emptyc[e]);
        }

        //
        if (within_inline == 1)
        {
            rim_report_error( "Inline code was open with << but not closed with >>");
        }
        //See comment above for this. In addition, this is when >> is the very last on line, so the above does not
        //execute. We have to start this, even if it's gonna be empty, because we need to print this new line below
        //to finish the @ line. This is only if we're in @, i.e. printing.
        RIM_OPEN_PRINT

        if (print_mode == 1)
        {
            // Print new line at the end
            oprintf("\\n\", RIM_EMPTY_LONG_PLAIN_ZERO, false);\n");
        }
    

        if (feof_unlocked (f)) break; // something read in, then EOF
    }

    // check if variable unused
    rim_rewind_hash(rim_varh);
    rim_num ost;
    rim_var *ovar;
    while (1)
    {
        rim_next_hash(rim_varh,  (void**)&ovar, &ost, false);
        if (ost == RIM_ERR_EXIST) break;
        char *n = strchr (ovar->name, '+');
        if (n != NULL) 
        {
            n++;
            if (!ovar->used) 
            {
                // _rim_report_error (*note* underscore) will NOT exit, so this is to be used for errors where
                // v1 *can* continue. This is the first, as so far code has been written to exit right away.
                // Of course it's possible to continue, but the code must be built for it.
                // In _rim_report_error, rim_curr_errors is set to >0, and at the end of main() this is checked and 
                // main exits with code 1, which is needed for v1 calls in vmakefile, so v1 doesn't keep going and 
                // trying to compile C code which we know is bad.
                _rim_report_error ("Variable [%s] defined on line [%ld] is not used", n, ovar->lnum);
            }
        }
    }
    
    if (rim_check_lib)
    {
        // mark exact libs we need for linking, this must happen *BEFORE* any rim_report_error below, 
        // or otherwise rimlib won't be able to tell what needs to be done
        if (rim_mod_mariadb) mark_lib ("MARIADB");
        if (rim_mod_sqlite) mark_lib ("SQLITE");
        if (rim_mod_postgres) mark_lib ("POSTGRES");
        if (rim_mod_curl) mark_lib ("CURL");
        if (rim_mod_pcre2) mark_lib ("PCRE2");
        if (rim_mod_crypto) mark_lib ("CRYPTO");
        if (rim_mod_xml) mark_lib ("XML");
        if (rim_mod_service) mark_lib ("SERVICE");
        if (rim_mod_array) mark_lib ("ARRAY");
        if (rim_mod_tree) mark_lib ("TREE");
#define RIM_INST "Required library [%s] is not installed. You can install it from command line with:\n%s\nPlease install it and try again"
        // Examine libraries and offer exact advice on how to install
        if (rim_mod_mariadb)
        {
            char *has_mariadb = getenv ("RIM_MARIADB_EX");
            if (has_mariadb == NULL || has_mariadb[0] != '0')
            {
                char *pkg = getenv ("RIM_MARIADB_PACKAGE");
                char *ipkg = getenv ("RIM_MARIADB_INSTALL");
                rim_report_error (RIM_INST, pkg==NULL?"MariaDB":pkg, ipkg==NULL?"unknown":ipkg ); 
            }
        }
        if (rim_mod_sqlite)
        {
            char *has_sqlite = getenv ("RIM_SQLITE_EX");
            if (has_sqlite == NULL || has_sqlite[0] != '0')
            {
                char *pkg = getenv ("RIM_SQLITE_PACKAGE");
                char *ipkg = getenv ("RIM_SQLITE_INSTALL");
                rim_report_error (RIM_INST, pkg==NULL?"SQLite":pkg, ipkg==NULL?"unknown":ipkg ); 
            }
        }
        if (rim_mod_postgres)
        {
            char *has_postgres = getenv ("RIM_POSTGRES_EX");
            if (has_postgres == NULL || has_postgres[0] != '0')
            {
                char *pkg = getenv ("RIM_POSTGRES_PACKAGE");
                char *ipkg = getenv ("RIM_POSTGRES_INSTALL");
                rim_report_error (RIM_INST, pkg==NULL?"Postgres":pkg, ipkg==NULL?"unknown":ipkg ); 
            }
        }
        if (rim_mod_curl)
        {
            char *has_curl = getenv ("RIM_CURL_EX");
            if (has_curl == NULL || has_curl[0] != '0')
            {
                char *pkg = getenv ("RIM_CURL_PACKAGE");
                char *ipkg = getenv ("RIM_CURL_INSTALL");
                rim_report_error (RIM_INST, pkg==NULL?"curl":pkg, ipkg==NULL?"unknown":ipkg ); 
            }
        }
        if (rim_mod_pcre2) 
        {
            char *has_pcre2 = getenv ("RIM_PCRE2_EX");
            if (has_pcre2 == NULL || has_pcre2[0] != '0')
            {
                char *pkg = getenv ("RIM_PCRE2_PACKAGE");
                char *ipkg = getenv ("RIM_PCRE2_INSTALL");
                rim_report_error (RIM_INST, pkg==NULL?"PCRE2":pkg, ipkg==NULL?"unknown":ipkg ); 
            }
        }
        if (rim_mod_crypto)
        {
            char *has_crypto = getenv ("RIM_CRYPTO_EX");
            if (has_crypto == NULL || has_crypto[0] != '0')
            {
                char *pkg = getenv ("RIM_CRYPTO_PACKAGE");
                char *ipkg = getenv ("RIM_CRYPTO_INSTALL");
                rim_report_error (RIM_INST, pkg==NULL?"OpenSSL":pkg, ipkg==NULL?"unknown":ipkg ); 
            }
        }
        if (rim_mod_xml)
        {
            char *has_xml = getenv ("RIM_XML_EX");
            if (has_xml == NULL || has_xml[0] != '0')
            {
                char *pkg = getenv ("RIM_XML_PACKAGE");
                char *ipkg = getenv ("RIM_XML_INSTALL");
                rim_report_error (RIM_INST, pkg==NULL?"XML":pkg, ipkg==NULL?"unknown":ipkg ); 
            }
        }
        //
        char *rim_mod_fpe = getenv ("RIM_C_FPE"); // --fpe used
        char *has_fpe = getenv ("RIM_UBSAN_EX"); // has installed libubsan (1 means it's not)
        if ((rim_mod_fpe != NULL && rim_mod_fpe[0] == '1') && (has_fpe != NULL && has_fpe[0] == '1'))
        {
            char *pkg = getenv ("RIM_UBSAN_PACKAGE");
            char *ipkg = getenv ("RIM_UBSAN_INSTALL");
            rim_report_error (RIM_INST, pkg==NULL?"UBSan":pkg, ipkg==NULL?"unknown":ipkg ); 
        }
        //
    }


    if (do_once_open)
    {
        rim_report_error( "do-once statement found open on line [%ld], but it was never closed", do_once_line);
    }

    if (ccomm_open)
    {
        rim_report_error( "Opening C comment /* found on line [%ld], but it was never closed", ccomm_line);
    }

    if (gen_ctx->total_write_string != 0)
    {
        rim_report_error( "Imbalance in write-string/end-write-string statements, too many open or not closed");
    }

    if (open_loops != 0)
    {
        rim_report_error( "'start-loop' code block imbalance at line %ld, %ld %s open than closed", last_line_loop_closed, labs(open_loops), open_loops > 0 ? "more" : "less" );
    }
    if (open_queries != 0)
    {
        rim_report_error( "'query' code block imbalance at line %ld, %ld %s open than closed", last_line_query_closed, labs(open_queries), open_queries > 0 ? "more" : "less" );
    }
    if (open_readline != 0)
    {
        rim_report_error( "'read-line' code block imbalance at line check line %ld, %ld %s open than closed", last_line_readline_closed, labs(open_readline), open_readline > 0 ? "more" : "less");
    }
    if (open_ifs != 0)
    {
        rim_report_error( "'if-true' code block imbalance at line %ld, %ld %s open than closed", last_line_number_closed, labs(open_ifs), open_ifs > 0 ? "more" : "less" );
    }
    if (gen_ctx->curr_qry_ptr !=0)
    {
        rim_report_error( "Query imbalance (too many queries opened, too few closed)");
    }

    if (!done_handler) rim_report_error( "No begin-handler found");

}

#undef RIM_FILE
#undef RIM_LINE
#define RIM_FILE "[no file opened yet]"
#define RIM_LINE 0


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

    // create memory, no rim_malloc and such prior to here
    rim_pc = rim_alloc_config ();
    rim_memory_init();

    rim_gen_ctx *gen_ctx = (rim_gen_ctx*)rim_malloc(sizeof (rim_gen_ctx));
    init_rim_gen_ctx (gen_ctx);

    //
    // The following two options allow URL and WEB encoding of strings
    // from the command line. Typically used to construct URLs for command line
    // execution.
    //
    if (argc == 3 && !strcmp (argv[1], "-urlencode"))
    {
        // display URL encoded version of a string
        char *res = NULL;
        rim_encode (RIM_URL, argv[2], -1, &res, false);
        fprintf (stdout, "%s", res);
        exit(0);
    }
    if (argc == 3 && !strcmp (argv[1], "-webencode"))
    {
        // display URL encoded version of a string
        char *res = NULL;
        rim_encode (RIM_WEB, argv[2], -1, &res, false);
        fprintf (stdout, "%s", res);
        exit(0);
    }

    //
    // Get help from command line
    //
    if (argc == 1 || (argc == 2 && !strcmp (argv[1], "-version")))
    {
        fprintf (stdout, "%s %s on %s (%s)\n",argv[0], RIM_VERSION, RIM_OS_NAME, RIM_OS_VERSION);
        exit(0);
    }

    //
    // parse input parameters
    //
    rim_num i;
    char *_item = NULL;
    rim_num _main = 0;
    for (i = 1; i < argc; i ++)
    {
        if (!strcmp (argv[i], "-main"))
        {
            _main = 1;
            continue;
        }
        else if (!strcmp (argv[i], "-app-path"))
        {
            if (i + 1 >= argc)
            {
                rim_report_error ( "URL application prefix not specified after -app-path option");
                exit (1);
            }

            app_path = rim_strdup (argv[i+1]);
            i++; // skip app path
            continue;
        }
        else if (!strcmp (argv[i], "-client-timeout"))
        {
            if (i + 1 >= argc)
            {
                rim_report_error ( "Climate timeout not specified after -client-timeout option");
                exit (1);
            }
            rim_client_timeout = atol (argv[i+1]);
            i++; // skip db location now
            continue;
        }
        else if (!strcmp (argv[i], "-max-upload"))
        {
            if (i + 1 >= argc)
            {
                rim_report_error ( "Maximum upload length not specified after -max-upload option");
                exit (1);
            }
            rim_max_upload = atol (argv[i+1]);
            i++; // skip db location now
            continue;
        }
        else if (!strcmp (argv[i], "-name"))
        {
            if (i + 1 >= argc)
            {
                rim_report_error ( "Application name not specified after -name option");
                exit (1);
            }
            RIM_STRDUP (rim_app_name, argv[i+1]);
            if (rim_is_valid_param_name(rim_app_name, true, false) != 1)
            {
                rim_report_error( "Application name must have only alphanumeric characters, hyphens or underscores, found [%s]", rim_app_name);
            }
            i++; // skip db location now
            continue;
        }
        else if (!strcmp (argv[i], "-srcdir"))
        {
            if (i + 1 >= argc)
            {
                rim_report_error ( "Source directory not specified after -srcdir option");
                exit (1);
            }
            RIM_STRDUP (rim_valid_rim_dir, argv[i+1]);
            i++; 
            continue;
        }
        else if (!strcmp (argv[i], "-out"))
        {
            if (i + 1 >= argc)
            {
                rim_report_error ( "Output file not specified after -out option");
                exit (1);
            }
            RIM_STRDUP (out_file_name, argv[i+1]);
            i++; // skip db location now
            continue;
        }
        else if (!strcmp (argv[i], "-ignore-warn"))
        {
            rim_ignore_warn = true;
        }
        else if (!strcmp (argv[i], "-max-errors"))
        {
            if (i + 1 >= argc)
            {
                rim_report_error ( "Maximum number of errors not specified");
                exit (1);
            }
            rim_max_errors = atol (argv[i+1]);
            i++;
            continue;
        }
        else if (!strcmp (argv[i], "-x"))
        {
            no_rim_line = 1;
        }
        else if (!strcmp (argv[i], "-single-file"))
        {
            rim_single_file = true;
        }
        else if (!strcmp (argv[i], "-debug")) // internal for debugging only
        {
            rim_check_lib = false;
        }
        else if (!strcmp (argv[i], "-public"))
        {
            rim_public = true;
        }
        else
        {
            if (argv[i][0] == '-') 
            {
                rim_report_error ( "Invalid input parameters [%s]", argv[i]);
                exit (1);
            }
            _item = argv[i];
            if (src_file_name != NULL)
            {
                rim_report_error ( "Only one file name can be specified for processing, already specified [%s]", src_file_name);
                exit (1);
            }
            if (src_file_name != NULL && _main == 1)
            {
                rim_report_error ( "Cannot specify file name to process [%s], and the -main option to generate program main code. Use one or the other.", src_file_name);
                exit (1);
            }
            src_file_name = _item;
            char *src_bn = rim_basename (src_file_name); // this is alloc'd mem
            char rn[RIM_MAX_REQ_NAME_LEN];
            char *q = src_bn; // q may be NULL after decorating below
            rim_num len = strlen (src_bn);
            if (rim_decorate_path (rn, sizeof(rn), &q, len) != 1) rim_report_error( "request path [%s] is not valid", src_bn);

            char *dot = strstr (rn, ".rim");
            if (dot == NULL || dot[sizeof(".rim")-1] != 0) rim_report_error ("RimStone source file names must have a .rim extension, file [%s]", rn);
            *dot = 0;
            if (rim_is_valid_param_name (rn, false, false) != 1) rim_report_error (RIM_MSG_SRC_NAME, rn);
            rim_free (src_bn);
        }
    }

    if (rim_valid_rim_dir[0] == 0)
    {
        rim_report_error ( "Source directory must be specified (-srcdir)");
        exit (1);
    }
    if (rim_client_timeout <2 || rim_client_timeout > 360)
    {
        rim_report_error ( "Client timeout (-client-timeout) must be a number between 2 and 360");
        exit (1);
    }
    rim_num upper_limit = (rim_num)1024*1024*1024*1024;
    if (rim_max_upload <1024 || rim_max_upload > upper_limit)
    {
        rim_report_error ( "Max_upload_size (-max-upload) must be a number between 1024 and %ld", upper_limit);
        exit (1);
    }
    if (rim_app_name == NULL || rim_app_name[0] == 0) 
    {
        rim_report_error ( "Application name is missing (-name option)");
        exit (1);
    }
    // app path is by default /<app name>
    if (app_path[0] == 0) 
    {
        rim_num namel = (rim_num)strlen (rim_app_name);
        app_path = (char*)rim_malloc (namel + 2); // 1 for ending-null and 1 for /
        snprintf (app_path, namel+2, "/%.*s", (int)namel, rim_app_name);
    }
    rim_is_valid_app_path(app_path);

    rim_dir (RIM_DIR_USER, rim_user_dir, sizeof(rim_user_dir), rim_app_name, NULL);

    rim_bld_dir_len = rim_dir (RIM_DIR_BLD, rim_bld_dir, sizeof(rim_bld_dir), rim_app_name, NULL)+1;
    rim_dir (RIM_DIR_DB, rim_dbconf_dir, sizeof(rim_dbconf_dir), rim_app_name, NULL);


    //
    //
    // We must set various variables used in RIM shared library, for example, global connection data (description, transaction marker, 
    // connection marker). Encryption function is set too, as well as other variable.
    // This is the SAME code as below generated for application. This is because for RIM, we need it in order for db connection etc. to work.
    // And we need it for application too. Here in rimstone, we don't need it to be static since it's only one run of this process.
    // We can't really put this code in a common place because it spans two projects. Perhaps something could be done with generated code
    // but the effort doesn't seem worth it.
    //
    memset (&rim_dbs, 0, sizeof(rim_dbs));
    rim_get_config()->ctx.db=&rim_dbs;
    totconn = 0;
    rim_get_config()->app.dbconf_dir=rim_dbconf_dir;

    // For any .rim file, we need a list of connections available (in /.../bld/.dbvendors file). Only connections available
    // there can be used in .rim files
    char rim_db_file[300];
    snprintf (rim_db_file, sizeof(rim_db_file), "%s/.dbvendors", rim_bld_dir);

    FILE *f = fopen (rim_db_file, "r");
    // it is okay for f to be NULL - this is if app has no connections, i.e. it does not use databases
    if (f != NULL)
    {
        rim_dbs.conn = NULL; // rim_malloc in add_connection
        char line[200];
        rim_num dbtype;
        while (1)
        {
            if (fgets_unlocked (line, sizeof (line) - 1, f) != NULL)
            {
                rim_num len = strlen (line);
                rim_trim (line, &len, false);
                char *eq = strchr (line, '=');
                if (eq == NULL) 
                {
                    rim_report_error ( "dbvendors file format is incorrect");
                    exit (1);
                }
                *(eq++) = 0;
                if (!strcmp (eq, "mariadb")) {  dbtype = RIM_DB_MARIADB; }
                else if (!strcmp (eq, "sqlite")) {  dbtype = RIM_DB_SQLITE;}
                else if (!strcmp (eq, "postgres")) { dbtype = RIM_DB_POSTGRES;}
                else 
                {
                    rim_report_error ( "Unsupported database type [%s]", eq);
                    exit (1);
                }
                add_connection (line, dbtype); // add connection one by one, none is actually found since the list was empty
            } else break;
        }
        fclose (f);
    }


    if (out_file_name != NULL)
    {
        outf = rim_fopen1(out_file_name, "w");
        if (outf == NULL)
        {
            rim_report_error ( "Cannot open output file [%s] for writing, error [%s]", out_file_name, strerror(errno));
            exit (1);
        }
    }


    if (_main == 0 && src_file_name == NULL)
    {
        rim_report_error ( "Neither -main option or the file name to process is specified.");
        exit (1);
    }

    // create hash for variables
    // first check the size of the source file, make hash size comparable to code length
    rim_num sz;
    rim_file_stat (src_file_name, NULL, NULL, NULL, &sz, NULL);
    sz = sz/10;
    if (sz < 100) sz = 100;
    rim_create_hash (&rim_varh, sz, NULL, false);
    add_var ("true", RIM_DEFBOOL, NULL, false);
    add_var ("false", RIM_DEFBOOL, NULL, false);
    //
    // These are user-interfacing variables from rim.h and vcfgi.h
    // When they are added/removed there, they must be here too
    //
    add_var ("RIM_CLI_ERR_SOCK_READ", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_CLI_ERR_PROT_ERR", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_CLI_ERR_BAD_VER", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_CLI_ERR_SRV", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_CLI_ERR_UNK", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_CLI_ERR_OUT_MEM", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_CLI_ERR_RESOLVE_ADDR", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_CLI_ERR_PATH_TOO_LONG", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_CLI_ERR_CONNECT", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_CLI_ERR_TIMEOUT", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_CLI_ERR_SOCK_WRITE", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_CLI_ERR_INTERNAL", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_CLI_ERR_ENV_TOO_LONG", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_CLI_ERR_BAD_TIMEOUT", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_CLI_ERR_ENV_ODD", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_CLI_ERR_SOCKET", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_CLI_ERR_TOTAL", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_GET", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_PUT", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_POST", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_PATCH", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_DELETE", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_OTHER", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_FIFO", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_CHAR", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_BLOCK", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_LINK", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_SOCK", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_UNKNOWN", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_FILE", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_DIR", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_OKAY", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_ERR_OPEN", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_ERR_OPEN_TARGET", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_ERR_READ", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_ERR_WRITE", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_ERR_POSITION", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_ERR_TOO_MANY", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_ERR_DELETE", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_ERR_REFERENCE", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_ERR_FAILED", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_ERR_WEB_CALL", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_ERR_CREATE", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_ERR_EXIST", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_ERR_INVALID", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_ERR_RENAME", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_ERR_MEMORY", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_ERR_UTF", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_ERR_FORMAT", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_ERR_CLOSE", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_ERR_OVERFLOW", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_ERR_LENGTH", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_ERR_JSON", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_ERR_XML", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_ERR_UNKNOWN", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_DB_MARIADB", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_DB_POSTGRES", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_DB_SQLITE", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_INFO_EXIST", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_JSON_TYPE_STRING", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_JSON_TYPE_NUMBER", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_JSON_TYPE_REAL", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_JSON_TYPE_BOOL", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_JSON_TYPE_NULL", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_JSON_TYPE_NONE", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_SOURCE_LINE", RIM_DEFNUMBER, NULL, false);
    add_var ("RIM_EMPTY_STRING", RIM_DEFSTRING, NULL, false);
    add_var ("NULL", RIM_DEFSTRING, NULL, false);
    //
    // C errno constant
    //
    add_var ("E2BIG", RIM_DEFNUMBER, NULL, false);
    add_var ("EACCES", RIM_DEFNUMBER, NULL, false);
    add_var ("EADDRINUSE", RIM_DEFNUMBER, NULL, false);
    add_var ("EADDRNOTAVAIL", RIM_DEFNUMBER, NULL, false);
    add_var ("EAFNOSUPPORT", RIM_DEFNUMBER, NULL, false);
    add_var ("EAGAIN", RIM_DEFNUMBER, NULL, false);
    add_var ("EWOULDBLOCK)", RIM_DEFNUMBER, NULL, false);
    add_var ("EALREADY", RIM_DEFNUMBER, NULL, false);
    add_var ("EBADE", RIM_DEFNUMBER, NULL, false);
    add_var ("EBADF", RIM_DEFNUMBER, NULL, false);
    add_var ("EBADFD", RIM_DEFNUMBER, NULL, false);
    add_var ("EBADMSG", RIM_DEFNUMBER, NULL, false);
    add_var ("EBADR", RIM_DEFNUMBER, NULL, false);
    add_var ("EBADRQC", RIM_DEFNUMBER, NULL, false);
    add_var ("EBADSLT", RIM_DEFNUMBER, NULL, false);
    add_var ("EBUSY", RIM_DEFNUMBER, NULL, false);
    add_var ("ECANCELED", RIM_DEFNUMBER, NULL, false);
    add_var ("ECHILD", RIM_DEFNUMBER, NULL, false);
    add_var ("ECHRNG", RIM_DEFNUMBER, NULL, false);
    add_var ("ECOMM", RIM_DEFNUMBER, NULL, false);
    add_var ("ECONNABORTED", RIM_DEFNUMBER, NULL, false);
    add_var ("ECONNREFUSED", RIM_DEFNUMBER, NULL, false);
    add_var ("ECONNRESET", RIM_DEFNUMBER, NULL, false);
    add_var ("EDEADLK", RIM_DEFNUMBER, NULL, false);
    add_var ("EDEADLOCK", RIM_DEFNUMBER, NULL, false);
    add_var ("EDESTADDRREQ", RIM_DEFNUMBER, NULL, false);
    add_var ("EDOM", RIM_DEFNUMBER, NULL, false);
    add_var ("EDQUOT", RIM_DEFNUMBER, NULL, false);
    add_var ("EEXIST", RIM_DEFNUMBER, NULL, false);
    add_var ("EFAULT", RIM_DEFNUMBER, NULL, false);
    add_var ("EFBIG", RIM_DEFNUMBER, NULL, false);
    add_var ("EHOSTDOWN", RIM_DEFNUMBER, NULL, false);
    add_var ("EHOSTUNREACH", RIM_DEFNUMBER, NULL, false);
    add_var ("EHWPOISON", RIM_DEFNUMBER, NULL, false);
    add_var ("EIDRM", RIM_DEFNUMBER, NULL, false);
    add_var ("EILSEQ", RIM_DEFNUMBER, NULL, false);
    add_var ("EINPROGRESS", RIM_DEFNUMBER, NULL, false);
    add_var ("EINTR", RIM_DEFNUMBER, NULL, false);
    add_var ("EINVAL", RIM_DEFNUMBER, NULL, false);
    add_var ("EIO", RIM_DEFNUMBER, NULL, false);
    add_var ("EISCONN", RIM_DEFNUMBER, NULL, false);
    add_var ("EISDIR", RIM_DEFNUMBER, NULL, false);
    add_var ("EISNAM", RIM_DEFNUMBER, NULL, false);
    add_var ("EKEYEXPIRED", RIM_DEFNUMBER, NULL, false);
    add_var ("EKEYREJECTED", RIM_DEFNUMBER, NULL, false);
    add_var ("EKEYREVOKED", RIM_DEFNUMBER, NULL, false);
    add_var ("EL2HLT", RIM_DEFNUMBER, NULL, false);
    add_var ("EL2NSYNC", RIM_DEFNUMBER, NULL, false);
    add_var ("EL3HLT", RIM_DEFNUMBER, NULL, false);
    add_var ("EL3RST", RIM_DEFNUMBER, NULL, false);
    add_var ("ELIBACC", RIM_DEFNUMBER, NULL, false);
    add_var ("ELIBBAD", RIM_DEFNUMBER, NULL, false);
    add_var ("ELIBMAX", RIM_DEFNUMBER, NULL, false);
    add_var ("ELIBSCN", RIM_DEFNUMBER, NULL, false);
    add_var ("ELIBEXEC", RIM_DEFNUMBER, NULL, false);
    add_var ("ELNRNG", RIM_DEFNUMBER, NULL, false);
    add_var ("ELOOP", RIM_DEFNUMBER, NULL, false);
    add_var ("EMEDIUMTYPE", RIM_DEFNUMBER, NULL, false);
    add_var ("EMFILE", RIM_DEFNUMBER, NULL, false);
    add_var ("EMLINK", RIM_DEFNUMBER, NULL, false);
    add_var ("EMSGSIZE", RIM_DEFNUMBER, NULL, false);
    add_var ("EMULTIHOP", RIM_DEFNUMBER, NULL, false);
    add_var ("ENAMETOOLONG", RIM_DEFNUMBER, NULL, false);
    add_var ("ENETDOWN", RIM_DEFNUMBER, NULL, false);
    add_var ("ENETRESET", RIM_DEFNUMBER, NULL, false);
    add_var ("ENETUNREACH", RIM_DEFNUMBER, NULL, false);
    add_var ("ENFILE", RIM_DEFNUMBER, NULL, false);
    add_var ("ENOANO", RIM_DEFNUMBER, NULL, false);
    add_var ("ENOBUFS", RIM_DEFNUMBER, NULL, false);
    add_var ("ENODATA", RIM_DEFNUMBER, NULL, false);
    add_var ("ENODEV", RIM_DEFNUMBER, NULL, false);
    add_var ("ENOENT", RIM_DEFNUMBER, NULL, false);
    add_var ("ENOEXEC", RIM_DEFNUMBER, NULL, false);
    add_var ("Exec", RIM_DEFNUMBER, NULL, false);
    add_var ("ENOKEY", RIM_DEFNUMBER, NULL, false);
    add_var ("ENOLCK", RIM_DEFNUMBER, NULL, false);
    add_var ("ENOLINK", RIM_DEFNUMBER, NULL, false);
    add_var ("ENOMEDIUM", RIM_DEFNUMBER, NULL, false);
    add_var ("ENOMEM", RIM_DEFNUMBER, NULL, false);
    add_var ("ENOMSG", RIM_DEFNUMBER, NULL, false);
    add_var ("ENONET", RIM_DEFNUMBER, NULL, false);
    add_var ("ENOPKG", RIM_DEFNUMBER, NULL, false);
    add_var ("ENOPROTOOPT", RIM_DEFNUMBER, NULL, false);
    add_var ("ENOSPC", RIM_DEFNUMBER, NULL, false);
    add_var ("ENOSR", RIM_DEFNUMBER, NULL, false);
    add_var ("ENOSTR", RIM_DEFNUMBER, NULL, false);
    add_var ("ENOSYS", RIM_DEFNUMBER, NULL, false);
    add_var ("ENOTBLK", RIM_DEFNUMBER, NULL, false);
    add_var ("ENOTCONN", RIM_DEFNUMBER, NULL, false);
    add_var ("ENOTDIR", RIM_DEFNUMBER, NULL, false);
    add_var ("ENOTEMPTY", RIM_DEFNUMBER, NULL, false);
    add_var ("ENOTRECOVERABLE", RIM_DEFNUMBER, NULL, false);
    add_var ("ENOTSOCK", RIM_DEFNUMBER, NULL, false);
    add_var ("ENOTSUP", RIM_DEFNUMBER, NULL, false);
    add_var ("ENOTTY", RIM_DEFNUMBER, NULL, false);
    add_var ("ENOTUNIQ", RIM_DEFNUMBER, NULL, false);
    add_var ("ENXIO", RIM_DEFNUMBER, NULL, false);
    add_var ("EOPNOTSUPP", RIM_DEFNUMBER, NULL, false);
    add_var ("EOVERFLOW", RIM_DEFNUMBER, NULL, false);
    add_var ("EOWNERDEAD", RIM_DEFNUMBER, NULL, false);
    add_var ("EPERM", RIM_DEFNUMBER, NULL, false);
    add_var ("EPFNOSUPPORT", RIM_DEFNUMBER, NULL, false);
    add_var ("EPIPE", RIM_DEFNUMBER, NULL, false);
    add_var ("EPROTO", RIM_DEFNUMBER, NULL, false);
    add_var ("EPROTONOSUPPORT", RIM_DEFNUMBER, NULL, false);
    add_var ("EPROTOTYPE", RIM_DEFNUMBER, NULL, false);
    add_var ("ERANGE", RIM_DEFNUMBER, NULL, false);
    add_var ("EREMCHG", RIM_DEFNUMBER, NULL, false);
    add_var ("EREMOTE", RIM_DEFNUMBER, NULL, false);
    add_var ("EREMOTEIO", RIM_DEFNUMBER, NULL, false);
    add_var ("ERESTART", RIM_DEFNUMBER, NULL, false);
    add_var ("ERFKILL", RIM_DEFNUMBER, NULL, false);
    add_var ("EROFS", RIM_DEFNUMBER, NULL, false);
    add_var ("ESHUTDOWN", RIM_DEFNUMBER, NULL, false);
    add_var ("ESPIPE", RIM_DEFNUMBER, NULL, false);
    add_var ("ESOCKTNOSUPPORT", RIM_DEFNUMBER, NULL, false);
    add_var ("ESRCH", RIM_DEFNUMBER, NULL, false);
    add_var ("ESTALE", RIM_DEFNUMBER, NULL, false);
    add_var ("ESTRPIPE", RIM_DEFNUMBER, NULL, false);
    add_var ("ETIME", RIM_DEFNUMBER, NULL, false);
    add_var ("ETIMEDOUT", RIM_DEFNUMBER, NULL, false);
    add_var ("ETOOMANYREFS", RIM_DEFNUMBER, NULL, false);
    add_var ("ETXTBSY", RIM_DEFNUMBER, NULL, false);
    add_var ("EUCLEAN", RIM_DEFNUMBER, NULL, false);
    add_var ("EUNATCH", RIM_DEFNUMBER, NULL, false);
    add_var ("EUSERS", RIM_DEFNUMBER, NULL, false);
    add_var ("EWOULDBLOCK", RIM_DEFNUMBER, NULL, false);
    add_var ("EXDEV", RIM_DEFNUMBER, NULL, false);
    add_var ("EXFULL", RIM_DEFNUMBER, NULL, false);
    //
    // End of user interfacing variables
    //


    for (i = 0; i < RIM_MAX_CODE_LEVEL; i++) rim_resurr[i] = 0; // resurrection count for each code block level set to 0
                                                              // each time level N closes, this count increases
                                                              // so new code block of level N will not see old variables of
                                                              // the same level that are now out of scope 



    if (_main == 1)
    {

        // Generate C code
        // 
        // NOTE: NOTHING IN THIS MAIN CODE CAN HAVE RIM_MALLOC OR SUCH - that memory is release at the end of the request, thus anything that needs to be 
        // global must be C's malloc etc.
        //

        oprintf("#include \"rim.h\"\n");
        oprintf("#include \"rim_sparam.c\"\n"); // in main we include actual data definitions, and in rim_sparam.h just externs
        oprintf("extern rim_num rim_end_program;\n");

        // 
        // code can be generated for standalone (program) version that can be executed from command line
        // or a web-server plug-in for the web
        //

        //
        // ANY variables used in main MUST be outside, or longjmp will destroy them (they cannot be automatic within the function)
        //
        oprintf("volatile rim_num rim_in_request=0;\n");
        oprintf("volatile rim_num rim_done_setjmp=0;\n");
        oprintf("volatile rim_num rim_done_err_setjmp=0;\n");
        oprintf("volatile rim_num rim_in_fatal_exit=0;\n");
        oprintf("sigjmp_buf rim_jmp_buffer;\n");
        oprintf("sigjmp_buf rim_err_jmp_buffer;\n");
        oprintf("char *rim_app_name=\"%s\";\n", rim_app_name);
        // default application path is /<app name>
        oprintf("char * rim_app_path=\"%s\";\n", app_path); // app-path cannot have quotes
        oprintf("unsigned long rim_app_path_len=%lu;\n", strlen(app_path)); 
        oprintf("rim_num _rim_st=RIM_OKAY;\n"); // status used when not specified in statement
        oprintf("char *_rim_st_str;\n"); // status string used when not specified in statement
        oprintf("char rim_user_dir[RIM_MAX_OS_UDIR_LEN];\n"); // os user
        oprintf("bool _rim_st_bool = true;\n"); // status boolean used when not specified in statement
        oprintf ("rim_hash rim_dispatch;\n");
        oprintf ("rim_hash rim_paramhash;\n");
        oprintf ("bool rim_true = true;\n");
        oprintf ("bool rim_false = false;\n");
        oprintf ("bool rim_path_changed = false;\n");
        oprintf("rim_num rim_client_timeout=%ld;\n", rim_client_timeout);
        oprintf("rim_num rim_max_upload=%ld;\n", rim_max_upload);
        oprintf ("rim_num rim_sub_max_depth = 300;\n"); //Maximum stack depth
        oprintf("char *rim_inp_body;\n");
        oprintf("char *rim_inp_url;\n");
        // The following are static variables, i.e. those that need not be seen outside this module
        oprintf ("static rim_db_connections rim_dbs;\n");
        oprintf("static rim_num rim_done_init=0;\n");
        oprintf("rim_input_req rim_req;\n");
        oprintf("rim_config *rim_s_pc;\n");
    
        oprintf("int main (int argc, char *argv[])\n");
        oprintf("{\n");
        //
        // If a program is devel (not release), give ptracer privilege to anyone who otherwise has privileges on it
        // This isn't something that *must* succeed, we will not stop or issue message if it doesn't as it may cause malfunctioning otherwise
        // We would check if -1 if we ever  do that.
        oprintf("#ifdef RIM_DEVEL\n");
        oprintf ("prctl(PR_SET_PTRACER, PR_SET_PTRACER_ANY);\n");
        oprintf("#endif\n");
        // this is with -fpe option in gg
        oprintf("#ifdef RIM_C_FPE\n");
        oprintf ("feenableexcept(FE_INVALID | FE_DIVBYZERO | FE_OVERFLOW);\n");
        oprintf("#endif\n");
        //

        // rim_done_init and allocation of config structure are done once per program run
        // this also reads config file
        oprintf("_rim_st_str=RIM_EMPTY_STRING;\n"); 
        oprintf("rim_s_pc = rim_alloc_config ();\n");
        // // get current user name (OS) first, as that's used in init structures
        oprintf ("if (rim_dir (RIM_DIR_USER, rim_user_dir, sizeof(rim_user_dir), NULL, NULL) == -1) RIM_FATAL (\"Cannot obtain current Operating System user home directory\");\n");
        oprintf("rim_get_runtime_options();\n"); 

        setup_internal_hash(".reqlist","rim_dispatch", true); // setup request hash for near-instant request routing
        setup_internal_hash(".rim_sparam","rim_paramhash", false); // setup hash for list of set-params

        oprintf("rim_memory_init();\n");

        oprintf("umask (077);\n");
        // user-only permissions is used for new dir/files (i.e. mask of 077 which is 700)
        oprintf("rim_get_tz ();\n");
        // first memset base struct, then connections (not the other way around as conn would be wiped out)
        oprintf("   memset (&rim_dbs, 0, sizeof(rim_dbs));\n");
        if (totconn == 0) oprintf("   rim_dbs.conn = NULL;\n"); else oprintf("   rim_dbs.conn = calloc (%ld, sizeof(one_db));\n    if (rim_dbs.conn ==  NULL) RIM_FATAL (\"Cannot allocate database array memory\");\n", totconn);
        oprintf("   rim_s_pc->ctx.db = &rim_dbs;\n");
        oprintf("   rim_s_pc->ctx.db->ind_current_db=-1;\n"); // no db used yet
        // Generate name of database connections
        for (i = 0; i < totconn; i++)
        {
            oprintf ("   rim_s_pc->ctx.db->conn[%ld].db_name = \"%s\";\n", i, rim_get_config()->ctx.db->conn[i].db_name);
            oprintf ("   rim_s_pc->ctx.db->conn[%ld].db_type = %ld;\n", i, rim_get_config()->ctx.db->conn[i].db_type);
            oprintf ("   rim_s_pc->ctx.db->conn[%ld].dbc = NULL;\n", i); // not connected yet
            oprintf ("   rim_s_pc->ctx.db->conn[%ld].exit_on_error = 1;\n", i); // all db errors fatal
        }
        oprintf ("  rim_s_pc->ctx.tot_dbs = %ld;\n", totconn); // all db errors fatal
        oprintf("RIM_UNUSED (argc);\n");
        oprintf("RIM_UNUSED (argv);\n");
        //
        // Setup crash handler. Also print out all shared libraries loaded and their start/end addresses.
        //
        oprintf("if (!rim_done_init) rim_set_crash_handler (rim_s_pc->app.btrace);\n");
        //
        // Initialize curl
        //
        oprintf("#ifdef RIM_INC_CURL\nif (!rim_done_init) curl_global_init(CURL_GLOBAL_ALL);\n#endif\n");
        //
        // Initialize crypto
        //
        oprintf("#ifdef RIM_INC_CRYPTO\nif (!rim_done_init) rim_sec_load_algos();\n#endif\n");

        // Make static memory that by default accepts input requests, compile time deal
        // body of request
        // NOTE: gcc doesn't impose limit on size of string constant; below are 32K (body) and 4k (url)
        char *inp_body = make_empty_const (RIM_MAX_SIZE_OF_BODY);
        oprintf ("rim_inp_body = %s;\n", inp_body);
        // URL of request
        char *inp_url = make_empty_const (RIM_MAX_SIZE_OF_URL);
        oprintf ("rim_inp_url = %s;\n", inp_url);

        oprintf ("while(rim_SERVICE_Accept() >= 0) {\n");
        oprintf ("rim_in_request = 1;\n");

        // start request anew
        oprintf("rim_reset_config (rim_s_pc);\n");

        // need to startprint, it is automatic with first rim_printf

        oprintf("rim_init_input_req(&rim_req);\n");

        oprintf("rim_s_pc->ctx.req = &rim_req;\n"); 
        oprintf("rim_req.app = &(rim_s_pc->app);\n");



        // input parameter(s) from the SERVICE process manager (mrim),  but present for standalone too
        oprintf ("rim_s_pc->ctx.req->args.num_of_args = argc - 1;\nrim_s_pc->ctx.req->args.args = argv + 1;\n");

        // This is if there was an error caught in the request, go to handler after all requests
        oprintf("rim_num ret_val = sigsetjmp(rim_err_jmp_buffer, 1);\n");
        oprintf("if (ret_val != 0) goto end_point;\n");
        oprintf("rim_done_err_setjmp = 1;\n");

        // this must be AFTER the above sigsetjmp() in order for error handling to work in case chdir cannot be done
        // which would be for instance if home directory was delete between change directory and here
        oprintf ("if (rim_path_changed) {if (chdir (rim_s_pc->app.home_dir) != 0) rim_report_error(\"Cannot find RimStone home directory [%%s]\", rim_s_pc->app.home_dir);}\n");

        // this gets input from either web server or from the environment
        // We check if return is 1 or 0. If not 1, this means there was a problem, but it was
        // handled (such as Forbidden reply) (otherwise there would be an erorring out). If return
        // value is 0, we just go directly to rim_shut() to flush the response out.
        //
        oprintf("if (rim_get_input(&rim_req, NULL, NULL) == 1)\n");
        oprintf("{\n");
        // main function that handles everything - programmer must implement this or use generated
        oprintf("rim_dispatch_request();\n");

        // if there is a transaction that wasn't explicitly committed or rollbacked
        // rollback it and error out, as this is a programming bug.
        oprintf("rim_check_transaction (0);\n");
        oprintf("}\n");

        // we can jump here and rim_req will be valid because long jump to above (where we goto end_point) is valid only between above sigsetjmp and below
        // setting of rim_done_err_setjmp to 0; outside of this block of code we just _Exit()
        oprintf("end_point:\n");


        //
        // rim_shut MUST ALWAYS be called at the end - no request can bypass it
        //
        oprintf("rim_shut(&rim_req);\n");
        // now we now at this point rim_req is valid; if it was NULL, rim_shut would exit

        // by setting rim_done_err_setjmp to 0 here, we make sure that report-error will jump to 
        // end-point only between above setting rim_done_err_setjump to 1 
        // and here where we set it to 0. Because rim_req is valid in this case, we can do rim_shut after end_point:
        oprintf("rim_done_err_setjmp = 0;\n");

        // All data must be flushed at the end of the request, no none is outstanding somehow later
        oprintf("rim_SERVICE_Finish();\n"); 
        // if rim_end_program set to 1:
        // All db connections (rim_end_all_db) will be closed and program exists
        // For web program, we exit here when program catches SIGTERM, and the exit code is always 0.
        // For command line, we use exit-code value, if set.
        oprintf("if (rim_end_program == 1) { rim_end_all_db(); rim_exit ();}\n"); // if SIGHUP or SIGTERM happened
        oprintf ("rim_in_request = 0;\n");
        //
        // rim_memory_init() MUST be at the very end befoe the loop goes back to the next request
        // if it were anywhere before (it used to be before rim_SERVICE_Finish()), the memory gets released
        // and all references fail with random values or sigsegv (for instance req->ec would be like that in rim_SERVICE_Finish).
        // DO NOT PLACE ANY CODE BETWEEN rim_memory_init() and <curlybrace> that ends the request processing
        //
        oprintf("rim_memory_init();\n");
        oprintf("}\n");
        oprintf("return 0;\n");

        oprintf("}\n");

    }
    else
    {
        // get valid rimstone name, so for example subdir/file.rim, which is the valid file name from source directory, used in reporting
        rim_valid_rim_name = undecorate (src_file_name+RIM_BLD_DIR_OFFSET);
        rim_gen_c_code (gen_ctx, src_file_name);
    }

    oprintf("// END OF GENERATED CODE\n");
    oprintf (NULL); //  flush output
    if (outf != NULL) fclose (outf);

    // release of memory done automatically by the OS and likely faster, so no
    // rim_done();
    // check if errors raised, if so exit with code 1
    if (rim_curr_errors>0) return 1; else return 0;
}


