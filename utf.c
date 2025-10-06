// SPDX-License-Identifier: Apache-2.0
// Copyright 2018-2025 Gliim LLC. 
// Licensed under Apache License v2. See LICENSE file.
// On the web http://rimstone-lang.com/ - this file is part of RimStone framework.

// 
// UTF-related module
//

//
// Based on UTF standard: https://www.rfc-editor.org/rfc/rfc3629
//

#include "rim.h"

//
// get original unicode from two surrogates. Return that value.
//
RIM_ALWAYS_INLINE inline rim_num32 rim_make_from_utf_surrogate (rim_num32 u0, rim_num32 u1)
{
    return (u0 << 10) + u1 + (0x10000 - (0xD800 << 10) - 0xDC00);
}

//
// Encode binary UTF string 'r' to unicode value 'u'. Return number of bytes of UTF picked up (1,2,3 or 4) or -1 if error
// 'e' is error text
//
RIM_ALWAYS_INLINE inline rim_num rim_encode_utf (char *r, rim_num32 *u, char **e)
{
    *e = RIM_EMPTY_STRING;
    if ((r[0] & 128) == 0) 
    {
        // single byte, ascii
        *u = (rim_num32) r[0];
        return 1;
    }
    // These 3 else if must be in this order, because checking for 192 is true for both 224 and 240
    // so if 192 were first, 224 and 240 would never happen
    else if ((r[0] & 240) == 240)
    {
        // four byte
        *u = (r[0] & 7) << 18;
        if ((r[1] & 128) == 128) *u += ((r[1] & 63)<<12);
        else { *e = rim_strdup(RIM_UTF_ERR_SECOND_BYTE); return -1;}
        if ((r[2] & 128) == 128) *u += ((r[2] & 63)<<6);
        else { *e = rim_strdup(RIM_UTF_ERR_THIRD_BYTE); return -1;}
        if ((r[3] & 128) == 128) *u += (r[3] & 63);
        else { *e = rim_strdup(RIM_UTF_ERR_FOURTH_BYTE); return -1;}
        return 4;
    }
    else if ((r[0] & 224) == 224)
    {
        // three byte
        *u = (r[0] & 15) << 12;
        if ((r[1] & 128) == 128) *u += ((r[1] & 63)<<6);
        else { *e = rim_strdup(RIM_UTF_ERR_SECOND_BYTE); return -1;}
        if ((r[2] & 128) == 128) *u += (r[2] & 63);
        else { *e = rim_strdup(RIM_UTF_ERR_THIRD_BYTE); return -1;}
        return 3;
    }
    else if ((r[0] & 192) == 192)
    {
        // two byte
        *u = (r[0] & 31) << 6;
        if ((r[1] & 128) == 128) *u += (r[1] & 63);
        else { *e = rim_strdup(RIM_UTF_ERR_SECOND_BYTE); return -1;}
        return 2;
    }
    else
    {
        *e = rim_strdup(RIM_UTF_INVALID); return -1;
    }
}

//
// Break unicode u into surrogate u0 and u1. 
// 
//
RIM_ALWAYS_INLINE inline void rim_get_utf_surrogate (rim_num32 u, rim_num32 *u0, rim_num32 *u1)
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
RIM_ALWAYS_INLINE inline rim_num rim_decode_utf (rim_num32 u, unsigned char *r, char **e)
{
    *e = RIM_EMPTY_STRING;
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
        if (u == 0xFEFF) {*e = rim_strdup(RIM_UTF_ERR_ILLEGAL_CHARACTER_FEFF); return -1;}
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
    else { *e = rim_strdup(RIM_UTF_ERR_OUT_OF_RANGE); return -1; }
    return -1; // just to satisfy compiler
}


//
// Get value of string representing hex in 4/8 digits at 'v'. If error, err will be filled and returns 0.
// Return value of hex.
// if bytes is 4, then it's 4 bytes, if it's 8 then it's 8 bytes
//
RIM_ALWAYS_INLINE inline rim_num32 rim_get_hex(char *v, char **err, char bytes)
{
    rim_num k;
    rim_num r = 0;
    for (k = 0; k < bytes; k++)
    {
        if (*v >= '0' && *v <= '9') r += (*v - '0')*rim_topower(16,bytes-1-k);
            else if (*v >= 'a' && *v <= 'f') r += (*v - 'a' + 10)*rim_topower(16,bytes-1-k);
            else if (*v >= 'A' && *v <= 'F') r += (*v - 'A' + 10)*rim_topower(16,bytes-1-k);
            else { *err = rim_strdup(RIM_ERR_UTF_BADUTF); return 0;} // not a hex value
        v++;
    }
    return r;
}

//
// Given Unicode text val \U (8 hex), starting with *ubeg byte, compute the actual 32 bit number that is the UTF8 for 
// this 
// Return -1 and set o_errm. Otherwise return the unicode.
//
RIM_ALWAYS_INLINE inline rim_num rim_utf_get_code8 (char *val, rim_num *ubeg, rim_num *totjv, char **o_errm)
{
    rim_num c = *ubeg;
    *totjv = 10;
    c++;
    rim_num r = rim_get_hex (val + c, o_errm, 8);
    if ((*o_errm)[0] != 0) return -1;
    // if in surrogate range, that's invalid for \U unicode (valid for \u)
    if (r >= 0xD800 && r <= 0xDFFF)  { *o_errm = rim_strdup (RIM_UTF_INVALID); return -1; }
    return r;
}

//
// Given Unicode text val \u (4 hex), starting with *ubeg byte, compute the actual 32 bit number that is the UTF8 for 
// this (either by itself or with a surrogate, which follows as \u.... right afterwards).
// Return -1 and set o_errm. Otherwise return the unicode.
//
RIM_ALWAYS_INLINE inline rim_num rim_utf_get_code (char *val, rim_num *ubeg, rim_num *totjv, char **o_errm)
{
    rim_num c = *ubeg;
    *totjv = 6;
    c++;
    rim_num r = rim_get_hex (val + c, o_errm, 4);
    if ((*o_errm)[0] != 0) return -1;
    rim_num32 rtot;
    if (r >= 0xD800 && r <= 0xDFFF)  
    {  // this is when surrogate is needed, so another 6 bytes (as in \uxyzw)
        *totjv += 6;
        c+=4; // get passed XXXX\u
        if (val[c] != '\\' || val[c+1] != 'u')
        {
            RIM_ERR0;
            *o_errm = rim_strdup(RIM_ERR_UTF_SURROGATE); return -1;
        }
        rim_num r1 = rim_get_hex (val + c + 2 , o_errm, 4); // c+2 to skip \u
        if ((*o_errm)[0] != 0) return -1;
        rtot = rim_make_from_utf_surrogate (r, r1);
    } else rtot = r;
    return rtot;
}



