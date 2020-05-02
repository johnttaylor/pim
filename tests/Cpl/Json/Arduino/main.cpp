#include "Cpl/System/Api.h"
#include "Cpl/Text/atob.h"
#include "Cpl/Json/Arduino.h"

#define OUTPUT_LEN      1024

static StaticJsonDocument<1024> doc_;
static char output[OUTPUT_LEN + 1];
int main( int argc, char* argv[] )
{
    // Initialize Colony
    Cpl::System::Api::initialize();

    // Simple Parse
    char json1[]   = "{ apple:123, orange:\"hello there\", cherry:null, plum:\"0x1234\" }";
    DeserializationError err = deserializeJson(doc_, json1);
    if ( err )
    {
        printf( "deserializeJson::error=%s\n", err.c_str() );
    }
    else
    {
        int apple              = doc_["apple"];
        const char* cherry     = doc_["cherry"] | "not_found";
        const char* orange     = doc_["orange"] | "not_found";
        const char* plumstring = doc_["plum"] | "";
        int plum;
        if ( !Cpl::Text::a2i( plum, plumstring, 16 ) )
        {
            plum = -1;
        }

        printf( "apple=%d, cherry=[%s], orange=[%s], plum=%X\n", apple, cherry, orange, plum );
    }

    // Create embedded array
    doc_.clear();
    doc_["name"]  = "bob";
    JsonArray array = doc_.createNestedArray( "val" );
    array.add( 1 );
    array.add( 2 );
    array.add( 3 );
    serializeJson( doc_, output, OUTPUT_LEN );
    printf( "serialization: [%s]\n", output );

        // Create embedded string array
    doc_.clear();
    doc_["name"]  = "uncle";
    array = doc_.createNestedArray( "val" );
    array.add( "1" );
    array.add( "2" );
    array.add( "3" );
    serializeJson( doc_, output, OUTPUT_LEN );
    printf( "serialization: [%s]\n", output );
    return 0;
}
