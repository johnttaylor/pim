#ifndef Metrics_h_
#define Metrics_h_
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

#include <stdint.h>
#include <stdlib.h>
#include <memory.h>


/** The following typedef is used to store metrics values generated by the
    applications Algorithm.

    Note: The Data Model's default isDataEqual_() method used the memcmp()
          function.  In order to leverage the Data Model's default method with
          the Metrics_T data structure as a MP value - constructor/copy methods 
          are provided to ensure any pad bytes in the structure get zero'd so 
          that the memcmp() function behaves as expected.  An alternative to 
          this approach is for the Application to override the default 
          isDataEqual() method when defining a model point (e.g. see the 
          Cpl::Dm::Mp::Float and Cpl::Dm::Mp::Double classes).
 */
struct Metrics_T
{
    size_t   numSamples;    //!< Total Number of samples
    size_t   sumTotal;      //!< Cumulative summation of the values
    uint32_t maxValue;      //!< Largest value reported (by the driver)
    uint32_t minValue;      //!< Smallest value reported (by the driver)

    /// Constructor (to ensure any pad bytes get zero'd)
    Metrics_T()
    {
        memset( (void*) this, 0, sizeof( Metrics_T ) );
    }

    /// Copy Constructor (to ensure any pad bytes get zero'd)
    Metrics_T( const Metrics_T& other )
    {
        memcpy( (void*) this, (void*) &other, sizeof( Metrics_T ) );
    }

    /// Copy operator
    Metrics_T& operator =( const Metrics_T& other )
    {
        memcpy( (void*) this, (void*) &other, sizeof( Metrics_T ) );
        return *this;
    }
};

#endif  // end header latch
