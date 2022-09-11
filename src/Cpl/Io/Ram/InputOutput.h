#ifndef Cpl_Io_Ram_InputOutput_h_
#define Cpl_Io_Ram_InputOutput_h_
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

#include "Cpl/Io/InputOutput.h"
#include "Cpl/Container/RingBuffer.h"
#include "Cpl/System/Semaphore.h"
#include "Cpl/System/Mutex.h"


///
namespace Cpl {
///
namespace Io {
///
namespace Ram {


/** This concrete class implements an InputOutput stream using a Ring Buffer.

    NOTES:
    - Thread safe and read and write operations have blocking semantics.
    - This is NOT shared memory across processes, i.e. assumes a SINGLE
      address space.
    - Implementation is not necessarily efficient - it is more focused on
      clarity and robustness.
 */
class InputOutput : public Cpl::Io::InputOutput
{
public:
    /// Constructor.  The application supplies the Memory
    InputOutput( void* rawMemory, unsigned memorySizeInBytes );

    /// Destructor
    ~InputOutput();

public:
    /** This function will clear/reset-to-empty the internal ring buffer IF
        the instance is still opened.
     */
    virtual void reset();

public:
    /// Pull in overloaded methods from base class
    using Cpl::Io::InputOutput::read;

    /// See Cpl::Io::Input
    bool read( void* buffer, int numBytes, int& bytesRead );

    /// See Cpl::Io::Input
    bool available();


public:
    /// Pull in overloaded methods from base class
    using Cpl::Io::InputOutput::write;

    /// See Cpl::Io::Output
    bool write( const void* buffer, int maxBytes, int& bytesWritten );

    /// See Cpl::Io::Output
    void flush();

    /// See Cpl::Io::IsEos
    bool isEos();

    /// See Cpl::Io::Close. Note: Once closed() has been called, all of the write() method will return false;
    void close();


protected:
    /// Ring buffer
    Cpl::Container::RingBuffer<uint8_t> m_ringBuffer;

    /// Mutex to provide thread safety
    Cpl::System::Mutex     m_lock;

    /// Semaphore to provide blocking semantics
    Cpl::System::Semaphore m_semaWriters;

    /// Semaphore to provide blocking semantics
    Cpl::System::Semaphore m_semaReaders;

    /// Track my Write waiters
    unsigned m_writersWaiting;

    /// Track my Read waiters
    unsigned m_readersWaiting;

    /// Track my opened/closed state
    bool m_opened;

};

};      // end namespaces
};
};
#endif  // end header latch
