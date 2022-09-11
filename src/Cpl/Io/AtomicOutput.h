#ifndef Cpl_Io_AtomicOutput_h_
#define Cpl_Io_AtomicOutput_h_
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
/** @file */

#include "Cpl/Io/AtomicOutputApi.h"
#include "Cpl/System/Mutex.h"


///
namespace Cpl {
///
namespace Io {


/** This concrete template class implements the AtomicOutputApi using a mutex
    to enforce the 'atomic-ness' of the operation, i.e. all output operations
    occur synchronously in the caller's thread.

    The template arg - CONTEXT - is the class that implements the callback
    when requestOutputs() is called.
 */
template <class CONTEXT>
class AtomicOutput : public AtomicOutputApi<CONTEXT>
{
protected:
    /// Underlying output stream 
    Output & m_stream;

    /// Lock to provide the 'atomic' functionality
    Cpl::System::Mutex& m_lock;


public:
    /** The application is responsible for supplying the actual Output stream.
        In addition, the application must supply a 'lock'.  This allows the
        application to associate each Output stream with it own lock or the
        application could chose to use a single lock for all Atomic
        Output streams.
     */
    AtomicOutput( Output& stream, Cpl::System::Mutex& lock );


public:
    /// See AtomicOutputApi
    bool requestOutputs( CONTEXT& client, typename AtomicOutputApi<CONTEXT>::OutputsFunction_T clientsMethod );


public:
    /// Pull in overloaded methods from base class
    using Cpl::Io::Output::write;

    /// See Cpl::Io::Output
    bool write( const void* buffer, int maxBytes, int& bytesWritten );

    /// See Cpl::Io::Output
    void flush();

	/// See Cpl::Io::IsEos
	bool isEos();
	
	/// See Cpl::Io::Output
    void close();

};



/////////////////////////////////////////////////////////////////////////////
//                  INLINE IMPLEMENTAION
/////////////////////////////////////////////////////////////////////////////
template <class CONTEXT>
AtomicOutput<CONTEXT>::AtomicOutput( Output& stream, Cpl::System::Mutex& lock )
    :m_stream( stream )
    , m_lock( lock )
{
}

/////////////////
template <class CONTEXT>
bool AtomicOutput<CONTEXT>::requestOutputs( CONTEXT& client, typename AtomicOutputApi<CONTEXT>::OutputsFunction_T clientsMethod )
{
    Cpl::System::Mutex::ScopeBlock criticalSection( m_lock );
    bool io = (client.*clientsMethod)(m_stream);
    return io;
}

template <class CONTEXT>
bool AtomicOutput<CONTEXT>::write( const void* buffer, int maxBytes, int& bytesWritten )
{
    Cpl::System::Mutex::ScopeBlock criticalSection( m_lock );
    bool io = m_stream.write( buffer, maxBytes, bytesWritten );
    return io;
}

template <class CONTEXT>
void AtomicOutput<CONTEXT>::flush()
{
    Cpl::System::Mutex::ScopeBlock criticalSection( m_lock );
    m_stream.flush();
}

template <class CONTEXT>
bool AtomicOutput<CONTEXT>::isEos()
{
	Cpl::System::Mutex::ScopeBlock criticalSection( m_lock );
	return m_stream.isEos();
}

template <class CONTEXT>
void AtomicOutput<CONTEXT>::close()
{
    Cpl::System::Mutex::ScopeBlock criticalSection( m_lock );
    m_stream.close();
}

};      // end namespaces
};
#endif  // end header latch
