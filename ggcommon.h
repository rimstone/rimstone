// SPDX-License-Identifier: Apache-2.0
// Copyright 2018-2025 Gliim LLC. 
// Licensed under Apache License v2. See LICENSE file.
// On the web http://golf-lang.com/ - this file is part of Golf framework.

// 
// Include file common for GOLF C Client API and the Server/command-line
//
#ifndef _GG_GGCOMMON
#define _GG_GGCOMMON

#include <sys/types.h>
#include <pwd.h>
#include <stdbool.h>
#include <stddef.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>

typedef int64_t gg_num;

#ifdef GG_GOLFMEM
void *gg_malloc(size_t size);
#endif

// Max length of Linux user dir allowed, plus any additions, app subdirs etc. Should be generous.
#define GG_MAX_OS_UDIR_LEN 192

size_t gg_dir (int type, char *dir, size_t dir_size, char *app, char *user);

#define GG_DIR_USER 0
#define GG_DIR_APPS 1
#define GG_DIR_APP 2
#define GG_DIR_TRACE 3
#define GG_DIR_DB 4
#define GG_DIR_SOCK 5
#define GG_DIR_FILE 6
#define GG_DIR_BLD 7
#define GG_DIR_MGRG 8
#define GG_DIR_APPNAME 9
#define GG_DIR_SOCKFILE 10
#define GG_DIR_MEM 11
#define GG_DIR_LOCK 12
#define GG_DIR_MGRG1 13
#define GG_DIR_TMP 14
#define GG_DIR_TRACENAME 15


#endif
