/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2022  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/

#include "format.h"

//
using namespace Cpl::Text;


////////////////////////////
bool Cpl::Text::bufferToString( const void* buffer, int len, String& destString, bool appendToString )
{
	if ( !buffer || len == 0 )
	{
		return false; // Do nothing!
	}

	// Clear Destination
	if ( !appendToString )
	{
		destString.clear();
	}

	// Convert the data
	char* ptr = (char*) buffer;
	char  c;
	int   i;
	for ( i=0; i < len; i++, ptr++ )
	{
		c = *ptr < ' ' || *ptr > '~' ? '.' : *ptr;
		destString += c;
	}

	return !destString.truncated();
}

bool Cpl::Text::bufferToAsciiHex( const void* binaryData, int len, String& destString, bool upperCase, bool appendToString, char separator )
{
	if ( !binaryData || len == 0 )
	{
		return false; // Do nothing!
	}

	// Select look-up table based on a requested 'case'
	static const char lowerCaseTable[] = { '0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f' };
	static const char upperCaseTable[] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F' };
	const char* tableP                 = upperCase ? upperCaseTable : lowerCaseTable;

	// Clear Destination
	if ( !appendToString )
	{
		destString.clear();
	}

	// Convert the data
	char* ptr = (char*) binaryData;
	char  c;
	int   i;
	for ( i=0; i < len; i++, ptr++ )
	{
		c = *ptr;
		destString += tableP[( c >> 4 ) & 0x0F];
		destString += tableP[c & 0x0F];
		if ( separator != '\0' && (i + 1) < len )
		{
			destString += separator;
		}
	}

	return !destString.truncated();
}

bool Cpl::Text::bufferToAsciiBinary( const void* binaryData, int len, Cpl::Text::String& destString, bool appendToString, bool reverse )
{
    static const char *bitRep[16] = { "0000", "0001", "0010", "0011", "0100", "0101", "0110", "0111",
                                      "1000", "1001", "1010", "1011", "1100", "1101", "1110", "1111" };

    if ( !binaryData || len == 0 )
    {
        return false; // Do nothing!
    }

    // Clear Destination
    if ( !appendToString )
    {
        destString.clear();
    }
    
    uint8_t* ptr  = reverse ? ((uint8_t*)binaryData) + len - 1: ((uint8_t*)binaryData);
    int direction = reverse ? -1: 1;
    while ( len-- )
    {
        destString += bitRep[*ptr >> 4];
        destString += bitRep[*ptr & 0x0F];
        ptr += direction;
    }

    return !destString.truncated();
}

bool Cpl::Text::bufferToViewer( const void* binaryData, int len, Cpl::Text::String& destString, int bytesPerLine, const char* separator, bool upperCase, bool appendToString )
{
	if ( !bufferToAsciiHex( binaryData, len, destString, upperCase, appendToString ) )
	{
		return false;
	}

	// Fill in 'missing' bytes
	for ( int i=len; i < bytesPerLine; i++ )
	{
		destString += "  ";
	}

	// Add separator
	destString += separator;
	if ( destString.truncated() )
	{
		return false;
	}

	return bufferToString( binaryData, len, destString, true );
}

static bool formatMsec( Cpl::Text::String& buffer, unsigned msec, unsigned long long elaspedSecs, bool encodeDay, bool appendToString )
{
	unsigned long long t, dd, hh, mm, ss;
	lldiv_t            result;

	// Get hours
	result = lldiv( elaspedSecs, 60L * 60LL );
	hh     = result.quot;
	t      = result.rem;

	// Get Minutes & Seconds
	result = lldiv( t, 60L );
	mm     = result.quot;
	ss     = result.rem;

	// Format string
	if ( encodeDay )
	{
		result = lldiv( hh, 24LL );
		dd     = result.quot;
		hh     = result.rem;

		if ( !appendToString )
		{
			buffer.format( "%02u %02u:%02u:%02u.%03u", (unsigned) dd, (unsigned) hh, (unsigned) mm, (unsigned) ss, msec );
		}
		else
		{
			buffer.formatAppend( "%02u %02u:%02u:%02u.%03u", (unsigned) dd, (unsigned) hh, (unsigned) mm, (unsigned) ss, msec );
		}
	}
	else
	{
		if ( !appendToString )
		{
			buffer.format( "%02u:%02u:%02u.%03u", (unsigned) hh, (unsigned) mm, (unsigned) ss, (unsigned) msec );
		}
		else
		{
			buffer.formatAppend( "%02u:%02u:%02u.%03u", (unsigned) hh, (unsigned) mm, (unsigned) ss, (unsigned) msec );
		}
	}

	return !buffer.truncated();
}


bool Cpl::Text::formatPrecisionTimeStamp( Cpl::Text::String& buffer, Cpl::System::ElapsedTime::Precision_T timeStamp, bool encodeDay, bool appendToString )
{
	return formatMsec( buffer, timeStamp.m_thousandths, timeStamp.m_seconds, encodeDay, appendToString );
}


bool Cpl::Text::formatMsecTimeStamp( Cpl::Text::String& buffer, unsigned long long timeStampInMsecs, bool encodeDay, bool appendToString )
{
	lldiv_t result;

	// Separate milliseconds and elapsed seconds
	result = lldiv( timeStampInMsecs, 1000LL );
	return formatMsec( buffer, (unsigned) result.rem, result.quot, encodeDay, appendToString );
}


bool Cpl::Text::formatSecTimeStamp( Cpl::Text::String& buffer, unsigned long long timeStampInSecs, bool encodeDay, bool appendToString )
{
	unsigned long long t, dd, hh, mm, ss;
	lldiv_t            result;

	// Get hours
	result = lldiv( timeStampInSecs, 60L * 60LL );
	hh     = result.quot;
	t      = result.rem;

	// Get Minutes & Seconds
	result = lldiv( t, 60LL );
	mm     = result.quot;
	ss     = result.rem;

	// Format string
	if ( encodeDay )
	{
		result = lldiv( hh, 24LL );
		dd     = result.quot;
		hh     = result.rem;

		if ( !appendToString )
		{
			buffer.format( "%02llu %02u:%02u:%02u", dd, (unsigned) hh, (unsigned) mm, (unsigned) ss );
		}
		else
		{
			buffer.formatAppend( "%02llu %02u:%02u:%02u", dd, (unsigned) hh, (unsigned) mm, (unsigned) ss );
		}
	}
	else
	{
		if ( !appendToString )
		{
			buffer.format( "%02llu:%02u:%02u", hh, (unsigned) mm, (unsigned) ss );
		}
		else
		{
			buffer.formatAppend( "%02llu:%02u:%02u", hh, (unsigned) mm, (unsigned) ss );
		}
	}

	return !buffer.truncated();
}


