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

#include "RxFrameHandler.h"


using namespace Driver::TPipe;

RxFrameHandler::RxFrameHandler( Cpl::Container::Map<RxFrameHandlerApi>& handlerList, const char* verb ) noexcept
    :m_mapKey( verb )
{
    // Self register into the Handler list
    handlerList.insert( *this );
}

const char* RxFrameHandler::getVerb() const noexcept
{
    return m_mapKey.getKeyValue();
}


const Cpl::Container::Key& RxFrameHandler::getKey() const noexcept
{
    return m_mapKey;
}
