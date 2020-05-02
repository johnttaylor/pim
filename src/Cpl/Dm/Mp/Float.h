#ifndef Cpl_Dm_Mp_Float_h_
#define Cpl_Dm_Mp_Float_h_
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
#include "Cpl/Math/real.h"

///
namespace Cpl {
///
namespace Dm {
///
namespace Mp {


/** This class provides a concrete implementation for a Point who's data is a
	float.

	The toJSON()/fromJSON format is:
	\code

	{ name:"<mpname>", type:"<mptypestring>", invalid:nn, seqnum:nnnn, locked:true|false, val:<numvalue> }

	\endcode

	NOTE: All methods in this class ARE thread Safe unless explicitly
		  documented otherwise.
 */
class Float : public BasicReal<float>
{
public:
	/// Constructor. Invalid MP. 
	Float( Cpl::Dm::ModelDatabase& myModelBase, StaticInfo& staticInfo )
		:BasicReal<float>( myModelBase, staticInfo )
	{
	}

	/// Constructor. Valid MP.  Requires an initial value
	Float( Cpl::Dm::ModelDatabase& myModelBase, StaticInfo& staticInfo, float initialValue )
		:BasicReal<float>( myModelBase, staticInfo, initialValue )
	{
	}

public:
	/// Type safe read-modify-write client callback interface
	typedef Cpl::Dm::ModelPointRmwCallback<float> Client;

	/** Type safe read-modify-write. See Cpl::Dm::ModelPoint

	   NOTE: THE USE OF THIS METHOD IS STRONGLY DISCOURAGED because it has
			 potential to lockout access to the ENTIRE Model Base for an
			 indeterminate amount of time.  And alternative is to have the
			 concrete Model Point leaf classes provide the application
			 specific read, write, read-modify-write methods in addition or in
			 lieu of the read/write methods in this interface.
	 */
	virtual uint16_t readModifyWrite( Client& callbackClient, LockRequest_T lockRequest = eNO_REQUEST )
	{
		return ModelPointCommon_::readModifyWrite( callbackClient, lockRequest );
	}


public:
	/// Type safe subscriber
	typedef Cpl::Dm::Subscriber<Float> Observer;

	/// Type safe register observer
	virtual void attach( Observer& observer, uint16_t initialSeqNumber=SEQUENCE_NUMBER_UNKNOWN ) noexcept
	{
		ModelPointCommon_::attach( observer, initialSeqNumber );
	}

	/// Type safe un-register observer
	virtual void detach( Observer& observer ) noexcept
	{
		ModelPointCommon_::detach( observer );
	}


public:
	///  See Cpl::Dm::ModelPoint.
	const char* getTypeAsText() const noexcept
	{
		return "Cpl::Dm::Mp::Float";
	}

protected:
	/// See Cpl::Dm::ModelPoint.  Note: Use the system wide default epsilon of CPL_MATH_REAL_FLOAT_EPSILON when testing for equality
	bool isDataEqual_( const void* otherData ) const noexcept
	{
		float left = *( (float*) otherData );
		return Cpl::Math::areFloatsEqual( m_data, left );
	}
};



};      // end namespaces
};
};
#endif  // end header latch
