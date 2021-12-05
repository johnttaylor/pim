#include "Bsp/Api.h"
#include "FreeRtos.h"
#include "task.h"
#include <new>
#include <stdlib.h>


void blink( void* arg );
void spin( void* arg );


#define TIMER1_HZ_RATE      1000

static bool edge[3] = { false, 0 };

static void display_font( uint8_t n )
    {
    if ( n == 0 )
        {
        Bsp_GLcd_clearScreen();
        Bsp_GLcd_setFont( eBSP_GCLD_FONT_5X7 );
        Bsp_GLcd_writeRowColLen(0, 0, "0  3456789^12345678", 19 );
        Bsp_GLcd_writeRowColLen(1, 0, "1  3456789g12345678", 19 );
        Bsp_GLcd_writeRowColLen(2, 0, "2  3456789y12345678", 19 );
        Bsp_GLcd_writeRowColLen(3, 0, "3  3456789`12345678", 19 );
        Bsp_GLcd_writeRowColLen(4, 0, "4  3456789~12345678", 19 );
        Bsp_GLcd_writeRowColLen(5, 0, "5  3456789j12345678", 19 );
        Bsp_GLcd_writeRowColLen(6, 0, "6  3456789|12345678", 19 );
        Bsp_GLcd_writeRowColLen(7, 0, "7  3456789{12345678", 19 );
        }

    else if ( n == 1 )
        {
        Bsp_GLcd_clearScreen();
        Bsp_GLcd_setFont( eBSP_GCLD_FONT_8X8 );
        Bsp_GLcd_writeRowColLen(0, 0, "0  3456789^12345678", 12 );
        Bsp_GLcd_writeRowColLen(1, 0, "1  3456789g12345678", 12 );
        Bsp_GLcd_writeRowColLen(2, 0, "2  3456789|12345678", 12 );
        Bsp_GLcd_writeRowColLen(3, 0, "3  3456789`12345678", 12 );
        Bsp_GLcd_writeRowColLen(4, 0, "4  3456789~12345678", 12 );
        Bsp_GLcd_writeRowColLen(5, 0, "5  3456789j12345678", 12 );
        Bsp_GLcd_writeRowColLen(6, 0, "6  3456789|12345678", 12 );
        Bsp_GLcd_writeRowColLen(7, 0, "7  3456789{12345678", 12 );
        }
    else if ( n == 2 )
        {
        Bsp_GLcd_clearScreen();
        Bsp_GLcd_setFont( eBSP_GCLD_FONT_8X16 );
        Bsp_GLcd_writeRowColLen(0, 0, "0  3456789^12345678", 12 );
        Bsp_GLcd_writeRowColLen(1, 0, "1  3456789g12345678", 12 );
        Bsp_GLcd_writeRowColLen(2, 0, "2  3456789|12345678", 12 );
        Bsp_GLcd_writeRowColLen(3, 0, "3  3456789`12345678", 12 );
        }

    }

int main(void)
    {
    // Initialize the board
    Bsp_Api_initialize();
    Bsp_initCompareMatchTimer( 3, TIMER1_HZ_RATE, 0, 14 );  // Used by the Test App 
    Bsp_setCircleLED( 11, true );
    
    Bsp_GLcd_intialize();
    display_font( 0 );

    xTaskCreate( blink, "Blink", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL );
    xTaskCreate( spin, "Spin",   configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL );

	/* Start the tasks running. */
	vTaskStartScheduler();


    // I should never get here!
	for( ;; );
    return 0;
    }

void blink( void* arg )
    {
    // Start my app timer
    Bsp_startCompareMatchTimer( 3 );
    Bsp_setCircleLED( 12, true );

    for(;;)
        {
        vTaskDelay( 200 );
        Bsp_toggleCircleLED( 12 );
        }        
    }


void spin( void* arg )
    {
    // Spin the LEDs
    for(;;)
        {
        uint8_t m;
        int     i;


        for( m = 4; m <= 10; m++ )
            {
            Bsp_toggleCircleLED( m );

            for(i=0; i< 100000; i++)
                {
                uint8_t n;
                for( n=1; n <= 3; n++ )
                    {
                    if ( Bsp_isButtonPressed( n ) )
                        {
                        Bsp_setCircleLED( 12+n, true );

                        if ( edge[n-1] == false )
                            {
                            edge[n-1] = true;
                            display_font( n-1 );
                            }
                        }
                    else
                        {
                        Bsp_setCircleLED( 12+n, false );
                        edge[n-1] = false;
                        }
                    }
                }
            }
        }
    }



void irqTimer1( void )
    {
    static uint16_t counter = 0;
    static bool     oncycle = true;

    counter++;
    if ( oncycle )
        {
        if (  counter > TIMER1_HZ_RATE * 3 / 4 )
            {
            Bsp_setCircleLED( 11, false );
            oncycle = false;
            }
        }
    else if ( counter > TIMER1_HZ_RATE )
        {
        Bsp_setCircleLED( 11, true );
        counter = 0;
        oncycle = true;
        }
    }

