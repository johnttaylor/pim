/*-----------------------------------------------------------------------------
* COPYRIGHT_HEADER_TO_BE_FILLED_LATER
*----------------------------------------------------------------------------*/
#include "integer.h"
#include "Cpl/System/Assert.h"


void Cpl::Math::incrementLargeInteger( uint8_t* integerToBeIncrement, int integerSizeInBytes, uint8_t incValue, bool littleEndian ) noexcept
{
    CPL_SYSTEM_ASSERT( integerToBeIncrement );
    CPL_SYSTEM_ASSERT( integerSizeInBytes >= 1 );

    // Deal with Big vs Little endian
    int idx       = 0;
    int direction = 1;
    int endIdx    = integerSizeInBytes - 1;
    if ( !littleEndian )
    {
        idx       = integerSizeInBytes - 1;
        direction = -1;
        endIdx    = 0;

    }

    // Loop through all of the bytes in the 'integer'
    for( ; idx != (endIdx+direction); idx += direction )
    {
        uint16_t result = (uint16_t) integerToBeIncrement[idx] + incValue;
        integerToBeIncrement[idx] = (uint8_t) result;
        if ( ( result >> 8 ) == 0 )
        {
            break;
        }
        incValue = (uint8_t) ( result >> 8 );
    }
}