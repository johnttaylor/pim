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

#include "colony_config.h"
#include "Catch/catch.hpp"
#include "Cpl/Io/File/Api.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "Cpl/Text/FString.h"
#include <time.h>


#define SECT_     "_0test"


/** Hack to get around the fact the SCM systems will convert newline 
    characters in Text Files based on the target host, i.e. on Windows my 
    newline character will be 2 chars, whereas on posix/linux it will be
    a single character -->hence delta in the file length.
 */
#ifndef TESTING_POSIX
#define TESTINPUT_TXT_FILE_LENGTH   106
#define TESTINPUT_TEXT_HELLO_OFFEST 0x5D
#else
#define TESTINPUT_TXT_FILE_LENGTH   101
#define TESTINPUT_TEXT_HELLO_OFFEST 0x58
#endif

/// 
using namespace Cpl::Io::File;

#define TEST_PATH1      "A:/hello/world.txt"
#define TEST_PATH2      "world.txt"
#define TEST_PATH3      "world"
#define TEST_PATH4      ".txt"
#define TEST_PATH5      "A:/hello/world.txt/foo"
#define TEST_PATH6      "A:/hello/world.txt/foo/bar/just.kidding"
#define TEST_PATH7      "/hello"
#define TEST_PATH8      "/hello/foo"
#define TEST_PATH9      "A:hello."

#define TEST_DOS_PATH1  "A:\\hello\\world.txt"
#define TEST_DOS_PATH2  "world.txt"
#define TEST_DOS_PATH3  "world"
#define TEST_DOS_PATH4  ".txt"
#define TEST_DOS_PATH5  "A:\\hello\\world.txt\\foo"
#define TEST_DOS_PATH6  "A:\\hello\\world.txt\\foo\\bar\\just.kidding"
#define TEST_DOS_PATH7  "\\hello"
#define TEST_DOS_PATH8  "\\hello\\foo"
#define TEST_DOS_PATH9  "A:hello."


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
namespace {

class Walker: public Api::DirectoryWalker
{
public:
    int        m_depth;
    bool       m_files;
    bool       m_dirs;
    NameString m_workName;
    bool       m_contentCheck;

public:
    ///
    Walker( int depth, bool files=true, bool dirs=true)
    :m_depth(depth),
     m_files(files),
     m_dirs(dirs),
     m_contentCheck(true) 
        {
        }

public:
    Cpl::Type::Traverser::Status_T item( const char* currentParent, const char* fsEntryName, Api::Info& entryInfo )
        {
        // File check
        m_workName = fsEntryName;
        if ( entryInfo.m_isFile )
            {
            if ( !m_files )
                {
                m_contentCheck = false;
                }

            else if ( m_depth > 2  )
                {
                if ( m_workName != "d1.txt" && m_workName != "d2.txt" && m_workName != "d3.txt" )
                    {
                    m_contentCheck = false;
                    }
                }
            else if ( m_depth > 1 )
                {
                if ( m_workName != "d1.txt" && m_workName != "d2.txt" )
                    {
                    m_contentCheck = false;
                    }
                }
            else if ( m_workName != "d1.txt" )
                {
                m_contentCheck = false;
                }
            }
                                    
        // Dir check
        m_workName = fsEntryName;
        if ( entryInfo.m_isDir )
            {
            if ( !m_dirs )
                {
                m_contentCheck = false;
                }

            else if ( m_depth >= 2 )
                {
                if ( m_workName != "d2" && m_workName != "d3" )
                    {
                    m_contentCheck = false;
                    }
                }
            else if ( m_depth >= 1 )
                {
                if ( m_workName != "d2" )
                    {
                    m_contentCheck = false;
                    }
                }
            else 
                {
                m_contentCheck = false;
                }
            }

        CPL_SYSTEM_TRACE_MSG( SECT_, ( "%s%s Parent=%s, item=%s.  (content=%d)", entryInfo.m_isFile? "f": "-", entryInfo.m_isDir? "d": "-", currentParent, fsEntryName, m_contentCheck ) );
        return Cpl::Type::Traverser::eCONTINUE;
        }
};

}; // end anonymous namespace


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "api", "[api]" )
    {
    CPL_SYSTEM_TRACE_FUNC( SECT_ );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    NameString name;
    NameString name2;
    
    ///
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "Walk directories..." ));
    REQUIRE( Api::createDirectory( "d1" ) );
    REQUIRE( Api::createFile( "d1/d1.txt" ) );
    REQUIRE( Api::createDirectory( "d1/d2" ) );
    REQUIRE( Api::createFile( "d1/d2/d2.txt" ) );
    REQUIRE( Api::createDirectory( "d1/d2/d3" ) );
    REQUIRE( Api::createFile( "d1/d2/d3/d3.txt" ));
    {
        Walker iterator(3);
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "Walk 'd1', 100" ));
        REQUIRE( Api::walkDirectory( "d1",   iterator, 100 ) );
        REQUIRE( iterator.m_contentCheck );
    }
    {
        Walker iterator(3,true,false);
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "Walk 'd1', 100" ));
        REQUIRE( Api::walkDirectory( "d1",   iterator, 100, true,false ) );
        REQUIRE( iterator.m_contentCheck );
    }
    {
        Walker iterator(3,false,true);
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "Walk 'd1', 100" ));
        REQUIRE( Api::walkDirectory( "d1",   iterator, 100, false,true ) );
        REQUIRE( iterator.m_contentCheck );
    }

    {
        Walker iterator(2);
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "Walk 'd1', 2" ));
        REQUIRE( Api::walkDirectory( "d1",   iterator, 2 ) );
        REQUIRE( iterator.m_contentCheck );
    }
    {
        Walker iterator(2,true,false);
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "Walk 'd1', 2" ));
        REQUIRE( Api::walkDirectory( "d1",   iterator, 2, true,false ) );
        REQUIRE( iterator.m_contentCheck );
    }
    {
        Walker iterator(2,false,true);
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "Walk 'd1', 2" ));
        REQUIRE( Api::walkDirectory( "d1",   iterator, 2, false,true ) );
        REQUIRE( iterator.m_contentCheck );
    }

    {
        Walker iterator(1);
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "Walk 'd1', 1" ));
        REQUIRE( Api::walkDirectory( "d1",   iterator, 1 ) );
        REQUIRE( iterator.m_contentCheck );
    }
    {
        Walker iterator(1,true,false);
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "Walk 'd1', 12" ));
        REQUIRE( Api::walkDirectory( "d1",   iterator, 1, true,false ) );
        REQUIRE( iterator.m_contentCheck );
    }
    {
        Walker iterator(1,false,true);
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "Walk 'd1', 1" ));
        REQUIRE( Api::walkDirectory( "d1",   iterator, 1, false,true ) );
        REQUIRE( iterator.m_contentCheck );
    }

    REQUIRE( Api::createDirectory( "d1/d2/d3/d4" ) );
    REQUIRE( Api::createFile( "d1/d2/d3/d3a.txt" ));
    {
        Walker iterator(3,true,false);
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "Walk 'd1', 100" ));
        REQUIRE( Api::walkDirectory( "d1",   iterator, 100, true,false ) );
        REQUIRE( iterator.m_contentCheck == false );
    }
    {
        Walker iterator(3,false,true);
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "Walk 'd1', 100" ));
        REQUIRE( Api::walkDirectory( "d1",   iterator, 100, false,true ) );
        REQUIRE( iterator.m_contentCheck == false);
    }

    REQUIRE( Api::remove( "d1/d2/d3/d4" ) );
    REQUIRE( Api::remove( "d1/d2/d3/d3.txt" ));
    REQUIRE( Api::remove( "d1/d2/d3/d3a.txt" ));
    REQUIRE( Api::remove( "d1/d2/d3" ) );
    REQUIRE( Api::remove( "d1/d2/d2.txt" ) );
    REQUIRE( Api::remove( "d1/d2" ) );
    REQUIRE( Api::remove( "d1/d1.txt" ) );
    REQUIRE( Api::remove( "d1" ) );
   

    ///
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "Copy, Appended..." ));
    REQUIRE( Api::exists( "copy.txt" ) == false );
    REQUIRE( Api::exists( "copy2.txt" ) == false );
    REQUIRE( Api::copyFile( "testinput.txt", "copy.txt" ) );
    REQUIRE( Api::isFile( "copy.txt" ) );
    REQUIRE( Api::size( "copy.txt" ) == TESTINPUT_TXT_FILE_LENGTH );
    REQUIRE( Api::appendFile( "testinput.txt", "copy2.txt" ) );
    REQUIRE( Api::size( "copy2.txt" ) == TESTINPUT_TXT_FILE_LENGTH );
    REQUIRE( Api::appendFile( "testinput.txt", "copy.txt" ) );
    REQUIRE( Api::size( "copy.txt" ) == 2 * TESTINPUT_TXT_FILE_LENGTH );
    REQUIRE( Api::remove( "copy.txt" ) );
    REQUIRE( Api::isFile( "copy.txt" ) == false );
    REQUIRE( Api::remove( "copy2.txt" ) );
    REQUIRE( Api::exists( "copy2.txt" ) == false );

    ///
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "CanonicalPath & getCwd..." ));
    REQUIRE( Api::canonicalPath( ".", name ) );
    REQUIRE( Api::getCwd( name2 ) );
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "[%s] ('.') =? [%s] ('cwd')", name.getString(), name2.getString() ));
    REQUIRE( name == name2 );
    REQUIRE( Api::canonicalPath( "..", name ) );
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "[%s] (cwd) startsWith? [%s] ('..')", name2.getString(), name.getString() ));
    REQUIRE( name2.startsWith( name ) );
    
    ///
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "Create, move, remove, etc. ..." ));
    REQUIRE( Api::exists( "bob" ) == false );
    REQUIRE( Api::createDirectory( "bob" ) );
    REQUIRE( Api::exists( "bob" ) );
    REQUIRE( Api::isDirectory( "bob" ) );
    REQUIRE( Api::exists( "uncle.txt" ) == false );
    REQUIRE( Api::createFile( "bob/uncle.txt" ) );
    REQUIRE( Api::exists( "bob/uncle.txt" ) );
    REQUIRE( Api::isFile( "bob/uncle.txt" ) );
    
    REQUIRE( Api::exists( "charlie" ) == false );
    REQUIRE( Api::renameInPlace( "bob", "charlie" ) );
    REQUIRE( Api::isDirectory( "charlie" ) );
    REQUIRE( Api::isFile( "charlie/uncle.txt" ) );

    REQUIRE( Api::exists( "uncle.txt" ) == false );
    REQUIRE( Api::moveFile( "charlie/uncle.txt", "uncle.txt" ) );
    REQUIRE( Api::isFile( "uncle.txt" ) );
    REQUIRE( Api::renameInPlace( "uncle.txt", "your.txt" ) );
    REQUIRE( Api::isFile( "your.txt" ) );
    
    REQUIRE( Api::createFile( "charlie/test.txt" ) );
    REQUIRE( Api::exists( "charlie/test.txt" ) );
    REQUIRE( Api::remove( "charlie" ) == false );
    REQUIRE( Api::remove( "charlie/test.txt" ) );
    REQUIRE( Api::remove( "charlie" ) );
    REQUIRE( Api::exists( "charlie" ) == false );
    REQUIRE( Api::remove( "your.txt" ) );
    REQUIRE( Api::exists( "your.txt" ) == false );
    

    ///
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "Info..." ));
    REQUIRE( Api::exists( ".." ) );
    REQUIRE( Api::exists( "testinput.txt" ) );
    REQUIRE( Api::exists( "nothere.txt" ) == false );
    REQUIRE( Api::isFile( "." ) == false );
    REQUIRE( Api::isFile( "testinput.txt" ) );
    REQUIRE( Api::isDirectory( "." ) == true );
    REQUIRE( Api::isDirectory( ".." ) == true );
    REQUIRE( Api::isDirectory( "testinput.txt" ) == false );
    REQUIRE( Api::isReadable( "." ) );
    REQUIRE( Api::isReadable( ".." ) );
    REQUIRE( Api::isReadable( "testinput.txt" ) );
    REQUIRE( Api::isReadable( "testinput.txte" ) == false );
    REQUIRE( Api::isWriteable( "." ) );
    REQUIRE( Api::isWriteable( ".." ) );
    REQUIRE( Api::isWriteable( "testinput.txt" ) );
    REQUIRE( Api::isWriteable( "testinput.txtdd" ) == false );
    REQUIRE( Api::size( "testinput.txt" ) == TESTINPUT_TXT_FILE_LENGTH  );
    time_t t1,t2,t3;
    t1 = Api::timeModified( "." );
    t2 = Api::timeModified( ".." );
    t3 = Api::timeModified( "testinput.txt" );
    name = ctime( &t2 );
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "'..' mtime           := %s", name.getString() ));
    name = ctime( &t1 );
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "'.' mtime            := %s", name.getString() ));
    name = ctime( &t3 );
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "'testinput.txt' mtime:= %s", name.getString() ));
    REQUIRE( t1 != ((time_t)-1) );
    REQUIRE( t2 != ((time_t)-1) );
    REQUIRE( t3 != ((time_t)-1) );



    ///
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "Split..." ));
    Api::splitDrive( TEST_PATH1, name );
    REQUIRE( name == "A" );
    Api::splitDrive( TEST_PATH2, name );
    REQUIRE( name == "" );
    Api::splitDrive( TEST_PATH3, name );                                                                                           
    REQUIRE( name == "" );
    Api::splitDrive( TEST_PATH4, name );
    REQUIRE( name == "" );
    Api::splitDrive( TEST_PATH5, name );
    REQUIRE( name == "A" );
    Api::splitDrive( TEST_PATH6, name );
    REQUIRE( name == "A" );
    Api::splitDrive( TEST_PATH7, name );
    REQUIRE( name == "" );
    Api::splitDrive( TEST_PATH8, name );
    REQUIRE( name == "" );
    Api::splitDrive( TEST_PATH9, name );
    REQUIRE( name == "A" );

    Api::splitFullpath( TEST_PATH1, name );
    REQUIRE( name == "A:/hello/" );
    Api::splitFullpath( TEST_PATH2, name );
    REQUIRE( name == "" );
    Api::splitFullpath( TEST_PATH3, name );
    REQUIRE( name == "" );
    Api::splitFullpath( TEST_PATH4, name );
    REQUIRE( name == "" );
    Api::splitFullpath( TEST_PATH5, name );
    REQUIRE( name == "A:/hello/world.txt/" );
    Api::splitFullpath( TEST_PATH6, name );
    REQUIRE( name == "A:/hello/world.txt/foo/bar/" );
    Api::splitFullpath( TEST_PATH7, name );
    REQUIRE( name == "/" );
    Api::splitFullpath( TEST_PATH8, name );
    REQUIRE( name == "/hello/" );
    Api::splitFullpath( TEST_PATH9, name );
    REQUIRE( name == "A:" );

    Api::splitPath( TEST_PATH1, name );
    REQUIRE( name == "/hello/" );
    Api::splitPath( TEST_PATH2, name );
    REQUIRE( name == "" );
    Api::splitPath( TEST_PATH3, name );
    REQUIRE( name == "" );
    Api::splitPath( TEST_PATH4, name );
    REQUIRE( name == "" );
    Api::splitPath( TEST_PATH5, name );
    REQUIRE( name == "/hello/world.txt/" );
    Api::splitPath( TEST_PATH6, name );
    REQUIRE( name == "/hello/world.txt/foo/bar/" );
    Api::splitPath( TEST_PATH7, name );
    REQUIRE( name == "/" );
    Api::splitPath( TEST_PATH8, name );
    REQUIRE( name == "/hello/" );
    Api::splitPath( TEST_PATH9, name );
    REQUIRE( name == "" );

    Api::splitFullname( TEST_PATH1, name );
    REQUIRE( name == "world.txt" );
    Api::splitFullname( TEST_PATH2, name );
    REQUIRE( name == "world.txt" );
    Api::splitFullname( TEST_PATH3, name );
    REQUIRE( name == "world" );
    Api::splitFullname( TEST_PATH4, name );
    REQUIRE( name == ".txt" );
    Api::splitFullname( TEST_PATH5, name );
    REQUIRE( name == "foo" );
    Api::splitFullname( TEST_PATH6, name );
    REQUIRE( name == "just.kidding" );
    Api::splitFullname( TEST_PATH7, name );
    REQUIRE( name == "hello" );
    Api::splitFullname( TEST_PATH8, name );
    REQUIRE( name == "foo" );
    Api::splitFullname( TEST_PATH9, name );
    REQUIRE( name == "hello." );

    Api::splitName( TEST_PATH1, name );
    REQUIRE( name == "world" );
    Api::splitName( TEST_PATH2, name );
    REQUIRE( name == "world" );
    Api::splitName( TEST_PATH3, name );
    REQUIRE( name == "world" );
    Api::splitName( TEST_PATH4, name );
    REQUIRE( name == "" );
    Api::splitName( TEST_PATH5, name );
    REQUIRE( name == "foo" );
    Api::splitName( TEST_PATH6, name );
    REQUIRE( name == "just" );
    Api::splitName( TEST_PATH7, name );
    REQUIRE( name == "hello" );
    Api::splitName( TEST_PATH8, name );
    REQUIRE( name == "foo" );
    Api::splitName( TEST_PATH9, name );
    REQUIRE( name == "hello" );

    Api::splitExtension( TEST_PATH1, name );
    REQUIRE( name == "txt" );
    Api::splitExtension( TEST_PATH2, name );
    REQUIRE( name == "txt" );
    Api::splitExtension( TEST_PATH3, name );
    REQUIRE( name == "" );
    Api::splitExtension( TEST_PATH4, name );
    REQUIRE( name == "txt" );
    Api::splitExtension( TEST_PATH5, name );
    REQUIRE( name == "" );
    Api::splitExtension( TEST_PATH6, name );
    REQUIRE( name == "kidding" );
    Api::splitExtension( TEST_PATH7, name );
    REQUIRE( name == "" );
    Api::splitExtension( TEST_PATH8, name );
    REQUIRE( name == "" );
    Api::splitExtension( TEST_PATH9, name );
    REQUIRE( name == "" );

    Api::splitFullpathFullName( TEST_PATH1, name, name2 );
    REQUIRE( name == "A:/hello/" );
    REQUIRE( name2 == "world.txt" );
    Api::splitFullpathFullName( TEST_PATH2, name, name2 );
    REQUIRE( name == "" );
    REQUIRE( name2 == "world.txt" );
    Api::splitFullpathFullName( TEST_PATH3, name, name2 );
    REQUIRE( name == "" );
    REQUIRE( name2 == "world" );
    Api::splitFullpathFullName( TEST_PATH4, name, name2 );
    REQUIRE( name == "" );
    REQUIRE( name2 == ".txt" );
    Api::splitFullpathFullName( TEST_PATH5, name, name2 );
    REQUIRE( name == "A:/hello/world.txt/" );
    REQUIRE( name2 == "foo" );
    Api::splitFullpathFullName( TEST_PATH6, name, name2 );
    REQUIRE( name == "A:/hello/world.txt/foo/bar/" );
    REQUIRE( name2 == "just.kidding" );
    Api::splitFullpathFullName( TEST_PATH7, name, name2 );
    REQUIRE( name == "/" );
    REQUIRE( name2 == "hello" );
    Api::splitFullpathFullName( TEST_PATH8, name, name2 );
    REQUIRE( name == "/hello/" );
    REQUIRE( name2 == "foo" );
    Api::splitFullpathFullName( TEST_PATH9, name, name2 );
    REQUIRE( name == "A:" );
    REQUIRE( name2 == "hello." );


    name = Api::dos2unix( TEST_DOS_PATH1 );
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "PATH=[%s], dos2unix=[%s]", TEST_DOS_PATH1, name.getString() ) );
    REQUIRE( name == TEST_PATH1 );
    name = Api::dos2unix( TEST_DOS_PATH2 );
    REQUIRE( name == TEST_PATH2 );
    name = Api::dos2unix( TEST_DOS_PATH3 );
    REQUIRE( name == TEST_PATH3 );
    name = Api::dos2unix( TEST_DOS_PATH4 );
    REQUIRE( name == TEST_PATH4 );
    name = Api::dos2unix( TEST_DOS_PATH5 );
    REQUIRE( name == TEST_PATH5 );
    name = Api::dos2unix( TEST_DOS_PATH6 );
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "PATH=[%s], dos2unix=[%s]", TEST_DOS_PATH6, name.getString() ) );
    REQUIRE( name == TEST_PATH6 );
    name = Api::dos2unix( TEST_DOS_PATH7 );
    REQUIRE( name == TEST_PATH7 );
    name = Api::dos2unix( TEST_DOS_PATH8 );
    REQUIRE( name == TEST_PATH8 );
    name = Api::dos2unix( TEST_DOS_PATH9 );
    REQUIRE( name == TEST_PATH9 );

    name = Api::getStandard( TEST_DOS_PATH1 );
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "PATH=[%s], getStandard=[%s]", TEST_DOS_PATH1, name.getString() ) );
    REQUIRE( name == TEST_PATH1 );
    name = Api::getStandard( TEST_DOS_PATH2 );
    REQUIRE( name == TEST_PATH2 );
    name = Api::getStandard( TEST_DOS_PATH3 );
    REQUIRE( name == TEST_PATH3 );
    name = Api::getStandard( TEST_DOS_PATH4 );
    REQUIRE( name == TEST_PATH4 );
    name = Api::getStandard( TEST_DOS_PATH5 );
    REQUIRE( name == TEST_PATH5 );
    name = Api::getStandard( TEST_DOS_PATH6 );
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "PATH=[%s], getStandard=[%s]", TEST_DOS_PATH6, name.getString() ) );
    REQUIRE( name == TEST_PATH6 );
    name = Api::getStandard( TEST_DOS_PATH7 );
    REQUIRE( name == TEST_PATH7 );
    name = Api::getStandard( TEST_DOS_PATH8 );
    REQUIRE( name == TEST_PATH8 );
    name = Api::getStandard( TEST_DOS_PATH9 );
    REQUIRE( name == TEST_PATH9 );

    name = Api::unix2dos( TEST_PATH1 );
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "PATH=[%s], unix2dos=[%s]", TEST_PATH1, name.getString() ) );
    REQUIRE( name == TEST_DOS_PATH1 );
    name = Api::unix2dos( TEST_PATH2 );
    REQUIRE( name == TEST_DOS_PATH2 );
    name = Api::unix2dos( TEST_PATH3 );
    REQUIRE( name == TEST_DOS_PATH3 );
    name = Api::unix2dos( TEST_PATH4 );
    REQUIRE( name == TEST_DOS_PATH4 );
    name = Api::unix2dos( TEST_PATH5 );
    REQUIRE( name == TEST_DOS_PATH5 );
    name = Api::unix2dos( TEST_PATH6 );
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "PATH=[%s], unix2dos=[%s]", TEST_PATH6, name.getString() ) );
    REQUIRE( name == TEST_DOS_PATH6 );
    name = Api::unix2dos( TEST_PATH7 );
    REQUIRE( name == TEST_DOS_PATH7 );
    name = Api::unix2dos( TEST_PATH8 );
    REQUIRE( name == TEST_DOS_PATH8 );
    name = Api::unix2dos( TEST_PATH9 );
    REQUIRE( name == TEST_DOS_PATH9 );

    name = Api::getStandard( TEST_PATH1 );
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "PATH=[%s], getStandard=[%s]", TEST_PATH1, name.getString() ) );
    REQUIRE( name == TEST_PATH1 );
    name = Api::getStandard( TEST_PATH2 );
    REQUIRE( name == TEST_PATH2 );
    name = Api::getStandard( TEST_PATH3 );
    REQUIRE( name == TEST_PATH3 );
    name = Api::getStandard( TEST_PATH4 );
    REQUIRE( name == TEST_PATH4 );
    name = Api::getStandard( TEST_PATH5 );
    REQUIRE( name == TEST_PATH5 );
    name = Api::getStandard( TEST_PATH6 );
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "PATH=[%s], getStandard=[%s]", TEST_PATH6, name.getString() ) );
    REQUIRE( name == TEST_PATH6 );
    name = Api::getStandard( TEST_PATH7 );
    REQUIRE( name == TEST_PATH7 );
    name = Api::getStandard( TEST_PATH8 );
    REQUIRE( name == TEST_PATH8 );
    name = Api::getStandard( TEST_PATH9 );
    REQUIRE( name == TEST_PATH9 );

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
    }
