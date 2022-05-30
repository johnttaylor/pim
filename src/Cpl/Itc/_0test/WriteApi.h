#ifndef WriteApi_h_
#define WriteApi_h_
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
/** @file */



/** This abstract class defines a synchronous 'write' interface to set new
    value in the 'Model'
 */
class WriteApi
{
public:
    /// Synchronously writes a new value to the model
    virtual void write( int newValue ) noexcept = 0;


public:
    /// Virtual destructor
    virtual ~WriteApi() {}
};


#endif  // end header latch
