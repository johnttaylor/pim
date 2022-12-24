#ifndef Cpl_TShell_Security_h_
#define Cpl_TShell_Security_h_
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
namespace TShell {

/** This abstract defines the interface for validating a 'user login' for
    TShell.

    NOTE: This class does NOT define any thread safety semantics - it is the
          responsibility of the Application to provide thread safety if it is
          needed.
 */

class Security
{
public:
    /// Permissions levels
    enum Permission_T {
        ePUBLIC = 0,            //!< No permissions, i.e. the 'users' has no privilege and/or the command requires NO permission to be executed
        eUSER   = 1,            //!< Lowest permission setting, i.e. a 'user' can only executes commands that have eUSER or ePUBLIC permission levels
        eSYSTEM = 2,            //!< Higher permission setting, i.e. a 'user' can only executes commands that have eSYSTEM, eUSER or ePUBLIC permission levels
        eADMIN  = 3             //!< Highest permission setting, i.e. a 'user' can executes any/all commands
    };


public:
    /** This method returns the Permission level for the specified user/password.
        If the user-name or password are not valid - then ePUBLIC is returned.
     */
    virtual Permission_T validateCredentials( const char* username, const char* password ) noexcept = 0;

public:
    /// Virtual destructor
    virtual ~Security(){}

};


};      // end namespaces
};
#endif  // end header latch
