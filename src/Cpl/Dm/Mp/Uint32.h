#ifndef Cpl_Dm_Mp_Uint32_h_
#define Cpl_Dm_Mp_Uint32_h_
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
	uint32_t.

	The toJSON()/fromJSON format is:
		\code

		{ name:"<mpname>", type:"<mptypestring>", valid:true|false, seqnum:nnnn, locked:true|false, val:<numvalue> }

		where <numvalue> is integer numeric  

		\endcode

 NOTE: All methods in this class ARE thread Safe unless explicitly
		  documented otherwise.
 */
class Uint32 : public Numeric<uint32_t,Uint32>
{
public:
	/** Constructor. Invalid MP.  
	 */
	Uint32(Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName )
		: Numeric<uint32_t, Uint32>(myModelBase, symbolicName )
	{
	}

	/// Constructor. Valid MP.  Requires an initial value
	Uint32(Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName, uint32_t initialValue )
		: Numeric<uint32_t, Uint32>(myModelBase, symbolicName, initialValue)
	{
	}

public:
	/// Type safe subscriber
	typedef Cpl::Dm::Subscriber<Uint32> Observer;


public:
	///  See Cpl::Dm::ModelPoint.
	const char* getTypeAsText() const noexcept
	{
		return "Cpl::Dm::Mp::Uint32";
	}
};



};      // end namespaces
};
};
#endif  // end header latch
