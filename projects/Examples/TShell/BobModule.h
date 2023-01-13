#ifndef BobModule_h
#define BobModule_h
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

#include "Cpl/System/Mutex.h"
#include "Cpl/System/Timer.h"
#include "Cpl/System/ElapsedTime.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Itc/CloseSync.h"
#include "Cpl/Dm/MailboxServer.h"
#include "ModelPoints.h"


// Trace section name for Bob
#define BOB_SECT_		"bob"


/*---------------------------------------------------------------------------*/
/** This class represents an sub-system.  Bob is a silly module that increments 
	a counter every N msecs and echoes the counter value to the TRACE engine 
	(when enabled).  Bob uses the following model points to interact with the 
	rest of the application.

	mp::delayMs			- delay time, in milliseconds, between counter increments
	mp::enableTrace		- flag to enable/disable trace output

	Note: Bob uses polled semantic for accessing its model points.  This is
	      done keep the example as simply as possible and to keep the focus
		  on the TShell (not Data Model subscriptions)
 */
class BobModule : public Cpl::Itc::CloseSync, public Cpl::System::Timer
{

public:
	/** Constructor. The 'myMbox' argument is reference to the 'Application'
		runnable-object/thread (i.e. the thread that Bob will execute in).
	 */
	BobModule( Cpl::Dm::MailboxServer&  myMbox )
		: Cpl::Itc::CloseSync( myMbox )
		, Cpl::System::Timer( myMbox )
	{
	}


public:
	/** This ITC method is used to initialize Bob while executing
		in the 'application' thread.  See Cpl::Itc::OpenRequest
	 */
	void request( Cpl::Itc::OpenRequest::OpenMsg& msg )
	{
		m_counter    = 0;
		m_timeMarker = Cpl::System::ElapsedTime::milliseconds();
		mp::bobsDelayMs.write( 1000 );	// Set the default delay to 1 sec
		expired();						// Process Bob's state
		msg.returnToSender();			// Return the ITC message as required
	}

	/** This ITC method is used to shutdown Bob while executing
		in the 'application' thread.  See Cpl::Itc::OpenRequest
	 */
	void request( Cpl::Itc::CloseRequest::CloseMsg& msg )
	{
		Timer::stop();			// Make sure my internal timer gets stopped
		msg.returnToSender();	// Return the ITC message as required
	}


protected:
	/// Periodic timer to inspect the current state of affairs
	void expired( void ) noexcept
	{
		// Read the delay model point (only consume if it has valid value)
		uint32_t delayMs;
		if ( mp::bobsDelayMs.read( delayMs ) )
		{
			// Check the delay has expired
			uint32_t now = Cpl::System::ElapsedTime::milliseconds();
			if ( Cpl::System::ElapsedTime::expiredMilliseconds( m_timeMarker, delayMs, now ) )
			{
				m_counter++;
				m_timeMarker = now;

				// Echo the new value using the Trace engine - but only when 'enabled'
				bool enabled;
				if ( mp::enableBobsTrace.read( enabled ) && enabled )
				{
					CPL_SYSTEM_TRACE_MSG( BOB_SECT_, ("Bob's counter: %lu", m_counter) );
				}
			}
		}


		// Poll my state every 10ms
		Timer::start( 10 );
	}

protected:
	/// Time marker (in milliseconds) of when Bob's counter was incremented
	uint32_t	m_timeMarker;

	/// The counter
	uint32_t	m_counter;
};

#endif	// end header latch







