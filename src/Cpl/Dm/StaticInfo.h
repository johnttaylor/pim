#ifndef Cpl_Dm_StaticInfo_h_
#define Cpl_Dm_StaticInfo_h_
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


#include "Cpl/Container/Key.h"

///
namespace Cpl {
///
namespace Dm {

/** This symbol defines the size, in bytes not including the null terminator, 
    of the longest Model Point name.
 */
#ifndef OPTION_CPL_DM_STATIC_INFO_MAX_NAME_LENGTH
#define OPTION_CPL_DM_STATIC_INFO_MAX_NAME_LENGTH   32
#endif

/** This class defines an interface for a Model Point to store its constant/static 
    data in program memory.  Concrete leaf classes can extended this class
    to provide additional constant parameters.  

    How to extend:
    1) A new child class is defined and must inherit from StaticInfo.  The new 
       class contains additional static parameters (e.g. Units, Range limits,
       Network Name, etc.) per the needs of the Application.
    2) The constructor for a concrete Model Point class takes a static reference to 
       the class defined in step 1.
    3) The concrete Model Point class implementation can safely 'down cast' the
       Model Points' m_staticInfo reference to the concrete class type from
       step 1.
 */
class StaticInfo : public Cpl::Container::KeyLiteralString
{
public:
    /// Returns the Point's name
    inline const char* getName() const noexcept 
    {
        return m_stringKeyPtr;
    }


public:
    /// Constructor
    StaticInfo( const char* pointName ) : Cpl::Container::KeyLiteralString( pointName ) {}
};



};      // end namespaces
};
#endif  // end header latch
