#ifndef Storm_Component_Equipment_Off_h_
#define Storm_Component_Equipment_Off_h_
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

#include "Storm/Component/Control.h"

///
namespace Storm
{
///
namespace Component
{
///
namespace Equipment
{
/** This concrete class provides the business logic for controlling the HVAC
    system when the system is off mode.
 */
class Off : public Storm::Component::Control::Equipment
{
public:
    /// Constructor
    Off();

public:
    /// See Storm::Component::Control::Equipment
    bool executeActive( Args_T& args ) noexcept;

    /// See Storm::Component::Control::Equipment
    bool executeOff( Args_T& args ) noexcept;

    /// See Storm::Component::Control::Equipment
    bool start( Cpl::System::ElapsedTime::Precision_T intervalTime ) noexcept;

    /// See Storm::Component::Control::Equipment
    void reset() noexcept;

    /// See Storm::Component::Control::Equipment
    Storm::Type::OperatingMode getOperatingMode() const noexcept { return Storm::Type::OperatingMode::eOFF; }
};



};      // end namespace
};
};
#endif  // end header latch
