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
#include "atob.h"
#include "strip.h"
#include <stdint.h>
#include <string.h>
#include "Cpl/System/Assert.h"


//
using namespace Cpl::Text;

///////////////////
static bool conversionOk( const char* originalString, char* endPtr, const char* validStopChars )
{
    // No conversion occurred
    if ( !originalString || *originalString == '\0' || endPtr == originalString )
    {
        return false;
    }

    // Valid conversion (stopped on End-of-String)
    if ( *endPtr == '\0' )
    {
        return true;
    }

    // At least a partial conversion occurred, determine if it is OK
    while ( validStopChars && *validStopChars != '\0' )
    {
        if ( *endPtr == *validStopChars )
        {
            return true;
        }
        validStopChars++;
    }

    return false;
}


///////////////////
bool Cpl::Text::a2i( int& convertedValue, const char* string, int base, const char* validStopChars, const char** end )
{
    int   value  = 0;
    char* endPtr = 0;
    if ( string )
    {
        value = ( int) strtol( string, &endPtr, base );
    }
    if ( end )
    {
        *end = endPtr;
    }

    // Only update the client's variable if the conversion was successful
    if ( conversionOk( string, endPtr, validStopChars ) )
    {
        convertedValue = value;
        return true;
    }

    return false;
}
bool Cpl::Text::a2ui( unsigned& convertedValue, const char* string, int base, const char* validStopChars, const char** end )
{
    unsigned value  = 0;
    char*    endPtr = 0;
    if ( string )
    {
        value = ( unsigned) strtoul( string, &endPtr, base );
    }
    if ( end )
    {
        *end = endPtr;
    }

    // Only update the client's variable if the conversion was successful
    if ( conversionOk( string, endPtr, validStopChars ) )
    {
        convertedValue = value;
        return true;
    }

    return false;
}


bool Cpl::Text::a2l( long& convertedValue, const char* string, int base, const char* validStopChars, const char** end )
{
    long  value  = 0;
    char* endPtr = 0;
    if ( string )
    {
        value = strtol( string, &endPtr, base );
    }
    if ( end )
    {
        *end = endPtr;
    }

    // Only update the client's variable if the conversion was successful
    if ( conversionOk( string, endPtr, validStopChars ) )
    {
        convertedValue = value;
        return true;
    }

    return false;
}

bool Cpl::Text::a2ul( unsigned long& convertedValue, const char* string, int base, const char* validStopChars, const char** end )
{
    unsigned long value  = 0;
    char*         endPtr = 0;
    if ( string )
    {
        value = strtoul( string, &endPtr, base );
    }
    if ( end )
    {
        *end = endPtr;
    }

    // Only update the client's variable if the conversion was successful
    if ( conversionOk( string, endPtr, validStopChars ) )
    {
        convertedValue = value;
        return true;
    }

    return false;
}

bool Cpl::Text::a2ll( long long& convertedValue, const char* string, int base, const char* validStopChars, const char** end )
{
    long long value  = 0;
    char*     endPtr = 0;
    if ( string )
    {
        value = strtoll( string, &endPtr, base );
    }
    if ( end )
    {
        *end = endPtr;
    }

    // Only update the client's variable if the conversion was successful
    if ( conversionOk( string, endPtr, validStopChars ) )
    {
        convertedValue = value;
        return true;
    }

    return false;
}

bool Cpl::Text::a2ull( unsigned long long& convertedValue, const char* string, int base, const char* validStopChars, const char** end )
{
    unsigned long long value  = 0;
    char*              endPtr = 0;
    if ( string )
    {
        value = strtoull( string, &endPtr, base );
    }
    if ( end )
    {
        *end = endPtr;
    }

    // Only update the client's variable if the conversion was successful
    if ( conversionOk( string, endPtr, validStopChars ) )
    {
        convertedValue = value;
        return true;
    }

    return false;
}

///////////////////
bool Cpl::Text::a2d( double& convertedValue, const char* string, const char* validStopChars, const char** end )
{
    double value  = 0;
    char*  endPtr = 0;
    if ( string )
    {
        value = strtod( string, &endPtr );
    }
    if ( end )
    {
        *end = endPtr;
    }

    // Only update the client's variable if the conversion was successful
    if ( conversionOk( string, endPtr, validStopChars ) )
    {
        convertedValue = value;
        return true;
    }

    return false;
}

///////////////////
bool Cpl::Text::a2b( bool& convertedValue, const char* string, const char* trueToken, const char* falseToken, const char** end )
{
    size_t tokenSize = 0;
    bool   result    = false;

    if ( !string || !trueToken || !falseToken )
    {
        return false;
    }

    if ( strncmp( string, trueToken, strlen( trueToken ) ) == 0 )
    {
        convertedValue = true;
        tokenSize      = strlen( trueToken );
        result         = true;
    }
    else if ( strncmp( string, falseToken, strlen( falseToken ) ) == 0 )
    {
        convertedValue = false;
        tokenSize      = strlen( falseToken );
        result         = true;
    }

    if ( end )
    {
        *end = string + tokenSize;
    }

    return result;
}

///////////////////
long Cpl::Text::asciiHexToBuffer( void* dstBinary, const char* srcP, size_t dstMaxLen )
{
    if ( !dstBinary || !srcP || dstMaxLen == 0 )
    {
        return -1;
    }

    char        temp[3] = { '\0', };
    uint8_t*    dstP    = ( uint8_t*) dstBinary;
    size_t      len     = strlen( srcP );
    unsigned    rawByte;
    size_t      i;

    // Length must be even
    if ( ( len & 1 ) == 1 )
    {
        return -1;
    }

    // Do NOT exceed the destination buffer 
    if ( len / 2 > dstMaxLen )
    {
        return -1;
    }

    // Walk the entire string
    for ( i=0; i < len; i+=2 )
    {
        // Convert the data
        temp[0] = *srcP++;
        temp[1] = *srcP++;
        if ( !a2ui( rawByte, temp, 16 ) )
        {
            // Error -->end conversion
            return -1;
        }

        *dstP++ = ( uint8_t) rawByte;
    }

    return len / 2;
}

////////////////////////////////////////////
static bool parseTime( const char* time, Cpl::System::ElapsedTime::Precision_T& convertedValue )
{
    unsigned long hh;
    const char*   endPtr;
    if ( a2ul( hh, time, 10, ":", &endPtr ) && *endPtr != '\0' )
    {
        unsigned long mm;
        if ( a2ul( mm, endPtr + 1, 10, ":", &endPtr ) && *endPtr != '\0' )
        {
            unsigned long ss;
            if ( a2ul( ss, endPtr + 1, 10, ".", &endPtr ) )
            {
                unsigned int msecs = 0;
                if ( *endPtr == '\0' || a2ui( msecs, endPtr + 1 ) )
                {
                    if ( msecs < 1000 )
                    {
                        msecs                        = strlen( endPtr + 1 ) == 1 ? msecs * 100 : strlen( endPtr + 1 ) == 2 ? msecs * 10 : msecs;
                        convertedValue.m_thousandths = msecs;
                        convertedValue.m_seconds     = ss + ( mm * 60 ) + ( hh * 60 * 60 );
                        return true;
                    }
                }
            }
        }
    }

    // If I get here - the parse failed
    return false;
}

// format: [DD ]HH:MM:SS.sss
bool Cpl::Text::parsePrecisionTimeStamp( const char* timeStampToParse, Cpl::System::ElapsedTime::Precision_T& convertedValue )
{
    CPL_SYSTEM_ASSERT( timeStampToParse );

    const char* firstArg  = stripSpace( timeStampToParse );
    const char* secondArg = stripSpace( stripNotSpace( firstArg ) );

    // No day field
    if ( !secondArg || *secondArg == '\0' )
    {
        return parseTime( firstArg, convertedValue );
    }

    // Have day field
    else
    {
        unsigned long day;
        if ( a2ul( day, firstArg, 10, " " ) )
        {
            if ( parseTime( secondArg, convertedValue ) )
            {
                convertedValue.m_seconds += day * 60 * 60 * 24;
                return true;
            }
        }
    }

    // If I get here, then the parse failed
    return false;
}