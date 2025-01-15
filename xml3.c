// SPDX-License-Identifier: Apache-2.0
// Copyright 2019 Gliim LLC. 
// Licensed under Apache License v2. See LICENSE file.
// On the web http://golf-lang.com/ - this file is part of Golf framework.


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

#include "golf.h"
#include <libxml/parser.h>
#include <libxml/SAX.h>
#include <string.h>
#include <ctype.h>
#include <libxml/parserInternals.h>
xmlParserCtxtPtr ctxt;

char ename[20][400];
char eval[20][400];
int elen[20];
int dep;
int main_xml() ;
void OnStartElementNs(
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
void OnEndElementNs(
    void* ctx,
    const xmlChar* localname,
    const xmlChar* prefix,
    const xmlChar* URI
);
void OnCharacters(void* ctx, const xmlChar * ch, int len);

// Your custom SAX handler functions
void OnStartElementNs(
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
    GG_UNUSED(ctx);
    GG_UNUSED(prefix);
    GG_UNUSED(URI);
    GG_UNUSED(nb_namespaces);
    GG_UNUSED(namespaces);
    GG_UNUSED(nb_defaulted);
    //printf("\n------\n");

    strcpy (ename[dep+1], ename[dep]);
    elen[dep+1]=elen[dep];
    dep++;
    elen[dep]+=snprintf (ename[dep]+elen[dep], sizeof(ename[dep])-elen[dep], "%s/", localname);

    //printf ("'%s'=", ename[dep]);
    unsigned int index = 0;
    // now the attributes
    int indexAttribute;
    for (indexAttribute = 0; indexAttribute < nb_attributes; ++indexAttribute, index += 5)
      {
        const xmlChar *localname = attributes[index];
        const xmlChar *valueBegin = attributes[index + 3];
        const xmlChar *valueEnd = attributes[index + 4];
        int alen = valueEnd-valueBegin;
        strcpy (ename[dep+1], ename[dep]);
        elen[dep+1]=elen[dep];
        dep++;
        elen[dep]+=snprintf (ename[dep]+elen[dep], sizeof(ename[dep])-elen[dep], "%s/@", localname);

        //printf ("  %sattribute: localname='%s',  value='%s' ", indexAttribute >= (nb_attributes - nb_defaulted) ? "defaulted " : "", ename[dep], valueBegin);
        printf ("'%s'='%.*s'\n", ename[dep], alen, valueBegin);
        dep--;
      }
}

void OnEndElementNs(
    void* ctx,
    const xmlChar* localname,
    const xmlChar* prefix,
    const xmlChar* URI
)
{
    GG_UNUSED(ctx);
    GG_UNUSED(prefix);
    GG_UNUSED(URI);
    GG_UNUSED(localname);
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
    dep--;
}

void OnCharacters(void* ctx, const xmlChar * ch, int len)
{
    GG_UNUSED(ctx);
    int l;
    memcpy (eval[dep]+(l=strlen(eval[dep])), ch, len);
    (eval[dep]+l)[len]= 0;
}

void xml_err(void *userData, xmlErrorPtr error) {
    GG_UNUSED(userData);
    fprintf(stderr, "Error: %s %d %s %s %s %d\n", error->message, error->line, error->str1, error->str2, error->str3, error->int2);
    // Handle the error, e.g., log it or exit
    xmlSetStructuredErrorFunc(NULL, NULL);
}


int main_xml() {
    int i;
    for (i = 0; i < 20; i ++)
    {
        ename[i][0] = 0;
        eval[i][0] = 0;
        elen[i] = 0;
    }
    dep = 0;
    xmlSAXHandler handler;
    memset(&handler, 0, sizeof(handler));

    // Set up handler functions
    handler.initialized = XML_SAX2_MAGIC;
    handler.startElementNs = OnStartElementNs;
    handler.endElementNs = OnEndElementNs;
    handler.characters = OnCharacters;
    // ... set other handler functions as needed

    xmlSetStructuredErrorFunc(NULL, xml_err);


    xmlDocPtr doc;
    ctxt = xmlCreatePushParserCtxt( &handler, NULL, xmld, strlen(xmld), NULL);
    xmlCtxtUseOptions		(ctxt, XML_PARSE_NONET|XML_PARSE_HUGE|XML_PARSE_IGNORE_ENC);

    // Parse the memory buffer
    //xmlParseChunk(ctxt, xmld, strlen(xmld), 0);
    xmlParseChunk(ctxt, "", 0, 1);


    doc = ctxt->myDoc; //destroy the tree since not used
    xmlFreeParserCtxt(ctxt);
    if (doc != NULL) xmlFreeDoc(doc);

    // this is once per program
    xmlCleanupParser();

    return 0;

}

//int main() { main_xml(); }*/
