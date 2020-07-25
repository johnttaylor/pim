#ifndef Storm_Dm_MpFanMode_h_
#define Storm_Dm_MpFanMode_h_
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


#include "Storm/Type/FanMode.h"
#include "Cpl/Dm/Mp/Enum.h"
#include "Cpl/Dm/Subscriber.h"

///
namespace Storm {
namespace Dm {

/** Concrete Model Point the FanMode Enum
 */
class MpFanMode : public Cpl::Dm::Mp::Enum<Storm::Type::FanMode>
{
public:
    /// Constructor. 
    MpFanMode( Cpl::Dm::ModelDatabase& myModelBase, Cpl::Dm::StaticInfo& staticInfo )
		:Cpl::Dm::Mp::Enum<Storm::Type::FanMode>( myModelBase, staticInfo )
	{
	}

    /// Constructor. 
    MpFanMode( Cpl::Dm::ModelDatabase& myModelBase, Cpl::Dm::StaticInfo& staticInfo, Storm::Type::FanMode initialValue )
		: Cpl::Dm::Mp::Enum<Storm::Type::FanMode>( myModelBase, staticInfo, initialValue )
	{
	}

    /// Type
    const char* getTypeAsText() const noexcept
	{
		return "Storm::Dm::MpFanMode";
	}

public:
    /// Type safe subscriber
    typedef Cpl::Dm::Subscriber<MpFanMode> Observer;

    /// Type safe register observer
    void attach( Observer& observer, uint16_t initialSeqNumber=SEQUENCE_NUMBER_UNKNOWN ) noexcept
	{
		ModelPointCommon_::attach( observer, initialSeqNumber );
	}

    /// Type safe un-register observer
    void detach( Observer& observer ) noexcept
	{
		ModelPointCommon_::detach( observer );
	}
};


};      // end namespaces
};
#endif  // end header latch
