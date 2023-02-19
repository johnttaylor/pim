/*-----------------------------------------------------------------------------
* COPYRIGHT_HEADER_TO_BE_FILLED_LATER
*----------------------------------------------------------------------------*/

#include "Catch/catch.hpp"
#include "Cpl/Math/IntegerExpressionParser.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include <stdint.h>


#define SECT_     "_0test"

/// 
using namespace Cpl::Math;


#define STR1(s) #s
#define TOSTRING(s) STR1(s)

/// Test expressions
#define EXPR1       45345 + 0 + 0xdf234 - 1000 % 7
#define EXPR2       (0 + 0xdf234 - 1000) * 3 / 2 % 999
#define EXPR3       1 << 16
#define EXPR4       (0 + ~(0xdf234 & 1000) * 3) / -2
#define EXPR4u      (0 + ~(0xdf234 & 1000) * -3) / 2
#define EXPR5       ((1 << 16) + (1 << 16)) >> 0X5
#define EXPR6       1+(((2+(3+(4+(5+6)* -7)/8))&127)<<1) *-3
#define EXPR6u      1+(((2+(3+(4+(5+6)* 7)/8))&127)<<1) * 3
#define EXPR7       100000000 + (1 << 16) + (1 << 16)
#define EXPR8       1-~1
#define EXPR9       1- ~1*0xfFa/( ((((8+(6|(4 *(2*(1)*3)*5)|7)+9)))))
#define EXPRa       ((12|13)<<8)>>((1|127) %10&(31+7))
#define EXPRb       ((((((((((5))))))  ))))- ((((((((( 6)))))))))
#define EXPRc_STR   4 ** 3
#define EXPRc_INT   4 * 4 * 4

#define EXP_ERR1    a + 45345
#define EXP_ERR2    2 * * 3
#define EXP_ERR3    2 + two
#define EXP_ERR4    2 + 3.14


template <typename T>
static void compare( IntegerExpressionParser<T>& parser, T expectedResult, const char* str )
{
    T result;
    bool success = parser.eval( str, result );
    CPL_SYSTEM_TRACE_MSG( SECT_, ("expr=%s, expected=%ld, result=%ld",
                                   str,
                                   (long) expectedResult,
                                   (long) result) );
    REQUIRE( success == true );
    REQUIRE( expectedResult == result );
}

template <typename T>
static void compareUnsigned( IntegerExpressionParser<T>& parser, T expectedResult, const char* str )
{
    T result;
    bool success = parser.eval( str, result );
    CPL_SYSTEM_TRACE_MSG( SECT_, ("expr=%s, expected=%lu, result=%lu",
                                   str,
                                   (unsigned long) expectedResult,
                                   (unsigned long) result) );
    REQUIRE( success == true );
    REQUIRE( expectedResult == result );
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "integerexpressionparser" )
{
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    SECTION( "Integer" )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("integerexpressionparser" ) );
        IntegerExpressionParser<int> parser;

        compare<int>( parser, EXPR1, TOSTRING( EXPR1 ) );
        compare<int>( parser, EXPR2, TOSTRING( EXPR2 ) );
        compare<int>( parser, EXPR3, TOSTRING( EXPR3 ) );
        compare<int>( parser, EXPR4, TOSTRING( EXPR4 ) );
        compare<int>( parser, EXPR5, TOSTRING( EXPR5 ) );
        compare<int>( parser, EXPR6, TOSTRING( EXPR6 ) );
        compare<int>( parser, EXPR7, TOSTRING( EXPR7 ) );
        compare<int>( parser, EXPR8, TOSTRING( EXPR8 ) );
        compare<int>( parser, EXPR9, TOSTRING( EXPR9 ) );
        compare<int>( parser, EXPRa, TOSTRING( EXPRa ) );
        compare<int>( parser, EXPRb, TOSTRING( EXPRb ) );
        compare<int>( parser, EXPRc_INT, TOSTRING( EXPRc_STR ) );
    }

    SECTION( "uint32_t" )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("uint32_t" ) );
        IntegerExpressionParser<uint32_t> parser;

        compareUnsigned<uint32_t>( parser, EXPR1, TOSTRING( EXPR1 ) );
        compareUnsigned<uint32_t>( parser, EXPR2, TOSTRING( EXPR2 ) );
        compareUnsigned<uint32_t>( parser, EXPR3, TOSTRING( EXPR3 ) );
        compareUnsigned<uint32_t>( parser, EXPR4u, TOSTRING( EXPR4u ) );
        compareUnsigned<uint32_t>( parser, EXPR5, TOSTRING( EXPR5 ) );
        compareUnsigned<uint32_t>( parser, EXPR6u, TOSTRING( EXPR6u ) );
        compareUnsigned<uint32_t>( parser, EXPR7, TOSTRING( EXPR7 ) );
        compareUnsigned<uint32_t>( parser, EXPR8, TOSTRING( EXPR8 ) );
        compareUnsigned<uint32_t>( parser, EXPRa, TOSTRING( EXPRa ) );
        compareUnsigned<uint32_t>( parser, EXPRb, TOSTRING( EXPRb ) );
    }

    SECTION( "Errors" )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Errors") );
        IntegerExpressionParser<int> parser;

        int result;
        bool success = parser.eval( TOSTRING( EXP_ERR1 ), result );
        REQUIRE( success == false );
        success = parser.eval( TOSTRING( EXP_ERR2 ), result );
        REQUIRE( success == false );
        success = parser.eval( TOSTRING( EXP_ERR3 ), result );
        REQUIRE( success == false );
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}

