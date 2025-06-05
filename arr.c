// SPDX-License-Identifier: Apache-2.0
// Copyright 2018-2025 Gliim LLC.
// Licensed under Apache License v2. See LICENSE file.
// On the web http://golf-lang.com/ - this file is part of Golf framework.

//
// Array implementation
//


#include "golf.h"

// function prototypes
#ifdef GG_ARR_STRING
static void gg_init_array (gg_arraystring *arr, gg_num max_elem, bool process);
#endif
#ifdef GG_ARR_NUMBER
static void gg_init_array (gg_arraynumber *arr, gg_num max_elem, bool process);
#endif
#ifdef GG_ARR_BOOL
static void gg_init_array (gg_arraybool *arr, gg_num max_elem, bool process);
#endif


// 
// Init new array with 0 elements.
// max_elem is the absolute maximum size of an array (0 means default of 1,000,000)
// process is true if this is process-scoped array
//
#ifdef GG_ARR_STRING
static void gg_init_array (gg_arraystring *arr, gg_num max_elem, bool process)
#endif
#ifdef GG_ARR_NUMBER
static void gg_init_array (gg_arraynumber *arr, gg_num max_elem, bool process)
#endif
#ifdef GG_ARR_BOOL
static void gg_init_array (gg_arraybool *arr, gg_num max_elem, bool process)
#endif
{
    GG_TRACE("");
    // make top array
    if (max_elem < 0) gg_report_error ("Maximum number of elements in array cannot be negative");
    if (max_elem == 0) max_elem = 1000000;
    if (max_elem < GG_ARRAY_INC) max_elem = GG_ARRAY_INC;
    arr->max_elem = max_elem;
    arr->process = process;
    // make initial array
#ifdef GG_ARR_STRING
    arr->str = gg_calloc (arr->alloc_elem = GG_ARRAY_INC, sizeof(char*)); // all values NULL
#endif
#ifdef GG_ARR_NUMBER
    arr->num = gg_calloc (arr->alloc_elem = GG_ARRAY_INC, sizeof(gg_num)); // all values 0
#endif
#ifdef GG_ARR_BOOL
    arr->logic = gg_calloc (arr->alloc_elem = GG_ARRAY_INC, sizeof(bool)); // all values false
#endif
}

// 
// Make new array. Returns fresh array with 0 elements.
// max_elem is the absolute maximum size of an array (0 means default of 1,000,000)
// process is true if this is process-scoped array
//
#ifdef GG_ARR_STRING
gg_arraystring *gg_new_arraystring (gg_num max_elem, bool process)
#endif
#ifdef GG_ARR_NUMBER
gg_arraynumber *gg_new_arraynumber (gg_num max_elem, bool process)
#endif
#ifdef GG_ARR_BOOL
gg_arraybool *gg_new_arraybool (gg_num max_elem, bool process)
#endif
{
    GG_TRACE("");
#ifdef GG_ARR_STRING
    gg_arraystring *arr = gg_malloc (sizeof(gg_arraystring));
#endif
#ifdef GG_ARR_NUMBER
    gg_arraynumber *arr = gg_malloc (sizeof(gg_arraynumber));
#endif
#ifdef GG_ARR_BOOL
    gg_arraybool *arr = gg_malloc (sizeof(gg_arraybool));
#endif
    gg_init_array (arr, max_elem, process);
    return arr;
}


//
// Purges array arr. All elements are deleted including the values, and array initialized back to 256 elements.
// max_elem remains to whatever it was.
//
#ifdef GG_ARR_STRING
void gg_purge_arraystring (gg_arraystring *arr)
#endif
#ifdef GG_ARR_NUMBER
void gg_purge_arraynumber (gg_arraynumber *arr)
#endif
#ifdef GG_ARR_BOOL
void gg_purge_arraybool (gg_arraybool *arr)
#endif
{
    GG_TRACE("");
#ifdef GG_ARR_STRING
    gg_num i;
    for (i = 0; i < arr->alloc_elem; i++) 
    {
        if (arr->str[i] != NULL) gg_free (arr->str[i]);
    }
    gg_free (arr->str);
#endif
#ifdef GG_ARR_BOOL
    gg_free (arr->logic);
#endif
#ifdef GG_ARR_NUMBER
    gg_free (arr->num);
#endif
    gg_init_array (arr, arr->max_elem, arr->process);
    
}

//
// Write element to array arr. key is the number index, old_val is the old value that was there (if any).
// Will expand array as needed, starting from only 256, up to ->max_elem.
// The actual value is written in v1.c in code generated there to allow old value to be used in expression for new value
//
#ifdef GG_ARR_STRING
void gg_write_arraystring (gg_arraystring *arr, gg_num key, char **old_val)
#endif
#ifdef GG_ARR_NUMBER
void gg_write_arraynumber (gg_arraynumber *arr, gg_num key, gg_num *old_val)
#endif
#ifdef GG_ARR_BOOL
void gg_write_arraybool (gg_arraybool *arr, gg_num key, bool *old_val)
#endif
{
    GG_TRACE("");
    if (key < 0) gg_report_error ("Index to array is negative [%ld]", key);
    if (key >= arr->max_elem) gg_report_error ("Index to array is too large for array sizing [%ld], maximum allowed is set to [%ld]", key, arr->max_elem);
    if (key >= arr->alloc_elem)
    {
        gg_num old_alloc = arr->alloc_elem;
        // assure newly alloc_elem is greater than key
        if (key < 65536) arr->alloc_elem = key* 2; else arr->alloc_elem = key+ 65536;
        if (arr->alloc_elem > arr->max_elem) arr->alloc_elem = arr->max_elem;
#ifdef GG_ARR_STRING
        arr->str = gg_realloc (gg_mem_get_id(arr->str), arr->alloc_elem * sizeof (char*));
        memset (&(arr->str[old_alloc]),0, sizeof(char*)*(arr->alloc_elem - old_alloc));
#endif
#ifdef GG_ARR_NUMBER
        arr->num = gg_realloc (gg_mem_get_id(arr->num), arr->alloc_elem * sizeof (gg_num));
        memset (&(arr->num[old_alloc]),0, sizeof(gg_num)*(arr->alloc_elem - old_alloc));
#endif
#ifdef GG_ARR_BOOL
        arr->logic = gg_realloc (gg_mem_get_id(arr->logic), arr->alloc_elem * sizeof (bool));
        memset (&(arr->logic[old_alloc]),0, sizeof(bool)*(arr->alloc_elem - old_alloc));
#endif
    }

    if (old_val != NULL) 
    {
#ifdef GG_ARR_STRING
        *old_val = arr->str[key]==NULL?GG_EMPTY_STRING:arr->str[key]; 
#endif
#ifdef GG_ARR_NUMBER
        *old_val = arr->num[key]; 
#endif
#ifdef GG_ARR_BOOL
        *old_val = arr->logic[key]; 
#endif
    }
#ifdef GG_ARR_STRING
    else gg_free (arr->str[key]);
#endif

    // 
    // The actual setting of arr->xxx[key] is done in v1.c in order be able to use old_val
    // This increases performance as read/write can happen in one memory check
    //
}

//
// Read array from arr, using key number. Delete if del true, and status st is GG_OKAY if read, GG_ERR_EXIST if key is not existing
// Returns the value in the array. 
//
#ifdef GG_ARR_STRING
char *gg_read_arraystring (gg_arraystring *arr, gg_num key, bool del)
#endif
#ifdef GG_ARR_NUMBER
gg_num gg_read_arraynumber (gg_arraynumber *arr, gg_num key, bool del)
#endif
#ifdef GG_ARR_BOOL
bool gg_read_arraybool (gg_arraybool *arr, gg_num key, bool del)
#endif
{
    GG_TRACE("");
    if (key >= arr->max_elem || key < 0) gg_report_error ("Index [%ld] to array is negative or is beyond maximum allowable size", key);
    if (key >= arr->alloc_elem) {
        return 
#ifdef GG_ARR_STRING
        GG_EMPTY_STRING;
#endif
#ifdef GG_ARR_NUMBER
        0;
#endif
#ifdef GG_ARR_BOOL
        false;
#endif
    }
#ifdef GG_ARR_STRING
    char *rval = arr->str[key]==NULL?GG_EMPTY_STRING:arr->str[key];
#endif
#ifdef GG_ARR_NUMBER
    gg_num rval = arr->num[key];
#endif
#ifdef GG_ARR_BOOL
    bool rval = (bool)(arr->logic[key]);
#endif
    if (del)  
    {
#ifdef GG_ARR_STRING
        arr->str[key] = GG_EMPTY_STRING;
        gg_mem_delete_and_return (rval);
#endif
#ifdef GG_ARR_NUMBER
        arr->num[key] = 0;
#endif
#ifdef GG_ARR_BOOL
        arr->logic[key] = false;
#endif
    }
    return rval;
}



