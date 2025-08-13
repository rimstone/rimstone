// SPDX-License-Identifier: Apache-2.0
// Copyright 2018-2025 Gliim LLC. 
// Licensed under Apache License v2. See LICENSE file.
// On the web http://golf-lang.com/ - this file is part of Golf framework.

// 
// JSON-related module
//

// Implementation based on JSON standard https://datatracker.ietf.org/doc/html/rfc7159
// See examples of JSON at https://www.json.org/example.html (by douglas@crockford.com)
// Note by standard, JSON is always UTF8 encoded, so this provides implicit UTF8/16 support


#include "golf.h"


//
// temporary string binding, zero-out byte with BINDV and restore with UNBINDV. Cannot be nested.
//
static char tb;
#define BINDV(x) tb=*(x); *(x) =0
#define UNBINDV(x) *(x) = tb
//
// end temporary string binding
//


// default number of json nodes allocated, incremented by
#ifdef DEBUG
#define GG_JSON_NODES 32
#else
#define GG_JSON_NODES 1024
#endif
// max depth of normalized name
#define GG_JSON_MAX_NESTED 32

// Add json node to list of nodes.
// j_tp is the type of node, j_str value, lc is the count in the list of nodes prior to
// this node, l is the list of nodes (normalized name). 
// A node after the last has empty name.
#define GG_ADD_JSON(j_tp, j_str, lc, l) { gg_add_json(); char *n = gg_json_fullname (l, lc); nodes[node_c].name = n; nodes[node_c].type = j_tp; nodes[node_c].str =  j_str; nodes[node_c].alloced = false; node_c++; }; 

// Prototypes
static char *gg_json_fullname (json_node *list, gg_num list_c);
static char *gg_json_num(char *val, gg_num *rv);
static void gg_add_json ();

// Variables used by json parser (recursive)
static json_node list[GG_JSON_MAX_NESTED]; // max depth of nested names, used to construct them
static gg_num list_c = -1; // index of current node being traversed, it's index into array of json_node type, increments with {
static gg_num node_tot = 0; //node total alloc'd
static gg_num node_c = 0; //node counter (current)
static gg_jsonn *nodes = NULL; // nodes of normalized json (name, type, value)
static gg_json *jloc = NULL; // final result, super structure for the whole json document
static char nulled = 0; // character that is nulled to bind a string, used in the following parser iteration
static char *errm; // error message
static gg_num ec = -1; // location where error happened (0..size) or -1 if okay
static gg_num line_ec = -1; // line where error happened (1..) 
static gg_num last_line_ec = 0; // position (byte) of last line beginning (so ec - pos_line is character position since we always detect error one byte afterward)
static gg_num depth = 0; // depth of recursion


//
// Get the normalized name of a leaf
// "list" is the array of names leading up to here, list_c is the index of the final name
// val is the value of leaf, type is its type
// returns normalized name for a leaf name in name:value,  this is
// returned as an allocated value
// this accounts for any arrays
// returns NULL if issue in converting UTF8
//
static char *gg_json_fullname (json_node *list, gg_num list_c)
{


    if (list_c == 0) 
    { 
        char *fulln = gg_malloc (3); //  2 for "" + null 
        strcpy (fulln, "\"\""); 
        gg_mem_set_len (fulln, 3);
        return fulln; 
    } // in case json doc is just a string or gg_number and nothing else, so list_c is 0

    gg_num i;
    gg_num nlen = 0; // length of normalized name
    // first calculate the memory needed to hold normalized name
    for (i = 0; i < list_c; i++)
    {
        if (list[i].index == -1) 
        {
            nlen += 1 + 2 + list[i].name_len; /* 1 for dot, 2 for ", plus length of name (we ignore i==0 and just allocate one byte extra)*/
        }
        else
        {
            gg_num a = list[i].index;
            gg_num alen; // length of digits in array index
            if (a == 0) alen = 1; else { for (alen = 0; a != 0; alen++, a = a / 10){} }
            list[i].index_len = alen;
            nlen += 1 + 2 + list[i].name_len + 2 + list[i].index_len; /* 1 for dot, 2 for ", 2 for [], plus length of array index*/
        }
    }
    char *fulln = gg_malloc (nlen + 1); // +1 for null
    gg_num fullnc = 0; // curr length of normalized name
    for (i = 0; i < list_c; i++)
    {
        // construct normalized name "x"[..]."y"...
        // first ."x" or "x" if first
        if (i != 0) { memcpy (fulln + fullnc, ".\"", 2); fullnc += 2; } // do not include leading dot, just 
                                                                        // in between nodes
        else { memcpy (fulln + fullnc, "\"", 1); fullnc += 1; }
        memcpy (fulln + fullnc, list[i].name, list[i].name_len); fullnc += list[i].name_len;
        memcpy (fulln + fullnc, "\"", 1); fullnc += 1;
        // then if array, add [xxx]
        if (jloc->noenum == false && list[i].index != -1) 
        {
            memcpy (fulln + fullnc, "[", 1); fullnc += 1;
            // output index number, first check for 0
            gg_num al = list[i].index;
            if (al == 0) { fulln[fullnc] = '0'; fullnc += 1; }
            else
            {
                // here, get all digits, fill last first, since we're doing moduo 10
                gg_num k = 0;
                while (al != 0)
                {
                    int r = '0' + (al % 10);
                    fulln[fullnc + list[i].index_len - 1 - k] = r;
                    k++;
                    al = al/10;
                }
                fullnc += k;
            }
            // finish with ]
            memcpy (fulln + fullnc, "]", 1); fullnc += 1;
        }
    }
    fulln[fullnc] = 0;
    gg_mem_set_len (fulln, fullnc+1);
    return fulln;
}

//
// Set the end result of json parsing. 'j' is the json object, 
// noenum is true if no [] in normalized path
// data is string to parse
//
void gg_set_json (gg_json **j, bool noenum, char *data)
{
    // get json object
    *j = (gg_json*)gg_malloc (sizeof(gg_json));

    jloc = *j; // set local processing object
    jloc->noenum = noenum;
    jloc->data = gg_strdupl(data, 0, gg_mem_get_len(data));

}

//
// Delete all allocated data for json j
//
void gg_del_json (gg_json **j)
{
    gg_num i;
    for (i = 0; i < (*j)->node_c; i++)
    {
        gg_free ((*j)->nodes[i].name);
        // do NOT delete str as that's not allocated memory. If user asks, we lazily copy this and deliver,
        // but a lot of times, user does NOT ask, so it saves time 
        if ((*j)->nodes[i].alloced) gg_free ((*j)->nodes[i].str);
    }
    if ((*j)->node_c != 0) gg_free ((*j)->nodes);
    (*j)->node_c = 0;
    gg_free ((*j)->data); // delete data
    gg_free (*j); // delete the entire json structure
    *j = NULL;
}



//
// Make sure json nodes always have room allocated for new elements
// As more elements are added, double the storage, up until 4K blocks
//
static void gg_add_json ()
{
    static gg_num incby;
    
    if (node_tot == 0) incby = GG_JSON_NODES/2; // must start with half, so that initial block below is GG_JSON_NODES, since 
                                    // malloc/realloc choice depends on it
    if (node_c >= node_tot)
    {
        if (incby < 4096) incby *= 2; // initial block is GG_JSON_NODES
        node_tot += incby;
        if (node_tot == GG_JSON_NODES) nodes = gg_malloc (node_tot*sizeof (gg_jsonn));
        else 
        {
            gg_num id = gg_mem_get_id(nodes);
            nodes = gg_realloc (id, node_tot*sizeof (gg_jsonn));
        }
        // initialize nodes to prevent program crashing if developer fails to check the status
        gg_num i;
        for (i = node_c; i < node_tot; i++) {nodes[i].name = GG_EMPTY_STRING; nodes[i].str = GG_EMPTY_STRING; nodes[i].type = GG_JSON_TYPE_STRING; }
    }
}

//
// Returns current error or "" if none.
//
char *gg_json_err()
{
    return errm;
}

//
// parse val as JSON text. 
// len is the length of val, if -1 then we use strlen to figure it out
// curr is the current position from where to start parsing, it's NULL for top call in recursion
// returns -1 if okay, or position of error (as in byte in the whole text) if not. 
// To get error, use gg_json_err()
// if "dec" is 0, do not decode strings, otherwise decode
// if errl is not NULL, store line where error ocurred there
// if errs is not NULL, store char on line where error ocurred there
//
gg_num gg_json_new (char *val, gg_num *curr, gg_num len, char dec, gg_num *errc, gg_num *errl)
{

    char root_call = 0; // 1 if this is top call in recursive processing
    gg_num c;
    gg_num *i;
    if (curr == NULL) 
    { 

        // this is root call
        root_call = 1;
        errm = GG_EMPTY_STRING; // no error by default
        ec = -1; // exit code by default
        line_ec = 1; //line number were error is
        last_line_ec = 0; // byte # of last line processed

        depth = 0; // max dept allowed, currently we're in first (root) invocation
        list_c = -1; // start with root for arrays

        // set byte counter to start from the beginning
        c = 0; 
        i = &c; 

        gg_num j;
        for (j = 0; j < GG_JSON_MAX_NESTED; j++) 
        {
            list[j].index = -1; // array adds 1, so -1 means no array at this level
            list[j].name = NULL;
        }

        // create initial block of normalized nodes
        node_c = 0;
        node_tot = 0; // both node_c and node_tot must be 0 for allocation to work properly, see gg_add_json
        gg_add_json();

    } else i = curr; // inherit byte counter from a recursive parent

    // check if too many nested
    depth ++;
    if (depth >= GG_JSON_MAX_NESTED) { ec = *i; GG_ERR0; errm = gg_strdup(GG_ERR_JSON_DEPTH); goto endj; }
    
    // various flags for checking the validity of JSON doc, mostly what's expected to be found at any point
    char expected_comma_or_end_array = 0;
    char expected_comma_or_end_object = 0;
    char expected_colon = 0;
    char expected_name = 0; // when string is found, it's 1 if it's name, 0 for value. By default, we look for value.
    char isarr = 0; // 0 if not in array [], 1 if in array
    char isobj = 0; // 0 if not in object, 1 if in object

    //
    // JSON text is the same as value. So just "123" is a valid JSON text
    // A JSON value MUST be an object, array, number, or string, or one of the following three literal names: false null true
    //
    gg_num vlen = gg_mem_get_len(val);
    if (len == -1) len = vlen; // len is -1 only in root invocation
    else if (len > vlen) gg_report_error ("Memory used is of length [%ld] but only [%ld] allocated", len, vlen);

    list_c++; // every time value is about to be found, go one level up (and when found, one down)
    // the limit for list_c is GG_JSON_MAX_NESTED -1 so there is always one empty after the last with empty name
    // to mark the end if key-count in read-json isn't used
    if (list_c >= GG_JSON_MAX_NESTED - 1) { ec = *i; GG_ERR0; errm = gg_strdup(GG_ERR_JSON_DEPTH); goto endj; }

    char nchar = 0;
    while (*i < len) // initial value of i is determined at the beginning of this function, which is recursive
    {
        if (nulled != 0) { nchar = nulled; nulled = 0; } else nchar = val[*i];
        if (nchar == '\n') { line_ec++; last_line_ec = *i; }
        switch (nchar) 
        {
            // begin object, zero or more name:value inside separated by commas. Names should be unique.
            case '{':
                if (expected_colon == 1) { ec = *i; GG_ERR0; errm = gg_strdup(GG_ERR_JSON_COLON_EXPECTED); goto endj; }
                if (expected_name == 1) { ec = *i; GG_ERR0; errm = gg_strdup(GG_ERR_JSON_NAME_EXPECTED); goto endj; }
                if (expected_comma_or_end_array == 1) { ec = *i; GG_ERR0; errm = gg_strdup(GG_ERR_JSON_COMMA_END_ARRAY_EXPECTED); goto endj; }
                if (expected_comma_or_end_object == 1) { ec = *i; GG_ERR0; errm = gg_strdup(GG_ERR_JSON_COMMA_END_OBJECT_EXPECTED); goto endj; }
                expected_name = 1;
                isobj = 1;
                (*i)++;
                break;

            // end object
            case '}':
                if (expected_colon == 1) { ec = *i; GG_ERR0; errm = gg_strdup(GG_ERR_JSON_COLON_EXPECTED); goto endj; }
                if (expected_comma_or_end_array == 1) { ec = *i; GG_ERR0; errm = gg_strdup(GG_ERR_JSON_COMMA_END_ARRAY_EXPECTED); goto endj; }
                if (expected_name == 1) { ec = *i; GG_ERR0; errm = gg_strdup(GG_ERR_JSON_NAME_EXPECTED); goto endj; }
                expected_comma_or_end_object = 0;
                isobj = 0;
                list_c --; 
                (*i)++;
                { goto endj; }
                break;

            // begin array, zero or more values inside separated by commas. Values can be of different types.
            case '[':
            {
                if (expected_colon == 1) { ec = *i; GG_ERR0; errm = gg_strdup(GG_ERR_JSON_COLON_EXPECTED); goto endj; }
                if (expected_comma_or_end_array == 1) { ec = *i; GG_ERR0; errm = gg_strdup(GG_ERR_JSON_COMMA_END_ARRAY_EXPECTED); goto endj; }
                if (expected_comma_or_end_object == 1) { ec = *i; GG_ERR0; errm = gg_strdup(GG_ERR_JSON_COMMA_END_OBJECT_EXPECTED); goto endj; }
                if (expected_name == 1) { ec = *i; GG_ERR0; errm = gg_strdup(GG_ERR_JSON_NAME_EXPECTED); goto endj; }
                isarr = 1;
                (*i)++; // get passed [ to find the value that follows
                // use previous element because array applies to it
                list_c--;
                list[list_c].index++; // if index was -1, now it's 0 (first element in array), otherwise increments it
                if (gg_json_new (val, i, len,dec, errc, errl) != -1) { goto endj; }  
                // no incrementing *i because it's done in gg_json_new()
                expected_comma_or_end_array = 1;
                break;
            }
            // end array
            case ']':
                expected_comma_or_end_array = 0;
                if (expected_colon == 1) { ec = *i; GG_ERR0; errm = gg_strdup(GG_ERR_JSON_COLON_EXPECTED); goto endj; }
                if (expected_comma_or_end_object == 1) { ec = *i; GG_ERR0; errm = gg_strdup(GG_ERR_JSON_COMMA_END_OBJECT_EXPECTED); goto endj; }
                if (expected_name == 1) { ec = *i; GG_ERR0; errm = gg_strdup(GG_ERR_JSON_NAME_EXPECTED); goto endj; }
                isarr = 0;
                (*i)++; // get passed ] to find the value that follows
                list[list_c].index = -1; // no longer array at this level
                list_c++; // increase to put it back where it was before we decreased it in [
                list_c --; 
                { goto endj; }
                break;

            // name:value separator
            case ':': 
            {
                expected_colon = 0;
                if (expected_comma_or_end_array == 1) { ec = *i; GG_ERR0; errm = gg_strdup(GG_ERR_JSON_COMMA_END_ARRAY_EXPECTED); goto endj; }
                if (expected_comma_or_end_object == 1) { ec = *i; GG_ERR0; errm = gg_strdup(GG_ERR_JSON_COMMA_END_OBJECT_EXPECTED); goto endj; }
                if (expected_name == 1) { ec = *i; GG_ERR0; errm = gg_strdup(GG_ERR_JSON_NAME_EXPECTED); goto endj; }
                (*i)++; // get passed : to find the value that follows
                if (gg_json_new (val, i, len ,dec, errc, errl) != -1) { goto endj; }  // return value if failed
                // no incrementing *i because it's done in gg_json()
                expected_comma_or_end_object = 1;
                break;
            }

            // value separator
            case ',':
            {
                if (expected_colon == 1) { ec = *i; GG_ERR0; errm = gg_strdup(GG_ERR_JSON_COLON_EXPECTED); goto endj; }
                expected_comma_or_end_array = 0;
                expected_comma_or_end_object = 0;
                if (expected_name == 1) { ec = *i; GG_ERR0; errm = gg_strdup(GG_ERR_JSON_NAME_EXPECTED); goto endj; }
                (*i)++; // get passed : to find the value that follows
                // must be within object or array
                if (isobj ==1) { expected_name = 1; continue;} // if we're in name:value list of pairs, continue to next name
                // if we're in array of values, find the next value
                else if (isarr == 1) list[list_c].index++; // this is next array element, advance the index
                else { ec = *i; GG_ERR0; errm = gg_strdup(GG_ERR_JSON_UNRECOGNIZED); goto endj; }
                if (gg_json_new (val, i, len ,dec, errc, errl) != -1) { goto endj; }  // return value if failed
                // no incrementing *i because it's done in gg_json_new()
                if (isobj == 1) expected_comma_or_end_object = 1;
                if (isarr == 1) expected_comma_or_end_array = 1;
                break;
            }

            // white spaces 
            case ' ':
            case '\t':
            case '\n':
            case '\r':
                (*i)++;
                continue;

            // string
            case '"': 
            {
                if (expected_colon == 1) { ec = *i; GG_ERR0; errm = gg_strdup(GG_ERR_JSON_COLON_EXPECTED); goto endj; }
                if (expected_comma_or_end_array == 1) { ec = *i; GG_ERR0; errm = gg_strdup(GG_ERR_JSON_COMMA_END_ARRAY_EXPECTED); goto endj; }
                if (expected_comma_or_end_object == 1) { ec = *i; GG_ERR0; errm = gg_strdup(GG_ERR_JSON_COMMA_END_OBJECT_EXPECTED); goto endj; }
                char *end;
                // get length of string
                end=gg_text_to_utf (val+*i, 1, &errm, dec, false);
                if (end == NULL) { goto endj; }
                int lstr = (end - (val+*i)); 
                BINDV(end); // put 0 at the end, do NOT set nulled because there's double quote 
                            // which we nulled and we don't want to continue from this double quote
                            // but rather from one byte ahead
                char *str = val + *i + 1;
                (*i) += lstr; // points to final 0, but *i gets increased in for() to get passed it
                if (expected_name == 1) 
                {
                    // this is name in an array of names leading up to name:value
                    list[list_c].name = str;
                    list[list_c].name_len = lstr - 1; // because there's leading quote we're avoiding
                    (*i)++;
                    expected_name = 0;
                    expected_colon = 1;
                }
                else
                {
                    //set node with value
                    GG_ADD_JSON(GG_JSON_TYPE_STRING, str, list_c, list);

                    (*i)++; // increase to get passed 0 byte when it returns
                    list_c --; 
                    { goto endj; }
                }
                break;
            }
            // number
            case '-':
            case '0' ... '9': 
                if (expected_colon == 1) { ec = *i; GG_ERR0; errm = gg_strdup(GG_ERR_JSON_COLON_EXPECTED); goto endj; }
                if (expected_comma_or_end_array == 1) { ec = *i; GG_ERR0; errm = gg_strdup(GG_ERR_JSON_COMMA_END_ARRAY_EXPECTED); goto endj; }
                if (expected_comma_or_end_object == 1) { ec = *i; GG_ERR0; errm = gg_strdup(GG_ERR_JSON_COMMA_END_OBJECT_EXPECTED); goto endj; }
                if (expected_name == 1) { ec = *i; GG_ERR0; errm = gg_strdup(GG_ERR_JSON_NAME_EXPECTED); goto endj; }
                gg_num rv;
                char *r = gg_json_num (val+*i, &rv);
                if (r == NULL) { ec = *i; goto endj; } // errm set in gg_json_num()
                char *str = val + *i;
                BINDV(r); // put 0 at the end
                nulled = tb;
                if (rv == 1) 
                {
                    //set node with value
                    GG_ADD_JSON(GG_JSON_TYPE_REAL, str, list_c, list);
                    ;
                }
                else if (rv == 0) 
                {
                    //set node with value
                    GG_ADD_JSON(GG_JSON_TYPE_NUMBER, str, list_c, list);
                    ;
                }
                else { { ec = *i; GG_ERR0; errm = gg_strdup(GG_ERR_JSON_NUMBER); goto endj; }}
                (*i) += (r - val-*i); 
                list_c --; 
                { goto endj; }

                break;

            // true
            case 't': 
            {
                if (expected_colon == 1) { ec = *i; GG_ERR0; errm = gg_strdup(GG_ERR_JSON_COLON_EXPECTED); goto endj; }
                if (expected_comma_or_end_array == 1) { ec = *i; GG_ERR0; errm = gg_strdup(GG_ERR_JSON_COMMA_END_ARRAY_EXPECTED); goto endj; }
                if (expected_comma_or_end_object == 1) { ec = *i; GG_ERR0; errm = gg_strdup(GG_ERR_JSON_COMMA_END_OBJECT_EXPECTED); goto endj; }
                if (expected_name == 1) { ec = *i; GG_ERR0; errm = gg_strdup(GG_ERR_JSON_NAME_EXPECTED); goto endj; }
                if (strncmp (val+*i, "true", sizeof("true")-1)) { ec = *i; GG_ERR0; errm = gg_strdup(GG_ERR_JSON_UNKNOWN); goto endj; }
                char *str = val+*i;
                BINDV(val+*i+strlen("true")); // put 0 at the end
                nulled = tb;
                //set node with value
                GG_ADD_JSON(GG_JSON_TYPE_BOOL, str, list_c, list);

                (*i) += strlen("true"); // get passed value 
                list_c --; 
                { goto endj; }
                break;
            }

            // false
            case 'f':
            {
                if (expected_colon == 1) { ec = *i; GG_ERR0; errm = gg_strdup(GG_ERR_JSON_COLON_EXPECTED); goto endj; }
                if (expected_comma_or_end_array == 1) { ec = *i; GG_ERR0; errm = gg_strdup(GG_ERR_JSON_COMMA_END_ARRAY_EXPECTED); goto endj; }
                if (expected_comma_or_end_object == 1) { ec = *i; GG_ERR0; errm = gg_strdup(GG_ERR_JSON_COMMA_END_OBJECT_EXPECTED); goto endj; }
                if (expected_name == 1) { ec = *i; GG_ERR0; errm = gg_strdup(GG_ERR_JSON_NAME_EXPECTED); goto endj; }
                if (strncmp (val+*i, "false", sizeof("false")-1)) { ec = *i; GG_ERR0; errm = gg_strdup(GG_ERR_JSON_UNKNOWN); goto endj; }
                char *str = val+*i;
                BINDV(val+*i+strlen("false")); // put 0 at the end
                nulled = tb;
                //set node with value
                GG_ADD_JSON(GG_JSON_TYPE_BOOL, str, list_c, list);

                (*i) += strlen("false"); // get passed value 
                list_c --; 
                { goto endj; }
                break;
            }
            // null
            case 'n':
            {
                if (expected_colon == 1) { ec = *i; GG_ERR0; errm = gg_strdup(GG_ERR_JSON_COLON_EXPECTED); goto endj; }
                if (expected_comma_or_end_array == 1) { ec = *i; GG_ERR0; errm = gg_strdup(GG_ERR_JSON_COMMA_END_ARRAY_EXPECTED); goto endj; }
                if (expected_comma_or_end_object == 1) { ec = *i; GG_ERR0; errm = gg_strdup(GG_ERR_JSON_COMMA_END_OBJECT_EXPECTED); goto endj; }
                if (expected_name == 1) { ec = *i; GG_ERR0; errm = gg_strdup(GG_ERR_JSON_NAME_EXPECTED); goto endj; }
                if (strncmp (val+*i, "null", sizeof("null")-1)) { ec = *i; GG_ERR0; errm = gg_strdup(GG_ERR_JSON_UNKNOWN); goto endj; }
                char *str = val+*i;
                BINDV(val+*i+strlen("null")); // put 0 at the end
                nulled = tb;
                //set node with value
                GG_ADD_JSON(GG_JSON_TYPE_NULL, str, list_c, list);

                (*i) += strlen("null"); // get passed value 
                list_c --; 
                { goto endj; }
                break;
            }
            
            default:
            {
                ec = *i; GG_ERR0; errm = gg_strdup(GG_ERR_JSON_UNRECOGNIZED); goto endj; // unrecognized token
            }
        }
    }
    list_c --; 

endj:    
    depth--;
    if (root_call == 1)
    {
        // final result, this is root call
        jloc->nodes = nodes;
        jloc->node_c = node_c;
        jloc->node_r = 0; // first to read with read-json

        // parser always get json value, and there can be whitespace afterwards. After cleaning it up,
        // it must amount to the full document; otherwise there's something left-over that's not json.
        while (isspace (val[*i])) (*i)++; 

        // check if there's something extra not processed, but only if error not already set
        if (ec == -1 && *i < len)
        {
            ec = *i; GG_ERR0; errm = gg_strdup(GG_ERR_JSON_SYNTAX);
        }
    }
    if (errl != NULL) *errl = line_ec;
    if (errc != NULL) *errc = ec-last_line_ec;
    return ec;
}

//
// rv is return value: 0 for number, 1 for double, 2 if bad
// returns pointer to first byte after number or NULL if failed. Sets errm.
//
static char *gg_json_num(char *val, gg_num *rv)
{
    gg_num i = 0;
    char isdbl = 0;
    
    // get the sign
    if (val[i] == '-')
    {
        i++;
    }
    
    // get the int, check if first char is digit and if 0, check there's nothing else
    gg_num dig = i;
    while (isdigit(val[i])) i++;
    gg_num edig = i;
    if (!isdigit(val[dig])) { GG_ERR0; errm = gg_strdup(GG_ERR_JSON_NUMBER); return NULL;}
    if (val[dig] == '0' && edig != dig+1) { GG_ERR0; errm = gg_strdup(GG_ERR_JSON_NUMBER); return NULL;}

    // get the decimal point
    if (val[edig] == '.')
    {
        i = edig + 1;
        // fraction start with dot to easily convert to double
        while (isdigit(val[i])) i++;
        // check there's at least one digit after dot
        if (!isdigit(val[edig+1])) { GG_ERR0; errm = gg_strdup(GG_ERR_JSON_NUMBER); return NULL;}
        isdbl = 1;
    }

    // get the exponent
    gg_num exp = -1;
    if (val[i] == 'e' || val[i] == 'E')
    {
        i++;
        if (val[i] == '+') { i++;} 
        else if (val[i] == '-') { i++;} 
        exp = i;
        while (isdigit(val[i])) i++;
        // check there's at least one digit in exponent
        if (!isdigit(val[exp])) { GG_ERR0; errm = gg_strdup(GG_ERR_JSON_NUMBER); return NULL;}
        isdbl = 1;
    }

    if (isdbl == 0)
    {
        *rv = 0; // number
    }
    else if (isdbl == 1)
    {
        *rv = 1;
    }
    else
    {
        *rv = 2;
    }
    return val + i;
}




