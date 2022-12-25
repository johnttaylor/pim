#ifndef Cpl_TShell_SecurityNull_h_
#define Cpl_TShell_SecurityNull_h_
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

#include "Cpl/TShell/Security.h"
#include <string.h>

///
namespace Cpl {
///
namespace TShell {

/** This concrete class dummy/null implementation of the Security interface
 */

class SecurityNull : public Security
{
public:
	/// Constructor
	SecurityNull(){}

public:
	/// See Cpl::TShell::Security
	Permission_T validateCredentials( const char* username, const char* password ) noexcept
	{
		if ( strcmp( username, "user" ) == 0 )
		{
			return eUSER;
		}
		if ( strcmp( username, "system" ) == 0 )
		{
			return eSYSTEM;
		}
		if ( strcmp( username, "admin" ) == 0 )
		{
			return eADMIN;
		}
		return ePUBLIC;
	}
};


}		// end namespaces
}
#endif  // end header latch
