// SPDX-License-Identifier: Apache-2.0
// Copyright 2018 Gliim LLC. 
// Licensed under Apache License v2. See LICENSE file.
// On the web http://golf-lang.com/ - this file is part of Golf framework. 

// 
// Memory handling, including garbage collector
// GOLF memory is made for request-based processing. Simplest form of memory management is for 
// memory to be allocated anywhere and released at the end of the request, GOLF will de-allocate 
// it automatically. 'string' is memory and is any binary or text memory. Objects such as indexes, arrays,
// lists, split strings etc are never freed; but the items they hold (aka elements, mostly key/values) can be freed when
// an element is deleted; however superstructure of an index, array etc remains but is miniscule in memory footprint.
// Process-scoped memory will never be automatically released, but it can be deleted with delete-string.
// Each string has a reference count - note it's not a variable that has refcount, but a region of memory that variable(s) point to. 
// When a new memory is created (such as write-string or random-string), and a variable points to it in a statement,
// then its refcount is zero. When a variable X is assigned to variable Y (Y=X), then the refcount of memory pointed by Y is
// decreased by 1, and refcount of memory pointed by X is increased by 1; except when Y==X (meaning both X and Y point to the 
// same memory) in which case nothing is done. Data in elements (such as index, array etc) have refcount increased when created; so
// insert into index or list from a variable means such data has refcount +1. When element data is updated, then old data's refcount
// is decreased by 1, and variable's that's used to update is increased by 1. Parameters (get/set-param), cookies and request body (request-body)
// get refcount of 1 when assigned to variable (get-param, request-body); set-param will decrease old param's refcount by 1 and increase
// new one's by 1. When refcount is 0 and we decrease it, memory is freed. When a scope exits (like {} or start/end-loop) all local variables
// are freed - the actual memory may still be there is memory has refcount>0 such as if assigned to process-scope variable. 
// Scope also means call-handler and before/after handlers. Note that process-scope memory, even if refcount is 0, won't be freed until the end of
// the request.
//
//
//


#include "golf.h"

bool gg_mem_process = false; // if set to true, any allocs are process-scoped. Memory is not released with request's end. Process memory.

// functions
bool gg_mem_get_process (gg_num r);
void *gg_mem_get_data (gg_num r);

char *gg_mem_msg_outmem = "Out of memory for [%ld] bytes";

// Common empty string constant, used for initialization
// When a variable has this value, it means it is freshly initialized and it
// will be either re-assigned, or allocated. (value assigned later)
// so in the program it's never actually NULL, always ""
char *GG_EMPTY_STRING="";


// free block
#define GG_MEM_FREE 1
// open file
#define GG_MEM_FILE 2
// process-request mem
#define GG_MEM_PROCESS 4
// memory that's made from string literal
#define GG_MEM_CONST 8

// memory list, maximum of about 2^47-1 allocated memory blocks per request
// len is the length of memory pointed by ptr. null byte is always there and is included. Max length is 63 bit max.
// next_free is index to next freed block, could be anything by default. This relates to the number of variables in the program,
// status has: 
// GG_MEM_FREE bit set if this is freed block, 0 if not. 
// GG_MEM_FILE bit set if this is a file that eventually needs to close (unless closed already)
// GG_MEM_PROCESS bit set if this is process memory, i.e. not to be released at the end of request
// GG_MEM_CONST bit set if this is memory that can't be freed
// ref is the number of references to process memory (max of 127), meaning max of 127 indexes, arrays and such can reference the same memory
// ref is also the max number of vars that can reference a non-process memory.
typedef struct s_vml {
    void *ptr;
    gg_num next_free:48;  
    gg_num status:8; 
    gg_num len:48; 
    gg_num ref:24;
} vml;
#define GG_MAX_REF ((1<<23)-1)

//
// Header prior to alloced memory. id is index into vm[]
//
typedef struct s_gg_head {
    gg_num id;
} gg_head;

// static variables used in memory handling
// We delete old's request memory at the very start of a new request in generated code before any user code,
// (unless process scoped memory)
//
static vml *vm = NULL;
static gg_num vm_curr = 0;
static gg_num vm_tot = 0;
static gg_num vm_first_free = -1; // first free memory block


// determines the size of the block allocated (and the size of consequent expansions) for the memory
// block that keeps all pointers to allocated blocks.
#define GOLFMSIZE 512


// 
// must be called at the very beginning of program. Initializes memory and 
// cleans up any memory left over from a previous request.
//
void gg_memory_init ()
{
    assert (sizeof(gg_num) == 8); // if not, we need to rework GG_ALIGN-sized header placement (8 bytes)
    // setup "non-initialized memory" (neither NULL nor initialized)

    // release of previous request's memory and creation of new memory for the following request
    // is done here; gg_memory_init() is called at the end of any request.
    gg_done ();

}

//
// Get alloc'd size of vm[]
//
inline gg_num gg_mem_size ()
{
    return vm_tot;
}

//
// Get number of actually alloc'd memory blocks
//
inline gg_num gg_mem_items ()
{
    gg_num res = 0;
    gg_num i;
    for (i = 0; i < vm_curr; i++)
    {
        if (!(vm[i].status & GG_MEM_FREE)) res++; else continue;
    }
    return res;
}


//
// Set status of memory, s is the status such as GG_MEM_FILE
// ind is the index into vm[]
//
inline void gg_mem_set_status (gg_num ind, unsigned char s)
{
    vm[ind].status |= s;
}

// 
// Add point to the block of memory. 'p' is the memory pointer (allocated elsewhere here) added.
// Returns the index in memory block where the pointer is.
// Once a block of pointers is exhausted, add another block. We do not increase the blocks
// size size requests are generally small and typically do not need much more memory, and increasing
// the block size might cause swaping elsewhere.
//
inline gg_num gg_add_mem (void *p)
{
    gg_num r;
    if (vm_first_free != -1)
    {
        r = vm_first_free;
        vm_first_free = vm[vm_first_free].next_free; // this block if free for sure b/c vm_first_free points to it
                                                     // next_free is -1 if there's no next, and -1 is vm_first_free when none
    } else {
        r = vm_curr;
        vm_curr++;
        if (vm_curr >= vm_tot)
        {
            gg_num old_vm_tot = vm_tot;
            vm_tot += GOLFMSIZE;
            vm = realloc (vm, vm_tot * sizeof (vml));
            if (vm == NULL)
            {
                gg_report_error (gg_mem_msg_outmem, vm_tot*sizeof(vml));
            }
            // initialize memory status, this is done automatically in calloc, but NOT in realloc
            while (old_vm_tot != vm_tot) vm[old_vm_tot++].status = 0;
        }
    }
    vm[r].ptr = p;
    // .len must be set always set by caller
    vm[r].status = 0; // not a freed block, not process, cannot delete, not a file
    vm[r].ref = 0;
    if (gg_mem_process) vm[r].status |= GG_MEM_PROCESS;
    return r;
}




// 
// Adds string constant to memory pool
// 'r' is the index in vm[].
// The memory returned is the actually a pointer to useful memory (that a GOLF program can use). We place
// some information at the beginning of the memory pointed to by alloc'd mem: the reference to the 
// index in the block of memory where memory (p) is;
//
inline void *gg_mem_add_const (void *p, gg_num len)
{
    gg_num id = gg_add_mem (p);
    void * pm = gg_vmset(p, id);
    gg_mem_set_process(pm, true);
    gg_mem_set_len(id, len);
    vm[id].status |= GG_MEM_CONST;
    return pm;

}

// 
// Adds pointer to our block of memory. 
// 'r' is the index in vm[].
// The memory returned is the actually a pointer to useful memory (that a GOLF program can use). We place
// some information at the beginning of the memory pointed to by alloc'd mem: the reference to the 
// index in the block of memory where memory (p) is;
//
inline void *gg_vmset (void *p, gg_num r)
{
    gg_head h;
#ifdef DEBUG
    if (r > ((gg_num)256*1024*1024*1024)) // check it's not a runaway program // 256 billion
    {
        gg_mem_release(r); // otherwise final gg_done would fail at this one, as it's incomplete
        gg_report_error ("Too many variables [%ld]", r); 
    }
#endif
    h.id = r; 
    memcpy ((unsigned char*)p, (unsigned char*)&h, sizeof (gg_head));
    return (unsigned char*)p + GG_ALIGN;
}


// 
// input and returns are like malloc().
//
inline void *gg_malloc(size_t size)
{
    size_t t;
    void *p = malloc (t=size + GG_ALIGN);
    if (p == NULL) 
    {
        gg_report_error (gg_mem_msg_outmem, size+GG_ALIGN);
    }
    // add memory pointer to memory block
    gg_num r = gg_add_mem (p);
    void *pm = gg_vmset(p,r);
    gg_mem_set_len (r, size);
    return pm;
}

// 
// input and returns are like calloc()
// See malloc for the rest
//
inline void *gg_calloc(size_t nmemb, size_t size)
{
    size_t t = nmemb*size;
    void *p =  calloc (1, t + GG_ALIGN);
    if (p == NULL) 
    {
        gg_report_error (gg_mem_msg_outmem, t);
    }
    gg_num r = gg_add_mem (p);
    void *pm = gg_vmset(p,r);
    gg_mem_set_len (r, t);
    return pm;
}

//
// Get index of pointer in memory block, -1 if empty or NULL
//
inline gg_num gg_mem_get_id (void *ptr)
{
    if (ptr == GG_EMPTY_STRING) return -1; // this is just empty string
    if (ptr == NULL) gg_report_error ("Invalid memory detected");
    gg_head *h = (gg_head*)((unsigned char*)ptr-GG_ALIGN);
    gg_num r = h->id;
#ifdef DEBUG
    // at debug time, check each memory accessed is actually correct!
    if ((r < 0 || r >= vm_curr) || vm[r].ptr != ((unsigned char*)ptr-GG_ALIGN)) gg_report_error("Attempted to get length of invalid memory");
#endif
    return r;
}

//
// Get length of memory block. GG_ALIGN is 2*sizeof(gg_num)
//
inline gg_num gg_mem_get_len (gg_num r)
{
    if (r == -1) return 0; // this is just empty string
    return vm[r].len-1; // there's always trailing 0 set by all golf statements, so .len is useful length + 1
}


//
// Link new freed memory block r
//
void gg_mem_release(gg_num r)
{
    vm[r].ptr = NULL;
    vm[r].status = GG_MEM_FREE; // freed, all other flags cleared
    // Set memory marked as freed, would be -1 if there's no free blocks at this moment
    vm[r].next_free = vm_first_free;
    vm_first_free = r;
}

//
// Set length of memory. ptr is the memory alloc'd by Golf.
// Memory in Golf is always sized exactly as needed
// len is useful data plus 1 for nullbyte, so for "ab", it's 3
//
inline void gg_mem_set_len (gg_num r, gg_num len)
{
#ifndef GG_MEM_CLIENT
    vm[r].len = len;
#else
    GG_UNUSED(r);
    GG_UNUSED(len);
#endif
}


//
// Get actual data from vm[] index
//
inline void *gg_mem_get_data (gg_num r)
{
    if (r == -1) return GG_EMPTY_STRING; // no need to set for predefined empty string
    return vm[r].ptr+GG_ALIGN;
    
}

// 
//
// Set memory to be process-scoped. ptr is the memory alloc'd by Golf.
// This is done only if gg_mem_process is true, or if force is true
//
inline void gg_mem_set_process (char *m, bool force)
{
    if (m == GG_EMPTY_STRING) return; // no need to set for predefined empty string
    gg_num r;
    // if not optimized-memory, we only use ref to make process-scoped memory unfreeable until all those that
    // reference it are deleted. For instance a set-string (constant or process-scope) is assigned to 5 other strings, all
    // of which are not process-scope; or it is inserted as value in 10 index records. Deleting any  of those variables or less than
    // all 10 index records should NOT free this variable, not even at the end of the request. In non-optimized-memory, this is the only 
    // reference counting. All other variables will be released at the end of the request, no matter their scope or how they are used, which
    // makes for about 20% speed improvement in highly computational request (probably 5% otherwise), still significant.
    // this below is just if it's actually process-scoped
    if (gg_mem_process || force) 
    { 
        r = gg_mem_get_id (m); 
        vm[r].ref++; 
        vm[r].status |= GG_MEM_PROCESS;
    }
}

// 
//
// Get if memory is process scoped, true or false, r is index into vm[]
//
inline bool gg_mem_get_process (gg_num r)
{
    if (r == -1) return true; // no need to set for predefined empty string
    return vm[r].status & GG_MEM_PROCESS;
}

// 
// Input and return the same as for realloc()
// Checks memory to make sure it's valid block allocated here.
// if safe is 1, perform additional checks to make sure it's okay memory
// Golf never realloc's existing variable, b/c Golf variables reference each other
// directly, meaning there's no indirection (first to a structure, then pointer) for performance.
//
inline void *gg_realloc(gg_num r, size_t size)
{
    size_t t;
    //
    // Check if string uninitialized, if so, allocate it for the first time
    // Also, if pointer is a NULL ptr, just allocate memory.
    //
    if (r == -1)
    {
        return gg_malloc (size);
    }

    bool is_process = gg_mem_get_process (r);

    // first realloc pointer
    void *p= realloc ((unsigned char*)(vm[r].ptr), t=size + GG_ALIGN);
    if (p == NULL) 
    {
        gg_report_error (gg_mem_msg_outmem, size+GG_ALIGN);
    }
    // then nullify pointer and release vm[] entry
    gg_mem_release(r);
    // then add new vm[] entry
    r = gg_add_mem(p);
    void *pm = gg_vmset(p,r);
    gg_mem_set_len (r, size);
    if (is_process) gg_mem_set_process (pm,1);
    return pm;
}

//
// When process scoped element is deleted, but value returned, then this needs to happen.
// We reduce ref by 1, and check if down to 0, if so and if it's a process-scoped memory, then
// remove process-scoped designation, and increase ref back by 1. This way such memory will be 
// released at the end of the request. If not, such memory would remain (because it would still be
// process-scoped and it shouldn't be), resulting in ever-growing memory and eventual crash.
//
inline void gg_mem_delete_and_return (void *ptr)
{
    if (ptr == GG_EMPTY_STRING || ptr == NULL) return;
    gg_num r = gg_mem_get_id(ptr); // index in the list of memory blocks
#ifdef DEBUG
    if (r < 0 || r >= vm_curr) gg_report_error ("Memory is not within valid virtual range [%ld]", r);
    if (vm[r].ptr != ((unsigned char*)ptr-GG_ALIGN)) gg_report_error ("Memory header is invalid");
#endif
    if ((vm[r].status & GG_MEM_PROCESS))
    {
        // only remove process scoped designation in non-optimized-memory case if all references to it have
        // been freed; it means all process-scoped ones - see comments in gg_mem_set_process()
        if (vm[r].ref > 0) 
        { 
            vm[r].ref--; 
            if (vm[r].ref == 0) vm[r].status &= ~GG_MEM_PROCESS;
            vm[r].ref++;
            return; 
        }
    }
}

// 
// Similar to free(), returns GG_OKAY if okay, others if not. If check is 1, it 
// checks if memory is valid in debug mode. If check is 2, then this is free at the end of request, 3 for delete-string that is not defined as 
// process-scoped, or 0 for string that's defined as process-scoped (meaning just that one, not any other this string gets assigned to!).
// Checks memory to make sure it's valid block allocated here.
// Returns GG_ERR_MEMORY if bad memory, GG_ERR_INVALID if cannot delete because it's not a string marked to have the ability for deletion
//
inline void _gg_free (void *ptr, char check)
{
    //
    // if programmer mistakenly frees up GG_EMPTY_STRING, just ignore it
    // this is true whether using golf mem or OS mem
    //
    if (ptr == GG_EMPTY_STRING || ptr == NULL) return;

    //
    // Any point forward code can sigegv in this function or return error to indicate memory is bad
    //
    gg_num r = gg_mem_get_id(ptr); // index in the list of memory blocks

#ifdef DEBUG
    if (r < 0 || r >= vm_curr) gg_report_error ("Memory is not within valid virtual range [%ld]", r);
    if (vm[r].ptr != ((unsigned char*)ptr-GG_ALIGN)) gg_report_error ("Memory header is invalid");
#endif
    if (vm[r].status & GG_MEM_CONST) 
    {
        // const memory can't be released nor freed, however it can be referenced, so we can decrease ref
        // this must be before checking for GG_MEM_PROCESS
        // because otherwise we might remove process-scope designation
        return;
    }
    // if memory is process, just mark it non-process and delay until the end of request, much faster
    // and simpler than counting references. This will be freed at the end of request
    // Do not do this if this is freeing at the end of request, of course.
    if (check == 0)
    { // this is free from delete-string etc (not from end of request)
        // delete process-scoped delayed, only at the end of request, so if a variable takes its value
        // it stays valid through the end of request - must to keep functionality intact!
        if ((vm[r].status & GG_MEM_PROCESS)) 
        {
            // only remove process scoped designation in non-optimized-memory case if all references to it have
            // been freed; it means all process-scoped ones - see comments in gg_mem_set_process()
            if (vm[r].ref > 0)  vm[r].ref--; 
            if (vm[r].ref == 0) vm[r].status &= ~GG_MEM_PROCESS; 
            return;
        }
        return; // do not delete memory until the end of request, regardless of the reason! Takes the most
                // memory but by far the fastest, not just b/c there's no overhead, but deleting memory in one go
                // caches the vm[] and makes it faster
    } else if (check != 2) return; // right now the only place memory is released is at the end of request (check==2)

    //
    // if memory on the list of freed blocks, just ignore it, otherwise double free or free
    // of an invalid pointer ensues. 
    //
    if (vm[r].status & GG_MEM_FREE) return;

    // free mem
#ifdef DEBUG
    // check number of references

    // for debugging, make the freed memory empty to aid in testing of freeing, since often it remains the same
    // and freed memory always has at least one byte
    memset (ptr, 0, vm[r].len); // .len is at least 1
#endif
    free ((unsigned char*)ptr-GG_ALIGN);
    // release vm[]
    gg_mem_release(r);
    //
    return;
}

// 
// 
// Like strdup below, but copies exactly l+1 bytes and sets length
// Starting from byte 'from' (0 is first)
//
inline char *gg_strdupl (char *s, gg_num from, gg_num l)
{
   if (from > l)  gg_report_error ("Cannot copy from byte [%ld] when length is [%ld]", from, l);
   char *n = (char*)gg_malloc (l+1-from);
   gg_num r = gg_mem_get_id(n);
   gg_mem_set_len(r, l-from+1);
   if (n == NULL) 
   {
       gg_report_error (gg_mem_msg_outmem, l+1-from);
   }
   memcpy (n, s+from, l+1-from);
   return n;
}

// 
// Input and return the same as for strdup()
// Strdup guaranteed set memory length
//
inline char *gg_strdup (char *s)
{
   if (s == GG_EMPTY_STRING) return GG_EMPTY_STRING;
   gg_num l = strlen(s);
   char *n = (char*)gg_malloc (l+1);
   gg_num r = gg_mem_get_id(n);
   gg_mem_set_len(r, l+1);
   if (n == NULL) 
   {
       gg_report_error (gg_mem_msg_outmem, l+1);
   }
   memcpy (n, s, l+1);
   return n;
}

// 
// Frees all memory allocated so far.
// This is called at the beginning of a request before memory is allocated again.
// The reason this is NOT called at the end of the request is that web server NEEDS
// some of the allocated memory even after the request ends, for example, the 
// actual web output or header information.
//
void gg_done ()
{
    bool keep_mem = false;
    if (vm != NULL)
    {
        gg_num i;
        for (i = 0; i < vm_curr; i++)
        {
            // if already freed, continue; we can do such a check prior to actually freeing since
            // we have the index into vm[] here
            if (vm[i].status & GG_MEM_FREE) continue; 
                                                                        
            if (vm[i].ptr != NULL)
            {
                //GG_TRACE("Freeing [%ld] block of memory", i);
                if (vm[i].status & GG_MEM_FILE)
                {
                    FILE **f = (FILE**)(vm[i].ptr);
                    if (*f != NULL) fclose (*f);
                    *f=NULL; // make sure it's NULL so the following request can use this file descriptor
                }
                else if (!(vm[i].status & GG_MEM_PROCESS))
                {
                    _gg_free ((unsigned char*)vm[i].ptr+GG_ALIGN, 2);
                } else keep_mem = true;
            }
        }
        //GG_TRACE("Freeing vm");
        if (!keep_mem) 
        {
            free (vm);
            vm = NULL;
        }
    }
    if (vm == NULL) 
    {
        // init memory if it's not there. Keep it if it has process-memory, but release all others
        vm = calloc (vm_tot = GOLFMSIZE, sizeof (vml));
        if (vm == NULL) gg_report_error ("Out of memory");
        vm_curr = 0;
        vm_first_free = -1;
    }
}






