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


#include "Api.h"
#include "Cpl/Io/File/Input.h"
#include "Cpl/Io/File/Output.h"
#include "Cpl/System/Private_.h"
#include "Cpl/System/Tls.h"
#include "Cpl/System/Thread.h"
#include "Cpl/System/FatalError.h"
#include <string.h>



///
using namespace Cpl::Io::File;



/////////////////////////////////////////////////////////////////////////////
static Cpl::System::Tls* workNameTlsPtr_;


// Anonymous namespace
namespace {

class RegisterInitHandler : public Cpl::System::StartupHook_
{
public:
	///
	RegisterInitHandler() :StartupHook_( eMIDDLE_WARE ) {}


protected:
	/// 
	void notify( InitLevel_T init_level )
	{
		// Set up TLS storage for working buffer for File/Path names
		if ( !workNameTlsPtr_ )
		{
			workNameTlsPtr_ = new( std::nothrow ) Cpl::System::Tls();
		}
	}

};
}; // end namespace

///
static RegisterInitHandler autoRegister_systemInit_hook;


static bool copyHelper_( Input& src, Output& dst )
{
	int  bytesRead = 0;
	char buf[256];
	while ( src.isEof() == false )
	{
		if ( !src.read( buf, sizeof( buf ), bytesRead ) )
		{
			// was it a read error OR eof?
			return src.isEof()? true: false;
		}

		if ( !dst.write( buf, bytesRead ) && !dst.isEof() )
		{
			return false;   // write error
		}
	}

	return true;
}


/////////////////////////////////////////////////////////////////////////////
const char* Api::getNative( const char* fsEntryName )
{
	// Convert any/all directory separators -->Use Brute force to ensure I ALWAYS ended up with ALL native separators (i.e. handle mixed-separator cases)
	Cpl::Text::String& workName = getWorkBuffer();
	workName = fsEntryName;
	workName.replace( '/', nativeDirectorySep() );
	workName.replace( '\\', nativeDirectorySep() );
	return workName;
}

const char* Api::getStandard( const char* fsEntryName )
{
	// Convert any/all directory separators -->Use Brute force to ensure I ALWAYS ended up with ALL Standard separators (i.e. handle mixed-separator cases)
	Cpl::Text::String& workName = getWorkBuffer();
	workName = fsEntryName;
	workName.replace( '/', directorySep() );
	workName.replace( '\\', directorySep() );
	return workName;
}


const char* Api::dos2unix( const char* fsEntryName )
{
	Cpl::Text::String& workName = getWorkBuffer();
	workName = fsEntryName;
	workName.replace( '\\', '/' );
	return workName;
}

const char* Api::unix2dos( const char* fsEntryName )
{
	Cpl::Text::String& workName = getWorkBuffer();
	workName = fsEntryName;
	workName.replace( '/', '\\' );
	return workName;
}



/////////////////////////////////////////////////////////////////////////////
void Api::split( const char* fileEntryName,
	Cpl::Text::String* fullPath,
	Cpl::Text::String* drive,
	Cpl::Text::String* path,
	Cpl::Text::String* fullName,
	Cpl::Text::String* name,
	Cpl::Text::String* extension,
	char               dirSeparator,
	char               extensionSeparator,
	char               driveSeparator
)
{
	// Initialize all parameters to 'NULL'
	if ( drive )* drive     = "";
	if ( fullPath )* fullPath  = "";
	if ( path )* path      = "";
	if ( fullName )* fullName  = "";
	if ( name )* name      = "";
	if ( extension )* extension = "";

	// Split off the Drive letter
	const char* entry = fileEntryName;
	const char* ptr;
	if ( ( ptr=strchr( entry, driveSeparator ) ) != 0 )
	{
		int len = ptr - entry + 1;
		if ( drive )
		{
			drive->copyIn( entry, len - 1 );
		}
		if ( fullPath )
		{
			fullPath->copyIn( entry, len );
		}
		entry += len;
	}

	// Split off the path
	if ( ( ptr=strrchr( entry, dirSeparator ) ) != 0 )
	{
		int len = ptr - entry + 1;
		if ( fullPath )
		{
			fullPath->appendTo( entry, len );
		}
		if ( path )
		{
			path->copyIn( entry, len );
		}
		entry += len;
	}

	// Get full name
	if ( fullName )
	{
		*fullName = entry;
	}

	// Split off name+extension
	if ( ( ptr=strrchr( entry, extensionSeparator ) ) != 0 )
	{
		int len = ptr - entry;
		if ( name )
		{
			name->copyIn( entry, len );
		}
		if ( extension )
		{
			*extension = entry + len + 1;
		}
	}

	// Just name ->no extension
	else
	{
		if ( name )
		{
			*name = entry;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
bool Api::isFile( const char* fsEntryName )
{
	Api::Info entryInfo;
	if ( Api::getInfo( fsEntryName, entryInfo ) )
	{
		return entryInfo.m_isFile;
	}

	return false;
}


bool Api::isDirectory( const char* fsEntryName )
{
	Api::Info entryInfo;
	if ( Api::getInfo( fsEntryName, entryInfo ) )
	{
		return entryInfo.m_isDir;
	}

	return false;
}


bool Api::isReadable( const char* fsEntryName )
{
	Api::Info entryInfo;
	if ( Api::getInfo( fsEntryName, entryInfo ) )
	{
		return entryInfo.m_readable;
	}

	return false;
}


bool Api::isWriteable( const char* fsEntryName )
{
	Api::Info entryInfo;
	if ( Api::getInfo( fsEntryName, entryInfo ) )
	{
		return entryInfo.m_writeable;
	}

	return false;
}


unsigned long Api::size( const char* fsEntryName )
{
	Api::Info entryInfo;
	if ( Api::getInfo( fsEntryName, entryInfo ) )
	{
		return entryInfo.m_size;
	}
	return 0;
}


time_t Api::timeModified( const char* fsEntryName )
{
	Api::Info entryInfo;
	if ( Api::getInfo( fsEntryName, entryInfo ) )
	{
		return entryInfo.m_mtime;
	}

	return -1;
}



/////////////////////////////////////////////////////////////////////////////
bool Api::copyFile( const char* srcName, const char* dstName )
{
	if ( !isFile( srcName ) || isDirectory( dstName ) )
	{
		return false;
	}

	Input  src( srcName );
	Output dst( dstName );
	return copyHelper_( src, dst );
}


bool Api::appendFile( const char* srcName, const char* dstName )
{
	if ( !isFile( srcName ) || isDirectory( dstName ) )
	{
		return false;
	}

	Input  src( srcName );
	Output dst( dstName );
	dst.setToEof();
	return copyHelper_( src, dst );
}



/////////////////////////////////////////////////////////////////////////////
Cpl::Text::String& Api::getWorkBuffer( void )
{
	// Trap the first time this method is called (per thread)
	Cpl::Text::String* workNamePtr = ( Cpl::Text::String* ) workNameTlsPtr_->get();
	if ( workNamePtr == 0 )
	{
		workNamePtr = new( std::nothrow ) NameString;
		if ( !workNamePtr )
		{
			Cpl::System::FatalError::logf( "Failed to allocate work string for Cpl::Io::File namespace in thread=%s", Cpl::System::Thread::myName() );
		}

		workNameTlsPtr_->set( workNamePtr );
	}

	return *workNamePtr;
}

