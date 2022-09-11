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

#include "Key.h"
#include <string.h>



/// 
using namespace Cpl::Container;


/////////////////////////////////////////////////////////////////////////////
KeyStringBuffer::KeyStringBuffer( const char* startOfString, size_t lenOfStringInBytes )
    :m_stringKeyPtr( startOfString )
    , m_len( lenOfStringInBytes )
{
}

int KeyStringBuffer::compareKey( const Key& key ) const
{
    unsigned    otherLen = 0;
    const char* otherPtr = (const char*) key.getRawKey( &otherLen );
    return compare( m_stringKeyPtr, m_len, otherPtr, otherLen );
}


const void* KeyStringBuffer::getRawKey( unsigned* returnRawKeyLenPtr ) const
{
    if ( returnRawKeyLenPtr )
    {
        *returnRawKeyLenPtr = m_stringKeyPtr ? m_len : 0;
    }

    return m_stringKeyPtr;
}

int KeyStringBuffer::compare( const char* myString, unsigned myLen, const char* otherString, unsigned otherLen )
{
    if ( otherString )
    {
        if ( myString )
        {
            unsigned cmpLen      = myLen > otherLen ? otherLen : myLen;
            int      comparision = strncmp( myString, otherString, cmpLen );

            if ( comparision == 0 && myLen != otherLen )
            {
                return myLen - (int) otherLen;
            }

            return comparision;
        }
    }

    return -1;
}