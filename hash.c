// SPDX-License-Identifier: Apache-2.0
// Copyright 2019 Gliim LLC. 
// Licensed under Apache License v2. See LICENSE file.
// On the web http://gliimly.github.io/ - this file is part of Gliimly framework.

// 
// Hash-table-related module
//

#include "gliim.h"


// prototypes
static gg_num gg_compute_hash (char* d, char **dlist, gg_num size);
static gg_hash_table *gg_new_hash_item (char *key, void *data);
void gg_hash_process (gg_hash_table *el, char *key, void *data);

//
// Create new hash hres_ptr. size is the size of hash table. The actual object is created here, the caller handlers pointer only.
// If process is true, then all memory allocated is process-scoped, and all key/data memory must be Gliimly memory.
// If in_h is provided, then gg_hash hres_ptr is not allocated, and neither is its gg_hash_table (which is set to in_h).
// Normally in_h is NULL; it's only non-NULL internally when building Gliimly internal hash values to find string values 
// quickly with just initialization of the table (see setup_reqhash() in v1.c).
//
void gg_create_hash (gg_hash **hres_ptr, gg_num size, gg_hash_table **in_h, bool process)
{
    GG_TRACE("");
    // get object
    if (in_h == NULL) 
    {
        *hres_ptr = (gg_hash*)gg_malloc(sizeof(gg_hash));
    }

    gg_hash *hres = *hres_ptr;

    if (size < 10) size = 10; // minimum size 10
    // create hash array of lists
    gg_hash_table **h;
    if (in_h == NULL) h = (gg_hash_table **)gg_calloc (size, sizeof(gg_hash_table*)); else h = in_h;

    // create top structure
    hres->num_buckets = size;
    hres->table = h; // set the table, each element is a linked list
    hres->tot = 0;
    hres->hits = 0;
    hres->reads = 0;
    hres->process = process;
    gg_rewind_hash(hres);// start from beginning when rewinding, in general index from which to keep dumping 

}

//
// Purges memory structure for hash h. Deletes the entire hash table. The actual object remains,
// so hash itself can be used again. 
// if del is used, then key/data is deleted (for all but resize-hash only)
//
void gg_delete_hash (gg_hash **h, bool del)
{
    GG_TRACE("");
    if (*h == NULL || (*h)->table == NULL) return; // object isn't created yet
    gg_num i;
    // loop through the table of linked lists
    for (i = 0; i < (*h)->num_buckets; i++) 
    { 
        // free all linked list elements
        gg_hash_table *hnext = (*h)->table[i];
        while (hnext != NULL)
        {
            gg_hash_table *tmp = hnext;
            hnext = hnext->next;
            if (del)
            {
                gg_free (tmp->key);
                gg_free (tmp->data);
            }
            gg_free (tmp);
        } 
    }
    gg_free ((*h)->table); // free table of linked lists
    if (del)
    {
        // save stats
        gg_num hits = (*h)->hits;
        gg_num reads = (*h)->reads;
        gg_num size = (*h)->num_buckets;
        bool process = (*h)->process;
        gg_free (*h); // delete old hash structure
        //Recreate from scratch to make sure all is properly initialized
        gg_create_hash (h, size, NULL, process); // create new hash
        // restore stats and process-scope
        (*h)->hits = hits;
        (*h)->reads = reads;
        (*h)->process = process;
    }
    else
    {
        gg_free (*h); // delete old hash structure
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
void gg_del_hash_entry (gg_hash *h, gg_hash_table *todel, gg_hash_table *prev, gg_num hashind, bool keydel)
{
    gg_hash_table *next = todel->next;
    if (prev == NULL) {
        // if bucket index is unknown, calculate it since we need it when deleting the first in the bucket
        // in order to set 'prev' which is the bucket itself (i.e. denoted as NULL)
        if (hashind == -1) hashind = gg_compute_hash (todel->key, NULL, h->num_buckets);
        h->table[hashind] = next; // if first in bucket list deleted
                                                // the next one is now the first
    }
    else // if there is a one before the one to delete
    {
        prev->next = next; // update previous to point to the one after the deleted entry
                           // which can be NULL
    }
    if (keydel) gg_free (todel->key); // delete key in hash, value can be deleted by user if desired
    gg_free (todel);
    // account for rewinding, if we just deleted the current element
    if (h->dcurr == todel)
    {
        h->dcurr = next; // set current to next, which can be NULL
    }

    h->tot--; // one element less
}

//
// Search / delete. Search hash 'h' for key 'key' and return data (or "" if not found).
// If keylist!=NULL, then iterate over keylist array until NULL.
// If 'del' is 1, delete this element and return data (data is only linked). 'found' is GG_OKAY
// if something was found (say if delete was done), or GG_ERR_EXIST if not (found is "" then).
//
void *gg_find_hash (gg_hash *h, char *key, char **keylist, bool del, gg_num *found)
{
    GG_TRACE("");

    h->hits++;

    char *ret = GG_EMPTY_STRING;
    // get hash id of a key
    gg_num hashind = gg_compute_hash (key, keylist, h->num_buckets);

    gg_hash_table *hresult = NULL;
    gg_hash_table *prev = NULL;
    gg_hash_table *curr = h->table[hashind];
    
    while (curr != NULL)
    {
        h->reads++;
        bool is_match;
        if (keylist == NULL) is_match = !strcmp (key, curr->key);
        else
        {
            // Check if all elements in the list combined are the same as key
            gg_num di = 0; // data index
            gg_num lel; // length of element in key list
            gg_num tot_len = 0; // current length compared in key
            char *el = keylist[di]; // element in key list
            if (el != NULL) 
            {
                is_match = true; // default, will set to false if not
                while (el != NULL) 
                {
                    // check if current element matches
                    lel = gg_mem_get_len(gg_mem_get_id(el));
                    if (memcmp (curr->key + tot_len, el, lel)) {is_match = false; break;}
                    // advance the position in key and get next element to check
                    tot_len += lel;
                    di++;
                    el = keylist[di];
                }
            } else is_match = false; // if first is NULL, no match
        }
        if (is_match)  { hresult = curr; break;}
        else
        {
            prev = curr;
            curr = curr->next;
        }
        
    }
    // if nothing here, not found, return NULL
    if (hresult == NULL) 
    { 
        if (found != NULL) {GG_ERR0; *found = GG_ERR_EXIST;}
        return GG_EMPTY_STRING; // no old data
    }

    // save key,data before deleting, to be returned if asked for
    ret = hresult->data;

    if (found != NULL) *found = GG_OKAY;
    if (del) 
    {
        gg_del_hash_entry (h, hresult, prev, hashind, true);
    }
    return ret; // old data
}

//
// Resize hash h based on newsize
//
void gg_resize_hash (gg_hash **h, gg_num newsize)
{
    GG_TRACE("");

    // temp hash
    gg_hash *nh = NULL;
    gg_create_hash (&nh, newsize, NULL, (*h)->process);

    // copy data from old to new one using fast rewind
    gg_rewind_hash (*h);
    gg_num st;
    while (1)
    {
        void *data;
        char *key = gg_next_hash (*h, &data, &st, false);
        if (st == GG_ERR_EXIST) break;
        gg_add_hash (nh, key, NULL, data, NULL, NULL);
    }
    gg_num hits = (*h)->hits;
    gg_num reads = (*h)->reads;
    gg_delete_hash (h, false); // remove old hash, it also creates new one with old size, but empty
    // copy back all the info about hash
    *h = nh;
    (*h)->hits = hits;
    (*h)->reads = reads;
}

//
// Rewind hash 'h' for getting all data.
//
void gg_rewind_hash(gg_hash *h)
{
    GG_TRACE("");
    h->dnext = 0;
    h->dcurr = h->table[h->dnext]; // can be NULL
    h->dprev = NULL;
}

//
// Return average number of searches to get data
// It is multipled by 100 and presented as number (long long)
//
gg_num gg_hash_reads (gg_hash *h)
{
    GG_TRACE("");
    if (h->hits == 0) return 0;
    return (gg_num)((100*h->reads)/(double)(h->hits));
}

//
// Return how many buckets are in hash h
//
gg_num gg_hash_size (gg_hash *h)
{
    GG_TRACE("");
    return h->num_buckets;
}

//
// Return how many elements are in hash h
//
gg_num gg_total_hash (gg_hash *h)
{
    GG_TRACE("");
    return h->tot;
}


//
// Get next hash item from hash 'h'. 'data' is the value, and returns key.
// Returns key or "" if no more (end of table) and st is set to GG_ERR_EXIST (st can be NULL).
// This goes from ->dnext and to the next element, traversing all lists.
// If del is true, then delete the read entry and position to the next.
// If del is false, h->dprev is the entry with key found
//
char *gg_next_hash(gg_hash *h, void **data, gg_num *st, bool del)
{
    GG_TRACE("");
    if (h->dnext == h->num_buckets) { if (st) *st = GG_ERR_EXIST; *data = GG_EMPTY_STRING; return GG_EMPTY_STRING; }
    if (h->dcurr == NULL)
    {
        h->dprev = NULL; // no previous if head of table
        while (1)
        {
            h->dnext++;
            if (h->dnext == h->num_buckets) { if (st) *st = GG_ERR_EXIST; *data = GG_EMPTY_STRING; return GG_EMPTY_STRING; }
            h->dcurr = h->table[h->dnext];
            if (h->dcurr != NULL) break;
        }
    }

    // get key and data
    *data = h->dcurr->data;
    char *key = h->dcurr->key;
    if (st) *st = GG_OKAY;
    if (del)
    {
        // must add reference here and not in v1.c because we delete it here, and so it would be gone
        // by the time caller gets to it
        // in this case current is bumped to next one, and previous remains the same
        gg_del_hash_entry (h, h->dcurr, h->dprev, h->dnext, false);
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
// Set key/data into el element of hash. process is true if hash is process-scoped.
//
void gg_hash_process (gg_hash_table *el, char *key, void *data)
{
    GG_TRACE("");
    //
    // Check if setting idempotent, i.e. with the same pointer, as that would otherwise
    // cause increasing ref count to the point where this memory would have to be deleted multiple
    // times to really be deleted.
    //
    if (gg_optmem) 
    {
        if (el->key != key) {gg_mem_add_ref(1, NULL, key); gg_mem_set_process (key, false);}
        if (el->data != data) {gg_mem_add_ref(1, NULL, data); gg_mem_set_process (data, false);}
    }
    else
    {
        gg_mem_set_process (key, false);
        gg_mem_set_process (data, false);
    }
}


//
// Create new linked list item from key and data. Return pointer to it. 
//
gg_hash_table *gg_new_hash_item (char *key, void *data)
{
    GG_TRACE("");
    // create new hash linked list item
    gg_hash_table *new = (gg_hash_table *)gg_malloc (sizeof (gg_hash_table));
    // set data and key
    gg_hash_process (new, key, data);
    // since new->data/key were just created, they didn't point anywhere so we don't decrease their reference
    new->data = data; 
    new->key = key; 
    //
    return new;
}

// 
// Add new string 'data' to hash 'h' with key 'key'. If keylist!=NULL, then key is all keys in keylist until NULL.
// If this key existed, old_data is old data (old_data can be NULL), and st is GG_OKAY is insert worked
// (st can be NULL)
//
void gg_add_hash (gg_hash *h, char *key, char **keylist, void *data, void **old_data, gg_num *st)
{
    GG_TRACE("");

    // compute hash id for key
    gg_num hashind = gg_compute_hash (key, keylist, h->num_buckets);
    

    if (h->table[hashind] == NULL) { // nothing here, just add first list element 
        h->table[hashind] = gg_new_hash_item (key, data);
        h->table[hashind]->next = NULL; // no one following yet
        if (st != NULL) *st = GG_OKAY;
        h->tot ++; // one more element
        // we no longer set old value to empty string if none is there, it's unchanged
        //if (old_data) *old_data = GG_EMPTY_STRING;  // no old data
        return;
    } 
    else 
    {
        // check if this key exists already, must be unique
        gg_hash_table *bucket = h->table[hashind];
        while (bucket != NULL)
        {
            if (!strcmp (key, bucket->key))
            {
                // match found, set new key/data to process if needed
                gg_hash_process (bucket, key, data);
                // delete old key/data
                if (old_data) *old_data = bucket->data; else gg_free(bucket->data);
                gg_free (bucket->key); // delete old key, see gg_hash_process()
                                          // for a full cycle of read/write
                // no increase of h->tot because this is replacement
                // assign new key data
                bucket->data = data;
                bucket->key = key; 
                // set status
                if (st != NULL) 
                {
                    if (old_data) *st = GG_INFO_EXIST; else *st = GG_OKAY;
                }
                return;
            } else bucket = bucket->next;
        }
        // save current head of the list
        gg_hash_table *el = h->table[hashind];
        // connect to old head of the list
        gg_hash_table *new =  gg_new_hash_item (key, data);
        new->next = el;
        // make it a new head of the list
        h->table[hashind] = new;
        if (st != NULL) *st = GG_OKAY;
        h->tot ++; // one more element
        // we no longer set old value to empty string if none is there, it's unchanged
        // if (old_data) *old_data = GG_EMPTY_STRING;  // no old data
        return;
    }
}


#define GG_FNVPRIME 16777619
#define GG_FNVOFFSETBASIS 2166136261
//
// Compute hash value for string d. Size of hash table is size.
// If dlist!=NULL, iterate over dlist array until NULL.
// Return hash.
// Based on FNV1a for 32 bit, which is in public domain (https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function)
// Based on http://www.isthe.com/chongo/tech/comp/fnv/index.html, this algorithm is not patented
// authors: fnvhash-mail@asthe.com
//
#define GG_FNVCOMP(h,c) h ^= (c); h *= GG_FNVPRIME;
inline gg_num gg_compute_hash (char* d, char **dlist, gg_num size)
{
    GG_TRACE("");
    uint32_t h = GG_FNVOFFSETBASIS;
    gg_num i;
    // either use single value d or the list of keys dlist
    if (dlist == NULL) 
    {
        for (i = 0 ; d[i] ; i++)
        {
            GG_FNVCOMP(h, d[i]);
        }
    }
    else
    {
        // Go through key fragments to compute hash
        gg_num dindex = 0;
        char *in;
        in = dlist[dindex];
        while (1)
        {
            if (in == NULL) break; // cut it short if key fragment is NULL
                                   // account if it's the first key fragment
                                   // this way
            for (i = 0 ; in[i] ; i++)
            {
                GG_FNVCOMP(h, in[i]);
            }
            dindex++;
            in = dlist[dindex]; 
        }
    }
    return (gg_num)(h % size);
}





