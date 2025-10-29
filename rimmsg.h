// SPDX-License-Identifier: Apache-2.0
// Copyright 2018-2025 Gliim LLC. 
// Licensed under Apache License v2. See LICENSE file.
// On the web http://rimstone-lang.com/ - this file is part of RimStone framework.

// 
// Include file for RIM MSG (SEMI) message system
//

#ifndef _RIM_MSG_INC

#define _RIM_MSG_INC

//
// Message structure (generic)
//
typedef struct rim_msg_s
{
    char *data; // message itself
    rim_num len; // current message length
    rim_num tot; // total allocated, which can be far greater than current message length to avoid massive realloc-s
    rim_num addinc; // add memory increment, starts with 128 and goes up to 4K
    rim_num curr; // where in parsing, or adding to, is message currently
    char mode; // RIM_MSG_NONE, RIM_MSG_READ, RIM_MSG_WRITE, default MSG_NONE, must delete msg to change from read to write and vice versa
} rim_msg;

// MSG (SEMI) API
#ifndef RIM_MSG_CLI
rim_msg * rim_new_msg (char *from);
#else
rim_msg * rim_new_msg (char *from, rim_num l);
#endif
rim_num rim_get_msg_len (rim_msg *msg);
#ifndef RIM_MSG_CLI
rim_num rim_read_msg(rim_msg *msg, char **key, char **val);
#else
rim_num rim_read_msg(rim_msg *msg, char **key, char **val, rim_num *val_len);
#endif
#ifndef RIM_MSG_CLI
void rim_write_msg(rim_msg *msg, char *key, char *val);
#else
void rim_write_msg(rim_msg *msg, char *key, char *val, rim_num val_len);
#endif
char *rim_get_msg(rim_msg *msg);


//mode of message usage
#define RIM_MSG_NONE 0
#define RIM_MSG_READ 1
#define RIM_MSG_WRITE 2

#endif
