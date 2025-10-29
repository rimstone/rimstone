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
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>

#define RIM_OKAY 0
// Error codes
#define RIM_ERR_OPEN -1
#define RIM_ERR_OPEN_TARGET -2
#define RIM_ERR_READ -3
#define RIM_ERR_WRITE -4
#define RIM_ERR_POSITION -5
#define RIM_ERR_TOO_MANY -6
#define RIM_ERR_DELETE -7
#define RIM_ERR_FAILED -8
#define RIM_ERR_WEB_CALL -9
#define RIM_ERR_CREATE -10 
#define RIM_ERR_EXIST -11
#define RIM_ERR_INVALID -12
#define RIM_ERR_RENAME -13
#define RIM_ERR_MEMORY -14
#define RIM_ERR_UTF -15
#define RIM_ERR_FORMAT -16
#define RIM_ERR_CLOSE -17
#define RIM_ERR_OVERFLOW -18
#define RIM_ERR_LENGTH -20
#define RIM_ERR_REFERENCE -21
#define RIM_ERR_JSON -22
#define RIM_ERR_XML -23
// new errors below
// the last error, it's NOT user interfacing
// Note that there's RIM_CLI_ERR_TOTAL error in cli.h under -255 (so -254, -253 etc.) and those can NOT
// be used here in rim.h (must not overlap), so currently its 16, so the last error number here is 
// actually -255+16 (and not -255), which is -239 currently
#define RIM_ERR_UNKNOWN -255

typedef int64_t rim_num;
typedef double rim_dbl; // future enhancement might include GMP (GNU Multiple Precision Arithmetic Lib) or MPFR


// memory alignment of 16 bytes for alloc'd data storage
#define RIM_ALIGN (sizeof(rim_head))
//
// Header prior to alloced memory. id is index into vm[]. This is the string in RimStone, and the actual string follows right after,
// allowing for good prefetching for a string.
//
typedef struct s_rim_head {
    rim_num id; 
    rim_num len;
} rim_head;


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
