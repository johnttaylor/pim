/*-----------------------------------------------------------------------------
* COPYRIGHT_HEADER_TO_BE_FILLED_LATER
*----------------------------------------------------------------------------*/
/** @file */


#include "Manager.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Itc/SyncReturnHandler.h"
#include "Cpl/Text/FString.h"

using namespace Cpl::MApp;

static MAppApi* searchList( Cpl::Container::Map<MAppApi>& listToSearch,
                            const char*                   nameToFind );

////////////////////////////////////////////////////////////////////////////////
Manager::Manager( Cpl::Dm::MailboxServer&       myMbox,
                  Cpl::Container::Map<MAppApi>& listOfMApps )
    : Cpl::Itc::CloseSync( *((Cpl::Itc::PostApi*) &myMbox) )
    , m_inactiveMApps( listOfMApps )
    , m_startedMApps()
    , m_opened( false )
{
}


void Manager::request( Cpl::Itc::OpenRequest::OpenMsg& msg )
{
    if ( m_opened )
    {
        CPL_SYSTEM_TRACE_MSG( OPTION_CPL_MAPP_TRACE_SECTION, ("Cpl::MApp::Manager: open() called when already opened") );
    }
    else
    {
        // Housekeeping
        m_opened  = true;

        // initialize my instances
        MAppApi* itemPtr = m_inactiveMApps.first();
        while ( itemPtr )
        {
            itemPtr->intialize_();
            itemPtr = m_inactiveMApps.next( *itemPtr );
        }
    }

    // Return the message
    msg.returnToSender();
}

void Manager::request( Cpl::Itc::CloseRequest::CloseMsg& msg )
{
    if ( !m_opened )
    {
        CPL_SYSTEM_TRACE_MSG( OPTION_CPL_MAPP_TRACE_SECTION, ("Cpl::MApp::Manager: close() called when not opened") );
    }
    else
    {
        // Housekeeping
        m_opened  = false;

        // shutdown my inactive instances
        MAppApi* itemPtr = m_inactiveMApps.first();
        while ( itemPtr )
        {
            itemPtr->shutdown_();
            itemPtr = m_inactiveMApps.next( *itemPtr );
        }

        // shutdown my started instances (and return the instance to the inactive list (for the use case of restarting the Manager)
        itemPtr = m_startedMApps.getFirst();
        while ( itemPtr )
        {
            itemPtr->stop_();
            itemPtr->shutdown_();

            // Return the instance to the inactive list
            m_inactiveMApps.insert( *itemPtr );

            // Get the next item
            itemPtr = m_startedMApps.getFirst();
        }
    }

    // Return the message
    msg.returnToSender();
}
//////////////////////////////////////////////////////////////////////////////
void Manager::request( StartMAppMsg& msg )
{
    StartMAppRequest::Payload& payload = msg.getPayload();
    payload.success = false;

    // Look-up the MApp by name
    MAppApi* mapp = searchList( m_inactiveMApps, payload.mappName );
    if ( mapp != nullptr )
    {
        payload.success = true;

        // Put the MApp instance into the started list
        m_inactiveMApps.removeItem( *mapp );
        m_startedMApps.insert( *mapp );

        // Start the MApp
        char* args = payload.mappArgs;
        if ( args == nullptr )
        {
            CPL_SYSTEM_TRACE_MSG( OPTION_CPL_MAPP_TRACE_SECTION, ("FAILED to start: %s due to nullptr for 'args'", mapp->getName()) );
            payload.success = false;

            // Restore the MApp to the inactive list
            m_startedMApps.removeItem( *mapp );
            m_inactiveMApps.insert( *mapp );
        }
        else
        {
            CPL_SYSTEM_TRACE_MSG( OPTION_CPL_MAPP_TRACE_SECTION, ("Starting: %s", mapp->getName()) );
            if ( !mapp->start_( args ) )
            {
                CPL_SYSTEM_TRACE_MSG( OPTION_CPL_MAPP_TRACE_SECTION, ("FAILED to start: %s %s", mapp->getName(), args) );
                payload.success = false;

                // Restore the MApp to the inactive list
                m_startedMApps.removeItem( *mapp );
                m_inactiveMApps.insert( *mapp );
            }
        }
    }

    msg.returnToSender();
}

void Manager::request( StopMAppMsg & msg )
{
    StopMAppRequest::Payload& payload = msg.getPayload();
    payload.success = false;

    // Look-up the MApp by name in the 'running' list
    MAppApi* mapp = searchList( m_startedMApps, payload.mappName );
    if ( mapp != nullptr )
    {
        CPL_SYSTEM_TRACE_MSG( OPTION_CPL_MAPP_TRACE_SECTION, ("Stopping: %s", mapp->getName()) );
        mapp->stop_();

        // Return the instance to the inactive list
        m_startedMApps.removeItem( *mapp );
        m_inactiveMApps.insert( *mapp );
    
        payload.success = true;
    }

    msg.returnToSender();
}

void Manager::request( StopAllMAppMsg & msg )
{
    // Walk the running list
    MAppApi* item = m_startedMApps.getFirst();
    while ( item  )
    {
        // Stop the instance
        CPL_SYSTEM_TRACE_MSG( OPTION_CPL_MAPP_TRACE_SECTION, ("Stopping: %s", item->getName()) );
        item->stop_();

        // Move the stop instance to the inactive list
        m_inactiveMApps.insert( *item );
        
        // Get the next item
        item = m_startedMApps.getFirst();
    }

    msg.returnToSender();
}

void Manager::request( GetAvailableMAppMsg& msg )
{
    GetAvailableMAppRequest::Payload& payload = msg.getPayload();
    size_t idx                                = 0;
    size_t maxElems                           = payload.dstMaxElements;

    // Walk the inactive list
    MAppApi* item = m_inactiveMApps.first();
    while ( item && maxElems )
    {
        payload.dstList[idx] = item;
        idx++;
        maxElems--;
        payload.numElements++;

        item = m_inactiveMApps.next( *item );
    }

    // Walk the running list
    item = m_startedMApps.first();
    while ( item && maxElems )
    {
        payload.dstList[idx] = item;
        idx++;
        maxElems--;
        payload.numElements++;

        item = m_startedMApps.next( *item );
    }

    msg.returnToSender();
}

void Manager::request( GetStartedMAppMsg & msg )
{
    GetStartedMAppRequest::Payload& payload = msg.getPayload();
    size_t idx                              = 0;
    size_t maxElems                         = payload.dstMaxElements;

    // Walk the running list
    MAppApi* item = m_startedMApps.first();
    while ( item && maxElems )
    {
        payload.dstList[idx] = item;
        idx++;
        maxElems--;
        payload.numElements++;

        item = m_startedMApps.next( *item );
    }

    msg.returnToSender();
}

void Manager::request( LookupMAppMsg & msg )
{
    LookupMAppRequest::Payload& payload = msg.getPayload();

    // Search both lists
    MAppApi* mapp = searchList( m_inactiveMApps, payload.name );
    if ( mapp == nullptr )
    {
        mapp = searchList( m_startedMApps, payload.name );
    }
    payload.foundInstance = mapp;

    msg.returnToSender();
}

//////////////////////////////////////////////////////////////////////////////
bool Manager::startMApp( const char* name, char* args ) noexcept
{
    StartMAppRequest::Payload      payload( name, args );
    Cpl::Itc::SyncReturnHandler    srh;
    StartMAppMsg                   msg( *this, payload, srh );
    m_mbox.postSync( msg );

    return payload.success;
}


bool Manager::stopMApp( const char* name ) noexcept
{
    StopMAppRequest::Payload     payload( name );
    Cpl::Itc::SyncReturnHandler  srh;
    StopMAppMsg                  msg( *this, payload, srh );
    m_mbox.postSync( msg );
    return payload.success;
}

void Manager::stopAllMApps() noexcept
{
    StopAllMAppRequest::Payload  payload;
    Cpl::Itc::SyncReturnHandler  srh;
    StopAllMAppMsg               msg( *this, payload, srh );
    m_mbox.postSync( msg );
}

bool Manager::getAvailableMApps( Cpl::MApp::MAppApi* dstList[], size_t dstMaxElements, size_t& numElemsFound ) noexcept
{
    GetAvailableMAppRequest::Payload  payload( dstList, dstMaxElements );
    Cpl::Itc::SyncReturnHandler       srh;
    GetAvailableMAppMsg               msg( *this, payload, srh );
    m_mbox.postSync( msg );

    numElemsFound = payload.numElements;
    return payload.dstMaxElements != 0;
}

bool Manager::getStartedMApps( Cpl::MApp::MAppApi* dstList[], size_t dstMaxElements, size_t& numElemsFound ) noexcept
{
    GetStartedMAppRequest::Payload  payload( dstList, dstMaxElements );
    Cpl::Itc::SyncReturnHandler     srh;
    GetStartedMAppMsg               msg( *this, payload, srh );
    m_mbox.postSync( msg );

    numElemsFound = payload.numElements;
    return payload.dstMaxElements != 0;
}

Cpl::MApp::MAppApi* Manager::lookUpMApp( const char* mappName ) noexcept
{
    LookupMAppRequest::Payload  payload( mappName );
    Cpl::Itc::SyncReturnHandler srh;
    LookupMAppMsg               msg( *this, payload, srh );
    m_mbox.postSync( msg );

    return payload.foundInstance;
}

///////////////////////////////
MAppApi* searchList( Cpl::Container::Map<MAppApi>& listToSearch,
                     const char*                   nameToFind )
{

    Cpl::Container::KeyLiteralString keyToFind( nameToFind );
    return listToSearch.find( keyToFind );
}
