/*-----------------------------------------------------------------------------* 
* Base64 encoding/decoding (RFC1341)
* Copyright (c) 2005-2011, Jouni Malinen <j@w1.fi>
*
* This software may be distributed under the terms of the BSD license.

* This file is modification of Jouni Malinen code to remove the dynamic
* memory allocation.
*----------------------------------------------------------------------------*/


#include "Base64.h"


///
using namespace Cpl::Text::Encoding;


///////////////////////////////////
static const unsigned char base64_table[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

bool Cpl::Text::Encoding::base64Encode( const void *encodedTextSrc,
                                        size_t      srcLen,
                                        char*       dstEncodedText,
                                        size_t      dstSize,
                                        size_t&     encodedOutputStringLen,
                                        bool        insertMIMELineFeeds )
{
    // Calculate and check encoded length
    size_t olen = srcLen * 4 / 3 + 4;   /* 3-byte blocks to 4-byte */
    if ( insertMIMELineFeeds )
    {
        olen += olen / 76;              /* line feeds */
    }
    olen++;                             /* nul termination */
    if ( olen < srcLen || olen > dstSize )
    {
        return false; /* integer overflow AND/OR insufficient output memory */
    }
    if ( dstEncodedText == nullptr || encodedTextSrc == nullptr )
    {
        return false;
    }

    const unsigned char* end = (const unsigned char*) encodedTextSrc + srcLen;
    const unsigned char* in  = (const unsigned char*) encodedTextSrc;
    unsigned char*       pos = (unsigned char*) dstEncodedText;
    int line_len = 0;
    while ( end - in >= 3 )
    {
        *pos++ = base64_table[in[0] >> 2];
        *pos++ = base64_table[((in[0] & 0x03) << 4) | (in[1] >> 4)];
        *pos++ = base64_table[((in[1] & 0x0f) << 2) | (in[2] >> 6)];
        *pos++ = base64_table[in[2] & 0x3f];
        in += 3;
        line_len += 4;
        if ( insertMIMELineFeeds && line_len >= 76 )
        {
            *pos++ = '\n';
            line_len = 0;
        }
    }

    if ( end - in )
    {
        *pos++ = base64_table[in[0] >> 2];
        if ( end - in == 1 )
        {
            *pos++ = base64_table[(in[0] & 0x03) << 4];
            *pos++ = '=';
        }
        else
        {
            *pos++ = base64_table[((in[0] & 0x03) << 4) | (in[1] >> 4)];
            *pos++ = base64_table[(in[1] & 0x0f) << 2];
        }
        *pos++ = '=';
        line_len += 4;
    }

    if ( insertMIMELineFeeds && line_len )
    {
        *pos++ = '\n';
    }

    *pos = '\0';
    encodedOutputStringLen = pos - (unsigned char*) dstEncodedText;
    return dstEncodedText;
}


bool Cpl::Text::Encoding::base64Decode( const char* encodedTextSrc,
                                        size_t      encodedTextSrcLen,
                                        void*       dstBinary,
                                        size_t      dstSize,
                                        size_t&     dstBinaryLen )
{
    const static unsigned char dtable[256] =
    {
        0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x3e, 0x80, 0x80, 0x80, 0x3f,
        0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x80, 0x80, 0x80, 0x00, 0x80, 0x80,
        0x80, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e,
        0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x80, 0x80, 0x80, 0x80, 0x80,
        0x80, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
        0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x80, 0x80, 0x80, 0x80, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80 };

    if ( encodedTextSrc == nullptr || dstBinary == nullptr )
    {
        return false;
    }

    // Get a count of valid encoding characters
    size_t count = 0;
    for ( size_t i = 0; i < encodedTextSrcLen; i++ )
    {
        if ( dtable[(int)(encodedTextSrc[i])] != 0x80 )
        {
            count++;
        }
    }

    // Error checking (make sure there is room for the null terminator)
    if ( count == 0 || count % 4 || ((count / 4 * 3)+1) > dstSize )
    {
        return false;
    }
    
    unsigned char* pos = (unsigned char*) dstBinary;
    unsigned char  block[4];
    int            pad = 0;
    count = 0;
    for ( size_t i = 0; i < encodedTextSrcLen; i++ )
    {
        unsigned char tmp = dtable[(int)(encodedTextSrc[i])];
        if ( tmp == 0x80 )
        {
            continue;
        }

        if ( encodedTextSrc[i] == '=' )
        {
            pad++;
        }
        block[count] = tmp;
        count++;
        if ( count == 4 )
        {
            *pos++ = (block[0] << 2) | (block[1] >> 4);
            *pos++ = (block[1] << 4) | (block[2] >> 2);
            *pos++ = (block[2] << 6) | block[3];
            count = 0;
            if ( pad )
            {
                if ( pad == 1 )
                {
                    pos--;
                }
                else if ( pad == 2 )
                {
                    pos -= 2;
                }
                else
                {
                    /* Invalid padding */
                    return false;
                }
                break;
            }
        }
    }

    *pos         = '\0';
    dstBinaryLen = pos - (unsigned char*) dstBinary;
    return true;
}
