#ifndef Cpl_Dm_Mp_Void_h_
#define Cpl_Dm_Mp_Void_h_
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
	void pointer.  
	
	On general principle there should NOT be model point type that is generic 
	(it goes against the whole 'model-points-are-type-safe' thingy). HOWEVER, 
	there are some cases where it is desirable to have MP contain a pointer - 
	where the actual definition of what that pointer points to is 'not visible' 
	
	Shorter version: DO NOT USE THIS MODEL POINT TYPE UNLESS YOU HAVE NO 
	ALTERNATIVE. Not wanting to take the time to create a new model point type
	(with test code) is NOT an acceptable excuse for using this model point 
	type.

	The toJSON()/fromJSON format is:
		\code

		{ name:"<mpname>", type:"<mptypestring>", valid:true|false, seqnum:nnnn, locked:true|false, val:"<hexvalue>" }

		\endcode

 NOTE: All methods in this class ARE thread Safe unless explicitly
		  documented otherwise.
 */
class Void : public Pointer_<Void>
{
public:
    /// Constructor. Invalid MP. 
    Void( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName )
        :Pointer_<Void>( myModelBase, symbolicName )
    {
    }

    /// Constructor. Valid MP.  Requires an initial value
    Void( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName, void* initialValue )
        : Pointer_<Void>( myModelBase, symbolicName, initialValue )
    {
    }


public:
    /// Type safe read. See Cpl::Dm::ModelPoint
    inline bool read( void*& dstData, uint16_t* seqNumPtr = 0 ) const noexcept
    {
        return readData( &dstData, sizeof( void* ), seqNumPtr );
    }

    /// Type safe write. See Cpl::Dm::ModelPoint
    inline uint16_t write( void* newValue, Cpl::Dm::ModelPoint::LockRequest_T lockRequest = Cpl::Dm::ModelPoint::eNO_REQUEST ) noexcept
    {
        return writeData( &newValue, sizeof( void* ), lockRequest );
    }

public:
    /// Type safe subscriber
    typedef Cpl::Dm::Subscriber<Void> Observer;

    ///  See Cpl::Dm::ModelPoint.
    const char* getTypeAsText() const noexcept
    {
        return "Cpl::Dm::Mp::Void";
    }
};



};      // end namespaces
};
};
#endif  // end header latch
