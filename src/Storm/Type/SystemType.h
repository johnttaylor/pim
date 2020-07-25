#ifndef Storm_Type_SystemType_h_
#define Storm_Type_SystemType_h_
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


#include "Cpl/Type/enum.h"

/// Bit Pattern for an AC outdoor unit
#define STORM_TYPE_SYSTEM_TYPE_AC_BITS          0x0001

/// Bit Pattern for an HeatPump outdoor unit
#define STORM_TYPE_SYSTEM_TYPE_HP_BITS          0x0002

/// Bit Pattern for NO outdoor unit
#define STORM_TYPE_SYSTEM_TYPE_NO_ODUNIT_BITS   0x000F

/// Bit pattern to get the OD Unit type
#define STORM_TYPE_SYSTEM_TYPE_ODUNIT_MASK      0x000F


/// Bit Pattern for no outdoor unit stages (only valid when odunit == none)
#define STORM_TYPE_SYSTEM_TYPE_OD_NOSTAGES      0x0000

/// Bit Pattern for outdoor unit with a single stage
#define STORM_TYPE_SYSTEM_TYPE_OD_STAGE1        0x0010

/// Bit Pattern for outdoor unit with two stages
#define STORM_TYPE_SYSTEM_TYPE_OD_STAGE2        0x0020

/// Bit Pattern for outdoor unit with three stages
#define STORM_TYPE_SYSTEM_TYPE_OD_STAGE3        0x0030

/// Bit pattern to get the number of OD stages
#define STORM_TYPE_SYSTEM_TYPE_OD_STAGE_MASK    0x00F0

/// Number of bit-shifts for normalize the OD stages
#define STORM_TYPE_SYSTEM_TYPE_OD_STAGE_SHIFT_BITS  4


/// Bit Pattern for an Air Handler indoor unit
#define STORM_TYPE_SYSTEM_TYPE_AH_BITS          0x0000

/// Bit Pattern for an Furnace indoor unit
#define STORM_TYPE_SYSTEM_TYPE_FURN_BITS        0x0100

/// Bit pattern to get the ID Unit type
#define STORM_TYPE_SYSTEM_TYPE_IDUNIT_MASK      0x0F00


/// Bit Pattern for no indoor unit heating stages (only valid when idunit == AirHandler and no electric heat is installed)
#define STORM_TYPE_SYSTEM_TYPE_ID_NOSTAGES      0x0000

/// Bit Pattern for indoor unit with a single heating stage
#define STORM_TYPE_SYSTEM_TYPE_ID_STAGE1        0x1000

/// Bit Pattern for indoor unit with two heating stages
#define STORM_TYPE_SYSTEM_TYPE_ID_STAGE2        0x2000

/// Bit Pattern for indoor unit with three heating stages
#define STORM_TYPE_SYSTEM_TYPE_ID_STAGE3        0x3000

/// Bit pattern to get the number of ID stages
#define STORM_TYPE_SYSTEM_TYPE_ID_STAGE_MASK    0xF000

/// Number of bit-shifts for normalize the ID stages
#define STORM_TYPE_SYSTEM_TYPE_ID_STAGE_SHIFT_BITS  12


///
namespace Storm {
///
namespace Type {

/** The following enum/class defines the possible/supported system
    configurations.
    @code 

    Format of enum:
        <odtype>[<ostages>]_<idtype><istages>
        where:
            <odtype>    Type of Outdoor Unit
            <ostages>   Number of outdoor stages
            <idtype>    Type of Indoor Unit
            <istages>   Number of indoor stages

    @param SystemType                       - Class name for the System Type enum
    @param uint16_t                         - storage type of the enum
    @param eUNDEFINED                       - The system has not been configured and/or is an invalid/supported configuration
    @param eAC1_AH0                         - Single Stage AC with Air Handler with NO electric heat
    @param eAC1_FURN1                       - Single Stage AC with a Single Stage furnace
    @param eAC1_FURN2                       - Single Stage AC with a Two Stage furnace
    @param eAC1_FURN3                       - Single Stage AC with a Three Stage furnace
    @param eNONE_FURN1                      - Single stage furnace and NO outdoor unit
    @param eNONE_FURN2                      - Two stage furnace and NO outdoor unit
    @param eNONE_FURN3                      - Three stage furnace and NO outdoor unit
 
 @endcode
 */
BETTER_ENUM( SystemType, uint16_t,
             eUNDEFINED  = 0,
             eAC1_AH0    = STORM_TYPE_SYSTEM_TYPE_AC_BITS | STORM_TYPE_SYSTEM_TYPE_OD_STAGE1 | STORM_TYPE_SYSTEM_TYPE_AH_BITS | STORM_TYPE_SYSTEM_TYPE_ID_NOSTAGES,
             eAC1_FURN1  = STORM_TYPE_SYSTEM_TYPE_AC_BITS | STORM_TYPE_SYSTEM_TYPE_OD_STAGE1 | STORM_TYPE_SYSTEM_TYPE_FURN_BITS | STORM_TYPE_SYSTEM_TYPE_ID_STAGE1,
             eAC1_FURN2  = STORM_TYPE_SYSTEM_TYPE_AC_BITS | STORM_TYPE_SYSTEM_TYPE_OD_STAGE1 | STORM_TYPE_SYSTEM_TYPE_FURN_BITS | STORM_TYPE_SYSTEM_TYPE_ID_STAGE2,
             eAC1_FURN3  = STORM_TYPE_SYSTEM_TYPE_AC_BITS | STORM_TYPE_SYSTEM_TYPE_OD_STAGE1 | STORM_TYPE_SYSTEM_TYPE_FURN_BITS | STORM_TYPE_SYSTEM_TYPE_ID_STAGE3,
             eNONE_FURN1 = STORM_TYPE_SYSTEM_TYPE_NO_ODUNIT_BITS | STORM_TYPE_SYSTEM_TYPE_OD_NOSTAGES | STORM_TYPE_SYSTEM_TYPE_FURN_BITS | STORM_TYPE_SYSTEM_TYPE_ID_STAGE1,
             eNONE_FURN2 = STORM_TYPE_SYSTEM_TYPE_NO_ODUNIT_BITS | STORM_TYPE_SYSTEM_TYPE_OD_NOSTAGES | STORM_TYPE_SYSTEM_TYPE_FURN_BITS | STORM_TYPE_SYSTEM_TYPE_ID_STAGE2,
             eNONE_FURN3 = STORM_TYPE_SYSTEM_TYPE_NO_ODUNIT_BITS | STORM_TYPE_SYSTEM_TYPE_OD_NOSTAGES | STORM_TYPE_SYSTEM_TYPE_FURN_BITS | STORM_TYPE_SYSTEM_TYPE_ID_STAGE3
); 


};      // end namespaces
};
#endif  // end header latch
