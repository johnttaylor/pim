#ifndef Cpl_Dm_Mp_Float_h_
#define Cpl_Dm_Mp_Float_h_
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

    { name:"<mpname>", type:"<mptypestring>", valid:true|false, seqnum:nnnn, locked:true|false, val:<numvalue> }

    \endcode

    NOTE: All methods in this class ARE thread Safe unless explicitly
          documented otherwise.
 */
class Float : public Numeric<float, Float>
{
public:
    /** Constructor. Invalid MP.
     */
    Float( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName )
        : Numeric<float, Float>( myModelBase, symbolicName )
    {
    }

    /// Constructor. Valid MP.  Requires an initial value
    Float( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName, float initialValue )
        : Numeric<float, Float>( myModelBase, symbolicName, initialValue )
    {
    }

public:
    /// Type safe subscriber
    typedef Cpl::Dm::Subscriber<Float> Observer;


public:
    ///  See Cpl::Dm::ModelPoint.
    const char* getTypeAsText() const noexcept
    {
        return "Cpl::Dm::Mp::Float";
    }

protected:
    /// Override parent implementation for 'correct' floating point comparison
    bool isDataEqual_( const void* otherData ) const noexcept
    {
        float* other = (float*) otherData;
        return Cpl::Math::areFloatsEqual( m_data, *other );
    }
};



};      // end namespaces
};
};
#endif  // end header latch
