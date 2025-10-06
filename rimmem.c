// SPDX-License-Identifier: Apache-2.0
// Copyright 2018-2025 Gliim LLC. 
// Licensed under Apache License v2. See LICENSE file.
// On the web http://rimstone-lang.com/ - this file is part of RimStone framework. 

// 
// Memory handling
// RIM memory is made for request-based processing. 
// Memory can be ordinary or process-scoped (PS). Ordinary memory is always automatically released at the end of the request, and you 
// don't have to release it. You can still do that immediately with delete-string, but only if memory is not referenced by another variable or by an object
// like a tree, hash etc. If it is, it will be released at the end of the request. This eliminates excessive reference count updating, which can slow down
// execution 20% or more. So ordinary memory's ref counting is really true or false (either referenced or not) when it comes to whether delete-string
// will work right away.
// PS memory is not released at the end of the request. It is used to create server applications, that keep data for the life of the process.
// PS memory has a reference counter attached to it, and it is released when no other PS memory refers to it. For instance, you can have a string
// in a tree and a hash. The element holding it must be deleted in both tree and hash in order to be truly deleted. Even then, such memory is only
// deleted at the end of the request.
// Ordinary memory is linked into a list that uses the same 'next' member as free memory. This list allows for very fast disposal of memory at the end
// of the request, since PS memory isn't checked; this means process could hold millions of PS memory items and not be slowed down a bit from them.
// Memory in RimStone is assigned by reference. RimStone's memory safety system is minimal for performance reason and it uses the fact that requests are generally
// quick and memory released at the of the request is likely to be faster and cause less memory fragmentation than otherwise. 
// Memory is checked for boundaries. This is however done only at the input of the statement, and the actual implementation doesn't do this (since
// it's carefully crafted for memory safety), improving performance greatly.
//
//


#include "rim.h"

unsigned char rim_mem_process = 0; // if RIM_MEM_PROCESS bit is set, any allocs are process-scoped. Memory is not released with request's end, but when
                             // reference count goes down to zero. Process memory.

// functions
inline static bool rim_mem_get_process (rim_num r);
inline static void rim_mem_add_ord (rim_num r);
inline static rim_num rim_mem_del_ord (rim_num r);

char *rim_mem_msg_outmem = "Out of memory for [%ld] bytes";

// Common empty string constant, used for initialization
// When a variable has this value, it means it is freshly initialized and it
// will be either re-assigned, or allocated. (value assigned later)
// so in the program it's never actually NULL, always ""
char _RIM_EMPTY_STRING[1+16]="                " ""; // this allows [0] to be assigned 0
char *RIM_EMPTY_STRING = NULL; // initial value, so that origin adding of empty string via set_process succeeds


// free block
#define RIM_MEM_FREE 1
// open file
#define RIM_MEM_FILE 2



// static variables used in memory handling
// We delete old's request memory at the very start of a new request in generated code before any user code,
// (unless process scoped memory)
//
vml *vm = NULL; // memory list
rim_num vm_curr = 0; // last used-up index in memory list
static rim_num vm_tot = 0; // total size of memory list
static rim_num vm_first_free = -1; // first free memory block
static rim_num vm_first_ord = -1; // first ordinary memory block


// determines the size of the block allocated (and the size of consequent expansions) for the memory
// block that keeps all pointers to allocated blocks.
#define RIMMSIZE 512

//
// Handles when reference count is decreased for a process-scope string, which is ID of r. 
// This is used in lists, arrays, hashes and trees, which can be process-scoped; the data doesn't have to 
// process scoped.
//
RIM_ALWAYS_INLINE inline void rim_mem_dec_process (void *s)
{
    rim_num r = rim_mem_get_id (s);
    if (r != -1 && (vm[r].status & RIM_MEM_PROCESS))
    {
        if (vm[r].ref >0) vm[r].ref--;
        vm[r].ref == 0 ? ( vm[r].status &= ~RIM_MEM_PROCESS, rim_mem_add_ord(r), vm[r].ref = 1, true): true; // means don't delete until the end of request
    }
}


//
// Set status of memory, s is the status such as RIM_MEM_FILE
// ind is the index into vm[], ind cannot be -1.
//
RIM_ALWAYS_INLINE inline void rim_mem_set_status (rim_num ind, unsigned char s)
{
    if (ind != -1) vm[ind].status |= s;
}


//
// This adds reference to string s. mem_process is 0 if s will not be transformed to process-scoped; or <>0 if
// s will be transformed to process-scoped.
//
RIM_ALWAYS_INLINE inline void rim_mem_add_ref (char *s, unsigned char mem_process)
{
    rim_num ind = rim_mem_get_id (s);
    if (ind != -1) 
    {
        if (!mem_process)
        {
            // if object we're assigning this memory isn't process-scoped, then only ordinary memory ref is set to 1
            // process-scoped memory isn't affected by being assigned to say ordinary list
            if (!(vm[ind].status & RIM_MEM_PROCESS)) vm[ind].ref=1;
        } else 
        {
            // this is if assigning to process-scoped list, tree etc
            // if memory isn't process, make it process-scoped
            !(vm[ind].status & RIM_MEM_PROCESS) ?  (ind = rim_mem_del_ord (ind), vm[ind].status |= RIM_MEM_PROCESS, vm[ind].ref = 1, true): ((__builtin_expect (vm[ind].ref++==RIM_MAX_REF,0)?rim_report_error ("Too many process-scoped memory references, use constants or make copies instead or consider changing the logic"):true), true); // if it is process-scoped, increase its ref count
        }
    }
}




// 
// must be called at the very end of program. 
// cleans up any memory 
//
void rim_memory_init ()
{
    static bool first=true;
    if (first)
    {
        // add empty string, so it's a regular string, no need for gimmicks of -1 etc as a special case, overall speeds up everything in memory access
        // this is done just once, and never released
        // RIM_EMPTY_STRING is *not* in vm[], it's a string constant that remains constant across the process
        RIM_EMPTY_STRING = rim_mem_add_const (_RIM_EMPTY_STRING, sizeof(_RIM_EMPTY_STRING)-RIM_ALIGN);
        first = false;
    }
    rim_done ();
}

//
// Get alloc'd size of vm[]
//
inline rim_num rim_mem_size ()
{
    return vm_tot;
}



// 
// Add point to the block of memory. 'p' is the memory pointer (allocated elsewhere here) added.
// Returns the index in memory block where the pointer is.
// Once a block of pointers is exhausted, add another block. We do not increase the blocks
// size requests are generally small and typically do not need much more memory, and increasing
// the block size might cause swaping elsewhere.
//
RIM_ALWAYS_INLINE inline rim_num rim_add_mem (void *p)
{
    rim_num r;
    if (vm_first_free != -1)
    {
        // find next free
        r = vm_first_free;
        vm_first_free = vm[vm_first_free].next; // this block if free for sure b/c vm_first_free points to it
                                                     // next is -1 if there's no next, and -1 is vm_first_free when none
    } else {
        // there's no free, get more memory
        r = vm_curr;
        vm_curr++;
        if (vm_curr >= vm_tot)
        {
            if (__builtin_expect ((vm_tot += RIMMSIZE) >= RIM_MAX_MEM-1, 0)) rim_report_error ("Too many variables in the program");
            vm = realloc (vm, vm_tot * sizeof (vml)); // this also creates first time memory
            if (__builtin_expect (vm == NULL, 0))
            {
                rim_report_error (rim_mem_msg_outmem, vm_tot*sizeof(vml));
            }
        }
    }
    vm[r].ptr = p;
    // .len must be set always set by caller
    vm[r].status = 0; // not a freed block, not process, cannot delete, not a file
    vm[r].ref = 0;
    // this checking of rim_mem_process works for both process and const memory
    rim_mem_process ? (vm[r].status |= rim_mem_process, true): (rim_mem_add_ord (r), true);
    return r;
}

//
// Internal RimStone delete that always deletes memory x. Used only in code where we have 100% control over the data flow.
//
RIM_ALWAYS_INLINE inline void rim_free_int(void *x) 
{
    rim_num id;
    x!=NULL? (id = rim_mem_get_id(x), id!=-1 ? (id=rim_mem_del_ord (id),rim_mem_release(id),free ((unsigned char*)(x)-RIM_ALIGN),true):true):true;
}

//
// Delete memory r from list of ordinary memory
// Since we don't keep prev to save memory, we swap the memory to delete with the first one in the list
// and then delete the first one. Given this doesn't happen often, and in many cases ever, this is the cheapest
// way to do this both in terms of performance and memory.
// Returns new location where r was (in case we're deleting it too).
// r cannot be -1
//
RIM_ALWAYS_INLINE inline static rim_num rim_mem_del_ord (rim_num r)
{
    rim_num first_ord_next = vm[vm_first_ord].next; // save next ordinary memory, either real mem id or -1
    if (__builtin_expect (r != vm_first_ord, 1))
    {
        // swap vm[] entries
        register vml temp = vm[r];
        vm[r] = vm[vm_first_ord];
        vm[vm_first_ord] = temp;
        // swap -> next 
        register rim_num tnext= vm[r].next;
        vm[r].next = vm[vm_first_ord].next;
        vm[vm_first_ord].next = tnext;
        // swap ids in memory
        register rim_num tid = ((rim_head*)(vm[r].ptr))->id;
        ((rim_head*)(vm[r].ptr))->id = ((rim_head*)(vm[vm_first_ord].ptr))->id;
        ((rim_head*)(vm[vm_first_ord].ptr))->id = tid;
        //
        r = vm_first_ord;
    }  
    vm_first_ord = first_ord_next;
    return r;
}

//
// Add memory r to list of ordinary memory
// r cannot be -1
//
RIM_ALWAYS_INLINE inline static void rim_mem_add_ord (rim_num r)
{
    __builtin_expect (vm_first_ord == -1,0) ?  (vm_first_ord = r, vm[r].next = -1, true): (vm[r].next = vm_first_ord, vm_first_ord = r, true);
}


// 
// Makes a string constant (that's not static allocation though) out of ordinary memory; for that reason such "constant"
// isn't persistent for the process, just a request. It's not in vm[], but whatever memory it's off of, its fate is tied to that memory.
// p is mem-RIM_ALIGN, len is the length at mem (including \0, so it's <useful len>+1).
//
RIM_ALWAYS_INLINE inline void *rim_mem_add_const (void *p, rim_num len)
{
    rim_head *h = (rim_head*)p;
    h->id = -1;
    h->len = len;
    return p+RIM_ALIGN;

}

// 
// Adds pointer to our block of memory. 
// 'r' is the index in vm[].
// The memory returned is the actually a pointer to useful memory (that a RIM program can use). We place
// some information at the beginning of the memory pointed to by alloc'd mem: the reference to the 
// index in the block of memory where memory (p) is;
//
RIM_ALWAYS_INLINE inline void *rim_vmset (void *p, rim_num r)
{
    // Copy ID just before memory
    ((rim_head*)p)->id = r;
    return (unsigned char*)p + RIM_ALIGN;
}


// 
// input and returns are like malloc().
//
RIM_ALWAYS_INLINE inline void *rim_malloc(size_t size)
{
    void *p = malloc (size + RIM_ALIGN);
    if (__builtin_expect (p == NULL,0)) 
    {
        rim_report_error (rim_mem_msg_outmem, size+RIM_ALIGN);
    }
    // add memory pointer to memory block
    rim_num r = rim_add_mem (p);
    void *pm = rim_vmset(p,r);
    rim_mem_set_len (pm, size);
    return pm;
}

// 
// input and returns are like calloc()
// See malloc for the rest
//
RIM_ALWAYS_INLINE inline void *rim_calloc(size_t nmemb, size_t size)
{
    size_t t = nmemb*size;
    void *p =  calloc (1, t + RIM_ALIGN);
    if (__builtin_expect (p == NULL, 0)) 
    {
        rim_report_error (rim_mem_msg_outmem, t);
    }
    rim_num r = rim_add_mem (p);
    void *pm = rim_vmset(p,r);
    rim_mem_set_len (pm, t);
    return pm;
}


//
// Free block, Link new freed memory block r
//
RIM_ALWAYS_INLINE inline void rim_mem_release(rim_num r)
{
    vm[r].ptr = NULL;
    vm[r].status = RIM_MEM_FREE; // freed, all other flags cleared
    // Set memory marked as freed, would be -1 if there's no free blocks at this moment
    vm[r].next = vm_first_free;
    vm_first_free = r;
}



// 
//
// Get if memory is process scoped, true or false, r is index into vm[]
//
RIM_ALWAYS_INLINE inline static bool rim_mem_get_process (rim_num r)
{
    return r == -1 ? true:vm[r].status & RIM_MEM_PROCESS;
}

// 
// Input and return the same as for realloc()
// Checks memory to make sure it's valid block allocated here.
// RimStone will never realloc's existing variable, b/c RimStone variables reference each other
// directly, meaning there's no indirection (first to a structure, then pointer) for performance.
// So this is only for internally created variable, within a statement.
//
RIM_ALWAYS_INLINE inline void *rim_realloc(rim_num r, size_t size)
{
    if (r == -1) 
    {
        // this is if we realloc RIM_EMPTY_STRING; all string constants use -1 as ID, and it would be
        // bad if we passed one of those to here. BUT, this is meant *ONLY* for RIM_EMPTY_STRING
        // RimStone implementation should *never* realloc string constants as they can't be realloced (obviously, they are static).
        // This should never happen because RimStone NEVER reuses existing memory. The only time we do realloc is for 
        // internal results, starting from a newly created internal variable, never from existing RimStone variable!!
        // Otherwise, if we realloced existing RimStone variables, anything pointing to those would point to now-invalid memory!!!
        return rim_malloc (size); // since it's RIM_EMPTY_STRING, nothing to initialize.
    }
    //
    // Check if string uninitialized, if so, allocate it for the first time
    // Also, if pointer is a NULL ptr, just allocate memory.
    //

    bool is_process = rim_mem_get_process (r);

    // first realloc pointer
    void *p= realloc ((unsigned char*)(vm[r].ptr), size + RIM_ALIGN);
    if (p == NULL) 
    {
        rim_report_error (rim_mem_msg_outmem, size+RIM_ALIGN);
    }
    void *pm;
    // If pointer returned is the same, there's no need to delete it, just change the length
    if (p != vm[r].ptr)
    {
        // 
        // What's done here must match what's done in _rim_free, i.e. rim_mem_del_ord, rim_mem_release
        // and then what's done in rim_malloc, i.e. rim_add_mem, rim_vmset
        // finally always set new length with rim_mem_set_len
        //
        // then nullify pointer and release vm[] entry
        vm[r].ptr = p; // otherwise rim_mem_del_ord/rim_mem_release would operate on invalid memory
        rim_num old_ref = vm[r].ref;
        if (!(vm[r].status & RIM_MEM_PROCESS)) { r = rim_mem_del_ord (r);}
        rim_mem_release(r);
        // then add new vm[] entry
        r = rim_add_mem(p);
        pm = rim_vmset(p,r);
        if (is_process) rim_mem_set_status (r, RIM_MEM_PROCESS);
        vm[r].ref = old_ref;
    } else pm = vm[r].ptr + RIM_ALIGN;
    rim_mem_set_len (pm, size);
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
RIM_ALWAYS_INLINE inline void _rim_free (void *ptr)
{
    //
    // RIM_EMPTY_STRING is now a regular constant, so no need to check if freed

    //
    // Any point forward code can sigegv in this function or return error to indicate memory is bad
    //
    rim_num r = rim_mem_get_id(ptr); // index in the list of memory blocks
    if (r == -1) return;

#ifdef DEBUG
    if (r < 0 || r >= vm_curr) rim_report_error ("Memory is not within valid virtual range [%ld]", r);
    if (vm[r].ptr != ((unsigned char*)ptr-RIM_ALIGN)) rim_report_error ("Memory header is invalid");
#endif
    if (vm[r].status & RIM_MEM_PROCESS) return; 
    else if (vm[r].ref > 0) return;
#ifdef DEBUG
    //
    // if memory on the list of freed blocks, just ignore it, otherwise double free or free
    // of an invalid pointer ensues. Should not happen though because when we free a string we 1) for ordinary
    // don't actually delete it during request run (and at the end it can't happen since we go in a loop straight), 2) for process
    // ones, we keep reference count.
    //
    if (__builtin_expect (vm[r].status & RIM_MEM_FREE, 0)) return;
    // check number of references, this must be *before* rim_mem_del_ord() below, since that will change the header of 
    // this memory, and it will be invalid!
    // for debugging, make the freed memory empty to aid in testing of freeing, since often it remains the same
    // and freed memory always has at least one byte
    memset (ptr, 0, rim_mem_get_len (ptr)); // .len is at least 1
#endif
    r = rim_mem_del_ord (r); // no need to mess with ordinary memory list, since it will be entirely
                                            // deleted in rim_done, and we'll just set it to empty there
                                            // we can delete from list of ordinary, because ONLY ordinary memory is deleted here

    // free mem
    free ((unsigned char*)ptr-RIM_ALIGN);
    // release vm[]
    rim_mem_release(r);
    //
    return;
}

// 
// 
// Like strdup below, but copies starting from byte 'from' (0 is first), through byte 'l' inclusive.
// Since s does not have to be RimStone mem, cannot and does not check 'l' is not beyond the length of 's', that MUST be correct going in.
// We do check if from is negative or greater than 'l' though, so if l is known to be lesser or equal than length of 's', then no additional checks are needed.
//
RIM_ALWAYS_INLINE inline char *rim_strdupl (char *s, rim_num from, rim_num l)
{
   if (from < 0 || from > l)  rim_report_error ("Cannot copy from byte [%ld] when length is [%ld], or byte position is negative", from, l);
   char *n = (char*)rim_malloc (l+1-from);
   rim_mem_set_len(n, l-from+1);
   if (n == NULL) 
   {
       rim_report_error (rim_mem_msg_outmem, l+1-from);
   }
   memcpy (n, s+from, l+1-from);
   return n;
}

// 
// Input and return the same as for strdup()
// Strdup guaranteed set memory length
// Since s does not have to be RimStone mem, cannot and does not check boundaries of 's', those MUST be correct, i.e. strlen(s) CANNOT go to bad memory
//
RIM_ALWAYS_INLINE inline char *rim_strdup (char *s)
{
   if (s == RIM_EMPTY_STRING) return RIM_EMPTY_STRING;
   rim_num l = strlen(s);
   char *n = (char*)rim_malloc (l+1);
   rim_mem_set_len(n, l+1);
   if (n == NULL) 
   {
       rim_report_error (rim_mem_msg_outmem, l+1);
   }
   memcpy (n, s, l+1);
   return n;
}

// 
// Frees all memory allocated so far. Called at the end of request.
//
RIM_ALWAYS_INLINE inline void rim_done ()
{
    FILE **f;
    if (vm != NULL)
    {
        rim_num i=vm_first_ord;
        while (i != -1)
        {
            rim_num j = vm[i].next; // get .next here, because rim_mem_release() below will change it
            (__builtin_expect (!(vm[i].status & RIM_MEM_FILE), 1)) ? (free ((unsigned char*)vm[i].ptr), rim_mem_release(i), true) : (f = (FILE**)(vm[i].ptr), *f != NULL ? (fclose (*f), *f=NULL, true): true, true); // make sure it's NULL so the following request can use this file descriptor
            i = j;
        }
    }
    // In order to test if all memory is released, change DEBUG0 to DEBUG below. It's not done normally since
    // it makes everything very slow.
#ifdef DEBUG0
    {
    int i;
    for (i = 0; i < vm_curr; i++)
    {
        if (!(vm[i].status & RIM_MEM_FREE) && !(vm[i].status & RIM_MEM_PROCESS)) rim_report_error ("Memory not freed [%d]", i);
    }
    }
#endif
    // The following would be a part of compacting memory (a future maybe-feature)
    /*if (vm == NULL) 
    {
        // init memory if it's not there. Keep it if it has process-memory, but release all others
        vm = calloc (vm_tot = RIMMSIZE, sizeof (vml));
        if (vm == NULL) rim_report_error ("Out of memory");
        vm_curr = 0;
        vm_first_free = -1;
    }*/
    vm_first_ord = -1;
}






