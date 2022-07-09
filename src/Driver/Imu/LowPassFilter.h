#ifndef Driver_Imu_LowPassFilter_h_
#define Driver_Imu_LowPassFilter_h_
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


namespace Driver {
namespace Imu {

/** This concrete template class provides a simple Low Pass filter for
    a single data stream (and/or single Axis of IMU data).
    See https://en.wikipedia.org/wiki/Low-pass_filter

    The template arguments:
        DATA_T      data type of the data being filtered
        FILTERED_T  data type of filtered data (i.e the output of filter). It
                    is also the data type use for all internal calculations.
 */
template <class DATA_T, class FILTERED_T>
class LowPassFilter
{
protected:
    /// The smoothing factor.  Range is 0 < m_alpha < 1
    FILTERED_T  m_alpha;

    /// Previous filtered value
    FILTERED_T  m_prevFiltered;

    /// Flag to track the first time the filter is being called
    bool        m_firstTime;

public:
    /** Constructor.  The 'alpha' parameter should be a value between 0 and 1
     */
    LowPassFilter( FILTERED_T alpha ) :m_alpha( alpha ), m_firstTime( true ) {}


public:
    /** Filters the incoming data.  This method should be called at regular
        (and fixed) time intervals
     */
    FILTERED_T filter( DATA_T rawData )
    {
        // No actual filtering occurs on the initial call -->just pass through the rawdata value
        if ( m_firstTime )
        {
            m_prevFiltered = rawData;
            m_firstTime    = false;
            return rawData;
        }

        // Apply the filter
        m_prevFiltered = m_prevFiltered + m_alpha * ((FILTERED_T) rawData - m_prevFiltered);
        return m_prevFiltered;
    }
};

};      // end Namespaces
};
#endif  // end Header latch