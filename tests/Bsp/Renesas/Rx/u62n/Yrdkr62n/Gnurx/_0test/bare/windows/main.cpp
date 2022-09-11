#include "Bsp/Api.h"
//#include "bsp_glcd.h"
//#include "Bsp/Renesas/Rx/Yrdkr62n/_renesas/Glyph.h"

#define TIMER1_HZ_RATE      1000
#define TIMER2_HZ_RATE      1000

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

static uint8_t rxbyte_;
static uint8_t rxstatus_;

void transmit_byte( uint8_t data )
    {
    // Make sure previous byte was transmitted
    while( !Bsp_uartIsTxDataEmpty(BSP_DB9_CONSOLE) )
        ;

    // Transmit byte
    Bsp_uartTransmitByte(BSP_DB9_CONSOLE, data);
    }
    
void poll_uart(void)
    {
    if ( Bsp_uartIsByteReceived(BSP_DB9_CONSOLE) )
        {
        rxbyte_   = Bsp_uartGetReceivedByte(BSP_DB9_CONSOLE);
        rxstatus_ = Bsp_uartGetReceiveStatus(BSP_DB9_CONSOLE);
        Bsp_uartClrReceivedFlag(BSP_DB9_CONSOLE);
        Bsp_uartClrReceiveError(BSP_DB9_CONSOLE);

        transmit_byte( '[' );
        transmit_byte( rxbyte_ );
        transmit_byte( ']' );
        }
    }


int main(void)
    {
    // Initialize the board
    Bsp_Api_initialize();
    Bsp_initCompareMatchTimer( 0, TIMER1_HZ_RATE, 0, 14 );  
    Bsp_initCompareMatchTimer( 3, TIMER2_HZ_RATE, 0, 14 );  
    
    Bsp_uartInit( BSP_DB9_CONSOLE, BSP_DB9_CONSOLE_PIN_SELECT, 4,BSP_BAUDRATE_9600, BSP_BAUDRATE_DIVIDER_9600, (BSP_PARITY_NONE|BSP_DATABITS_8|BSP_STOPBITS_1) );
    Bsp_uartEnableRX_TX( BSP_DB9_CONSOLE );

    Bsp_setCircleLED( 11, true );
    Bsp_setCircleLED( 12, false );
    
    Bsp_GLcd_intialize();
    display_font( 0 );


    // Enable interrupts
    Bsp_Api_enableIrqs();

    // Start my timers
    Bsp_startCompareMatchTimer( 0 );
    Bsp_startCompareMatchTimer( 3 );
    

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
                    poll_uart();

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
        
    return 0;
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

void irqTimer2( void )
    {
    static uint16_t counter = 0;
    static bool     oncycle = false;

    counter++;
    if ( !oncycle )
        {
        if ( counter > TIMER2_HZ_RATE * 3 / 4 )
            {
            Bsp_setCircleLED( 12, true );
            oncycle = true;
            }
        }
    else if ( counter > TIMER1_HZ_RATE )
        {
        Bsp_setCircleLED( 12, false );
        counter = 0;
        oncycle = false;
        }
    }
