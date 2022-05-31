#ifndef Cpl_Dm_Persistent_IndexedEntryReader_h_
#define Cpl_Dm_Persistent_IndexedEntryReader_h_
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

#include "Cpl/Persistent/Payload.h"
#include <stdint.h>

///
namespace Cpl {
///
namespace Persistent {

/** This abstract class defines interface for reading/retrieve 'entries' from a
    a collection of entries (i.e. read an entry from a IndexedEntryRecord).

    From a logical perspective, 'entries' are stored in a Ring Buffer.  The
    Ring Buffer can be 'traversed' from the either end (i.e. oldest to newest, or
    newest to oldest).

    Each entry that is stored has an 'index' value associated with it.  This
    'index' is used to uniquely identify each entry (even across overwritten
    entries) and it is used to identify the relative age between the entries.

    NOTE: ALL entries (for a given IndexEntryRecord) must be the same fixed
          length.

    NOTE: This interface/class is NOT THREAD SAFE and should only be 'used' from
          the Record Server's thread.
 */
class IndexedEntryReader
{
public:
    /** This structure define an 'marker' that identifies an entry's location
        in persistent media.
     */
    typedef struct
    {
        uint64_t indexValue;        //!< The index/timestamp value for the entry (Note: This is NOT the 'Buffer Index' used by the getByBufferIndex() call)
        size_t   mediaOffset;       //!< Offset, within a RegionMedia to the start of the Entry
    } EntryMarker_T;

public:
    /** This method reads/retrieves the latest entry (from the list of Indexed
        Entries) stored in the persistent media.  The method is synchronous in
        that the method does not return until the entry has been 'read' from
        the persistent media.
 
        Returns true if the latest entry is valid and has been read into 'dst';
        else false is returned.

        NOTE: When getLatest() returns false, this means there are NO entries
              stored.

        NOTE: 'dst' is ALWAYS updated EVEN if no entry was 'found', basically 'dst'
              is used as a work buffer when traversing the list.
     */
    virtual bool getLatest( Payload& dst, EntryMarker_T& entryMarker ) noexcept = 0;

public:
    /** This method walks the entire 'list of entries' and returns the next
        newer entry as specified by the 'newerThan' value.  The traversal
        starts with the 'beginHereMarker' The method is synchronous in that the
        method does not return until the entry has been 'read' from the persistent 
        media.

        Returns true if the 'next' entry was found and has been read into 'dst';
        else false is returned.

        NOTE: 'dst' is ALWAYS updated EVEN if no entry was 'found', basically 'dst'
              is used as a work buffer when traversing the list.

     */
    virtual bool getNext( uint64_t             newerThan,
                          const EntryMarker_T  beginHereMarker,
                          Payload&             dst,
                          EntryMarker_T&       entryMarker) noexcept = 0;

    /** This method is similar to getNext(), except that it returns the next
        oldest entry.
     */
    virtual bool getPrevious( uint64_t             olderThan,
                              const EntryMarker_T  beginHereMarker,
                              Payload&             dst,
                              EntryMarker_T&       entryMarker ) noexcept = 0;

public:
    /** This method can be used to read an entry by its 'buffer index'.  The
        buffer index is a zero based index. 

        Returns true if the entry at 'bufferIndex' is a valid entry; else false 
        is returned. Note: if 'bufferIndex' is out of range, false is returned.

        NOTE: 'dst' is ALWAYS updated EVEN if no valid entry was 'found', 
              basically 'dst' is used as a work buffer when traversing the list.
     */
    virtual bool getByBufferIndex( size_t         bufferIndex,
                                   Payload&       dst,
                                   EntryMarker_T& entryMarker ) noexcept = 0;


    /** This method returns the maximum allowed 'bufferIndex' when calling 
        getByIndex().
     */
    virtual size_t getMaxIndex() const noexcept = 0;

public:
    /// Virtual destructor
    virtual ~IndexedEntryReader() {}
};



};      // end namespaces
};
#endif  // end header latch