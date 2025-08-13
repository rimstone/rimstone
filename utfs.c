// SPDX-License-Identifier: Apache-2.0
// Copyright 2018-2025 Gliim LLC. 
// Licensed under Apache License v2. See LICENSE file.
// On the web http://golf-lang.com/ - this file is part of Golf framework.

// 
// UTF-related module
//

//
// Based on UTF standard: https://www.rfc-editor.org/rfc/rfc3629
//

#include "golf.h"

//
// Obtain the string from text "val", which starts with a first byte after double quote. 
// This string can be name or a string value in text. Sets errm.
// quoted is 1 if string is double quoted (as is with text)
// returns pointer to first byte after the string or NULL on error
// If 'enc' is 0, do not enccode strings at all.
// alloced is true if val is allocated memory
//
char *gg_text_to_utf (char *val, char quoted, char **o_errm, char enc, bool alloced)
{
    *o_errm = GG_EMPTY_STRING;
    gg_num i = 1; // if quoted, start right after first char (which is a quote)
    if (quoted == 0) i = 0; // if not quoted, start from the beginning of string

    gg_num pull = 0; // when interpreting escaped value, current tally of how many byte to copy current byte
                  // all val[] assignments subtract "pull" when being assigned

    if (enc == 0)
    {
        // this is no-encode
        while (val[i] != 0) // do not go past the end of string
        {
            if (val[i] == '\\' && val[i+1] != 0) i+=2; // if there is an escape, get passed both chars
            else if (val[i] == '"')  // found unescaped quote
            {
                // stop with unescaped quote, if our string is quoted, otherwise a quote is nothing special
                // and just continue until null char
                if (quoted == 1) break; else continue;
                i++;
            } else i++;
        }
    }
    else
    {
        // this is "encode", i.e. convert strings to binary format
        //
        // look for closing quote of end of string, or zero character. A quote inside is escaped and would be processed, so it would
        // not be caught here. Depending on whether quoted is 0 or 1, either one may be valid ending
        while (val[i] != '"' && val[i] != 0) 
        {
            // process escaped char
            if (val[i] == '\\')
            {
                i++; // move on to byte after the escape
                switch (val[i])
                {
                    // one byte escaped. we put in one bytes instead of two, so pull increases by 1
                    case '"': val[i-1-pull] = val[i]; pull++; break;
                    case '\\': val[i-1-pull] = val[i]; pull++; break;
                    case '/': val[i-1-pull] = val[i]; pull++; break;
                    case 'b': val[i-1-pull] = '\b'; pull++; break;
                    case 'f': val[i-1-pull] = '\f'; pull++; break;
                    case 'n': val[i-1-pull] = '\n'; pull++; break;
                    case 'r': val[i-1-pull] = '\r'; pull++; break;
                    case 't': val[i-1-pull] = '\t'; pull++; break;
                    // 5 bytes escaped for UTF-8 encoding
                    case 'u': 
                    {
                        gg_num rtot; // unicode
                        gg_num totjv; // # of bytes used up
                        if ((rtot = gg_utf_get_code (val, &i, &totjv, o_errm)) == -1) return NULL;  
                        // turn unicode to binary
                        gg_num32 bytes = gg_decode_utf (rtot, (unsigned char*) (val+i-1-pull), o_errm);
                        if ((*o_errm)[0] != 0) return NULL;
                        pull += totjv - bytes; // binary is less space than \uXXXX 
                        i += totjv - 1 - 1; // 1 for getting passed \ and one for i++ further down
                        
                        break;
                    }
                    default: { GG_ERR0; *o_errm = gg_strdup(GG_ERR_UTF_BADESCAPE); return NULL;} // unknown escape sequence
                }
                i++; // to account for \, ", /, b, f, u etc.
            }
            else
            {
                // normal character, if no escapes just proceed to the end
                if (pull !=0) val[i-pull] = val[i];
                i++;
            }
        }
    }
    if (pull != 0) 
    {
        val[i-pull] = 0; // if pulled whole string back, finish it with 0
        if (alloced) gg_mem_set_len (val, i-pull+1); // set it before gg_puts because it needs length to output below (if so)
    }
    if (val[i] == 0 && quoted == 1) { GG_ERR0; *o_errm = gg_strdup(GG_ERR_UTF_NOQUOTE); return NULL;} // must have double quote at the end if quoted set
    return val+i; // return where the end is
}



//
// Convert binary utf val of len 'len' into text/unicode text 'resptr' (which is allocated here), 
// any error in err (also set here)
// If error, return -1, otherwise length of res
// len can be -1 in which case it's computed
//
gg_num gg_utf_to_text (char *val, gg_num len, char **resptr, char **err)
{

    *err = GG_EMPTY_STRING;
    gg_num lval = gg_mem_get_len(val);
    if (len == -1) len = lval;
    else if (len > lval) gg_report_error ("Memory used is of length [%ld] but only [%ld] allocated", len, lval);


    // allocate 3x memory, worst case scenario 2-byte utf to 6 byte unicode like \uXXXX
    // or 4-byte utf to 2x 6 byte surrogate unicodes. For others it's only 2x (such as \t)
    char *res = (char*)gg_malloc(3*len + 1);
    res[0] = 0; // by default it's empty in case of error
    if (resptr != NULL) *resptr = res;

    // note use of sprintf/like here is okay, as we have guaranteed enough space to write into (see above gg_malloc)
    gg_num i;
    gg_num r = 0;
    for (i = 0; i < len; i++)
    {
        if ((val[i] & 192) == 192)
        {
            // this is the beginning of utf sequence of bytes (2,3 or 4 bytes)
            // create \uXXXX (and possibly a surrogate pair)
            gg_num32 u;
            gg_num bytes = gg_encode_utf (val+i, &u, err);
            if (bytes == -1) return -1; else i+=(bytes-1); // since for loop will do i++
            if (u >= 0x10000) { // this means we need a surrogate pair
                gg_num32 u0;
                gg_num32 u1;
                gg_get_utf_surrogate (u, &u0, &u1);
                //
                //Use direct memory manipulation instead of sprintf which is many times slower
                //
                //sprintf (res+r, "\\u%04x", u0);
                res[r] = '\\';
                res[r+1] = 'u';
                GG_HEX_FROM_INT16(res+r+2,u0);
                r+=6; // no need to set res[r] to 0, since we continue below
                //sprintf (res+r, "\\u%04x", u1);
                res[r] = '\\';
                res[r+1] = 'u';
                GG_HEX_FROM_INT16(res+r+2,u1);
                r+=6;
                res[r] = 0;
            }
            else
            {
                //
                //Use direct memory manipulation instead of sprintf which is many times slower
                //
                //sprintf (res+r, "\\u%04x", u);
                res[r] = '\\';
                res[r+1] = 'u';
                GG_HEX_FROM_INT16(res+r+2,u);
                r+=6;
                res[r] = 0;
            }
            continue;
        }
        else
        {
            switch (val[i])
            {
                // one byte escaped. we put in one bytes instead of two, so pull increases by 1
                case '"': memcpy (res+r, "\\\"", 2); r+=2; break;
                case '\\': memcpy (res+r, "\\\\", 2); r+=2; break;
                //
                // solidus is not encoded but for decoding it's recognized - this is common implementation
                //
                //case '/':  memcpy (res+r, "\\/", 2); r+=2;break;
                case '\b': memcpy (res+r, "\\b", 2); r+=2; break;
                case '\f': memcpy (res+r, "\\f", 2); r+=2; break;
                case '\n': memcpy (res+r, "\\n", 2); r+=2; break;
                case '\r': memcpy (res+r, "\\r", 2); r+=2; break;
                case '\t': memcpy (res+r, "\\t", 2); r+=2; break;
                default: { res[r++] = val[i]; } // just copy over
            }
        }
    }
    res[r] = 0;
    gg_mem_set_len (res, r+1); // set it before gg_puts because it needs length to output below (if so)
    // if "to" is not used in utf-text, output string
    if (resptr == NULL) 
    {
        gg_puts (GG_NOENC, res, r, true);
        gg_free_int (res); // free output string
    }
    return r;
}



