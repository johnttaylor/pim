#ifndef Cpl_System_Signable_h_
#define Cpl_System_Signable_h_
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

#include "Cpl/Container/Item.h"


///
namespace Cpl {
///
namespace System {

/** This abstract class defines the interface by which a client
    can cause an object that is waiting-on-a-signal to become
    unblocked.
 */
class Signable : public Cpl::Container::Item
{
public:
    /// Virtual destructor
    virtual ~Signable() {};

public:
    /** This method is called by client to cause the server to become
        unblock and ready to run.  This method can ONLY be called from
        a thread context. The method returns zero on success.
     */
    virtual int signal( void ) noexcept =0;

    /** Same as signal(), EXCEPT this method can ONLY be called from
        supervisor mode and/or ISR contexts.  The method returns zero on
        success.
     */
    virtual int su_signal( void ) noexcept = 0;
};


};      // end namespaces
};
#endif  // end header latch
