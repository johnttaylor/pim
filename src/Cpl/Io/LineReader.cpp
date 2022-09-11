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

#include "LineReader.h"


//
using namespace Cpl::Io;


static bool discardRemainingLine_( Input& fd, const char* newline, Cpl::Text::String& destString, char lastChar, char overflowChar );


///////////////////////////////
LineReader::LineReader( Input& stream, const char* newline )
    :m_stream( stream )
    , m_newline( newline )
{
}

///////////////////////////////
bool LineReader::available()
{
    return m_stream.available();
}


void LineReader::close()
{
    m_stream.close();
}


// NOT the best/most-efficient algo - but it works for now
bool LineReader::readln( Cpl::Text::String& destString )
{
    bool io;
    char nextChar;
    char prevChar = ' ';

    // Make sure the destination string is empty
    destString.clear();

    // Loop till newline is found
    while ( (io = m_stream.read( nextChar )) )
    {
		// Read one character at a time
        destString += nextChar;

        // Trap the case: the current line exceeds the size of my destination buffer
        if ( destString.truncated() )
        {
            io = discardRemainingLine_( m_stream, m_newline, destString, prevChar, nextChar );
            break;  // Exit once newline was found
        }

        // Match newline
        else if ( destString.endsWith( m_newline ) )
        {
            destString.trimRight( strlen( m_newline ) );
            break;
        }

        // cache the previous/last character read
        prevChar = nextChar;

		// Check for EOS/EOF
		if ( m_stream.isEos() )
		{
			io = false;
			break;
		}
	}

    // Return the underlying stream status 
    return io;
}


///////////////////
bool discardRemainingLine_( Input& fd, const char* newline, Cpl::Text::String& destString, char lastChar, char overflowChar )
{
    bool io = true;

    // CASE: Single character newline
    if ( strlen( newline ) == 1 )
    {
        // Drain the input till newline is found (staring the overflowChar from the 'line read')
        while ( io && overflowChar != *newline )
        {
            io = fd.read( overflowChar );
        }
    }


    // CASE: Newline is two characters
    else
    {
        Cpl::Text::FString<2> buffer( lastChar );
        buffer += overflowChar;

        // Trap the case where newline is 2 characters and is the 1st character is in the client's buffer
        if ( buffer.startsWith( newline ) )
        {
            destString.trimRight( 1 );    // Remove the partial newline from the client's buffer
            return true;
        }

        // Search for newline characters
        while ( io && !buffer.startsWith( newline ) )
        {
            char inChar;

            buffer.trimLeft( 1 );
            io      = fd.read( inChar );
            buffer += inChar;
        }
    }


    return io;
}