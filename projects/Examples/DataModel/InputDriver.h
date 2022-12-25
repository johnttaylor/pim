#ifndef InputDriver_h_
#define InputDriver_h_
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

#include "Cpl/Itc/CloseSync.h"
#include "Cpl/System/Timer.h"
#include "Cpl/Dm/MailboxServer.h"
#include "Cpl/Dm/Mp/Uint32.h"


/** This concrete class is an dumb-downed simulation of an input driver that
    'samples' a physical signal and provide the value to the application via a
    model point.  Since the example is intended to run on a desktop PC - the
    sample is generated using the C Library's rand() function. A new value is
    generated every 10 ms.
 */
class InputDriver : public Cpl::Itc::CloseSync, public Cpl::System::Timer
{

public:
    /// Constructor.
    InputDriver( Cpl::Dm::MailboxServer& myMbox, 
                 Cpl::Dm::Mp::Uint32&    mpToUpdate );


public:
    /** This ITC method is used to initialize the driver while executing
        in the 'driver' thread.  See Cpl::Itc::OpenRequest
     */
    void request( Cpl::Itc::OpenRequest::OpenMsg& msg );

    /** This ITC method is used to shutdown the driver while executing
        in the 'driver' thread.  See Cpl::Itc::OpenRequest
     */
    void request( Cpl::Itc::CloseRequest::CloseMsg& msg );


protected:
    /// My 100Hz Timer callback
    void expired( void ) noexcept;

protected:
    /// MP to update when new sample values
    Cpl::Dm::Mp::Uint32&    m_mpSample;

    /// My open state
    bool                    m_opened;
};

#endif  // end header latch
