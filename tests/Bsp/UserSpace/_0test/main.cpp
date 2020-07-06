#include "Bsp/Api.h"


// UNIT TEST -->tests that the generic API compiles & link

int main( int argc, char* argv[] )
{
	/// Initialize the BSP
	Bsp_Api_initialize();


	// Call generic APIs
	Bsp_Api_disableIrqs();
	Bsp_Api_enableIrqs();
	Bsp_Api_pushAndDisableIrqs();
	Bsp_Api_popIrqs();
	Bsp_Api_nop();
	Bsp_Api_turnOn_debug1();
	Bsp_Api_turnOff_debug1();
	Bsp_Api_toggle_debug1();
	Bsp_Api_turnOn_debug2();
	Bsp_Api_turnOff_debug2();
	Bsp_Api_toggle_debug2();

	// Return 'pass'
	return 0;
}


