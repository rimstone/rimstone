// SPDX-License-Identifier: Apache-2.0
// Copyright 2018-2025 Gliim LLC.
// Licensed under Apache License v2. See LICENSE file.
// On the web http://rimstone-lang.com/ - this file is part of RimStone framework.

//
// Array implementation
//

#include "rim.h"


// function prototypes
#ifdef RIM_ARR_STRING
static void rim_init_array (rim_arraystring *arr, rim_num max_elem, unsigned char process);
#endif
#ifdef RIM_ARR_DOUBLE
static void rim_init_array (rim_arraydouble *arr, rim_num max_elem, unsigned char process);
static inline void rim_set_dbl (rim_dbl *arr, rim_num tot, rim_dbl val)
{
    rim_num i;
    for (i = 0; i < tot; i++) arr[i] = val;
}
#endif
#ifdef RIM_ARR_NUMBER
static void rim_init_array (rim_arraynumber *arr, rim_num max_elem, unsigned char process);
static inline void rim_set_num (rim_num *arr, rim_num tot, rim_num val)
{
    rim_num i;
    for (i = 0; i < tot; i++) arr[i] = val;
}
#endif
#ifdef RIM_ARR_BOOL
static void rim_init_array (rim_arraybool *arr, rim_num max_elem, unsigned char process);
static inline void rim_set_bool (char *arr, rim_num tot, char val)
{
    rim_num i;
    for (i = 0; i < tot; i++) arr[i] = val;
}
#endif


// 
// Init new array with 0 elements.
// max_elem is the absolute maximum size of an array (0 means default of 1,000,000)
// process is true if this is process-scoped array
//
#ifdef RIM_ARR_STRING
static void rim_init_array (rim_arraystring *arr, rim_num max_elem, unsigned char process)
#endif
#ifdef RIM_ARR_DOUBLE
static void rim_init_array (rim_arraydouble *arr, rim_num max_elem, unsigned char process)
#endif
#ifdef RIM_ARR_NUMBER
static void rim_init_array (rim_arraynumber *arr, rim_num max_elem, unsigned char process)
#endif
#ifdef RIM_ARR_BOOL
static void rim_init_array (rim_arraybool *arr, rim_num max_elem, unsigned char process)
#endif
{
    // make top array
    if (max_elem < 0) rim_report_error ("Maximum number of elements in array cannot be negative");
    if (max_elem == 0) max_elem = 1000000;
    if (max_elem < RIM_ARRAY_INC) max_elem = RIM_ARRAY_INC;
    arr->max_elem = max_elem;
    arr->process = process;
    // make initial array
#ifdef RIM_ARR_STRING
    arr->str = rim_calloc (arr->alloc_elem = RIM_ARRAY_INC, sizeof(char*)); // all values NULL, i.e. RIM_STRING_NONE
#endif
#ifdef RIM_ARR_DOUBLE
    arr->dbl = rim_malloc ((arr->alloc_elem = RIM_ARRAY_INC)*sizeof(rim_dbl));
    rim_set_dbl (arr->dbl, arr->alloc_elem, RIM_DOUBLE_NONE);
#endif
#ifdef RIM_ARR_NUMBER
    arr->num = rim_malloc ((arr->alloc_elem = RIM_ARRAY_INC)*sizeof(rim_num));
    rim_set_num (arr->num, arr->alloc_elem, RIM_NUMBER_NONE);
#endif
#ifdef RIM_ARR_BOOL
    arr->logic = rim_malloc ((arr->alloc_elem = RIM_ARRAY_INC)*sizeof(char));
    rim_set_bool (arr->logic, arr->alloc_elem, RIM_BOOL_NONE);
#endif
}

// 
// Make new array. Returns fresh array with 0 elements.
// max_elem is the absolute maximum size of an array (0 means default of 1,000,000)
// process is true if this is process-scoped array
//
#ifdef RIM_ARR_STRING
rim_arraystring *rim_new_arraystring (rim_num max_elem, unsigned char process)
#endif
#ifdef RIM_ARR_DOUBLE
rim_arraydouble *rim_new_arraydouble (rim_num max_elem, unsigned char process)
#endif
#ifdef RIM_ARR_NUMBER
rim_arraynumber *rim_new_arraynumber (rim_num max_elem, unsigned char process)
#endif
#ifdef RIM_ARR_BOOL
rim_arraybool *rim_new_arraybool (rim_num max_elem, unsigned char process)
#endif
{
#ifdef RIM_ARR_STRING
    rim_arraystring *arr = rim_malloc (sizeof(rim_arraystring));
#endif
#ifdef RIM_ARR_DOUBLE
    rim_arraydouble *arr = rim_malloc (sizeof(rim_arraydouble));
#endif
#ifdef RIM_ARR_NUMBER
    rim_arraynumber *arr = rim_malloc (sizeof(rim_arraynumber));
#endif
#ifdef RIM_ARR_BOOL
    rim_arraybool *arr = rim_malloc (sizeof(rim_arraybool));
#endif
    rim_init_array (arr, max_elem, process);
    return arr;
}


//
// Purges array arr. All elements are deleted including the values, and array initialized back to 256 elements.
// max_elem remains to whatever it was.
//
#ifdef RIM_ARR_STRING
void rim_purge_arraystring (rim_arraystring *arr)
#endif
#ifdef RIM_ARR_DOUBLE
void rim_purge_arraydouble (rim_arraydouble *arr)
#endif
#ifdef RIM_ARR_NUMBER
void rim_purge_arraynumber (rim_arraynumber *arr)
#endif
#ifdef RIM_ARR_BOOL
void rim_purge_arraybool (rim_arraybool *arr)
#endif
{
#ifdef RIM_ARR_STRING
    rim_num i;
    for (i = 0; i < arr->alloc_elem; i++) 
    {
        if (arr->str[i] != NULL) rim_mem_dec_process (arr->str[i]);
    }
    rim_mem_dec_process (arr->str);
#endif
#ifdef RIM_ARR_BOOL
    rim_mem_dec_process (arr->logic);
#endif
#ifdef RIM_ARR_DOUBLE
    rim_mem_dec_process (arr->dbl);
#endif
#ifdef RIM_ARR_NUMBER
    rim_mem_dec_process (arr->num);
#endif
    rim_init_array (arr, arr->max_elem, arr->process);
    
}

//
// Write element to array arr. key is the number index, old_val is the old value that was there (if any).
// Will expand array as needed, starting from only 256, up to ->max_elem.
// The actual value is written in v1.c in code generated there to allow old value to be used in expression for new value
//
#ifdef RIM_ARR_STRING
RIM_ALWAYS_INLINE inline char *rim_write_arraystring (rim_arraystring *arr, rim_num key, char **old_val)
#endif
#ifdef RIM_ARR_DOUBLE
RIM_ALWAYS_INLINE inline void rim_write_arraydouble (rim_arraydouble *arr, rim_num key, rim_num *old_val)
#endif
#ifdef RIM_ARR_NUMBER
RIM_ALWAYS_INLINE inline void rim_write_arraynumber (rim_arraynumber *arr, rim_num key, rim_num *old_val)
#endif
#ifdef RIM_ARR_BOOL
RIM_ALWAYS_INLINE inline void rim_write_arraybool (rim_arraybool *arr, rim_num key, bool *old_val)
#endif
{
    if (key < 0) rim_report_error ("Index to array is negative [%ld]", key);
    if (key >= arr->max_elem) rim_report_error ("Index to array is too large for array sizing [%ld], maximum allowed is currently set to [%ld] (see max-size clause in new-array)", key, arr->max_elem);
    if (key >= arr->alloc_elem)
    {
        rim_num old_alloc = arr->alloc_elem;
        // assure newly alloc_elem is greater than key
        if (key < 65536) arr->alloc_elem = key* 2; else arr->alloc_elem = key+ 65536;
        if (arr->alloc_elem > arr->max_elem) arr->alloc_elem = arr->max_elem;
#ifdef RIM_ARR_STRING
        arr->str = rim_realloc (rim_mem_get_id(arr->str), arr->alloc_elem * sizeof (char*));
        memset (&(arr->str[old_alloc]),0, sizeof(char*)*(arr->alloc_elem - old_alloc));
#endif
#ifdef RIM_ARR_DOUBLE
        arr->dbl = rim_realloc (rim_mem_get_id(arr->dbl), arr->alloc_elem * sizeof (rim_dbl));
        rim_set_dbl (&(arr->dbl[old_alloc]), arr->alloc_elem - old_alloc, RIM_DOUBLE_NONE);
#endif
#ifdef RIM_ARR_NUMBER
        arr->num = rim_realloc (rim_mem_get_id(arr->num), arr->alloc_elem * sizeof (rim_num));
        rim_set_num (&(arr->num[old_alloc]), arr->alloc_elem - old_alloc, RIM_NUMBER_NONE);
#endif
#ifdef RIM_ARR_BOOL
        arr->logic = rim_realloc (rim_mem_get_id(arr->logic), arr->alloc_elem * sizeof (bool));
        rim_set_bool (&(arr->logic[old_alloc]), arr->alloc_elem - old_alloc, RIM_BOOL_NONE);
#endif
    }

    if (old_val != NULL) 
    {
#ifdef RIM_ARR_STRING
        char *cstr = arr->str[key]==RIM_STRING_NONE?RIM_EMPTY_STRING:arr->str[key];
        *old_val = cstr;
        return cstr;
#endif
#ifdef RIM_ARR_DOUBLE
        *old_val = isnan(arr->dbl[key])?0:arr->dbl[key];
#endif
#ifdef RIM_ARR_NUMBER
        *old_val = arr->num[key]==RIM_NUMBER_NONE?0:arr->num[key];
#endif
#ifdef RIM_ARR_BOOL
        *old_val = arr->logic[key]==RIM_BOOL_NONE?false:(arr->logic[key]);
#endif
    }
#ifdef RIM_ARR_STRING
    else return arr->str[key]==RIM_STRING_NONE?RIM_EMPTY_STRING:arr->str[key];
#endif

    // 
    // The actual setting of arr->xxx[key] is done in v1.c in order be able to use old_val
    // This increases performance as read/write can happen in one memory check
    //
}

//
// Read array from arr, using key number. Delete if del true, and status st is RIM_OKAY if read, RIM_ERR_EXIST if key is not existing
// s is status, always available (never NULL)
// Returns the value in the array. 
//
#ifdef RIM_ARR_STRING
RIM_ALWAYS_INLINE inline char *rim_read_arraystring (rim_arraystring *arr, rim_num key, rim_num *st)
#endif
#ifdef RIM_ARR_DOUBLE
RIM_ALWAYS_INLINE inline rim_dbl rim_read_arraydouble (rim_arraydouble *arr, rim_num key, rim_num *st)
#endif
#ifdef RIM_ARR_NUMBER
RIM_ALWAYS_INLINE inline rim_num rim_read_arraynumber (rim_arraynumber *arr, rim_num key, rim_num *st)
#endif
#ifdef RIM_ARR_BOOL
RIM_ALWAYS_INLINE inline bool rim_read_arraybool (rim_arraybool *arr, rim_num key, rim_num *st)
#endif
{
    if (__builtin_expect(key >= arr->max_elem || key < 0,0)) rim_report_error ("Index [%ld] to array is negative or is beyond maximum allowable size", key);
    if (__builtin_expect(key >= arr->alloc_elem,0)) {
        return 
#ifdef RIM_ARR_STRING
        RIM_EMPTY_STRING;
#endif
#ifdef RIM_ARR_DOUBLE
        0.0;
#endif
#ifdef RIM_ARR_NUMBER
        0;
#endif
#ifdef RIM_ARR_BOOL
        false;
#endif
    }
#ifdef RIM_ARR_STRING
    if (__builtin_expect (arr->str[key]==RIM_STRING_NONE, 0))
    {
        *st=RIM_ERR_EXIST;
        return RIM_EMPTY_STRING;
    }
    else
    {
        *st=RIM_OKAY;
        return arr->str[key];
    }
#endif
#ifdef RIM_ARR_DOUBLE
    if (__builtin_expect (isnan(arr->dbl[key]), 0)) // RIM_DOUBLE_NONE is NAN!
    {
        *st=RIM_ERR_EXIST;
        return 0;
    }
    else
    {
        *st=RIM_OKAY;
        return arr->dbl[key];
    }
#endif
#ifdef RIM_ARR_NUMBER
    if (__builtin_expect (arr->num[key]==RIM_NUMBER_NONE, 0))
    {
        *st=RIM_ERR_EXIST;
        return 0;
    }
    else
    {
        *st=RIM_OKAY;
        return arr->num[key];
    }
#endif
#ifdef RIM_ARR_BOOL
    if (__builtin_expect (arr->logic[key]==RIM_BOOL_NONE, 0))
    {
        *st=RIM_ERR_EXIST;
        return false;
    }
    else
    {
        *st=RIM_OKAY;
        return arr->logic[key];
    }
#endif
}



