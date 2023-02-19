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
#include "Ui.h"
#include "Private_.h"
#include "Layout_.h"
#include "EventMonitor.h"
#include "Storm/Thermostat/ModelPoints.h"
#include "Storm/Thermostat/Ui/PicoDisplay/ModelPoints.h"
#include "Storm/Thermostat/Main/_pico/Main.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Dm/SubscriberComposer.h"
#include "Cpl/Dm/MailboxServer.h"
#include "Cpl/System/Trace.h"

static void drawStartScreen();

Storm::Type::ThermostatMode Storm::Thermostat::Ui::PicoDisplay::g_uiMode            = Storm::Type::ThermostatMode::eOFF;
Storm::Type::FanMode        Storm::Thermostat::Ui::PicoDisplay::g_uiFan             = Storm::Type::FanMode::eAUTO;
float                       Storm::Thermostat::Ui::PicoDisplay::g_uiCoolingSetpoint = 0.0F;
float                       Storm::Thermostat::Ui::PicoDisplay::g_uiHeatingSetpoint = 0.0F;


using namespace Storm::Thermostat::Ui::PicoDisplay;

static EventMonitor events_( *g_uiRunnablePtr );

// Graphics library: Use RGB332 mode (256 colours) on the Target to limit RAM usage canvas 
pimoroni::PicoGraphics_PenRGB332 graphics_( OPTION_DRIVER_PICO_DISPLAY_LCD_WIDTH, OPTION_DRIVER_PICO_DISPLAY_LCD_HEIGHT, nullptr );


void Storm::Thermostat::Ui::PicoDisplay::intializeUI()
{
    // Populate my working variables
    mp_userMode.read( g_uiMode );
    mp_fanMode.read( g_uiFan );
    mp_setpoints.read( g_uiCoolingSetpoint, g_uiHeatingSetpoint );

    // Draw start/splash screen
    drawStartScreen();

    // Start the Event monitor
    events_.start();
}

void Storm::Thermostat::Ui::PicoDisplay::processUI( Cpl::System::ElapsedTime::Precision_T currentTick,
                                                    Cpl::System::ElapsedTime::Precision_T currentInterval )
{
    // Update Temperatures (they are analog values and subject to change at any time)
    drawIdt();
    drawOdt();

    // Update the Relay outputs
    drawHVACOutputs();

    // Periodic Refresh of the screen
    Driver::PicoDisplay::Api::updateLCD( graphics_ );

    // Update the time on the simulation every half seconds (on the target this does nothing)
    static unsigned counter = 0;
    if ( ++counter > 5 )
    {
        counter = 0;
        Driver::PicoDisplay::Api::nop();
    }
}

void Storm::Thermostat::Ui::PicoDisplay::shutdownUI()
{
    events_.stop();
}

///////////////////////////////////////////////////////////////////////////////

//
// SCREEN OPERATIONS
//


/*---------------------------------------------------------------------------*/

void drawStartScreen()
{
    // Default the RGB LED to off
    Driver::PicoDisplay::Api::rgbLED().setOff();
    Driver::PicoDisplay::Api::rgbLED().setBrightness( 64 );

    // Clear the screen
    graphics_.set_pen( BACKGROUND_COLOR_R, BACKGROUND_COLOR_G, BACKGROUND_COLOR_B );
    graphics_.clear();

    // Draw labels
    graphics_.set_font( &font8 );
    graphics_.set_pen( MODE_LABEL_COLOR_R, MODE_LABEL_COLOR_G, MODE_LABEL_COLOR_B );
    graphics_.text( "mode:", pimoroni::Point( MODE_LABEL_X0, MODE_LABEL_Y0 ), 240 );
    graphics_.set_pen( FAN_LABEL_COLOR_R, FAN_LABEL_COLOR_G, FAN_LABEL_COLOR_B );
    graphics_.text( "fan:", pimoroni::Point( FAN_LABEL_X0, FAN_LABEL_Y0 ), 240 );
    graphics_.set_pen( IDT_LABEL_COLOR_R, IDT_LABEL_COLOR_G, IDT_LABEL_COLOR_B );
    graphics_.text( "room:", pimoroni::Point( IDT_LABEL_X0, IDT_LABEL_Y0 ), 240 );
    graphics_.set_pen( ODT_LABEL_COLOR_R, ODT_LABEL_COLOR_G, ODT_LABEL_COLOR_B );
    graphics_.text( "odt:", pimoroni::Point( ODT_LABEL_X0, ODT_LABEL_Y0 ), 240 );
    graphics_.set_pen( COOL_LABEL_COLOR_R, COOL_LABEL_COLOR_G, COOL_LABEL_COLOR_B );
    graphics_.text( "cool:", pimoroni::Point( COOL_LABEL_X0, COOL_LABEL_Y0 ), 240 );
    graphics_.set_pen( HEAT_LABEL_COLOR_R, HEAT_LABEL_COLOR_G, HEAT_LABEL_COLOR_B );
    graphics_.text( "heat:", pimoroni::Point( HEAT_LABEL_X0, HEAT_LABEL_Y0 ), 240 );

    // Draw values
    drawMode();
    drawFan();
    drawIdt();
    drawOdt();
    drawCoolingSetpoint();
    drawHeatingSetpoint();
    drawHVACOutputs();

    // now we've done our drawing let's update the screen
    Driver::PicoDisplay::Api::updateLCD( graphics_ );
}

void Storm::Thermostat::Ui::PicoDisplay::drawHVACOutputs()
{
    // Clear all fields

    // Fan
    pimoroni::Rect box( G_OUTPUT_X0, G_OUTPUT_Y0, G_OUTPUT_WIDTH, G_OUTPUT_HEIGHT );
    graphics_.set_pen( BACKGROUND_COLOR_R, BACKGROUND_COLOR_G, BACKGROUND_COLOR_B );
    graphics_.rectangle( box );

    // Compressor: 1st stage
    pimoroni::Rect box2( Y1_OUTPUT_X0, Y1_OUTPUT_Y0, Y1_OUTPUT_WIDTH, Y1_OUTPUT_HEIGHT );
    graphics_.set_pen( BACKGROUND_COLOR_R, BACKGROUND_COLOR_G, BACKGROUND_COLOR_B );
    graphics_.rectangle( box2 );

    // Compressor: 2nd stage
    pimoroni::Rect box3( Y2_OUTPUT_X0, Y2_OUTPUT_Y0, Y2_OUTPUT_WIDTH, Y2_OUTPUT_HEIGHT );
    graphics_.set_pen( BACKGROUND_COLOR_R, BACKGROUND_COLOR_G, BACKGROUND_COLOR_B );
    graphics_.rectangle( box3 );

    // Indoor: 1st stage
    pimoroni::Rect box4( W1_OUTPUT_X0, W1_OUTPUT_Y0, W1_OUTPUT_WIDTH, W1_OUTPUT_HEIGHT );
    graphics_.set_pen( BACKGROUND_COLOR_R, BACKGROUND_COLOR_G, BACKGROUND_COLOR_B );
    graphics_.rectangle( box4 );

    // Indoor: 2nd stage
    pimoroni::Rect box5( W2_OUTPUT_X0, W2_OUTPUT_Y0, W2_OUTPUT_WIDTH, W2_OUTPUT_HEIGHT );
    graphics_.set_pen( BACKGROUND_COLOR_R, BACKGROUND_COLOR_G, BACKGROUND_COLOR_B );
    graphics_.rectangle( box5 );

    // Indoor: third stage
    pimoroni::Rect box6( W3_OUTPUT_X0, W3_OUTPUT_Y0, W3_OUTPUT_WIDTH, W3_OUTPUT_HEIGHT );
    graphics_.set_pen( BACKGROUND_COLOR_R, BACKGROUND_COLOR_G, BACKGROUND_COLOR_B );
    graphics_.rectangle( box6 );


    // Get the outputs - and only update the screen if we have valid values
    Storm::Type::HvacRelayOutputs_T outputs;
    if ( mp_relayOutputs.read( outputs ) )
    {
        // Fan
        if ( outputs.g )
        {
            Cpl::Text::FString<6> val;
            val.format( "G:%d%%", outputs.bk );
            graphics_.set_pen( G_OUTPUT_NOMINAL_COLOR_R, G_OUTPUT_NOMINAL_COLOR_G, G_OUTPUT_NOMINAL_COLOR_B );
            graphics_.text( val.getString(), pimoroni::Point( G_OUTPUT_X0, G_OUTPUT_Y0 ), OPTION_DRIVER_PICO_DISPLAY_LCD_WIDTH );
        }

        // Compressor: 1st stage
        if ( outputs.y1 )
        {
            graphics_.set_pen( Y1_OUTPUT_NOMINAL_COLOR_R, Y1_OUTPUT_NOMINAL_COLOR_G, Y1_OUTPUT_NOMINAL_COLOR_B );
            graphics_.text( "y1", pimoroni::Point( Y1_OUTPUT_X0, Y1_OUTPUT_Y0 ), OPTION_DRIVER_PICO_DISPLAY_LCD_WIDTH );
        }

        // Compressor: 2nd stage
        if ( outputs.y2 )
        {
            graphics_.set_pen( Y2_OUTPUT_NOMINAL_COLOR_R, Y2_OUTPUT_NOMINAL_COLOR_G, Y2_OUTPUT_NOMINAL_COLOR_B );
            graphics_.text( "y2", pimoroni::Point( Y2_OUTPUT_X0, Y2_OUTPUT_Y0 ), OPTION_DRIVER_PICO_DISPLAY_LCD_WIDTH );
        }

        // Indoor: 1st stage
        if ( outputs.w1 )
        {
            graphics_.set_pen( W1_OUTPUT_NOMINAL_COLOR_R, W1_OUTPUT_NOMINAL_COLOR_G, W1_OUTPUT_NOMINAL_COLOR_B );
            graphics_.text( "w1", pimoroni::Point( W1_OUTPUT_X0, W1_OUTPUT_Y0 ), OPTION_DRIVER_PICO_DISPLAY_LCD_WIDTH );
        }

        // Indoor: 2nd stage
        if ( outputs.w2 )
        {
            graphics_.set_pen( W2_OUTPUT_NOMINAL_COLOR_R, W2_OUTPUT_NOMINAL_COLOR_G, W2_OUTPUT_NOMINAL_COLOR_B );
            graphics_.text( "w2", pimoroni::Point( W2_OUTPUT_X0, W2_OUTPUT_Y0 ), OPTION_DRIVER_PICO_DISPLAY_LCD_WIDTH );
        }

        // Indoor: third stage
        if ( outputs.w3 )
        {
            graphics_.set_pen( W3_OUTPUT_NOMINAL_COLOR_R, W3_OUTPUT_NOMINAL_COLOR_G, W3_OUTPUT_NOMINAL_COLOR_B );
            graphics_.text( "w3", pimoroni::Point( W3_OUTPUT_X0, W3_OUTPUT_Y0 ), OPTION_DRIVER_PICO_DISPLAY_LCD_WIDTH ); \
        }
    }

    // Set the RGB state to reflect the Heating/Cooling state
    Storm::Type::SystemConfig_T systemConfig;
    if ( mp_systemConfig.read( systemConfig ) )
    {
        // Cooling: BLUE
        if ( systemConfig.currentOpMode == +Storm::Type::OperatingMode::eCOOLING )
        {
            Driver::PicoDisplay::Api::rgbLED().setRgb( 0, 0, 255 );
        }

        // Heating: RED
        else if ( systemConfig.currentOpMode == +Storm::Type::OperatingMode::eHEATING ||
                  systemConfig.currentOpMode == +Storm::Type::OperatingMode::eID_HEATING )
        {
            Driver::PicoDisplay::Api::rgbLED().setRgb( 255, 0, 0 );
        }

        // Off: off
        else
        {
            Driver::PicoDisplay::Api::rgbLED().setOff();
        }
    }
}

void Storm::Thermostat::Ui::PicoDisplay::drawCoolingSetpoint( bool active )
{
    pimoroni::Rect box( COOL_OUTLINE_X0, COOL_OUTLINE_Y0, COOL_OUTLINE_WIDTH, COOL_OUTLINE_HEIGHT );

    // Clear the current value
    graphics_.set_pen( BACKGROUND_COLOR_R, BACKGROUND_COLOR_G, BACKGROUND_COLOR_B );
    graphics_.rectangle( box );

    // Only display a value when my setpoint is valid
    if ( g_uiCoolingSetpoint >= OPTION_STORM_DM_MP_SETPOINTS_MIN_COOLING )
    {
        // nominal display (i.e. not being updated)
        if ( !active )
        {
            graphics_.set_pen( COOL_NOMINAL_COLOR_R, COOL_NOMINAL_COLOR_G, COOL_NOMINAL_COLOR_B );
        }

        // Active/being-updated
        else
        {
            graphics_.set_pen( COOL_BKGRD_ACTIVE_COLOR_R, COOL_BKGRD_ACTIVE_COLOR_G, COOL_BKGRD_ACTIVE_COLOR_B );
            graphics_.rectangle( box );
            graphics_.set_pen( COOL_ACTIVE_COLOR_R, COOL_ACTIVE_COLOR_G, COOL_ACTIVE_COLOR_B );
        }

        Cpl::Text::FString<2> val = (unsigned) (g_uiCoolingSetpoint + 0.5);
        graphics_.text( val.getString(), pimoroni::Point( COOL_X0, COOL_Y0 ), OPTION_DRIVER_PICO_DISPLAY_LCD_WIDTH );
    }
}

void Storm::Thermostat::Ui::PicoDisplay::drawHeatingSetpoint( bool active )
{
    pimoroni::Rect box( HEAT_OUTLINE_X0, HEAT_OUTLINE_Y0, HEAT_OUTLINE_WIDTH, HEAT_OUTLINE_HEIGHT );

    // Clear the current value
    graphics_.set_pen( BACKGROUND_COLOR_R, BACKGROUND_COLOR_G, BACKGROUND_COLOR_B );
    graphics_.rectangle( box );

    // Only display a value when my setpoint is valid
    if ( g_uiHeatingSetpoint >= OPTION_STORM_DM_MP_SETPOINTS_MIN_HEATING )
    {
        // nominal display (i.e. not being updated)
        if ( !active )
        {
            graphics_.set_pen( HEAT_NOMINAL_COLOR_R, HEAT_NOMINAL_COLOR_G, HEAT_NOMINAL_COLOR_B );
        }

        // Active/being-updated
        else
        {
            graphics_.set_pen( HEAT_BKGRD_ACTIVE_COLOR_R, HEAT_BKGRD_ACTIVE_COLOR_G, HEAT_BKGRD_ACTIVE_COLOR_B );
            graphics_.rectangle( box );
            graphics_.set_pen( HEAT_ACTIVE_COLOR_R, HEAT_ACTIVE_COLOR_G, HEAT_ACTIVE_COLOR_B );
        }

        Cpl::Text::FString<2> val = (unsigned) (g_uiHeatingSetpoint + 0.5);
        graphics_.text( val.getString(), pimoroni::Point( HEAT_X0, HEAT_Y0 ), OPTION_DRIVER_PICO_DISPLAY_LCD_WIDTH );
    }
}

void Storm::Thermostat::Ui::PicoDisplay::drawIdt()
{
    // Clear the temperature
    pimoroni::Rect box( IDT_OUTLINE_X0, IDT_OUTLINE_Y0, IDT_OUTLINE_WIDTH, IDT_OUTLINE_HEIGHT );
    graphics_.set_pen( BACKGROUND_COLOR_R, BACKGROUND_COLOR_G, BACKGROUND_COLOR_B );
    graphics_.rectangle( box );

    // Get the value
    float temperature;
    if ( mp_activeIdt.read( temperature ) )
    {
        graphics_.set_pen( IDT_NOMINAL_COLOR_R, IDT_NOMINAL_COLOR_G, IDT_NOMINAL_COLOR_B );
        Cpl::Text::FString<5> val;
        val.format( "%.1f", temperature );
        graphics_.text( val.getString(), pimoroni::Point( IDT_X0, IDT_Y0 ), OPTION_DRIVER_PICO_DISPLAY_LCD_WIDTH );
    }
}

void Storm::Thermostat::Ui::PicoDisplay::drawOdt()
{
    // Clear the temperature
    pimoroni::Rect box( ODT_OUTLINE_X0, ODT_OUTLINE_Y0, ODT_OUTLINE_WIDTH, ODT_OUTLINE_HEIGHT );
    graphics_.set_pen( BACKGROUND_COLOR_R, BACKGROUND_COLOR_G, BACKGROUND_COLOR_B );
    graphics_.rectangle( box );

    // Get the value
    float temperature;
    if ( mp_outdoorTemp.read( temperature ) )
    {
        graphics_.set_pen( ODT_NOMINAL_COLOR_R, ODT_NOMINAL_COLOR_G, ODT_NOMINAL_COLOR_B );
        Cpl::Text::FString<5> val;
        val.format( "%.1f", temperature );
        graphics_.text( val.getString(), pimoroni::Point( ODT_X0, ODT_Y0 ), OPTION_DRIVER_PICO_DISPLAY_LCD_WIDTH );
    }
}

void Storm::Thermostat::Ui::PicoDisplay::drawMode( bool active )
{
    pimoroni::Rect box( MODE_OUTLINE_X0, MODE_OUTLINE_Y0, MODE_OUTLINE_WIDTH, MODE_OUTLINE_HEIGHT );

    // nominal display (i.e. not being updated)
    if ( !active )
    {
        graphics_.set_pen( BACKGROUND_COLOR_R, BACKGROUND_COLOR_G, BACKGROUND_COLOR_B );
        graphics_.rectangle( box );
        graphics_.set_pen( MODE_NOMINAL_COLOR_R, MODE_NOMINAL_COLOR_G, MODE_NOMINAL_COLOR_B );
    }

    // Active/being-updated
    else
    {
        graphics_.set_pen( MODE_BKGRD_ACTIVE_COLOR_R, MODE_BKGRD_ACTIVE_COLOR_G, MODE_BKGRD_ACTIVE_COLOR_B );
        graphics_.rectangle( box );
        graphics_.set_pen( MODE_ACTIVE_COLOR_R, MODE_ACTIVE_COLOR_G, MODE_ACTIVE_COLOR_B );
    }

    const char* val = "OFF";
    switch ( g_uiMode )
    {
    case Storm::Type::ThermostatMode::eAUTO: val = "AUTO"; break;
    case Storm::Type::ThermostatMode::eCOOLING: val = "COOL"; break;
    case Storm::Type::ThermostatMode::eHEATING: val = "HEAT"; break;
    case Storm::Type::ThermostatMode::eID_HEATING: val = "IHEAT"; break;
    default:
        break;
    }
    graphics_.text( val, pimoroni::Point( MODE_X0, MODE_Y0 ), OPTION_DRIVER_PICO_DISPLAY_LCD_WIDTH );
}

void Storm::Thermostat::Ui::PicoDisplay::drawFan( bool active )
{
    pimoroni::Rect box( FAN_OUTLINE_X0, FAN_OUTLINE_Y0, FAN_OUTLINE_WIDTH, FAN_OUTLINE_HEIGHT );

    // nominal display (i.e. not being updated)
    if ( !active )
    {
        graphics_.set_pen( BACKGROUND_COLOR_R, BACKGROUND_COLOR_G, BACKGROUND_COLOR_B );
        graphics_.rectangle( box );
        graphics_.set_pen( FAN_NOMINAL_COLOR_R, FAN_NOMINAL_COLOR_G, FAN_NOMINAL_COLOR_B );
    }

    // Active/being-updated
    else
    {
        graphics_.set_pen( FAN_BKGRD_ACTIVE_COLOR_R, FAN_BKGRD_ACTIVE_COLOR_G, FAN_BKGRD_ACTIVE_COLOR_B );
        graphics_.rectangle( box );
        graphics_.set_pen( FAN_ACTIVE_COLOR_R, FAN_ACTIVE_COLOR_G, FAN_ACTIVE_COLOR_B );
    }

    const char* val = "AUTO";
    if ( g_uiFan == +Storm::Type::FanMode::eCONTINUOUS )
    {
        val = "CONT";
    }
    graphics_.text( val, pimoroni::Point( FAN_X0, FAN_Y0 ), OPTION_DRIVER_PICO_DISPLAY_LCD_WIDTH );
}

