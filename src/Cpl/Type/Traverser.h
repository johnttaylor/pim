#ifndef Cpl_Type_Traverser_h_
#define Cpl_Type_Traverser_h_
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



/// 
namespace Cpl {
/// 
namespace Type {

/** This class defines a set of type(s) that are used with
	the traverser design pattern.  The traverser pattern is an
	adaptation of a internal iterator.
 */
class Traverser
{
public:
	/** Return codes (for the callback method) that determine if the traversal
		should continue.
	 */
	enum Status_T {
		eABORT    = 0, //!< Indicates the client wants to cancel/terminate the traversal
		eCONTINUE = 1  //!< Indicates the client wants to continue the traversal
	};

public:
	/** This constructor does NOTHING.  It is work-around/hack to get Doxygen
		to properly document the enum above when there is ONLY a enum in the
		class.
	 */
	Traverser() {};
};

};      // end namespaces
};
#endif  // end header latch

