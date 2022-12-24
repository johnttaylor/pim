#include "Catch/catch.hpp"
#include "Cpl/System/Trace.h"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "Cpl/System/Api.h"
#include "Cpl/Text/atob.h"
#include "Cpl/Json/Arduino.h"


#define SECT_   "_0test"




////////////////////////////////////////////////////////////////////////////////

#define OUTPUT_LEN      1024

static StaticJsonDocument<1024> doc_;
static char output[OUTPUT_LEN + 1];
TEST_CASE( "jsonArduino" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    // Simple Parse
    char json1[]   = "{ apple:123, orange:\"hello there\", cherry:null, plum:\"0x1234\" }";
    DeserializationError err = deserializeJson( doc_, json1 );
    if ( err )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("deserializeJson::error=%s\n", err.c_str()) );
        FAIL();
    }

    int apple              = doc_["apple"];
    const char* cherry     = doc_["cherry"] | "not_found";
    const char* orange     = doc_["orange"] | "not_found";
    const char* plumstring = doc_["plum"] | "";
    int plum;
    if ( !Cpl::Text::a2i( plum, plumstring, 16 ) )
    {
        plum = -1;
    }

    REQUIRE( apple == 123 );
    REQUIRE( strcmp( cherry, "not_found" ) == 0 );
    REQUIRE( strcmp( orange, "hello there" ) == 0 );
    CPL_SYSTEM_TRACE_MSG( SECT_, ("apple=%d, cherry=[%s], orange=[%s], plum=%X\n", apple, cherry, orange, plum) );
    REQUIRE( plum != -1 );

    // Create embedded array
    doc_.clear();
    doc_["name"]  = "bob";
    JsonArray array = doc_.createNestedArray( "val" );
    array.add( 1 );
    array.add( 2 );
    array.add( 3 );
    serializeJson( doc_, output, OUTPUT_LEN );
    CPL_SYSTEM_TRACE_MSG( SECT_, ("serialization: [%s]\n", output) );

    // Create embedded string array
    doc_.clear();
    doc_["name"]  = "uncle";
    array = doc_.createNestedArray( "val" );
    array.add( "1" );
    array.add( "2" );
    array.add( "3" );
    serializeJson( doc_, output, OUTPUT_LEN );
    CPL_SYSTEM_TRACE_MSG( SECT_, ("serialization: [%s]\n", output) );

    //
    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}
