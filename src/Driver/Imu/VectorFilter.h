#ifndef Driver_Imu_VectorFilter_h_
#define Driver_Imu_VectorFilter_h_
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

/** This pure virtual template class define the interface for applying a filter
    to IMU Vector
 */
template <class T>
class VectorFilter
{
public:
    /** Executes the filter algorithm and returns the 'filtered' value. The
        assumption is that this method is called on every raw sample and that
        is called on a fixed frequency.
     */
    virtual Driver::Imu::Vector<T> filterValue( const Driver::Imu::Vector<T>& rawValve ) = 0;

public:
    /// Virtual destructor
    virtual ~VectorFilter() {}
};

};      // end Namespaces
};
#endif  // end Header latch