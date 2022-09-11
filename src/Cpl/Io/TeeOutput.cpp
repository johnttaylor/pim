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

#include "TeeOutput.h"


//
using namespace Cpl::Io;

///////////////////////////////
TeeOutput::TeeOutput()
    :m_opened( true )
{
}

TeeOutput::TeeOutput( Output& streamA )
    : m_opened( true )
{
    m_streams.put( streamA );
}

TeeOutput::TeeOutput( Output& streamA, Output& streamB )
    : m_opened( true )
{
    m_streams.put( streamA );
    m_streams.put( streamB );
}

///////////////////////////////
void TeeOutput::add( Output& stream )
{
    m_streams.put( stream );
}

bool TeeOutput::remove( Output& stream )
{
    if ( !m_streams.remove( stream ) )
    {
        return m_failed.remove( stream );
    }

    return true;
}

///////////////////////////////
Output* TeeOutput::firstFailed()
{
    return m_failed.first();
}

Output* TeeOutput::nextFailed( Output& currentFailedStream )
{
    return m_failed.next( currentFailedStream );
}


Output* TeeOutput::removeAndGetNextFailed( Output& currentFailedStream )
{
    Output* next = m_failed.next( currentFailedStream );
    if ( !m_failed.remove( currentFailedStream ) )
    {
        return 0;
    }

    return next;
}


///////////////////////////////
bool TeeOutput::write( const void* buffer, int maxBytes, int& bytesWritten )
{
    bool    rc     = true;
    Output* stream = m_streams.first();

    // Handle the special case of NO output streams
    if ( !stream )
    {
        // In this case -->act like the Null Output stream
        bytesWritten = maxBytes;
        return m_opened;         // Normally I return true, but If I have been closed -->I need to return false
    }

    // Ensure 'bytesWritten' starts at zero (so the trap-max-value logic works)
    bytesWritten = 0;

    // Loop through all active streams
    while ( stream )
    {
        // Cache the next stream in the 'active list'
        Output* next = m_streams.next( *stream );

        // Output to stream -->and trap any error
        int tempWritten;
        if ( !stream->write( buffer, maxBytes, tempWritten ) )
        {
            // move the stream with an error to the failed list
            m_streams.remove( *stream );
            m_failed.put( *stream );
            rc = false;
        }

        // Write succeeded -->trap max number of bytes written.
        else
        {
            if ( tempWritten > bytesWritten )
            {
                bytesWritten = tempWritten;
            }
        }

        // Continue with the next stream in the active list
        stream = next;
    }

    return rc;
}


void TeeOutput::flush()
{
    Output* stream = m_streams.first();
    while ( stream )
    {
        stream->flush();
        stream = m_streams.next( *stream );
    }
}

bool TeeOutput::isEos()
{
	bool    rc     = false;
	Output* stream = m_streams.first();
	while ( stream )
	{
		if ( stream->isEos() )
		{
			// move the stream with an error to the failed list
			m_streams.remove( *stream );
			m_failed.put( *stream );
			rc = true;
		}
	}

	return rc;
}

void TeeOutput::close()
{
    if ( m_opened )
    {
        // Remember that I am closed
        m_opened = false;

        // Close my active streams
        Output* stream = m_streams.first();
        while ( stream )
        {
            stream->close();
            stream = m_streams.next( *stream );
        }

        // Close any failed streams
        stream = m_failed.first();
        while( stream )
        {
            stream->close();
            stream = m_failed.next( *stream );
        }
    }
}
