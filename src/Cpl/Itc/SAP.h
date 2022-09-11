#ifndef Cpl_Itc_SAP_h_
#define Cpl_Itc_SAP_h_
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

#include "Cpl/Itc/PostApi.h"


///
namespace Cpl {
///
namespace Itc {

/** This concrete template class represents the interface to a ITC Service
    Access Point (SAP). A SAP 'binds' a Request API with a mailbox interface.
    In effect a Service is the 'fully-qualified' identifier for posting a message
    to a specific Request API.
 */
template <class SERVER>
class SAP : public PostApi
{
protected:
    /// Reference to the instance that implements the request service
    SERVER & m_api;

    /// Reference to the associated post-message/mailbox interface
    PostApi&   m_mbox;

public:
    /** This constructor requires a reference to the
        request API and its associated mailbox/post-message
        API.
     */
    SAP( SERVER& api, PostApi& mbox );

public:
    /// Returns a reference to the associated Cpl::Itc::Request instance
    SERVER & getServer() noexcept;


public:
    /// See PostApi                                 
    void post( Message& msg ) noexcept;

    /// See PostApi
    void postSync( Message& msg ) noexcept;
};


/////////////////////////////////////////////////////////////////////////////
//                          IMPLEMENATION
/////////////////////////////////////////////////////////////////////////////

template <class SERVER>
SAP<SERVER>::SAP( SERVER& api, PostApi& mbox )
    :m_api( api ),
    m_mbox( mbox )
{
}

template <class SERVER>
SERVER& SAP<SERVER>::getServer() noexcept
{
    return m_api;
}

template <class SERVER>
void SAP<SERVER>::post( Message& msg ) noexcept
{
    m_mbox.post( msg );
}

template <class SERVER>
void SAP<SERVER>::postSync( Message& msg ) noexcept
{
    m_mbox.postSync( msg );
}

};      // end namespaces
};
#endif  // end header latch
