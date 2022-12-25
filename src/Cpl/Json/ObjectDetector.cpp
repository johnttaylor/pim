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

#include "ObjectDetector.h"
#include <stdint.h>

using namespace Cpl::Json;


//////////////////////////////////
ObjectDetector::ObjectDetector()
{
    reset();
}

void ObjectDetector::reset()
{
    m_offset      = 0;
    m_startOffset = 0;
    m_braceCount  = 0;
    m_state       = eNOT_STARTED;
}

bool ObjectDetector::scan( const void*	inputStream,
                           size_t	    numBytesToScan,
                           size_t&	    startOffset,
                           size_t&	    endOffset )
{
    const uint8_t* srcPtr = (const uint8_t*) inputStream;
    while ( numBytesToScan )
    {
        switch ( m_state )
        {
        case eNOT_STARTED:
            if ( *srcPtr == '{' )
            {
                m_state       = ePLAIN;
                m_startOffset = m_offset;
                m_braceCount++;
            }
            break;

        case ePLAIN:
            if ( *srcPtr == '{' )
            {
                m_braceCount++;
            }
            else if ( *srcPtr == '}' )
            {
                m_braceCount--;
                if ( m_braceCount == 0 )
                {
                    m_state = eFOUND;
                    startOffset = m_startOffset;
                    endOffset = m_offset;
                    return true;
                }
            }
            else if ( *srcPtr == '"' )
            {
                m_state = eQUOTED;
            }
            break;

        case eQUOTED:
            if ( *srcPtr == '"' )
            {
                m_state = ePLAIN;
            }
            else if ( *srcPtr == '\\' )
            {
                m_state = eESCAPED;
            }
            break;

        case eESCAPED:
            m_state = eQUOTED;
            break;

        case eFOUND:    // Intentional fall through
        default:
            // Generate 'null' match
            startOffset = 0;
            endOffset   = 0;
            return true;
            break;
        }

        m_offset++;
        srcPtr++;
        numBytesToScan--;
    }

    // If I get here, no object has been found yet
    return false;
}
