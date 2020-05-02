#ifndef Storm_Component_Api_h_
#define Storm_Component_Api_h_
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Apps Project.  The Colony.Apps Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.apps/license.txt
*
* Copyright (c) 2015-2020  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file */

#include "Cpl/System/ElapsedTime.h"

/// Namespaces
namespace Storm {
/// Namespaces
namespace Component {


/** This class define the interface for a Component.  A Component is a data
	driven 'block' that implements a function, algorithm, stuff, etc.  Components
	are designed to be run periodically.  The application is responsible for
	determining/controlling the runtime execution order of component instances.
 */
class Api
{
public:
	/** This method completes any/all initialization for the Component.  This
		method MUST be called in the same thread context as the do() method.
		The 'interval' argument is the periodic time interval between component
		executions, e.g. a value of '50msec' means the component's do()
		method will be executes its logic at 20Hz. This method should ONLY be
		ONCE (without a call to stop()) and BEFORE any calls to do();  This
		method return true when successfully; else false is returned.
	 */
	virtual bool start( Cpl::System::ElapsedTime::Precision_T intervalTime ) = 0;


	/** This method is called to have a component perform its work.  This
		method MUST be called at least a frequently (if not faster) as the
		desired/designed periodic rate of the component (see in the start()
		call).  Component are responsible for managing their own periodic
		rate, i.e. this method should be called from the thread's top level
		forever main() loop.

		The 'enabled' argument provides a runtime/per-each-cycle option to
		enabled or disable the Component's processing.  When 'enabled' is
		false this method does nothing.

		The 'currentTick' argument represents the current elapsed time
		since power-up of the application.  The application should pass
		the same 'currentTick' value to ALL components being executed in
		the thread's main loop. This allows all component to synchronize
		their execution with respect other components in main loop.

		This method return true if the Component completed ALL of its
		logic for the current processing cycle; else if an error occurred
		then false is returned.
	 */
	virtual bool doWork( bool enabled, Cpl::System::ElapsedTime::Precision_T currentTick ) = 0;


	/** This method will stop/shutdown the Component.  Once this method is
		called any future calls to the do() method will treated as NOPs. This
		method MUST be called in the same thread context as the do() method.
	 */
	virtual bool stop( void ) = 0;


public:
	/// Virtual destructor
	~Api() {}

};


};      // end namespace
};      // end namespace
#endif  // end header latch
