#ifndef Cpl_Dm_Mp_Bool_h_
#define Cpl_Dm_Mp_Bool_h_
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


/** This class provides a concrete implementation for a Point who's data is a
    bool.

 	The toJSON()/fromJSON format is:
	\code
	
	{ name:"<mpname>", type:"<mptypestring>", invalid:nn, seqnum:nnnn, locked:true|false, val:true|false }
	
	\endcode
	
	NOTE: All methods in this class ARE thread Safe unless explicitly
          documented otherwise.
 */
class Bool : public Basic<bool>
{
public:
    /// Constructor. Invalid MP. 
    Bool( Cpl::Dm::ModelDatabase& myModelBase, StaticInfo& staticInfo );

    /// Constructor. Valid MP.  Requires an initial value
    Bool( Cpl::Dm::ModelDatabase& myModelBase, StaticInfo& staticInfo, bool initialValue );

public:
    /// Type safe read. See Cpl::Dm::ModelPoint
    virtual int8_t read( bool& dstData, uint16_t* seqNumPtr=0 ) const noexcept;

    /// Type safe write. See Cpl::Dm::ModelPoint
    virtual uint16_t write( bool newValue, LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

    /// Type safe read-modify-write client callback interface
    typedef Cpl::Dm::ModelPointRmwCallback<bool> Client;

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
    typedef Cpl::Dm::Subscriber<Bool> Observer;

    /// Type safe register observer
    virtual void attach( Observer& observer, uint16_t initialSeqNumber=SEQUENCE_NUMBER_UNKNOWN ) noexcept;

    /// Type safe un-register observer
    virtual void detach( Observer& observer ) noexcept;

public:
	/// See Cpl::Dm::Point.  
	bool toJSON( char* dst, size_t dstSize, bool& truncated, bool verbose=true ) noexcept;

public:
    /// See Cpl::Dm::Point.  
    bool fromJSON_( JsonVariant& src, LockRequest_T lockRequest, uint16_t& retSequenceNumber, Cpl::Text::String* errorMsg ) noexcept;

    ///  See Cpl::Dm::ModelPoint.
    const char* getTypeAsText() const noexcept;

};



};      // end namespaces
};
};
#endif  // end header latch
