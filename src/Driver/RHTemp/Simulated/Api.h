#ifndef Driver_RHTemp_Simulated_Api_h_
#define Driver_RHTemp_Simulated_Api_h_
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


#include "Driver/RHTemp/Api.h"
#include "Cpl/Dm/Mp/Float.h"
#include "Cpl/Dm/Mp/Bool.h"


///
namespace Driver {
///
namespace RHTemp {
///
namespace Simulated {

/** This class implements the RH/Temperature driver interface using model points
    as the physical signals.  
    
    The Application is responsible for providing the Model Point instances.

    The class is NOT thread safe.  The application is RESPONSIBLE for ensuring
    thread-safe usage.
 */
class Api: public Driver::RHTemp::Api
{
public:
    /// Constructor
    Api( Cpl::Dm::Mp::Float& simulatedRHInputMP,
         Cpl::Dm::Mp::Float& simulatedTempInputMP,
         Cpl::Dm::Mp::Bool&  simulateHeaterStateMP );

public:
    /// See Driver::RHTemp::Api
    bool start() noexcept;

    /// See Driver::RHTemp::Api
    void stop() noexcept;

public:
    /// See Driver::RHTemp::Api
    bool sample( float& rhOut, float& tempCOut ) noexcept;

public:
    /// See Driver::RHTemp::Api
    bool setHeaterState( bool enabled ) noexcept;

public:
    /// See Driver::RHTemp::Api
    Driver::RHTemp::Api::SamplingState_T startSample() noexcept;

    /// See Driver::RHTemp::Api
    Driver::RHTemp::Api::SamplingState_T getSamplingState() noexcept;

    /// See Driver::RHTemp::Api
    Driver::RHTemp::Api::SamplingState_T getSample( float& rhOut, float& tempCOut ) noexcept;

protected:
    /// Simulated input value
    Cpl::Dm::Mp::Float&     m_mpRh;

    /// Simulated input value
    Cpl::Dm::Mp::Float&     m_mpTemp;

    /// Simulated output value
    Cpl::Dm::Mp::Bool&      m_mpHeaterEnabled;

    /// Started state
    bool                    m_started;
};

} // End namespace(s)
}
}

/*--------------------------------------------------------------------------*/
#endif  // end header latch
