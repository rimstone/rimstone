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
// get original unicode from two surrogates. Return that value.
//
gg_num32 gg_make_from_utf_surrogate (gg_num32 u0, gg_num32 u1)
{
    return (u0 << 10) + u1 + (0x10000 - (0xD800 << 10) - 0xDC00);
}

//
// Encode binary UTF string 'r' to unicode value 'u'. Return number of bytes of UTF picked up (1,2,3 or 4) or -1 if error
// 'e' is error text
//
gg_num gg_encode_utf (char *r, gg_num32 *u, char **e)
{
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
        else { *e = gg_strdup(GG_UTF_ERR_SECOND_BYTE); return -1;}
        if ((r[2] & 128) == 128) *u += ((r[2] & 63)<<6);
        else { *e = gg_strdup(GG_UTF_ERR_THIRD_BYTE); return -1;}
        if ((r[3] & 128) == 128) *u += (r[3] & 63);
        else { *e = gg_strdup(GG_UTF_ERR_FOURTH_BYTE); return -1;}
        return 4;
    }
    else if ((r[0] & 224) == 224)
    {
        // three byte
        *u = (r[0] & 15) << 12;
        if ((r[1] & 128) == 128) *u += ((r[1] & 63)<<6);
        else { *e = gg_strdup(GG_UTF_ERR_SECOND_BYTE); return -1;}
        if ((r[2] & 128) == 128) *u += (r[2] & 63);
        else { *e = gg_strdup(GG_UTF_ERR_THIRD_BYTE); return -1;}
        return 3;
    }
    else if ((r[0] & 192) == 192)
    {
        // two byte
        *u = (r[0] & 31) << 6;
        if ((r[1] & 128) == 128) *u += (r[1] & 63);
        else { *e = gg_strdup(GG_UTF_ERR_SECOND_BYTE); return -1;}
        return 2;
    }
    else
    {
        *e = gg_strdup(GG_UTF_INVALID); return -1;
    }
}

//
// Break unicode u into surrogate u0 and u1. 
// 
//
void gg_get_utf_surrogate (gg_num32 u, gg_num32 *u0, gg_num32 *u1)
{
    *u0 = (0xD800 - (0x10000 >> 10)) + (u>>10);
    *u1 = 0xDC00 + (u&0x3FF);
}

//
// Decode 32 bit Unicode to 1,2,3 or 4 UTF string
// 'u' is unicode number of a character, range 0 to 10FFFF.
// 'r' is the result string of UTF (1,2,3 or 4 bytes), r must point to at least 5 bytes of memory (4+null)
// 'e' is error message. 
// returns -1 on error, number of bytes (excluding 0) if okay
//
gg_num gg_decode_utf (gg_num32 u, unsigned char *r, char **e)
{
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
        if (u == 0xFEFF) {*e = gg_strdup(GG_UTF_ERR_ILLEGAL_CHARACTER_FEFF); return -1;}
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
    else { *e = gg_strdup(GG_UTF_ERR_OUT_OF_RANGE); return -1; }
    return -1; // just to satisfy compiler
}


//
// Get value of string representing hex in 4 digits at 'v'. If error, err will be filled and returns 0.
// Return value of hex.
//
gg_num32 gg_get_hex(char *v, char **err)
{
    gg_num k;
    gg_num r = 0;
    for (k = 0; k < 4; k++)
    {
        if (*v >= '0' && *v <= '9') r += (*v - '0')*gg_topower(16,3-k);
            else if (*v >= 'a' && *v <= 'f') r += (*v - 'a' + 10)*gg_topower(16,3-k);
            else if (*v >= 'A' && *v <= 'F') r += (*v - 'A' + 10)*gg_topower(16,3-k);
            else { *err = GG_ERR_UTF_BADUTF; return 0;} // not a hex value
        v++;
    }
    return r;
}

//
// Given UTF text val, starting with *ubeg byte, compute the actual 32 bit number that is the unicode for 
// this utf (either by itself or with a surrogate, which follows as \u.... right afterwards).
// Return -1 and set o_errm. Otherwise return the unicode.
//
GG_ALWAYS_INLINE inline gg_num gg_utf_get_code (char *val, gg_num *ubeg, gg_num *totjv, char **o_errm)
{
    gg_num c = *ubeg;
    *totjv = 6;
    c++;
    gg_num r = gg_get_hex (val + c, o_errm);
    if ((*o_errm)[0] != 0) return -1;
    gg_num32 rtot;
    if (r >= 0xD800 && r <= 0xDFFF)  
    { 
        *totjv += 6;
        c+=4; // get passed XXXX\u
        if (val[c] != '\\' || val[c+1] != 'u')
        {
            GG_ERR0;
            *o_errm = gg_strdup(GG_ERR_UTF_SURROGATE); return -1;
        }
        gg_num r1 = gg_get_hex (val + c + 2 , o_errm); // c+2 to skip \u
        if ((*o_errm)[0] != 0) return -1;
        rtot = gg_make_from_utf_surrogate (r, r1);
    } else rtot = r;
    return rtot;
}




