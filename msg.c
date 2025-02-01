// SPDX-License-Identifier: Apache-2.0
// Copyright 2018 Gliim LLC. 
// Licensed under Apache License v2. See LICENSE file.
// On the web http://golf-lang.com/ - this file is part of Golf framework.

// 
// Message-exchange module
//

#include "golf.h"
// add 38 for ' equals length', and null at the end
#define GG_MSG_ADD_LEN 30 
// buffer size to add when adding items, it increases up to 4K
#ifdef DEBUG
#define GG_MSG_BUFF_LEN 128
#else
#define GG_MSG_BUFF_LEN 1024
#endif
#define GG_MSG_SEP "="
#define GG_MSG_SEP_LEN sizeof(GG_MSG_SEP)-1
void gg_init_msg(gg_msg *t);
char *gg_parse_item (char *item, char **name, gg_num *name_len, char **data, gg_num *data_len, gg_num *st, gg_num len);

//
// Delete message, this is not currently used
//
void gg_del_msg(gg_msg *msg)
{
    GG_TRACE("");
    // we do not delete the 'from' data, i.e. msg->data that is still available, as it should be,
    // or otherwise all pieces of it (values) would become invalid now
    gg_init_msg(msg);
}

//
// get-message uses this to get message (msg) data which it returns
//
char *gg_get_msg (gg_msg *msg)
{
    GG_TRACE("");
    if (msg->data == GG_EMPTY_STRING) return GG_EMPTY_STRING;
    gg_num id = gg_mem_get_id(msg->data);
    gg_mem_set_len (id, msg->len+1);
    // 
    // Once data is taken out, we save the pointer to it, then we set ->data internally to empty string
    // After this, only a new message can be written. This is because once we return this message, if we 
    // continued to build, we would likely realloc, which would invalidate previous pointer, which can now fail
    // if used.
    //
    char *res = msg->data;
    gg_init_msg (msg);
    return res;
}

//
// Write key/value to message msg. The buffer starts with adding 128, then 256, etc  up to 4K, then if total allocd is
// greater than 8x increment, make increment 0.25 of total each time to avoid massive reallocs and copying.
//
void gg_write_msg(gg_msg *msg, char *key, char *value)
{
    GG_TRACE("");
    if (msg->mode != GG_MSG_WRITE) 
    {
        if (msg->mode == GG_MSG_NONE)
        {
            gg_init_msg(msg); msg->mode = GG_MSG_WRITE; // message destroyed and init if it was none or read
        } else gg_report_error ("Once message has been read, it cannot be written to");
    }
    gg_num truelen = gg_mem_get_len(gg_mem_get_id(msg->data));
    if (msg->len > truelen) gg_report_error ("Message is too short to write to, or was deleted"); // the message could have been obtained from get-message and delete-string'd in which
                                               // case truelen will be 0. Trying to access it beyond this would cause SIGSEGV.
    gg_num keyl = gg_mem_get_len (gg_mem_get_id(key));
    gg_num valuel = gg_mem_get_len (gg_mem_get_id(value));
    if (msg->curr == 0)
    {
        msg->addinc = GG_MSG_BUFF_LEN;
        msg->data = gg_malloc (msg->tot = keyl+valuel+GG_MSG_ADD_LEN + msg->addinc); 
    }
    else
    {
        //
        // Because we realloc here, it's a new pointer. Thus when get-message
        // via gg_get_msg(), we can't add any more data! Otherwise any
        // previous get-message would become invalid pointer!
        //
        if (msg->addinc <= 4096) msg->addinc *= 2;
        else if (msg->tot > 8*msg->addinc) msg->addinc = msg->tot/4; 
        if (msg->tot < msg->len+keyl+valuel+GG_MSG_ADD_LEN) msg->data = gg_realloc (gg_mem_get_id(msg->data), msg->tot = msg->len+keyl+valuel+GG_MSG_ADD_LEN + msg->addinc); 
    }
    memcpy (msg->data+msg->curr, key, keyl);
    memcpy (msg->data+msg->curr+keyl, GG_MSG_SEP, GG_MSG_SEP_LEN);
    uint64_t nv =  htobe64((uint64_t)valuel);
    // sizeof(valuel) is 8, which is the same as GG_ALIGN
    memcpy (msg->data+msg->curr+keyl+GG_MSG_SEP_LEN,(unsigned char*)&nv, GG_ALIGN); 
    memcpy (msg->data+msg->curr+keyl+GG_MSG_SEP_LEN+GG_ALIGN,value, valuel+1);
    memcpy (msg->data+msg->curr+keyl+GG_MSG_SEP_LEN+GG_ALIGN+valuel,"\n", 1);
    gg_num final = keyl+GG_MSG_SEP_LEN+GG_ALIGN+valuel+1;
    msg->data[msg->curr+ final] = 0; // finish msg as a string
    msg->len += final;
    msg->curr = msg->len;
    gg_mem_set_len (gg_mem_get_id(msg->data), msg->len+1); // set exact length of message
    return;
}

//
// Key 'comment' is special and ignored.
// Returns GG_ERR_LENGTH if trying to read beyond the end, or GG_ERR_FORMAT if message has bad format.
// key and value are alloced mem, though value is part of msg->data which is the reason there's no delete-message
// Otherwise GG_OKAY returned if got key/value
//
gg_num gg_read_msg(gg_msg *msg, char **key, char **value)
{
    GG_TRACE("");
    if (msg->mode != GG_MSG_READ) {msg->curr = 0; msg->mode = GG_MSG_READ;} // message rewound to beginning if mode none or write
    gg_num truelen = gg_mem_get_len(gg_mem_get_id(msg->data));
    if (msg->len > truelen) return GG_ERR_LENGTH; // the message could have been obtained from get-message and delete-string'd in which
                                               // case truelen will be 0. Trying to access it beyond this would cause SIGSEGV.
    if (msg->curr>=msg->len) return GG_ERR_LENGTH;
    gg_num key_len;
    gg_num value_len;
    gg_num st;
    char *ikey;
    char *ivalue;
    while (1)
    {
        char *next = gg_parse_item (msg->data+msg->curr, &ikey, &key_len, &ivalue, &value_len, &st, msg->len-msg->curr);
        if (st != GG_OKAY) return st;
        else
        {
            // because GG_MSG_SEP has GG_MSG_SEP_LEN bytes, and there's at least @ or one digit before value, we have plus + 1 
            if (key != NULL) *key = gg_strdupl(ikey,0,key_len);
            msg->curr = next - msg->data;
            if (!strcmp (ikey, "comment")) continue; // skip key 'comment'
            if (value != NULL) 
            {
                *value = gg_mem_add_const (ivalue-GG_ALIGN, value_len+1); 
                //*value = gg_strdupl (ivalue,0, value_len);            
            }
            return GG_OKAY;
        }
    }
}


//
// 'item' is parsed, it's of form key=length,value. 'key' cannot have '=' in it, otherwise can be any string that
// doesn't have null in it, including empty. 'length' is a positive number, which can be 0. 'value' is any data, including
// binary with nulls.
// Returns pointer to the next 'item', just passed the current one. 'key' is the key. key_len is its length. 'value' is
// the data and value_len is its length. st is GG_OKAY if okay, or GG_ERR_FORMAT if this simple format is not parsable.
// All pointers are returned as non-golf memory. That's because the programmer can skip some records and just use some, so
// the caller must turn key and equals into golf memory, when requested.
// len is the length of item
//
char *gg_parse_item (char *item, char **key, gg_num *key_len, char **value, gg_num *value_len, gg_num *st, gg_num len)
{
    GG_TRACE("");
    char *beg = item;
    beg[len] = 0; // cap possibilities for out of bounds, such as strstr below
    // get key
    char *eq = strstr (beg, GG_MSG_SEP); 
    if (eq == NULL) { if (st) *st = GG_ERR_FORMAT; return GG_EMPTY_STRING; }
    *key = beg;
    *eq = 0;
    *key_len = eq - *key;
    if (*key_len != 0)
    {
        while (isspace((*key)[0])) {(*key)++; (*key_len)--;}
        if (*key_len != 0)
        {
            gg_num p = -1;
            while (eq+p != *key && isspace(eq[p])) {eq[p--] = 0; (*key_len)--;}
        }
    }
    beg = eq + GG_MSG_SEP_LEN; 
    len -= beg - item;
    //
    char *lvalue;
    if (len < (gg_num)GG_ALIGN) { if (st) *st = GG_ERR_FORMAT; return GG_EMPTY_STRING; }
    // convert length from big endian to local host 64 bit
    uint64_t bl, vl;
    memcpy ((unsigned char*)&bl, beg, GG_ALIGN);
    vl = be64toh(bl);
    len -= GG_ALIGN;
    //
    *value_len = (gg_num)vl;
    if (len < *value_len) { if (st) *st = GG_ERR_LENGTH; return GG_EMPTY_STRING; }
    // get data of length 'len'
    lvalue = beg+GG_ALIGN; // value starts here
    //
    if (lvalue[*value_len] != '\n' && lvalue[*value_len] != 0) { if (st) *st = GG_ERR_LENGTH; return GG_EMPTY_STRING; }
    lvalue[*value_len] = 0;
    *value = lvalue;
    if (st) *st = GG_OKAY;
    return lvalue + *value_len + 1;
}

//
// Creates new message
//
gg_msg * gg_new_msg (char *from)
{
    GG_TRACE("");
    gg_msg *t = gg_malloc(sizeof(gg_msg));
    gg_init_msg(t);
    if (from != NULL)
    {
        // msg cannot be deleted now that it has const mems from it
        gg_num id = gg_mem_get_id (from);
        gg_num l = gg_mem_get_len (id);
        t->data = from;
        t->len = l;
        t->mode = GG_MSG_READ; // once we message 'from' only can read it.
    }
    return t;
}

//
// Init message
//
void gg_init_msg(gg_msg *t)
{
    GG_TRACE("");
    t->data = GG_EMPTY_STRING;
    t->len = 0;
    t->curr = 0;
    t->addinc = GG_MSG_BUFF_LEN;
    t->mode = GG_MSG_NONE;
}


