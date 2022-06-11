#ifndef Driver_Imu_TransposeAxises_h_
#define Driver_Imu_TransposeAxises_h_
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

/** This template class defines the interface for transposes the XYZ axises vector
    data from an IMU.
 */
template <class T>
class TransposeAxises
{
public:
    /// Transposes the axises of specified vector.  Default is no transposition.
    virtual void transpose( Driver::Imu::Vector<T>& original ) {}

public:
    /// Virtual destructor
    virtual ~TransposeAxises() {}
};


};      // end Namespaces
};
#endif  // end Header latch
