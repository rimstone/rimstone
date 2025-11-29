// SPDX-License-Identifier: Apache-2.0
// Copyright 2018-2025 Gliim LLC.
// Licensed under Apache License v2. See LICENSE file.
// On the web http://rimstone-lang.com/ - this file is part of RimStone framework.

//
// Sorting of Arrays implementation
//

#include "rimcommon.h"

#ifdef RIM_SORT_DESC
#define RIM_SORT_MOD(x) (-(x))
#else
#define RIM_SORT_MOD(x) (x)
#endif


// function prototypes
#ifdef RIM_ARR_STRING
#ifdef RIM_SORT_DESC
#define SORT_NAME rim_string_desc
#else
#define SORT_NAME rim_string
#endif
#define SORT_TYPE char*
#define SORT_CMP(x, y) RIM_SORT_MOD(rim_cm_str_sort ((const char*)(x), rim_mem_get_len(x), (const char*)(y), rim_mem_get_len(y)))
#include "libsort/sort.h"
#endif


#ifdef RIM_ARR_DOUBLE
#ifdef RIM_SORT_DESC
#define SORT_NAME rim_double_desc
#else
#define SORT_NAME rim_double
#endif
#define SORT_TYPE rim_dbl
#define SORT_CMP(x, y) RIM_SORT_MOD(((x) < (y) ? -1 : ((y) < (x) ? 1 : 0)))
#include "libsort/sort.h"
#endif


#ifdef RIM_ARR_NUMBER
#ifdef RIM_SORT_DESC
#define SORT_NAME rim_number_desc
#else
#define SORT_NAME rim_number
#endif
#define SORT_TYPE rim_num
#define SORT_CMP(x, y) RIM_SORT_MOD(((x) - (y))) 
#include "libsort/sort.h"
#endif

