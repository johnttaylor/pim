#ifndef Cpl_Io_File_Input_h_
#define Cpl_Io_File_Input_h_
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

#include "Cpl/Io/File/InputApi.h"
#include "Cpl/Io/Stdio/Input_.h"


///
namespace Cpl {
///
namespace Io {
///
namespace File {


/** This concrete class provides a platform independent 'standard'
    implementation of a Cpl::Io::File::Input object.

    NOTE: All the read operations return 'false' if an error occurred, this
          INCLUDES the end-of-file condition (which is error when dealing
          with streams). To differentiate between a true error and EOF, the
          client must call isEof().
 */
class Input : public InputApi
{
protected:
    /// Provides core read functionality
    Cpl::Io::Stdio::Input_  m_stream;


public:
    /// Constructor -->Opens the file
    Input( const char* fileName );

    /// Constructor. 'streamfd' is a the file descriptor of a existing/opened file.
    Input( Cpl::Io::Descriptor streamfd );

    /// Destructor -->Will insure the file gets closed
    ~Input();


public:
    /** This method returns true if the file was successfully open and/or
        is still opened (i.e. close() has not been called). Note: it is okay
        to call other methods in the class if the file is not open - i.e.
        nothing 'bad' will happen and the method will return 'failed'
        status (when appropriate).
     */
    bool isOpened();


public:
    /// See Cpl::Io::Input
    bool read( char& c );

    /// See Cpl::Io::Input
    bool read( Cpl::Text::String& destString );

    /// See Cpl::Io::Input
    bool read( void* buffer, int numBytes, int& bytesRead );

    /// See Cpl::Io::Input
    bool available();

	/// See Cpl::Io::IsEos (is equivalent to isEof())
	bool isEos();
	
	/// See Cpl::Io::Close
    void close();


public:
    /// See Cpl::Io::File::ObjectApi (is quantile to isEos())
    bool isEof();

    /// See Cpl::Io::File::ObjectApi
    bool length( unsigned long& len);

    /// See Cpl::Io::File::ObjectApi
    bool currentPos( unsigned long& curPos );

    /// See Cpl::Io::File::ObjectApi
    bool setRelativePos( long deltaOffset );

    /// See Cpl::Io::File::ObjectApi
    bool setAbsolutePos( unsigned long newoffset );

    /// See Cpl::Io::File::ObjectApi
    bool setToEof();
};

};      // end namespaces
};
};
#endif  // end header latch

