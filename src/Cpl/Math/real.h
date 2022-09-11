#ifndef Cpl_Math_real_h_
#define Cpl_Math_real_h_
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
/** @file

	This file contains a collection of methods comparing, manipulating, etc.
	floating point numbers (i.e. floats and doubles).

*/

#include "colony_config.h"
#include <cmath>
#include <cfloat>

#undef abs

/** This symbols provides the default Epsilon value when testing for 'almost
	equal' between to float numbers.  Note: This is a GLOBAL setting.
 */
#ifndef CPL_MATH_REAL_FLOAT_EPSILON
#define CPL_MATH_REAL_FLOAT_EPSILON     (FLT_EPSILON)
#endif

 /** This symbols provides the default Epsilon value when testing for 'almost
	 equal' between to double numbers.  Note: This is a GLOBAL setting.
  */
#ifndef CPL_MATH_REAL_DOUBLE_EPSILON
#define CPL_MATH_REAL_DOUBLE_EPSILON    (DBL_EPSILON)
#endif



///
namespace Cpl {
///
namespace Math {


/** This template function implements a 'almost equals' comparison function for
	floating points numbers.  See the following link for why an 'almost equals'
	function is necessary:

	http://www.cygnus-software.com/papers/comparingfloats/comparingfloats.htm
 */
template <class T>
bool almostEquals( T a, T b, T epsilon )
{
	// Filter out NaN (always return false since by the IEEE standard NaN != NaN)
	if ( std::isnan( a ) || std::isnan( b ) )
	{
		return false;
	}

	// Try the 'quick' case (also handles infinites)
	if ( a == b )
	{
		return true;
	}


	// Handle the case of one of the arguments being zero or close to zero
	T diff = std::abs( a - b );
	if ( ( a == (T) ( 0.0 ) || b == (T) ( 0.0 ) ) && diff < epsilon )
	{
		return diff < ( epsilon * epsilon );
	}

	// Do a relative error check
	T absA = std::abs( a );
	T absB = std::abs( b );
	return ( diff / ( absA + absB ) ) < epsilon;
}


/** This method is short hand for almostEquals<float> AND provides a default epsilon
 */
inline bool areFloatsEqual( float a, float b, float epsilon = CPL_MATH_REAL_FLOAT_EPSILON )
{
	return almostEquals<float>( a, b, epsilon );
}


/** This method is short hand for almostEquals<double> AND provides a default epsilon
 */
inline bool areDoublesEqual( double a, double b, double epsilon = CPL_MATH_REAL_DOUBLE_EPSILON )
{
	return almostEquals<double>( a, b, epsilon );
}


};      // end namespaces
};
#endif  // end header latch
