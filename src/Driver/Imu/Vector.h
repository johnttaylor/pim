#ifndef Driver_Imu_Vector_h_
#define Driver_Imu_Vector_h_
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

/** This template class contains the vector results from an IMU.
    For methods that use array type of notation the following mapping
    applies:
        [0] = X-Axis
        [1] = Y-Axis
        [2] = Z-Axis
 */
template<class T>
class Vector
{
public:
    /// Constructor
    Vector( T initX=0, T initY=0, T initZ=0 ):x(initX),y(initY),z(initZ){}

    /// Assignment operator
    Vector<T>& operator=(const Vector<T>& src)
    {
        x = src.x;
        y = src.y;
        z = src.z;

        return *this;
    }

public:
    /// Access by index (read operation, e.g. a = myvector[0])
    T operator [](int n) const
    {
        if ( n == 0 )
        {
            return x;
        }
        else if ( n == 1 )
        {
            return y;
        }
        else
        {
            return z;
        }
    }

    /// Access by index (write operation, e.g. myvector[0] = 12)
    T& operator [](int n)
    {
        if ( n == 0 )
        {
            return x;
        }
        else if ( n == 1 )
        {
            return y;
        }
        else
        {
            return z;
        }
    }
    
    /// Access by index  (read)
    T operator ()(int n) const
    {
        return this[n];
    }

    /// Access by index  (write)
    T& operator ()(int n) 
    {
        return this[n];
    }

    /// Copies the content of the Vector into the provided array
    void toArray( T dst[3] ) const
    {
        dst[0] = x;
        dst[1] = y;
        dst[2] = z;
    }

public:
    /// X-Axis
    T   x;

    /// Y-Axis
    T   y;

    /// Z-Axis
    T   z;

};

/** This template class contains the quaternion results from an IMU.
    For methods that use array type of notation the following mapping
    applies:
        [0] = W
        [1] = X-Axis
        [2] = Y-Axis
        [3] = Z-Axis
 */
template<class T>
class Quat
{
public:
    /// Constructor
    Quat( T initW=0, T initX=0, T initY=0, T initZ=0 ):x(initX),y(initY),z(initZ){}

    /// Assignment operator
    Quat<T>& operator=(const Quat<T>& src)
    {
        w = src.w;
        x = src.x;
        y = src.y;
        z = src.z;

        return *this;
    }

public:
    /// Access by index (read operation, e.g. a = myquat[0])
    T operator [](int n) const
    {
        if ( n == 0 )
        {
            return w;
        }
        else if ( n == 1 )
        {
            return x;
        }
        else if ( n == 2 )
        {
            return y;
        }
        else 
        {
            return z;
        }
    }

    /// Access by index (write operation, e.g. myquat[0] = 12)
    T& operator [](int n) 
    {
        if ( n == 0 )
        {
            return w;
        }
        else if ( n == 1 )
        {
            return x;
        }
        else if ( n == 2 )
        {
            return y;
        }
        else 
        {
            return z;
        }
    }

    /// Access by index (read)
    T operator ()(int n) const
    {
        return this[n];
    }

    /// Access by index (write)
    T& operator ()(int n) 
    {
        return this[n];
    }

    /// Copies the content of the Quat into the provided array
    void toArray( T dst[4] ) const
    {
        dst[0] = w;
        dst[1] = x;
        dst[2] = y;
        dst[3] = z;
    }

public:
    /// W
    T   w;

    /// X-Axis
    T   x;

    /// Y-Axis
    T   y;

    /// Z-Axis
    T   z;
};


};      // end Namespaces
};
#endif  // end Header latch
