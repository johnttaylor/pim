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

#include "DirList_.h"
#include "Cpl/Text/strip.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>


//
using namespace Cpl::Io::File;




///////////////
DirList_::DirList_( const char* rootDir, int depth, bool filesOnly, bool dirsOnly )
    :m_depth( depth ),
    m_curDepth( 0 ),
    m_filesOnly( filesOnly ),
    m_dirsOnly( dirsOnly ),
    m_name( Api::getNative( rootDir ) )
{
}

DirList_::~DirList_()
{
}


///////////////
bool DirList_::traverse( Api::DirectoryWalker& callback )
{
    bool completed = true;
    int  added     = 0;

    // Ensure my current root ends with a trailing dir separator
    m_curDepth++;
    if ( m_name[m_name.length() - 1] != '/' )
    {
        m_name += '/';
        added++;
    }

    // Get the first item in current root
    DIR* hdlPtr = opendir( m_name() );

    // While there something in the current root
    if ( hdlPtr )
    {
        struct dirent* dirPtr = readdir( hdlPtr );
        while ( dirPtr )
        {
            const char* ptr = Cpl::Text::stripChars( dirPtr->d_name, "." );
            if ( *ptr != '\0' )
            {
                // Construct full path for the current item
                m_file  = m_name;
                m_file += dirPtr->d_name;

                // Get the File Entry info for the current item
                if ( Api::getInfo( m_file(), m_info ) )
                {
                    // Call back the Walker
                    if ( (!m_filesOnly && !m_dirsOnly) || (m_filesOnly && m_info.m_isFile) || (m_dirsOnly && m_info.m_isDir) )
                    {
                        if ( callback.item( Api::getStandard( m_name ), dirPtr->d_name, m_info ) == Cpl::Type::Traverser::eABORT )
                        {
                            completed = false;
                            break;
                        }
                    }

                    // Do I need to dive into a sub directory?
                    if ( m_info.m_isDir && m_curDepth < m_depth )
                    {
                        int len = strlen( dirPtr->d_name );
                        m_name += dirPtr->d_name;

                        // Walk the sub directory
                        if ( !traverse( callback ) )
                        {
                            completed = false;
                            break;
                        }
                        m_name.trimRight( len );
                    }
                }
            }

            // Get next item in the current root
            dirPtr = readdir( hdlPtr );
        }


        // Close my directory handle
        closedir( hdlPtr );
    }


    // Housekeeping
    m_curDepth--;
    if ( completed )
    {
        m_name.trimRight( added );
    }

    return completed;
}
