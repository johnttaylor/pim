#ifndef Storm_Component_AirFilterMonitor_h_
#define Storm_Component_AirFilterMonitor_h_
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Apps Project.  The Colony.Apps Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.apps/license.txt
*
* Copyright (c) 2015-2020  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file */

#include "Storm/Component/Base.h"
#include "Cpl/Dm/Mp/Uint32.h"
#include "Cpl/Dm/Mp/ElapsedPrecisionTime.h"
#include "Storm/Dm/MpSimpleAlarm.h"
#include "Storm/Dm/MpVirtualOutputs.h"


/// Namespaces
namespace Storm
{
/// Namespaces
namespace Component
{
/** This class is responsible for monitoring how long the indoor fan has been
    running and raise an alert when it is time to change the air filter.
 */
class AirFilterMonitor : public Base
{
public:
    /// Input Model Points
    struct Input_T
    {
        Cpl::Dm::Mp::Uint32*                maxAirFilterHours;      //!< The user configured number of operation hours between Indoor Air filter changes
        Cpl::Dm::Mp::ElapsedPrecisionTime*  airFilterOperationTime; //!< The elapsed time that the indoor blower has been on
        Storm::Dm::MpVirtualOutputs*        vOutputs;               //!< The virtual system outputs
        Storm::Dm::MpSimpleAlarm*           airFilterAlert;         //!< The Air Filter Alert
    };


    /// Output Model Points
    struct Output_T
    {
        Storm::Dm::MpSimpleAlarm*           airFilterAlert;         //!< The Air Filter Alert
        Cpl::Dm::Mp::ElapsedPrecisionTime*  airFilterOperationTime; //!< The elapsed time that the indoor blower has been on
    };


public:
    /// Constructor
    AirFilterMonitor( struct Input_T ins, struct Output_T outs );

    /// See Storm::Component::Api
    bool start( Cpl::System::ElapsedTime::Precision_T intervalTime );

protected:
    /// See Storm::Component::Base
    bool execute( Cpl::System::ElapsedTime::Precision_T currentTick,
                  Cpl::System::ElapsedTime::Precision_T currentInterval );

protected:
    /// My Model Point inputs
    struct Input_T                          m_in;

    /// My Model Point outputs
    struct Output_T                         m_out;

    /// Time marker of when the blower was turned on
    Cpl::System::ElapsedTime::Precision_T   m_markBlowerOn;

    /// Remember the previous blower on state
    bool                                    m_blowerWasOn;
};



};      // end namespace
};      // end namespace
#endif  // end header latch
