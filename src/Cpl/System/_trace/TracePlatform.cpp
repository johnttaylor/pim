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

#include "Cpl/System/Thread.h"
#include "Cpl/System/ElapsedTime.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/SimTick.h"
#include "Cpl/System/Private_.h"
#include "Cpl/Text/format.h"


/// 
using namespace Cpl::System;

///
static Cpl::Io::Output* activePtr_ = Trace::getDefaultOutputStream_();


////////////////////////////////////////////////////////////////////////////////
void Trace::redirect_( Cpl::Io::Output& newMedia )
{
    Locks_::tracing().lock();
    activePtr_ = &newMedia;
    Locks_::tracing().unlock();
}


void Trace::revert_( void )
{
    Locks_::tracing().lock();
    activePtr_ = getDefaultOutputStream_();
    Locks_::tracing().unlock();
}



////////////////////////////////////////////////////////////////////////////////
void TracePlatform_::output( Cpl::Text::String& src )
{
    activePtr_->write( src.getString() );
}



/* Info settings:

    eNONE     -->  ">> <user message>"
    eBRIEF    -->  ">> DD HH:MM:SS.MMM (<section name>) <user message>"
    eINFO     -->  ">> DD HH:MM:SS.MMM (<section name>) [<thread name>] <user message>"
    eVERBOSE  -->  ">> DD HH:MM:SS.MMM (<section name>) [<thread name>, <thread ID>] <user message>"
    eMAX      -->  ">> DD HH:MM:SS.MMM (<section name>) [<thread name>, <thread ID>] {<filename>,<line#>[,<funcname>]} <user message>"

*/
void TracePlatform_::appendInfo( Cpl::Text::String& dst, Trace::InfoLevel_T info, const char* section, const char* filename, int linenum, const char* funcname )
{
    bool isCplThread = Cpl::System::Thread::tryGetCurrent() != nullptr;

    // Level: eBRIEF 
    if ( info > Trace::eNONE )
    {
        // Indent "simulated time" time stamps
        if ( CPL_SYSTEM_SIM_TICK_USING_SIM_TICKS() )
        {
            dst += "  ";
        }

        // Add time stamp (Note: Elapsed time may not be valid/working when
        // the scheduler has not been started - so use 'zero' instead)
        ElapsedTime::Precision_T now = { 0, 0 };
        if ( Api::isSchedulingEnabled() )
        {
            now =  ElapsedTime::precision();
        }
        formatPrecisionTimeStamp( dst, now, true, true );

        // Add section name         
        dst += " (";
        dst += section;
        dst += ") ";


        // LEVEL: eINFO
        if ( info > Trace::eBRIEF )
        {
            // Add Thread name
            dst += '[';
            dst += isCplThread? Thread::myName(): "n/a";

            // Add closing bracket when NO higher info level
            if ( info == Trace::eINFO )
            {
                dst += "] ";
            }


            // LEVEL: eVERBOSE  
            else
            {
                // Add thread id 
                dst += ", ";
                dst += isCplThread ? Thread::myId(): 0;
                dst += "] ";

                // LEVEL: eMAX
                if ( info > Trace::eVERBOSE )
                {
                    dst += '{';
                    dst += filename;
                    dst += ',';
                    dst += linenum;

                    // Skip funcname if the compiler does not support it
                    if ( funcname && *funcname != '\0' )
                    {
                        dst += ',';
                        dst += funcname;
                    }
                    dst += "} ";
                }
            }
        }
    }
}



