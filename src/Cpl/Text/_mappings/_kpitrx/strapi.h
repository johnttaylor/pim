#ifndef Cpl_Text_x_mappings_kpitrx_strapi_h_
#define Cpl_Text_x_mappings_kpitrx_strapi_h_
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
/** @file

	This file provides the mapping of the 'strapi' to the KPit GNURX C/C++ compiler.

	The GNURX compiler provides all of the strapi natively.

 */




 ///////////////////////////////////////////////////////////////////////////////

#include <string.h>


#include <stdio.h> // For sprintf

#define HAVE_CPL_TEXT_STRCASECMP
#define HAVE_CPL_TEXT_STRNCASECMP
#define HAVE_CPL_TEXT_STRTOK_R
#define HAVE_CPL_TEXT_SNPRINTF
#define HAVE_CPL_TEXT_VSNPRINTF      

#define strset_MAP(s,f)		strnset_MAP(s,f,strlen(s))

extern char* strupr_MAP( char* s1 );
extern char* strlwr_MAP( char* s1 );
extern char* strnset_MAP( char* s1, int fill, size_t len );
extern char* strrev_MAP( char* s1 );




#endif  // end header latch
