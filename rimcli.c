// SPDX-License-Identifier: Apache-2.0
// Copyright 2018-2025 Gliim LLC. 
// Licensed under Apache License v2. See LICENSE file.
// On the web http://rimstone-lang.com/ - this file is part of RimStone framework. 

// 
// FastCGI client. Takes either IP:port or path to Unix socket and the rest it takes
// from standard HTTP env variables; then contacts the server and gets the response.
// Uses RimStone FastCGI client library.
//

#include "rcli.h"
#include <unistd.h>

int main (int argc, char **argv)
{
    // Request type rim_cli - create a request variable and zero it out
    rim_cli req;
    memset ((char*)&req, 0, sizeof(rim_cli));

    // account for empty '' argument
    if (argc != 2 || argv[1][0] == 0) {fprintf(stderr, "Server parameter is missing.\n"); exit (1);}

    // get server/Unix socket
    req.server = argv[1];

    // get content type
    char *type = getenv("CONTENT_TYPE"); 
    if (type != NULL) req.content_type = type;

    char *silent = getenv("RIM_SILENT_HEADER");  // silent header is RimStone Specific

    // env variables, currently only RIM_SILENT_HEADER: in general taking actual client env variables is questionable
    // because there's lots of them, and it's unclear if it's safe to push all through
    char *env[3]; // must be outside of the following if(), or would go out of scope
    if (silent != NULL)
    {
        env[0] = "RIM_SILENT_HEADER";
        env[1] = silent;
        env[2] = NULL;
        req.env = env;
    }

    // Content length
    char *clen = getenv("CONTENT_LENGTH"); 
    if (clen != NULL) req.content_len = atoi (clen);

    // Get content body from stdin if length set
    if (req.content_len != 0)
    {
        req.req_body = malloc (req.content_len +1 ); // account for +1 null char
        if (req.req_body == NULL) {fprintf (stderr, "Cannot allocate [%d] bytes for content.\n", req.content_len); exit (1);}
        int bread = read(STDIN_FILENO, req.req_body, req.content_len);
        // make sure all data is present and accounted for
        if (bread != req.content_len) {fprintf (stderr, "Cannot read [%d] bytes for content.\n", req.content_len); exit (1);}
        req.req_body[req.content_len] =0; // set null byte though not really used
    }

    // request method, GET is default
    char *method = getenv("REQUEST_METHOD"); 
    if (method != NULL) req.req_method = method; else req.req_method = "GET"; // GET HTTP method

    // HTTP script name
    char *script = getenv("SCRIPT_NAME"); 
    if (script != NULL) req.app_path = script;

    // HTTP path info
    char *pinfo = getenv("PATH_INFO"); 
    if (pinfo != NULL) req.req = pinfo;

    // HTTP query string
    char *query = getenv("QUERY_STRING"); 
    if (query != NULL) req.url_params = query;

    // Make a request
    int res = rim_cli_request (&req);

    // Output exactly the bytes sent back, both stderr and stdout
    fwrite_unlocked(rim_cli_error(&req), req.error_len, 1, stderr); 
    // Print stdout as well, since service can produce both!
    fwrite_unlocked(rim_cli_data(&req), req.data_len, 1, stdout); 

    // Free up resources so there are no memory leaks;  though we're exiting in this case, so it's faster for the OS to perform this
    //rim_cli_delete(&req);
    exit(res); // okay status
}
