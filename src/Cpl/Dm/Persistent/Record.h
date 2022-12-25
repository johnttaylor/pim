#ifndef Cpl_Dm_Persistent_Record_h_
#define Cpl_Dm_Persistent_Record_h_
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

#include "Cpl/Persistent/Record.h"
#include "Cpl/Persistent/RecordServer.h"
#include "Cpl/Persistent/Chunk.h"
#include "Cpl/Persistent/Payload.h"
#include "Cpl/Dm/ModelPoint.h"
#include "Cpl/Dm/SubscriberComposer.h"
#include "Cpl/Dm/Persistent/FlushRequest.h"
#include "Cpl/Dm/Persistent/EraseRequest.h"
#include "Cpl/System/Timer.h"

///
namespace Cpl {
///
namespace Dm {
///
namespace Persistent {

/** This mostly concrete class implements the Cpl::Persistent::Record interface
    where a Record instance contains the data from N model points.  A final
    child class is needed to provide the specifics of 'resetting' the Record's
    data.
 */
class Record :
    public Cpl::Persistent::Record,
    public Cpl::Persistent::Payload,
    public FlushRequest,
    public EraseRequest,
    public Cpl::System::Timer
{
public:
    /** This data structure associates a Data Model subscriber instance with a
        Model Point instance.
     */
    typedef struct
    {
        Cpl::Dm::ModelPoint*                                        mpPtr;          //!< Pointer to a Model Point 
        Cpl::Dm::SubscriberComposer<Record, Cpl::Dm::ModelPoint>*   observerPtr;    //!< Place holder to for dynamically allocated Subscriber Instance.  Note: A ZERO value will not allocate a subscriber
    } Item_T;

public:
    /** Constructor. The 'itemList' is variable length array where the last item
        in the list MUST be a 'null' entry, e.g. {0,0}.  The application is
        responsible for ALLOCATING all of the Model Point and Subscriber
        instances in the itemList.

        If the schema indexes does not match when reading data from persistent
        storage, THEN the upgrade method is called.record data is defaulted.  If the schema minor index from
        persistent storage does is less than 'schemaMinorIndex' argument, the
        the record is data loaded and the upgrade() method is called. If the
        schema minor index from persistent storage is greater than the
        'schemaMinorIndex' argument, THEN the record data is defaulted.

        The 'writeDelayMs' and 'maxWriteDelayMs' arguments are used to impose
        a 'settling time' after an model point is updated before writing the
        change(s) to NVRAM.  This is useful when multiple MP changes at the
        'same' time - so that there is single update to NVRAM instead of 
        attempting multiple NVRAM writes for each MP change notification.
     */
    Record( Item_T                  itemList[],
            Cpl::Persistent::Chunk& chunkHandler,
            uint8_t                 schemaMajorIndex,
            uint8_t                 schemaMinorIndex,
            uint32_t                writeDelayMs    = 0,
            uint32_t                maxWriteDelayMs = 0 ) noexcept;

    /// Destructor
    ~Record();


public:
    /// See Cpl::Persistent::Record
    void start( Cpl::Dm::MailboxServer& myMbox ) noexcept;

    /// See Cpl::Persistent::Record
    void stop() noexcept;

    /// See Cpl::Persistent::Payload
    size_t getData( void* dst, size_t maxDstLen ) noexcept;

    /// See Cpl::Persistent::Payload
    bool putData( const void* src, size_t srcLen ) noexcept;

public:
    /// Returns the size of record.  Does NOT include any meta-data that the Chunk Handler adds
    size_t getRecordSize() noexcept;

public:
    /** Synchronous Flush/update of the Record to persistent storage.
        Note: Use this method with CAUTION.  The call will BLOCK the calling
        thread for an undetermined amount of time!!
     */
    bool flush( Cpl::Persistent::RecordServer& myRecordsServer ) noexcept;

    /** Synchronous Invalidate/logically-erase of the Record in persistent storage.
        Note: Use this method with CAUTION.  The call will BLOCK the calling
        thread for an undetermined amount of time!!
     */
    bool erase( Cpl::Persistent::RecordServer& myRecordsServer ) noexcept;

public:
    /// See Cpl::Dm::Persistent::FlushRequest
    void request( FlushMsg& msg );

    /// See Cpl::Dm::Persistent::EraseRequest
    void request( EraseMsg& msg );

protected:
    /** This method is responsible for updating all of the Model Points in
        record to their default values.  This method is called when there is
        NO valid data when reading the record's data from persistence storage.

        When the method returns true, the 'reset data' is written to persistence
        storage; else the persistence storage is NOT updated and NO further updates
        of the persistence storage will be allowed.
     */
    virtual bool resetData() noexcept = 0;

    /** This method is called when the stored record data has different
        schema indexes. It is up to the child record class on what to do when
        the schema is different.  The default behavior is default all of the
        data.

        The 'src' argument contains the record data (as stored in persistent
        storage).  The child class can make use of the data as needed.  Note:
        the data in 'src' is AFTER the schema indexes fields.

        The method should return true if the child class has successfully
        processed in the incoming data.  Returning false will cause the
        record data to be defaulted;
     */
    virtual bool schemaChange( uint8_t      previousSchemaMajorIndex,
                               uint8_t      previousSchemaMinorIndex,
                               const void*  src,
                               size_t       srcLen ) noexcept {
        return false;
    }

    /** This method is called when the Record has been successfully loaded into
        RAM/Model-Points.  The default action does nothing.
     */
    virtual void hookProcessPostRecordLoaded() noexcept {};

    /// Settling timer expired callback
    void expired( void ) noexcept;


protected:
    /// Callback method for Model Point change notifications
    virtual void dataChanged( Cpl::Dm::ModelPoint& point, Cpl::Dm::SubscriberApi& observer ) noexcept;

    /// Helper method that is used to initiate the update to the NVRAM
    virtual void updateNVRAM() noexcept;

protected:
    /// List of model points
    Item_T*                     m_items;

    /// Chunk handler for the Record
    Cpl::Persistent::Chunk&    m_chunkHandler;

    /// Delay time, in milliseconds, when updated NVRAM (i.e. 'settling time' after an MP update before writing NVRAM)
    uint32_t                    m_delayMs;

    /// Maximum amount of time, in milliseconds, to delay before updating NVRAM
    uint32_t                    m_maxDelayMs;

    /// Timer marker of the 'first' MP change notification (for a NVRAM update)
    uint32_t                    m_timerMarker;

    /// Schema Major version
    uint8_t                     m_major;

    /// Schema Minor version
    uint8_t                     m_minor;

    /// Remember my started state
    bool                        m_started;
};

/// Magic value to indicate that Subscriber instance should be allocated for Model Point item
#define CPL_DM_PERISTENCE_RECORD_USE_SUBSCRIBER     ((Cpl::Dm::SubscriberComposer<Record,Cpl::Dm::ModelPoint>*) 1 )

/// Magic value to indicate that Subscriber instance should NOT be allocated for Model Point item
#define CPL_DM_PERISTENCE_RECORD_NO_SUBSCRIBER      ((Cpl::Dm::SubscriberComposer<Record,Cpl::Dm::ModelPoint>*) 0 )



};      // end namespaces
};
};
#endif  // end header latch