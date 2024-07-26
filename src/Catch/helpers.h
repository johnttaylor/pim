#ifndef Cpl_Catch_helpers_h_
#define Cpl_Catch_helpers_h_
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2023  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file

    This file defines helper functions/interface for writing unit tests
 */

#include "colony_config.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/Mutex.h"
#include "Cpl/System/Semaphore.h"
#include "Cpl/Dm/ModelPoint.h"
#include "Cpl/Dm/MailboxServer.h"
#include "Cpl/Itc/MailboxServer.h"
#include "Cpl/Itc/CloseSync.h"
#include "Cpl/Io/Input.h"


/** Default Trace section used by/for the Helper methods
 */
#ifndef OPTION_CPL_CATCH_HELPERS_TRACE_SECT
#define OPTION_CPL_CATCH_HELPERS_TRACE_SECT     "_0test"
#endif

/*----------------------------------------------------------------------------*/
/** This function performs a blocking wait by monitoring a Model Point for
    a specific value.  The method waits a specified amount of time BEFORE
    checking for the expected value. The method returns true if the specified
    MP's values matches the expected value; else the method will eventually time
    out and return false.
 */
template<class MPTYPE, class VALTYPE>
bool minWaitOnModelPoint( MPTYPE& src, VALTYPE expectedVal, unsigned long minWait, VALTYPE* lastVal=nullptr, unsigned long maxWaitMs=10000, unsigned long waitDelayMs=10 )
{
    Cpl::System::Api::sleep( minWait );

    VALTYPE val;
    bool  valid = src.read( val );
    while ( maxWaitMs )
    {
        if ( valid && val == expectedVal )
        {
            // Return the last read value
            if ( lastVal )
            {
                *lastVal = val;
            }
            return true;
        }

        Cpl::System::Api::sleep( waitDelayMs );
        maxWaitMs -= waitDelayMs;
        valid      = src.read( val );
    }

    // Return the last read value
    if ( lastVal )
    {
        *lastVal = val;
    }
    return false;
};

/** This function is the same minWaitOnModelPoint() above, expect that it
    expected VALTYPE to be BETTER_NUM
 */
template<class MPTYPE, class VALTYPE>
bool minWaitOnEnumModelPoint( MPTYPE& src, VALTYPE expectedVal, unsigned long minWait, VALTYPE* lastVal=nullptr, unsigned long maxWaitMs=10000, unsigned long waitDelayMs=10 )
{
    Cpl::System::Api::sleep( minWait );

    VALTYPE val = VALTYPE::_from_integral_unchecked( 0 );
    bool  valid = src.read( val );
    while ( maxWaitMs )
    {
        if ( valid && val == expectedVal )
        {
            // Return the last read value
            if ( lastVal )
            {
                *lastVal = val;
            }
            return true;
        }

        Cpl::System::Api::sleep( waitDelayMs );
        maxWaitMs -= waitDelayMs;
        valid      = src.read( val );
    }

    // Return the last read value
    if ( lastVal )
    {
        *lastVal = val;
    }
    return false;
};

/** This method is similar to minWaitOnModelPoint<>(), except that it waits on
    valid/invalid state of the MP
 */
template<class MPTYPE>
bool minWaitOnModelPointValidState( MPTYPE& src, bool expectedValidState, unsigned long minWait, int8_t* lastVal=nullptr, unsigned long maxWaitMs=10000, unsigned long waitDelayMs=10 )
{
    Cpl::System::Api::sleep( minWait );

    bool state = !src.isNotValid();
    while ( maxWaitMs )
    {
        if ( state == expectedValidState )
        {
            // Return the last read state
            if ( lastVal )
            {
                *lastVal = state;
            }
            return true;
        }

        Cpl::System::Api::sleep( waitDelayMs );
        maxWaitMs -= waitDelayMs;
        state      = !src.isNotValid();
    }

    // Return the last read value
    if ( lastVal )
    {
        *lastVal = state;
    }
    return false;
};

/** This method is similar to minWaitOnModelPoint<>(), except that it waits for
    a change in the MP's sequence number
 */
template<class MPTYPE>
bool minWaitOnModelPointSeqNumChange( MPTYPE& src, uint16_t currentSeqNum, unsigned long minWait, uint16_t* newSeqNum=nullptr, unsigned long maxWaitMs=10000, unsigned long waitDelayMs=10 )
{
    Cpl::System::Api::sleep( minWait );

    uint16_t seqNum = src.getSequenceNumber();
    while ( maxWaitMs )
    {
        if ( seqNum != currentSeqNum )
        {
            // Return the new sequence number
            if ( newSeqNum )
            {
                *newSeqNum = seqNum;
            }
            return true;
        }

        Cpl::System::Api::sleep( waitDelayMs );
        maxWaitMs -= waitDelayMs;
        seqNum = src.getSequenceNumber();;
    }

    // Return the last read value
    if ( newSeqNum )
    {
        *newSeqNum = seqNum;
    }
    return false;
};

/** This class is a DM Mailbox server that signals the provided semaphore when 
    it begins execution.  This is helpful when you need synchronize your test 
    with the mailbox thread actually running

    The class also supports 'Pausing' the Mailbox server.  When the mailbox
    is paused the thread essentially stops executing - providing a pseudo thread 
    safe mechanism for the test manager to operate on objects/data/methods that
    execute in the paused thread.
 */
class DmMailbox : public Cpl::Dm::MailboxServer
{
public:
    /// Constructor
    DmMailbox( Cpl::System::Semaphore&              signalToNotify,
               unsigned long                        timingTickInMsec = OPTION_CPL_SYSTEM_EVENT_LOOP_TIMEOUT_PERIOD,
               Cpl::System::SharedEventHandlerApi*  eventHandler     = 0)
        : MailboxServer( timingTickInMsec, eventHandler )
        , m_sema( signalToNotify )
    {
    }

public:
    /// See Cpl::System::Runnable
    void appRun()
    {
        m_sema.signal();
        Cpl::Dm::MailboxServer::appRun();
    }

    /// See Cpl::System::EventFlag
    void processEventFlag( uint8_t eventNumber ) noexcept
    {
        if ( eventNumber == 31 )
        {
            m_semaPauseTread.wait();
            m_sema.signal();
        }
    }

public:
    /// This method freezes/Pauses the Mailbox
    void freeze()
    {
        notify( 31 );
    }

    /// This method resumes/unfreezes the Mailbox.  The application is signaled when the Mailbox has resumed
    void thaw()
    {
        // Signal the paused thread to wake up and then wait for it to actually wake up
        m_semaPauseTread.signal();
        m_sema.wait();
    }

public:
    /// Semaphore to signal when I run
    Cpl::System::Semaphore& m_sema;

    /// Semaphore used to pause the Mailbox
    Cpl::System::Semaphore  m_semaPauseTread;
};

/** Same as the a DmMailbox class, except for ITC Mailbox server
 */
class ItcMailbox : public Cpl::Itc::MailboxServer
{
public:
    /// Constructor
    ItcMailbox( Cpl::System::Semaphore&              signalToNotify,
                unsigned long                        timingTickInMsec = OPTION_CPL_SYSTEM_EVENT_LOOP_TIMEOUT_PERIOD,
                Cpl::System::SharedEventHandlerApi*  eventHandler     = 0 )
        : MailboxServer( timingTickInMsec, eventHandler )
        , m_sema( signalToNotify )
    {
    }

public:
    /// See Cpl::System::Runnable
    void appRun()
    {
        m_sema.signal();
        Cpl::Itc::MailboxServer::appRun();
    }

public:
    /// Semaphore to signal when I run
    Cpl::System::Semaphore& m_sema;
};
/** This class is used to 'convert' the synchronous ITC semantics of open/close
    requests to asynchronous.  The initial use case is when using Simulate
    time and the thread calling the open/close is also the thread that is
    responsible for advancing simulated time.  There use case is essentially
    a dead-lock scenario. By changing the open/close semantics to asynchronous
    the dead-lock is avoid (albeit polling and an extra thread is required).
 */
class AsyncOpenClose : public Cpl::System::Runnable
{
public:
    /// Synchronize open/close calls
    Cpl::System::Semaphore m_sema;
    
    /// Synchronize the thread is ready (for an open call)
    Cpl::System::Semaphore m_semaReady;
    
    /// Critical section for flags
    Cpl::System::Mutex     m_lock;
    
    /// The instance to be opened/closed
    Cpl::Itc::CloseSync&   m_target;
    
    /// Optional open argument
    void*                  m_openArgs;

    /// Open/close state
    volatile bool          m_opened;
    
    /// Option to exit the runnable object/thread when closeSubject() is called
    volatile bool          m_exitOnClose;

public:
    /// Constructor
    AsyncOpenClose( Cpl::Itc::CloseSync& target )
        : m_target( target )
        , m_openArgs( nullptr )
        , m_opened( false )
        , m_exitOnClose( true )
    {
    }

public:
    /// Used to invoke the open() request using asynchronous semantics (with respect to the caller)
    void openSubject( void* args=nullptr )
    {
        m_semaReady.wait();
        m_openArgs = args;
        m_sema.signal();
    }
    
    /// Used to invoke the close() request using asynchronous semantics (with respect to the caller)
    void closeSubject( bool exitThreadOnClose = false )
    {
        m_exitOnClose = exitThreadOnClose;
        m_sema.signal();
    }

    /// Test executive calls this method to get the status of open/close class
    bool isOpened()
    {
        Cpl::System::Mutex::ScopeBlock criticalSection( m_lock );
        return m_opened;
    }

protected:
    /// See Cpl::System::Runnable
    void appRun()
    {
        for ( ;;)
        {
            m_semaReady.signal();
            m_sema.wait();
            m_target.open( m_openArgs );
            setState( true );
            m_sema.wait();
            m_target.close();
            setState( false );
            if ( m_exitOnClose )
            {
                break;
            }
        }
    }

    /// Helper method
    inline void setState( bool opened )
    {
        Cpl::System::Mutex::ScopeBlock criticalSection( m_lock );
        m_opened = opened;
    }
};


/*----------------------------------------------------------------------------*/
/** This function performs a blocking wait by monitoring an input stream for
    available data. The method waits a specified amount of time BEFORE
    checking for available data. The method returns true if there is data
    available; else the method will eventually time out and return false.
 */
bool minWaitOnStreamData( Cpl::Io::Input& stream, unsigned long minWait=10, unsigned long maxWaitMs=10000, unsigned long waitDelayMs=10 )
{
    Cpl::System::Api::sleep( minWait );

    bool avail = false;
    while ( maxWaitMs && !avail )
    {
        Cpl::System::Api::sleep( waitDelayMs );
        maxWaitMs -= waitDelayMs;
        avail = stream.available();
    }
    return avail;
};

#ifdef USE_CPL_SYSTEM_SIM_TICK
#include "Cpl/System/SimTick.h"

/** This is a convenience method that for the 'test executive' to advance simulated
    time AND poll the status of the asynchronous open call.
    
    Returns the number of ticks advanced
 */
static inline size_t simAdvanceTillOpened( AsyncOpenClose& openerCloser, size_t advanceStep=1 )
{
    size_t ticks = 0;
    while ( openerCloser.isOpened() == false )
    {
        ticks += advanceStep;
        Cpl::System::SimTick::advance( advanceStep );
        Cpl::System::Api::sleepInRealTime( 10 );
    }

    return ticks;
}

/** This is a convenience method that for the 'test executive' to advance simulated
    time AND poll the status of the asynchronous close call.

    Returns the number of ticks advanced
 */
static inline size_t simAdvanceTillClosed( AsyncOpenClose& openerCloser, size_t advanceStep=1 )
{
    size_t ticks = 0;
    while ( openerCloser.isOpened() == true )
    {
        ticks += advanceStep;
        Cpl::System::SimTick::advance( advanceStep );
        Cpl::System::Api::sleepInRealTime( 10 );
    }

    return ticks;
}
#endif

#endif  // end header latch
