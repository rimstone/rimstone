// SPDX-License-Identifier: Apache-2.0
// Copyright 2018-2025 Gliim LLC. 
// Licensed under Apache License v2. See LICENSE file.
// On the web http://golf-lang.com/ - this file is part of Golf framework. 

// 
// Low-level implementation of network functions for FastCGI. 
//
//

#ifndef GG_FCGI_SOCK
#define GG_FCGI_SOCK

int gg_fcgi_connect(char *bindPath);
int gg_fcgi_read(int fd, char * buf, size_t len);
int gg_fcgi_write(int fd, char * buf, size_t len);
int gg_fcgi_close(int fd, int shut);
int gg_fcgi_closeRead(int fd);
int gg_fcgi_accept(int listen_sock, int set_timeout);


#endif 
