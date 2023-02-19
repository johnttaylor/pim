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

#include "Driver/PicoDisplay/Api.h" // Must be first #include (because of the Pimoroni/Pico SDK)
#include "Api.h"
#include "Driver/TPipe/Maker.h"
#include "Cpl/System/Thread.h"
#include "Cpl/Dm/PeriodicScheduler.h"
#include "Driver/Button/TPipe/Hal.h"
#include "Driver/LED/TPipe/RedGreenBlue.h"


static Cpl::Container::Map<Driver::TPipe::RxFrameHandlerApi> frameHandlers_( "ignoreThisParameter_usedToSelecStaticContructor" );
static Driver::TPipe::Maker                                  tpipe_( frameHandlers_, OPTION_DRIVER_PICO_DISPLAY_TPIPE_RX_FRAME_HANDLER_SIZE );

static Cpl::Io::Input*  tpipeInfd_;
static Cpl::Io::Output* tpipeOutfd_;

// Create the Button drivers (assumes that the TPipe HAL layer is linked)
static Driver::Button::PolledDebounced buttonA_( OPTION_DRIVER_PICO_DISPLAY_TPIPE_BUTTON_A_NAME );
static Driver::Button::PolledDebounced buttonB_( OPTION_DRIVER_PICO_DISPLAY_TPIPE_BUTTON_B_NAME );
static Driver::Button::PolledDebounced buttonX_( OPTION_DRIVER_PICO_DISPLAY_TPIPE_BUTTON_X_NAME );
static Driver::Button::PolledDebounced buttonY_( OPTION_DRIVER_PICO_DISPLAY_TPIPE_BUTTON_Y_NAME );

// Create RGB LED Driver
static Driver::LED::TPipe::RedGreeBlue rgbLedDriver_( tpipe_, OPTION_DRIVER_PICO_DISPLAY_TPIPE_RGB_LED_NAME );

// Public accessors
Driver::Button::PolledDebounced& Driver::PicoDisplay::Api::buttonA() noexcept { return buttonA_; }
Driver::Button::PolledDebounced& Driver::PicoDisplay::Api::buttonB() noexcept { return buttonB_; }
Driver::Button::PolledDebounced& Driver::PicoDisplay::Api::buttonX() noexcept { return buttonX_; }
Driver::Button::PolledDebounced& Driver::PicoDisplay::Api::buttonY() noexcept { return buttonY_; }
Driver::LED::RedGreenBlue& Driver::PicoDisplay::Api::rgbLED() noexcept { return rgbLedDriver_; }

/*-----------------------------------------------------------*/
//
// Set up Periodic scheduling for the TPipe thread
//

static Cpl::System::PeriodicScheduler::Interval_T tpipeIntervals_[] =
{
    /* No intervals because we are not (currently) sharing the thread.
       The TPipe only needs/uses the 'Idle function'
    */
    CPL_SYSTEM_PERIODIC_SCHEDULAR_END_INTERVALS
};

// In thread initialization for code the executes in the TPipe thread
void tpipeStart( Cpl::System::ElapsedTime::Precision_T currentTick )
{
    tpipe_.getPipeProcessor().start( *tpipeInfd_, *tpipeOutfd_ );
}

// Idle function for the TPipe thread
void tpipeIdle( Cpl::System::ElapsedTime::Precision_T currentTick, bool atLeastOneIntervalExecuted )
{
    tpipe_.getPipeProcessor().poll();
}

// Runnable object for the TPipe thread 
Cpl::Dm::PeriodicScheduler tpipeMbox_( tpipeIntervals_,
                                       tpipeStart,
                                       nullptr,
                                       nullptr,
                                       Cpl::System::ElapsedTime::precision,
                                       tpipeIdle );


/*-----------------------------------------------------------*/
Driver::TPipe::Pipe& Driver::PicoDisplay::TPipe::tpipe() noexcept
{
    return tpipe_;
}

void Driver::PicoDisplay::TPipe::initialize( Cpl::Io::Input& tpipeInStream, Cpl::Io::Output& tpipeOutStream ) noexcept
{
    // Initialize the HAL for the button driver
    driverButtonHalTPipe_initialize( frameHandlers_ );

    // Cache the TPipe Streams 
    tpipeInfd_  = &tpipeInStream;
    tpipeOutfd_ = &tpipeOutStream;

    // Create thread for the TPipe
    Cpl::System::Thread::create( tpipeMbox_, "TPIPE" );
}


void Driver::PicoDisplay::Api::setLCDBrightness( uint8_t value )
{
    // Not supported
}

#define NUM_DISPLAY_BYTES         (OPTION_DRIVER_PICO_DISPLAY_LCD_WIDTH * OPTION_DRIVER_PICO_DISPLAY_LCD_HEIGHT * sizeof(uint8_t))  
#define TPIPE_WORK_BUF_SIZE       (NUM_DISPLAY_BYTES*2 + 128)
#define DATE_SIZE_ROW             (OPTION_DRIVER_PICO_DISPLAY_LCD_WIDTH * sizeof(uint8_t))

static Cpl::Text::FString<TPIPE_WORK_BUF_SIZE> buffer_;
static uint8_t                                 frameCache_[NUM_DISPLAY_BYTES];
static uint8_t*                                nextFrameCacheByte_;
static unsigned                                rowIndex_;
static bool                                    dirty_;

void Driver::PicoDisplay::Api::nop()
{
    // Update Elapsed time
    buffer_ = OPTION_DRIVER_PICO_DISPLAY_TPIP_FRAME_SOF;
    formatMsecTimeStamp( buffer_, Cpl::System::ElapsedTime::precision().asFlatTime(), true, true );
    buffer_.formatAppend( " nop" );
    buffer_ += OPTION_DRIVER_PICO_DISPLAY_TPIP_FRAME_EOF;
    tpipe_.getPipeProcessor().sendRawCommand( buffer_.getString(), buffer_.length() );
}


static void beginLCDData()
{
    nextFrameCacheByte_ = frameCache_;
    rowIndex_           = 0;
    dirty_              = false;
}

static void endLCDData()
{
    if ( dirty_ )
    {
        buffer_ = OPTION_DRIVER_PICO_DISPLAY_TPIP_FRAME_SOF;
        formatMsecTimeStamp( buffer_, Cpl::System::ElapsedTime::precision().asFlatTime(), true, true );
        buffer_.formatAppend( " updateLCD" );
        buffer_ += OPTION_DRIVER_PICO_DISPLAY_TPIP_FRAME_EOF;
        tpipe_.getPipeProcessor().sendRawCommand( buffer_.getString(), buffer_.length() );
    }
}

static void sendLCDData( unsigned rowIndex, const void* data, size_t length )
{
    // Start building the TPipe command
    buffer_ = OPTION_DRIVER_PICO_DISPLAY_TPIP_FRAME_SOF;
    formatMsecTimeStamp( buffer_, Cpl::System::ElapsedTime::precision().asFlatTime(), true, true );
    buffer_.formatAppend( " writeLCDData 0 %u %u 1 ",
                          OPTION_DRIVER_PICO_DISPLAY_LCD_WIDTH,
                          rowIndex );

    // Add the pixel data and send the command
    Cpl::Text::bufferToAsciiHex( data, length, buffer_, true, true );
    buffer_ += OPTION_DRIVER_PICO_DISPLAY_TPIP_FRAME_EOF;
    tpipe_.getPipeProcessor().sendRawCommand( buffer_.getString(), buffer_.length() );
}

static void appendLCDRowData( const void* data, size_t len )
{
    if ( memcmp( nextFrameCacheByte_, data, len ) != 0 )
    {
        memcpy( nextFrameCacheByte_, data, len );
        sendLCDData( rowIndex_, data, len );
        dirty_ = true;
    }

    nextFrameCacheByte_ += len;
    rowIndex_++;
}

void Driver::PicoDisplay::Api::updateLCD( pimoroni::PicoGraphics& graphics )
{
    beginLCDData();

    // Display buffer is screen native
    if ( graphics.pen_type == pimoroni::PicoGraphics::PEN_RGB332 )
    {
        const uint8_t* srcPtr = (const uint8_t*) graphics.frame_buffer;
        for ( int row=0; row < OPTION_DRIVER_PICO_DISPLAY_LCD_HEIGHT; row++ )
        {
            appendLCDRowData( srcPtr, DATE_SIZE_ROW );
            srcPtr += DATE_SIZE_ROW;
        }
    }

    // Convert App's color palette to PEN_RGB332
    else
    {
        graphics.scanline_convert( pimoroni::PicoGraphics::PEN_RGB332,
                                   []( void *data, size_t length )
                                   {
                                       appendLCDRowData( data, length );
                                   }
        );
    }

    endLCDData();
}

