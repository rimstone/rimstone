// SPDX-License-Identifier: Apache-2.0
// Copyright 2018 Gliim LLC. 
// Licensed under Apache License v2. See LICENSE file.
// On the web http://golf-lang.com/ - this file is part of Golf framework.
//
// gcc -o xml3 xml3.c $(pkg-config --cflags libxml-2.0) -lxml2 -g -std=gnu99


//
//
// This is XML module. At this point it's inoperative, and it's just a first cut for the future XML feature.
//
//
// https://github.com/tiran/defusedxml SECURITY OF XML, NO DTD, NO ENTITY EXPANSION, NO EXTERNAL RESOLUTION, LIMIT PARSE DEPTH, LIMIT INPUT SIZE, LIMIT PARSE TIME, SAX, NO XPATH NO XSL 
//<!DOCTYPE note SYSTEM \"Note.dtd\">
/*char *xmld = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\
<countries>\n\
    <country id='1'>\n\
        <name>USA&amp;America</name>\n\
        <state id='1'>Arizona\n\
            <city id=\"1\">\n\
                <name>Phoenix</name>\n\
                <population>5000000</population>\n\
            </city>\n\
            <city id=\"2\">\n\
                <name>Tuscon</name>\n\
                <population>1000000</population>\n\
            </city>\n\
        </state>\n\
        <state id='2'>California\n\
            <city id=\"1\">\n\
                <name>Los Angeles</name>\n\
                <population>19000000</population>\n\
            </city>\n\
            <city id=\"2\">\n\
                <name>Irvine</name>\n\
            </city>\n\
        </state>\n\
    </country>\n\
    <country id='2'>\n\
        <name>Mexico</name>\n\
        <state id='1'>Veracruz\n\
            <city id=\"1\">\n\
                <name>Xalapa-Enriquez</name>\n\
                <population>8000000</population>\n\
            </city>\n\
            <city id=\"2\">\n\
                <name>C\u00F3rdoba</name>\n\
                <population>220000</population>\n\
            </city>\n\
        </state>\n\
        <state id='2'>Sinaloa\n\
            <city id=\"1\">\n\
                <name>Culiac\u00E1n Rosales</name>\n\
                <population>3000000</population>\n\
            </city>\n\
        </state>\n\
    </country>\n\
</countries>\n";

//#include "golf.h"
#include <libxml/parser.h>
#include <libxml/SAX.h>
#include <string.h>
#include <ctype.h>
#include <libxml/parserInternals.h>
xmlParserCtxtPtr ctxt;

char ename[20][400];
char *eval[20];
int eval_len[20];
int eval_alloc[20];
int ename_len[20];
int dep;
int main_xml() ;
void process_start(
    void *ctx,
    const xmlChar *localname,
    const xmlChar *prefix,
    const xmlChar *URI,
    int nb_namespaces,
    const xmlChar **namespaces,
    int nb_attributes,
    int nb_defaulted,
    const xmlChar **attributes
);
void xml_err(void *userData, xmlErrorPtr error);
void process_end(
    void* ctx,
    const xmlChar* localname,
    const xmlChar* prefix,
    const xmlChar* URI
);
void process_chars(void* ctx, const xmlChar * ch, int len);

// Your custom SAX handler functions
void process_start(
    void *ctx,
    const xmlChar *localname,
    const xmlChar *prefix,
    const xmlChar *URI,
    int nb_namespaces,
    const xmlChar **namespaces,
    int nb_attributes,
    int nb_defaulted,
    const xmlChar **attributes
)
{
    //GG_UNUSED(ctx);
    //GG_UNUSED(prefix);
    //GG_UNUSED(URI);
    //GG_UNUSED(nb_namespaces);
    //GG_UNUSED(namespaces);
    //GG_UNUSED(nb_defaulted);
    //printf("\n------\n");

    strcpy (ename[dep+1], ename[dep]);
    ename_len[dep+1]=ename_len[dep];
    dep++;
    ename_len[dep]+=snprintf (ename[dep]+ename_len[dep], sizeof(ename[dep])-ename_len[dep], "%s/", localname);

    //printf ("'%s'=", ename[dep]);
    unsigned int index = 0;
    // now the attributes
    int i_att;
    for (i_att = 0; i_att < nb_attributes; ++i_att, index += 5)
      {
        const xmlChar *localname = attributes[index];
        const xmlChar *begv = attributes[index + 3];
        const xmlChar *endv = attributes[index + 4];
        int alen = endv-begv;
        strcpy (ename[dep+1], ename[dep]);
        ename_len[dep+1]=ename_len[dep];
        dep++;
        ename_len[dep]+=snprintf (ename[dep]+ename_len[dep], sizeof(ename[dep])-ename_len[dep], "%s/@", localname);

        printf ("'%s'='%.*s'\n", ename[dep], alen, begv);
        dep--;
      }
}

void process_end(
    void* ctx,
    const xmlChar* localname,
    const xmlChar* prefix,
    const xmlChar* URI
)
{
    //GG_UNUSED(ctx);
    //GG_UNUSED(prefix);
    //GG_UNUSED(URI);
    //GG_UNUSED(localname);
    //
    //TODO:check localname to be in eval[]
    //
    //printf("\nENDOF '%s'", localname);
    int i = 0;
    while (isspace(eval[dep][i])) i++;
    if (eval[dep][i] != 0) 
    {
        int j = strlen(eval[dep]);
        while (isspace(eval[dep][j-1])) j--;
        eval[dep][j] = 0;
        printf("'%s$'='%s'\n", ename[dep], eval[dep]+i);
    }
    eval[dep][0] = 0;
    eval_len[dep] = 0;
    dep--;
}

void process_chars(void* ctx, const xmlChar * ch, int len)
{
    //GG_UNUSED(ctx);
    int l;
    int alen = (len < 256 ? 256:len);
    if (eval[dep] == NULL) {eval_alloc[dep] = alen; eval[dep] = malloc (eval_alloc[dep]);}
    //else {if (eval_len[dep]+len >= eval_alloc[dep]) eval_alloc[dep] += alen; eval[dep] = realloc(gg_mem_get_id(eval[dep]), eval_alloc[dep]); }
    else {if (eval_len[dep]+len >= eval_alloc[dep]) eval_alloc[dep] += alen; eval[dep] = realloc(eval[dep], eval_alloc[dep]); }
    memcpy (eval[dep]+eval_len[dep], ch, len);
    eval_len[dep] += len;
    eval[dep][eval_len[dep]]= 0;
}

void xml_err(void *userData, xmlErrorPtr error) {
    //GG_UNUSED(userData);
    fprintf(stderr, "Message: %s", error->message);
    fprintf(stderr, "Line: %d\n", error->line);
    fprintf(stderr, "Position:%d\n", error->int2);
    xmlSetStructuredErrorFunc(NULL, NULL);
}


int main_xml() {
    int i;
    for (i = 0; i < 20; i ++)
    {
        ename[i][0] = 0;
        eval[i] = NULL;
        eval_len[i] = 0;
        eval_alloc[i] = 0;
        ename_len[i] = 0;
    }
    dep = 0;
    xmlSAXHandler handler;
    memset(&handler, 0, sizeof(handler));

    // Set up handler functions
    handler.startElementNs = process_start;
    handler.endElementNs = process_end;
    handler.characters = process_chars;
    handler.initialized = XML_SAX2_MAGIC;
    // ... set other handler functions as needed

    xmlSetStructuredErrorFunc(NULL, xml_err);


    xmlDocPtr doc;
    ctxt = xmlCreatePushParserCtxt( &handler, NULL, xmld, strlen(xmld), NULL);
    xmlCtxtUseOptions		(ctxt, XML_PARSE_NONET|XML_PARSE_HUGE|XML_PARSE_IGNORE_ENC);

    // finish parsing
    xmlParseChunk(ctxt, "", 0, 1);


    doc = ctxt->myDoc; //destroy the tree since not used
    xmlFreeParserCtxt(ctxt);
    if (doc != NULL) xmlFreeDoc(doc);

    // this is once per program
    xmlCleanupParser();

    return 0;

}

int main() { main_xml(); }*/
