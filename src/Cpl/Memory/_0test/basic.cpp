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
#include "Cpl/Memory/SPool.h"
#include "Cpl/Memory/HPool.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/_testsupport/Shutdown_TS.h"


#define SECT_   "_0test"

/// 
using namespace Cpl::Memory;



////////////////////////////////////////////////////////////////////////////////

struct struct1_T
{
	int  i;
	bool flag;
	char data[5];
};

struct struct2_T
{
	char data[5];
};


#define NUM_POOL1       5
#define NUM_POOL2       3
#define NUM_POOL3       6
#define NUM_POOL4       4
#define NUM_POOL5       5
#define NUM_POOL6       3
#define NUM_POOL7       6
#define NUM_POOL8       4

#define ALIGMENT    sizeof(size_t)


static SPool<struct2_T, NUM_POOL5>       pool5_s2;
static HPool<struct2_T>                  pool6_s2( NUM_POOL6 );
static SPool<struct1_T, NUM_POOL7>       pool7_s1( true );
static HPool<struct1_T>                  pool8_s1( NUM_POOL8, true );

static struct1_T* cache_s1[50];
static struct2_T* cache_s2[50];


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "basic", "[basic]" )
{
	Cpl::System::Shutdown_TS::clearAndUseCounter();

	SPool<struct1_T, NUM_POOL1>   pool1_s1( true );
	HPool<struct1_T>              pool2_s1( NUM_POOL2, true );
	SPool<struct2_T, NUM_POOL3>   pool3_s2;
	HPool<struct2_T>              pool4_s2( NUM_POOL4 );

	//
		// allocate test
	struct1_T* s1Ptr;
	int i;
	for ( i=0; i < NUM_POOL1; i++ )
	{
		s1Ptr       = (struct1_T*) pool1_s1.allocate( sizeof( struct1_T ) );
		cache_s1[i] = s1Ptr;
		REQUIRE( s1Ptr != 0 );
		int alignment = (size_t) s1Ptr % ALIGMENT;
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "STACK, S1, Static: ptr=%p, alignment=%d, sizeof=%p", s1Ptr, alignment, sizeof( struct1_T ) ) );
		REQUIRE( alignment == 0 );
	}
	s1Ptr = (struct1_T*) pool1_s1.allocate( sizeof( struct1_T ) );
	REQUIRE( s1Ptr == 0 );
	REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 1u );

	// release test
	for ( i=0; i < NUM_POOL1; i++ )
	{
		pool1_s1.release( cache_s1[i] );
	}
	pool1_s1.release( cache_s1[0] );
	s1Ptr = (struct1_T*) pool1_s1.allocate( sizeof( struct1_T ) );
	REQUIRE( s1Ptr != 0 );
	REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 1u );

	//

		// allocate test
	for ( i=0; i < NUM_POOL2; i++ )
	{
		s1Ptr       = (struct1_T*) pool2_s1.allocate( sizeof( struct1_T ) );
		cache_s1[i] = s1Ptr;
		int alignment = (size_t) s1Ptr % ALIGMENT;
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "STACK, S1, Heap: ptr=%p, alignment=%d, sizeof=%p", s1Ptr, alignment, sizeof( struct1_T ) ) );
		REQUIRE( alignment == 0 );
	}
	s1Ptr = (struct1_T*) pool2_s1.allocate( sizeof( struct1_T ) );
	REQUIRE( s1Ptr == 0 );
	REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 1u );

	// release test
	for ( i=0; i < NUM_POOL2; i++ )
	{
		pool2_s1.release( cache_s1[i] );
	}
	pool2_s1.release( cache_s1[0] );
	s1Ptr = (struct1_T*) pool2_s1.allocate( sizeof( struct1_T ) );
	REQUIRE( s1Ptr != 0 );
	REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 1u );


	//
		// Pool allocate test
	struct2_T* s2Ptr;
	for ( i=0; i < NUM_POOL3; i++ )
	{
		s2Ptr       = (struct2_T*) pool3_s2.allocate( sizeof( struct2_T ) );
		cache_s2[i] = s2Ptr;
		REQUIRE( s2Ptr != 0 );
		int alignment = (size_t) s2Ptr % ALIGMENT;
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "STACK, S2, Static: ptr=%p, alignment=%d, sizeof=%p", s2Ptr, alignment, sizeof( struct2_T ) ) );
		REQUIRE( alignment == 0 );
	}
	s2Ptr = (struct2_T*) pool3_s2.allocate( sizeof( struct2_T ) );
	REQUIRE( s2Ptr == 0 );
	REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );

	// release test
	for ( i=0; i < NUM_POOL3; i++ )
	{
		pool3_s2.release( cache_s2[i] );
	}
	pool3_s2.release( cache_s2[0] );
	s2Ptr = (struct2_T*) pool3_s2.allocate( sizeof( struct2_T ) );
	REQUIRE( s2Ptr != 0 );
	REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 1u );


	//
		// Pool allocate test
	for ( i=0; i < NUM_POOL4; i++ )
	{
		s2Ptr       = (struct2_T*) pool4_s2.allocate( sizeof( struct2_T ) );
		cache_s2[i] = s2Ptr;
		REQUIRE( s2Ptr != 0 );
		int alignment = (size_t) s2Ptr % ALIGMENT;
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "HEAP, S2, Static: ptr=%p, alignment=%d, sizeof=%p", s2Ptr, alignment, sizeof( struct2_T ) ) );
		REQUIRE( alignment == 0 );
	}
	s2Ptr = (struct2_T*) pool4_s2.allocate( sizeof( struct2_T ) );
	REQUIRE( s2Ptr == 0 );
	REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );

	// release test
	for ( i=0; i < NUM_POOL4; i++ )
	{
		pool4_s2.release( cache_s2[i] );
	}
	pool4_s2.release( cache_s2[0] );
	s2Ptr = (struct2_T*) pool4_s2.allocate( sizeof( struct2_T ) );
	REQUIRE( s2Ptr != 0 );
	REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 1u );


	//
		// Pool allocate test (static pool)
	for ( i=0; i < NUM_POOL5; i++ )
	{
		s2Ptr       = (struct2_T*) pool5_s2.allocate( sizeof( struct2_T ) );
		cache_s2[i] = s2Ptr;
		REQUIRE( s2Ptr != 0 );
		int alignment = (size_t) s2Ptr % ALIGMENT;
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "STATIC, S2, Static: ptr=%p, alignment=%d, sizeof=%p", s2Ptr, alignment, sizeof( struct2_T ) ) );
		REQUIRE( alignment == 0 );
	}
	s2Ptr = (struct2_T*) pool5_s2.allocate( sizeof( struct2_T ) );
	REQUIRE( s2Ptr == 0 );
	REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );

	// release test
	for ( i=0; i < NUM_POOL5; i++ )
	{
		pool5_s2.release( cache_s2[i] );
	}
	pool5_s2.release( cache_s2[0] );
	s2Ptr = (struct2_T*) pool5_s2.allocate( sizeof( struct2_T ) );
	REQUIRE( s2Ptr != 0 );
	REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 1u );


	//
		// Pool allocate test (static pool)
	for ( i=0; i < NUM_POOL6; i++ )
	{
		s2Ptr       = (struct2_T*) pool6_s2.allocate( sizeof( struct2_T ) );
		cache_s2[i] = s2Ptr;
		REQUIRE( s2Ptr != 0 );
		int alignment = (size_t) s2Ptr % ALIGMENT;
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "STATIC, S2, Heap: ptr=%p, alignment=%d, sizeof=%p", s2Ptr, alignment, sizeof( struct2_T ) ) );
		REQUIRE( alignment == 0 );
	}
	s2Ptr = (struct2_T*) pool6_s2.allocate( sizeof( struct2_T ) );
	REQUIRE( s2Ptr == 0 );
	REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );

	// release test
	for ( i=0; i < NUM_POOL6; i++ )
	{
		pool6_s2.release( cache_s2[i] );
	}
	pool6_s2.release( cache_s2[0] );
	s2Ptr = (struct2_T*) pool6_s2.allocate( sizeof( struct2_T ) );
	REQUIRE( s2Ptr != 0 );
	REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 1u );


	//
		// allocate test
	for ( i=0; i < NUM_POOL7; i++ )
	{
		s1Ptr       = (struct1_T*) pool7_s1.allocate( sizeof( struct1_T ) );
		cache_s1[i] = s1Ptr;
		REQUIRE( s1Ptr != 0 );
		int alignment = (size_t) s1Ptr % ALIGMENT;
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "STATIC, S1, Static: ptr=%p, alignment=%d, sizeof=%p", s1Ptr, alignment, sizeof( struct1_T ) ) );
		REQUIRE( alignment == 0 );
	}
	s1Ptr = (struct1_T*) pool7_s1.allocate( sizeof( struct1_T ) );
	REQUIRE( s1Ptr == 0 );
	REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 1u );

	// release test
	for ( i=0; i < NUM_POOL7; i++ )
	{
		pool7_s1.release( cache_s1[i] );
	}
	pool7_s1.release( cache_s1[0] );
	s1Ptr = (struct1_T*) pool7_s1.allocate( sizeof( struct1_T ) );
	REQUIRE( s1Ptr != 0 );
	REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 1u );

	//

		// allocate test
	for ( i=0; i < NUM_POOL8; i++ )
	{
		s1Ptr       = (struct1_T*) pool8_s1.allocate( sizeof( struct1_T ) );
		cache_s1[i] = s1Ptr;
		int alignment = (size_t) s1Ptr % ALIGMENT;
		CPL_SYSTEM_TRACE_MSG( SECT_, ( "STATIC, S1, Heap: ptr=%p, alignment=%d, sizeof=%p", s1Ptr, alignment, sizeof( struct1_T ) ) );
		REQUIRE( alignment == 0 );
	}
	s1Ptr = (struct1_T*) pool8_s1.allocate( sizeof( struct1_T ) );
	REQUIRE( s1Ptr == 0 );
	REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 1u );

	// release test
	for ( i=0; i < NUM_POOL8; i++ )
	{
		pool8_s1.release( cache_s1[i] );
	}
	pool8_s1.release( cache_s1[0] );
	s1Ptr = (struct1_T*) pool8_s1.allocate( sizeof( struct1_T ) );
	REQUIRE( s1Ptr != 0 );
	REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 1u );


	//
	REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}
