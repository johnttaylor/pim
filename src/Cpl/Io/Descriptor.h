#ifndef Cpl_Io_Descriptor_h_
#define Cpl_Io_Descriptor_h_
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
namespace Io {

/** This union defines a 'IO descriptor' in terms of a an integer
    and/or a void*.  This allows the concrete 'Standard IO' class some
    flexibility in their implementation.
 */
union Descriptor
{
public:
    /// Union the different possible types for a descriptor
    union
    {
        /// The traditional standard Posix file descriptor
        int   m_fd;

        /// Pointer implementation (such as FILE*)
        void* m_handlePtr;
    };

public:
    /// fd Constructor
    Descriptor( int fd ):m_fd( fd ) {}

    /// handle Constructor
    Descriptor( void* ptr ):m_handlePtr( ptr ) {}

    /// Default Constructor
    Descriptor():m_handlePtr( nullptr ) {}
};

};      // end namespaces
};
#endif  // end header latch
