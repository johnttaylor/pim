#ifndef Cpl_Type_SeqNumber_h_
#define Cpl_Type_SeqNumber_h_
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


/// 
namespace Cpl {
/// 
namespace Type {

/** This class defines a sequence number and the operation that can be performed
	on it.  A valid sequence number is positive integer that is greater 0.
	Sequence numbers are helpful for determining when something has changed,
	i.e. by comparing two sequence number -->if there are not equal then
	something has changed and/or is out-of-sequence.
 */
class SeqNumber
{
protected:
	/// Internal counter
	int32_t m_counter;

public:
	/// Constructor
	SeqNumber( int32_t initialValue=1 );


public:
	/** Sets the sequence number value.  If the value is negative or zero then
		no update is performed.
	 */
	void set( int32_t newValue );

	/// Assignment - does a true copy, i.e. will copy/assign the invalid state
	SeqNumber& operator=( const SeqNumber rvalue );

	/** This method increment the sequence number and ensure that the new
		value is valid.  Once the maximum range of the sequence number has
		been reach, the value rolls over to 1. Note: If the sequence is
		invalid at the time of this call, then the sequence number will
		be set to 1, i.e. will be VALID after the call..
	 */
	void increment( void );

	/** Forces the sequence number to be invalid.  This is useful to force
		the compare() operation to always return false.  To 'undo' the this
		method, call the increment method() or the set() method with a positive
		value greater than zero.
	 */
	void invalidate( void );

    /// Returns true if the sequence number is valid (i.e NOT in the invalid state)
    inline bool isValid( void ) const { return m_counter > 0;  }

    /// Cast to an int32_t 
    inline operator int32_t ( ) const { return m_counter; }

public:
    /** This method return true if 'other' matches this sequence number
        Note: If this instance or 'other' is invalid (aka zero), then the method 
        always returns false;
	 */
	bool compare( const SeqNumber& other ) const;

	/// Short hand for the compare() method
	inline bool operator == ( const SeqNumber& other ) const { return compare( other ); }

	/// Short hand for the !compare() method
	inline bool operator != ( const SeqNumber& other ) const { return !compare( other ); }

public:
    /** This method return true if 'other' matches this sequence number
        Note: If this instance or 'other' is invalid (aka zero), then the method 
        always returns false;
     */
    bool compare( int32_t other ) const;

    /// Short hand for the compare() method
    inline bool operator == ( int32_t other ) const { return compare( other ); }

    /// Short hand for the !compare() method
    inline bool operator != ( int32_t other ) const { return !compare( other ); }

};

};      // end namespaces
};
#endif  // end header latch

