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

#include "colony_config.h"
#include "Cpl/System/Api.h"

#include "Bsp/Api.h"
#include "Cpl/System/Thread.h"
#include "Cpl/System/Mutex.h"
#include "Cpl/System/Private_.h"
#include "Cpl/Text/atob.h"
#include "Cpl/Text/FString.h"
#include "Cpl/Text/Tokenizer/TextBlock.h"

#include "Cpl/TShell/Cmd/Help.h"
#include "Cpl/TShell/Cmd/Bye.h"
#include "Cpl/TShell/Cmd/Trace.h"
#include "Cpl/TShell/Cmd/TPrint.h"
#include "Cpl/TShell/Cmd/Threads.h"


#define SECT_     "_0test"

////////////////////////////////////////////////////////////////////////////////
namespace {


class Apple : public Cpl::System::Runnable
{
public:
	///
	Cpl::System::Mutex m_lock;
	///
	unsigned long      m_delay;
	///
	bool               m_outputTrace;

public:
	///
	Apple()
		:m_delay( 250 )
		, m_outputTrace( false )
	{
	}

public:
	/// 
	void setOutputState( bool newstate )
	{
		Cpl::System::Mutex::ScopeBlock lock( m_lock );
		m_outputTrace = newstate;
	}

	/// 
	void setDelay( unsigned long newdelay )
	{
		Cpl::System::Mutex::ScopeBlock lock( m_lock );
		m_delay = newdelay;
	}

public:
	///
	void appRun()
	{
		unsigned counter = 0;
		for ( ;;)
		{
			Bsp_Api_toggle_debug1();
			counter++;

			m_lock.lock();
			bool          output = m_outputTrace;
			unsigned long delay  = m_delay;
			m_lock.unlock();

			Cpl::System::Api::sleep( delay );
			if ( output )
			{
				CPL_SYSTEM_TRACE_MSG( SECT_, ( "Trace (_0test): loop counter=%u", counter ) );
				CPL_SYSTEM_TRACE_MSG( "bob", ( "Trace (bob): loop counter=%u", counter ) );
			}
		}
	}
};


class Bob : public Cpl::TShell::Cmd::Command
{
public:
	/// See Cpl::TShell::Command
	const char* getUsage() const noexcept { return "bob on|off [delay]"; }

	/// See Cpl::TShell::Command
	const char* getHelp() const noexcept { return "  Sets the test trace output to on/off and delay time between msgs"; }

	///
	Apple& m_app;


public:
	/// Constructor
	Bob( Cpl::Container::Map<Cpl::TShell::Command>& commandList, Apple& application ) noexcept
		:Command( commandList, "bob" )
		, m_app( application )
	{
	}

public:
	/// See Cpl::TShell::Command
	Cpl::TShell::Command::Result_T execute( Cpl::TShell::Context_& context, char* cmdString, Cpl::Io::Output& outfd ) noexcept
	{
		Cpl::Text::Tokenizer::TextBlock tokens( cmdString, context.getDelimiterChar(), context.getTerminatorChar(), context.getQuoteChar(), context.getEscapeChar() );
		Cpl::Text::String& token = context.getTokenBuffer();

		// Error checking
		if ( tokens.numParameters() > 3 || tokens.numParameters() < 2 )
		{
			return Command::eERROR_INVALID_ARGS;
		}

		// Set output state
		token = tokens.getParameter( 1 );
		m_app.setOutputState( token == "on" ? true : false );

		// Set delay
		if ( tokens.numParameters() > 2 )
		{
			unsigned long newdelay = 250;
			Cpl::Text::a2ul( newdelay, tokens.getParameter( 2 ) );
			m_app.setDelay( newdelay );
		}

		return Command::eSUCCESS;
	}
};


};  // end namespace






