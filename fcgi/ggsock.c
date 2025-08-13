// SPDX-License-Identifier: Apache-2.0
// Copyright 2018-2025 Gliim LLC. 
// Licensed under Apache License v2. See LICENSE file.
// On the web http://golf-lang.com/ - this file is part of Golf framework. 

// 
// Low-level implementation of network functions for FastCGI. 
//
//

#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>      
#include <math.h>
#include <memory.h>     
#include <netinet/tcp.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/un.h>
#include <signal.h>
#include <poll.h>
#include <netdb.h>
#include <sys/socket.h> /* for getpeername */
#include <unistd.h>
#include "fastcgi.h"
#include "ggsock.h"



//
// Read data from socket f into buf of length len
// Returns -1 on error, or number of bytes read
//
int gg_fcgi_read(int f, char * buf, size_t len)
{
    return read(f, buf, len);
}

//
// Write data buf of length len to socket f
// Returns -1 on error or number of bytes written
//
int gg_fcgi_write(int f, char * buf, size_t len)
{
    return write(f, buf, len);
}


//
// Close the socket, returns 0 if okay, -1 otherwise
//
int gg_fcgi_close(int fd, int shut)
{
    if (fd == -1) return 0; // if for some reason is bad
    if (shut)
    {
        if (shutdown(fd, SHUT_WR) == 0)
        {
            char trash[1024];

            while (read(fd, trash, sizeof(trash)) > 0);
        }
    }

    return close(fd);
}



//
// Accept a socket when connection comes in. Returns a socket or -1 if can't continue.
// sec_timeout is a number of seconds to wait for a read, to avoid deadbeats.
//
int gg_fcgi_accept(int listen_sock, int sec_timeout)
{
    int socket = -1;
    union {
        struct sockaddr_un un;
        struct sockaddr_in in;
    } sa;

    while (1)
    {
        socklen_t len = sizeof(sa);
        socket = accept(listen_sock, (struct sockaddr *)&sa, &len);
        if (socket < 0 && errno == EINTR) continue;
        else break;
        if (socket < 0)
        {
            if (errno == ETIMEDOUT || errno == ECONNRESET || errno == ENETUNREACH || errno == ECONNABORTED || errno == EHOSTUNREACH) continue; else return -1;
        } 
        else
        {
        }
    }
    // make sure the read (we do read then write for a request in Golf, and then close!)
    // does not take forever. This just ensures that if a number of seconds passes and we get
    // <0 from read(), then it's a deadbeat connection.
    if (sa.in.sin_family == AF_INET || sa.in.sin_family == AF_UNIX) 
    {
        if (sa.in.sin_family == AF_INET) // no Nagle can only be set on internet socket, not on Unix
                                     // Since Unix socket are managed by Linux kernel
        {
            // Golf generally deals with request-replies typically over Unix sockets
            // or high speed low-latency local networks for FCGI, so disabling Nagle
            // should provide performance boost.
            int val = 1;
            setsockopt(socket, IPPROTO_TCP, TCP_NODELAY, (char *)&val, sizeof(val));
        }  
        struct timeval remaining; 
        remaining.tv_sec = sec_timeout;
        remaining.tv_usec = 0;
        setsockopt (socket, SOL_SOCKET, SO_RCVTIMEO, &remaining, sizeof(remaining));
    }
    return socket;
}


