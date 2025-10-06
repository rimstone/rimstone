// SPDX-License-Identifier: Apache-2.0
// Copyright 2018-2025 Gliim LLC. 
// Licensed under Apache License v2. See LICENSE file.
// On the web http://rimstone-lang.com/ - this file is part of RimStone framework.

// 
// Hash-table-related module
//

#include "rim.h"


// prototypes
static inline rim_num rim_compute_hash (char* d, rim_num size);
static rim_hash_table *rim_new_hash_item (char *key, void *data);
static void rim_del_hash_entry (rim_hash *h, rim_hash_table *todel, rim_hash_table *prev, rim_num hashind, bool keydel);

//
// Create new hash hres_ptr. size is the size of hash table. The actual object is created here, the caller handlers pointer only.
// If process is true, then all memory allocated is process-scoped, and all key/data memory must be RimStone memory.
// If in_h is provided, then rim_hash hres_ptr is not allocated, and neither is its rim_hash_table (which is set to in_h).
// Normally in_h is NULL; it's only non-NULL internally when building RimStone internal hash values to find string values 
// quickly with just initialization of the table (see setup_reqhash() in v1.c).
//
void rim_create_hash (rim_hash **hres_ptr, rim_num size, rim_hash_table **in_h, unsigned char process)
{
    // get object
    if (in_h == NULL) 
    {
        *hres_ptr = (rim_hash*)rim_malloc(sizeof(rim_hash));
    }

    rim_hash *hres = *hres_ptr;

    if (size < 256) size = 256; // minimum size 256
    // create hash array of lists
    rim_hash_table **h;
    if (in_h == NULL) h = (rim_hash_table **)rim_calloc (size, sizeof(rim_hash_table*)); else h = in_h;

    // create top structure
    hres->num_buckets = size;
    hres->table = h; // set the table, each element is a linked list
    hres->tot = 0;
    hres->hits = 0;
    hres->reads = 0;
    hres->process = process;
    rim_rewind_hash(hres);// start from beginning when rewinding, in general index from which to keep dumping 

}

//
// Purges memory structure for hash h. Deletes the entire hash table. The actual object remains,
// so hash itself can be used again. 
// if del is used, then key/data is deleted (for all but resize-hash only)
//
void rim_delete_hash (rim_hash **h, bool del)
{
    if (*h == NULL || (*h)->table == NULL) return; // object isn't created yet
    rim_num i;
    // loop through the table of linked lists
    for (i = 0; i < (*h)->num_buckets; i++) 
    { 
        // free all linked list elements
        rim_hash_table *hnext = (*h)->table[i];
        while (hnext != NULL)
        {
            rim_hash_table *tmp = hnext;
            hnext = hnext->next;
            if (del)
            {
                rim_mem_dec_process (tmp->key);
                rim_mem_dec_process (tmp->data);
            }
            rim_mem_dec_process (tmp);
        } 
    }
    rim_mem_dec_process ((*h)->table); // free table of linked lists
    if (del)
    {
        // save stats
        rim_num hits = (*h)->hits;
        rim_num reads = (*h)->reads;
        rim_num size = (*h)->num_buckets;
        unsigned char process = (*h)->process;
        rim_mem_dec_process (*h); // delete old hash structure
        //Recreate from scratch to make sure all is properly initialized
        rim_create_hash (h, size, NULL, process); // create new hash
        // restore stats and process-scope
        (*h)->hits = hits;
        (*h)->reads = reads;
        (*h)->process = process;
    }
    else
    {
        rim_mem_dec_process (*h); // delete old hash structure
    }
}


//
// Delete hash entry. todel is the entry to delete, prev is the pointer to previous entry,
// which can be NULL. The actual data must be freed (if needed) separately.
// hashind is the bucket index; if -1 we will calculate it; this avoids double calculation.
// h is the hash itself. key is freed.
// hashind is the bucket index, or -1 if we need to compute it.
// This will delete the key, but not value.
// If keydel is true, delete key, otherwise don't (such as with traverse read when both key and value are obtained).
//
static void rim_del_hash_entry (rim_hash *h, rim_hash_table *todel, rim_hash_table *prev, rim_num hashind, bool keydel)
{
    rim_hash_table *next = todel->next;
    if (prev == NULL) {
        // if bucket index is unknown, calculate it since we need it when deleting the first in the bucket
        // in order to set 'prev' which is the bucket itself (i.e. denoted as NULL)
        if (hashind == -1) hashind = rim_compute_hash (todel->key, h->num_buckets);
        h->table[hashind] = next; // if first in bucket list deleted
                                                // the next one is now the first
    }
    else // if there is a one before the one to delete
    {
        prev->next = next; // update previous to point to the one after the deleted entry
                           // which can be NULL
    }
    if (keydel) 
    {
        rim_mem_dec_process (todel->key);
        rim_mem_dec_process (todel->data);
    }
    rim_mem_dec_process (todel);
    // account for rewinding, if we just deleted the current element
    if (h->dcurr == todel)
    {
        h->dcurr = next; // set current to next, which can be NULL
    }

    h->tot--; // one element less
}

//
// Search / delete. Search hash 'h' for key 'key' and return data (or "" if not found).
// If 'del' is 1, delete this element and return data (data is only linked). 'found' is RIM_OKAY
// if something was found (say if delete was done), or RIM_ERR_EXIST if not (found is "" then).
// If is_rim is true, the key/value are both rimstone memory; not so if false. This is here because hash is used for some internal purposes
// where memory is not RimStone, for instance to find input parameters, request names, file upload attributes etc. 
// This is faster than giving those a header; this function is always inlined, and is_rim is always a constant; hence gcc will
// perform dead-code elimination, effectively logically splitting this function into two: one that does RimStone safe-memory comparison (for 
// everything but the our internal purposes as above), and plain strcmp() for internal purposes.
//
RIM_ALWAYS_INLINE inline void *rim_find_hash (rim_hash *h, char *key, bool del, rim_num *found, bool is_rim)
{

    h->hits++;

    char *ret = RIM_EMPTY_STRING;
    // get hash id of a key
    rim_num hashind = rim_compute_hash (key, h->num_buckets);

    rim_hash_table *hresult = NULL;
    rim_hash_table *prev = NULL;
    rim_hash_table *curr = h->table[hashind];
    
    while (curr != NULL)
    {
        h->reads++;
        if (is_rim ? rim_cm_str(key, rim_mem_get_len(key), curr->key, rim_mem_get_len(curr->key)) :!strcmp (key, curr->key)) 
        { 
            hresult = curr;
            break;
        }
        else
        {
            prev = curr;
            curr = curr->next;
        }
        
    }
    // if nothing here, not found, return NULL
    if (hresult == NULL) 
    { 
        if (found != NULL) {RIM_ERR0; *found = RIM_ERR_EXIST;}
        return RIM_EMPTY_STRING; // no old data
    }

    // save key,data before deleting, to be returned if asked for
    ret = hresult->data;

    if (found != NULL) *found = RIM_OKAY;
    if (del) 
    {
        rim_del_hash_entry (h, hresult, prev, hashind, true);
    }
    return ret; // old data
}

//
// Resize hash h based on newsize
//
void rim_resize_hash (rim_hash **h, rim_num newsize)
{

    // temp hash
    rim_hash *nh = NULL;
    rim_create_hash (&nh, newsize, NULL, (*h)->process);

    // copy data from old to new one using fast rewind
    rim_rewind_hash (*h);
    rim_num st;
    while (1)
    {
        void *data;
        char *key = rim_next_hash (*h, &data, &st, false);
        if (st == RIM_ERR_EXIST) break;
        rim_add_hash (nh, key, data, NULL, NULL);
    }
    rim_num hits = (*h)->hits;
    rim_num reads = (*h)->reads;
    rim_delete_hash (h, false); // remove old hash, it also creates new one with old size, but empty
    // copy back all the info about hash
    *h = nh;
    (*h)->hits = hits;
    (*h)->reads = reads;
}

//
// Rewind hash 'h' for getting all data.
//
void rim_rewind_hash(rim_hash *h)
{
    h->dnext = 0;
    h->dcurr = h->table[h->dnext]; // can be NULL
    h->dprev = NULL;
}

//
// Return average number of searches to get data
// It is multipled by 100 and presented as number (long long)
//
rim_num rim_hash_reads (rim_hash *h)
{
    if (h->hits == 0) return 0;
    return (rim_num)((100*h->reads)/(double)(h->hits));
}

//
// Return how many buckets are in hash h
//
rim_num rim_hash_size (rim_hash *h)
{
    return h->num_buckets;
}

//
// Return how many elements are in hash h
//
rim_num rim_total_hash (rim_hash *h)
{
    return h->tot;
}


//
// Get next hash item from hash 'h'. 'data' is the value, and returns key.
// Returns key or "" if no more (end of table) and st is set to RIM_ERR_EXIST (st can be NULL).
// This goes from ->dnext and to the next element, traversing all lists.
// If del is true, then delete the read entry and position to the next.
// If del is false, h->dprev is the entry with key found
//
char *rim_next_hash(rim_hash *h, void **data, rim_num *st, bool del)
{
    if (h->dnext == h->num_buckets) { if (st) *st = RIM_ERR_EXIST; *data = RIM_EMPTY_STRING; return RIM_EMPTY_STRING; }
    if (h->dcurr == NULL)
    {
        h->dprev = NULL; // no previous if head of table
        while (1)
        {
            h->dnext++;
            if (h->dnext == h->num_buckets) { if (st) *st = RIM_ERR_EXIST; *data = RIM_EMPTY_STRING; return RIM_EMPTY_STRING; }
            h->dcurr = h->table[h->dnext];
            if (h->dcurr != NULL) break;
        }
    }

    // get key and data
    *data = h->dcurr->data;
    char *key = h->dcurr->key;
    if (st) *st = RIM_OKAY;
    if (del)
    {
        // must add reference here and not in v1.c because we delete it here, and so it would be gone
        // by the time caller gets to it
        // in this case current is bumped to next one, and previous remains the same
        rim_del_hash_entry (h, h->dcurr, h->dprev, h->dnext, false);
        return key; // return because deleting will position to next
    }
    else
    {
        // in this case previous is now what current was
        h->dprev = h->dcurr;
        h->dcurr = h->dcurr->next; // current now
        return key;
    }
}




//
// Create new linked list item from key and data. Return pointer to it. 
//
static rim_hash_table *rim_new_hash_item (char *key, void *data)
{
    // create new hash linked list item
    rim_hash_table *new = (rim_hash_table *)rim_malloc (sizeof (rim_hash_table));
    // set data and key
    rim_mem_add_ref (key, rim_mem_process);
    rim_mem_add_ref (data, rim_mem_process);
    // since new->data/key were just created, they didn't point anywhere so we don't decrease their reference
    new->data = data; 
    new->key = key; 
    //
    return new;
}

// 
// Add new string 'data' to hash 'h' with key 'key'. 
// If this key existed, old_data is old data (old_data can be NULL), and st is RIM_OKAY is insert worked
// (st can be NULL)
//
void rim_add_hash (rim_hash *h, char *key, void *data, void **old_data, rim_num *st)
{

    // compute hash id for key
    rim_num hashind = rim_compute_hash (key, h->num_buckets);
    

    if (h->table[hashind] == NULL) { // nothing here, just add first list element 
        h->table[hashind] = rim_new_hash_item (key, data);
        h->table[hashind]->next = NULL; // no one following yet
        if (st != NULL) *st = RIM_OKAY;
        h->tot ++; // one more element
        // we no longer set old value to empty string if none is there, it's unchanged
        //if (old_data) *old_data = RIM_EMPTY_STRING;  // no old data
        return;
    } 
    else 
    {
        // check if this key exists already, must be unique
        rim_hash_table *bucket = h->table[hashind];
        while (bucket != NULL)
        {
            if (!strcmp (key, bucket->key))
            {
                // match found, set new key/data to process if needed
                rim_mem_add_ref (key, rim_mem_process);
                rim_mem_add_ref (data, rim_mem_process);
                // delete old key/data
                if (old_data) 
                {
                    *old_data = bucket->data; 
                } else rim_mem_dec_process (bucket->data);
                rim_mem_dec_process (bucket->key); // delete old key, 
                // no increase of h->tot because this is replacement
                // assign new key data
                bucket->data = data;
                bucket->key = key; 
                // set status
                if (st != NULL) 
                {
                    if (old_data) *st = RIM_INFO_EXIST; else *st = RIM_OKAY;
                }
                return;
            } else bucket = bucket->next;
        }
        // save current head of the list
        rim_hash_table *el = h->table[hashind];
        // connect to old head of the list
        rim_hash_table *new =  rim_new_hash_item (key, data);
        new->next = el;
        // make it a new head of the list
        h->table[hashind] = new;
        if (st != NULL) *st = RIM_OKAY;
        h->tot ++; // one more element
        // we no longer set old value to empty string if none is there, it's unchanged
        // if (old_data) *old_data = RIM_EMPTY_STRING;  // no old data
        return;
    }
}


#define RIM_FNVPRIME 16777619
#define RIM_FNVOFFSETBASIS 2166136261
//
// Compute hash value for string d. Size of hash table is size.
// Return hash.
// Based on FNV1a for 32 bit, which is in public domain (https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function)
// Based on http://www.isthe.com/chongo/tech/comp/fnv/index.html, this algorithm is not patented
// authors: fnvhash-mail@asthe.com
//
#define RIM_FNVCOMP(h,c) h ^= (c); h *= RIM_FNVPRIME;
RIM_ALWAYS_INLINE inline static rim_num rim_compute_hash (char* d, rim_num size)
{
    uint32_t h = RIM_FNVOFFSETBASIS;
    rim_num i;
    for (i = 0 ; d[i] ; i++)
    {
        RIM_FNVCOMP(h, d[i]);
    }
    return (rim_num)(h % size);
}


//
//
//Get value of hash element (returned value) based on key or RIM_EMPTY_STRING when not found
//This is used for a shortcut []
//This is for *regular* hash only. If ever changes, change 'true' below for last param in rim_find_hash
//
char *rim_hash_get (rim_hash *hash, char *key)
{
    rim_num st;
    char *ret = rim_find_hash (hash, key, false, &st, true); // memory being compared is always RimStone
    if (st == RIM_OKAY) 
    {
        return ret;
    }
    else return RIM_EMPTY_STRING;
}
