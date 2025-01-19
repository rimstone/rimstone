// SPDX-License-Identifier: Apache-2.0
// Copyright 2019 Gliim LLC.
// Licensed under Apache License v2. See LICENSE file.
// On the web http://golf-lang.com/ - this file is part of Golf framework.

//
// Array implementation
//

typedef gg_array_data_s {
    char *key;
    char *value;
} gg_array_data;
// 
// Array type. Two dimensional array where first one points to a block of pointers to data. This way each new element in top array points to a block
// of arrays. Only when all are exhausted, a new top level is relloced. So if we have 1024 per block, each of 1024 top level pointers points to a block
// of 1024 bottom level pointers. It takes 1M pointers to realloc top level from 1024 to 2048, etc.
// This is bottom array.
//
typedef gg_array_bottom_s {
    gg_array_data *arr; // array as 2 dimensional array underneath so we do realloc sparingly
    gg_num tot; // total number of elements in arr that's used, so originally 0
} gg_array_bottom;
//
// Top array for an array type (see above).
//
typedef gg_array_top_s {
    gg_array_bottom *bottom_arr; // list of bottom blocks of pointers
    gg_num tot_bottom; // how many of those we have allocated, so 1 means one block of GG_ARRAY_INC_BOTTOM elements
    gg_num tot_elem; // how many total elements (key/value pairs) there are
    gg_num max_elem; // maximum elements we can hold now - can be calculated at run time for each add, this caches it so it's faster
} gg_array_top;
// increments for size of top and bottom. Top gets realloced, bottom just malloc's new blocks.
#define GG_ARRAY_INC_TOP 1
#define GG_ARRAY_INC_BOTTOM 2

// 
// Make new array. Returns fresh array with 0 elements.
//
gg_array *gg_new_array ()
{
    // make top array
    gg_array_top *garr = gg_malloc ((garr->tot_bottom = GG_ARRAY_INC_TOP) * sizeof(gg_array_top));
    garr->tot_elem = 0;
    // make one bottom array
    gg_array_data **arr = gg_malloc (GG_ARRAY_INC_BOTTOM *sizeof(gg_array_data*));
    garr->bottom_arr[0]->arr = arr;
    garr->bottom_arr[0]->tot = 0;
    garr->max_elem = GG_ARRAY_INC_TOP * GG_ARRAY_INC_BOTTOM;
    return garr;
}

void gg_add_array (gg_array *arr, char *key, char *val)
{
    if (arr->gg_tot_elem == arr->max_elem)
    {
        arr->gg_
        *arr = gg_realloc ((garr->tot_bottom += GG_ARRAY_INC_TOP) * sizeof (gg_array_top));
    }

}



