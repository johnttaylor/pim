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


#include "Shutdown.h"
#include "Private_.h"
#include "Cpl/Container/SList.h"


/// 
using namespace Cpl::System;


///
static Cpl::Container::SList<Shutdown::Handler> callbacks_( "invoke_special_static_constructor" );



////////////////////////////////////////////////////////////////////////////////
void Shutdown::registerHandler( Shutdown::Handler& instanceToRegister )
{
    // Use a LIFO for registered handlers so the first handler registered is 
    // the last handler run.  NOTE: This is UNDOCUMENUTED BEHAVIOR but is done this
    // way to facility unit testing.  NO APPLICATION CAN RELY-ON/ASSUME THIS BEHAVIOR!
    Locks_::system().lock();
    callbacks_.push( instanceToRegister );
    Locks_::system().unlock();
}


////////////////////////////////////////////////////////////////////////////////
int Shutdown::notifyShutdownHandlers_( int exit_code )
{
    Locks_::system().lock();
    Shutdown::Handler* ptr = callbacks_.get();
    Locks_::system().unlock();

    while ( ptr )
    {
        exit_code = ptr->notify( exit_code );

        Locks_::system().lock();
        ptr = callbacks_.pop();
        Locks_::system().unlock();
    }

    return exit_code;
}
