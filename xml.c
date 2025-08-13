// SPDX-License-Identifier: Apache-2.0
// Copyright 2018-2025 Gliim LLC. 
// Licensed under Apache License v2. See LICENSE file.
// On the web http://golf-lang.com/ - this file is part of Golf framework.
//

//
//
// This is XML module. At this point it's inoperative, and it's just a first cut for the future XML feature.
//
//
// https://github.com/tiran/defusedxml SECURITY OF XML, NO DTD, NO ENTITY EXPANSION, NO EXTERNAL RESOLUTION, LIMIT PARSE DEPTH, LIMIT INPUT SIZE, LIMIT PARSE TIME, SAX, NO XPATH NO XSL 
//
// Note that XML, unlike JSON, can be encoded in any way (while JSON is *always* UTF8), so any decodings must be done once data is extracted! (meaning text-utf)
#include "golf.h"



#ifdef DEBUG
#define GG_XML_NODES 32
#else
#define GG_XML_NODES 1024
#endif

#define GG_XML_MAX_DEPTH 32
#define GG_XML_MAX_NAME_LEN 500

// Internal structure that holds data needed to parse a single XML document
// The parser walks the documents in order of appearance of the closing tag and provides name/value pairs
// Meaning it's a fast SAX parsing, there's no document building (no DOM)
typedef struct s_gg_ixml {
    xmlParserCtxtPtr xml_parser; // parsing context from the library
    char ename[GG_XML_MAX_DEPTH][GG_XML_MAX_NAME_LEN]; // normalized names
    char *eval[GG_XML_MAX_DEPTH]; // values
    gg_num eval_len[GG_XML_MAX_DEPTH]; // values lengths in bytes
    gg_num eval_alloc[GG_XML_MAX_DEPTH]; // how much memory actually allocated for values
    gg_num ename_len[GG_XML_MAX_DEPTH];  // normalized name lengths
    int dep;
} gg_ixml;


static gg_xml *xloc = NULL; // final result, super structure for the whole xml document
static gg_num node_tot = 0; //node total alloc'd
static gg_num node_c = 0; //node counter (current)
static gg_xmln *nodes = NULL; // nodes of normalized xml (name, value)
static gg_ixml xp; // context of a single parse
static gg_num char_ec = -1; // char within line where error happened
static gg_num line_ec = -1; // line where error happened (1..)
static char *errm; // error message
static bool is_error; // if true, there was an error
static void gg_add_xml (); // add more memory for XML nodes

// add name/value pair to the list of nodes (names are normalized)
#define GG_ADD_XML(x_name, x_str) { gg_add_xml(); nodes[node_c].name = x_name; nodes[node_c].str = x_str; node_c++; }; 


//
// Set local static variable xloc to Golf's variable x, which is created here
// This local variable is used in parsing, since it's entirely encapsulated here
//
void gg_set_xml (gg_xml **x)
{
    // get json object
    *x = (gg_xml*)gg_malloc (sizeof(gg_xml));

    xloc = *x; // set local processing object

}

// prototypes
static void gg_xml_beg( void *ctx, const xmlChar *localname, const xmlChar *prefix, const xmlChar *URI, int nb_namespaces, const xmlChar **namespaces, int nb_attributes, int nb_defaulted, const xmlChar **attributes);
static void gg_xml_seterr(void *userData, const struct _xmlError *error);
static void gg_xml_end( void* ctx, const xmlChar* localname, const xmlChar* prefix, const xmlChar* URI);
static void gg_xml_data(void* ctx, const xmlChar * ch, int len);

// 
// Called when there's a a beginning of XML node or attribute. See libxml2 doc for input parameters. Some are ignored b/c
// we don't do validation
//
static void gg_xml_beg( void *ctx, const xmlChar *localname, const xmlChar *prefix, const xmlChar *URI, int nb_namespaces, const xmlChar **namespaces, int nb_attributes, int nb_defaulted, const xmlChar **attributes)
{
    GG_UNUSED(ctx);
    GG_UNUSED(prefix);
    GG_UNUSED(URI);
    GG_UNUSED(nb_namespaces);
    GG_UNUSED(namespaces);
    GG_UNUSED(nb_defaulted);

    // Copy previous name to the current one (one deeper in the hierarchy), so that normalized name path is ready 
    memcpy (xp.ename[xp.dep+1], xp.ename[xp.dep], (xp.ename_len[xp.dep+1]=xp.ename_len[xp.dep])+1);
    xp.dep++; // a new node is one level deeper in XML hierarchy of nodes
              //
    // bufsize is the the space left for this node (and all deeper ones that may follow)
    gg_num bufsize = sizeof(xp.ename[xp.dep])-xp.ename_len[xp.dep];
    gg_num localen = strlen ((char*)localname); // localname is the name of the xml node
    // check 1+1 for / and null byte, to see if the normalized name is too long
    if (localen + 1 + 1 > bufsize) gg_report_error ("XML element is too long [%s...]", xp.ename[xp.dep]);
    // add this node to ../../.. normalized name and finish with /
    memcpy (xp.ename[xp.dep]+xp.ename_len[xp.dep], localname,  localen);
    memcpy (xp.ename[xp.dep]+xp.ename_len[xp.dep]+localen, "/", 2);
    xp.ename_len[xp.dep] += localen+1; // +1 is for /

    unsigned int index = 0;
    // now process any attributes associated with this node
    int i_att;
    for (i_att = 0; i_att < nb_attributes; ++i_att, index += 5)
    {
        const xmlChar *localname = attributes[index];
        // beginning and ending of attribute value
        const xmlChar *begv = attributes[index + 3];
        const xmlChar *endv = attributes[index + 4];
        // attribute's name is one deeper in hierarchy (see comment above)
        memcpy (xp.ename[xp.dep+1], xp.ename[xp.dep], (xp.ename_len[xp.dep+1]=xp.ename_len[xp.dep])+1);
        xp.dep++;
        gg_num bufsize = sizeof(xp.ename[xp.dep])-xp.ename_len[xp.dep]; // bytes left for a normalized name
        gg_num att_len = endv-begv; // attribute value length
        gg_num localen = strlen ((char*)localname); // name of attribute and it's length
        // check 1+1 for / and null byte, if too long a normalized name
        if (localen + 1 + 1 > bufsize) gg_report_error ("XML element name is too long [%s...]", xp.ename[xp.dep]);
        // create full normalized name for this attribute and finish it with @
        memcpy (xp.ename[xp.dep]+xp.ename_len[xp.dep], localname, localen);
        memcpy (xp.ename[xp.dep]+xp.ename_len[xp.dep]+localen, "/@", 3);
        xp.ename_len[xp.dep]+=localen+2; // +2 is for /@

        // attributes and node values are allocated, so copy and finish attribute with a null-char
        // the size of attribute is exactly correct (as it includes +1 byte for null) for the purpose of memory safety
        char *att = gg_malloc(att_len+1);
        memcpy (att, (char*)begv, att_len);
        att[att_len] = 0;
        // add node to the list
        GG_ADD_XML(gg_strdupl(xp.ename[xp.dep],0,xp.ename_len[xp.dep]), att);
        xp.dep--; // go one level shallower in hierarchy, since we just finished processing an attribute
    }
}

//
// Process the end of a node
//
static void gg_xml_end( void* ctx, const xmlChar* localname, const xmlChar* prefix, const xmlChar* URI
)
{
    GG_UNUSED(ctx);
    GG_UNUSED(prefix);
    GG_UNUSED(URI);
    GG_UNUSED(localname);

    if (xp.eval[xp.dep] == NULL) { xp.dep--;return;} // node may have been without a value
                                                     // so skip it (typically a node that only contains other nodes)
    // At this point, value has already been filled in gg_xml_data() callback,
    // so nothing to do other than to trim data (which updates its length with Golmem for memory safety)
    // It is IMPORTANT not to miss setting length, if gg_trim is not used, then MUST USE gg_mem_set_len()!
    //
    gg_trim (xp.eval[xp.dep], &(xp.eval_len[xp.dep]), true); // trim the value of a node (on left and right), if not used, MUST use gg_mem_set_len !!
    // realloc mem so we don't use +256 etc for super small blocks of data! This should
    // fast, since we're effectively shrinking the last memory we realloced
    // 
    //
    // and for that reason, to avoid fragmentation, we must do this BEFORE gg_maloc below!!
    //
    //
    xp.eval[xp.dep] = gg_realloc(gg_mem_get_id(xp.eval[xp.dep]), xp.eval_len[xp.dep]+1); 
                                                             //
    gg_num namel = xp.ename_len[xp.dep]; // length of normalized name
    // allocate memory for name to return to Golf and place null at the end
    char *name = gg_malloc (namel+1);
    memcpy (name, xp.ename[xp.dep], namel);
    name[namel] = 0;
    // add name/value to list of xml nodes
    GG_ADD_XML(name, xp.eval[xp.dep]);
    // clear out this node (at depth xp.dep) so the next node/attribute that comes along at that depth in hierarchy doesn't
    // concatenate to some old random values
    xp.eval[xp.dep] = NULL;
    xp.eval_len[xp.dep] = 0;
    xp.dep--; // processed node, go down in depth
}


//
// Callback from libxml2 that processes data. It's often delivered in chunks, so even the very same continuous node value
// could be delivered as more than one chunk.
//
static void gg_xml_data(void* ctx, const xmlChar * ch, int len)
{
    GG_UNUSED(ctx);
    int alen = (len < 256 ? 256:len+256); // allocate more memory so we don't realloc when (and if) another chunk comes in
    // Because this XML parser generally returns data in chunks (and not a whole), we have to realloc if and when more data comes along
    // but we do allocate more to minimize realloc's
    // Being NULL is the sign we need to malloc, otherwise realloc
    if (xp.eval[xp.dep] == NULL) {xp.eval_alloc[xp.dep] = alen; xp.eval[xp.dep] = gg_malloc (xp.eval_alloc[xp.dep]);}
    else 
    {
        // if not enough space, add a chunk and then reall
        if (xp.eval_len[xp.dep]+len >= xp.eval_alloc[xp.dep]) xp.eval_alloc[xp.dep] += alen; 
        xp.eval[xp.dep] = gg_realloc(gg_mem_get_id(xp.eval[xp.dep]), xp.eval_alloc[xp.dep]); 
    }
    // Copy data to further complete the node value
    memcpy (xp.eval[xp.dep]+xp.eval_len[xp.dep], ch, len);
    xp.eval_len[xp.dep] += len; // update its length
    xp.eval[xp.dep][xp.eval_len[xp.dep]]= 0; // set length
    // length will be set in gg_xml_end()
}

//
// Called when there's an error. Set error message (errm), line_ec/char_ec and is_error static flags
// to be given to the caller in v1.c
//
static void gg_xml_seterr(void *userData, const struct _xmlError *error)
{
    GG_UNUSED(userData);
    errm = gg_strdup(error->message);
    line_ec = error->line;
    char_ec = error->int2;
    is_error = true;
    //xmlSetStructuredErrorFunc(NULL, NULL);
}

//
// Golf code gets error message this way, since errm is static
//
char *gg_xml_err()
{
    return errm;
}

//
// Add another chunk of XML nodes
//
void gg_add_xml ()
{
    static gg_num incby;
    
    if (node_tot == 0) incby = GG_XML_NODES/2; // must start with half, so that initial block below is GG_XML_NODES, since 
                                    // malloc/realloc choice depends on it
    if (node_c >= node_tot)
    {
        if (incby < 4096) incby *= 2; // initial block is GG_XML_NODES
        node_tot += incby;
        if (node_tot == GG_XML_NODES) nodes = gg_malloc (node_tot*sizeof (gg_xmln));
        else 
        {
            gg_num id = gg_mem_get_id(nodes);
            nodes = gg_realloc (id, node_tot*sizeof (gg_xmln));
        }
        // initialize nodes to prevent program crashing if developer fails to check the status
        gg_num i;
        for (i = node_c; i < node_tot; i++) {nodes[i].name = GG_EMPTY_STRING; nodes[i].str = GG_EMPTY_STRING; }
    }
}

//
// Delete XML x, first all its key/value pairs and then the whole nodes structure
//
void gg_del_xml (gg_xml **x)
{
    gg_num i;
    for (i = 0; i < (*x)->node_c; i++)
    {
        gg_free ((*x)->nodes[i].name);
        gg_free ((*x)->nodes[i].str);
    }
    if ((*x)->node_c != 0) gg_free ((*x)->nodes);
    (*x)->node_c = 0;
    gg_free (*x); // delete the entire xml structure
    *x = NULL;
}


//
// Parse XML. val is data to parse, len is its length, errc/errl and char and line where error may have happened
// Returns 0 if error, -1 if okay 
//
gg_num gg_xml_new (char *val, gg_num len, gg_num *errc, gg_num *errl)
{

    // create initial block of normalized nodes
    node_c = 0;
    node_tot = 0; // both node_c and node_tot must be 0 for allocation to work properly, see gg_add_json
    gg_add_xml(); // add initial chunk of nodes
    is_error = false; // no error initially
    errm = GG_EMPTY_STRING; // no error text initially

    // Initialize hierarchy of XML nodes up to maximum depth. We reuse this set of depth data, so once a node
    // is processed say in depth 5 (xp.ename[4], xp.eval[4] etc.), some other node who comes later at the same
    // depth will use it again. 
    int i;
    for (i = 0; i < GG_XML_MAX_DEPTH; i ++)
    {
        xp.ename[i][0] = 0;
        xp.eval[i] = NULL;
        xp.eval_len[i] = 0;
        xp.eval_alloc[i] = 0;
        xp.ename_len[i] = 0;
    }
    xp.dep = 0; // initially depth 0

    // Init SAX XML handler
    xmlSAXHandler handler;
    memset(&handler, 0, sizeof(handler));

    // Set up handler functions
    handler.startElementNs = gg_xml_beg; // beginning of node
    handler.endElementNs = gg_xml_end; // end of done
    handler.characters = gg_xml_data; // data of node
    handler.initialized = XML_SAX2_MAGIC; 

    xmlSetStructuredErrorFunc(NULL, (xmlStructuredErrorFunc)gg_xml_seterr); // set error callback

    // use either length of data, or passed length, but passed length cannot be greater than
    // the actually data length (memory safety)
    gg_num vlen = gg_mem_get_len(val);
    if (len == -1) len = vlen; // len is -1 only in root invocation
    else if (len > vlen) len = vlen; // do not cause segmentation violation

    // Do XML parsing
    // We do NOT ever use any network validations. All parsing is strictly local.
    // We allow large XML.
    xmlDocPtr doc;
    xp.xml_parser = xmlCreatePushParserCtxt( &handler, NULL, val, len, NULL);
    xmlCtxtUseOptions		(xp.xml_parser, XML_PARSE_NONET|XML_PARSE_HUGE|XML_PARSE_IGNORE_ENC);

    // finish parsing
    xmlParseChunk(xp.xml_parser, "", 0, 1);

    // Since we don't use DOM, release it
    doc = xp.xml_parser->myDoc; //destroy the tree since not used
    xmlFreeParserCtxt(xp.xml_parser); // release resources for parsing
    if (doc != NULL) xmlFreeDoc(doc);

    // this is once per program, it should be at the end. Since it may (or may not?) cause sigsegv, we're omitting it as exiting will release all resources
    // xmlCleanupParser();

    // Get error info, if requested
    if (errl) *errl = line_ec;
    if (errc) *errc = char_ec;

    // Set actual nodes to be with the XML variable defined in Golf program, so that developer can use it there
    xloc->nodes = nodes;
    xloc->node_c = node_c;
    xloc->node_r = 0;  // iteration set to first node

    return is_error ? 0:-1; // success or failure
}

