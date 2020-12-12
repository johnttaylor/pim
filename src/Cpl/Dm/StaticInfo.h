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
#include "Cpl/Json/Arduino.h"

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
    3) The concrete Model Point class implementation can safely 'down cast' 
       the Model Points' m_staticInfo reference to the concrete class type 
       from step 1.
    4) The child class can optionally support JSON formating (to be used 
       when the MP instance is serialized to a JSON string).  To support 
       JSON formatting a child class must override the hasJSONFormatter() 
       method and have it return true. And override the toJSON() method to 
       provide the actual JSON formating.

 */
class StaticInfo : public Cpl::Container::KeyLiteralString
{
public:
    /// Returns the Point's name
    inline const char* getName() const noexcept 
    {
        return m_stringKeyPtr;
    }

    /** Returns true if the instance has a JSON formatter.  The default is 
        not formatted.  A child class must override this method to add its
        type specific formatter.
     */
    virtual bool hasJSONFormatter() const noexcept 
    {
        return false;
    }

    /** This method is responsible for populating the provided JSON
        object instance.  The JSON format/key-value-pairs/content is
        child class specific.  The default implementation does NOTHING,
        the child classes are responsible for providing a meaningful
        implementation.
     */
    virtual void toJSON(JsonObject& dstObject) const noexcept {}

public:
    /// Constructor
    StaticInfo( const char* pointName ) : Cpl::Container::KeyLiteralString( pointName ) {}
};



};      // end namespaces
};
#endif  // end header latch
