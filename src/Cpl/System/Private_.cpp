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


#include "Cpl/Container/SList.h"
#include "Private_.h"

/// 
using namespace Cpl::System;

///
static Cpl::Container::SList<StartupHook_> levelTest_( "invoke_special_static_constructor" );
static Cpl::Container::SList<StartupHook_> levelSystem_( "invoke_special_static_constructor" );
static Cpl::Container::SList<StartupHook_> levelMiddleWare_( "invoke_special_static_constructor" );
static Cpl::Container::SList<StartupHook_> levelApplication_( "invoke_special_static_constructor" );



////////////////////////////////////////////////////////////////////////////////
StartupHook_::StartupHook_( InitLevel_T myInitLevel )
{
    registerHook( *this, myInitLevel );
}


void StartupHook_::registerHook( StartupHook_& callbackInstance, InitLevel_T initOrder )
{
    // NOTE: The assumption is that sub-system register at the time of when
    //       constructor for static object execute - which is before main()
    //       is enter so there should be only one thread.
    if ( initOrder == StartupHook_::eTEST_INFRA )
    {
        levelTest_.put( callbackInstance );
    }
    else if ( initOrder == StartupHook_::eSYSTEM )
    {
        levelSystem_.put( callbackInstance );
    }
    else if ( initOrder == StartupHook_::eMIDDLE_WARE )
    {
        levelMiddleWare_.put( callbackInstance );
    }
    else
    {
        levelApplication_.put( callbackInstance );
    }
}

void StartupHook_::notifyStartupClients( void )
{
    // NOTE: The assumption is that no thread protection is required since
    //       there should only be one caller to the initialize() method.

    // Do eTEST_INFRA level first
    StartupHook_* ptr = levelTest_.get();
    while ( ptr != 0 )
    {
        ptr->notify( eTEST_INFRA );
        ptr = levelTest_.get();
    }

    // Do eSYSTEM level next
    ptr = levelSystem_.get();
    while ( ptr != 0 )
    {
        ptr->notify( eSYSTEM );
        ptr = levelSystem_.get();
    }

    // Do eMIDDLE_WARE level next
    ptr = levelMiddleWare_.get();
    while ( ptr != 0 )
    {
        ptr->notify( eMIDDLE_WARE );
        ptr = levelMiddleWare_.get();
    }

    // Do eAPPLICATION level last
    ptr = levelApplication_.get();
    while ( ptr != 0 )
    {
        ptr->notify( eAPPLICATION );
        ptr = levelApplication_.get();
    }
}
