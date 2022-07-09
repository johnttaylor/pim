#ifndef Cpl_Io_Socket_Factory_h_
#define Cpl_Io_Socket_Factory_h_
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

#include "Cpl/Io/Socket/InputOutput.h"
#include "Cpl/Io/Descriptor.h"


///
namespace Cpl {
///
namespace Io {
///
namespace Socket {


/** This abstract class defines an interface for a Socket factory.  This
	class will create/destroy socket InputOutput streams.
 */
class Factory
{
public:
	/** This method creates an 'unactivated' Socket InputOutput stream from the
		supplied socket descriptor.  If there is an error (e.g. out-of-memory,
		invalid socket, etc.) then a null pointer is returned.  The caller is
		responsible for calling destroy() method once it is done with socket
		stream.
	 */
	virtual Cpl::Io::Socket::InputOutput* create() = 0;


	/** This method destroys an previously created socket InputOutput stream.
		The application is responsible for ensuing that ONLY InputOutput streams
		that where create using this factory are destroyed be this factory.
	 */
	virtual void destory( Cpl::Io::Socket::InputOutput& socketStream ) = 0;


public:
	/// Virtual destructor
	virtual ~Factory() {}
};


};      // end namespaces
};
};
#endif  // end header latch
