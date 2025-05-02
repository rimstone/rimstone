// SPDX-License-Identifier: Apache-2.0
// Copyright 2018-2025 Gliim LLC.
// Licensed under Apache License v2. See LICENSE file.
// On the web http://golf-lang.com/ - this file is part of Golf framework.

//
// Array implementation
//


#include "golf.h"

// function prototypes
void gg_init_array (gg_array *arr, gg_num max_elem, bool process);

// 
// Init new array with 0 elements.
// max_elem is the absolute maximum size of an array (0 means default of 1,000,000)
// process is true if this is process-scoped array
//
void gg_init_array (gg_array *arr, gg_num max_elem, bool process)
{
    GG_TRACE("");
    // make top array
    if (max_elem < 0) gg_report_error ("Maximum number of elements in array cannot be negative");
    if (max_elem == 0) max_elem = 1000000;
    if (max_elem < GG_ARRAY_INC) max_elem = GG_ARRAY_INC;
    arr->max_elem = max_elem;
    arr->process = process;
    // make initial array
    arr->arr = gg_calloc (arr->alloc_elem = GG_ARRAY_INC, sizeof(char*)); // all values NULL
}

// 
// Make new array. Returns fresh array with 0 elements.
// max_elem is the absolute maximum size of an array (0 means default of 1,000,000)
// process is true if this is process-scoped array
//
gg_array *gg_new_array (gg_num max_elem, bool process)
{
    GG_TRACE("");
    gg_array *arr = gg_malloc (sizeof(gg_array));
    gg_init_array (arr, max_elem, process);
    return arr;
}


//
// Purges array arr. All elements are deleted including the values, and array initialized back to 256 elements.
// max_elem remains to whatever it was.
//
void gg_purge_array (gg_array *arr)
{
    GG_TRACE("");
    gg_num i;
    for (i = 0; i < arr->alloc_elem; i++) 
    {
        if (arr->arr[i] != NULL) gg_free (arr->arr[i]);
    }
    gg_free (arr->arr);
    gg_init_array (arr, arr->max_elem, arr->process);
    
}

//
// Write element to array arr. key is the number index, val is value, old_val is the old value that was there (if any).
// st is status: GG_OKAY if written, GG_INFO_EXISTS if written but there was old value which is now in old_val.
// Will expand array as needed, starting from only 256, up to ->max_elem.
//
void gg_write_array (gg_array *arr, gg_num key, char *val, char **old_val, gg_num *st)
{
    GG_TRACE("");
    if (key < 0) gg_report_error ("Index to array is negative [%ld]", key);
    if (key >= arr->max_elem) gg_report_error ("Index to array is too large for array sizing [%ld], maximum allowed is set to [%ld]", key, arr->max_elem);
    if (key >= arr->alloc_elem)
    {
        gg_num old_alloc = arr->alloc_elem;
        if (arr->alloc_elem < 65536) arr->alloc_elem *= 2; else arr->alloc_elem += 65536;
        if (arr->alloc_elem > arr->max_elem) arr->alloc_elem = arr->max_elem;
        arr->arr = gg_realloc (gg_mem_get_id(arr->arr), arr->alloc_elem * sizeof (char*));
        memset (&(arr->arr[old_alloc]),0, sizeof(char*)*(arr->alloc_elem - old_alloc));
    }

    if (arr->arr[key] != NULL) { if (st) *st = GG_INFO_EXIST; } else { arr->arr[key] = GG_EMPTY_STRING; if (st) *st = GG_OKAY; }
    if (old_val != NULL) 
    {
        *old_val = arr->arr[key]; 
        gg_mem_replace_and_return (*old_val, val);
    }
    else gg_free (arr->arr[key]);

    gg_mem_set_process (arr->arr[key], val, arr->process, true);
    arr->arr[key] = val;
}

//
// Read array from arr, using key number. Delete if del true, and status st is GG_OKAY if read, GG_ERR_EXIST if key is not existing
// Returns the value in the array. 
//
char *gg_read_array (gg_array *arr, gg_num key, bool del, gg_num *st)
{
    GG_TRACE("");
    if (key >= arr->max_elem || key < 0) gg_report_error ("Index [%ld] to array is negative or is beyond maximum allowable size", key);
    if (key >= arr->alloc_elem || arr->arr[key] == NULL)
    {
        if (st != NULL) *st = GG_ERR_EXIST;
        return GG_EMPTY_STRING;
    }
    if (st != NULL) *st = GG_OKAY;
    char *rval = arr->arr[key];
    if (del)  
    {
        arr->arr[key] = NULL; 
        gg_mem_delete_and_return (rval);
    }
    return rval;
}



