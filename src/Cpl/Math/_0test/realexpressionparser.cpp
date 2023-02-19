/*-----------------------------------------------------------------------------
* COPYRIGHT_HEADER_TO_BE_FILLED_LATER
*----------------------------------------------------------------------------*/

#include "Catch/catch.hpp"
#include "Cpl/Math/RealExpressionParser.h"
#include "Cpl/Math/real.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include <stdint.h>


#define SECT_     "_0test"

/// 
using namespace Cpl::Math;


#define STR1(s) #s
#define TOSTRING(s) STR1(s)

/// Test expressions
#define EXPR1_STR   45345.0 + 0 + 0xdf234 - 1000 % 7
#define EXPR1_FLOAT 45345.0 + 0 + 0xdf234 - (fmod(1000,7))

#define EXPR2_STR   (0.0 + 0xdf234 - 1000) * 3 / 2 % 999
#define EXPR2_FLOAT fmod((0.0 + 0xdf234 - 1000) * 3 / 2,999)

#define EXPR3       1.0 / 16
#define EXPR4       3.14e2 / 100
#define EXPR6       1.0+(((2+(3+(4+(5+6)* -7.0)/8.0))/127.0)*2) *-3.0
#define EXPR7       100000000 + (1 * 256 * 256) + (1 * 256 * 256)
#define EXPR8       1-1
#define EXPR9       1.0 - 0xfFa/( ((((8+(6+(4 *(2*(1.0)*3)*5)+7)+9)))))
#define EXPRa       ((12/13.0)*16) /((1+127.0) / 10.0 / (31+7)+3.14)
#define EXPRb       ((((((((((5))))))  ))))- ((((((((( 6)))))))))
#define EXPRc       4 * .25 / 1e-1

#define EXPRd_STR   4 ** .3
#define EXPRd_FLOAT pow(4,.3)

#define EXPRe_STR   4 ** 2
#define EXPRe_FLOAT 4 * 4

#define EXP_ERR1    a + 45345
#define EXP_ERR2    2 * * 3
#define EXP_ERR3    2 + two


template <typename T>
static void compare( RealExpressionParser<T>& parser, T expectedResult, const char* str )
{
    T result;
    bool success = parser.eval( str, result );
    CPL_SYSTEM_TRACE_MSG( SECT_, ("expr=%s, expected=%g, result=%g",
                                   str,
                                   (double) expectedResult,
                                   (double) result) );
    REQUIRE( success == true );
    REQUIRE( Cpl::Math::almostEquals<T>( expectedResult, result, CPL_MATH_REAL_FLOAT_EPSILON ) );
}


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "realexpressionparser" )
{
    CPL_SYSTEM_TRACE_FUNC( SECT_ );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    SECTION( "float" )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("float") );
        RealExpressionParser<float> parser;

        compare<float>( parser, (float) (EXPR1_FLOAT), TOSTRING( EXPR1_STR ) );
        compare<float>( parser, (float) (EXPR2_FLOAT), TOSTRING( EXPR2_STR ) );
        compare<float>( parser, (float) (EXPR3), TOSTRING( EXPR3 ) );
        compare<float>( parser, (float) (EXPR4), TOSTRING( EXPR4 ) );
        compare<float>( parser, (float) (EXPR6), TOSTRING( EXPR6 ) );
        compare<float>( parser, (float) (EXPR7), TOSTRING( EXPR7 ) );
        compare<float>( parser, (float) (EXPR8), TOSTRING( EXPR8 ) );
        compare<float>( parser, (float) (EXPR9), TOSTRING( EXPR9 ) );
        compare<float>( parser, (float) (EXPRa), TOSTRING( EXPRa ) );
        compare<float>( parser, (float) (EXPRb), TOSTRING( EXPRb ) );
        compare<float>( parser, (float) (EXPRc), TOSTRING( EXPRc ) );
        compare<float>( parser, (float) (EXPRd_FLOAT), TOSTRING( EXPRd_STR ) );
        compare<float>( parser, (float) (EXPRe_FLOAT), TOSTRING( EXPRe_STR ) );
    }

    SECTION( "double" )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("double") );
        RealExpressionParser<double> parser;

        compare<double>( parser, (EXPR1_FLOAT), TOSTRING( EXPR1_STR ) );
        compare<double>( parser, (EXPR2_FLOAT), TOSTRING( EXPR2_STR ) );
        compare<double>( parser, (EXPR3), TOSTRING( EXPR3 ) );
        compare<double>( parser, (EXPR4), TOSTRING( EXPR4 ) );
        compare<double>( parser, (EXPR6), TOSTRING( EXPR6 ) );
        compare<double>( parser, (EXPR7), TOSTRING( EXPR7 ) );
        compare<double>( parser, (EXPR8), TOSTRING( EXPR8 ) );
        compare<double>( parser, (EXPR9), TOSTRING( EXPR9 ) );
        compare<double>( parser, (EXPRa), TOSTRING( EXPRa ) );
        compare<double>( parser, (EXPRb), TOSTRING( EXPRb ) );
        compare<double>( parser, (EXPRc), TOSTRING( EXPRc ) );
        compare<double>( parser, (EXPRd_FLOAT), TOSTRING( EXPRd_STR ) );
        compare<double>( parser, (EXPRe_FLOAT), TOSTRING( EXPRe_STR ) );
    }


    SECTION( "Errors" )
    {
        RealExpressionParser<double> parser;

        double result;
        bool success = parser.eval( TOSTRING( EXP_ERR1 ), result );
        REQUIRE( success == false );
        success = parser.eval( TOSTRING( EXP_ERR2 ), result );
        REQUIRE( success == false );
        success = parser.eval( TOSTRING( EXP_ERR3 ), result );
        REQUIRE( success == false );
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}

