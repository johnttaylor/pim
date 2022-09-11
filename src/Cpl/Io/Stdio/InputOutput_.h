#ifndef Cpl_Io_StdIo_InputOutput_x_h_
#define Cpl_Io_StdIo_InputOutput_x_h_
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
#include "Cpl/Io/Stdio/Input_.h"
#include "Cpl/Io/Stdio/Output_.h"


// Forward declaration in other namespaces (needed for 'friends' declaration)
namespace Cpl {
namespace Io {
namespace File {
class InputOutput;
};
};
};

// Forward declaration in other namespaces (needed for 'friends' declaration)
namespace Cpl {
namespace Io {
namespace Serial {
class Port;
};
};
};


///
namespace Cpl {
///
namespace Io {
///
namespace Stdio {

/** This concrete class implements a Input-Output stream.  The actual
    implementation is dependent on the linked-in implementation (which
    is platform/behavior specific).
 */
class InputOutput_ : public Cpl::Io::InputOutput
{
protected:
    /// Input Stream
    Input_  m_in;

    /// Output Stream
    Output_ m_out;

    /// Keep track if I have two or a single stream
    bool    m_single;


public:
    /** Constructor. 'fd' is a the 'file/stream descriptor' of a existing/opened
        stream.
     */
    InputOutput_( int fd );

    /** Constructor. 'handle' is a the 'file/stream descriptor' of a existing/opened
        stream.
     */
    InputOutput_( void* handle );

    /** Constructor. 'streamfd' is a the 'file/stream descriptor' of a existing/opened
        stream.
     */
    InputOutput_( Cpl::Io::Descriptor streamfd );


public:
    /** Constructor - Split personality IO stream - allows the underlying input
        and output streams to reference different streams.
     */
    InputOutput_( int infd, int outfd );

    /** Constructor - Split personality IO stream - allows the underlying input
        and output streams to reference different streams.
     */
    InputOutput_( void* inhandle, void* outhandle );

    /** Constructor - Split personality IO stream - allows the underlying input
        and output streams to reference different streams.
     */
    InputOutput_( Cpl::Io::Descriptor instreamfd, Cpl::Io::Descriptor outstreamfd );


public:
    /** Constructor. No underlying file descriptors - the stream must be activated before using
     */
    InputOutput_( void );


public:
    /// Destructor
    ~InputOutput_( void );


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
    /** Activates and/or resets the underlying 'fd' for the stream.  If the
        current 'fd' is not in the closed state - a fatal error is generated
     */
    void activate( int infd, int outfd );

    /** Activates and/or resets the underlying 'handle' for the stream.  If the
        current 'fd' is not in the closed state - a fatal error is generated
     */
    void activate( void* inhandle, void* outhandle );

    /** Activates and/or resets the underlying 'streamfd' for the stream.  If the
        current 'fd' is not in the closed state - a fatal error is generated
     */
    void activate( Cpl::Io::Descriptor instreamfd, Cpl::Io::Descriptor outstreamfd );


public:
    /** This method returns true if the Input AND Output streams where
        successfully open and/or is still opened (i.e. close() has not been
        called). Note: it is okay to call other methods in the class if the file
        is not open - i.e. nothing 'bad' will happen and the method will return
        'failed' status (when appropriate).
     */
    bool isOpened();

    /// Same isOpened() - but only report the Input stream's open/closed status
    bool isInputOpened();

    /// Same isOpened() - but only report the Output stream's open/closed status
    bool isOutputOpened();



public:
    /// See Cpl::Io::Input
    bool read( char& c );

    /// See Cpl::Io::Input
    bool read( Cpl::Text::String& destString );

    /// See Cpl::Io::Input
    bool read( void* buffer, int numBytes, int& bytesRead );

    /// See Cpl::Io::Input
    bool available();


public:
    /// See Cpl::Io::Output
    bool write( char c );

    /// See Cpl::Io::Output
    bool write( const char* string );

    /// See Cpl::Io::Output
    bool write( const Cpl::Text::String& string );

    /// See Cpl::Io::Output
    bool write( Cpl::Text::String& formatBuffer, const char* format, ... );

    /// See Cpl::Io::Output
    bool vwrite( Cpl::Text::String& formatBuffer, const char* format, va_list ap );

    /// See Cpl::Io::Output
    bool write( const void* buffer, int numBytes );

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
    friend class Cpl::Io::File::InputOutput;
    friend class Cpl::Io::Serial::Port;
};

};      // end namespaces
};
};
#endif  // end header latch
