#ifndef Cpl_Dm_Mp_Double_h_
#define Cpl_Dm_Mp_Double_h_
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
    double.

    The toJSON()/fromJSON format is:
    \code

    { name:"<mpname>", type:"<mptypestring>", valid:true|false, seqnum:nnnn, locked:true|false, val:<numvalue> }

    \endcode

    NOTE: All methods in this class ARE thread Safe unless explicitly
          documented otherwise.
 */
class Double : public Numeric<double, Double>
{
public:
    /** Constructor. Invalid MP.
     */
    Double( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName )
        : Numeric<double, Double>( myModelBase, symbolicName )
    {
    }

    /// Constructor. Valid MP.  Requires an initial value
    Double( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName, double initialValue )
        : Numeric<double, Double>( myModelBase, symbolicName, initialValue )
    {
    }

public:
    /// Type safe subscriber
    typedef Cpl::Dm::Subscriber<Double> Observer;

public:
    ///  See Cpl::Dm::ModelPoint.
    const char* getTypeAsText() const noexcept
    {
        return "Cpl::Dm::Mp::Double";
    }

protected:
    /// Override parent implementation for 'correct' floating point comparison
    bool isDataEqual_( const void* otherData ) const noexcept
    {
        double* other = (double*) otherData;
        return Cpl::Math::areDoublesEqual( m_data, *other );
    }
};



};      // end namespaces
};
};
#endif  // end header latch
