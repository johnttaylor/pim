#ifndef Cpl_Io_TeeOutput_h_
#define Cpl_Io_TeeOutput_h_
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2020  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file */

#include "Cpl/Io/Output.h"
#include "Cpl/Container/SList.h"


///
namespace Cpl {
///
namespace Io {



/** This concrete class implements a Output stream that outputs
    the data to MANY (or none) output streams. The write() methods will
    return false when at least of the Output stream(s) had an error.  There
    are methods provided (firstFailed(), nextFailed()) which returns a list of
    all currently failed Output streams.  Once a stream has been marked as failed
    it will not be added back to the list of "good/active" streams.  The user
    must explicitly delete the stream and then re-add it. NOTE: There is no
    guaranty of order or 'atomicity' with respect to other streams when
    writing to multiple streams.

    NOTE: The implementation is NOT thread safe.  The application must provide
          its own locking/critical section logic if calling a TeeOutput instance
          from multiple threads.
 */
class TeeOutput : public Output
{
private:
    /// List of active writers
    Cpl::Container::SList<Output> m_streams;

    /// List of failed writers
    Cpl::Container::SList<Output> m_failed;

    /// my open/close state
    bool m_opened;


public:
    /** Constructor. Starts off with NO output streams
     */
    TeeOutput();

    /** Constructor. Starts off with ONE output stream
     */
    TeeOutput( Output& streamA );

    /** Constructor. Starts off with TWO output streams
     */
    TeeOutput( Output& streamA, Output& streamB );


public:
    /** Adds a stream */
    void add( Output& stream );

    /** Removes a stream.  Returns true if the stream was actually
        removed (i.e. that it was in the list to start with).  NOTE: If
        the stream exists it will be deleted regardless if it is consider
        good or failed.
     */
    bool remove( Output& stream );


public:
    /** Returns the first failed stream.  If no failed streams exist 0 will
        be returned.
      */
    Output* firstFailed();

    /** Returns the next failed stream in the list.  If there are no more
        failed streams 0 will be returned.
     */
    Output* nextFailed( Output& currentFailedStream );

    /** Removes the specified failed stream AND returns the next failed stream
        in the list.  If there are no more failed streams (or the specified stream
        is not 'failed') 0 will be returned.  This method allows the user to remove
        the failed streams as he/she walks the failed list. NOTE: Do NOT call remove()
        while walking the failed list, remove() can invalidate the link pointers of the
        current failed stream object!!!
     */
    Output* removeAndGetNextFailed( Output& currentFailedStream );


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

};      // end namespaces
};
#endif  // end header latch
