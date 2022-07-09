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


#include "Subscriber.h"
#include "Cpl/System/FatalError.h"

using namespace Cpl::Dm;



/////////////////////////////
SubscriberBase::SubscriberBase( Cpl::Dm::EventLoop& myEventLoop )
    : m_state( 0 )
    , m_point( 0 )
    , m_eventLoopHdl( myEventLoop )
    , m_seqNumber( ModelPoint::SEQUENCE_NUMBER_UNKNOWN )
{
}

NotificationApi_* SubscriberBase::getNotificationApi_() const noexcept
{
    return &m_eventLoopHdl;
}

void SubscriberBase::setModelPoint_( ModelPoint* modelPoint ) noexcept
{
    m_point = modelPoint;
}

int SubscriberBase::getState_() const noexcept
{
    return m_state;
}

void SubscriberBase::setState_( int newState ) noexcept
{
    m_state = newState;
}

uint16_t SubscriberBase::getSequenceNumber_() const noexcept
{
    return m_seqNumber;
}

void SubscriberBase::setSequenceNumber_( uint16_t newSeqNumber ) noexcept
{
    m_seqNumber = newSeqNumber;
}

ModelPoint* SubscriberBase::getModelPoint_() noexcept
{
    if ( m_point == 0 )
    {
        Cpl::System::FatalError::logf( "Cpl::Dm::Subscriber().  Protocol Error.  getModelPoint() was called before setModelPoint()" );
        return 0;
    }

    return m_point;
}
