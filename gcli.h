// SPDX-License-Identifier: Apache-2.0
// Copyright 2018 Gliim LLC. 
// Licensed under Apache License v2. See LICENSE file.
// On the web http://golf-lang.com/ - this file is part of Golf framework.

// 
// Include file for GOLF FastCGI client
//

#ifndef _GG_SERVICE_INC

#define _GG_SERVICE_INC

// Needed for memmem
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

// Include basics
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


// Version+Release. We use major plus minor plus release, as in 1.3.34,2.1.11,3.7.41... 
#define GG_VERSION "353"

// Client error codes
#define GG_OKAY 0 // success
//
//
// User-interfacing constants, to be added to variable hash for golf compiler
//
//
#define GG_CLI_MAX_ERRS -255 // maximum number of error messages, see golf.h's GG_ERR_UNKNOWN - must match here!
//
#define GG_CLI_ERR_SOCK_READ GG_CLI_MAX_ERRS+1 // socket read
#define GG_CLI_ERR_PROT_ERR GG_CLI_MAX_ERRS+2  // protocol error
#define GG_CLI_ERR_BAD_VER GG_CLI_MAX_ERRS+3 // bad fcgi version
#define GG_CLI_ERR_SRV GG_CLI_MAX_ERRS+4 // server cannot complete request
#define GG_CLI_ERR_UNK GG_CLI_MAX_ERRS+5 // server says unknown type
#define GG_CLI_ERR_OUT_MEM GG_CLI_MAX_ERRS+6 // out of memory
#define GG_CLI_ERR_RESOLVE_ADDR GG_CLI_MAX_ERRS+7 //ip cannot be resolved
#define GG_CLI_ERR_PATH_TOO_LONG GG_CLI_MAX_ERRS+8 //sock path too long
#define GG_CLI_ERR_CONNECT GG_CLI_MAX_ERRS+9 //cannot connect
#define GG_CLI_ERR_TIMEOUT GG_CLI_MAX_ERRS+10 // timed out
#define GG_CLI_ERR_SOCK_WRITE GG_CLI_MAX_ERRS+11 // error writing to server
#define GG_CLI_ERR_INTERNAL GG_CLI_MAX_ERRS+12 // not used yet
#define GG_CLI_ERR_ENV_TOO_LONG GG_CLI_MAX_ERRS+13 // env too long
#define GG_CLI_ERR_BAD_TIMEOUT GG_CLI_MAX_ERRS+14 // invalid timeout
#define GG_CLI_ERR_ENV_ODD GG_CLI_MAX_ERRS+15 // number of env vars must be even
#define GG_CLI_ERR_SOCKET GG_CLI_MAX_ERRS+16 //cannot create socket
#define GG_CLI_ERR_TOTAL 16 //total number of message (the last is GG_CLI_MAX_ERRS+16, so 16)
//
//
// End of user interfacing constants
//
//

// Each protocol must have its own set of types and functions
// here it's gg_cli*. They would be abstracted in Golf as server-*
// but types/functions must be separated. server-* would have 
// "type" clause to differentiate and C declaration would be gg_cli,
// gg_xy etc.
typedef struct gg_cli gg_cli;

// Hooks
typedef void (*gg_cli_out_hook)(char *recv, int recv_len, gg_cli *req);
typedef void (*gg_cli_err_hook)(char *err, int err_len, gg_cli *req);
typedef void (*gg_cli_done_hook)(char *recv, int recv_len, char *err, int err_len, gg_cli *req);

// FastCGI request
typedef struct gg_cli {
    char *server; // the IP:port/socket_path to server
    char *req_method; // request method
    char *app_path; // application path
    char *req; // request name
    char *url_params; // URL payload (path+query string)
    char *content_type; // content type
    int content_len; // content len
    char *req_body; // request body (i.e. content)
    char **env; // environment to pass into request on server side
    int timeout; // timeout for request
    int req_status; // status of request from server
    int data_len; // length of response from server
    int error_len; // length of error from server
    char *other; // actual response from server, other kind (not stdout or stderr)
    int other_len; // length of response from server for other kind
    char *errm; // error message when gg_cli_request returns other than GG_OKAY
    gg_cli_out_hook out_hook; // get output data as soon as it arrives
    gg_cli_err_hook err_hook; // get error data as soon as it arrives
    gg_cli_done_hook done_hook; // get all data when request is complete
    int thread_id; // thread ID when executing in a multithreaded fashion
    volatile char done; // true if request completed. This is in addition to req_status (as some servers
               // may not set it) and read_status (as a request may fail at writing prior to reading)
               // false if request did not complete. "Complete" means done, no matter how, even with failure.
               // It's volatile because it is generally meant to be set by a running thread, but read by a 
               // controlling thread
    int return_code; // the return code from gg_cli_request(), either GG_OKAY or error code, usefule
                     // when multithreaded, in single threaded not really.
    struct s_internal  // these are internal members, do NOT use them directly!!
    {
        char invalid_thread; // 1 if thread that was supposed to be created to process this request is invalid
                         // eg. pthread_create() failed. 0 if okay.
        char *data; // actual response from server
        char *error; // error message from server
        int read_status; // status of reading from server
        bool server_alloc; // true if server name alloc'd (golf server only)
        bool path_alloc; // true if app_path and url_params alloc'd (golf server only)
    } internal;
} gg_cli;


// API
int gg_cli_request (gg_cli *fc_in);
char *gg_cli_error (gg_cli *callin);
char *gg_cli_data (gg_cli *callin);
void gg_cli_delete (gg_cli *callin);

#endif
