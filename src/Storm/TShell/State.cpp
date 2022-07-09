/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2020  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/

#include "State.h"
#include "Cpl/Text/atob.h"
#include "Cpl/Text/format.h"
#include "Cpl/Text/Tokenizer/TextBlock.h"
#include "Cpl/System/ElapsedTime.h"
#include "Storm/Thermostat/ModelPoints.h"
#include <string.h>         


///
using namespace Storm::TShell;

///////////////////////////
State::State( Cpl::Container::Map<Cpl::TShell::Command>& commandList ) noexcept
    :Command( commandList, STORMTSHELLCMD_CMD_STATE_ )
{
}


///////////////////////////
Cpl::TShell::Command::Result_T State::execute( Cpl::TShell::Context_& context, char* cmdString, Cpl::Io::Output& outfd ) noexcept
{
    Cpl::Text::Tokenizer::TextBlock tokens( cmdString, context.getDelimiterChar(), context.getTerminatorChar(), context.getQuoteChar(), context.getEscapeChar() );
    Cpl::Text::String&              outtext  = context.getOutputBuffer();
    unsigned                        numParms = tokens.numParameters();
    bool                            io       = true;

    // Error checking
    if ( numParms > 2 )
    {
        return Cpl::TShell::Command::eERROR_EXTRA_ARGS;
    }

    // All output
    if ( numParms == 1 )
    {
        io  &= outputUser( context );
        io  &= outputSystem( context );
        io  &= outputRelays( context );
        io  &= outputLoad( context );
        io  &= outputAlarms( context );
        io  &= outputCycle( context );
        io  &= outputTimes( context );
    }

    //  User
    else
    {
        const char* ptr = tokens.getParameter( 1 );
        while ( ptr && *ptr != '\0' )
        {
            switch ( *ptr )
            {
            case 'u':
                io  &= outputUser( context );
                break;
            case 's':
                io  &= outputSystem( context );
                break;
            case 'r':
                io  &= outputRelays( context );
                break;
            case 'a':
                io  &= outputAlarms( context );
                break;
            case 'l':
                io  &= outputLoad( context );
                break;
            case 'c':
                io  &= outputCycle( context );
                break;
            case 't':
                io  &= outputTimes( context );
                break;

            default:
                break;
            }
            ptr++;
        }
    }

    // Print the current SIM time
    Cpl::System::ElapsedTime::Precision_T simTime  = Cpl::System::ElapsedTime::precision();
    Cpl::Text::formatPrecisionTimeStamp( outtext, simTime, true );
    io  &= context.writeFrame( outtext.getString() );

    // If I get here -->the command syntax was correct
    return io ? Command::eSUCCESS : Command::eERROR_IO;
}


bool State::outputUser( Cpl::TShell::Context_ & context )
{
    Storm::Type::ThermostatMode mode = Storm::Type::ThermostatMode::eOFF;
    Storm::Type::FanMode        fan  = Storm::Type::FanMode::eAUTO;
    float                       cool;
    float                       heat;
    float                       idt;
    float                       odt;
    if ( mp_userMode.read( mode ) == false ||
         mp_fanMode.read( fan ) == false ||
         mp_setpoints.read( cool, heat ) == false ||
         mp_activeIdt.read( idt ) == false )
    {
        return context.writeFrame( "User: One or more inputs are invalid!" );
    }

    Cpl::Text::String& outtext  = context.getOutputBuffer();
    if ( mp_outdoorTemp.read( odt ) )   // ODT is 'optional'
    {
        outtext.format( "ThermostatMode=%s, FanMode=%s, CoolSetpt=%0.02f, HeatSetpt=%0.02f, idt=%0.02f, odt=%0.02f",
                        mode._to_string(), fan._to_string(), cool, heat, idt, odt );
    }
    else
    {
        outtext.format( "ThermostatMode=%s, FanMode=%s, CoolSetpt=%0.02f, HeatSetpt=%0.02f, idt=%0.02f, odt=<n/a>",
                        mode._to_string(), fan._to_string(), cool, heat, idt );
    }
    return context.writeFrame( outtext.getString() );
}

bool State::outputSystem( Cpl::TShell::Context_ & context )
{
    bool                        systemOn;
    Storm::Type::SystemConfig_T sysCfg;
    uint32_t                    force;
    if ( mp_systemOn.read( systemOn ) == false ||
         mp_systemConfig.read( sysCfg ) == false ||
         mp_systemForcedOffRefCnt.read( force ) == false )
    {
        return context.writeFrame( "System: One or more inputs are invalid!" );
    }

    Cpl::Text::String& outtext  = context.getOutputBuffer();
    outtext.format( "SystemOn=%s, OpMode=%s, Idu=%s, Odu=%s, IduStages=%d, OduStages=%d, force=%d",
                    systemOn ? "ON" : "off",
                    Storm::Type::OperatingMode::_from_integral_unchecked( sysCfg.currentOpMode )._to_string(),
                    Storm::Type::IduType::_from_integral_unchecked( sysCfg.indoorUnitType )._to_string(),
                    Storm::Type::OduType::_from_integral_unchecked( sysCfg.outdoorUnitType )._to_string(),
                    sysCfg.numIndoorStages, sysCfg.numCompressorStages, force );
    return context.writeFrame( outtext.getString() );
}

bool State::outputRelays( Cpl::TShell::Context_ & context )
{
    Storm::Type::HvacRelayOutputs_T relays;
    if ( mp_relayOutputs.read( relays ) == false )
    {
        return context.writeFrame( "Relays: One or more inputs are invalid!" );
    }

    Cpl::Text::String& outtext  = context.getOutputBuffer();
    outtext.format( "g=%s, bk=%d,  w1=%s, w2=%s, w3=%s,  y1=%s, y2=%s,  o=%s",
                    relays.g ? "ON" : "off", relays.bk,
                    relays.w1 ? "ON" : "off", relays.w2 ? "ON" : "off", relays.w3 ? "ON" : "off",
                    relays.y1 ? "ON" : "off", relays.y2 ? "ON" : "off", relays.o ? "COOL" : "heat" );
    return context.writeFrame( outtext.getString() );
}

bool State::outputLoad( Cpl::TShell::Context_ & context )
{
    bool     pvInhibit;
    float    pv, err, sum;
    uint32_t freezeCnt, inhibitCnt;

    if ( mp_pvOut.read( pv ) == false ||
         mp_deltaIdtError.read( err ) == false ||
         mp_sumError.read( sum ) == false ||
         mp_freezePiRefCnt.read( freezeCnt ) == false ||
         mp_pvInhibited.read( pvInhibit ) == false ||
         mp_inhibitfRefCnt.read( inhibitCnt ) == false )
    {
        return context.writeFrame( "Load: One or more inputs are invalid!" );
    }
    Cpl::Text::String& outtext  = context.getOutputBuffer();
    outtext.format( "pv=%0.02f, error=%0.02f, sum=%0.02f, pvInhibit=%s, freezeCnt=%d, inhibitCnt=%d",
                    pv, err, sum, pvInhibit ? "YES" : "no", freezeCnt, inhibitCnt );
    return context.writeFrame( outtext.getString() );
}

bool State::outputAlarms( Cpl::TShell::Context_ & context )
{
    Storm::Dm::MpSimpleAlarm::Data  noActiveCond;
    Storm::Dm::MpSimpleAlarm::Data  userCfgMode;
    Storm::Dm::MpSimpleAlarm::Data  airFilter;
    Storm::Dm::MpIdtAlarm::Data     idt;
    if ( mp_noActiveConditioningAlarm.read( noActiveCond ) == false ||
         mp_userCfgModeAlarm.read( userCfgMode ) == false ||
         mp_airFilterAlert.read( airFilter ) == false ||
         mp_idtAlarms.read( idt ) == false )
    {
        return context.writeFrame( "Alarms: One or more inputs are invalid!" );
    }

    Cpl::Text::String& outtext  = context.getOutputBuffer();
    outtext.format( "IdtSensorAlarm:            pri=%s, sec=%s, priAck=%s, secAck=%s, critical=%s",
                    idt.primaryAlarm ? "YES" : "no", idt.secondaryAlarm ? "YES" : "no",
                    idt.primaryAck ? "YES" : "no", idt.secondaryAck ? "YES" : "no",
                    idt.critical ? "YES" : "no" );
    bool io = context.writeFrame( outtext.getString() );
    outtext.format( "NoActiveConditioningAlarm: active=%s, ack=%s, critical=%s",
                    noActiveCond.active ? "YES" : "no", noActiveCond.acked ? "YES" : "no", noActiveCond.critical ? "YES" : "no" );
    io &= context.writeFrame( outtext.getString() );
    outtext.format( "UserCfgModeAlarm:          active=%s, ack=%s, critical=%s",
                    userCfgMode.active ? "YES" : "no", userCfgMode.acked ? "YES" : "no", userCfgMode.critical ? "YES" : "no" );
    io &= context.writeFrame( outtext.getString() );
    outtext.format( "AirFilterAlert:            active=%s, ack=%s, critical=%s",
                    airFilter.active ? "YES" : "no", airFilter.acked ? "YES" : "no", airFilter.critical ? "YES" : "no" );
    io &= context.writeFrame( outtext.getString() );
    return io;
}

bool State::outputCycle( Cpl::TShell::Context_ & context )
{
    Storm::Type::CycleInfo_T cycle;
    if ( mp_cycleInfo.read( cycle ) == false )
    {
        return context.writeFrame( "Cycle: One or more inputs are invalid!" );
    }

    Cpl::Text::String& outtext  = context.getOutputBuffer();
    outtext.format( "Cycle=%s, BeginOn=", Storm::Type::CycleStatus::_from_integral_unchecked( cycle.mode )._to_string() );
    Cpl::Text::formatPrecisionTimeStamp( outtext, cycle.beginOnTime, false, true );
    outtext.formatAppend( ", TimeOn=%d, BeginOff=", cycle.onTime );
    Cpl::Text::formatPrecisionTimeStamp( outtext, cycle.beginOffTime, false, true );
    outtext.formatAppend( ", TimeOff=%d", cycle.offTime );
    return context.writeFrame( outtext.getString() );
}

bool State::outputTimes( Cpl::TShell::Context_ & context )
{
    Storm::Type::EquipmentTimes_T equip;
    if ( mp_equipmentBeginTimes.read( equip ) == false )
    {
        return context.writeFrame( "Times: One or more inputs are invalid!" );
    }

    Cpl::Text::String& outtext  = context.getOutputBuffer();
    outtext.format( "Outdoor: BeginOn=" );
    Cpl::Text::formatPrecisionTimeStamp( outtext, equip.outdoorUnitBeginOnTime, false, true );
    outtext += ", BeginOff=";
    Cpl::Text::formatPrecisionTimeStamp( outtext, equip.outdoorUnitBeginOffTime, false, true );
    bool io =  context.writeFrame( outtext.getString() );
    outtext.format( "Indoor:  BeginOn=" );
    Cpl::Text::formatPrecisionTimeStamp( outtext, equip.indoorUnitBeginOnTime, false, true );
    outtext += ", BeginOff=";
    Cpl::Text::formatPrecisionTimeStamp( outtext, equip.indoorUnitBeginOffTime, false, true );
    io &= context.writeFrame( outtext.getString() );
    outtext.format( "System:  BeginOn=" );
    Cpl::Text::formatPrecisionTimeStamp( outtext, equip.systemBeginOnTime, false, true );
    outtext += ", BeginOff=";
    Cpl::Text::formatPrecisionTimeStamp( outtext, equip.systemBeginOffTime, false, true );
    io &= context.writeFrame( outtext.getString() );
    return io;
}