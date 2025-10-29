// SPDX-License-Identifier: Apache-2.0
// Copyright 2018-2025 Gliim LLC. 
// Licensed under Apache License v2. See LICENSE file.
// On the web http://rimstone-lang.com/ - this file is part of RimStone framework.

// 
// Message-exchange module
//


// 
// When used by RimStone C API client, only the C's alloc functions are used
#ifdef RIM_MSG_CLI
#   include "rimcommon.h"
#   include "rimmsg.h"
static char *empty =  "";
#   undef rim_report_error
#   define  rim_report_error(...) fprintf(stderr, __VA_ARGS__),exit(1)
#   define  rim_malloc malloc
#   define  rim_realloc realloc
char *rim_strdupl (char *s, rim_num from, rim_num l);
RIM_ALWAYS_INLINE inline char *rim_strdupl (char *s, rim_num from, rim_num l)
{
   if (from < 0 || from > l)  rim_report_error ("Cannot copy from byte [%ld] when length is [%ld], or byte position is negative", from, l);
   char *n = (char*)malloc (l+1-from);
   if (n == NULL)
   {
       rim_report_error ("Out of memory for [%ld] bytes", l+1-from);
   }
   memcpy (n, s+from, l+1-from);
   return n;
}
#   define RIM_EMPTY_STRING empty

#else
#   include "rim.h"
#endif

// add 38 for ' equals length', and null at the end
#define RIM_MSG_ADD_LEN 30 
// buffer size to add when adding items, it increases up to 4K
#ifdef DEBUG
#define RIM_MSG_BUFF_LEN 128
#else
#define RIM_MSG_BUFF_LEN 1024
#endif
#define RIM_MSG_SEP "="
#define RIM_MSG_SEP_LEN sizeof(RIM_MSG_SEP)-1
static void rim_init_msg(rim_msg *t);
static char *rim_parse_item (char *item, char **name, rim_num *name_len, char **data, rim_num *data_len, rim_num *st, rim_num len);
void rim_del_msg(rim_msg *msg);

//
// Delete message, this is not currently used
//
void rim_del_msg(rim_msg *msg)
{
    // we do not delete the 'from' data, i.e. msg->data that is still available, as it should be,
    // or otherwise all pieces of it (values) would become invalid now
    rim_init_msg(msg);
}

//
// get-message uses this to get message (msg) data which it returns
//
rim_num rim_get_msg_len (rim_msg *msg)
{
    if (msg->data == RIM_EMPTY_STRING) return 0;
    return msg->len;
}

//
// get-message uses this to get message (msg) data which it returns
//
char *rim_get_msg (rim_msg *msg)
{
    if (msg->data == RIM_EMPTY_STRING) return RIM_EMPTY_STRING;
#ifndef RIM_MSG_CLI
    rim_mem_set_len (msg->data, msg->len+1);
#endif
    // 
    // Once data is taken out, we save the pointer to it, then we set ->data internally to empty string
    // After this, only a new message can be written. This is because once we return this message, if we 
    // continued to build, we would likely realloc, which would invalidate previous pointer, which can now fail
    // if used.
    //
    char *res = msg->data;
#ifndef RIM_MSG_CLI
    rim_init_msg (msg);
#endif
    return res;
}

//
// Write key/value to message msg. The buffer starts with adding 128, then 256, etc  up to 4K, then if total allocd is
// greater than 8x increment, make increment 0.25 of total each time to avoid massive reallocs and copying.
//
#ifndef RIM_MSG_CLI
void rim_write_msg(rim_msg *msg, char *key, char *value)
#else
void rim_write_msg(rim_msg *msg, char *key, char *value, rim_num valuel)
#endif
{
    if (msg->mode != RIM_MSG_WRITE) 
    {
        if (msg->mode == RIM_MSG_NONE)
        {
            rim_init_msg(msg); msg->mode = RIM_MSG_WRITE; // message destroyed and init if it was none or read
        } else rim_report_error ("Once message has been read, it cannot be written to");
    }
#ifndef RIM_MSG_CLI
    rim_num truelen = rim_mem_get_len(msg->data);
    if (msg->len > truelen) rim_report_error ("Message is too short to write to, or was deleted"); // the message could have been obtained from get-message and delete-string'd in which
                                               // case truelen will be 0. Trying to access it beyond this would cause SIGSEGV.
#endif
#ifndef RIM_MSG_CLI
    rim_num keyl = rim_mem_get_len (key);
    rim_num valuel = rim_mem_get_len (value);
#else
    rim_num keyl = strlen(key);
#endif
    // Check key is alphanum, underscore or hyphen
    rim_num i;
    for (i = 0; i < keyl; i++) 
    {
        if (!isalnum (key[i]) && !isspace(key[i]) && key[i] != '-' && key[i] != '_')
        {
            rim_report_error ("Invalid key in SEMI message [%s]", key);
        }
    }
    if (msg->curr == 0)
    {
        msg->addinc = RIM_MSG_BUFF_LEN;
        msg->data = rim_malloc (msg->tot = keyl+valuel+RIM_MSG_ADD_LEN + msg->addinc); 
    }
    else
    {
        //
        // Because we realloc here, it's a new pointer. Thus when get-message
        // via rim_get_msg(), we can't add any more data! Otherwise any
        // previous get-message would become invalid pointer!
        //
        if (msg->addinc <= 4096) msg->addinc *= 2;
        else if (msg->tot > 8*msg->addinc) msg->addinc = msg->tot/4; 
        if (msg->tot < msg->len+keyl+valuel+RIM_MSG_ADD_LEN) msg->data = rim_realloc (
#ifdef RIM_MSG_CLI
                msg->data,
#else
                rim_mem_get_id(msg->data),
#endif
                msg->tot = msg->len+keyl+valuel+RIM_MSG_ADD_LEN + msg->addinc); 
    }
    memcpy (msg->data+msg->curr, key, keyl);
    memcpy (msg->data+msg->curr+keyl, RIM_MSG_SEP, RIM_MSG_SEP_LEN);
    uint64_t nv[2];
    nv[0] =  htobe64((uint64_t)valuel);
    nv[1] =  0; // second byte currently zero
    // sizeof(valuel) is 8, which is the same as RIM_ALIGN/2
    memcpy (msg->data+msg->curr+keyl+RIM_MSG_SEP_LEN,(unsigned char*)&(nv[0]), RIM_ALIGN); 
    memcpy (msg->data+msg->curr+keyl+RIM_MSG_SEP_LEN+RIM_ALIGN,value, valuel+1);
    memcpy (msg->data+msg->curr+keyl+RIM_MSG_SEP_LEN+RIM_ALIGN+valuel,"\n", 1);
    rim_num final = keyl+RIM_MSG_SEP_LEN+RIM_ALIGN+valuel+1;
    msg->data[msg->curr+ final] = 0; // finish msg as a string
    msg->len += final;
    msg->curr = msg->len;
#ifndef RIM_MSG_CLI
    rim_mem_set_len (msg->data, msg->len+1); // set exact length of message
#endif
    return;
}

//
// Key 'comment' is special and ignored.
// Returns RIM_ERR_LENGTH if trying to read beyond the end, or RIM_ERR_FORMAT if message has bad format.
// key and value are alloced mem, though value is part of msg->data which is the reason there's no delete-message
// Otherwise RIM_OKAY returned if got key/value
//
#ifndef RIM_MSG_CLI
rim_num rim_read_msg(rim_msg *msg, char **key, char **value)
#else
rim_num rim_read_msg(rim_msg *msg, char **key, char **value, rim_num *valuel)
#endif
{
    if (msg->mode != RIM_MSG_READ) {msg->curr = 0; msg->mode = RIM_MSG_READ;} // message rewound to beginning if mode none or write
#ifndef RIM_MSG_CLI
    rim_num truelen = rim_mem_get_len(msg->data);
    if (msg->len > truelen) return RIM_ERR_LENGTH; // the message could have been obtained from get-message and delete-string'd in which
                                               // case truelen will be 0. Trying to access it beyond this would cause SIGSEGV.
#endif
    if (msg->curr>=msg->len) return RIM_ERR_LENGTH;
    rim_num key_len;
    rim_num value_len;
    rim_num st;
    char *ikey;
    char *ivalue;
    while (1)
    {
        char *next = rim_parse_item (msg->data+msg->curr, &ikey, &key_len, &ivalue, &value_len, &st, msg->len-msg->curr);
        if (st != RIM_OKAY) return st;
        else
        {
            // because RIM_MSG_SEP has RIM_MSG_SEP_LEN bytes, and there's at least @ or one digit before value, we have plus + 1 
            if (key != NULL) *key = rim_strdupl(ikey,0,key_len);
            msg->curr = next - msg->data;
            if (!strcmp (ikey, "comment")) continue; // skip key 'comment'
            if (value != NULL) 
            {
                // this can be added as constant, because the underlying string will be deleted
#ifndef RIM_MSG_CLI
                *value = rim_mem_add_const (ivalue-RIM_ALIGN, value_len+1); 
#else
                *value = ivalue; 
                *valuel = value_len;
#endif
            }
            return RIM_OKAY;
        }
    }
}


//
// 'item' is parsed, it's of form key=length,value. 'key' cannot have '=' in it, otherwise can be any string that
// doesn't have null in it, including empty. 'length' is a positive number, which can be 0. 'value' is any data, including
// binary with nulls.
// Returns pointer to the next 'item', just passed the current one. 'key' is the key. key_len is its length. 'value' is
// the data and value_len is its length. st is RIM_OKAY if okay, or RIM_ERR_FORMAT if this simple format is not parsable.
// All pointers are returned as non-rimstone memory. That's because the programmer can skip some records and just use some, so
// the caller must turn key and equals into rimstone memory, when requested.
// len is the length of item
//
static char *rim_parse_item (char *item, char **key, rim_num *key_len, char **value, rim_num *value_len, rim_num *st, rim_num len)
{
    char *beg = item;
    beg[len] = 0; // cap possibilities for out of bounds, such as strstr below
    // get key
    char *eq = strstr (beg, RIM_MSG_SEP); 
    if (eq == NULL) { if (st) *st = RIM_ERR_FORMAT; return RIM_EMPTY_STRING; }
    *key = beg;
    *eq = 0;
    *key_len = eq - *key;
    if (*key_len != 0)
    {
        while (isspace((*key)[0])) {(*key)++; (*key_len)--;}
        if (*key_len != 0)
        {
            rim_num p = -1;
            while (eq+p != *key && isspace(eq[p])) {eq[p--] = 0; (*key_len)--;}
        }
    }
    beg = eq + RIM_MSG_SEP_LEN; 
    len -= beg - item;
    //
    char *lvalue;
    if (len < (rim_num)RIM_ALIGN) { if (st) *st = RIM_ERR_FORMAT; return RIM_EMPTY_STRING; }
    // convert length from big endian to local host 64 bit
    uint64_t bl[2], vl;
    memcpy ((unsigned char*)&(bl[0]), beg, RIM_ALIGN);
    vl = be64toh(bl[0]);
    len -= RIM_ALIGN;
    //
    *value_len = (rim_num)vl;
    if (len < *value_len) { if (st) *st = RIM_ERR_LENGTH; return RIM_EMPTY_STRING; }
    // get data of length 'len'
    lvalue = beg+RIM_ALIGN; // value starts here
    //
    if (lvalue[*value_len] != '\n' && lvalue[*value_len] != 0) { if (st) *st = RIM_ERR_LENGTH; return RIM_EMPTY_STRING; }
    lvalue[*value_len] = 0;
    *value = lvalue;
    if (st) *st = RIM_OKAY;
    return lvalue + *value_len + 1;
}

//
// Creates new message. from can be NULL
//
#ifndef RIM_MSG_CLI
rim_msg * rim_new_msg (char *from)
#else
rim_msg * rim_new_msg (char *from, rim_num l)
#endif
{
    rim_msg *t = rim_malloc(sizeof(rim_msg));
    rim_init_msg(t);
    if (from != NULL)
    {
        // msg cannot be deleted now that it has const mems from it
#ifndef RIM_MSG_CLI
        rim_num l = rim_mem_get_len (from);
#endif
        t->data = from;
        t->len = l;
        t->mode = RIM_MSG_READ; // once we message 'from' only can read it.
    }
    return t;
}

//
// Init message
//
static void rim_init_msg(rim_msg *t)
{
    t->data = RIM_EMPTY_STRING;
    t->len = 0;
    t->curr = 0;
    t->addinc = RIM_MSG_BUFF_LEN;
    t->mode = RIM_MSG_NONE;
}


