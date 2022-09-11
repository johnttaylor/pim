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

#include "Threads.h"
#include "task.h"


///
using namespace Cpl::TShell::Cmd::FreeRTOS;

static const char* state2text_( eTaskState state );


///////////////////////////
Threads::Threads( Cpl::Container::Map<Cpl::TShell::Command>& commandList ) noexcept
    :Cpl::TShell::Cmd::Threads( commandList )
{
}


/////////////////////////////////////////////////////////
void Threads::hookHeader1( Cpl::Text::String& text )
{
    text.formatAppend( "  %-3s  %-5s  %-5s", "Pri", "State", "Stack" );
}

void Threads::hookHeader2( Cpl::Text::String& text )
{
    text.formatAppend( "  %-3s  %-5s  %-5s", "---", "-----", "-----" );
}

void Threads::hookThreadEntry( Cpl::Text::String& text, Cpl::System::Thread& currentThread )
{
    Cpl_System_Thread_NativeHdl_T hdl = currentThread.getNativeHandle();

    text.formatAppend( "  %3u  %-5s  %5u", uxTaskPriorityGet( hdl ), state2text_( eTaskGetState( hdl ) ), uxTaskGetStackHighWaterMark( hdl ) );
}



const char* state2text_( eTaskState state )
{
    switch ( state )
    {
    case eReady:      return "Ready";
    case eRunning:    return " Run ";
    case eBlocked:    return "Blckd";
    case eSuspended:  return "Suspd";
    case eDeleted:    return " Del ";
    default:          return " ??? ";
    }

    return "*ERROR*";
}