#ifndef Cpl_Io_Tcp_InputOutput_h_
#define Cpl_Io_Tcp_InputOutput_h_
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
#include "Cpl/Io/Descriptor.h"

///
namespace Cpl {
///
namespace Io {
///
namespace Tcp {


/** This concrete class defines a platform independent implementation of an 
    InputOutput stream used by the Listener and Connector interfaces

    Note: Platform independent is this context is that application can 
          instantiate an instance of this class without any platform specific
          `#include` statements.  However, the actual implementation IS platform
          specific.
 */
class InputOutput : public Cpl::Io::InputOutput
{
protected:
    /// Protocol Control Block
    Cpl::Io::Descriptor  m_fd;

    /// End-of-Stream status
    bool                 m_eos;

public:
    /** Constructor. No 'fd' provided -->the instance MUST be activated prior
        to use.
     */
    InputOutput();


    /** Constructor. 'fd' is a PCB of a existing/opened TCP connection
     */
    InputOutput( Cpl::Io::Descriptor fd );


    /// Destructor
    ~InputOutput( void );


public:
    /** Activates the stream, i.e. initializes the instance's underlying
        PCB. If the instance's 'fd' is not in the closed state when
        this method is called a fatal error is generated.
     */
    void activate( Cpl::Io::Descriptor fd );


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

    /// See Cpl::Io::Output
    void close();

public:
    /** USE THIS METHOD WITH CAUTION. This method returns the Stream's
        internal 'file descriptor'
     */
    inline Cpl::Io::Descriptor getDescriptor() { return m_fd; }
};

};      // end namespaces
};
};
#endif  // end header latch
