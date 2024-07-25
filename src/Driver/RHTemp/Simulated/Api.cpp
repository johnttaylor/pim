/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2022  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/

#include "Api.h"

using namespace Driver::RHTemp::Simulated;



/////////////////////////////////////
Api::Api( Cpl::Dm::Mp::Float& simulatedRHInputMP,
          Cpl::Dm::Mp::Float& simulatedTempInputMP,
          Cpl::Dm::Mp::Bool&  simulateHeaterStateMP )
    : m_mpRh( simulatedRHInputMP )
    , m_mpTemp( simulatedTempInputMP )
    , m_mpHeaterEnabled( simulateHeaterStateMP )
    , m_started( false )
{
}

/////////////////////////////////////
bool Api::start() noexcept
{
    m_started = true;
    m_mpHeaterEnabled.write( false );
    return true;
}

void Api::stop() noexcept
{
    m_started = false;
    m_mpHeaterEnabled.write( false );
    return;
}

bool Api::sample( float& rhOut, float& tempCOut ) noexcept
{
    if ( m_started )
    {
        if ( m_mpRh.read( rhOut ) && m_mpTemp.read( tempCOut ) )
        {
            return true;
        }
    }

    return false;
}

bool Api::setHeaterState( bool enabled ) noexcept
{
    if ( m_started )
    {
        m_mpHeaterEnabled.write( enabled );
        return true;
    }

    return false;
}

Driver::RHTemp::Api::SamplingState_T Api::startSample() noexcept
{
    if ( m_started )
    {
        return Driver::RHTemp::Api::eSAMPLING;
    }

    return Driver::RHTemp::Api::eERROR;
}

Driver::RHTemp::Api::SamplingState_T Api::getSamplingState() noexcept
{
    if ( m_started )
    {
        return m_mpRh.isNotValid() || m_mpTemp.isNotValid() ? RHTemp::Api::eSAMPLING : RHTemp::Api::eSAMPLE_READY;
    }

    return Driver::RHTemp::Api::eERROR;
}

Driver::RHTemp::Api::SamplingState_T Api::getSample( float& rhOut, float& tempCOut ) noexcept
{
    if ( m_started )
    {
        if ( m_mpRh.read( rhOut ) && m_mpTemp.read( tempCOut ) )
        {
            return RHTemp::Api::eSAMPLE_READY;
        }

        return RHTemp::Api::eSAMPLING;
    }

    return Driver::RHTemp::Api::eERROR;
}

