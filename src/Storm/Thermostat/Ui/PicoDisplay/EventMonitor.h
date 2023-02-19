#ifndef Storm_Thermostat_Ui_PicoDisplay_EventMonitor_h
#define Storm_Thermostat_Ui_PicoDisplay_EventMonitor_h
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

#include "colony_config.h"
#include "Storm/Thermostat/Ui/PicoDisplay/Private_.h"
#include "Storm/Thermostat/ModelPoints.h"
#include "Storm/Thermostat/Ui/PicoDisplay/ModelPoints.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Dm/SubscriberComposer.h"
#include "Cpl/Dm/MailboxServer.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/Timer.h"

/** The number of milliseconds to delay updating the application's model
    points when changing user parameters, i.e. how long a 'UI element'
    stays highlight/selected/active
 */
#ifndef OPTION_STORM_THERMOSTAT_UI_PICO_DISPLAY_DELAY_UPDATES_MS
#define OPTION_STORM_THERMOSTAT_UI_PICO_DISPLAY_DELAY_UPDATES_MS        5000    // 5 sec
#endif

 /// 
namespace Storm {
///
namespace Thermostat {
///
namespace Ui {
///
namespace PicoDisplay {

/*-----------------------------------------------------------*/
/** This class monitors event sources (okay just the logical buttons
    at this point) and updates the UI appropriately
 */
class EventMonitor
{
public:
    /// Constructor
    EventMonitor( Cpl::Dm::MailboxServer&  myMbox )
        : m_timer( myMbox, *this, &EventMonitor::expired )
        , m_obModeButton( myMbox, *this, &EventMonitor::buttonEventMode )
        , m_obFanButton( myMbox, *this, &EventMonitor::buttonEventFan )
        , m_obUpButton( myMbox, *this, &EventMonitor::buttonEventUp )
        , m_obDownButton( myMbox, *this, &EventMonitor::buttonEventDown )
        , m_obTStatMode( myMbox, *this, &EventMonitor::thermostatModeChange )
        , m_obFanMode( myMbox, *this, &EventMonitor::fanModeChange )
        , m_obSetpoints( myMbox, *this, &EventMonitor::setpointsModeChange )
        , m_modeActive( false )
        , m_fanActive( false )
        , m_setPointActive( false )
        , m_coolingSetpoingActive( true )
    {
    }

public:
    /// Initialize
    void start()
    {
        // Housekeeping
        m_modeActive            = false;
        m_fanActive             = false;
        m_setPointActive        = false;
        m_coolingSetpoingActive = true;

        // Subscribe for change notifications
        mp_buttonEventMode.attach( m_obModeButton );
        mp_buttonEventFan.attach( m_obFanButton );
        mp_buttonEventUp.attach( m_obUpButton );
        mp_buttonEventDown.attach( m_obDownButton );
        mp_userMode.attach( m_obTStatMode );
        mp_fanMode.attach( m_obFanMode );
        mp_setpoints.attach( m_obSetpoints );
    }

    /// Shutdown
    void stop()
    {
        // Housekeeping
        m_timer.stop();

        // Cancel change notifications
        mp_buttonEventMode.detach( m_obModeButton );
        mp_buttonEventFan.detach( m_obFanButton );
        mp_buttonEventUp.detach( m_obUpButton );
        mp_buttonEventDown.detach( m_obDownButton );
        mp_userMode.detach( m_obTStatMode );
        mp_fanMode.detach( m_obFanMode );
        mp_setpoints.detach( m_obSetpoints );
    }


protected:
    /// Application changed the thermostat mode
    void thermostatModeChange( Storm::Dm::MpThermostatMode& mp )
    {
        if ( mp.read( g_uiMode ) )
        {
            m_modeActive = false;
            drawMode();
        }
    }

    /// Application changed the fan mode
    void fanModeChange( Storm::Dm::MpFanMode& mp )
    {
        if ( mp.read( g_uiFan ) )
        {
            m_fanActive = false;
            drawFan();
        }
    }

    /// Application changed the Setpoints
    void setpointsModeChange( Storm::Dm::MpSetpoints& mp )
    {
        if ( mp.read( g_uiCoolingSetpoint, g_uiHeatingSetpoint ) )
        {
            m_setPointActive = false;
            updateSetpoints();
        }
    }

protected:
    /// Button event
    void buttonEventMode( Cpl::Dm::Mp::Bool& mp )
    {
        bool pressed;
        if ( mp.read( pressed ) && pressed )
        {
            // Check for Setpoint Selection active
            if ( m_setPointActive )
            {
                toggleSetpoint();
                return;
            }

            // Start selection
            if ( !m_modeActive )
            {
                m_modeActive = true;
                m_fanActive  = false;
                drawFan();
            }

            // Toggle to the next value
            else
            {
                unsigned curIdx = g_uiMode._to_index();
                if ( ++curIdx >= Storm::Type::ThermostatMode::_size() )
                {
                    curIdx = 0;
                }
                g_uiMode = Storm::Type::ThermostatMode::_from_index_unchecked( curIdx );
            }

            m_timer.start( OPTION_STORM_THERMOSTAT_UI_PICO_DISPLAY_DELAY_UPDATES_MS );
            drawMode( true );
        }
    }

    /// Button event
    void buttonEventFan( Cpl::Dm::Mp::Bool& mp )
    {
        bool pressed;
        if ( mp.read( pressed ) && pressed )
        {
            // Check for Setpoint Selection active
            if ( m_setPointActive )
            {
                toggleSetpoint();
                return;
            }

            // Start selection
            if ( !m_fanActive )
            {
                m_modeActive = false;
                m_fanActive  = true;
                drawMode();
            }

            // Toggle to the next value
            else
            {
                if ( g_uiFan == +Storm::Type::FanMode::eCONTINUOUS )
                {
                    g_uiFan = Storm::Type::FanMode::eAUTO;
                }
                else
                {
                    g_uiFan = Storm::Type::FanMode::eCONTINUOUS;
                }
            }

            m_timer.start( OPTION_STORM_THERMOSTAT_UI_PICO_DISPLAY_DELAY_UPDATES_MS );
            drawFan( true );
        }
    }

    /// Button event
    void buttonEventUp( Cpl::Dm::Mp::Bool& mp )
    {
        bool pressed;
        if ( mp.read( pressed ) && pressed )
        {
            // Start selection
            if ( !m_setPointActive )
            {
                m_setPointActive        = true;
                m_coolingSetpoingActive = true;
                m_modeActive            = false;
                m_fanActive             = false;
                drawFan();
                drawMode();
            }
            
            // Increment the active setpoint
            else
            {
                changeSetpoint( +1.0F );
            }

            m_timer.start( OPTION_STORM_THERMOSTAT_UI_PICO_DISPLAY_DELAY_UPDATES_MS );
            updateSetpoints();
        }
    }

    /// Button event
    void buttonEventDown( Cpl::Dm::Mp::Bool & mp )
    {
        bool pressed;
        if ( mp.read( pressed ) && pressed )
        {
            // Start selection
            if ( !m_setPointActive )
            {
                m_setPointActive        = true;
                m_coolingSetpoingActive = true;
                m_modeActive            = false;
                m_fanActive             = false;
                drawFan();
                drawMode();
            }

            // Decrement the active setpoint
            else
            {
                changeSetpoint( -1.0F );
            }

            m_timer.start( OPTION_STORM_THERMOSTAT_UI_PICO_DISPLAY_DELAY_UPDATES_MS );
            updateSetpoints();
        }
    }

protected:
    /// Helper method to switch with setpoint is active
    void toggleSetpoint()
    {
        m_coolingSetpoingActive = !m_coolingSetpoingActive;
        updateSetpoints();
    }

    /// Helper method to update the state/value of the setpoints
    void updateSetpoints()
    {
        if ( m_setPointActive )
        {
            if ( m_coolingSetpoingActive )
            {
                drawCoolingSetpoint( true );
                drawHeatingSetpoint();
            }
            else
            {
                drawCoolingSetpoint();
                drawHeatingSetpoint( true );
            }
        }
        else
        {
            drawCoolingSetpoint();
            drawHeatingSetpoint();
        }
    }

    /// Helper method to change the active setpoint
    void changeSetpoint( float delta )
    {
        // Update Cooling Setpoint
        if ( m_coolingSetpoingActive )
        {
            g_uiCoolingSetpoint += delta;
      
            // Enforce cooling range
            if ( g_uiCoolingSetpoint > OPTION_STORM_DM_MP_SETPOINTS_MAX_COOLING )
            {
                g_uiCoolingSetpoint = OPTION_STORM_DM_MP_SETPOINTS_MAX_COOLING;
            }
            else if ( g_uiCoolingSetpoint < OPTION_STORM_DM_MP_SETPOINTS_MIN_COOLING )
            {
                g_uiCoolingSetpoint = OPTION_STORM_DM_MP_SETPOINTS_MIN_COOLING;
            }

            // Enforce Cooling is ALWAYS higher than the heating setpoint
            if ( g_uiCoolingSetpoint <= g_uiHeatingSetpoint )
            {
                // Note: The setpoint ranges are such that the +1 operation does not violate the other setpoint's range
                g_uiHeatingSetpoint = g_uiCoolingSetpoint - 1.0F;
            }
        }
        
        // Update Heating Setpoint
        else
        {
            g_uiHeatingSetpoint += delta;
           
            // Enforce heating range
            if ( g_uiHeatingSetpoint > OPTION_STORM_DM_MP_SETPOINTS_MAX_HEATING )
            {
                g_uiHeatingSetpoint = OPTION_STORM_DM_MP_SETPOINTS_MAX_HEATING;
            }
            else if ( g_uiHeatingSetpoint < OPTION_STORM_DM_MP_SETPOINTS_MIN_HEATING )
            {
                g_uiHeatingSetpoint = OPTION_STORM_DM_MP_SETPOINTS_MIN_HEATING;
            }

            // Enforce Cooling is ALWAYS higher than the heating setpoint 
            if ( g_uiCoolingSetpoint <= g_uiHeatingSetpoint )
            {
                // Note: The setpoint ranges are such that the +1 operation does not violate the other setpoint's range
                g_uiCoolingSetpoint = g_uiHeatingSetpoint + 1.0F;
            }
        }
 
    }

protected:
    /// 'Active selection' timer expired
    void expired() noexcept
    {
        // Update the application model points with current changes
        mp_userMode.write( g_uiMode );
        mp_fanMode.write( g_uiFan );
        mp_setpoints.write( g_uiCoolingSetpoint, g_uiHeatingSetpoint );
        m_modeActive      = false;
        m_fanActive       = false;
        m_setPointActive  = false;

        drawMode();
        drawFan();
        drawCoolingSetpoint();
        drawHeatingSetpoint();
    }

protected:
    /// Software timer for active/selected elements
    Cpl::System::TimerComposer<EventMonitor>                     m_timer;
    
    /// Observer
    Cpl::Dm::SubscriberComposer<EventMonitor, Cpl::Dm::Mp::Bool> m_obModeButton;

    /// Observer
    Cpl::Dm::SubscriberComposer<EventMonitor, Cpl::Dm::Mp::Bool> m_obFanButton;

    /// Observer
    Cpl::Dm::SubscriberComposer<EventMonitor, Cpl::Dm::Mp::Bool> m_obUpButton;

    /// Observer
    Cpl::Dm::SubscriberComposer<EventMonitor, Cpl::Dm::Mp::Bool> m_obDownButton;

    /// Observer
    Cpl::Dm::SubscriberComposer<EventMonitor, Storm::Dm::MpThermostatMode>  m_obTStatMode;

    /// Observer
    Cpl::Dm::SubscriberComposer<EventMonitor, Storm::Dm::MpFanMode>         m_obFanMode;

    /// Observer
    Cpl::Dm::SubscriberComposer<EventMonitor, Storm::Dm::MpSetpoints>       m_obSetpoints;

    /// Active state for the Mode
    bool m_modeActive;

    /// Active state for the Fan
    bool m_fanActive;

    /// Active state for the Setpoints 
    bool m_setPointActive;

    /// Which setpoint is active
    bool m_coolingSetpoingActive;
};

/*---------------------------------------------------------------------------*/
}   // end namespace(s)
}
}
}
#endif // end header latch
