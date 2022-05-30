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

#include "String.h"
#include <stdio.h>



////////////////////////////////////
namespace std {

ostream& operator<< ( ostream &out, const Cpl::Text::String& outstring )
{
	out << outstring.getString();
	return out;
}


istream& operator>> ( istream &in, Cpl::Text::String& instring )
{
	streamsize savelen = in.width();
	int        maxlen  = 0;
	char*      dstPtr  = instring.getBuffer( maxlen );

	in.width( maxlen + 1 );  // Add 1 because the 'width' parameter includes the null terminator
	in >> dstPtr;
	in.width( savelen );

	return in;
}

};