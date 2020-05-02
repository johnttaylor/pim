#ifndef Cpl_Dm_Mp_ElapsedPrecisionTime_h_
#define Cpl_Dm_Mp_ElapsedPrecisionTime_h_
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
#include "Cpl/System/ElapsedTime.h"

///
namespace Cpl {
///
namespace Dm {
///
namespace Mp {


/** This class provides a concrete implementation for a Point who's data a
    Cpl::System::ElapsedTime::Precision_T data structure

    The toJSON/fromJSON() format is:
    \code

    { name:"<mpname>", type=:<mptypestring>", invalid:nn, seqnum:nnnn, locked:true|false, val:"DD HH:MM:SS.sss" }

    Note: When writing a value, the 'DD' and 'sss' fields are optional.

    \endcode


    NOTE: All methods in this class ARE thread Safe unless explicitly
          documented otherwise.
 */
class ElapsedPrecisionTime : public Basic<Cpl::System::ElapsedTime::Precision_T>
{
public:
    /// Constructor. Invalid MP
    ElapsedPrecisionTime( Cpl::Dm::ModelDatabase& myModelBase, StaticInfo& staticInfo );

    /// Constructor. Valid MP.  Requires an initial value
    ElapsedPrecisionTime( Cpl::Dm::ModelDatabase& myModelBase, StaticInfo& staticInfo, Cpl::System::ElapsedTime::Precision_T initialValue );
    

public:
    /// Type safe read-modify-write client callback interface
    typedef Cpl::Dm::ModelPointRmwCallback<Cpl::System::ElapsedTime::Precision_T> Client;

    /** Type safe read-modify-write. See Cpl::Dm::ModelPoint

       NOTE: THE USE OF THIS METHOD IS STRONGLY DISCOURAGED because it has
             potential to lockout access to the ENTIRE Model Base for an
             indeterminate amount of time.  And alternative is to have the
             concrete Model Point leaf classes provide the application
             specific read, write, read-modify-write methods in addition or in
             lieu of the read/write methods in this interface.
     */
    virtual uint16_t readModifyWrite( Client& callbackClient, LockRequest_T lockRequest = eNO_REQUEST );

public:
    /// Type safe subscriber
    typedef Cpl::Dm::Subscriber<ElapsedPrecisionTime> Observer;

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

};



};      // end namespaces
};
};
#endif  // end header latch
