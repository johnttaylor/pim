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

#include "Item.h"
#include "Cpl/System/FatalError.h"    


/// 
using namespace Cpl::Container;
using namespace Cpl::System;


////////////////////////////////////////////////////////////////////////////////
bool Item::insert_( void* newContainerPtr )
{
    if ( m_inListPtr_ )
    {
        FatalError::logf( "Container Error: Double Insert. item=%p, prev container=%p, new container=%p", this, m_inListPtr_, newContainerPtr );
        return false;
    }
    else
    {
        m_inListPtr_ = newContainerPtr;
        return true;
    }
}

bool Item::isInContainer_( const void* containerPtr ) const noexcept
{
    if ( m_inListPtr_ != containerPtr )
    {
        return false;
    }
    return true;
}

void Item::remove_( Item* itemPtr ) noexcept
{
    if ( itemPtr )
    {
        itemPtr->m_inListPtr_ = 0;
    }
}


