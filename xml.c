// SPDX-License-Identifier: Apache-2.0
// Copyright 2019 Gliim LLC. 
// Licensed under Apache License v2. See LICENSE file.
// On the web http://golf-lang.com/ - this file is part of Golf framework.

// 
// XML-related module
//


#include "golf.h"

void visit_node(xmlNode * a_node);
gg_num parse_xml(char *text, char **err);
char *xmld = "<?xml version=\"1.0\"?>\
<catalog>\
   <book id=\"bk101\">\
      <author>Gambardella, Matthew</author>WOW BOOK!\
      <title>XML Developer's Guide</title>\
      <genre>Computer</genre>\
      <price>44.95</price>\
      <publish_date>2000-10-01</publish_date>\
      <description>An in-depth look at creating applications \
      with XML.</description>\
      Great book!\
   </book>\
   <book id=\"bk102\">\
      <author>Ralls, Kim</author>\
      <title>Midnight Rain</title>\
      <genre>Fantasy</genre>\
      <price>5.95</price>\
      <publish_date>2000-12-16</publish_date>\
      <description>A former architect battles corporate zombies, \
      an evil sorceress, and her own childhood to become queen \
      of the world.</description>\
   </book>\
   <book id=\"bk103\">\
      <author>Corets, Eva</author>\
      <title>Maeve Ascendant</title>\
      <genre>Fantasy</genre>\
      <price>5.95</price>\
      <publish_date>2000-11-17</publish_date>\
      <description>After the collapse of a nanotechnology \
      society in England, the young survivors lay the \
      foundation for a new society.</description>\
   </book>\
   <book id=\"bk104\">\
      <author>Corets, Eva</author>\
      <title>Oberon's Legacy</title>\
      <genre>Fantasy</genre>\
      <price>5.95</price>\
      <publish_date>2001-03-10</publish_date>\
      <description>In post-apocalypse England, the mysterious \
      agent known only as Oberon helps to create a new life \
      for the inhabitants of London. Sequel to Maeve \
      Ascendant.</description>\
   </book>\
   <book id=\"bk105\">\
      <author>Corets, Eva</author>\
      <title>The Sundered Grail</title>\
      <genre>Fantasy</genre>\
      <price>5.95</price>\
      <publish_date>2001-09-10</publish_date>\
      <description>The two daughters of Maeve, half-sisters, \
      battle one another for control of England. Sequel to \
      Oberon's Legacy.</description>\
   </book>\
   <book id=\"bk106\">\
      <author>Randall, Cynthia</author>\
      <title>Lover Birds</title>\
      <genre>Romance</genre>\
      <price>4.95</price>\
      <publish_date>2000-09-02</publish_date>\
      <description>When Carla meets Paul at an ornithology \
      conference, tempers fly as feathers get ruffled.</description>\
   </book>\
   <book id=\"bk107\">\
      <author>Thurman, Paula</author>\
      <title>Splish Splash</title>\
      <genre>Romance</genre>\
      <price>4.95</price>\
      <publish_date>2000-11-02</publish_date>\
      <description>A deep sea diver finds true love twenty \
      thousand leagues beneath the sea.</description>\
   </book>\
   <book id=\"bk108\">\
      <author>Knorr, Stefan</author>\
      <title>Creepy Crawlies</title>\
      <genre>Horror</genre>\
      <price>4.95</price>\
      <publish_date>2000-12-06</publish_date>\
      <description>An anthology of horror stories about roaches,\
      centipedes, scorpions  and other insects.</description>\
   </book>\
   <book id=\"bk109\">\
      <author>Kress, Peter</author>\
      <title>Paradox Lost</title>\
      <genre>Science Fiction</genre>\
      <price>6.95</price>\
      <publish_date>2000-11-02</publish_date>\
      <description>After an inadvertant trip through a Heisenberg\
      Uncertainty Device, James Salway discovers the problems \
      of being quantum.</description>\
   </book>\
   <book id=\"bk110\">\
      <author>O'Brien, Tim</author>\
      <title>Microsoft .NET: The Programming Bible</title>\
      <genre>Computer</genre>\
      <price>36.95</price>\
      <publish_date>2000-12-09</publish_date>\
      <description>Microsoft's .NET initiative is explored in \
      detail in this deep programmer's reference.</description>\
   </book>\
   <book id=\"bk111\">\
      <author>O'Brien, Tim</author>SOME TEXT111\
      <title>MSXML3: A Comprehensive Guide</title>\
      <genre>Computer</genre>\
      <price>36.95</price>\
      <publish_date>2000-12-01</publish_date>\
      <description>The Microsoft MSXML3 parser is covered in \
      detail, with attention to XML DOM interfaces, XSLT processing, \
      SAX and more.</description>\
   </book>\
   <book id=\"bk112\">SOME TEXT112\
      <author>Galos, Mike</author>\
      <title>Visual Studio 7: A Comprehensive Guide</title>\
      <genre>Computer</genre>\
      <price>49.95</price>\
      <publish_date>2001-04-16</publish_date>\
      <description>Microsoft Visual Studio 7 is explored in depth,\
      looking at how Visual Basic, Visual C++, C#, and ASP+ are \
      integrated into a comprehensive development \
      environment.</description>\
   </book>\
</catalog>";

void visit_node(xmlNode * a_node) {
    xmlNode *xnode = NULL;

    for (xnode = a_node; xnode; xnode = xnode->next) {
        if (xnode->type == XML_ELEMENT_NODE) {

            xmlAttrPtr attr = xnode->properties;
            while (attr != NULL) {
                xmlChar *aval = xmlGetProp(xnode, attr->name);
                gg_num aval_len = strlen ((char*)aval);
                char *attr_val = gg_trim_ptr ((char*)aval, &aval_len);
                printf("Attribute name for [%s]: [%s], value: [%s]\n", xnode->name,
                       attr->name, attr_val);
                if (aval != NULL) xmlFree (aval);
                attr = attr->next;
            }

            if (xnode->children) 
            {
                visit_node(xnode->children);
            }
        }
        else if (xnode->type == XML_TEXT_NODE && xmlIsBlankNode (xnode) == 0) {
            gg_num cont_len = strlen ((char*)xnode->content);
            char *cont = gg_trim_ptr ((char*)xnode->content, &cont_len);
            printf("Text content for [%s]: [%s]\n", xnode->parent->name, cont);
        }
    }
}

gg_num parse_xml(char *text, char **err) {
    xmlNode *xml_root = NULL;
    xmlDocPtr doc;

    text=xmld;
    LIBXML_TEST_VERSION;

    doc = xmlParseDoc((const xmlChar*)text);

    if (doc == NULL) {
        if (err != NULL) 
        {
            xmlErrorPtr error = xmlGetLastError();
            if (error != NULL) *err = gg_strdup (error->message);
        }
        return GG_ERR_XML_PARSE;
    }

    // get XML root
    xml_root = xmlDocGetRootElement(doc);

    if (xml_root == NULL) {
        xmlFreeDoc(doc);
        return GG_ERR_XML_EMPTY;
    }

    // Print element names
    visit_node(xml_root);

    // Free the document
    xmlFreeDoc(doc);

    xmlCleanupParser();

    return 0;
}

/*

#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <xml_file>\n", argv[0]);
        return 1;
    }

    xmlDocPtr doc = xmlReadFile(argv[1], NULL, 0);
    if (doc == NULL) {
        fprintf(stderr, "Failed to parse document\n");
        return 1;
    }

    xmlNodePtr root = xmlDocGetRootElement(doc);
    if (root == NULL) {
        fprintf(stderr, "Empty document\n");
        xmlFreeDoc(doc);
        return 1;
    }

    xmlNodePtr cur = root->children;
    while (cur != NULL) {
        if (cur->type == XML_ELEMENT_NODE) {
            printf("Element name: %s\n", cur->name);

            xmlAttrPtr attr = cur->properties;
            while (attr != NULL) {
                printf("  Attribute name: %s, value: %s\n", 
                       attr->name, 
                       xmlGetProp(cur, attr->name));
                attr = attr->next;
            }

            if (cur->children && cur->children->type == XML_TEXT_NODE) {
                printf("  Text content: %s\n", cur->children->content);
            }
        }

        cur = cur->next;
    }

    xmlFreeDoc(doc);
    return 0;
}
*/


