#ifndef Cpl_Persistent_Indexed_Entry_Requests_h_
#define Cpl_Persistent_Indexed_Entry_Requests_h_
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


#include "Cpl/Itc/RequestMessage.h"
#include "Cpl/Itc/ResponseMessage.h"
#include "Cpl/Itc/SAP.h"
#include "Cpl/Persistent/IndexedEntryReader.h"
#include "Cpl/Persistent/Payload.h"

///
namespace Cpl {
///
namespace Persistent {


/** This abstract class define ITC message type and payload for the application
    to request read Indexed Entry data

    See the Cpl/Itc/README.txt file for the semantics for the 'ownership' of the
    payload contents.

    NOTE: This interface can/should NOT be used synchronously.  The application
          is required to only use asynchronous semantics.
 */
class GetLatestRequest
{
public:
    /// SAP for this API
    typedef Cpl::Itc::SAP<GetLatestRequest> SAP;

public:
    /// Payload for Message: GetLatest
    class Payload
    {
    public:
        /// INPUT/OUTPUT: Memory to hold the retrieved entry
        Cpl::Persistent::Payload&           m_entryDst;

        /// OUTPUT (response field): The 'marker' associated with the found/retrieved entry
        IndexedEntryReader::EntryMarker_T   m_markerEntryRetreived;

        /** GET results (response field)
            true  = entry was successfully retrieved/found
            false = no valid entry (that met the getXxx() criteria)
         */
        bool                                m_success;

    public:
        /// Constructor. Use for getLatest() message
        Payload( Cpl::Persistent::Payload& entryDst )
            :m_entryDst( entryDst ), m_success( false )
        {
        }
    };


public:
    /// Message Type: GetLatest
    typedef Cpl::Itc::RequestMessage<GetLatestRequest, Payload> GetLatestMsg;

    /// Request: GetLatest message
    virtual void request( GetLatestMsg& msg ) = 0;

public:
    /// Virtual Destructor
    virtual ~GetLatestRequest() {}
};


/** This abstract class define ITC message type and payload for asynchronous
    response (to the application) of a GetLatest message.

    The Application is responsible for implementing the response method(s).
 */
class GetLatestResponse
{
public:
    /// Response Message Type
    typedef Cpl::Itc::ResponseMessage<GetLatestResponse,
        GetLatestRequest,
        GetLatestRequest::Payload> GetLatestMsg;

public:
    /// Response
    virtual void response( GetLatestMsg& msg ) = 0;


public:
    /// Virtual destructor
    virtual ~GetLatestResponse() {}
};

////////////////////////////////////////////////////////////////////////////////
/** This abstract class define ITC message type and payload for the application
    to request read Indexed Entry data

    See the Cpl/Itc/README.txt file for the semantics for the 'ownership' of the
    payload contents.

    NOTE: This interface can/should NOT be used synchronously.  The application
          is required to only use asynchronous semantics.
 */
class GetNextRequest
{
public:
    /// SAP for this API
    typedef Cpl::Itc::SAP<GetNextRequest> SAP;

public:
    /// Payload for Message: GetNext
    class Payload
    {
    public:
        /// INPUT: newer timestamp to search criteria
        uint64_t                            m_newerThan;

        /// INPUT/OUTPUT: Memory to hold the retrieved entry
        Cpl::Persistent::Payload&           m_entryDst;

        /// INPUT: The 'marker' on where to begin searching from
        IndexedEntryReader::EntryMarker_T&  m_beginHereMarker;

        /// OUTPUT (response field): The 'marker' associated with the found/retrieved entry
        IndexedEntryReader::EntryMarker_T   m_markerEntryRetreived;

        /** GET results (response field)
            true  = entry was successfully retrieved/found
            false = no valid entry (that met the getXxx() criteria)
         */
        bool                                m_success;

    public:
        /// Constructor. Use for getNext() message
        Payload( Cpl::Persistent::Payload& entryDst, IndexedEntryReader::EntryMarker_T& beginHere, uint64_t newerThan )
            :m_newerThan( newerThan ), m_entryDst( entryDst ), m_beginHereMarker( beginHere ), m_success( false )
        {
        }

    };

public:
    /// Message Type: GetNext
    typedef Cpl::Itc::RequestMessage<GetNextRequest, Payload> GetNextMsg;

    /// Request: GetNext message
    virtual void request( GetNextMsg& msg ) = 0;

public:
    /// Virtual Destructor
    virtual ~GetNextRequest() {}
};


/** This abstract class define ITC message type and payload for asynchronous
    response (to the application) of a GetNext message.

    The Application is responsible for implementing the response method(s).
 */
class GetNextResponse
{
public:
    /// Response Message Type
    typedef Cpl::Itc::ResponseMessage<GetNextResponse,
        GetNextRequest,
        GetNextRequest::Payload> GetNextMsg;

public:
    /// Response
    virtual void response( GetNextMsg& msg ) = 0;


public:
    /// Virtual destructor
    virtual ~GetNextResponse() {}
};

////////////////////////////////////////////////////////////////////////////////
/** This abstract class define ITC message type and payload for the application
    to request read Indexed Entry data

    See the Cpl/Itc/README.txt file for the semantics for the 'ownership' of the
    payload contents.

    NOTE: This interface can/should NOT be used synchronously.  The application
          is required to only use asynchronous semantics.
 */
class GetPreviousRequest
{
public:
    /// SAP for this API
    typedef Cpl::Itc::SAP<GetPreviousRequest> SAP;

public:
    /// Payload for Message: GetPrevious
    class Payload
    {
    public:
        /// INPUT: newer timestamp to search criteria
        uint64_t                            m_olderThan;

        /// INPUT (optional): The 'marker' on where to begin searching from
        IndexedEntryReader::EntryMarker_T&  m_beginHereMarker;

        /// INPUT/OUTPUT: Memory to hold the retrieved entry
        Cpl::Persistent::Payload&           m_entryDst;

        /// OUTPUT (response field): The 'marker' associated with the found/retrieved entry
        IndexedEntryReader::EntryMarker_T   m_markerEntryRetreived;

        /** GET results (response field)
            true  = entry was successfully retrieved/found
            false = no valid entry (that met the getXxx() criteria)
         */
        bool                                m_success;

    public:
        /// Constructor. Use for getPrevious() message
        Payload( Cpl::Persistent::Payload& entryDst, IndexedEntryReader::EntryMarker_T& beginHere, uint64_t olderThan )
            :m_olderThan( olderThan ), m_beginHereMarker( beginHere ), m_entryDst( entryDst ), m_success( false )
        {
        }
    };


    /// Message Type: GetPrevious
    typedef Cpl::Itc::RequestMessage<GetPreviousRequest, Payload> GetPreviousMsg;

    /// Request: GetPrevious message
    virtual void request( GetPreviousMsg& msg ) = 0;

public:
    /// Virtual Destructor
    virtual ~GetPreviousRequest() {}
};

/** This abstract class define ITC message type and payload for asynchronous
    response (to the application) of a GetPrevious message.

    The Application is responsible for implementing the response method(s).
 */
class GetPreviousResponse
{
public:
    /// Response Message Type
    typedef Cpl::Itc::ResponseMessage<GetPreviousResponse,
        GetPreviousRequest,
        GetPreviousRequest::Payload> GetPreviousMsg;

public:
    /// Response
    virtual void response( GetPreviousMsg& msg ) = 0;


public:
    /// Virtual destructor
    virtual ~GetPreviousResponse() {}
};


////////////////////////////////////////////////////////////////////////////////
/** This abstract class define ITC message type and payload for the application
    to request read Indexed Entry data

    See the Cpl/Itc/README.txt file for the semantics for the 'ownership' of the
    payload contents.

    NOTE: This interface can/should NOT be used synchronously.  The application
          is required to only use asynchronous semantics.
 */
class GetByBufferIndexRequest
{
public:
    /// SAP for this API
    typedef Cpl::Itc::SAP<GetByBufferIndexRequest> SAP;

public:
    /// Payload for Message: GetByBufferIndex
    class Payload
    {
    public:
        /// INPUT: buffer index
        size_t                              m_index;

        /// INPUT/OUTPUT: Memory to hold the retrieved entry
        Cpl::Persistent::Payload&           m_entryDst;

        /// OUTPUT (response field): The 'marker' associated with the found/retrieved entry
        IndexedEntryReader::EntryMarker_T   m_markerEntryRetreived;

        /** GET results (response field)
            true  = entry was successfully retrieved/found
            false = no valid entry (that met the getXxx() criteria)
         */
        bool                                m_success;

    public:
        /// Constructor. Use for getLatest() message
        Payload( Cpl::Persistent::Payload& entryDst, size_t index )
            :m_index( index ), m_entryDst( entryDst ), m_success( false )
        {
        }
    };


    /// Message Type: GetByBufferIndex
    typedef Cpl::Itc::RequestMessage<GetByBufferIndexRequest, Payload> GetByBufferIndexMsg;

    /// Request: GetByBufferIndex message
    virtual void request( GetByBufferIndexMsg& msg ) = 0;


public:
    /** This method returns the maximum allowed 'bufferIndex' when calling
        getByIndex().

        NOTE: THIS IS NOT AN ITC MESSAGE REQUEST - it is 'traditional' thread-safe
        function call.
     */
    virtual size_t getMaxIndex() const noexcept = 0;


public:
    /// Virtual Destructor
    virtual ~GetByBufferIndexRequest() {}
};


/** This abstract class define ITC message type and payload for asynchronous
    response (to the application) of a GetByBufferIndex message.

    The Application is responsible for implementing the response method(s).
 */
class GetByBufferIndexResponse
{
public:
    /// Response Message Type
    typedef Cpl::Itc::ResponseMessage<GetByBufferIndexResponse,
        GetByBufferIndexRequest,
        GetByBufferIndexRequest::Payload> GetByBufferIndexMsg;

public:
    /// Response
    virtual void response( GetByBufferIndexMsg& msg ) = 0;


public:
    /// Virtual destructor
    virtual ~GetByBufferIndexResponse() {}
};


////////////////////////////////////////////////////////////////////////////////
/** This abstract class define ITC message type and payload for the application
    to clear/delete all entries

    See the Cpl/Itc/README.txt file for the semantics for the 'ownership' of the
    payload contents.

    NOTE: This interface can/should NOT be used synchronously.  The application
          is required to only use asynchronous semantics.
 */
class ClearAllEntriesRequest
{
public:
    /// SAP for this API
    typedef Cpl::Itc::SAP<ClearAllEntriesRequest> SAP;

public:
    /// Payload for Message
    class Payload
    {
    public:
        /** Clear results (response field)
            true  = all entries where successfully cleared
            false = an error occurred
         */
        bool m_success;

    public:
        /// Constructor. Use for getLatest() message
        Payload()
            :m_success( false )
        {
        }
    };


    /// Message Type: GetPrevious
    typedef Cpl::Itc::RequestMessage<ClearAllEntriesRequest, Payload> ClearAllEntriesMsg;

    /// Request: GetPrevious message
    virtual void request( ClearAllEntriesMsg& msg ) = 0;

public:
    /// Virtual Destructor
    virtual ~ClearAllEntriesRequest() {}
};

/** This abstract class define ITC message type and payload for asynchronous
    response (to the application) of a ClearAllEntries message.

    The Application is responsible for implementing the response method(s).
 */
class ClearAllEntriesResponse
{
public:
    /// Response Message Type
    typedef Cpl::Itc::ResponseMessage<ClearAllEntriesResponse,
        ClearAllEntriesRequest,
        ClearAllEntriesRequest::Payload> ClearAllEntriesMsg;

public:
    /// Response
    virtual void response( ClearAllEntriesMsg& msg ) = 0;

public:
    /// Virtual destructor
    virtual ~ClearAllEntriesResponse() {}
};



};      // end namespaces
};
#endif  // end header latch
