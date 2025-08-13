// SPDX-License-Identifier: Apache-2.0
// Copyright 2018-2025 Gliim LLC. 
// Licensed under Apache License v2. See LICENSE file.
// On the web http://golf-lang.com/ - this file is part of Golf framework.

// 
// Tree implementation, in-memory modified AVL with optional double-linked sorted list. 
// Around 50% faster than in-memory B-tree variations.
//

#include "golf.h"


// connect parent (p) to child (r) given the direction from the tree node (dir)
#define GG_TREE_SET_PARENT(p, dir, r) if (dir == GG_TREE_LESSER) (p)->lesser_node=(r); else (p)->greater_node=(r)

gg_tree_cursor *gg_cursor; // internal cursor for the current tree operation

// Function prototypes for the implementation
static inline void gg_tree_rotate_left (gg_tree_node *parent_tree, int dir, gg_tree_node *tree);
static inline void gg_tree_rotate_right (gg_tree_node *parent_tree, int dir, gg_tree_node *tree);
static void gg_tree_insert(gg_tree_node *parent_tree, int dir, gg_tree_node *tree, void *data);
static inline int gg_tree_compare(char *k2);
static void gg_tree_search (gg_tree_node *tree);
static inline void gg_tree_height (gg_tree_node *tree, gg_num *factor);
static inline void gg_tree_balance (gg_tree_node *parent_tree, int dir, gg_tree_node *tree);
static void gg_tree_delete (gg_tree_node *parent_tree, int dir, gg_tree_node *tree);
static void gg_tree_find_leaf_del (gg_tree_node *parent_tree, int dir, gg_tree_node *tree_greater_node, gg_tree_node *found);
static void gg_tree_search_lesser_equal (gg_tree_node *tree, bool equal);
static void gg_tree_search_greater_equal (gg_tree_node *tree, bool equal);
static gg_tree_node *gg_tree_node_create(char sorted);
static void gg_tree_node_delete(gg_tree_node *tree);

// measuring tree hops (i.e. the cost to search), only for debug build
#ifdef DEBUG
#define GG_TREE_HOPS    gg_cursor->root->hops++,
#else
#define GG_TREE_HOPS    
#endif

// Evaluate the tree node key k2 with another fixed key obtained from gg_cursor->key, 
// i.e. compare fixed key<=>current_tree_node_key. Uses custom eval function if specified
#define GG_TREE_EVAL(k2) (GG_TREE_HOPS gg_tree_compare(k2))

// Default key evaluation function. Compares current tree node key with gg_cursor->key. Return -1, 0, or 1 if node-key lesser, equal or greater than given key.
// Note: POSIX actually specifies that strncmp() works if the length compared is greater than the length of one of the strings (memcmp doesn't do that)
// gg_cursor holds the key and key_len members, which are precomputed. k2 is the tree node key.
static inline int gg_tree_compare(char *k2)
{
    if (gg_cursor->root->key_type == GG_TREE_TYPE_NUM)
    {
        // key-as positive-integer, compare as numbers written as strings, much faster than atol()
        // works in any base
        gg_num l1 = gg_cursor->key_len;
        gg_num l2 = gg_mem_get_len(k2);
        if (l1<l2) return -1;
        if (l1>l2) return 1;
        return memcmp(gg_cursor->key,k2,l1);
    }
    else
    {
        // compare as classic strings, C collation
        gg_num l1 = gg_cursor->key_len;
        gg_num l2 = gg_mem_get_len(k2);
        gg_num l = MIN(l1, l2)+1;
        return memcmp (gg_cursor->key,k2, l);
    }
}

//
// Delete tree node. Actual key and data must be obtained prior and deleted if needed. tree is the node to delete.
//
static void gg_tree_node_delete(gg_tree_node *tree)
{
    // tree node deletion is always based on the exact key, meaning you can't say delete the node 'greater than X', you must first
    // have the exact key being deleted. Since the key being deleted is already known, there's no point in keeping the key, so we delete it.
    gg_mem_dec_process (tree->key); 
    gg_mem_dec_process (tree);
}

//
// Create tree node. Allocated pointers for linked list if sorted is 1. Returns the node.
//
static gg_tree_node *gg_tree_node_create(char sorted)
{
    gg_tree_node *res;
    res = gg_calloc (1, sizeof(gg_tree_node) + (sorted==1 ? 2*sizeof (gg_tree_node *):0));
    return res;
}


//
// Create root node of the tree. 
// res is the tree itself, sorted is true/false based on unsorted clause in new-tree.
//
void gg_tree_create_root (gg_tree *res, bool sorted) 
{
    res->root_node = gg_tree_node_create(sorted?1:0); // never used directly, only reference as tree->lesser, this is the actual tree root
    res->tree->lesser_node = res->root_node; // GG_TREE_LESSER must always be used with root reference because of this assignment
}

//
// Create the tree itself. key_type is for default eval function (number comparison)
// sorted is true if there's linked list for fast range access. Returns the tree.
//
gg_tree *gg_tree_create(char key_type, bool sorted, unsigned char process)
{
    gg_tree *res = gg_calloc (1, sizeof(gg_tree) + (sorted?2*sizeof (gg_tree_node *):0));
    res->process = process;
    res->sorted = sorted; // must be set before gg_tree_node_create() below
    res->key_type = key_type;
    // Tree has a dummy node, which has a lesser pointer that points to an actual root of the tree
    // This is so all recursive algorithms work faster without handling exceptions
    res->tree = gg_tree_node_create (sorted?1:0);  //->tree is a dummy node sitting on top of the actual root
    gg_tree_create_root (res, sorted);
    return res;
}

//
// Get the height of a node tree. factor is the difference between left and right (if not NULL).
// Node's ->height member is updated by doing this. 
// A node without any children has a height of 1, with at least one child it's 2 etc.
//
static inline void gg_tree_height (gg_tree_node *tree, gg_num *factor)
{
    gg_num left_height;
    gg_num right_height;
    if (tree->lesser_node == NULL) left_height = 0; else left_height = tree->lesser_node->height;
    if (tree->greater_node == NULL) right_height = 0; else right_height = tree->greater_node->height;
    if (factor) *factor = left_height - right_height; // instead of right-left in classic AVL
    if (left_height > right_height) tree->height = left_height+1; else tree->height = right_height+1;
}


//
// Rotate the node to the right. parent_tree is the parent of the node being rotated, dir is the direction from parent
// to the node being rotated, which is tree.
//
static inline void gg_tree_rotate_right (gg_tree_node *parent_tree, int dir, gg_tree_node *tree)
{
    // rotate right
    // save data to use when shuffling below. Old:
    //            T
    //       L         G
    //   LL     LG
    //
    // New:
    //            L 
    //      LL            T
    //                LG     G
    //
    // balance as above
    GG_TREE_SET_PARENT(parent_tree, dir, tree->lesser_node);
    gg_tree_node *t = tree->lesser_node->greater_node;
    tree->lesser_node->greater_node = tree;
    tree->lesser_node = t;
    gg_tree_height (tree, NULL);
    gg_tree_height (parent_tree, NULL);
}

//
// Rotate the node to the left. parent_tree is the parent of the node being rotated, dir is the direction from parent
// to the node being rotated, which is tree.
//
static inline void gg_tree_rotate_left (gg_tree_node *parent_tree, int dir, gg_tree_node *tree)
{
    // rotate left 
    // save data to use when shuffling below. Old:
    //            T
    //       L         G
    //            GL     GG
    //
    // New:
    //            G 
    //       T        GG
    //   L      GL
    //
    // balance as above
    GG_TREE_SET_PARENT(parent_tree, dir, tree->greater_node);
    gg_tree_node *t = tree->greater_node->lesser_node;
    tree->greater_node->lesser_node = tree;
    tree->greater_node = t;
    gg_tree_height (tree, NULL);
    gg_tree_height (parent_tree, NULL);
}

//
// Balance 'tree', with parent parent_tree coming to 'tree' by dir direction (left/lesser or right/greater)
//
static inline void gg_tree_balance (gg_tree_node *parent_tree, int dir, gg_tree_node *tree)
{
    // get the balance factor of the node to balance
    gg_num bal_factor;
    gg_tree_height (tree, &bal_factor);
    //printf("Left %d Right %d\n", tree->lesser_node?tree->lesser_node->height:0, tree->greater_node?tree->greater_node->height:0);
    //assert (-2<=bal_factor && bal_factor<=2);
    
    //Balance factor can be within -2 and 2 inclusive here. The goal is to bring it to 0 or 1/-1.
    if (bal_factor >= 2)
    {
        // This needs balancing to the right, since left is taller. However, if the right subbranch of the left branch is taller than the left subbranch,
        // the right rotation will just make the left-mirror-image of the same problem. Thus, we need to left-rotate the right subbranch of the left branch first.
        if (tree->lesser_node != NULL)
        {
            if ((tree->lesser_node->greater_node ? tree->lesser_node->greater_node->height:0) > (tree->lesser_node->lesser_node?tree->lesser_node->lesser_node->height:0)) gg_tree_rotate_left (tree, GG_TREE_LESSER, tree->lesser_node);
        }
        gg_tree_rotate_right (parent_tree, dir, tree);
        // Recalculate the height of the node rotated, as well as its parent
        gg_tree_height (tree, NULL);
    }
    else if (bal_factor <= -2)
    {
        // This needs balancing to the left, since rigtt is taller. However, if the left subbranch of the right branch is taller than the right subbranch,
        // the leftt rotation will just make the right-mirror-image of the same problem. Thus, we need to right-rotate the left subbranch of the right branch first.
        if (tree->greater_node != NULL)
        {
            if ((tree->greater_node->lesser_node ? tree->greater_node->lesser_node->height:0) > (tree->greater_node->greater_node?tree->greater_node->greater_node->height:0)) gg_tree_rotate_right (tree, GG_TREE_GREATER, tree->greater_node);
        }
        gg_tree_rotate_left (parent_tree, dir, tree);
        // Recalculate the height of the node rotated, as well as its parent
        gg_tree_height (tree, NULL);
    }
    //num bf;
    //gg_tree_height (tree, &bf);
    //assert (bf<=1 && bf>=-1);
}

//
// Insert data with gg_cursor/key/key_len into tree. tree is the node considered, parent_tree/dir is
// its parent and the direction to reach tree (lesser/greater).
// gg_cursor->current/status set.
//
static void gg_tree_insert(gg_tree_node *parent_tree, int dir, gg_tree_node *tree, void *data)
{
    GG_UNUSED(dir);
    if (tree->key_present  == 0)
    {
        // empty tree node, just add root
        tree->key = gg_cursor->key;
        //
        // There is no checking if existing data is the same as new data because in a tree
        // it's always new, there's no updating of data like say in lists
        //
        gg_mem_add_ref (data, gg_mem_process);
        tree->data = data;
        //
        tree->height = 1;
        tree->key_present = 1;
        if (gg_cursor->root->sorted) 
        {
            // setup a linked list if the first one
            if (!tree->dlist[GG_TREE_LESSER_LIST]) gg_cursor->root->min = tree;
            if (!tree->dlist[GG_TREE_GREATER_LIST]) gg_cursor->root->max = tree;
        }
        gg_cursor->current = tree;
        gg_cursor->status = GG_OKAY;
        gg_cursor->root->count++;
        return;
    }

    // tree has data and possibly subnodes
    int comparison = GG_TREE_EVAL(tree->key);
    if (comparison < 0)
    {
        // this is lesser key
        bool is_new = false;
        if (tree->lesser_node == NULL) 
        {
            // create new one if none there, with no key, that's added in gg_tree_insert below
            tree->lesser_node = gg_tree_node_create(gg_cursor->root->sorted?1:0);
            is_new = true;
            if (gg_cursor->root->sorted) 
            {
                // connect new node into linked list, if this tree has that feature
                if (tree->dlist[GG_TREE_LESSER_LIST]) tree->dlist[GG_TREE_LESSER_LIST]->dlist[GG_TREE_GREATER_LIST] = tree->lesser_node;
                tree->lesser_node->dlist[GG_TREE_LESSER_LIST] = tree->dlist[GG_TREE_LESSER_LIST];
                tree->dlist[GG_TREE_LESSER_LIST] = tree->lesser_node;
                tree->lesser_node->dlist[GG_TREE_GREATER_LIST] = tree;
            }
        }
        gg_tree_insert (tree, GG_TREE_LESSER, tree->lesser_node, data);
        if (!is_new) 
        {
            gg_tree_balance (tree, GG_TREE_LESSER, tree->lesser_node); // don't balance down-node if added a leaf
            gg_tree_balance (parent_tree, dir, tree);
        }
    }
    else if (comparison == 0)
    {
        GG_ERR0; gg_cursor->status = GG_ERR_EXIST; 
        return;
    }
    else
    {
        // this is greater key
        bool is_new = false;
        if (tree->greater_node == NULL) 
        {
            // create new one if none there, with no key, that's added in gg_tree_insert below
            tree->greater_node = gg_tree_node_create(gg_cursor->root->sorted?1:0);
            is_new = true;
            if (gg_cursor->root->sorted)
            {
                // connect new node into linked list, if this tree has that feature
                if (tree->dlist[GG_TREE_GREATER_LIST]) tree->dlist[GG_TREE_GREATER_LIST]->dlist[GG_TREE_LESSER_LIST] = tree->greater_node;
                tree->greater_node->dlist[GG_TREE_GREATER_LIST] = tree->dlist[GG_TREE_GREATER_LIST];
                tree->dlist[GG_TREE_GREATER_LIST] = tree->greater_node;
                tree->greater_node->dlist[GG_TREE_LESSER_LIST] = tree;
            }
        }
        gg_tree_insert (tree, GG_TREE_GREATER, tree->greater_node, data);
        if (!is_new) 
        {
            gg_tree_balance (tree, GG_TREE_GREATER, tree->greater_node); // don't balance down-node if added a leaf
            gg_tree_balance (parent_tree, dir, tree);
        }
    }
    //GG_UNUSED(parent_tree);
    /*char lh = tree->lesser_node?tree->lesser_node->height:0;
    char rh = tree->greater_node?tree->greater_node->height:0;
    int f = (lh-rh);
    if (f<0) f=-f;
    assert(f<2);*/
}


//
// Find lesser or equal key to that of gg_cursor->key. If 'equal' is true, then search for equal as well.
// gg_cursor->current/status set.
//
static void gg_tree_search_lesser_equal (gg_tree_node *tree, bool equal)
{
    gg_tree_node *prev_lesser = NULL;
    // Start from node 'tree' which is usually given as top root
    // go down the tree until found, if there's no key (empty tree), just declare not found below since prev_lesser is NULL
    if (tree && tree->key_present != 0) { 
        while (tree)
        {
            // check if key lesser, equal, greater
            int cmp = GG_TREE_EVAL(tree->key);
            if (cmp == 0)
            {
                // if equal not requested, then go the lesser node (since this is lesser search)
                if (!equal) { cmp = -1; } 
                else
                {
                    // and if equal requested, done
                    gg_cursor->status = GG_OKAY;
                    gg_cursor->current = tree;
                    return;
                }
            } 
            // check if key lesser or equal, move down the tree, cannot be 'else' here
            // since we set cmp manually in one case above
            if (cmp < 0) { tree = tree->lesser_node;  }
            else 
            { 
                prev_lesser = tree;  
                tree = tree->greater_node; 
            }
        }
    }
    // Here we come when we exhausted the search and are at some node where key is either lesser or greater
    // than the previous node, but in that direction there's nothing (NULL child)
    if (prev_lesser != NULL)
    {
        // this is the last key that was lesser (i.e. key search for was greater), and that's the maximum lesser key
        gg_cursor->status = GG_OKAY;
        gg_cursor->current = prev_lesser;
        return;
    }
    else 
    {
        // there was no lesser key, so nothing
        GG_ERR0; gg_cursor->status = GG_ERR_EXIST;
    }
    return;
}

//
// Find greater or equal key to that of gg_cursor->key. If 'equal' is true, then search for equal as well.
// gg_cursor->current/status set.
//
static void gg_tree_search_greater_equal (gg_tree_node *tree, bool equal)
{
    gg_tree_node *prev_greater = NULL;
    // start from the top
    // go down the tree until found, if there's no key (empty tree), just declare not found below since prev_greater is NULL
    if (tree && tree->key_present != 0) {
        while (tree)
        {
            int cmp = GG_TREE_EVAL(tree->key);
            if (cmp == 0)
            {
                if (!equal) { cmp = 1; } // if equal not requested, go down greater path
                else
                {
                    gg_cursor->status = GG_OKAY; // if equal, and 'equal' requested
                    gg_cursor->current = tree;
                    return;
                }
            } 
            // since we manipulate cmp above, cannot do 'else' here. Move down the tree 
            if (cmp < 0) 
            {
                prev_greater = tree;  
                tree = tree->lesser_node; 
            }
            else { tree = tree->greater_node; }
        }
    }
    if (prev_greater != NULL)
    {
        // the last key found greater than requested, is the minimum greater one
        gg_cursor->status = GG_OKAY;
        gg_cursor->current = prev_greater;
        return;
    }
    else 
    {
        // no key found greater
        GG_ERR0; gg_cursor->status = GG_ERR_EXIST;
    }
    return;
}


// 
// Search for the exact gg_cursor->key 
// gg_cursor->current/status set.
//
static void gg_tree_search (gg_tree_node *tree)
{
    // go down the tree until found, if there's no key (empty tree), just declare not found below
    if (tree && tree->key_present != 0) { 
        while (tree) 
        {
            int cmp = GG_TREE_EVAL(tree->key);
            if (cmp == 0)
            {
                gg_cursor->status = GG_OKAY;
                gg_cursor->current = tree;
                return;
            }
            if (cmp < 0) tree = tree->lesser_node; 
            else tree = tree->greater_node;
        }
    }
    // if here, none found
    GG_ERR0; gg_cursor->status = GG_ERR_EXIST;
    return;
}


//
// Search for minimum key. lcurs is the cursor to set, orig_tree is the root of the tree
// gg_cursor->current/status set.
//
void gg_tree_min_f (gg_tree_cursor *lcurs, gg_tree *orig_tree)
{
    gg_cursor = lcurs;
    gg_cursor->root = orig_tree;
    if (orig_tree->sorted)
    {
        // If there has a linked list, we have it's head on the left right away
        if (orig_tree->count == 0) { GG_ERR0; gg_cursor->status = GG_ERR_EXIST; return;}
        gg_cursor->status = GG_OKAY;
        gg_cursor->current = orig_tree->min;
    }
    else
    {
        // if we don't have a linked list, go down the tree, getting lesser and lesser 
        // until nothing found. The last one is the smallest.
        gg_tree_node *cur = orig_tree->tree->lesser_node;
        //this takes care of empty tree
        if (orig_tree->count == 0) { GG_ERR0; gg_cursor->status = GG_ERR_EXIST; return;}
        gg_cursor->status = GG_OKAY;
        while (cur->lesser_node) cur = cur->lesser_node;
        gg_cursor->current = cur;
    }
}

//
// Search for maximum key. lcurs is the cursor to set, orig_tree is the root of the tree
// gg_cursor->current/status set.
//
void gg_tree_max_f (gg_tree_cursor *lcurs, gg_tree *orig_tree)
{
    gg_cursor = lcurs;
    gg_cursor->root = orig_tree;
    if (orig_tree->sorted)
    {
        // if linked list present, get the max, i.e. head on the right
        if (orig_tree->count == 0) { GG_ERR0; gg_cursor->status = GG_ERR_EXIST; return;}
        gg_cursor->status = GG_OKAY;
        gg_cursor->current = orig_tree->max;
    }
    else
    {
        // if no linked list, go down greater always, until no greater found
        gg_tree_node *cur = orig_tree->tree->lesser_node;
        //this takes care of empty tree
        if (orig_tree->count == 0) { GG_ERR0; gg_cursor->status = GG_ERR_EXIST; return;}
        gg_cursor->status = GG_OKAY;
        while (cur->greater_node) cur = cur->greater_node;
        gg_cursor->current = cur;
    }
}

//
// Part of deleting a node, which is the most complex operation here. Deleting a node (when there's a greater branch from it)
// works by finding the lowest key in the greater node and copying it to node being deleted, then deleting this lowest key node, which is
// easy since it's always a leaf. 'tree_greater_node' is being looked at and we arrived to it from parent tree going in 'dir' direction.
// found is the actual node with found key (gg_cursor->key)
//
static void gg_tree_find_leaf_del (gg_tree_node *parent_tree, int dir, gg_tree_node *tree_greater_node, gg_tree_node *found)
{
    // Here we go to the lowest key in this branch of the tree
    if (tree_greater_node->lesser_node == NULL)
    {
        // once no more lesser nodes, this is the node to copy in place of found and then to be deleted
        if (gg_cursor->root->sorted)
        {
            // if linked list, set it up
            if (found->dlist[GG_TREE_LESSER_LIST]) found->dlist[GG_TREE_LESSER_LIST]->dlist[GG_TREE_GREATER_LIST] = found->dlist[GG_TREE_GREATER_LIST]; else gg_cursor->root->min = found->dlist[GG_TREE_GREATER_LIST];
            if (found->dlist[GG_TREE_GREATER_LIST]) found->dlist[GG_TREE_GREATER_LIST]->dlist[GG_TREE_LESSER_LIST] = found->dlist[GG_TREE_LESSER_LIST]; else gg_cursor->root->max = found->dlist[GG_TREE_LESSER_LIST];
            found->dlist[GG_TREE_LESSER_LIST] = tree_greater_node->dlist[GG_TREE_LESSER_LIST];
            found->dlist[GG_TREE_GREATER_LIST] = tree_greater_node->dlist[GG_TREE_GREATER_LIST];
            if (tree_greater_node->dlist[GG_TREE_LESSER_LIST]) tree_greater_node->dlist[GG_TREE_LESSER_LIST]->dlist[GG_TREE_GREATER_LIST] = found; else gg_cursor->root->min = found;
            if (tree_greater_node->dlist[GG_TREE_GREATER_LIST]) tree_greater_node->dlist[GG_TREE_GREATER_LIST]->dlist[GG_TREE_LESSER_LIST] = found; else gg_cursor->root->max = found;
        }
        // put this leaf node's key/data into one to delete
        // swap keys because we want to delete found's key, which is now in tree_greater_node, which we delete below
        char *temp = found->key;
        found->key = tree_greater_node->key;
        tree_greater_node->key = temp;
        // no need to swap data, because ->data in found node was already placed in cursor to be returned to end-user, 
        // and no need to change reference of ->data because it's deleted here and then assigned to variable (in which case it's refcount
        // remains the same), or it's deleted in v1.c (in delete-index) and that removes it
        // so just overwrite found node's data with the leaf node's data
        gg_mem_dec_process (found->data);
        found->data = tree_greater_node->data;
        //
        // make sure leaf node's parent is connected property
        GG_TREE_SET_PARENT(parent_tree, dir, tree_greater_node->greater_node);
        gg_tree_node_delete (tree_greater_node); // delete leaf node which now has key and data of the node we actually are deleting
        // no need to balance previous tree_greater_node->greater_node because it can be only 1 extra in height
        // and parent_tree is balanced in caller
        return;
    }
    else 
    {
        // go down lesser until above found; each time we back up, balance the lesser part (which is at the bottom the parent of the 
        // node found above (lesser_node == NULL)
        gg_tree_find_leaf_del (tree_greater_node, GG_TREE_LESSER, tree_greater_node->lesser_node, found);
        if (tree_greater_node->lesser_node) gg_tree_balance (tree_greater_node, GG_TREE_LESSER, tree_greater_node->lesser_node);
        gg_tree_balance (parent_tree, dir, tree_greater_node); // then balance it's parent tree_greater_node
        return;
    }
    return; 
}

//
// Delete a node with gg_cursor->key key. tree is the node looked at, and we arrived at it by going in
// 'dir' direction from parent_tree (so dir is either lesser or greater).
//
static void gg_tree_delete (gg_tree_node *parent_tree,  int dir, gg_tree_node *tree)
{
    void *res = NULL;
    // compare fixed key with tree->key 
    int cmp = GG_TREE_EVAL(tree->key);
    if (cmp == 0)
    {
        // if equal, save pointers to data and key before proceeding to delete.
        res = tree->data;
        if (tree->greater_node == NULL)
        {
            // if there is no greater node, then connect parent lesser node with the deleting-node's lesser one, easy case
            GG_TREE_SET_PARENT(parent_tree, dir, tree->lesser_node);
            if (gg_cursor->root->sorted)
            {
                // update the linked list. dlist may or may not be here (meaning allocated); it's not if sorted is false.
                if (tree->dlist[GG_TREE_LESSER_LIST]) tree->dlist[GG_TREE_LESSER_LIST]->dlist[GG_TREE_GREATER_LIST] = tree->dlist[GG_TREE_GREATER_LIST]; else gg_cursor->root->min = tree->dlist[GG_TREE_GREATER_LIST];
                if (tree->dlist[GG_TREE_GREATER_LIST]) tree->dlist[GG_TREE_GREATER_LIST]->dlist[GG_TREE_LESSER_LIST] = tree->dlist[GG_TREE_LESSER_LIST]; else gg_cursor->root->max = tree->dlist[GG_TREE_LESSER_LIST];
            }
            // delete the node, free it up
            gg_mem_dec_process (tree->data);// make sure value, if process-scoped, will be un-process-ed if ref was just 1
                                                   // since if we're assigning this value to a variable, this variable must not be process-scoped
                                                   // or otherwise this would be a leak and memory would grow as this memory would never
                                                   // be released
            gg_tree_node_delete (tree);
        }
        else
        {
            // if there is a greater node, go down to find the lowest key node in the 'greater' branch from the node to be deleted.
            // This lowest key node is always leaf and once the node to be deleted is taken out, it can take its place in the tree.
            // Then we move that node's key and data to the one we're 'deleting', and actually delete the leaf one.
            gg_tree_find_leaf_del (tree, GG_TREE_GREATER, tree->greater_node, tree); // the above is done here.
            // balance the node we started down towards, and then it's parent; they all may be affected
            if (tree->greater_node) gg_tree_balance (tree, GG_TREE_GREATER, tree->greater_node);
            gg_tree_balance (parent_tree, dir, tree);
        }
        // setup result
        gg_cursor->status = GG_OKAY;
        gg_cursor->root->count--;
        gg_cursor->res = res;
        return;
    }
    else
    {
        // go down the tree until exhausted; if nothing found, nothing to delete
        if (cmp < 0 && tree->lesser_node) 
        {
            gg_tree_delete (tree, GG_TREE_LESSER, tree->lesser_node);
            // balance both the tree we went down towards and its parent
            if (tree->lesser_node != NULL) gg_tree_balance (tree, GG_TREE_LESSER, tree->lesser_node);
            gg_tree_balance (parent_tree, dir, tree);
            return;
        }
        if (cmp > 0 && tree->greater_node)
        {
            gg_tree_delete (tree, GG_TREE_GREATER, tree->greater_node);
            // balance both the tree we went down towards and its parent
            if (tree->greater_node) gg_tree_balance (tree, GG_TREE_GREATER, tree->greater_node);
            gg_tree_balance (parent_tree, dir, tree);
            return;
        }
    }
    // nothing to delete is here
    GG_ERR0; gg_cursor->status = GG_ERR_EXIST;
    return;
}

//
// Internal support. Check the whole tree to make sure it's perfectly balanced. Used it tests to prove the tree
// is always perfectly balanced no matter what. It simply traverses the whole tree and checks each and every node.
// Returns >0 if problem.
//
gg_num gg_tree_bal (gg_tree_node *tree)
{
    gg_num res = 0;
    if (tree->lesser_node) res += gg_tree_bal(tree->lesser_node);
    if (tree->greater_node) res += gg_tree_bal (tree->greater_node); 

    int f = (tree->lesser_node?tree->lesser_node->height:0) - (tree->greater_node?tree->greater_node->height:0);
    if (f < -1 || f > 1) {
        printf("VELERROR %d %s %s\n", f, tree->lesser_node==NULL?"lesser null":"", tree->greater_node==NULL ? "greater null":"");
        return 1+res; 
    } else return res;
}


//
// Top level API for purge. Deletes all data (key, values and nodes) in the tree.
//
void gg_tree_purge_f (gg_tree *orig_tree)
{
    // first delete all nodes
    // static here is to avoid dangling pointer error - this is a local cursor, and then we find the miniminum in
    // the tree, and delete all, so in reality there is cursor beyond this function. But gcc doesn't know that.
    static gg_tree_cursor tcurs;
    while (1)
    {
        gg_tree_min_f (&tcurs, orig_tree);
        if (gg_cursor->status == GG_OKAY) 
        {
            gg_mem_dec_process (gg_cursor->current->data); // this must come before gg_tree_delete_f because the last one in this while loop
                                                   // will not be the valid node
            gg_tree_delete_f (&tcurs, orig_tree, gg_cursor->current->key);
        }
        else break;
    }
    // then delete all structure, which we don't do
    //if (orig_tree->count != 0) gg_report_error ("Cannot purge non-empty tree. Delete all nodes first.");
    //if (orig_tree->tree->lesser_node != NULL) gg_tree_node_delete (orig_tree->tree->lesser_node); 
    //gg_mem_dec_process (orig_tree->tree); 
    //gg_mem_dec_process(orig_tree);
}

//
// Top level API for search. lcurs is the cursor, orig_tree is the tree structure, key/key_len to search for.
// Sets gg_cursor->current to found, or ->status to GG_ERR_EXIST if not found (otherwise GG_OKAY). 
// If key_len is -1, sets key_len to strlen() of key
//
void gg_tree_search_f (gg_tree_cursor *lcurs, gg_tree *orig_tree, char *key, gg_num key_len)
{
    gg_cursor = lcurs;
    gg_cursor->root = orig_tree;
#ifdef DEBUG
    gg_cursor->root->hops=0;
#endif
    if (key_len == -1) gg_cursor->key_len = gg_mem_get_len(key); else gg_cursor->key_len = key_len;
    gg_cursor->key = key; 
    gg_tree_search (orig_tree->tree->lesser_node);
}

//
// Top level API for delete. lcurs is the cursor, orig_tree is the tree structure, key/key_len to delete.
// Sets gg_cursor->data of deleted node, since key is delete and there's nothing to set. 
//
void gg_tree_delete_f (gg_tree_cursor *lcurs, gg_tree *orig_tree, char *key)
{
    gg_cursor = lcurs;
    gg_cursor->root = orig_tree;
#ifdef DEBUG
    gg_cursor->root->hops=0;
#endif
    gg_cursor->key_len = gg_mem_get_len(key);
    gg_cursor->key = key; 
    if (orig_tree->tree->lesser_node && orig_tree->tree->lesser_node->key_present != 0) 
    {
        gg_tree_delete (orig_tree->tree, GG_TREE_LESSER, orig_tree->tree->lesser_node);
        // check if root deleted. If so, create empty root (with no key), or otherwise nothing else will work on the tree.
        if (orig_tree->tree->lesser_node == NULL) gg_tree_create_root (orig_tree, orig_tree->sorted);
    }
    else 
    {
        GG_ERR0; gg_cursor->status = GG_ERR_EXIST;
    }
}

//
// Top level API for insert. lcurs is the cursor, orig_tree is the tree structure, key/key_len to delete.
// Sets gg_cursor->cursor to inserted. If key_len is -1, sets key_len to strlen() of key
//
void gg_tree_insert_f (gg_tree_cursor *lcurs, gg_tree *orig_tree, char *key, gg_num key_len, void *data)
{
    gg_cursor = lcurs;
    gg_cursor->root = orig_tree;
#ifdef DEBUG
        gg_cursor->root->hops=0;
#endif
    //
    // There is no checking if existing key is the same as new key because in a tree
    // it's always new, there's no updating of key like say in lists
    //
    gg_mem_add_ref (key, gg_mem_process);
    gg_cursor->key = key;
    if (key_len == -1) gg_cursor->key_len = gg_mem_get_len(key); else gg_cursor->key_len = key_len;
    //
    gg_tree_insert (orig_tree->tree, GG_TREE_LESSER, orig_tree->tree->lesser_node ,data);
}
 
//
// Top level API for search <=. lcurs is the cursor, orig_tree is the tree structure, equal is true if it's <= otherwise <
// key/key_len to search for.
// Sets gg_cursor->current to found, or ->status to GG_ERR_EXIST if not found (otherwise GG_OKAY). 
//
void gg_tree_search_lesser_equal_f (gg_tree_cursor *lcurs, gg_tree *orig_tree, bool equal, char *key, gg_num key_len)
{
    gg_cursor = lcurs;
    gg_cursor->root = orig_tree;
#ifdef DEBUG
    gg_cursor->root->hops=0;
#endif
    if (key_len == -1) gg_cursor->key_len = gg_mem_get_len(key); else gg_cursor->key_len = key_len;
    gg_cursor->key = key; 
    gg_tree_search_lesser_equal (orig_tree->tree->lesser_node, equal);
}

//
// Top level API for search >=. lcurs is the cursor, orig_tree is the tree structure, equal is true if it's >= otherwise >
// key/key_len to search for.
// Sets gg_cursor->current to found, or ->status to GG_ERR_EXIST if not found (otherwise GG_OKAY). 
//
void gg_tree_search_greater_equal_f (gg_tree_cursor *lcurs, gg_tree *orig_tree, bool equal, char *key, gg_num key_len)
{
    gg_cursor = lcurs;
    gg_cursor->root = orig_tree;
#ifdef DEBUG
        gg_cursor->root->hops=0;
#endif
    if (key_len == -1) gg_cursor->key_len = gg_mem_get_len(key); else gg_cursor->key_len = key_len;
    gg_cursor->key = key;
    gg_tree_search_greater_equal (orig_tree->tree->lesser_node, equal);
}

