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
/** @file */

#include "ArduinoHelpers.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Text/DString.h"

using namespace Cpl::Json;

static void dump( Cpl::Text::String& dst, const JsonVariant &variant, int nesting );
static void dump( Cpl::Text::String& dst, bool value, int nesting );
static void dump( Cpl::Text::String& dst, long value, int nesting );
static void dump( Cpl::Text::String& dst, double value, int nesting );
static void dump( Cpl::Text::String& dst, const char *str, int nesting );
static void dump( Cpl::Text::String& dst, const JsonObject &obj, int nesting );
static void dump( Cpl::Text::String& dst, const JsonArray &arr, int nesting );
static void indent( Cpl::Text::String& dst, int nesting );

//////////////////////////////////
void Cpl::Json::dumpToTrace( const char* traceSection, const JsonVariant& srcObjToDump ) noexcept
{
    Cpl::Text::DString buf( "", 2 * 2048, 512 );
    dump( buf, srcObjToDump, 0 );
    CPL_SYSTEM_TRACE_MSG( traceSection, ("\n%s", buf.getString() ));
}


void dump( Cpl::Text::String& dst, const JsonVariant &variant, int nesting )
{
    if ( variant.is<bool>() )
    {
        dump( dst, variant.as<bool>(), nesting );
    }
    else if ( variant.is<long>() )
    {
        dump( dst, variant.as<long>(), nesting );
    }
    else if ( variant.is<double>() )
    {
        dump( dst, variant.as<double>(), nesting );
    }
    else if ( variant.is<const char *>() )
    {
        dump( dst, variant.as<const char *>(), nesting );
    }
    else if ( variant.is<JsonObject>() )
    {
        dump( dst, variant.as<JsonObject>(), nesting );
    }
    else if ( variant.is<JsonArray>() )
    {
        dump( dst, variant.as<JsonArray>(), nesting );
    }
    else 
    {
        indent( dst, nesting );
        dst += "Undefined";
    }
}

void dump( Cpl::Text::String& dst, bool value, int nesting ) 
{
    indent( dst, nesting );
    dst += "Bool: ";
    dst += value ? "true" : "false";
    dst += "\n";
}

void dump( Cpl::Text::String& dst, long value, int nesting ) 
{
    indent( dst, nesting );
    dst += "Integer: ";
    dst += value;
    dst += "\n";
}

void dump( Cpl::Text::String& dst, double value, int nesting ) 
{
    indent( dst, nesting );
    dst += "Float: ";
    dst.formatAppend( "%g", value );
    dst += "\n";
}

void dump( Cpl::Text::String& dst, const char *str, int nesting ) 
{
    indent( dst, nesting );
    dst += "String: ";
    dst += str;
    dst += "\n";
}

void dump( Cpl::Text::String& dst, const JsonObject &obj, int nesting ) 
{
    indent( dst, nesting );
    dst += "Object (size = ";
    dst += obj.size();
    dst += "):\n";

    // Iterate though all key-value pairs
    for ( JsonPair kvp : obj ) 
    {
        // Print the key
        indent( dst, nesting + 1 );
        dst += "[\"";
        dst += kvp.key().c_str();
        dst += "\"]\n";

        // Print the value
        dump( dst, kvp.value(), nesting + 2 ); // <- RECURSION
    }
}

void dump( Cpl::Text::String& dst, const JsonArray &arr, int nesting ) 
{
    indent( dst, nesting );
    dst += "Array (size = ";
    dst += arr.size();
    dst += ")\n";

    int index = 0;
    // Iterate though all elements
    for ( JsonVariant value : arr ) {
        // Print the index
        indent( dst, nesting + 1 );
        dst += "[";
        dst += index;
        dst += "]\n";

        // Print the value
        dump( dst, value, nesting + 2 ); // <- RECURSION

        index++;
    }
}

void indent( Cpl::Text::String& dst, int nesting )
{
    for ( int i = 0; i < nesting; i++ )
        dst += "  ";
}
