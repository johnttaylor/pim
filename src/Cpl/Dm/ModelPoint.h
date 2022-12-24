#ifndef Cpl_Dm_Model_Point_h_
#define Cpl_Dm_Model_Point_h_
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


#include "colony_config.h"
#include "Cpl/Container/Item.h"
#include "Cpl/Text/String.h"
#include "Cpl/Dm/SubscriberApi.h"
#include "Cpl/Json/Arduino.h"
#include <stddef.h>
#include <stdint.h>


 ///
namespace Cpl {
///
namespace Dm {


/** This mostly abstract class defines the interface for a Model Point.  A
    Model Point contains an instance of a Point's data and is responsible for
    managing thread safe access to the 'Point'.  The intent of a Model point is
    that it is the Application's canonical source for a 'Point'

    NOTES:
        1) The concrete leaf classes are responsible for ensuring type
           safety.
        2) All methods are atomic, i.e. a protected by the Model Base's mutex.
           This means that all methods will block if there is a call in-progress
           to ANY Model Point in a given Model Data Base.
        3) All methods in this class ARE thread Safe unless explicitly
           documented otherwise.
 */
class ModelPoint : public Cpl::Container::Item
{
public:
    /// Options related to the Model Point's locked state
    enum LockRequest_T
    {
        eNO_REQUEST,            //!< No change in the MP's lock state is requested
        eLOCK,                  //!< Request to lock the MP.  If the MP is already lock - the request is ignored and the update operation silent fails
        eUNLOCK,                //!< Request to unlock the MP.  If the MP is already unlocked - the request is ignored and the update operation is completed
    };


public:
    /** Magic value to use when registering for a change notification and
        application does not 'know' the current sequence number value of the
        Model Point.
     */
    static const uint16_t SEQUENCE_NUMBER_UNKNOWN;


public:
    /** This method returns the Model Point's name as a null terminated string.

        Note: Model Point names are assumed to be unique.  However, it is
              responsibility of the Application to enforce this constraint.  If
              names are not unique then the look-up by Model Point name
              functionality will fail.  No other functionality depends on have
              unique names.
     */
    virtual const char* getName() const noexcept = 0;

    /** This method returns the Model Point's current sequence number.
     */
    virtual uint16_t getSequenceNumber() const noexcept = 0;

    /** This method does NOT alter the MP's data or state, but unconditionally
        triggers the MP change notification(s). The method returns the Model
        Point's sequence number after the method completes.
     */
    virtual uint16_t touch() noexcept = 0;

    /** This method returns the RAM size, in bytes, of the Model Point's data.
      */
    virtual size_t getSize() const noexcept = 0;


public:
    /** This method returns true when the MP data is invalid.  The method
        optionally returns the Point's sequence number via the argument
        'seqNumPtr'.
     */
    virtual bool isNotValid( uint16_t* seqNumPtr=0 ) const noexcept = 0;

    /** This method is used to mark the element's data as invalid.

        The method returns the Model Point's sequence number after updating
        the valid state.

        Notes:
        1) Any write operation will set the Model Point's state to valid.
        2) Change Notification(s) will ONLY be triggered when there is valid to
           invalid state transition (i.e. if the Model Point is already in
           the invalid state - no change notification will be triggered).
        3) If the Model Point is locked then the invalidate operation...
           a) If lockRequest == eNO_REQUEST, the operation silently fails, i.e.
              nothing is done. OR
           b) If lockRequest != eNO_REQUEST, the operation is performed and the
              the new lock state is applied
     */
    virtual uint16_t setInvalid( LockRequest_T lockRequest = eNO_REQUEST ) noexcept = 0;

public:
    /** This method converts the Model Point's data to JSON string and
        copies the resultant string into 'dst'.  If the Model Point's data
        cannot be represented as a JSON object then the contents of 'dst' is
        set to an empty string and the method returns false; else the method
        returns true. The format of the string is specific to the concrete leaf class.
        However, it is strongly recommended that the output of this method be
        the same format that is expected for the fromJSON() method.

        NOTE: If the converted string is larger than the memory allocated by
              'dst' then the string result in 'dst' will be truncated. The
              caller is required to check 'truncated' flag for the truncated
              scenario.


        The general output format:
        \code

        { name:"<mpname>", type:"<mptypestring>", valid:true|false, seqnum:nnnn, locked:true|false, val:<value> }

        Notes:
            - The MP is in the valid state if/when the 'valid' value is true
            - The 'val' key/value pair is omitted if the MP is in the invalid state
            - The 'val' key/value pair can be a single element, an object, or
              array. etc. -- it is specific to the concrete MP type/class.


        \endcode
     */
    virtual bool toJSON( char*  dst,
                         size_t dstSize,
                         bool&  truncated,
                         bool   verbose = true,
                         bool   pretty = false ) noexcept = 0;


    /** This method returns a string identifier for the Model Point's data type.
        This value IS GUARANTEED to be unique (within an Application).  The
        format of the string is the Model Point's fully qualified namespace and
        class type as a string. For example, the for Cpl::Dm::Mp::Uint32 Model
        Point the function would return "Cpl::Dm::Mp::Uint32"

        Note: The type string can contain additional information, but adding
              a '-' character followed by the extra info.  For example:
              "Cpl::Dm::Mp::Uint32-hex"
     */
    virtual const char* getTypeAsText() const noexcept = 0;

public:
    /** This method returns true if the Model Point is in the locked state.
        In the locked state - ALL WRITE/UPDATE OPERATIONS (except for changing
        the locked state) are silently ignored/dropped.
     */
    virtual bool isLocked() const noexcept = 0;

    /** This updates the lock state of the Model Point. Model Points support
        the concept of a client 'locking' the MP's data value.  When a MP's
        data has been locked - any attempted writes/updated operation to the
        MP will SILENTLY fail.  The Application uses the 'eUNLOCK' lock request
        to remove the locked state from the MP's data.

        This method never triggers change notification(s).

        The method returns the MP's sequence number at the time of when then
        lock request was applied.
     */
    virtual uint16_t setLockState( LockRequest_T lockRequest ) noexcept = 0;

    /// Short hand for unconditionally removing the lock from the MP
    inline uint16_t removeLock() noexcept { return setLockState( eUNLOCK ); }

    /// Short hand for putting the MP into the locked state
    inline uint16_t applyLock() noexcept { return setLockState( eLOCK ); }


public:
    /** This method is used to export the Model Point's instance data content
        to a raw data stream.  It is the responsibility of the caller to ensure
        that there is sufficient memory available for the data being exported.
        The method returns the number of bytes exported.

        When the 'includeLockedState' argument is set to true, the method
        export's the MP's locked state (in addition to the MP's value and
        valid state).

        If the number of returns bytes equals zero then the export operation
        encountered an error.

        The method optionally return the Model Point's sequence number at the
        time of the export.
     */
    virtual size_t exportData( void*     dstDataStream,
                               size_t    maxDstLength,
                               uint16_t* retSequenceNumber=0,
                               bool      includeLockedState = false ) const noexcept = 0;

    /** This method is used to populate the Model Point's data content from the
        a raw data stream/pointer.  It is the responsibility of the caller to
        ensure that the data stream is appropriate for element type and that
        the data stream content was originally created by the corresponding
        export() method. The method returns the number of bytes consumed from
        the data stream.

        When the 'includeLockedState' argument is set to true, the method
        imports's the MP's locked state (in addition to the MP's value and
        valid state).

        If the number of returns bytes equals zero then the import operation
        encountered an error.

        The method optionally return the Model Point's sequence number once the
        import has completed.

        The method ALWAYS triggers a change notification(s) for the Model Point
     */
    virtual size_t importData( const void* srcDataStream,
                               size_t      srcLength,
                               uint16_t*   retSequenceNumber=0,
                               bool        includeLockedState = false ) noexcept = 0;

    /** Returns the size, in bytes, of the element's data content.

        When 'includeLockedState is set to true, the 'externalSize' includes
        space for the MP's locked state; else the returned size only includes
        space for the MP's value and valid state..

        NOTE: The size returned is the size of the Point data WHEN it
              is being exported/imported - this is NOT the value of
              the size of the Point's internal storage use for the
              data content.
     */
    virtual size_t getExternalSize( bool includeLockedState = false ) const = 0;


public:
    /** This method is used to subscribe to on-change notification in a GENERIC
        manner. This means the on-change callback notification WILL NOT be type
        specific.  USE WITH CAUTION - using type specific callbacks is the
        preferred usage.

        See the attach() method below for additional details of subscribing
     */
    virtual void genericAttach( SubscriberApi& observer,
                                uint16_t       initialSeqNumber=SEQUENCE_NUMBER_UNKNOWN ) noexcept = 0;

    /** This is method is used to detach a GENERIC Subscriber from a Model Point.

        See the detach() method below for additional details of subscribing
     */
    virtual void genericDetach( SubscriberApi& observer ) noexcept = 0;


protected:
    /** This method copies the Model Point's content to the caller's Point
        instance. The method returns the Model Point's valid state.  The
        MP's sequence number is optionally return if 'seqNumPtr' is set to
        a non-zero value.

        If the method returns false then the MP's the data is invalid and the 
        contents of 'dst' is meaningless.

        Notes:
        1) The assumption is that Model Point's internal data and 'dstData' are
           of the same type.
        2) The data size of the 'dstSize' is ALWAYS honored when coping the
           data from the Model Point
        3) The Model Point's sequence number is not changed.
     */
    virtual bool readData( void* dstData, size_t dstSize, uint16_t* seqNumPtr=0 ) const noexcept = 0;

    /** This method writes the caller Point instance to the Model Point's
        internal data.  The method returns the Model Point's sequence number
        after the method completes.

        Model Point supports the concept of a client 'locking' the MP's data
        value.  When a MP's data has been locked - all attempted writes to the
        MP - with lockRequest == eNO_REQUEST - will SILENTLY fail.  The
        Application uses the removeLock() method or the 'eUNLOCK' lock request
        to remove the locked state from the MP's data.  The application can
        also updated the value of MP when it is locked state by setting
        lockRequest == eLOCK.

        Notes:
        1) The assumption is that Model Point's internal data and 'srcData' are
           of the same type.
        2) The data size of the Model Points data instance is ALWAYS honored
           when coping the data from 'srcData'
     */
    virtual uint16_t writeData( const void*   srcData,
                            size_t        srcSize,
                            LockRequest_T lockRequest = eNO_REQUEST ) noexcept = 0;


protected:
    /** This method is used to attach a subscriber to a Model Point.  Once
        attached the Subscriber will receive a change notification (aka a
        callback) every time the Model Point's data/state changes. Once, a
        Subscriber is attached - it will stay attached to the application
        calls detach().

        There is no limit to the number of Subscribers that can attach to
        a Model Point.

        The attach() method can be called even if the Subscriber is already
        attached.  When this happens, the attach process is 'restarted', i.e.
        the 'initialSeqNumber' is used for the Subscriber's sequence number.

        The change-detect mechanism uses a sequence number.  Each Model Point
        and each Subscriber has sequence number.  When the Subscriber's sequence
        number does not equals the Model Point's sequence number - the Subscriber
        receives a change notification and the Subscriber's sequence number is
        updated to match the Model Point's sequence number at the time of
        change notification.  When a Subscriber attaches to Model Point with
        an the 'seqNumber' argument set to SEQUENCE_NUMBER_UNKNOWN, the
        Subscriber will get an 'immediate' change notification.

        The callbacks for the Change Notifications are called as part of the
        Data Model's Mailbox server.  As part of the asynchronous processing (timers,
        ITC, EventFlags, etc.) of the Data Model Mailbox server will also process
        all pending Change Notification and invoke the call backs.  What does
        that all mean?  The Change notifications are "local" to the Subscriber's
        thread very similar to how the Cpl::Timers work.  It also means that
        no change notification callback will be called till the Mailbox server
        loops back to the "top" of its forever loop.

        NOTE: The Change Notification mechanism does NOT guarantee that a
              client will receive a notification for EVERY change to a Model
              Point (i.e. 'fast edges' could/will be missed).  What is guaranteed
              is that once a Model Point's value has 'settle' all registered
              subscribers will have received change notification for the Model
              Point's current value.
     */
    virtual void attachSubscriber( SubscriberApi& observer,
                                   uint16_t       initialSeqNumber=SEQUENCE_NUMBER_UNKNOWN ) noexcept = 0;

    /** This method is used to detach a Subscriber from a Model Point.  See the
        attach() method for more details about the Subscription/Change
        Notification mechanism.

        The detach() method can be called even if the Subscriber is NOT
        currently attached.  The detach() method can be called within the
        Change Notification callback.
     */
    virtual void detachSubscriber( SubscriberApi& observer ) noexcept = 0;


public:
    /// Subscriber events
    enum Event_T
    {
        eATTACH,            //!< The Application is requesting to subscribe to a model point
        eDETACH,            //!< The Application is requesting to un-subscribe from the model point
        eDATA_CHANGED,      //!< The model point's data/state has change a pending change notification is needed
        eNOTIFYING,         //!< The subscriber's change notification callback is being called
        eNOTIFY_COMPLETE    //!< The subscriber's change notification callback has been completed
    };

    /** This method has PACKAGE Scope, i.e. it is intended to be ONLY accessible
        by other classes in the Cpl::Dm namespace.  The Application should
        NEVER call this method.

        This method is used by Model Point to process events related to the
        subscription/change-notification process

        This method is Thread Safe
     */
    virtual void processSubscriptionEvent_( SubscriberApi& subscriber,
                                            Event_T        event ) noexcept =0;


public:
    /** This method has PACKAGE Scope, i.e. it is intended to be ONLY accessible
        by other classes in the Cpl::Dm namespace.  The Application should
        NEVER call this method.

        This method is used to unconditionally update the Model Point's data.

        This method is NOT Thread Safe.

        Notes:
        1) The assumption is that Model Point Data instance and 'src' are the
           of the same type.
        2) The internal data size of the Model Point instance is ALWAYS honored
           when coping the data from 'src'
        3) The Model Point's sequence number is not changed.
    */
    virtual void copyDataFrom_( const void* srcData, size_t srcSize ) noexcept = 0;


    /** This method has PACKAGE Scope, i.e. it is intended to be ONLY accessible
        by other classes in the Cpl::Dm namespace.  The Application should
        NEVER call this method.

        This method is used update's the caller's 'Point Data' with the Model
        Point's data.

        This method is NOT Thread Safe.

        Notes:
        1) The assumption is that Model Point Data instance and 'dst' are the
           of the same type.
        2) The 'dstSize' of the destination ALWAYS honored when coping the
           Model Point's data to 'dst'
        3) The Model Point's sequence number is not changed.
    */
    virtual void copyDataTo_( void* dstData, size_t dstSize ) const noexcept = 0;

    /** This method has PACKAGE Scope, i.e. it is intended to be ONLY accessible
        by other classes in the Cpl::Dm namespace.  The Application should
        NEVER call this method.

        This method is NOT Thread Safe.

        This method compares the Model Point's data to the data of 'other' Model
        Point and returns true if the data of both points are the same. It is
        assumed that Model Point instance and 'other' are the of the same leaf
        class type.
     */
    virtual bool isDataEqual_( const void* otherData ) const noexcept = 0;

    /** This method has PACKAGE Scope, i.e. it is intended to be ONLY accessible
        by other classes in the Cpl::Dm namespace.  The Application should
        NEVER call this method.

        This method is NOT Thread Safe.

        This method returns a pointer to the Model Point's data for Import/Export
        operations.  BE VERY CAREFULL on how the pointer is used!
     */
    virtual const void* getImportExportDataPointer_() const noexcept = 0;

    /** This method has PACKAGE Scope, i.e. it is intended to be ONLY accessible
        by other classes in the Cpl::Dm namespace.  The Application should
        NEVER call this method.

        This method is NOT Thread Safe.

        This method returns the "internal size" of the Model Point's data point.
        This length (when applicable) INCLUDES ANY 'META/EXTRA' DATA that is
        NOT the valid/locked meta-data - but is used with the internal
        import()/export() methods.
   */
    virtual size_t getInternalDataSize_() const noexcept = 0;

    /** This method has PACKAGE Scope, i.e. it is intended to be ONLY accessible
        by other classes in the Cpl::Dm namespace.  The Application should
        NEVER call this method.

        This method is NOT Thread Safe.

        This method is used to export meta data when exporting a model
        point.  If there is error in exporting the metadata false is returned;
        else true is returned.

        A default implementation is provided that always returns true, i.e. for
        model points that do not have metadata.

        NOTE: The overall size of the destination has been validated with respect
              holding both the Model Point's metadata and the data - so the
              no 'dstLength' argument is passed/provided.
    */
    virtual bool exportMetadata_( void* dstDataStream, size_t& bytesAdded ) const noexcept { bytesAdded = 0;  return true; }

    /** This method has PACKAGE Scope, i.e. it is intended to be ONLY accessible
        by other classes in the Cpl::Dm namespace.  The Application should
        NEVER call this method.

        This method is NOT Thread Safe.

        This method is used to import meta data when importing a model
        point.  If the incoming meta data is 'not-valid/not-compatible' then
        false is returned and the import operation will fail; else true is
        returned.

        A default implementation is provided that always returns true, i.e. for
        model points that do not have metadata

        NOTE: The overall size of the source has been validated with respect
              containing both the Model Point's metadata and the data - so the
              no 'srcLength' argument is passed/provided.
   */
    virtual bool importMetadata_( const void* srcDataStream, size_t& bytesConsumed ) noexcept { bytesConsumed = 0;  return true; }


    /** This method has PACKAGE Scope, i.e. it is intended to be ONLY accessible
        by other classes in the Cpl::Dm namespace.  The Application should
        NEVER call this method.

        This method attempts to convert JSON object 'src' to its binary format
        and copies the result to the Model Point's internal data. The expected
        format of the JSON string is specific to the concrete leaf class.

        See Cpl::Dm::ModelDatabaseApi::fromJSON() method for JSON format.
     */
    virtual bool fromJSON_( JsonVariant&        src,
                            LockRequest_T       lockRequest,
                            uint16_t&           retSequenceNumber,
                            Cpl::Text::String*  errorMsg ) noexcept = 0;


protected:
    /** This method converts the MP data to a JSON key/value pair.  The key
        must be 'val' This method is only called when the MP data is valid
     */
    virtual void setJSONVal( JsonDocument& doc ) noexcept = 0;


public:
    /// Virtual destructor to make the compiler happy
    virtual ~ModelPoint() {}
};



};      // end namespaces
};
#endif  // end header latch
