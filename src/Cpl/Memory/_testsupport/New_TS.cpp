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

#include "New_TS.h"
#include "Cpl/System/Shutdown.h"
#include "Cpl/System/Private_.h"
#include <stdlib.h>
#include <stdio.h>
#include <new>

///
using namespace Cpl::Memory;

///
static New_TS::Stats      metrics_;
static bool               disabled_;
static long               newDelete_delta_;
static bool               ignoreDelta_;
static void clear_metrics( New_TS::Stats& stats );


//////////////////////////////////////////////////////////////////////////
namespace {

class ExitHandler : public Cpl::System::Shutdown::Handler
{
protected:
	///
	int notify( int exit_code )
	{
		const char* msg = "new/delete call delta MATCHES expected value.";
		long  delta     = (long) ( metrics_.m_numNewCalls - metrics_.m_numDeleteCalls );
		if ( ignoreDelta_ )
		{
			msg = "new/delete call delta being IGNORED.";
		}
		else if ( delta != newDelete_delta_ )
		{
			exit_code = OPTION_CPL_SYSTEM_SHUTDOWN_FAILURE_ERROR_CODE;
			msg       = "ERROR: new/delete call delta does NOT match expected value.";
		}

		printf( "\n----------------------------------------------------------" );
		printf( "\nCpl::Memory::New_TS Metrics:\n" );
		printf( "\n    Num new()              = %lu", metrics_.m_numNewCalls );
		printf( "\n    Num new() disabled     = %lu", metrics_.m_numNewCallsFailed );
		printf( "\n    Num delete()           = %lu", metrics_.m_numDeleteCalls );
		printf( "\n    Num delete(0)          = %lu", metrics_.m_numDeleteCalls_withNullPtr );
		printf( "\n    Bytes Allocated        = %lu", (unsigned long) metrics_.m_bytesAllocated );
		printf( "\n    Expected new/del delta = %ld", newDelete_delta_ );
		printf( "\n    Actual new/del delta   = %ld", delta );
		printf( "\n\n%s", msg );
		printf( "\n----------------------------------------------------------\n" );

		return exit_code;
	}
};


class RegisterInitHandler : public Cpl::System::StartupHook_
{
public:
	///
	ExitHandler m_shutdown;


public:
	///
	RegisterInitHandler() :StartupHook_( eTEST_INFRA ) {}


protected:
	///
	void notify( InitLevel_T init_level )
	{
		Cpl::System::Shutdown::registerHandler( m_shutdown );
	}

};
}; // end namespace

///
static RegisterInitHandler autoRegister_systemInit_hook;


//////////////////////////////////////////////////////////////////////////
static void* my_new( size_t sz ) noexcept
{
	// Update metrics
	metrics_.m_bytesAllocated += sz;
	if ( disabled_ )
	{
		// Fail allocation if not 'enabled'
		metrics_.m_numNewCallsFailed++;
		return 0;
	}
	else
	{
		metrics_.m_numNewCalls++;
	}

	return malloc( sz );
}


static void my_delete( void* ptr )
{
	// Update metrics
	metrics_.m_numDeleteCalls++;
	if ( !ptr )
	{
		metrics_.m_numDeleteCalls_withNullPtr++;
	}

	// C++ standard says it is okay to delete a null pointer (make sure we DON'T free a null pointer)
	if ( ptr )
	{
		free( ptr );
	}
}

void* operator new( size_t sz, const std::nothrow_t& nothrow_value ) noexcept
{
	return my_new( sz );
}

void* operator new[]( size_t sz, const std::nothrow_t& nothrow_value ) noexcept
{
	return my_new( sz );
}

void operator delete( void* ptr ) noexcept
{
	my_delete( ptr );
}

void operator delete[]( void* ptr ) noexcept
{
	my_delete( ptr );
}


////////////////////////////////////////////////////////////////////////////////
void New_TS::disable( void )
{
	disabled_ = true;
}


void New_TS::enable( void )
{
	disabled_ = false;
}


void New_TS::setNewDelete_delta( long delta, bool ignoreDelta )
{
	newDelete_delta_ = delta;
	ignoreDelta_     = ignoreDelta;
}


void New_TS::getStats( Stats& stats, bool resetStats )
{
	stats = metrics_;
	if ( resetStats )
	{
		clear_metrics( metrics_ );
	}
}

void New_TS::clearStats()
{
	clear_metrics( metrics_ );
}

void clear_metrics( New_TS::Stats& stats )
{
	stats.m_numNewCalls                = 0;
	stats.m_numNewCallsFailed          = 0;
	stats.m_numDeleteCalls             = 0;
	stats.m_numDeleteCalls_withNullPtr = 0;
	stats.m_bytesAllocated             = 0;
}
