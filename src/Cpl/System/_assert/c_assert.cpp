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

#include "c_assert.h"
#include "Cpl/System/FatalError.h"

void cpl_system_assert_c_wrapper( bool true_expression, const char* file, unsigned line, const char* func )
{
    if ( !true_expression )
    { 
        Cpl::System::FatalError::logf( "ASSERT Failed at: file=%s, line=%d, func=%s\n", file, line, func );
    }
}

