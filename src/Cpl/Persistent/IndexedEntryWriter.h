#ifndef Cpl_Dm_Persistent_IndexedEntryWriter_h_
#define Cpl_Dm_Persistent_IndexedEntryWriter_h_
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

#include "Cpl/Persistent/Payload.h"
#include <stdint.h>

///
namespace Cpl {
///
namespace Persistent {

/** This abstract class defines interface for appending a single 'entry' to a 
    a collection of entries (i.e. write a entry to a IndexedEntryRecord).    
    
    From a logical perspective, 'entries' are stored in a Ring Buffer 
    and when the buffer is full, the oldest entries are overwritten.   

    Each entry that is stored has an 'index' value associated with it.  This 
    'index' is used to uniquely identify each entry (even across overwritten 
    entries) and it is used to identify the relative age between the entries.

    NOTE: The max length entries (for a given IndexEntryRecord) IS a single
          fixed length. An 'unused' bytes/space are padded with zeros.

    NOTE: This interface/class is NOT THREAD SAFE and should only be 'used' from
          the Record Server's thread.
 */
class IndexedEntryWriter
{
public:
    /** This method appends entry to the list of Indexed Entries.  The method
        is synchronous in that the method does not return until the entry has 
        been 'written' the persistent media.

        Returns true on success; else if an error occurred (e.g. IO error while
        writing) false is returned.
     */
    virtual bool addEntry( const Payload& src ) noexcept = 0;

public:
    /** This method will 'clear' (i.e. invalidate) ALL entries in persistent
        storage.

        CAUTION: With great power comes, comes great responsibility!
       
        Returns true when successful; else false is returned.  When false is 
        returned that state of 'entries' in persistent storage is undetermined.
     */
    virtual bool clearAllEntries() noexcept = 0;

public:
    /// Virtual destructor
    virtual ~IndexedEntryWriter() {}
};



};      // end namespaces
};
#endif  // end header latch