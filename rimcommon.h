// SPDX-License-Identifier: Apache-2.0
// Copyright 2018-2025 Gliim LLC. 
// Licensed under Apache License v2. See LICENSE file.
// On the web http://rimstone-lang.com/ - this file is part of RimStone framework.

// 
// Include file common for RIM C Client API and the Server/command-line
//
#ifndef _RIM_COMMON
#define _RIM_COMMON

#include <sys/types.h>
#include <pwd.h>
#include <stdbool.h>
#include <stddef.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>

typedef int64_t rim_num;
typedef double rim_dbl; // future enhancement might include GMP (GNU Multiple Precision Arithmetic Lib) or MPFR

#ifdef RIM_RIMMEM
void *rim_malloc(size_t size);
#endif

// Max length of Linux user dir allowed, plus any additions, app subdirs etc. Should be generous.
#define RIM_MAX_OS_UDIR_LEN 192

size_t rim_dir (int type, char *dir, size_t dir_size, char *app, char *user);

#define RIM_DIR_USER 0
#define RIM_DIR_APPS 1
#define RIM_DIR_APP 2
#define RIM_DIR_TRACE 3
#define RIM_DIR_DB 4
#define RIM_DIR_SOCK 5
#define RIM_DIR_FILE 6
#define RIM_DIR_BLD 7
#define RIM_DIR_MGRG 8
#define RIM_DIR_APPNAME 9
#define RIM_DIR_SOCKFILE 10
#define RIM_DIR_MEM 11
#define RIM_DIR_LOCK 12
#define RIM_DIR_ART 13
#define RIM_DIR_TMP 14
#define RIM_DIR_TRACENAME 15


#endif
