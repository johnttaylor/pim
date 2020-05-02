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


#include "Decoder_.h"


///
using namespace Cpl::Text::Frame;




/////////////////////////////////////////////////////////////////////////////
Decoder_::Decoder_( char buffer[], size_t bufsize )
	:m_dataLen( 0 )
	, m_dataPtr( 0 )
	, m_buffer( buffer )
	, m_bufSize( bufsize )
{
}


/////////////////////////////////////////////////////////////////////////////
bool Decoder_::scan( size_t maxSizeOfFrame, char* frame, size_t& frameSize ) noexcept
{
	// Housekeeping
	bool  inFrame  = false;
	bool  escaping = false;
	char* framePtr = 0;

	// Zero out size of the found frame
	frameSize = 0;

	// Error case: Treat a null frame buffer as an IO failure case
	if ( !frame )
	{
		return false;
	}

	// Scan till a frame is found
	for ( ;;)
	{
		if ( !m_dataLen )
		{
			if ( !read( m_buffer, m_bufSize, m_dataLen ) )
			{
				// Error reading data -->exit scan
				m_dataLen = 0; // Reset my internal count so I start 'over' on the next call (if there is one)
				return false;
			}

			// Reset my data pointer
			m_dataPtr = m_buffer;
		}

		// Process my input buffer one character at a time
		for ( ; m_dataLen; m_dataLen--, m_dataPtr++ )
		{
			// OUTSIDE of a frame
			if ( !inFrame )
			{
				if ( isStartOfFrame() )
				{
					inFrame   = true;
					escaping  = false;
					frameSize = 0;
					framePtr  = frame;
				}
			}

			// INSIDE a frame
			else
			{
				// Trap illegal characters
				if ( !isLegalCharacter() )
				{
					inFrame = false;
				}

				// No escape sequence in progress
				else if ( !escaping )
				{
					// EOF Character

					if ( isEofOfFrame() )
					{
						// EXIT routine with a success return code
						m_dataPtr++;    // Explicitly consume the EOF character (since we are brute force exiting the loop)
						m_dataLen--;
						return true;
					}

					// Regular character
					else if ( !isEscapeChar() )
					{
						// Store incoming character into the Client's buffer
						if ( frameSize < maxSizeOfFrame )
						{
							*framePtr++ = *m_dataPtr;
							frameSize++;
						}

						// Exceeded the Client's buffer space -->internal error -->reset my Frame state
						else
						{
							inFrame = false;
						}
					}

					// Start escape sequence
					else
					{
						escaping = true;
					}
				}


				// Escape Sequence
				else
				{
					// Store the escaped character into the Client's buffer
					if ( frameSize < maxSizeOfFrame )
					{
						escaping    = false;
						*framePtr++ = *m_dataPtr;
						frameSize++;
					}

					// Exceeded the Client's buffer space -->internal error -->reset my Frame state
					else
					{
						inFrame = false;
					}
				}
			}
		}
	}

	// I should never get here!
	return false;
}



