#ifndef Cpl_Io_Stdio_Output_x_h_
#define Cpl_Io_Stdio_Output_x_h_
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


#include "Cpl/Io/Output.h"
#include "Cpl/Io/Descriptor.h"


// Forward declaration in other namespaces (needed for 'friends' declaration)
namespace Cpl {
namespace Io {
namespace File {
class Output;
class InputOutput;
};
};
};


///
namespace Cpl {
namespace Io {
namespace Stdio {


/** This concrete class implements a Output stream using the underlying
    platform's native OS 'file interface' for the C library's stdin,
    stdout, and stderr streams. This class is intended to be a helper
    class and/or implementation inheritance parent class than an class
    used directly by the Application.
 */
class Output_ : public Cpl::Io::Output
{
protected:
    /// Stream Handle
    Cpl::Io::Descriptor m_outFd;

    /// Cache end-of-stream status
    bool                m_outEos;


public:
    /** Constructor. 'fd' is a the 'file/stream descriptor' of a existing/opened
        stream.
     */
    Output_( int fd );

    /** Constructor. 'handle' is a the 'file/stream descriptor' of a existing/opened
        stream.
     */
    Output_( void* handle );

    /** Constructor. 'streamfd' is a the 'file/stream descriptor' of a existing/opened
        stream.
     */
    Output_( Cpl::Io::Descriptor streamfd );

    /** Constructor. No underlying file descriptor - the stream must be activated before using
     */
    Output_( void );


public:
    /// Destructor
    ~Output_( void );


public:
    /** Activates and/or resets the underlying 'fd' for the stream.  If the
        current 'fd' is not in the closed state - a fatal error is generated
     */
    void activate( int fd );

    /** Activates and/or resets the underlying 'handle' for the stream.  If the
        current 'fd' is not in the closed state - a fatal error is generated
     */
    void activate( void* handle );

    /** Activates and/or resets the underlying 'streamfd' for the stream.  If the
        current 'fd' is not in the closed state - a fatal error is generated
     */
    void activate( Cpl::Io::Descriptor streamfd );


public:
    /** This method returns true if the file was successfully open and/or
        is still opened (i.e. close() has not been called). Note: it is okay
        to call other methods in the class if the file is not open - i.e.
        nothing 'bad' will happen and the method will return 'failed'
        status (when appropriate).
     */
    bool isOpened();


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



public:
    // Allow the "Standard" Stream & File IO Classes have access to me
    friend class InputOutput_;
    friend class Cpl::Io::File::Output;
    friend class Cpl::Io::File::InputOutput;

};

};      // end namespaces
};
};
#endif  // end header latch
