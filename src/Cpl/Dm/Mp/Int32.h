#ifndef Cpl_Dm_Mp_Int32_h_
#define Cpl_Dm_Mp_Int32_h_
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2022  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file */


#include "Cpl/Dm/Mp/Numeric.h"

///
namespace Cpl {
///
namespace Dm {
///
namespace Mp {


/** This class provides a concrete implementation for a Point who's data is a
	int32_t.

	The toJSON()/fromJSON format is:
		\code

		{ name:"<mpname>", type:"<mptypestring>", valid:true|false, seqnum:nnnn, locked:true|false, val:<numvalue> }

		where <numvalue> is integer numeric

		\endcode

 NOTE: All methods in this class ARE thread Safe unless explicitly
		  documented otherwise.
 */
class Int32 : public Numeric<int32_t, Int32>
{
public:
	/** Constructor. Invalid MP.
	 */
	Int32( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName )
		: Numeric<int32_t, Int32>( myModelBase, symbolicName )
	{
	}

	/// Constructor. Valid MP.  Requires an initial value
	Int32( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName, int32_t initialValue )
		: Numeric<int32_t, Int32>( myModelBase, symbolicName, initialValue )
	{
	}

public:
	/// Type safe subscriber
	typedef Cpl::Dm::Subscriber<Int32> Observer;


public:
	///  See Cpl::Dm::ModelPoint.
	const char* getTypeAsText() const noexcept
	{
		return "Cpl::Dm::Mp::Int32";
	}
};



};      // end namespaces
};
};
#endif  // end header latch
