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

#include "Storm/Thermostat/Logger.h"
#include "ModelPoints.h"
#include "Storm/Thermostat/ModelPoints.h"
#include "Cpl/Text/FString.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Io/File/Output.h"
#include <new>

#define SECT_   "Logging"

using namespace Storm::Thermostat;

static Cpl::Text::FString<1024> entryBuffer_;
static Cpl::Io::File::Output*   fd_;
static bool isLoggingEnabled();

//////////////////////
void Logger::recordSystemData( Cpl::System::ElapsedTime::Precision_T currentInterval )
{
    if ( isLoggingEnabled() )
    {
        bool                            systemOn;
        float                           idt, odt, err, pvOut, pvSum, cool, heat;
        Storm::Type::HvacRelayOutputs_T relays;
        Storm::Type::SystemConfig_T     sysCfg;
        Storm::Type::ThermostatMode     thermostatMode = Storm::Type::ThermostatMode::eOFF;
        Storm::Type::FanMode            fanMode = Storm::Type::FanMode::eAUTO;
        if ( Cpl::Dm::ModelPoint::IS_VALID( mp_activeIdt.read( idt ) ) == true &&
             Cpl::Dm::ModelPoint::IS_VALID( mp_outdoorTemp.read( odt ) ) == true &&
             Cpl::Dm::ModelPoint::IS_VALID( mp_pvOut.read( pvOut ) ) == true &&
             Cpl::Dm::ModelPoint::IS_VALID( mp_sumError.read( pvSum ) ) == true &&
             Cpl::Dm::ModelPoint::IS_VALID( mp_deltaIdtError.read( err ) ) == true &&
             Cpl::Dm::ModelPoint::IS_VALID( mp_relayOutputs.read( relays ) ) == true &&
             Cpl::Dm::ModelPoint::IS_VALID( mp_setpoints.read( cool, heat ) ) == true &&
             Cpl::Dm::ModelPoint::IS_VALID( mp_systemOn.read( systemOn ) ) == true &&
             Cpl::Dm::ModelPoint::IS_VALID( mp_systemConfig.read( sysCfg ) ) == true &&
             Cpl::Dm::ModelPoint::IS_VALID( mp_userMode.read( thermostatMode ) ) == true &&
             Cpl::Dm::ModelPoint::IS_VALID( mp_fanMode.read( fanMode ) ) == true )
        {
            // HEADER: "TimeMs,TimeSec,TimeMin,TimeHrs Idt,Odt,Err, pvOut,pvSum, sysOn, G,BK,Y1,Y2,W1,W2,W3,SovIsCool, CoolSetpt,HeatSetpt, OpMode,UserMode,FanMode" 
            unsigned long timeMs = currentInterval.m_seconds * 1000 + currentInterval.m_thousandths;
            entryBuffer_.format( "%lu,%lu,%lu,%lu, %g,%g,%g, %g,%g, %d, %d,%d,%d,%d,%d,%d,%d,%d, %g,%g, %s,%s,%s\n",
                                 timeMs, timeMs/1000, timeMs/(1000*60), timeMs / ( 1000 * 60 *60),
                                 idt, odt, err,
                                 pvOut, pvSum,
                                 systemOn,
                                 relays.g, relays.bk, relays.y1, relays.y2, relays.w1, relays.w2, relays.w3, relays.o,
                                 cool, heat,
                                 Storm::Type::OperatingMode::_from_integral_unchecked( sysCfg.currentOpMode )._to_string(),
                                 thermostatMode._to_string(), fanMode._to_string() );
            fd_->write( entryBuffer_ );
            fd_->flush();
        }
    }
}

bool isLoggingEnabled()
{
    Cpl::Text::FString<OPTION_LOGGER_MAX_FILE_NAME> fname;
    int8_t valid = mp_loggingFileName.read( fname );

    // Transition to valid -->Start Logging
    if ( fd_ == nullptr && Cpl::Dm::ModelPoint::IS_VALID( valid ) == true )
    {
        fd_ = new( std::nothrow ) Cpl::Io::File::Output( fname.getString(), true, true );
        if ( fd_ == nullptr )
        {
            CPL_SYSTEM_TRACE_MSG( SECT_, ( "Error. Failed to create logfile: %s", fname ) );
        }
        else
        {
            // Write header row
            fd_->write( "TimeMs,TimeSec,TimeMin,TimeHrs,Idt,Odt,Err,pvOut,pvSum,sysOn,G,BK,Y1,Y2,W1,W2,W3,SovIsCool,CoolSetpt,HeatSetpt,OpMode,UserMode,FanMode\n" );
        }
    }

    // Transition to invalid -->Turn OFF logging
    else if ( fd_ && Cpl::Dm::ModelPoint::IS_VALID( valid ) == false )
    {
        fd_->close();
        delete fd_;
        fd_ = nullptr;
    }

    return fd_ != nullptr;
}
