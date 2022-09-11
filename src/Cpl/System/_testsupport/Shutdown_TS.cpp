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

#include "Shutdown_TS.h"
#include "Cpl/System/Shutdown.h"
#include "Cpl/System/Private_.h"
#include <stdlib.h>


///
using namespace Cpl::System;


///
static size_t counter_  = 0;
static bool   counting_ = true;
static bool   testing_  = true;
static int    exitCode_ = 0;


////////////////////////////////////////////////////////////////////////////////
void Shutdown_TS::clearAndUseCounter( void )
{
    Locks_::system().lock();
    counter_  = 0;
    counting_ = true;
    testing_  = true;
    Locks_::system().unlock();
}

size_t Shutdown_TS::getAndClearCounter( void )
{
    Locks_::system().lock();
    size_t temp = counter_;
    counter_    = 0;
    Locks_::system().unlock();

    return temp;
}


void Shutdown_TS::setExitCode( int new_exit_code )
{
    Locks_::system().lock();
    exitCode_ = new_exit_code;
    counting_ = false;
    testing_  = true;
    Locks_::system().unlock();
}


void Shutdown_TS::restore( void )
{
    Locks_::system().lock();
    testing_ = false;
    Locks_::system().unlock();
}


////////////////////////////////////////////////////////////////////////////////
static int preprocess_shutdown_( int exit_code, bool& true_exit )
{
    Locks_::system().lock();

    if ( !testing_ )
    {
        true_exit = true;
    }
    else
    {
        if ( counting_ )
        {
            counter_++;
            true_exit = false;
        }
        else
        {
            true_exit = true;
            exit_code = exitCode_;
        }
    }

    Locks_::system().unlock();
    return exit_code;
}


int Shutdown::success( void )
{
    bool true_exit = true;
    int  exit_code = preprocess_shutdown_( OPTION_CPL_SYSTEM_SHUTDOWN_SUCCESS_ERROR_CODE, true_exit );
    if ( true_exit )
    {
        exit_code = notifyShutdownHandlers_( exit_code );
        exit( exit_code );
    }

    return exit_code;
}

int Shutdown::failure( int exit_code )
{
    bool true_exit = true;
    exit_code      = preprocess_shutdown_( exit_code, true_exit );
    if ( true_exit )
    {
        exit_code = notifyShutdownHandlers_( exit_code );
        exit( exit_code );
    }

    return exit_code;
}





