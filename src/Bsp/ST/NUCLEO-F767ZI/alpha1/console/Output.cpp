/*-----------------------------------------------------------------------------
* This file is part of the Shift-Right Open Repository.  The Repository is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the root directory or on the Internet at
* http://www.shift-right.com/openrepo/license.htm
*
* Copyright (c) 2014-2022  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/

#include "Output.h"

static uint8_t                             txFifoMemory_[OPTION_BSP_CONSOLE_TX_FIFO_SIZE + 1];
static Cpl::Container::RingBuffer<uint8_t> txFifo_( sizeof( txFifoMemory_ ), txFifoMemory_ );

static uint8_t                             rxFifoMemory_[OPTION_BSP_CONSOLE_RX_FIFO_SIZE + 1];
static Cpl::Container::RingBuffer<uint8_t> rxFifo_( sizeof( rxFifoMemory_ ), rxFifoMemory_ );

Cpl::Io::Serial::ST::M32F4::InputOutput     g_bspConsoleStream( txFifo_, rxFifo_ );

