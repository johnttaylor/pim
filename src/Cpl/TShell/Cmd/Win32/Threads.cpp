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

#include "Threads.h"
#include <windows.h>


///
using namespace Cpl::TShell::Cmd::Win32;



///////////////////////////
Threads::Threads( Cpl::Container::Map<Cpl::TShell::Command>& commandList ) noexcept
	:Cpl::TShell::Cmd::Threads( commandList )
{
}


/////////////////////////////////////////////////////////
void Threads::hookHeader1( Cpl::Text::String& text )
{
	text.formatAppend( "  %-3s  %-12s  %-12s", "Pri", "User Time", "Kernel Time" );
}

void Threads::hookHeader2( Cpl::Text::String& text )
{
	text.formatAppend( "  %-3s  %-12s  %-12s", "---", "---------", "-----------" );
}

void Threads::hookThreadEntry( Cpl::Text::String& text, Cpl::System::Thread& currentThread )
{
	HANDLE hdl = currentThread.getNativeHandle();

	// Get Priority (and skip Win32 specific columns if an error (this will happen with C++11 threads on the 'main' thread)
	int priority = GetThreadPriority( hdl );
	if ( priority == THREAD_PRIORITY_ERROR_RETURN )
	{
		return;
	}

	// Get Thread Times. NOTE: The accuracy of the GetThreadTimes() values is VERY Suspect (google 'GetThreadTimes())
	FILETIME ftCreate, ftExit, ftKernel, ftUser;
	if ( !GetThreadTimes( hdl, &ftCreate, &ftExit, &ftKernel, &ftUser ) )
	{
		return;
	}

	// Output info
	SYSTEMTIME kt, ut;
	FileTimeToSystemTime( &ftKernel, &kt );
	FileTimeToSystemTime( &ftUser, &ut );
	text.formatAppend( "  %3d  %02d:%02d:%02d.%03d  %02d:%02d:%02d.%03d", GetThreadPriority( hdl ),
		ut.wHour,
		ut.wMinute,
		ut.wSecond,
		ut.wMilliseconds,
		kt.wHour,
		kt.wMinute,
		kt.wSecond,
		kt.wMilliseconds
	);
}

