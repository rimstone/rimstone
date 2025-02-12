// SPDX-License-Identifier: Apache-2.0
// Copyright 2018 Gliim LLC. 
// Licensed under Apache License v2. See LICENSE file.
// On the web http://golf-lang.com/ - this file is part of Golf framework. 

// 
// Memory handling
// GOLF memory is made for request-based processing. 
// Memory can be ordinary or process-scoped (PS). Ordinary memory is always automatically released at the end of the request, and you 
// don't have to release it. You can still do that immediately with delete-string, but only if memory is not referenced by another variable or by an object
// like a tree, hash etc. If it is, it will be released at the end of the request. This eliminates excessive reference count updating, which can slow down
// execution 20% or more.
// PS memory is not released at the end of the request. It is used to create server applications, that keep data for the life of the process.
// PS memory has a reference counter attached to it, and it is released when no other PS memory refers to it. For instance, you can have a string
// in a tree and a hash. The element holding it must be deleted in both tree and hash in order to be truly deleted. Even then, such memory is only
// deleted at the end of the request.
// Memory in Golf is assigned by reference. Golf's memory safety system is minimal for performance reason and it uses the fact that requests are generally
// quick and memory released at the of the request is likely to be faster and cause less memory fragmentation than otherwise. 
// Memory is also checked for boundaries. This is however done only at the input of the statement, and the actual implementation doesn't do this (since
// it's carefully crafted for memory safety), improving performance greatly.
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
// len is the length of memory pointed by ptr. null byte is always there and is included. Max length is 2^47-1.
// next_free is index to next freed block, could be anything by default. This relates to the number of variables in the program,
// status has: 
// GG_MEM_FREE bit set if this is freed block, 0 if not. 
// GG_MEM_FILE bit set if this is a file that eventually needs to close (unless closed already)
// GG_MEM_PROCESS bit set if this is process memory, i.e. not to be released at the end of request
// GG_MEM_CONST bit set if this is memory that can't be freed
// ref is the number of references to process memory (max of 2^23-1), which is the number of "duplications" (by reference) of the same memory.
typedef struct s_vml {
    void *ptr;
    gg_num next_free:48;  
    gg_num status:8; 
    gg_num len:48; 
    gg_num ref:24;
} vml;
#define GG_MAX_REF ((1<<23)-1)
#define GG_MAX_MEM (((gg_num)1<<47)-1)

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
// must be called at the very end of program. 
// cleans up any memory 
//
void gg_memory_init ()
{
    assert (sizeof(gg_num) == 8); // if not, we need to rework GG_ALIGN-sized header placement (8 bytes)

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
// size requests are generally small and typically do not need much more memory, and increasing
// the block size might cause swaping elsewhere.
//
inline gg_num gg_add_mem (void *p)
{
    gg_num r;
    if (vm_first_free != -1)
    {
        // find next free
        r = vm_first_free;
        vm_first_free = vm[vm_first_free].next_free; // this block if free for sure b/c vm_first_free points to it
                                                     // next_free is -1 if there's no next, and -1 is vm_first_free when none
    } else {
        // there's no free, get more memory
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
    gg_mem_set_process(GG_EMPTY_STRING, pm, true, false);
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
    if (r > GG_MAX_MEM) // check it's not a runaway program 
    {
        gg_mem_release(r); // otherwise final gg_done would fail at this one, as it's incomplete
        gg_report_error ("Too many variables [%ld]", r); 
    }
#endif
    // Copy ID just before memory
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
// Free block, Link new freed memory block r
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
// Memory in Golf is almost always sized exactly as needed
// len is useful data plus 1 for nullbyte, so for "ab", it's 3
//
inline void gg_mem_set_len (gg_num r, gg_num len)
{
    vm[r].len = len;
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
// When process-scoped memory in an object is replaced, and old value returned
//
inline void gg_mem_replace_and_return (void *ptr, void *new_var)
{
    if (ptr == GG_EMPTY_STRING || ptr == NULL) return;
    if (ptr == new_var) return; // don't do anything if replaced with the same value
    gg_mem_delete_and_return (ptr); // it's as if we deleted and returned the value
                                    // we do replace it with new value, but that's done in the caller
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
        if (vm[r].ref > 0) 
        { 
            vm[r].ref--; 
            if (vm[r].ref == 0) vm[r].status &= ~GG_MEM_PROCESS;
            vm[r].ref++; // restore or set back to 1
            return; 
        }
    }
}
// 
//
// Set ref count of memory when referenced by another variable, to 1. 
// This is done only if it's not process-mem. If it's process-mem, it's subject to gg_mem_set_process()
// which increases ref for those. 
//
inline void gg_mem_add_ref (char *m)
{
    // if (from == to) return; // do not make circular references - we'd check for this if we kept accurate ref count
                               // but for non-process, it's only 0 or 1
    // but we only keep a boolean if attempted to assign
    gg_num r = gg_mem_get_id (m);  // r is -1 if GG_EMPTY_STRING
    if (r != -1 && !(vm[r].status & GG_MEM_PROCESS)) vm[r].ref = 1;
}

// 
//
// This increases ref count. For either, it won't do anything if same memory is overwritten.
// For process-scoped, it will increase ref count and set process mem flag.
// For ordinary memory, it will set ref to 1.
//
inline void gg_mem_set_process (char *to, char *m, bool force, bool add_ref)
{
    if (m == GG_EMPTY_STRING) return; // no need to set for predefined empty string

    // We consider this memory to be process-scoped if 1) is already so 2) gg_mem_process is true, or force is true
    bool is_process = (gg_mem_process || force);
    gg_num r = -1;
    bool process_set = false;
    if (is_process || add_ref) 
    {
        if (to == m) 
        {
            return; // no ref increase if going back to write the same data
        }
        r = gg_mem_get_id (m); 
        process_set = (vm[r].status & GG_MEM_PROCESS);
    }
    if (is_process)
    { 
        // if this wasn't process variable, then wipe out references, since process-scoped works  on its own, they don't mix with non-process ones.
        if (!process_set) vm[r].ref = 1; else vm[r].ref++; 
        vm[r].status |= GG_MEM_PROCESS;
        process_set = true;
    }
    if (add_ref && !process_set)
    {
        if (r != -1) vm[r].ref = 1;
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
// Golf will never realloc's existing variable, b/c Golf variables reference each other
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
    if (is_process) gg_mem_set_process (GG_EMPTY_STRING, pm,true, false);
    return pm;
}


// 
// Similar to free()
// Check is:
// 0, this is free while request is running, and is just for process-scoped memory. Ordinary memory is never 
//      freed with this, because we do NOT keep accurate ref count for it; we only mark it's ref count to 1 if
//      it was ever referenced, and that's it (for performance). Because this is only for process-scoped memory,
//      delete string will use this only for objects (such as tree, hash, array) or if string was defined 
//      with set-string...proces-scope - meaning set-string A=B (if B is process), then A will NOT be freed - 
//      only the original B will; this is because keeping track of these relationships would be expensive (and not really
//      necessary). If ordinary memory is attempted to free with this, it just won't do anything.
// 2, this is release of all ordinary memory at the end of request
// 3, this is free while request is running, and is just for ordinary memory. Such memory is released ONLY if
//      it was never referenced. It doesn't matter if it was dereferenced later, still can't be freed. 
// Things we don't do, meaning keeping track of certain ref counts, aren't that important. For a request-based processor
// what we have is enough to be able to control memory even for the most demanding requests.
// Once a variable has been assigned from a process-scoped object (such as reading from hash, or assigned with set-string
// from process-scoped), it is also process-scoped; however those can never be deleted unless process-scoped delete takes
// place (such as for example delete hash, or delete-string on original process-scoped string). They are only references, so
// no new memory is used for letting them live during the request execution.
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
    if (check == 0) // this is while request is running, could be free from objects (like tree) or delete-string
                    // it's only for process-scoped
    { 
        if ((vm[r].status & GG_MEM_PROCESS))  // if process, reduce ref count, and if 0, strip process-scope
        {
            if (vm[r].ref > 0)  vm[r].ref--; 
            if (vm[r].ref == 0) vm[r].status &= ~GG_MEM_PROCESS; 
        }
        return;     // if not process (or not anymore), then nothing will be done, see above
    } else if (check == 3) // this is delete-string of non-process only
                                                               // even variable A from A=B (where B is process-scoped)
                                                               // cannot be deleted with this. A is just a reference to
                                                               // process-scoped B, and only B can be deleted
    {
        if (vm[r].ref > 0 || (vm[r].status & GG_MEM_PROCESS)) return; // delete-string with 3 cannot work if memory
                                                                      // if process-scoped or already referenced (even if
                                                                      // dereferenced later!)
    }  // else if (check != 2) return; // we could check, but there's only 0, 3 and 2, so if not zero or 3, then it's 2!
       // ADD THIS if we have more values for check!!!

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
// Frees all memory allocated so far. Called at the end of request.
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






