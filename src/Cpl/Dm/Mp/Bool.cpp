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
/** @file */


#include "Bool.h"

///
using namespace Cpl::Dm::Mp;


///////////////////////////////////////////////////////////////////////////////
void Bool::attach( Observer& observer, uint16_t initialSeqNumber ) noexcept
{
    ModelPointCommon_::attach( observer, initialSeqNumber );
}

void Bool::detach( Observer& observer ) noexcept
{
    ModelPointCommon_::detach( observer );
}

const char* Bool::getTypeAsText() const noexcept
{
    return "Cpl::Dm::Mp::Bool";
}

void Bool::setJSONVal( JsonDocument& doc ) noexcept
{
    doc["val"] = m_data;
}

bool Bool::fromJSON_( JsonVariant& src, LockRequest_T lockRequest, uint16_t& retSequenceNumber, Cpl::Text::String* errorMsg ) noexcept
{
    if ( src.is<bool>() )
    {
        retSequenceNumber = write( src.as<bool>(), lockRequest );
        return true;
    }
    if ( errorMsg )
    {
        *errorMsg = "Invalid syntax for the 'val' key/value pair";
    }
    return false;
}
