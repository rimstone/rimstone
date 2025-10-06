// SPDX-License-Identifier: Apache-2.0
// Copyright 2018-2025 Gliim LLC. 
// Licensed under Apache License v2. See LICENSE file.
// On the web http://rimstone-lang.com/ - this file is part of RimStone framework. 

// 
// Low-level implementation of network functions for FastCGI. 
//
//

#ifndef RIM_FCGI_SOCK
#define RIM_FCGI_SOCK

int rim_fcgi_connect(char *bindPath);
int rim_fcgi_read(int fd, char * buf, size_t len);
int rim_fcgi_write(int fd, char * buf, size_t len);
int rim_fcgi_close(int fd, int shut);
int rim_fcgi_closeRead(int fd);
int rim_fcgi_accept(int listen_sock, int set_timeout);


#endif 
