#ifndef Cpl_Json_ObjectDetector_h_
#define Cpl_Json_ObjectDetector_h_
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
/** @file */

#include <stdlib.h>     // for size_t


///
namespace Cpl {
///
namespace Json {

/** This helper class is used to determine when an entire Json object has
	been read from an input source.

	This class does not read or store the incoming data - it simply inspect
	the provide data (one byte at time) and declares a JSON object when it
	see as match trailing '}' for the leading '{'

	Usage:
	\code


		Cpl::Io::File::Input fd("bob.txt");
		ObjectDetector       detector;

		while( fd.eof() == false )
		{
			uint8_t buffer[256];
			int     bytesRead;
			if ( fd.read( buffer, sizeof(buffer), bytesRead ) )
			{
				size_t startPosition;
				size_t endPosition;

				if ( detector.scan( buffer, bytesRead, startPosition, endPosition ) )
				{
					// JSON object found.  
					// The initial '{' is at  'startPosition'
					// The final/closing '}' is at 'endPosition'
					// DO Something...
				}
			}
		}

	\endcode


 */
class ObjectDetector
{
public:
	ObjectDetector();

public:
	/** Inspect 'numBytes' and returns true if a complete JSON object was
		found.  The pointers 'startOffset' and 'endOffset' return the byte
		offset to the start and end of the found JSON object.  The offset is
		based on the number of number consumed since the object was created
		or reset() was called.

		NOTE: The class does NOT VALIDATE the JSON syntax. It ONLY finds
			  starting and trailing curly braces.  It assumes the JSON
			  syntax and quoted character escaping semantics when searching for 
			  the trailing curly brace.

		The method will ALWAYS return true if called after detecting a JSON 
		object and set 'startOffset' and 'endOffset' to zero. Use the reset() 
		method to restart/reuse the detector instance.
	 */
	bool scan( const void*	inputStream,
			   size_t		numBytesToScan,
			   size_t&		startOffset,
			   size_t&		endOffset );

	/// Resets the detector to being looking for a new JSON object
	void reset();

protected:
	/// Detector state
	enum State_T
	{
		eNOT_STARTED,
		ePLAIN,
		eQUOTED,
		eESCAPED,
		eFOUND
	};

	/// Current offset
	size_t	m_offset;

	/// Offset of the initial '{'
	size_t	m_startOffset;

	/// Number of '{' encountered
	size_t	m_braceCount;

	/// Quoted key/value state
	State_T m_state;
};

};      // end namespaces
};
#endif  // end header latch
