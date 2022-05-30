#ifndef Cpl_Persistent_Data_Record_h_
#define Cpl_Persistent_Data_Record_h_
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

#include "Cpl/Persistent/Record.h"
#include "Cpl/Persistent/Chunk.h"
#include "Cpl/Persistent/Payload.h"

///
namespace Cpl {
///
namespace Persistent {

/** This mostly concrete class implements the Cpl::Persistent::DataRecord interface
    where a DataRecord instance contains an unmanaged data store.  A final
    child class is needed to provide the specifics of the 'data store' (i.e.
    implement the Cpl::Persistent::Payload interface).

    The data is only read/written from Persistent storage 'on demand' from
    the application.

    NOTE: This interface/class is NOT THREAD SAFE and should only be 'used' from
          the Record Server's thread.
 */
class DataRecord : public Cpl::Persistent::Record, public Cpl::Persistent::Payload
{
public:
    /// Constructor.
    DataRecord( Cpl::Persistent::Chunk& chunkHandler ) noexcept;

    /// Destructor
    ~DataRecord();

public:
    /** This method is used by the application to write the DataRecord's data
        to persistent storage. Note: This method calls the DataRecord's getData()
        method for the data to write to persistent storage.

        The 'index' offset can be used to 'index into' the RegionMedia instead
        of starting at offset zero.  This argument should only be used when
        multiple instances of a Record is being stored in a single region.

        The method returns true on success; else false if an error occurred.

        NOTE: This method is NOT THREAD SAFE and should only be called from the
              Record Server's thread.
     */
    virtual bool writeToMedia( size_t index = 0 ) noexcept;

    /** This method is used by the application to read the DataRecord's data
        from persistent storage. Note: This method calls the DataRecord's putData()
        method with the data read from persistent storage.

        The 'index' offset can be used to 'index into' the RegionMedia instead
        of starting at offset zero.  This argument should only be used when
        multiple instances of a Record is being stored in a single region.

        The method returns true on success; else false if an error occurred

        NOTE: This method is NOT THREAD SAFE and should only be called from the
              Record Server's thread.
     */
    virtual bool readFromMedia( size_t index = 0 ) noexcept;

public:
    /// See Cpl::Persistent::DataRecord
    void start( Cpl::Dm::MailboxServer& myMbox ) noexcept;

    /// See Cpl::Persistent::DataRecord
    void stop() noexcept;


protected:
    /** This method is responsible for updating the 'data store' in RAM to
        default values.  This method is called when there is NO valid data when
        reading the record's data from persistence storage.

        The method returns true if the 'data store' is in an acceptable state
        and the end of the call; else if there is error or there is no acceptable
        state of the 'data store' then false is returned
     */
    virtual bool processNoValidData() noexcept = 0;


protected:
    /// Chunk handler for the DataRecord
    Cpl::Persistent::Chunk&    m_chunkHandler;

    /// Remember my started state
    bool                       m_started;
};



};      // end namespaces
};
#endif  // end header latch