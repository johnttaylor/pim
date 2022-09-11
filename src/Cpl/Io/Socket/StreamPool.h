#ifndef Cpl_Io_Socket_StreamPool_h_
#define Cpl_Io_Socket_StreamPool_h_
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

#include "Cpl/Io/Socket/Factory.h"
#include "Cpl/Io/Socket/InputOutput.h"
#include "Cpl/Memory/SPool.h"
#include <new>



///
namespace Cpl {
///
namespace Io {
///
namespace Socket {


/** This concrete template class implement a socket factory that is
	capable of creating N concurrently opened socket streams.
 */
template <int N>
class StreamPool : public Cpl::Io::Socket::Factory,
	public Cpl::Memory::SPool<Cpl::Io::Socket::InputOutput, N>
{
public:
	/// Constructor.  When 'fatalErrors' is true, out-of-memory conditions will be FatalErrors.
	StreamPool( bool fatalErrors = true );


public:
	/// See Cpl::Io::Socket::Factory
	Cpl::Io::Socket::InputOutput* create();

	/// See Cpl::Io::Socket::Factory
	void destory( Cpl::Io::Socket::InputOutput& socketStream );

};


/////////////////////////////////////////////////////////////////////////////
//                  INLINE IMPLEMENTAION
/////////////////////////////////////////////////////////////////////////////
template <int N>
StreamPool<N>::StreamPool( bool fatalErrors )
	:Cpl::Memory::SPool<Cpl::Io::Socket::InputOutput, N>( fatalErrors )
{
}


template <int N>
Cpl::Io::Socket::InputOutput* StreamPool<N>::create()
{
	void* memPtr = allocate( sizeof( Cpl::Io::Socket::InputOutput ) );
	if ( ptr )
	{
		return new( memPtr ) Cpl::Io::Socket::InputOutput();
	}

	return 0;
}

template <int N>
void StreamPool<N>::destory( Cpl::Io::Socket::InputOutput& socketStream )
{
	// Explicitly call the destructor
	socketStream.~InputOutput();

	// Free the memory back to the pool.  
	release( &socketStream );
}



};      // end namespaces
};
};
#endif  // end header latch
