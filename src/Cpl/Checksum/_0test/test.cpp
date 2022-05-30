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

#include "Catch/catch.hpp"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Text/FString.h"
#include "Cpl/Checksum/Fletcher16.h"
#include "Cpl/Checksum/Crc16CcittFast.h"
#include "Cpl/Checksum/Crc32EthernetFast.h"
#include "Cpl/Checksum/Md5Aladdin.h"


/// 
using namespace Cpl::Checksum;



////////////////////////////////////////////////////////////////////////////////

#define SECT_               "_0test"

#define DATALEN_            9
#define MAX_CRCLEN_         4

#define CRC_CCITT_xFFFF     0x29B1      // CRC value for the text string "12346789"   (see http://www.zorc.breitbandkatze.de/crc.html)
#define CRC_ETHERNET        0x376E6E7   // CRC value for the text string "12346789"   (see http://www.zorc.breitbandkatze.de/crc.html, Final XOR val=0)
#define FLETCHER16          0x1e03      // Fletcher checksum for the text "123456789"


uint8_t dataBuffer_[DATALEN_ + MAX_CRCLEN_] = { '1', '2', '3', '4', '5', '6', '7', '8', '9', 0, };
static Cpl::Text::FString<128> outbuf_;


/////////////////////////////
static void printBuf( uint8_t* buf, int len, const char* msg )
{
	int i;
	outbuf_.clear();
	for ( i=0; i < len; i++ )
	{
		outbuf_.formatAppend( "%02X ", buf[i] );
	}

	CPL_SYSTEM_TRACE_MSG( SECT_, ( "%s. Buffer (len=%d): %s", msg, len, outbuf_() ) );
}


bool test16( const char* testName, Api16& cksum, uint8_t* buffer, int len, uint16_t& sum )
{
	CPL_SYSTEM_TRACE_MSG( SECT_, ( "16BIT: Test for: %s", testName ) );
	printBuf( buffer, len, "Outbound buffer" );
	cksum.reset();
	cksum.accumulate( buffer, len );
	sum = cksum.finalize( buffer + len );
	CPL_SYSTEM_TRACE_MSG( SECT_, ( "CKSUM=%04X.", sum ) );

	// Validate a 'inbound' CRC'd buffer
	printBuf( buffer, len + sizeof( uint16_t ), "Inbound packet/buffer" );
	cksum.reset();
	cksum.accumulate( buffer, len + sizeof( uint16_t ) );
	return cksum.isOkay();
}


bool test32( const char* testName, Api32& cksum, uint8_t* buffer, int len, uint32_t& sum )
{
	CPL_SYSTEM_TRACE_MSG( SECT_, ( "32BIT: Test for: %s", testName ) );
	printBuf( buffer, len, "Outbound buffer" );
	cksum.reset();
	cksum.accumulate( buffer, len );
	sum = cksum.finalize( buffer + len );
	CPL_SYSTEM_TRACE_MSG( SECT_, ( "CKSUM=%04X.", sum ) );

	// Validate a 'inbound' CRC'd buffer
	printBuf( buffer, len + sizeof( uint32_t ), "Inbound packet/buffer" );
	cksum.reset();
	cksum.accumulate( buffer, len + sizeof( uint32_t ) );
	return cksum.isOkay();
}


#define MD5_ITEMS_  7
static const char *const md5_test_data_[MD5_ITEMS_ * 2] = {
	"",                                                                                 "d41d8cd98f00b204e9800998ecf8427e",
	"a",                                                                                "0cc175b9c0f1b6a831c399e269772661",
	"abc",                                                                              "900150983cd24fb0d6963f7d28e17f72",
	"message digest",                                                                   "f96b697d7cb7938d525a2f31aaf161d0",
	"abcdefghijklmnopqrstuvwxyz",                                                       "c3fcd3d76192e4007dfb496cca67e13b",
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",                   "d174ab98d277d9f5a5611c2c9f419d9f",
	"12345678901234567890123456789012345678901234567890123456789012345678901234567890", "57edf4a22be3c955ac49da2e2107b67a"
};


/////////////////////////////
TEST_CASE( "Checksums", "[chksum]" )
{

	Cpl::System::Shutdown_TS::clearAndUseCounter();


	SECTION( "Checksum - Fletcher16" )
	{
		Fletcher16 cksum;
		uint16_t   sum;
		REQUIRE( test16( "CheckSum Fletcher16", cksum, dataBuffer_, DATALEN_, sum ) );
		REQUIRE( sum == FLETCHER16 );
	}

	SECTION( "Crc16 - CCITT - Fast" )
	{
		Crc16CcittFast cksum;
		uint16_t       crc;
		REQUIRE( test16( "CRC CCITT-FAST", cksum, dataBuffer_, DATALEN_, crc ) );
		REQUIRE( crc == CRC_CCITT_xFFFF );
	}

	SECTION( "Crc32 - Ethernet - Fast" )
	{
		Crc32EthernetFast cksum;
		uint32_t       crc;
		REQUIRE( test32( "CRC Ethernet-FAST", cksum, dataBuffer_, DATALEN_, crc ) );
		REQUIRE( crc == CRC_ETHERNET );
	}

	SECTION( "Md5 - Aladdin" )
	{
		Md5Aladdin             hash;
		Cpl::Text::FString<32> resultAsString;
		Cpl::Text::FString<32> expectedResult;
		int                    i;

		for ( i=0; i < MD5_ITEMS_ * 2; i +=2 )
		{
			hash.reset();
			hash.accumulate( md5_test_data_[i], strlen( md5_test_data_[i] ) );
			hash.finalize( &resultAsString, false );
			expectedResult = md5_test_data_[i + 1];
			CPL_SYSTEM_TRACE_MSG( SECT_, ( "in=[%s], hash=[%s] =? expected=[%s]", md5_test_data_[i], resultAsString(), expectedResult() ) );
			REQUIRE( resultAsString == expectedResult );
		}
	}

	REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}
