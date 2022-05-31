#ifndef Cpl_Catch_helpers_h_
#define Cpl_Catch_helpers_h_
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
/** @file

    This file defines helper functions/interface for writing unit tests
 */

#include "colony_config.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Dm/ModelPoint.h"


 /** Default Trace section used by/for the Helper methods
  */
#ifndef OPTION_CPL_CATCH_HELPERS_TRACE_SECT
#define OPTION_CPL_CATCH_HELPERS_TRACE_SECT     "_0test"
#endif

/*----------------------------------------------------------------------------*/
/** This function performs a blocking wait by monitoring a Model Point for
    a specific value.  The method waits a specified amount of time BEFORE
    checking for the expected value. The method returns true if the specified
    MP's values matches the expected value; else the method will eventually time
    out and return false.
 */
template<class MPTYPE, class VALTYPE>
bool minWaitOnModelPoint( MPTYPE& src, VALTYPE expectedVal, unsigned long minWait, VALTYPE* lastVal=nullptr, unsigned long maxWaitMs=10000, unsigned long waitDelayMs=10 )
{
    Cpl::System::Api::sleep( minWait );

    VALTYPE val;
    bool  valid = src.read( val );
    while ( maxWaitMs )
    {
        if ( valid && val == expectedVal )
        {
            // Return the last read value
            if ( lastVal )
            {
                *lastVal = val;
            }
            return true;
        }

        Cpl::System::Api::sleep( waitDelayMs );
        maxWaitMs -= waitDelayMs;
        valid      = src.read( val );
    }

    // Return the last read value
    if ( lastVal )
    {
        *lastVal = val;
    }
    return false;
};

/** This function is the same minWaitOnModelPoint() above, expect that it
    expected VALTYPE to be BETTER_NUM
 */
template<class MPTYPE, class VALTYPE>
bool minWaitOnEnumModelPoint( MPTYPE& src, VALTYPE expectedVal, unsigned long minWait, VALTYPE* lastVal=nullptr, unsigned long maxWaitMs=10000, unsigned long waitDelayMs=10 )
{
    Cpl::System::Api::sleep( minWait );

    VALTYPE val = VALTYPE::_from_integral_unchecked( 0 );
    bool  valid = src.read( val );
    while ( maxWaitMs )
    {
        if ( valid && val == expectedVal )
        {
            // Return the last read value
            if ( lastVal )
            {
                *lastVal = val;
            }
            return true;
        }

        Cpl::System::Api::sleep( waitDelayMs );
        maxWaitMs -= waitDelayMs;
        valid      = src.read( val );
    }

    // Return the last read value
    if ( lastVal )
    {
        *lastVal = val;
    }
    return false;
};

/** This method is similar to minWaitOnModelPoint<>(), except that it waits on
    valid/invalid state of the MP
 */
template<class MPTYPE>
bool minWaitOnModelPointValidState( MPTYPE& src, bool expectedValidState, unsigned long minWait, int8_t* lastVal=nullptr, unsigned long maxWaitMs=10000, unsigned long waitDelayMs=10 )
{
    Cpl::System::Api::sleep( minWait );

    bool state = !src.isNotValid();
    while ( maxWaitMs )
    {
        if ( state == expectedValidState )
        {
            // Return the last read state
            if ( lastVal )
            {
                *lastVal = state;
            }
            return true;
        }

        Cpl::System::Api::sleep( waitDelayMs );
        maxWaitMs -= waitDelayMs;
        state      = !src.isNotValid();
    }

    // Return the last read value
    if ( lastVal )
    {
        *lastVal = state;
    }
    return false;
};

#endif  // end header latch
