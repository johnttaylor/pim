#ifndef Storm_Dm_MpModelNumString_h_
#define Storm_Dm_MpModelNumString_h_
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Apps Project.  The Colony.Apps Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.apps/license.txt
*
* Copyright (c) 2015-2020  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file */



#include "Cpl/Dm/Mp/String_.h"

/** Maximum file name length (in bytes not including the null terminator)
 */
#ifndef OPTION_STORM_DM_MAX_FILE_NAME
#define OPTION_STORM_DM_MAX_FILE_NAME     64
#endif

///
namespace Storm {
///
namespace Dm {


/** This class provides a concrete implementation for a Point who's data a
    null terminated string for storing a Mode Number

    The toJSON/fromJSON() format is:
    \code

    { name:"<mpname>", type:"<mptypestring>", valid:true|false, seqnum:nnnn, locked:true|false, val:"xyz.."} }

    \endcode


    NOTE: All methods in this class ARE thread Safe unless explicitly
          documented otherwise.
 */
class MpFileNameString : public Cpl::Dm::Mp::String_<OPTION_STORM_DM_MAX_FILE_NAME, MpFileNameString>
{
public:
    /** Constructor.  Invalid MP.
     */
    MpFileNameString( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName )
        : Cpl::Dm::Mp::String_<OPTION_STORM_DM_MAX_FILE_NAME, MpFileNameString>( myModelBase, symbolicName )
    {
    }

    ///  See Cpl::Dm::ModelPoint.
    const char* getTypeAsText() const noexcept
    {
        return "Storm::Dm::MpFileNameString";
    }

    /// Type safe subscriber
    typedef Cpl::Dm::Subscriber<MpFileNameString> Observer;

};



};      // end namespaces
};
#endif  // end header latch
