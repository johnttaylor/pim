#ifndef Cpl_MApp_Requests_h_
#define Cpl_MApp_Requests_h_
/*-----------------------------------------------------------------------------
* COPYRIGHT_HEADER_TO_BE_FILLED_LATER
*----------------------------------------------------------------------------*/
/** @file */


#include "Cpl/Itc/RequestMessage.h"
#include "Cpl/Itc/ResponseMessage.h"
#include "Cpl/Itc/SAP.h"
#include "Cpl/MApp/MAppApi.h"

///
namespace Cpl {
///
namespace MApp {


/** This abstract class define ITC message type and payload for the application
    to start a MApp
 */
class StartMAppRequest
{
public:
    /// SAP for this API
    typedef Cpl::Itc::SAP<StartMAppRequest> SAP;

public:
    /// Payload for Message: GetLaMApp
    class Payload
    {
    public:
        /// INPUT: The name of the MApp to run
        const char* mappName;

        /// INPUT: Optional 'command line' arguments for the MApp
        char*       mappArgs;

        /** OUTPUT: results
            true  = MApp was found and started
            false = no such MApp
         */
        bool        success;

    public:
        /// Constructor. 
        Payload( const char* name, char* args )
            :mappName( name ), mappArgs( args), success( false )
        {
        }
    };


public:
    /// Message Type: Start
    typedef Cpl::Itc::RequestMessage<StartMAppRequest, Payload> StartMAppMsg;

    /// Request: Start message
    virtual void request( StartMAppMsg& msg ) = 0;

public:
    /// Virtual Destructor
    virtual ~StartMAppRequest() {}
};



////////////////////////////////////////////////////////////////////////////////
/** This abstract class define ITC message type and payload for the application
    to stop the current MApp
 */
class StopMAppRequest
{
public:
    /// SAP for this API
    typedef Cpl::Itc::SAP<StopMAppRequest> SAP;

public:
    /// Payload for Message: StopMApp 
    class Payload
    {
    public:
        /// INPUT: The name of the MApp to stop
        const char* mappName;

        /** OUTPUT: results
            true  = the MApp was stopped
            false = the specified MApp was not the started state
         */
        bool        success;

    public:
        /// Constructor. 
        Payload( const char* name )
            :mappName( name ), success( false )
        {
        }
    };

public:
    /// Message Type: Stop
    typedef Cpl::Itc::RequestMessage<StopMAppRequest, Payload> StopMAppMsg;

    /// Request: Stop message
    virtual void request( StopMAppMsg& msg ) = 0;

public:
    /// Virtual Destructor
    virtual ~StopMAppRequest() {}
};

////////////////////////////////////////////////////////////////////////////////
/** This abstract class define ITC message type and payload for the application
    to stop the current MApp
 */
class StopAllMAppRequest
{
public:
    /// SAP for this API
    typedef Cpl::Itc::SAP<StopAllMAppRequest> SAP;

public:
    /// Payload for Message: StopMApp (No actual Data -->just a type name)
    class Payload
    {
    public:
        /// Constructor. 
        Payload()
        {
        }
    };

public:
    /// Message Type: Stop
    typedef Cpl::Itc::RequestMessage<StopAllMAppRequest, Payload> StopAllMAppMsg;

    /// Request: Stop message
    virtual void request( StopAllMAppMsg& msg ) = 0;

public:
    /// Virtual Destructor
    virtual ~StopAllMAppRequest() {}
};


////////////////////////////////////////////////////////////////////////////////
/** This abstract class define ITC message type and payload for the application
    to get list of all of the MApps instances.

 */
class GetAvailableMAppRequest
{
public:
    /// SAP for this API
    typedef Cpl::Itc::SAP<GetAvailableMAppRequest> SAP;

public:
    /// Payload for Message: 
    class Payload
    {
    public:
        /// INPUT/OUTPUT: Memory to store the returned list
        Cpl::MApp::MAppApi** dstList;

        /// INPUT/OUTPUT: Maximum number of elements that can be stored in 'dstList'.  Set to zero for the 'overflow' error case
        size_t               dstMaxElements;

        /// OUTPUT: Number of items returned
        size_t               numElements;

    public:
        /// Constructor. 
        Payload( Cpl::MApp::MAppApi** list, size_t maxElems ) : dstList( list ), dstMaxElements( maxElems ), numElements(0) {}
    };

public:
    /// Message Type: GetAvailable
    typedef Cpl::Itc::RequestMessage<GetAvailableMAppRequest, Payload> GetAvailableMAppMsg;

    /// Request: GetAvailable message
    virtual void request( GetAvailableMAppMsg& msg ) = 0;

public:
    /// Virtual Destructor
    virtual ~GetAvailableMAppRequest() {}
};


////////////////////////////////////////////////////////////////////////////////
/** This abstract class define ITC message type and payload for the application
    to Get a list of started MApp instances

 */
class GetStartedMAppRequest
{
public:
    /// SAP for this API
    typedef Cpl::Itc::SAP<GetStartedMAppRequest> SAP;

public:
    /// Payload for Message: 
    class Payload
    {
    public:
        /// INPUT/OUTPUT: Memory to store the returned list
        Cpl::MApp::MAppApi** dstList;

        /// INPUT/OUTPUT: Maximum number of elements that can be stored in 'dstList'.  Set to zero for the 'overflow' error case
        size_t               dstMaxElements;

        /// OUTPUT: Number of items returned
        size_t               numElements;

    public:
        /// Constructor. 
        Payload( Cpl::MApp::MAppApi** list, size_t maxElems ) : dstList( list ), dstMaxElements( maxElems ), numElements( 0 ) {}
    };

public:
    /// Message Type: GetStarted
    typedef Cpl::Itc::RequestMessage<GetStartedMAppRequest, Payload> GetStartedMAppMsg;

    /// Request: GetStarted message
    virtual void request( GetStartedMAppMsg& msg ) = 0;

public:
    /// Virtual Destructor
    virtual ~GetStartedMAppRequest() {}
};

////////////////////////////////////////////////////////////////////////////////
/** This abstract class define ITC message type and payload for the application
    to look-up a MApp by name

 */
class LookupMAppRequest
{
public:
    /// SAP for this API
    typedef Cpl::Itc::SAP<LookupMAppRequest> SAP;

public:
    /// Payload for Message: 
    class Payload
    {
    public:
        /// INPUT: Name of the MApp instance to lookup
        const char* name;

        /// OUTPUT: Found instance (or null if not found)
        MAppApi*    foundInstance;

    public:
        /// Constructor. 
        Payload( const char* nameToLookup ): name(nameToLookup), foundInstance(nullptr) {}
    };

public:
    /// Message Type: Lookup
    typedef Cpl::Itc::RequestMessage<LookupMAppRequest, Payload> LookupMAppMsg;

    /// Request: Lookup message
    virtual void request( LookupMAppMsg& msg ) = 0;

public:
    /// Virtual Destructor
    virtual ~LookupMAppRequest() {}
};


};      // end namespaces
};
#endif  // end header latch
