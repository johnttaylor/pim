/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2022  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/

#include "Driver/PicoDisplay/Api.h" // Must be first #include (because of the Pimoroni/Pico SDK)
#include "app.h"
#include "Sketch.h"
#include "colony_config.h"
#include "ModelPoints.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Dm/SubscriberComposer.h"
#include "Cpl/Dm/MailboxServer.h"
#include "Cpl/System/Trace.h"

static void drawStartScreen();
static void clearTheCanvas( uint8_t bgColorRed, uint8_t bgColorGreen, uint8_t bgColorBlue );
static void movePencil( int deltaX, int deltaY );
static void setPencilColor( uint8_t red, uint8_t green, uint8_t blue );
static void setPencilSize( unsigned size );
static void showCursor();
static void restoreCursorBlock();

/*-----------------------------------------------------------*/
class EventMonitor
{
public:
    /// Container for RGB color
    struct RGB_T
    {
        uint8_t red;
        uint8_t green;
        uint8_t blue;
    };

public:
    /// Constructor
    EventMonitor( Cpl::Dm::MailboxServer&  myMbox )
        : m_obAltButton( myMbox, *this, &EventMonitor::buttonEventAlt )
        , m_obLeftButton( myMbox, *this, &EventMonitor::buttonEventLeft )
        , m_obRightButton( myMbox, *this, &EventMonitor::buttonEventRight )
        , m_obUpButton( myMbox, *this, &EventMonitor::buttonEventUp )
        , m_obDownButton( myMbox, *this, &EventMonitor::buttonEventDown )
        , m_obSizeButton( myMbox, *this, &EventMonitor::buttonEventSize )
        , m_obColorButton( myMbox, *this, &EventMonitor::buttonEventColor )
        , m_obClearButton( myMbox, *this, &EventMonitor::buttonEventClear )
    {}

public:
    /// Initialize
    void start()
    {
        // Housekeeping
        m_waitingAnyKey = true;
        m_sizeIndex     = 0;
        m_colorIndex    = 0;
        m_cursorActive  = false;

        // Subscribe for change notifications
        mp::buttonEventAlt.attach( m_obAltButton );
        mp::buttonEventLeft.attach( m_obLeftButton );
        mp::buttonEventRight.attach( m_obRightButton );
        mp::buttonEventUp.attach( m_obUpButton );
        mp::buttonEventDown.attach( m_obDownButton );
        mp::buttonEventSize.attach( m_obSizeButton );
        mp::buttonEventColor.attach( m_obColorButton );
        mp::buttonEventClear.attach( m_obClearButton );
    }


public:
    /// Button event
    void buttonEventAlt( Cpl::Dm::Mp::Bool& mp )
    {
        bool pressed;
        if ( mp.readAndSync( pressed, m_obAltButton ) && pressed && !consumeAnyKeyWhenWaiting() )
        {
            toggleCursor();
        }
    }

    /// Button event
    void buttonEventRight( Cpl::Dm::Mp::Bool& mp )
    {
        bool pressed;
        if ( mp.readAndSync( pressed, m_obRightButton ) && pressed && !consumeAnyKeyWhenWaiting() )
        {
            deactivateCursor();
            movePencil( 1, 0 );
        }
    }

    /// Button event
    void buttonEventLeft( Cpl::Dm::Mp::Bool& mp )
    {
        bool pressed;
        if ( mp.readAndSync( pressed, m_obLeftButton ) && pressed && !consumeAnyKeyWhenWaiting() )
        {
            deactivateCursor();
            movePencil( -1, 0 );
        }
    }

    /// Button event
    void buttonEventUp( Cpl::Dm::Mp::Bool& mp )
    {
        bool pressed;
        if ( mp.readAndSync( pressed, m_obUpButton ) && pressed && !consumeAnyKeyWhenWaiting() )
        {
            deactivateCursor();
            movePencil( 0, -1 );
        }
    }

    /// Button event
    void buttonEventDown( Cpl::Dm::Mp::Bool& mp )
    {
        bool pressed;
        if ( mp.readAndSync( pressed, m_obDownButton ) && pressed && !consumeAnyKeyWhenWaiting() )
        {
            deactivateCursor();
            movePencil( 0, 1 );
        }
    }

    /// Button event
    void buttonEventSize( Cpl::Dm::Mp::Bool& mp )
    {
        bool pressed;
        if ( mp.readAndSync( pressed, m_obSizeButton ) && pressed && !consumeAnyKeyWhenWaiting() )
        {

            deactivateCursor();

            const static uint8_t sizes[] ={ 1, 2, 3, 4, 6, 8, 10, 20 };
            m_sizeIndex++;
            if ( m_sizeIndex >= sizeof( sizes ) )
            {
                m_sizeIndex = 0;
            }
            setPencilSize( sizes[m_sizeIndex] );

            activateCursor();
        }
    }

    /// Button event
    void buttonEventColor( Cpl::Dm::Mp::Bool& mp )
    {
        bool pressed;
        if ( mp.readAndSync( pressed, m_obColorButton ) && pressed && !consumeAnyKeyWhenWaiting() )
        {
            // Colors: Black, Blue, Green, Cyan, Red, Magenta, Yellow, White }
            const static RGB_T colors[] ={ {0,0,0},         // Black
                                           {0,0,255},       // Blue
                                           {0,255,0},       // Green
                                           {0,255,255},     // Cyan
                                           {255,0,0},       // Red
                                           {255,0,255},     // Magenta
                                           {255,255,0},    // Yellow
                                           {255,255,255}   // White
            };
            m_colorIndex++;
            if ( m_colorIndex >= (sizeof( colors ) / sizeof( RGB_T )) )
            {
                m_colorIndex = 0;
            }

            setPencilColor( colors[m_colorIndex].red, colors[m_colorIndex].green, colors[m_colorIndex].blue );
            Driver::PicoDisplay::Api::rgbLED().setRgb( colors[m_colorIndex].red, colors[m_colorIndex].green, colors[m_colorIndex].blue );
        }
    }

    /// Button event
    void buttonEventClear( Cpl::Dm::Mp::Bool & mp )
    {
        bool pressed;
        if ( mp.readAndSync( pressed, m_obClearButton ) && pressed && !consumeAnyKeyWhenWaiting() )
        {
            drawStartScreen();
            m_waitingAnyKey = true;
            m_sizeIndex     = 0;
            m_colorIndex    = 0;
            m_cursorActive  = false;
        }
    }

public:
    /// Helper function to the first key pess when on the start screen
    bool consumeAnyKeyWhenWaiting()
    {
        if ( m_waitingAnyKey )
        {
            clearTheCanvas( 255, 255, 255 );
            m_waitingAnyKey = false;
            return true;
        }
        return false;
    }

    /// Helper method to deactivates the cursor (if active)
    void deactivateCursor()
    {
        if ( m_cursorActive )
        {
            m_cursorActive = false;
            restoreCursorBlock();
        }
    }

    /// Helper method to activate the cursor
    void activateCursor()
    {
        if ( !m_cursorActive )
        {
            m_cursorActive = true;
            showCursor();
        }
    }

    /// Helper method to toggle the cursor
    void toggleCursor()
    {
        if ( !m_cursorActive )
        {
            m_cursorActive = true;
            showCursor();
        }
        else
        {
            m_cursorActive = false;
            restoreCursorBlock();
        }
    }

public:
    /// Observers
    Cpl::Dm::SubscriberComposer<EventMonitor, Cpl::Dm::Mp::Bool> m_obAltButton;
    Cpl::Dm::SubscriberComposer<EventMonitor, Cpl::Dm::Mp::Bool> m_obLeftButton;
    Cpl::Dm::SubscriberComposer<EventMonitor, Cpl::Dm::Mp::Bool> m_obRightButton;
    Cpl::Dm::SubscriberComposer<EventMonitor, Cpl::Dm::Mp::Bool> m_obUpButton;
    Cpl::Dm::SubscriberComposer<EventMonitor, Cpl::Dm::Mp::Bool> m_obDownButton;
    Cpl::Dm::SubscriberComposer<EventMonitor, Cpl::Dm::Mp::Bool> m_obSizeButton;
    Cpl::Dm::SubscriberComposer<EventMonitor, Cpl::Dm::Mp::Bool> m_obColorButton;
    Cpl::Dm::SubscriberComposer<EventMonitor, Cpl::Dm::Mp::Bool> m_obClearButton;

    /// Current size index
    unsigned    m_sizeIndex;

    /// Current color index
    unsigned    m_colorIndex;

    /// Waiting for any key
    bool        m_waitingAnyKey;

    /// State of the Cursor
    bool        m_cursorActive;
};


/*---------------------------------------------------------------------------*/

static EventMonitor events_( *g_uiRunnablePtr );

void intializeUI()
{
    // Draw start/splash screen
    drawStartScreen();

    // Start the Event monitor
    events_.start();
}

void processUI( Cpl::System::ElapsedTime::Precision_T currentTick,
                Cpl::System::ElapsedTime::Precision_T currentInterval )
{

}

///////////////////////////////////////////////////////////////////////////////

//
// SCREEN OPERATIONS
//

// Graphics library: Use RGB332 mode (256 colours) on the Target to limit RAM usage canvas 
pimoroni::PicoGraphics_PenRGB332 graphics_( MY_APP_DISPLAY_WIDTH, MY_APP_DISPLAY_HEIGHT, nullptr );


static unsigned pencilSize_;
static int pencilX0_;
static int pencilY0_;
static uint8_t pencilRed_;
static uint8_t pencilGreen_;
static uint8_t pencilBlue_;

/*---------------------------------------------------------------------------*/
void setPencilColor( uint8_t red, uint8_t green, uint8_t blue )
{
    pencilRed_   = red;
    pencilGreen_ = green;
    pencilBlue_  = blue;
    graphics_.set_pen( red, green, blue );
};

void restorePencilColor()
{
    setPencilColor( pencilRed_, pencilGreen_, pencilBlue_ );
};

void setPencilSize( unsigned size )
{
    pencilSize_ = size;
};

void movePencil( int deltaX, int deltaY )
{
    // Move the X coordinate
    pencilX0_ += deltaX;
    if ( pencilX0_ < 0 )
    {
        pencilX0_ = 0;
    }
    else if ( pencilX0_ >= MY_APP_DISPLAY_WIDTH )
    {
        pencilX0_ = MY_APP_DISPLAY_WIDTH - 1;
    }

    // Move the Y coordinate
    pencilY0_ += deltaY;
    if ( pencilY0_ < 0 )
    {
        pencilY0_ = 0;
    }
    else if ( pencilY0_ >= MY_APP_DISPLAY_HEIGHT )
    {
        pencilY0_ = MY_APP_DISPLAY_HEIGHT - 1;
    }

    // now update the screen
    pimoroni::Rect box( pencilX0_, pencilY0_, pencilSize_, pencilSize_ );
    graphics_.rectangle( box );
    Driver::PicoDisplay::Api::updateLCD( graphics_ );

}

void clearTheCanvas( uint8_t bgColorRed, uint8_t bgColorGreen, uint8_t bgColorBlue )
{
    // set the colour of the pen
    graphics_.set_pen( bgColorRed, bgColorGreen, bgColorBlue );

    // fill the screen with the current pen colour
    graphics_.clear();

    // Leave the Pencil color at black
    graphics_.set_pen( 0, 0, 0 );
    pencilX0_    = MY_APP_DISPLAY_WIDTH / 2;
    pencilY0_    = MY_APP_DISPLAY_HEIGHT / 2;
    pencilSize_  = 1;
    pencilRed_   = 0;
    pencilGreen_ = 0;
    pencilBlue_  = 0;

    // now we've done our drawing let's update the screen
    Driver::PicoDisplay::Api::updateLCD( graphics_ );
}

/*---------------------------------------------------------------------------*/
#define X0_TITLE        40
#define Y0_TITLE        0

#define X0_HELP_LINE                18
#define COLOFFSET_HELP_LINE_COL2    68
#define COLOFFSET_HELP_LINE_COL3    (COLOFFSET_HELP_LINE_COL2+38)
#define COLOFFSET_HELP_LINE_COL4    (COLOFFSET_HELP_LINE_COL3+60)
#define HEIGHT_HELP_LINES           22
#define Y0_HELP_LINE1               42
#define Y0_HELP_LINE2               (Y0_HELP_LINE1+HEIGHT_HELP_LINES)
#define Y0_HELP_LINE3               (Y0_HELP_LINE2+HEIGHT_HELP_LINES)
#define Y0_HELP_LINE4               (Y0_HELP_LINE3+HEIGHT_HELP_LINES)
#define WRAP_HELP_LINES 205

#define HELP_TEXT_LINE1_COL1        "Right:"
#define HELP_TEXT_LINE1_COL2        "X"
#define HELP_TEXT_LINE1_COL3        "Left:"
#define HELP_TEXT_LINE1_COL4        "A+X"

#define HELP_TEXT_LINE2_COL1        "Down:"
#define HELP_TEXT_LINE2_COL2        "Y"
#define HELP_TEXT_LINE2_COL3        "Up:"
#define HELP_TEXT_LINE2_COL4        "A+Y"

#define HELP_TEXT_LINE3_COL1        "Color:"
#define HELP_TEXT_LINE3_COL2        "B"
#define HELP_TEXT_LINE3_COL3        "Size:"
#define HELP_TEXT_LINE3_COL4        "A+B"

#define HELP_TEXT_LINE4_COL1        "Cursor:"
#define HELP_TEXT_LINE4_COL2        "A"
#define HELP_TEXT_LINE4_COL3        "Clear:"
#define HELP_TEXT_LINE4_COL4        "A+B+X"

void drawStartScreen()
{
    // Turn the RGB LED off
    Driver::PicoDisplay::Api::rgbLED().setOff();
    Driver::PicoDisplay::Api::rgbLED().setBrightness( 64 );

    // set the colour of the pen
    graphics_.set_pen( 0, 0, 255 );

    // fill the screen with the current pen colour
    graphics_.clear();

    // Title
    graphics_.set_pen( 0, 255, 0 );
    graphics_.set_font( &font14_outline );
    graphics_.text( "Pico-Sketch", pimoroni::Point( X0_TITLE, Y0_TITLE ), 240 );

    // Help
    graphics_.set_pen( 255, 255, 255 );
    graphics_.set_font( &font8 );
    graphics_.text( HELP_TEXT_LINE1_COL1, pimoroni::Point( X0_HELP_LINE, Y0_HELP_LINE1 ), 240 );
    graphics_.text( HELP_TEXT_LINE1_COL2, pimoroni::Point( X0_HELP_LINE + COLOFFSET_HELP_LINE_COL2, Y0_HELP_LINE1 ), 240 );
    graphics_.text( HELP_TEXT_LINE1_COL3, pimoroni::Point( X0_HELP_LINE + COLOFFSET_HELP_LINE_COL3, Y0_HELP_LINE1 ), 240 );
    graphics_.text( HELP_TEXT_LINE1_COL4, pimoroni::Point( X0_HELP_LINE + COLOFFSET_HELP_LINE_COL4, Y0_HELP_LINE1 ), 240 );

    graphics_.text( HELP_TEXT_LINE2_COL1, pimoroni::Point( X0_HELP_LINE, Y0_HELP_LINE2 ), 240 );
    graphics_.text( HELP_TEXT_LINE2_COL2, pimoroni::Point( X0_HELP_LINE + COLOFFSET_HELP_LINE_COL2, Y0_HELP_LINE2 ), 240 );
    graphics_.text( HELP_TEXT_LINE2_COL3, pimoroni::Point( X0_HELP_LINE + COLOFFSET_HELP_LINE_COL3, Y0_HELP_LINE2 ), 240 );
    graphics_.text( HELP_TEXT_LINE2_COL4, pimoroni::Point( X0_HELP_LINE + COLOFFSET_HELP_LINE_COL4, Y0_HELP_LINE2 ), 240 );

    graphics_.text( HELP_TEXT_LINE3_COL1, pimoroni::Point( X0_HELP_LINE, Y0_HELP_LINE3 ), 240 );
    graphics_.text( HELP_TEXT_LINE3_COL2, pimoroni::Point( X0_HELP_LINE + COLOFFSET_HELP_LINE_COL2, Y0_HELP_LINE3 ), 240 );
    graphics_.text( HELP_TEXT_LINE3_COL3, pimoroni::Point( X0_HELP_LINE + COLOFFSET_HELP_LINE_COL3, Y0_HELP_LINE3 ), 240 );
    graphics_.text( HELP_TEXT_LINE3_COL4, pimoroni::Point( X0_HELP_LINE + COLOFFSET_HELP_LINE_COL4, Y0_HELP_LINE3 ), 240 );

    graphics_.text( HELP_TEXT_LINE4_COL1, pimoroni::Point( X0_HELP_LINE, Y0_HELP_LINE4 ), 240 );
    graphics_.text( HELP_TEXT_LINE4_COL2, pimoroni::Point( X0_HELP_LINE + COLOFFSET_HELP_LINE_COL2, Y0_HELP_LINE4 ), 240 );
    graphics_.text( HELP_TEXT_LINE4_COL3, pimoroni::Point( X0_HELP_LINE + COLOFFSET_HELP_LINE_COL3, Y0_HELP_LINE4 ), 240 );
    graphics_.text( HELP_TEXT_LINE4_COL4, pimoroni::Point( X0_HELP_LINE + COLOFFSET_HELP_LINE_COL4, Y0_HELP_LINE4 ), 240 );


    // now we've done our drawing let's update the screen
    Driver::PicoDisplay::Api::updateLCD( graphics_ );
}


/*---------------------------------------------------------------------------*/
#define CURSOR_RADIUS       15
#define CURSOR_MARGIN       5
#define BYTES_PER_PIXEL     sizeof(uint8_t)

static int     blockX0_;
static int     blockY0_;
static int     blockWidth_;
static int     blockHeight_;
static int     blockRowSize_;
static uint8_t blockBuffer_[2 * CURSOR_RADIUS * 2 * CURSOR_RADIUS];

void showCursor()
{
    // Define and clamp the Cursor's dimensions
    int topOffset = CURSOR_RADIUS - pencilSize_ / 2;
    blockX0_      = topOffset > pencilX0_ ? 0 : pencilX0_ - topOffset;
    blockWidth_   = blockX0_ + 2 * CURSOR_RADIUS >= MY_APP_DISPLAY_WIDTH ? MY_APP_DISPLAY_WIDTH - blockX0_ : 2 * CURSOR_RADIUS;
    blockY0_      = topOffset > pencilY0_ ? 0 : pencilY0_ - topOffset;
    blockHeight_  = blockY0_ + 2 * CURSOR_RADIUS >= MY_APP_DISPLAY_HEIGHT ? MY_APP_DISPLAY_HEIGHT - blockY0_ : 2 * CURSOR_RADIUS;
    blockRowSize_ = blockWidth_ * BYTES_PER_PIXEL;

    // Get copy of screen content for the cursor
    const uint8_t* originPtr    = (const uint8_t*) graphics_.frame_buffer;
    const uint8_t* nextBytePtr  = originPtr + blockX0_ + blockY0_ * MY_APP_DISPLAY_WIDTH * BYTES_PER_PIXEL;
    uint8_t*       nextCachePtr = blockBuffer_;
    for ( int yidx=0; yidx < blockHeight_; yidx++ )
    {
        memcpy( nextCachePtr, nextBytePtr, blockRowSize_ );
        nextCachePtr += blockRowSize_;
        nextBytePtr  += MY_APP_DISPLAY_WIDTH;
    }


    // Draw the new cursor
    graphics_.set_pen( 0, 0, 0 );
    pimoroni::Rect cursor( blockX0_, blockY0_, blockWidth_, blockHeight_ );
    graphics_.rectangle( cursor );
    cursor.deflate( CURSOR_RADIUS - (pencilSize_ + 1) / 2 );
    graphics_.set_pen( 255, 255, 255 );
    graphics_.rectangle( cursor );

    // now we've done our drawing let's update the screen
    Driver::PicoDisplay::Api::updateLCD( graphics_ );
}

void restoreCursorBlock()
{
    restorePencilColor();

    // Restore the original screen contents
    uint8_t*       originPtr    = (uint8_t*) graphics_.frame_buffer;
    uint8_t*       nextBytePtr  = originPtr + blockX0_ + blockY0_ * MY_APP_DISPLAY_WIDTH * BYTES_PER_PIXEL;
    const uint8_t* nextCachePtr = blockBuffer_;
    for ( int yidx=0; yidx < blockHeight_; yidx++ )
    {
        memcpy( nextBytePtr, nextCachePtr, blockRowSize_ );
        nextCachePtr += blockRowSize_;
        nextBytePtr  += MY_APP_DISPLAY_WIDTH;
    }

    // now we've done our drawing let's update the screen
    Driver::PicoDisplay::Api::updateLCD( graphics_ );
}
