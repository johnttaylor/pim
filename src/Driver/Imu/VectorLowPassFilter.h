#ifndef Driver_Imu_VectorLowPassFilter_h_
#define Driver_Imu_VectorLowPassFilter_h_
/*-----------------------------------------------------------------------------
* This file is part of the Arduino Project.  The Arduino Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/arduino/license.txt
*
* Copyright (c) 2017  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file */

#include "Driver/Imu/VectorFilter.h"
#include "Driver/Imu/LowPassFilter.h"
#include <stdint.h>


namespace Driver {
namespace Imu {

/** This concrete class provides a basic Low Pass Filter algorithm across all
    axises of the Vector.

        The template arguments:
        DATA_T      data type of the vector data being filtered
        FILTERED_T  data type of filtered data (i.e the output of filter). It
                    is also the data type use for all internal calculations.
 */
template <class DATA_T, class FILTERED_T>
class VectorLowPassFilter : public VectorFilter<DATA_T>
{
protected:
    /// Low Pass filter for the X-Axis
    Driver::Imu::LowPassFilter<DATA_T, FILTERED_T> m_filterX;

    /// Low Pass filter for the Y-Axis
    Driver::Imu::LowPassFilter<DATA_T, FILTERED_T> m_filterY;

    /// Low Pass filter for the Z-Axis
    Driver::Imu::LowPassFilter<DATA_T, FILTERED_T> m_filterZ;

public:

    /** Constructor.  The 'alpha' arguments are the smoothing factors for
        each axis.
     */
    VectorLowPassFilter( FILTERED_T alphaX, FILTERED_T alphaY, FILTERED_T alphaZ )
        : m_filterX( alphaX )
        , m_filterY( alphaY )
        , m_filterZ( alphaZ )
    {
    }

public:
    /// See Filter
    Driver::Imu::Vector<DATA_T> filterValue( const Driver::Imu::Vector<DATA_T>& rawValue )
    {
        Driver::Imu::Vector<T> result;

        result.x = m_filterX.filter( rawValue.x );
        result.y = m_filterY.filter( rawValue.y );
        result.z = m_filterZ.filter( rawValue.z );

        return result;
    }
};


};      // end Namespaces
};
#endif  // end Header latch