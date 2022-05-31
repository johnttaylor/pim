#ifndef Cpl_Persistent_Payload_h_
#define Cpl_Persistent_Payload_h_
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


#include <stdlib.h>


///
namespace Cpl {
///
namespace Persistent {


/** This abstract class defines the interface accessing the 'data payload' of
    an individual Record instance.
 */
class Payload
{
public:
    /** This method returns a Record's data payload contents.  The Record
        is responsible for NOT over-running the 'dst' buffer.  The method
        returns the number of bytes copied to 'dst' on success; else zero is
        returned when there is failure.
        
        Note: On success, the returned number of bytes MUST be the same on every
              call of this method, i.e. variable length records at run time is
              NOT supported.

        Note: If the Record returns less data than 'maxDstLen' - the remaining
              bytes will be zero filled before the record data is written to
              the media.
     */
    virtual size_t getData( void* dst, size_t maxDstLen ) noexcept = 0;

    /** This method used to transfer persistently stored data into a Record.  
        The method returns true if successful; else false is returned.

        Note: The application is not required to consume all of the incoming
              data.  This is because typically region are 'over-allocated'
              in order to leave room to grow.
     */
    virtual bool putData( const void* src, size_t srcLen ) noexcept = 0;


public:
    /// Virtual destructor
    virtual ~Payload() {}
};


};      // end namespaces
};
#endif  // end header latch
