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

#include <stdlib.h>

/// Provide a non-exception-throwing new/delete so exception code does NOT get
/// linked in (i.e. -fno-exception is NOT enough to get rid of exception code
/// from libstdc++.a)

void *operator new( size_t size ) throw() { return malloc(size); }

void operator delete( void* p ) throw() { free(p); }


/* The function __cxa_atexit() handles the static destructors. In a bare-metal 
   system this function can be empty because application has no operating system 
   to return to, and consequently the static destructors are never called.
*/
extern "C" int __cxa_atexit( void ) { return 0; }

//extern "C" int __cxa_atexit( void* object,
//                             void (*destructor)(void*),
//                             void* dso_handle
//                           )
//    {
//    return 0;
//    }



/* These functions (name managed C++ calls) deal with initializing IO
   streams object when static constructors are called.  Since I don't
   want to use IO Streams and IO streams pull in the all of the
   exception stuff -->this is my cheat/hack to  get around this
*/
extern "C" void _ZNSt8ios_base4InitC1Ev(void) {}

extern "C" void _ZNSt8ios_base4InitD1Ev(void) {}
