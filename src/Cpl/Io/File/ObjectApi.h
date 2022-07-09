
#ifndef Cpl_Io_File_ObjectApi_h_
#define Cpl_Io_File_ObjectApi_h_
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



///
namespace Cpl {
///
namespace Io {
///
namespace File {



/** This abstract class defines the basic additional functionality,
    beyond that of a stream, for a file.
 */

class ObjectApi
{
public:
    /** After a read/write operation this method returns true if
        the file pointer is at EOF.

        NOTES: 
        - This method is ONLY VALID immediately following a read/write 
          operation!
        - If the File object has been closed, this method will return true.
     */
    virtual bool isEof() = 0;

    /** Returns the length, in bytes, of the file. If there is
        an error than false is returned.
     */
    virtual bool length( unsigned long& length ) = 0;


public:
    /** Returns the current file pointer offset, in bytes, from
        the top of the file.  If there is an error than false is
        returned.
     */
    virtual bool currentPos(unsigned long& currentPos ) = 0;

    /** Adjusts the current pointer offset by the specified delta (in bytes).
        Returns true if successful, else false (i.e. setting the pointer
        past/before the file boundaries).
     */
    virtual bool setRelativePos( long deltaOffset ) = 0;

    /** Sets the file pointer to the absolute specified offset (in bytes).
        Returns true if successful, else false (i.e. setting the
        pointer past the end of the file).
     */
    virtual bool setAbsolutePos( unsigned long newoffset ) = 0;

    /** Sets the file pointer to End-Of-File.  Returns true  if
        successful, else false if an error occurred.
     */
    virtual bool setToEof() = 0;


public:
    /// Virtual destructor
    virtual ~ObjectApi() {}

};


};      // end namespaces
};
};
#endif  // end header latch

