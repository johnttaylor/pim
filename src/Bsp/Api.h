#ifndef Bsp_Api_h_
#define Bsp_Api_h_
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2019  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file

	This file defines the common/generic interfaces that all Colony.* BSP
	are required to support.  Note: Many of the common interfaces are defined
	as C preprocessor macros - this provides the maximum flexibility in
	implementation for a concrete BSP.  Only functions that 'should be' or
	need to be true function calls are defined in this class.
 */


#include "colony_config.h"
#include "colony_map.h"  


// Start C++
#ifdef __cplusplus
extern "C" {
#endif


/////////////////////////////////////////////////////////////////////////////
//

/** This method will initialize the board/low level hardware such that BASIC
	board operation is possible. Additional initialization of peripherals may
	be required - see your specific BSP header file for details.

	NOTE: Since this method is intended to be called by the Application - it
		  is 'run' AFTER main() has been entered.
 */
void Bsp_Api_initialize( void );


	/** This method inserts a single 'nop' instruction

		\b Prototype:
			void Bsp_Api_nop(void);
	 */
#define Bsp_Api_nop                         Bsp_Api_nop_MAP


 /////////////////////////////////////////////////////////////////////////////
 //
 /** This method will disable ALL interrupts, i.e manipulate the global
	 enable/disable flag in the PSW.

	 \b Prototype:
		 void Bsp_Api_disableIrqs( void );
  */
#define Bsp_Api_disableIrqs                 Bsp_Api_disableIrqs_MAP


  /** This method will enable ALL interrupts, i.e manipulate the global
	  enable/disable flag in the PSW.

	  \b Prototype:
		  void Bsp_Api_enableIrqs( void );
   */
#define Bsp_Api_enableIrqs                  Bsp_Api_enableIrqs_MAP


   /** This method will push the current IRQ enabled/disabled flags onto
	   the stack and then disable ALL interrupts. This method should all
	   be called in 'pairs' with the Bsp_Api_popIrqs() method.

	   NOTE: This method MAY push the CPU's entire Program Status Word (PSW) onto
			 the stack and the corresponding Bsp_Api_popIrq() will restore the
			 ENTIRE PSW word.  What does this mean - usually nothing but things
			 like the zero, carry, overflow flags, etc. are stored in the PSW and
			 we be reverted (when 'pop' is called) to the state of when the 'push'
			 was done.

	   \b Prototype:
		   void Bsp_Api_pushAndDisableIrqs( void );
	*/
#define Bsp_Api_pushAndDisableIrqs          Bsp_Api_pushAndDisableIrqs_MAP


	/** This method will pop/update the IRQ enabled/disabled flags from
		a previously push IRQ state value.  This method should all
		be called in 'pairs' with the Bsp_Api_pushAndDisableIrqs() method.

		\b Prototype:
			void Bsp_Api_popIrqs( void );
	 */
#define Bsp_Api_popIrqs                     Bsp_Api_popIrqs_MAP


/////////////////////////////////////////////////////////////////////////////
//
/** This method will turn on "Debug LED 1".  The concept is that every board
	has one or more LEDs, IO Pins, etc. that can be used for debugging purposes.

	\b Prototype:
		void Bsp_Api_turnOn_debug1( void );
 */
#define Bsp_Api_turnOn_debug1               Bsp_Api_turnOn_debug1_MAP     


/** This method will turn off "Debug LED 1".  The concept is that every board
	has one or more LEDs, IO Pins, etc. that can be used for debugging purposes.

	\b Prototype:
		void Bsp_Api_turnOff_debug1( void );
 */
#define Bsp_Api_turnOff_debug1              Bsp_Api_turnOff_debug1_MAP

/** This method will toggle "Debug LED 1".  The concept is that every board
	has one or more LEDs, IO Pins, etc. that can be used for debugging purposes.

	\b Prototype:
		void Bsp_Api_toggle_debug1( void );
 */
#define Bsp_Api_toggle_debug1               Bsp_Api_toggle_debug1_MAP



/** This method will turn on "Debug LED 2".  The concept is that every board
	has one or more LEDs, IO Pins, etc. that can be used for debugging purposes.

	\b Prototype:
		void Bsp_Api_turnOn_debug2( void );
 */
#define Bsp_Api_turnOn_debug2               Bsp_Api_turnOn_debug2_MAP     


/** This method will turn off "Debug LED 2".  The concept is that every board
	has one or more LEDs, IO Pins, etc. that can be used for debugging purposes.

	\b Prototype:
		void Bsp_Api_turnOff_debug2( void );
 */
#define Bsp_Api_turnOff_debug2              Bsp_Api_turnOff_debug2_MAP

/** This method will toggle "Debug LED 2".  The concept is that every board
	has one or more LEDs, IO Pins, etc. that can be used for debugging purposes.

	\b Prototype:
		void Bsp_Api_toggle_debug2( void );
 */
#define Bsp_Api_toggle_debug2               Bsp_Api_toggle_debug2_MAP



// End C++
#ifdef __cplusplus
};
#endif
#endif  // end header latch
