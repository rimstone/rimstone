// SPDX-License-Identifier: Apache-2.0
// Copyright 2018 Gliim LLC. 
// Licensed under Apache License v2. See LICENSE file.
// On the web http://golf-lang.com/ - this file is part of Golf framework.

// 
// UTF8-related module
//

//
// Based on UTF8 standard: https://www.rfc-editor.org/rfc/rfc3629
//

#include "golf.h"

//
// get original unicode from two surrogates. Return that value.
//
gg_num32 gg_make_from_utf8_surrogate (gg_num32 u0, gg_num32 u1)
{
    GG_TRACE("");
    return (u0 << 10) + u1 + (0x10000 - (0xD800 << 10) - 0xDC00);
}

//
// Encode binary UTF8 string 'r' to unicode value 'u'. Return number of bytes of UTF8 picked up (1,2,3 or 4) or -1 if error
// 'e' is error text
//
gg_num gg_encode_utf8 (char *r, gg_num32 *u, char **e)
{
    GG_TRACE("");
    *e = GG_EMPTY_STRING;
    if ((r[0] & 128) == 0) 
    {
        // single byte, ascii
        *u = (gg_num32) r[0];
        return 1;
    }
    // These 3 else if must be in this order, because checking for 192 is true for both 224 and 240
    // so if 192 were first, 224 and 240 would never happen
    else if ((r[0] & 240) == 240)
    {
        // four byte
        *u = (r[0] & 7) << 18;
        if ((r[1] & 128) == 128) *u += ((r[1] & 63)<<12);
        else { *e = gg_strdup(GG_UTF8_ERR_SECOND_BYTE); return -1;}
        if ((r[2] & 128) == 128) *u += ((r[2] & 63)<<6);
        else { *e = gg_strdup(GG_UTF8_ERR_THIRD_BYTE); return -1;}
        if ((r[3] & 128) == 128) *u += (r[3] & 63);
        else { *e = gg_strdup(GG_UTF8_ERR_FOURTH_BYTE); return -1;}
        return 4;
    }
    else if ((r[0] & 224) == 224)
    {
        // three byte
        *u = (r[0] & 15) << 12;
        if ((r[1] & 128) == 128) *u += ((r[1] & 63)<<6);
        else { *e = gg_strdup(GG_UTF8_ERR_SECOND_BYTE); return -1;}
        if ((r[2] & 128) == 128) *u += (r[2] & 63);
        else { *e = gg_strdup(GG_UTF8_ERR_THIRD_BYTE); return -1;}
        return 3;
    }
    else if ((r[0] & 192) == 192)
    {
        // two byte
        *u = (r[0] & 31) << 6;
        if ((r[1] & 128) == 128) *u += (r[1] & 63);
        else { *e = gg_strdup(GG_UTF8_ERR_SECOND_BYTE); return -1;}
        return 2;
    }
    else
    {
        *e = gg_strdup(GG_UTF8_INVALID); return -1;
    }
}

//
// Break unicode u into surrogate u0 and u1. 
// 
//
void gg_get_utf8_surrogate (gg_num32 u, gg_num32 *u0, gg_num32 *u1)
{
    GG_TRACE("");
    *u0 = (0xD800 - (0x10000 >> 10)) + (u>>10);
    *u1 = 0xDC00 + (u&0x3FF);
}

//
// Decode 32 bit Unicode to 1,2,3 or 4 UTF8 string
// 'u' is unicode number of a character, range 0 to 10FFFF.
// 'r' is the result string of UTF8 (1,2,3 or 4 bytes), r must point to at least 5 bytes of memory (4+null)
// 'e' is error message. 
// returns -1 on error, number of bytes (excluding 0) if okay
//
gg_num gg_decode_utf8 (gg_num32 u, unsigned char *r, char **e)
{
    GG_TRACE("");
    *e = GG_EMPTY_STRING;
    if (u <= 0x7F) { r[0] = (unsigned char)u; return 1; } // single byte (ASCII)
    else if (u >= 0x80 && u <= 0x7FF) 
    { 
        // 192 = b11000000
        r[0] = 192+(u>>6); // >>6 to get the top 5 (out of 11) bits
        r[1] = 128+(u&63); // 63 is 111111 to extract lower 6 bits
        return 2; 
    }
    else if (u >= 0x800 && u <= 0xFFFF) 
    {
        if (u == 0xFEFF) {*e = gg_strdup(GG_UTF8_ERR_ILLEGAL_CHARACTER_FEFF); return -1;}
        // 224 = 11100000
        r[0] = 224+(u>>12); // get top 4 (out of 16) bits
        r[1] = 128+((u>>6)&63); // get middle 6 from 4+6+6 group of bits
        r[2] = 128+(u&63); // get lower 6 bits
        return 3;
    }
    else if (u >= 0x10000 && u <= 0x10FFFF) 
    {
        // 240 = 11110000
        r[0] = 240+(u>>18); // get top 3 (out of 21) bits
        r[1] = 128+((u>>12)&63); // get 6 after 3 from 3+6+6+6 group of bits
        r[2] = 128+((u>>6)&63); // get 6 after second 6 in 3+6+6+6 group of bits
        r[3] = 128+(u&63); // get lower 6 bits
        return 4;
    }
    else { *e = gg_strdup(GG_UTF8_ERR_OUT_OF_RANGE); return -1; }
    return -1; // just to satisfy compiler
}


//
// Get value of string representing hex in 4 digits at 'v'. If error, err will be filled and returns 0.
// Return value of hex.
//
gg_num32 gg_get_hex(char *v, char **err)
{
    GG_TRACE("");
    gg_num k;
    gg_num r = 0;
    for (k = 0; k < 4; k++)
    {
        if (*v >= '0' && *v <= '9') r += (*v - '0')*gg_topower(16,3-k);
            else if (*v >= 'a' && *v <= 'f') r += (*v - 'a' + 10)*gg_topower(16,3-k);
            else if (*v >= 'A' && *v <= 'F') r += (*v - 'A' + 10)*gg_topower(16,3-k);
            else { *err = GG_ERR_UTF8_BADUTF; return 0;} // not a hex value
        v++;
    }
    return r;
}

//
// Obtain the string from text "val", which starts with a first byte after double quote. 
// This string can be name or a string value in text. Sets errm.
// quoted is 1 if string is double quoted (as is with text)
// returns pointer to first byte after the string or NULL on error
// If 'enc' is 0, do not enccode strings at all.
// alloced is true if val is allocated memory
//
char *gg_text_to_utf8 (char *val, char quoted, char **o_errm, char enc, bool alloced)
{
    GG_TRACE("");
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
                        gg_num c = i;
                        gg_num totjv = 6;
                        c++;
                        gg_num r = gg_get_hex (val + c, o_errm);
                        if ((*o_errm)[0] != 0) return NULL;
                        gg_num32 rtot;
                        if (r >= 0xD800 && r <= 0xDFFF)  
                        { 
                            totjv += 6;
                            c+=4; // get passed XXXX\u
                            if (val[c] != '\\' || val[c+1] != 'u')
                            {
                                GG_ERR0;
                                *o_errm = gg_strdup(GG_ERR_UTF8_SURROGATE); return NULL;
                            }
                            gg_num r1 = gg_get_hex (val + c + 2 , o_errm); // c+2 to skip \u
                            if ((*o_errm)[0] != 0) return NULL;
                            rtot = gg_make_from_utf8_surrogate (r, r1);
                        } else rtot = r;
                        // turn unicode to binary
                        gg_num32 bytes = gg_decode_utf8 (rtot, (unsigned char*) (val+i-1-pull), o_errm);
                        if ((*o_errm)[0] != 0) return NULL;
                        pull += totjv - bytes; // binary is less space than \uXXXX 
                        i += totjv - 1 - 1; // 1 for getting passed \ and one for i++ further down
                        
                        break;
                    }
                    default: { GG_ERR0; *o_errm = gg_strdup(GG_ERR_UTF8_BADESCAPE); return NULL;} // unknown escape sequence
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
        if (alloced) gg_mem_set_len (gg_mem_get_id(val), i-pull+1); // set it before gg_puts because it needs length to output below (if so)
    }
    if (val[i] == 0 && quoted == 1) { GG_ERR0; *o_errm = gg_strdup(GG_ERR_UTF8_NOQUOTE); return NULL;} // must have double quote at the end if quoted set
    return val+i; // return where the end is
}



//
// Convert binary utf8 val of len 'len' into text/unicode text 'resptr' (which is allocated here), 
// any error in err (also set here)
// If error, return -1, otherwise length of res
// len can be -1 in which case it's computed
//
gg_num gg_utf8_to_text (char *val, gg_num len, char **resptr, char **err)
{
    GG_TRACE("");

    *err = GG_EMPTY_STRING;
    gg_num id = gg_mem_get_id(val);
    if (len == -1) len = gg_mem_get_len(id);
    else if (len > gg_mem_get_len(id)) gg_report_error ("Memory used is of length [%ld] but only [%ld] allocated", len, gg_mem_get_len(id));


    // allocate 3x memory, worst case scenario 2-byte utf8 to 6 byte unicode like \uXXXX
    // or 4-byte utf8 to 2x 6 byte surrogate unicodes. For others it's only 2x (such as \t)
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
            // this is the beginning of utf8 sequence of bytes (2,3 or 4 bytes)
            // create \uXXXX (and possibly a surrogate pair)
            gg_num32 u;
            gg_num bytes = gg_encode_utf8 (val+i, &u, err);
            if (bytes == -1) return -1; else i+=(bytes-1); // since for loop will do i++
            if (u >= 0x10000) { // this means we need a surrogate pair
                gg_num32 u0;
                gg_num32 u1;
                gg_get_utf8_surrogate (u, &u0, &u1);
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
    gg_mem_set_len (gg_mem_get_id(res), r+1); // set it before gg_puts because it needs length to output below (if so)
    // if "to" is not used in utf8-text, output string
    if (resptr == NULL) 
    {
        gg_num wres = gg_puts (GG_NOENC, res, r, true);
        if (wres < 0) GG_TRACE ("Error in writing direct, error [%s]", strerror(errno)); else GG_TRACE("Wrote direct [%ld] bytes", wres);
        gg_free (res); // free output string
    }
    return r;
}



