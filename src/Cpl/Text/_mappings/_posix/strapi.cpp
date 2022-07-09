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

#include <string.h>
#include <ctype.h>
#include "Cpl/Text/strapi.h"


//////////////
char* strupr_MAP( char *string )
{
	if ( string )
	{
		char* ptr = string;
		for ( ; *ptr; ptr++ )
		{
			*ptr = toupper( *ptr );
		}
	}
	return string;
}

char* strlwr_MAP( char *string )
{
	if ( string )
	{
		char* ptr = string;
		for ( ; *ptr; ptr++ )
		{
			*ptr = tolower( *ptr );
		}
	}
	return string;
}


//////////////
char* strnset_MAP( char *string, int c, size_t count )
{
	if ( string )
	{
		size_t i;
		char*  ptr = string;
		for ( i=0; i < count && *ptr; ptr++, i++ )
		{
			*ptr = (char) c;
		}
	}
	return string;
}


///////////////
char* strrev_MAP( char *string )
{
	if ( string )
	{
		size_t len = strlen( string );
		size_t i   = 0;
		char*  ptr = string;
		char*  end = string + len - 1;
		while ( i < len / 2 )
		{
			char temp = *end;
			*end      = *ptr;
			*ptr      = temp;
			ptr++;
			end--;
			i++;
		}
	}
	return string;
}


