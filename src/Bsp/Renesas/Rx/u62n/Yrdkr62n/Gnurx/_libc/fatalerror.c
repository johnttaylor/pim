/*----------------------------------------------------------------------------- 
* This file is part of the Colony.Core Project.  The Colony.Core Project is an   
* open source project with a BSD type of licensing agreement.  See the license  
* agreement (license.txt) in the top/ directory or on the Internet at           
* http://integerfox.com/colony.core/license.txt
*                                                                               
* Copyright (c) 2014, 2015  John T. Taylor                                        
*                                                                               
* Redistributions of the source code must retain the above copyright notice.    
*----------------------------------------------------------------------------*/ 

#include "Bsp/Api.h"
#include "syscall.h"

//////////////////////////////////////////////////////////////////////////////
/** This method implements a 'fatal error' when a method from libc that is 
    os/system depedent does not exist since I am not running an operating 
    system or my operating system does not provide an implementation
    for these methods (in newlib terms these methods would be methods in
    'libgloss')

    See the 'syscalls.h' header file for possible values of 'n'
 */

void bsp_libfake_fatalerror( int n )
    {
    // Essential freeze the application to get someone attention!
    Bsp_Api_disableIrqs();
    for(;;)
        ;
    }

