#ifndef Cpl_Text_x_mappings_vc12_strapi_h_
#define Cpl_Text_x_mappings_vc12_strapi_h_
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
/** @file

	This file provides the mapping of the 'strapi' to Microsoft's VC12 C/C++ compiler.

	The VC12 compiler provides all of the strapi - just with the 'wrong names'

 */




 ///////////////////////////////////////////////////////////////////////////////

#include <string.h>
#include <stdio.h>

///
#define strcasecmp_MAP          _stricmp
///
#define strncasecmp_MAP         _strnicmp
///
#define strupr_MAP              _strupr
///
#define strlwr_MAP              _strlwr
///
#define strset_MAP              _strset
///
#define strnset_MAP             _strnset
///
#define strrev_MAP              _strrev
///
#define strtok_r_MAP(s,d,p)     strtok(s,d)     // NOTE: strtok() under windows IS Thread safe (but not "interleave" safe inside a single thread)
///
#define snprintf_MAP            _snprintf       // Note: Does NOT guarantee that the final string is ALWAYS null terminated.
///
#define vsnprintf_MAP           _vsnprintf




#endif  // end header latch
