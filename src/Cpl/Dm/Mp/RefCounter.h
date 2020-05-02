#ifndef Cpl_Dm_Mp_RefCounter_h_
#define Cpl_Dm_Mp_RefCounter_h_
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


#include "Cpl/Dm/Mp/Basic.h"

///
namespace Cpl {
///
namespace Dm {
///
namespace Mp {


/** This class provides a concrete implementation for a Point who's data is 32
    bit unsigned reference counter.  A reference counter can be increment or
    decremented.  When incremented, the value is clamped at 2^16 (not allowed
    to overflow).  When decremented, the value is clamped at zero (not allowed
    to underflow).  In addition, change notifications are only generated on the
    following transitions:

        1) Invalid state to the Valid state
        2) Valid state to the Invalid state
        3) Transition to zero
        4) Transition from zero to value greater than zero

    The toJSON/fromJSON() format is:
    \code

    { name:"<mpname>", type:"<mptypestring>", invalid:nn, seqnum:nnnn, locked:true|false, val:"[<act>]<numvalue>" }

        where <act> can be:
            "+"				-->increment the counter
            "-"				-->decrement the counter
            not specified	-->set the counter

        NOTE: The value for the "val" key/value pair is a STRING, NOT a numeric

        Examples:
            toJSON():
                { name:"mp_visitors", type:"Cpl::Dm::Mp::RefCounter", invalid:0, seqnum:12, locked:false, val:"12" }

            fromJSON():
                { name:"mp_visitors", val:"+2" }	// Increments the point by 2
                { name:"mp_visitors", val:"-1" }	// Decrements the point by 1
                { name:"mp_visitors", val:"0" }		// Resets the counter to zero

    \endcode


    NOTE: All methods in this class ARE thread Safe unless explicitly
          documented otherwise.
 */
class RefCounter : public Basic<uint32_t>
{
public:
    /// Constructor. Invalid MP
    RefCounter( Cpl::Dm::ModelDatabase& myModelBase, StaticInfo& staticInfo );

    /// Constructor. Valid MP.  Requires an initial value
    RefCounter( Cpl::Dm::ModelDatabase& myModelBase, StaticInfo& staticInfo, uint32_t initialValue );

public:
    /**  See Cpl::Dm::ModelPoint.
        Note: When the Model Point is invalidate, the internal counter is reset
              to zero, i.e. calling increment() when the Model Point is in the
              invalid state will result the counter being set to 1.
     */
    uint16_t setInvalidState( int8_t newInvalidState, LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

    /// Increments the counter.  Note: The counter is protected from overflowing
    uint16_t increment( uint32_t incrementAmount=1, LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

    /// Decrements the counter. Note: The counter is protected from underflowing
    uint16_t decrement( uint32_t decrementAmount=1, LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

    /// Resets the counter to zero (or to a specific value)
    virtual uint16_t reset( uint32_t newValue=0, LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

public:
    /// Type safe subscriber
    typedef Cpl::Dm::Subscriber<RefCounter> Observer;

    /// Type safe register observer
    virtual void attach( Observer& observer, uint16_t initialSeqNumber=SEQUENCE_NUMBER_UNKNOWN ) noexcept;

    /// Type safe un-register observer
    virtual void detach( Observer& observer ) noexcept;


public:
    /// See Cpl::Dm::Point.  
    bool toJSON( char* dst, size_t dstSize, bool& truncated, bool verbose=true ) noexcept;

    ///  See Cpl::Dm::ModelPoint.
    const char* getTypeAsText() const noexcept;

public:
    /// See Cpl::Dm::Point.  
    bool fromJSON_( JsonVariant& src, LockRequest_T lockRequest, uint16_t& retSequenceNumber, Cpl::Text::String* errorMsg ) noexcept;


private:
    /// Hide/block the inherited write method
    uint16_t write( uint32_t newValue, LockRequest_T lockRequest = eNO_REQUEST ) noexcept
    {
        return reset( newValue, lockRequest );
    }
};



};      // end namespaces
};
};
#endif  // end header latch
