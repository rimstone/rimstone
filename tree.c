// SPDX-License-Identifier: Apache-2.0
// Copyright 2018-2025 Gliim LLC. 
// Licensed under Apache License v2. See LICENSE file.
// On the web http://rimstone-lang.com/ - this file is part of RimStone framework.

// 
// Tree implementation, in-memory modified AVL with optional double-linked sorted list. 
// Around 50% faster than in-memory B-tree variations.
//

#include "rim.h"


// connect parent (p) to child (r) given the direction from the tree node (dir)
#define RIM_TREE_SET_PARENT(p, dir, r) if (dir == RIM_TREE_LESSER) (p)->lesser_node=(r); else (p)->greater_node=(r)

rim_tree_cursor *rim_cursor; // internal cursor for the current tree operation

// Function prototypes for the implementation
static inline void rim_tree_rotate_left (rim_tree_node *parent_tree, int dir, rim_tree_node *tree);
static inline void rim_tree_rotate_right (rim_tree_node *parent_tree, int dir, rim_tree_node *tree);
static void rim_tree_insert(rim_tree_node *parent_tree, int dir, rim_tree_node *tree, void *data);
static inline int rim_tree_compare(char *k2);
static void rim_tree_search (rim_tree_node *tree);
static inline void rim_tree_height (rim_tree_node *tree, rim_num *factor);
static inline void rim_tree_balance (rim_tree_node *parent_tree, int dir, rim_tree_node *tree);
static void rim_tree_delete (rim_tree_node *parent_tree, int dir, rim_tree_node *tree);
static void rim_tree_find_leaf_del (rim_tree_node *parent_tree, int dir, rim_tree_node *tree_greater_node, rim_tree_node *found);
static void rim_tree_search_lesser_equal (rim_tree_node *tree, bool equal);
static void rim_tree_search_greater_equal (rim_tree_node *tree, bool equal);
static rim_tree_node *rim_tree_node_create(char sorted);
static void rim_tree_node_delete(rim_tree_node *tree);

// measuring tree hops (i.e. the cost to search), only for debug build
#ifdef RIM_DEVEL
#define RIM_TREE_HOPS    rim_cursor->root->hops++,
#else
#define RIM_TREE_HOPS    
#endif

// Evaluate the tree node key k2 with another fixed key obtained from rim_cursor->key, 
// i.e. compare fixed key<=>current_tree_node_key. Uses custom eval function if specified
#define RIM_TREE_EVAL(k2) (RIM_TREE_HOPS rim_tree_compare(k2))

// Default key evaluation function. Compares current tree node key with rim_cursor->key. Return -1, 0, or 1 if node-key lesser, equal or greater than given key.
// Note: POSIX actually specifies that strncmp() works if the length compared is greater than the length of one of the strings (memcmp doesn't do that)
// rim_cursor holds the key and key_len members, which are precomputed. k2 is the tree node key.
static inline int rim_tree_compare(char *k2)
{
    if (rim_cursor->root->key_type == RIM_TREE_TYPE_NUM)
    {
        // key-as positive-integer, compare as numbers written as strings, much faster than atol()
        // works in any base
        rim_num l1 = rim_cursor->key_len;
        rim_num l2 = rim_mem_get_len(k2);
        if (l1<l2) return -1;
        if (l1>l2) return 1;
        return memcmp(rim_cursor->key,k2,l1);
    }
    else
    {
        // compare as classic strings, C collation
        rim_num l1 = rim_cursor->key_len;
        rim_num l2 = rim_mem_get_len(k2);
        rim_num l = MIN(l1, l2)+1;
        return memcmp (rim_cursor->key,k2, l);
    }
}

//
// Delete tree node. Actual key and data must be obtained prior and deleted if needed. tree is the node to delete.
//
static void rim_tree_node_delete(rim_tree_node *tree)
{
    // tree node deletion is always based on the exact key, meaning you can't say delete the node 'greater than X', you must first
    // have the exact key being deleted. Since the key being deleted is already known, there's no point in keeping the key, so we delete it.
    rim_mem_dec_process (tree->key); 
    rim_mem_dec_process (tree);
}

//
// Create tree node. Allocated pointers for linked list if sorted is 1. Returns the node.
//
static rim_tree_node *rim_tree_node_create(char sorted)
{
    rim_tree_node *res;
    res = rim_calloc (1, sizeof(rim_tree_node) + (sorted==1 ? 2*sizeof (rim_tree_node *):0));
    return res;
}


//
// Create root node of the tree. 
// res is the tree itself, sorted is true/false based on unsorted clause in new-tree.
//
void rim_tree_create_root (rim_tree *res, bool sorted) 
{
    res->root_node = rim_tree_node_create(sorted?1:0); // never used directly, only reference as tree->lesser, this is the actual tree root
    res->tree->lesser_node = res->root_node; // RIM_TREE_LESSER must always be used with root reference because of this assignment
}

//
// Create the tree itself. key_type is for default eval function (number comparison)
// sorted is true if there's linked list for fast range access. Returns the tree.
//
rim_tree *rim_tree_create(char key_type, bool sorted, unsigned char process)
{
    rim_tree *res = rim_calloc (1, sizeof(rim_tree) + (sorted?2*sizeof (rim_tree_node *):0));
    res->process = process;
    res->sorted = sorted; // must be set before rim_tree_node_create() below
    res->key_type = key_type;
    // Tree has a dummy node, which has a lesser pointer that points to an actual root of the tree
    // This is so all recursive algorithms work faster without handling exceptions
    res->tree = rim_tree_node_create (sorted?1:0);  //->tree is a dummy node sitting on top of the actual root
    rim_tree_create_root (res, sorted);
    return res;
}

//
// Get the height of a node tree. factor is the difference between left and right (if not NULL).
// Node's ->height member is updated by doing this. 
// A node without any children has a height of 1, with at least one child it's 2 etc.
//
static inline void rim_tree_height (rim_tree_node *tree, rim_num *factor)
{
    rim_num left_height;
    rim_num right_height;
    if (tree->lesser_node == NULL) left_height = 0; else left_height = tree->lesser_node->height;
    if (tree->greater_node == NULL) right_height = 0; else right_height = tree->greater_node->height;
    if (factor) *factor = left_height - right_height; // instead of right-left in classic AVL
    if (left_height > right_height) tree->height = left_height+1; else tree->height = right_height+1;
}


//
// Rotate the node to the right. parent_tree is the parent of the node being rotated, dir is the direction from parent
// to the node being rotated, which is tree.
//
static inline void rim_tree_rotate_right (rim_tree_node *parent_tree, int dir, rim_tree_node *tree)
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
    RIM_TREE_SET_PARENT(parent_tree, dir, tree->lesser_node);
    rim_tree_node *t = tree->lesser_node->greater_node;
    tree->lesser_node->greater_node = tree;
    tree->lesser_node = t;
    rim_tree_height (tree, NULL);
    rim_tree_height (parent_tree, NULL);
}

//
// Rotate the node to the left. parent_tree is the parent of the node being rotated, dir is the direction from parent
// to the node being rotated, which is tree.
//
static inline void rim_tree_rotate_left (rim_tree_node *parent_tree, int dir, rim_tree_node *tree)
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
    RIM_TREE_SET_PARENT(parent_tree, dir, tree->greater_node);
    rim_tree_node *t = tree->greater_node->lesser_node;
    tree->greater_node->lesser_node = tree;
    tree->greater_node = t;
    rim_tree_height (tree, NULL);
    rim_tree_height (parent_tree, NULL);
}

//
// Balance 'tree', with parent parent_tree coming to 'tree' by dir direction (left/lesser or right/greater)
//
static inline void rim_tree_balance (rim_tree_node *parent_tree, int dir, rim_tree_node *tree)
{
    // get the balance factor of the node to balance
    rim_num bal_factor;
    rim_tree_height (tree, &bal_factor);
    //printf("Left %d Right %d\n", tree->lesser_node?tree->lesser_node->height:0, tree->greater_node?tree->greater_node->height:0);
    //assert (-2<=bal_factor && bal_factor<=2);
    
    //Balance factor can be within -2 and 2 inclusive here. The goal is to bring it to 0 or 1/-1.
    if (bal_factor >= 2)
    {
        // This needs balancing to the right, since left is taller. However, if the right subbranch of the left branch is taller than the left subbranch,
        // the right rotation will just make the left-mirror-image of the same problem. Thus, we need to left-rotate the right subbranch of the left branch first.
        if (tree->lesser_node != NULL)
        {
            if ((tree->lesser_node->greater_node ? tree->lesser_node->greater_node->height:0) > (tree->lesser_node->lesser_node?tree->lesser_node->lesser_node->height:0)) rim_tree_rotate_left (tree, RIM_TREE_LESSER, tree->lesser_node);
        }
        rim_tree_rotate_right (parent_tree, dir, tree);
        // Recalculate the height of the node rotated, as well as its parent
        rim_tree_height (tree, NULL);
    }
    else if (bal_factor <= -2)
    {
        // This needs balancing to the left, since rigtt is taller. However, if the left subbranch of the right branch is taller than the right subbranch,
        // the leftt rotation will just make the right-mirror-image of the same problem. Thus, we need to right-rotate the left subbranch of the right branch first.
        if (tree->greater_node != NULL)
        {
            if ((tree->greater_node->lesser_node ? tree->greater_node->lesser_node->height:0) > (tree->greater_node->greater_node?tree->greater_node->greater_node->height:0)) rim_tree_rotate_right (tree, RIM_TREE_GREATER, tree->greater_node);
        }
        rim_tree_rotate_left (parent_tree, dir, tree);
        // Recalculate the height of the node rotated, as well as its parent
        rim_tree_height (tree, NULL);
    }
    //num bf;
    //rim_tree_height (tree, &bf);
    //assert (bf<=1 && bf>=-1);
}

//
// Insert data with rim_cursor/key/key_len into tree. tree is the node considered, parent_tree/dir is
// its parent and the direction to reach tree (lesser/greater).
// rim_cursor->current/status set.
//
static void rim_tree_insert(rim_tree_node *parent_tree, int dir, rim_tree_node *tree, void *data)
{
    RIM_UNUSED(dir);
    if (tree->key_present  == 0)
    {
        // empty tree node, just add root
        tree->key = rim_cursor->key;
        //
        // There is no checking if existing data is the same as new data because in a tree
        // it's always new, there's no updating of data like say in lists
        //
        rim_mem_add_ref (data, rim_mem_process);
        tree->data = data;
        //
        tree->height = 1;
        tree->key_present = 1;
        if (rim_cursor->root->sorted) 
        {
            // setup a linked list if the first one
            if (!tree->dlist[RIM_TREE_LESSER_LIST]) rim_cursor->root->min = tree;
            if (!tree->dlist[RIM_TREE_GREATER_LIST]) rim_cursor->root->max = tree;
        }
        rim_cursor->current = tree;
        rim_cursor->status = RIM_OKAY;
        rim_cursor->root->count++;
        return;
    }

    // tree has data and possibly subnodes
    int comparison = RIM_TREE_EVAL(tree->key);
    if (comparison < 0)
    {
        // this is lesser key
        bool is_new = false;
        if (tree->lesser_node == NULL) 
        {
            // create new one if none there, with no key, that's added in rim_tree_insert below
            tree->lesser_node = rim_tree_node_create(rim_cursor->root->sorted?1:0);
            is_new = true;
            if (rim_cursor->root->sorted) 
            {
                // connect new node into linked list, if this tree has that feature
                if (tree->dlist[RIM_TREE_LESSER_LIST]) tree->dlist[RIM_TREE_LESSER_LIST]->dlist[RIM_TREE_GREATER_LIST] = tree->lesser_node;
                tree->lesser_node->dlist[RIM_TREE_LESSER_LIST] = tree->dlist[RIM_TREE_LESSER_LIST];
                tree->dlist[RIM_TREE_LESSER_LIST] = tree->lesser_node;
                tree->lesser_node->dlist[RIM_TREE_GREATER_LIST] = tree;
            }
        }
        rim_tree_insert (tree, RIM_TREE_LESSER, tree->lesser_node, data);
        if (!is_new) 
        {
            rim_tree_balance (tree, RIM_TREE_LESSER, tree->lesser_node); // don't balance down-node if added a leaf
            rim_tree_balance (parent_tree, dir, tree);
        }
    }
    else if (comparison == 0)
    {
        RIM_ERR0; rim_cursor->status = RIM_ERR_EXIST; 
        return;
    }
    else
    {
        // this is greater key
        bool is_new = false;
        if (tree->greater_node == NULL) 
        {
            // create new one if none there, with no key, that's added in rim_tree_insert below
            tree->greater_node = rim_tree_node_create(rim_cursor->root->sorted?1:0);
            is_new = true;
            if (rim_cursor->root->sorted)
            {
                // connect new node into linked list, if this tree has that feature
                if (tree->dlist[RIM_TREE_GREATER_LIST]) tree->dlist[RIM_TREE_GREATER_LIST]->dlist[RIM_TREE_LESSER_LIST] = tree->greater_node;
                tree->greater_node->dlist[RIM_TREE_GREATER_LIST] = tree->dlist[RIM_TREE_GREATER_LIST];
                tree->dlist[RIM_TREE_GREATER_LIST] = tree->greater_node;
                tree->greater_node->dlist[RIM_TREE_LESSER_LIST] = tree;
            }
        }
        rim_tree_insert (tree, RIM_TREE_GREATER, tree->greater_node, data);
        if (!is_new) 
        {
            rim_tree_balance (tree, RIM_TREE_GREATER, tree->greater_node); // don't balance down-node if added a leaf
            rim_tree_balance (parent_tree, dir, tree);
        }
    }
    //RIM_UNUSED(parent_tree);
    /*char lh = tree->lesser_node?tree->lesser_node->height:0;
    char rh = tree->greater_node?tree->greater_node->height:0;
    int f = (lh-rh);
    if (f<0) f=-f;
    assert(f<2);*/
}


//
// Find lesser or equal key to that of rim_cursor->key. If 'equal' is true, then search for equal as well.
// rim_cursor->current/status set.
//
static void rim_tree_search_lesser_equal (rim_tree_node *tree, bool equal)
{
    rim_tree_node *prev_lesser = NULL;
    // Start from node 'tree' which is usually given as top root
    // go down the tree until found, if there's no key (empty tree), just declare not found below since prev_lesser is NULL
    if (tree && tree->key_present != 0) { 
        while (tree)
        {
            // check if key lesser, equal, greater
            int cmp = RIM_TREE_EVAL(tree->key);
            if (cmp == 0)
            {
                // if equal not requested, then go the lesser node (since this is lesser search)
                if (!equal) { cmp = -1; } 
                else
                {
                    // and if equal requested, done
                    rim_cursor->status = RIM_OKAY;
                    rim_cursor->current = tree;
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
        rim_cursor->status = RIM_OKAY;
        rim_cursor->current = prev_lesser;
        return;
    }
    else 
    {
        // there was no lesser key, so nothing
        RIM_ERR0; rim_cursor->status = RIM_ERR_EXIST;
    }
    return;
}

//
// Find greater or equal key to that of rim_cursor->key. If 'equal' is true, then search for equal as well.
// rim_cursor->current/status set.
//
static void rim_tree_search_greater_equal (rim_tree_node *tree, bool equal)
{
    rim_tree_node *prev_greater = NULL;
    // start from the top
    // go down the tree until found, if there's no key (empty tree), just declare not found below since prev_greater is NULL
    if (tree && tree->key_present != 0) {
        while (tree)
        {
            int cmp = RIM_TREE_EVAL(tree->key);
            if (cmp == 0)
            {
                if (!equal) { cmp = 1; } // if equal not requested, go down greater path
                else
                {
                    rim_cursor->status = RIM_OKAY; // if equal, and 'equal' requested
                    rim_cursor->current = tree;
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
        rim_cursor->status = RIM_OKAY;
        rim_cursor->current = prev_greater;
        return;
    }
    else 
    {
        // no key found greater
        RIM_ERR0; rim_cursor->status = RIM_ERR_EXIST;
    }
    return;
}


// 
// Search for the exact rim_cursor->key 
// rim_cursor->current/status set.
//
static void rim_tree_search (rim_tree_node *tree)
{
    // go down the tree until found, if there's no key (empty tree), just declare not found below
    if (tree && tree->key_present != 0) { 
        while (tree) 
        {
            int cmp = RIM_TREE_EVAL(tree->key);
            if (cmp == 0)
            {
                rim_cursor->status = RIM_OKAY;
                rim_cursor->current = tree;
                return;
            }
            if (cmp < 0) tree = tree->lesser_node; 
            else tree = tree->greater_node;
        }
    }
    // if here, none found
    RIM_ERR0; rim_cursor->status = RIM_ERR_EXIST;
    return;
}


//
// Search for minimum key. lcurs is the cursor to set, orig_tree is the root of the tree
// rim_cursor->current/status set.
//
void rim_tree_min_f (rim_tree_cursor *lcurs, rim_tree *orig_tree)
{
    rim_cursor = lcurs;
    rim_cursor->root = orig_tree;
    if (orig_tree->sorted)
    {
        // If there has a linked list, we have it's head on the left right away
        if (orig_tree->count == 0) { RIM_ERR0; rim_cursor->status = RIM_ERR_EXIST; return;}
        rim_cursor->status = RIM_OKAY;
        rim_cursor->current = orig_tree->min;
    }
    else
    {
        // if we don't have a linked list, go down the tree, getting lesser and lesser 
        // until nothing found. The last one is the smallest.
        rim_tree_node *cur = orig_tree->tree->lesser_node;
        //this takes care of empty tree
        if (orig_tree->count == 0) { RIM_ERR0; rim_cursor->status = RIM_ERR_EXIST; return;}
        rim_cursor->status = RIM_OKAY;
        while (cur->lesser_node) cur = cur->lesser_node;
        rim_cursor->current = cur;
    }
}

//
// Search for maximum key. lcurs is the cursor to set, orig_tree is the root of the tree
// rim_cursor->current/status set.
//
void rim_tree_max_f (rim_tree_cursor *lcurs, rim_tree *orig_tree)
{
    rim_cursor = lcurs;
    rim_cursor->root = orig_tree;
    if (orig_tree->sorted)
    {
        // if linked list present, get the max, i.e. head on the right
        if (orig_tree->count == 0) { RIM_ERR0; rim_cursor->status = RIM_ERR_EXIST; return;}
        rim_cursor->status = RIM_OKAY;
        rim_cursor->current = orig_tree->max;
    }
    else
    {
        // if no linked list, go down greater always, until no greater found
        rim_tree_node *cur = orig_tree->tree->lesser_node;
        //this takes care of empty tree
        if (orig_tree->count == 0) { RIM_ERR0; rim_cursor->status = RIM_ERR_EXIST; return;}
        rim_cursor->status = RIM_OKAY;
        while (cur->greater_node) cur = cur->greater_node;
        rim_cursor->current = cur;
    }
}

//
// Part of deleting a node, which is the most complex operation here. Deleting a node (when there's a greater branch from it)
// works by finding the lowest key in the greater node and copying it to node being deleted, then deleting this lowest key node, which is
// easy since it's always a leaf. 'tree_greater_node' is being looked at and we arrived to it from parent tree going in 'dir' direction.
// found is the actual node with found key (rim_cursor->key)
//
static void rim_tree_find_leaf_del (rim_tree_node *parent_tree, int dir, rim_tree_node *tree_greater_node, rim_tree_node *found)
{
    // Here we go to the lowest key in this branch of the tree
    if (tree_greater_node->lesser_node == NULL)
    {
        // once no more lesser nodes, this is the node to copy in place of found and then to be deleted
        if (rim_cursor->root->sorted)
        {
            // if linked list, set it up
            if (found->dlist[RIM_TREE_LESSER_LIST]) found->dlist[RIM_TREE_LESSER_LIST]->dlist[RIM_TREE_GREATER_LIST] = found->dlist[RIM_TREE_GREATER_LIST]; else rim_cursor->root->min = found->dlist[RIM_TREE_GREATER_LIST];
            if (found->dlist[RIM_TREE_GREATER_LIST]) found->dlist[RIM_TREE_GREATER_LIST]->dlist[RIM_TREE_LESSER_LIST] = found->dlist[RIM_TREE_LESSER_LIST]; else rim_cursor->root->max = found->dlist[RIM_TREE_LESSER_LIST];
            found->dlist[RIM_TREE_LESSER_LIST] = tree_greater_node->dlist[RIM_TREE_LESSER_LIST];
            found->dlist[RIM_TREE_GREATER_LIST] = tree_greater_node->dlist[RIM_TREE_GREATER_LIST];
            if (tree_greater_node->dlist[RIM_TREE_LESSER_LIST]) tree_greater_node->dlist[RIM_TREE_LESSER_LIST]->dlist[RIM_TREE_GREATER_LIST] = found; else rim_cursor->root->min = found;
            if (tree_greater_node->dlist[RIM_TREE_GREATER_LIST]) tree_greater_node->dlist[RIM_TREE_GREATER_LIST]->dlist[RIM_TREE_LESSER_LIST] = found; else rim_cursor->root->max = found;
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
        rim_mem_dec_process (found->data);
        found->data = tree_greater_node->data;
        //
        // make sure leaf node's parent is connected property
        RIM_TREE_SET_PARENT(parent_tree, dir, tree_greater_node->greater_node);
        rim_tree_node_delete (tree_greater_node); // delete leaf node which now has key and data of the node we actually are deleting
        // no need to balance previous tree_greater_node->greater_node because it can be only 1 extra in height
        // and parent_tree is balanced in caller
        return;
    }
    else 
    {
        // go down lesser until above found; each time we back up, balance the lesser part (which is at the bottom the parent of the 
        // node found above (lesser_node == NULL)
        rim_tree_find_leaf_del (tree_greater_node, RIM_TREE_LESSER, tree_greater_node->lesser_node, found);
        if (tree_greater_node->lesser_node) rim_tree_balance (tree_greater_node, RIM_TREE_LESSER, tree_greater_node->lesser_node);
        rim_tree_balance (parent_tree, dir, tree_greater_node); // then balance it's parent tree_greater_node
        return;
    }
    return; 
}

//
// Delete a node with rim_cursor->key key. tree is the node looked at, and we arrived at it by going in
// 'dir' direction from parent_tree (so dir is either lesser or greater).
//
static void rim_tree_delete (rim_tree_node *parent_tree,  int dir, rim_tree_node *tree)
{
    void *res = NULL;
    // compare fixed key with tree->key 
    int cmp = RIM_TREE_EVAL(tree->key);
    if (cmp == 0)
    {
        // if equal, save pointers to data and key before proceeding to delete.
        res = tree->data;
        if (tree->greater_node == NULL)
        {
            // if there is no greater node, then connect parent lesser node with the deleting-node's lesser one, easy case
            RIM_TREE_SET_PARENT(parent_tree, dir, tree->lesser_node);
            if (rim_cursor->root->sorted)
            {
                // update the linked list. dlist may or may not be here (meaning allocated); it's not if sorted is false.
                if (tree->dlist[RIM_TREE_LESSER_LIST]) tree->dlist[RIM_TREE_LESSER_LIST]->dlist[RIM_TREE_GREATER_LIST] = tree->dlist[RIM_TREE_GREATER_LIST]; else rim_cursor->root->min = tree->dlist[RIM_TREE_GREATER_LIST];
                if (tree->dlist[RIM_TREE_GREATER_LIST]) tree->dlist[RIM_TREE_GREATER_LIST]->dlist[RIM_TREE_LESSER_LIST] = tree->dlist[RIM_TREE_LESSER_LIST]; else rim_cursor->root->max = tree->dlist[RIM_TREE_LESSER_LIST];
            }
            // delete the node, free it up
            rim_mem_dec_process (tree->data);// make sure value, if process-scoped, will be un-process-ed if ref was just 1
                                                   // since if we're assigning this value to a variable, this variable must not be process-scoped
                                                   // or otherwise this would be a leak and memory would grow as this memory would never
                                                   // be released
            rim_tree_node_delete (tree);
        }
        else
        {
            // if there is a greater node, go down to find the lowest key node in the 'greater' branch from the node to be deleted.
            // This lowest key node is always leaf and once the node to be deleted is taken out, it can take its place in the tree.
            // Then we move that node's key and data to the one we're 'deleting', and actually delete the leaf one.
            rim_tree_find_leaf_del (tree, RIM_TREE_GREATER, tree->greater_node, tree); // the above is done here.
            // balance the node we started down towards, and then it's parent; they all may be affected
            if (tree->greater_node) rim_tree_balance (tree, RIM_TREE_GREATER, tree->greater_node);
            rim_tree_balance (parent_tree, dir, tree);
        }
        // setup result
        rim_cursor->status = RIM_OKAY;
        rim_cursor->root->count--;
        rim_cursor->res = res;
        return;
    }
    else
    {
        // go down the tree until exhausted; if nothing found, nothing to delete
        if (cmp < 0 && tree->lesser_node) 
        {
            rim_tree_delete (tree, RIM_TREE_LESSER, tree->lesser_node);
            // balance both the tree we went down towards and its parent
            if (tree->lesser_node != NULL) rim_tree_balance (tree, RIM_TREE_LESSER, tree->lesser_node);
            rim_tree_balance (parent_tree, dir, tree);
            return;
        }
        if (cmp > 0 && tree->greater_node)
        {
            rim_tree_delete (tree, RIM_TREE_GREATER, tree->greater_node);
            // balance both the tree we went down towards and its parent
            if (tree->greater_node) rim_tree_balance (tree, RIM_TREE_GREATER, tree->greater_node);
            rim_tree_balance (parent_tree, dir, tree);
            return;
        }
    }
    // nothing to delete is here
    RIM_ERR0; rim_cursor->status = RIM_ERR_EXIST;
    return;
}

//
// Internal support. Check the whole tree to make sure it's perfectly balanced. Used it tests to prove the tree
// is always perfectly balanced no matter what. It simply traverses the whole tree and checks each and every node.
// Returns >0 if problem.
//
rim_num rim_tree_bal (rim_tree_node *tree)
{
    rim_num res = 0;
    if (tree->lesser_node) res += rim_tree_bal(tree->lesser_node);
    if (tree->greater_node) res += rim_tree_bal (tree->greater_node); 

    int f = (tree->lesser_node?tree->lesser_node->height:0) - (tree->greater_node?tree->greater_node->height:0);
    if (f < -1 || f > 1) {
        printf("VELERROR %d %s %s\n", f, tree->lesser_node==NULL?"lesser null":"", tree->greater_node==NULL ? "greater null":"");
        return 1+res; 
    } else return res;
}


//
// Top level API for purge. Deletes all data (key, values and nodes) in the tree.
//
void rim_tree_purge_f (rim_tree *orig_tree)
{
    // first delete all nodes
    // static here is to avoid dangling pointer error - this is a local cursor, and then we find the miniminum in
    // the tree, and delete all, so in reality there is cursor beyond this function. But gcc doesn't know that.
    static rim_tree_cursor tcurs;
    while (1)
    {
        rim_tree_min_f (&tcurs, orig_tree);
        if (rim_cursor->status == RIM_OKAY) 
        {
            rim_mem_dec_process (rim_cursor->current->data); // this must come before rim_tree_delete_f because the last one in this while loop
                                                   // will not be the valid node
            rim_tree_delete_f (&tcurs, orig_tree, rim_cursor->current->key);
        }
        else break;
    }
    // then delete all structure, which we don't do
    //if (orig_tree->count != 0) rim_report_error ("Cannot purge non-empty tree. Delete all nodes first.");
    //if (orig_tree->tree->lesser_node != NULL) rim_tree_node_delete (orig_tree->tree->lesser_node); 
    //rim_mem_dec_process (orig_tree->tree); 
    //rim_mem_dec_process(orig_tree);
}

//
// Top level API for search. lcurs is the cursor, orig_tree is the tree structure, key/key_len to search for.
// Sets rim_cursor->current to found, or ->status to RIM_ERR_EXIST if not found (otherwise RIM_OKAY). 
// If key_len is -1, sets key_len to strlen() of key
//
void rim_tree_search_f (rim_tree_cursor *lcurs, rim_tree *orig_tree, char *key, rim_num key_len)
{
    rim_cursor = lcurs;
    rim_cursor->root = orig_tree;
#ifdef RIM_DEVEL
    rim_cursor->root->hops=0;
#endif
    if (key_len == -1) rim_cursor->key_len = rim_mem_get_len(key); else rim_cursor->key_len = key_len;
    rim_cursor->key = key; 
    rim_tree_search (orig_tree->tree->lesser_node);
}

//
// Top level API for delete. lcurs is the cursor, orig_tree is the tree structure, key/key_len to delete.
// Sets rim_cursor->data of deleted node, since key is delete and there's nothing to set. 
//
void rim_tree_delete_f (rim_tree_cursor *lcurs, rim_tree *orig_tree, char *key)
{
    rim_cursor = lcurs;
    rim_cursor->root = orig_tree;
#ifdef RIM_DEVEL
    rim_cursor->root->hops=0;
#endif
    rim_cursor->key_len = rim_mem_get_len(key);
    rim_cursor->key = key; 
    if (orig_tree->tree->lesser_node && orig_tree->tree->lesser_node->key_present != 0) 
    {
        rim_tree_delete (orig_tree->tree, RIM_TREE_LESSER, orig_tree->tree->lesser_node);
        // check if root deleted. If so, create empty root (with no key), or otherwise nothing else will work on the tree.
        if (orig_tree->tree->lesser_node == NULL) rim_tree_create_root (orig_tree, orig_tree->sorted);
    }
    else 
    {
        RIM_ERR0; rim_cursor->status = RIM_ERR_EXIST;
    }
}

//
// Top level API for insert. lcurs is the cursor, orig_tree is the tree structure, key/key_len to delete.
// Sets rim_cursor->cursor to inserted. If key_len is -1, sets key_len to strlen() of key
//
void rim_tree_insert_f (rim_tree_cursor *lcurs, rim_tree *orig_tree, char *key, rim_num key_len, void *data)
{
    rim_cursor = lcurs;
    rim_cursor->root = orig_tree;
#ifdef RIM_DEVEL
        rim_cursor->root->hops=0;
#endif
    //
    // There is no checking if existing key is the same as new key because in a tree
    // it's always new, there's no updating of key like say in lists
    //
    rim_mem_add_ref (key, rim_mem_process);
    rim_cursor->key = key;
    if (key_len == -1) rim_cursor->key_len = rim_mem_get_len(key); else rim_cursor->key_len = key_len;
    //
    rim_tree_insert (orig_tree->tree, RIM_TREE_LESSER, orig_tree->tree->lesser_node ,data);
}
 
//
// Top level API for search <=. lcurs is the cursor, orig_tree is the tree structure, equal is true if it's <= otherwise <
// key/key_len to search for.
// Sets rim_cursor->current to found, or ->status to RIM_ERR_EXIST if not found (otherwise RIM_OKAY). 
//
void rim_tree_search_lesser_equal_f (rim_tree_cursor *lcurs, rim_tree *orig_tree, bool equal, char *key, rim_num key_len)
{
    rim_cursor = lcurs;
    rim_cursor->root = orig_tree;
#ifdef RIM_DEVEL
    rim_cursor->root->hops=0;
#endif
    if (key_len == -1) rim_cursor->key_len = rim_mem_get_len(key); else rim_cursor->key_len = key_len;
    rim_cursor->key = key; 
    rim_tree_search_lesser_equal (orig_tree->tree->lesser_node, equal);
}

//
// Top level API for search >=. lcurs is the cursor, orig_tree is the tree structure, equal is true if it's >= otherwise >
// key/key_len to search for.
// Sets rim_cursor->current to found, or ->status to RIM_ERR_EXIST if not found (otherwise RIM_OKAY). 
//
void rim_tree_search_greater_equal_f (rim_tree_cursor *lcurs, rim_tree *orig_tree, bool equal, char *key, rim_num key_len)
{
    rim_cursor = lcurs;
    rim_cursor->root = orig_tree;
#ifdef RIM_DEVEL
        rim_cursor->root->hops=0;
#endif
    if (key_len == -1) rim_cursor->key_len = rim_mem_get_len(key); else rim_cursor->key_len = key_len;
    rim_cursor->key = key;
    rim_tree_search_greater_equal (orig_tree->tree->lesser_node, equal);
}

