#ifndef Cpl_Io_Tcp_lwIP_Picow_Private_h_

#define Cpl_Io_Tcp_lwIP_Picow_Private_h_
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

#include "colony_config.h"
#include "lwip/pbuf.h"
#include "lwip/tcp.h"
#include "pico/sync.h"
#include "Cpl/System/Mutex.h"
#include <stdint.h>

/// Poll time in .5sec increment
#ifndef OPTION_CPL_IO_TCP_LWIP_PICOW_POLL_TICKS
#define	OPTION_CPL_IO_TCP_LWIP_PICOW_POLL_TICKS		1	
#endif

///
namespace Cpl {
///
namespace Io {
///
namespace Tcp {
///
namespace lwIP {
///
namespace Picow {


/** Defines the data/fields needed to manage a raw TCP connection.  The 
	Application SHOULD treat this an opaque structure, i.e. never directly
	operate on it.
 */
struct Socket_T
{
	struct tcp_pcb*		lwipPcb;				//!< Underlying lwIP Protocol Control block. If null, there is no connection or an error occurred
	struct pbuf*		recvPbuf;				//!< The PBUF pointer to the latest/current Receive data.  If null, then no incoming data.  Note when there is socket error, this field is set to null
	uint16_t			rxOffset;				//!< Starting offset when copying data from a PBUF into the client's buffer
	void*               connnectorClientPtr;	//!< Pointer to the Connector client.  This field has no meaning if the Socket was created by a 'Listener'
};

/** This method is PRIVATE to the lwIP namespace and should NOT be used
    by any module/class that is not part of the namespace.

	This method handles the 'data_sent' callback from lwIP.  

	Note: Depending on the 'architecture' implementation of lwIP this method
	      can be called from an Interrupt context.
 */
err_t lwipCb_dataSent_( void* arg, struct tcp_pcb* pcb, uint16_t len );

/** This method is PRIVATE to the lwIP namespace and should NOT be used
	by any module/class that is not part of the namespace.

	This method handles the 'data_recevied' callback from lwIP.

	Note: Depending on the 'architecture' implementation of lwIP this method
		  can be called from an Interrupt context.
 */
err_t lwipCb_dataReceived_( void* arg, struct tcp_pcb* pcb, struct pbuf* pbuf, err_t err );

/** This method is PRIVATE to the lwIP namespace and should NOT be used
	by any module/class that is not part of the namespace.

	This method handles the 'poll' callback from lwIP.

	Note: Depending on the 'architecture' implementation of lwIP this method
		  can be called from an Interrupt context.
 */
err_t lwipCb_poll_( void* arg, struct tcp_pcb* pcb );

/** This method is PRIVATE to the lwIP namespace and should NOT be used
	by any module/class that is not part of the namespace.

	This method handles the 'tcp error' callback from lwIP.

	Note: Depending on the 'architecture' implementation of lwIP this method
		  can be called from an Interrupt context.
 */
void lwipCb_error_( void* arg, err_t err );


/*---------------------------------------------------------------------------*/
/// Expose internal mutex when using: PICO_CYW43_ARCH_POLL
extern Cpl::System::Mutex g_internalLock;

};      // end namespaces
};
};
};
};
#endif  // end header latch
