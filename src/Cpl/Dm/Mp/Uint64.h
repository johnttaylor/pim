#ifndef Cpl_Dm_Mp_Uint64_h_
#define Cpl_Dm_Mp_Uint64_h_
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
	uint64_t.

	The toJSON()/fromJSON format is:
	\code

	{ name:"<mpname>", type:"<mptypestring>", invalid:nn, seqnum:nnnn, locked:true|false, val:<numvalue> }

	where <numvalue> is decimal numeric OR a quoted HEX string (when the MP
	instance was constructed with 'decimalFormat':=false).  For example:

	val:1234  or val:"4D2"

	\endcode

	NOTE: All methods in this class ARE thread Safe unless explicitly
		  documented otherwise.
 */
class Uint64 : public BasicInteger<uint64_t>
{
public:
	/** Constructor. Invalid MP.  Note: the 'decimalFormat' argument applies to the
		toString()/fromString() methods.   When set to true, the input/output
		values must be decimal numbers; else hexadecimal numbers.
	 */
	Uint64( Cpl::Dm::ModelDatabase& myModelBase, StaticInfo& staticInfo, bool decimalFormat=true )
		:BasicInteger<uint64_t>( myModelBase, staticInfo, decimalFormat )
	{
	}

	/// Constructor. Valid MP.  Requires an initial value
	Uint64( Cpl::Dm::ModelDatabase& myModelBase, StaticInfo& staticInfo, uint64_t initialValue, bool decimalFormat=true )
		:BasicInteger<uint64_t>( myModelBase, staticInfo, initialValue, decimalFormat )
	{
	}

public:
	/// Type safe read-modify-write client callback interface
	typedef Cpl::Dm::ModelPointRmwCallback<uint64_t> Client;

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
	typedef Cpl::Dm::Subscriber<Uint64> Observer;

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
		return m_decimal ? "Cpl::Dm::Mp::Uint64-dec" : "Cpl::Dm::Mp::Uint64-hex";
	}
};



};      // end namespaces
};
};
#endif  // end header latch
