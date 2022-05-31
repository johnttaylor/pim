#ifndef Storm_Type_ComfortConfig_h_
#define Storm_Type_ComfortConfig_h_
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

#include "Storm/Type/Cph.h"
#include <stdint.h>

///
namespace Storm {
///
namespace Type {

/** The 'comfort' parameters per stage
 */
struct ComfortStageParameters_T
{
    uint32_t    minOnTime;   //!< Minimum on time in seconds
    uint32_t    minOffTime;  //!< Minimum off time in seconds
    int         cph;         //!< Cycle-Per-Hour settings.   The actual type is: Storm::Type::Cph ('betterenums' do not play well with classes/struct)

    /// Constructor (to ensure any pad bytes get zero'd)
    ComfortStageParameters_T()
    {
        memset( (void*) this, 0, sizeof( ComfortStageParameters_T ) );
    }

    /// Constructor (to ensure any pad bytes get zero'd)
    ComfortStageParameters_T( uint32_t minOnTime, uint32_t minOffTime, Storm::Type::Cph cph )
    {
        memset( (void*) this, 0, sizeof( ComfortStageParameters_T ) );
        this->minOffTime = minOffTime;
        this->minOnTime  = minOnTime;
        this->cph        = cph;
    }

    /// Copy Constructor (to ensure any pad bytes get zero'd)
    ComfortStageParameters_T( const ComfortStageParameters_T & other )
    {
        memcpy( (void*) this, (void*) &other, sizeof( ComfortStageParameters_T ) );
    }

    /// Copy operator
    ComfortStageParameters_T& operator =( const ComfortStageParameters_T & other )
    {
        memcpy( (void*) this, (void*) &other, sizeof( ComfortStageParameters_T ) );
        return *this;
    }



    /** Method that limit/range checks the specified STAGE configuration. Invalid
        and/or out-of-range value are corrected, Returns true if one or more
        fields were corrected.
     */
    static bool validate( ComfortStageParameters_T& src ) noexcept;
};

/** The 'comfort' parameters for the system
 */
struct ComfortConfig_T
{
    ComfortStageParameters_T compressorCooling;     //!< Parameters for compressor cooling operation
    ComfortStageParameters_T compressorHeating;     //!< Parameters for compressor heating operation
    ComfortStageParameters_T indoorHeating;         //!< Parameters for indoor heating operation

    /// Constructor (to ensure any pad bytes get zero'd)
    ComfortConfig_T()
    {
        memset( (void*) this, 0, sizeof( ComfortConfig_T ) );
    }

    /// Constructor (to ensure any pad bytes get zero'd)
    ComfortConfig_T( ComfortStageParameters_T compressorCooling, ComfortStageParameters_T compressorHeating, ComfortStageParameters_T indoorHeating )
    {
        memset( (void*) this, 0, sizeof( ComfortConfig_T ) );
        this->compressorCooling = compressorCooling;
        this->compressorHeating = compressorHeating;
        this->indoorHeating     = indoorHeating;
    }

    /** Method that limit/range checks the specified configuration. Invalid and/or
        out-of-range value are corrected, Returns true if one or more fields
        were corrected.
     */
    static bool validate( Storm::Type::ComfortConfig_T& src ) noexcept;
};




};      // end namespaces
};
#endif  // end header latch
