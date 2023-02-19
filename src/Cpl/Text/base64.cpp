/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2020  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/

#include "base64.h"
#include <stdint.h>

using namespace Cpl::Text;


/* This is a public domain base64 implementation written by WEI Zhicheng. */

#include "base64.h"

#define BASE64_PAD '='
#define BASE64DE_FIRST '+'
#define BASE64DE_LAST 'z'

/* BASE 64 encode table */
static const char base64en[] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
    'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
    'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3',
    '4', '5', '6', '7', '8', '9', '+', '/',
};

/* ASCII order for BASE 64 decode, 255 in unused character */
static const unsigned char base64de[] = {
    /* nul, soh, stx, etx, eot, enq, ack, bel, */
       255, 255, 255, 255, 255, 255, 255, 255,

    /*  bs,  ht,  nl,  vt,  np,  cr,  so,  si, */
       255, 255, 255, 255, 255, 255, 255, 255,

    /* dle, dc1, dc2, dc3, dc4, nak, syn, etb, */
       255, 255, 255, 255, 255, 255, 255, 255,

    /* can,  em, sub, esc,  fs,  gs,  rs,  us, */
       255, 255, 255, 255, 255, 255, 255, 255,

    /*  sp, '!', '"', '#', '$', '%', '&', ''', */
       255, 255, 255, 255, 255, 255, 255, 255,

    /* '(', ')', '*', '+', ',', '-', '.', '/', */
       255, 255, 255,  62, 255, 255, 255,  63,

    /* '0', '1', '2', '3', '4', '5', '6', '7', */
        52,  53,  54,  55,  56,  57,  58,  59,

    /* '8', '9', ':', ';', '<', '=', '>', '?', */
        60,  61, 255, 255, 255, 255, 255, 255,

    /* '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', */
       255,   0,   1,  2,   3,   4,   5,    6,

    /* 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', */
         7,   8,   9,  10,  11,  12,  13,  14,

    /* 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', */
        15,  16,  17,  18,  19,  20,  21,  22,

    /* 'X', 'Y', 'Z', '[', '\', ']', '^', '_', */
        23,  24,  25, 255, 255, 255, 255, 255,

    /* '`', 'a', 'b', 'c', 'd', 'e', 'f', 'g', */
       255,  26,  27,  28,  29,  30,  31,  32,

    /* 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', */
        33,  34,  35,  36,  37,  38,  39,  40,

    /* 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', */
        41,  42,  43,  44,  45,  46,  47,  48,

    /* 'x', 'y', 'z', '{', '|', '}', '~', del, */
        49,  50,  51, 255, 255, 255, 255, 255
};

int Cpl::Text::base64Encode( char *out, int outputLen, const void *input, int inlen )
{
    if ( out == nullptr ||
         input == nullptr ||
         inlen == 0 ||
         (outputLen-1) < base64EncodeLen( inlen ) ) // Account for the null terminator
    {
        return -1;
    }
    memset( out, 0, outputLen );

    const unsigned char*in = (const unsigned char*) input;
    int s;
    int i;
    int j;
    unsigned char c;
    unsigned char l;

    s = 0;
    l = 0;
    for ( i = j = 0; i < inlen; i++ )
    {
        c = in[i];

        switch ( s )
        {
        case 0:
            s = 1;
            out[j++] = base64en[( c >> 2 ) & 0x3F];
            break;
        case 1:
            s = 2;
            out[j++] = base64en[( ( l & 0x3 ) << 4 ) | ( ( c >> 4 ) & 0xF )];
            break;
        case 2:
            s = 0;
            out[j++] = base64en[( ( l & 0xF ) << 2 ) | ( ( c >> 6 ) & 0x3 )];
            out[j++] = base64en[c & 0x3F];
            break;
        }
        l = c;
    }

    switch ( s )
    {
    case 1:
        out[j++] = base64en[( l & 0x3 ) << 4];
        out[j++] = BASE64_PAD;
        out[j++] = BASE64_PAD;
        break;
    case 2:
        out[j++] = base64en[( l & 0xF ) << 2];
        out[j++] = BASE64_PAD;
        break;
    }

    out[j] = 0;

    return j;
}

int Cpl::Text::base64Decode( void *output, int outputLen, const char *in, int inlen )
{
    if ( output == nullptr ||
         in == nullptr ||
         inlen == 0 ||
         outputLen < base64DecodeLen( in, inlen ) )
    {
        return -1;
    }

    unsigned char* out = (unsigned char*) output;
    int i;
    int j;
    unsigned char c;

    if ( inlen & 0x3 )
    {
        return -1;
    }

    for ( i = j = 0; i < inlen; i++ )
    {
        if ( in[i] == BASE64_PAD )
        {
            break;
        }
        if ( in[i] < BASE64DE_FIRST || in[i] > BASE64DE_LAST )
        {
            return -1;
        }

        c = base64de[(unsigned char) in[i]];
        if ( c == 255 )
        {
            return -1;
        }

        switch ( i & 0x3 )
        {
        case 0:
            out[j] = ( c << 2 ) & 0xFF;
            break;
        case 1:
            out[j++] |= ( c >> 4 ) & 0x3;
            out[j] = ( c & 0xF ) << 4;
            break;
        case 2:
            out[j++] |= ( c >> 2 ) & 0xF;
            out[j] = ( c & 0x3 ) << 6;
            break;
        case 3:
            out[j++] |= c;
            break;
        }
    }

    return j;
}

int Cpl::Text::base64EncodeLen( int plainLen )
{
    int n = plainLen;
    return ( n + 2 - ( ( n + 2 ) % 3 ) ) / 3 * 4;
}

int Cpl::Text::base64DecodeLen( const char * input, int inputLen )
{
    int i = 0;
    int numEq = 0;
    for ( i = inputLen - 1; input[i] == '='; i-- )
    {
        numEq++;
    }

    return ( ( 6 * inputLen ) / 8 ) - numEq;
}
