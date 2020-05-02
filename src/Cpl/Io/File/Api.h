#ifndef Cpl_Io_File_Api_h_
#define Cpl_Io_File_Api_h_
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
/** @file */ 


#include "colony_map.h"
#include "Cpl/Text/FString.h"
#include "Cpl/Type/Traverser.h"
#include <time.h>



///
namespace Cpl { namespace Io { namespace File {

/// The platform's native directory separator
#define CPL_IO_FILE_NATIVE_DIR_SEP              CPL_IO_FILE_NATIVE_DIR_SEP_MAP


/// The size, in bytes, of the maximum allowed path/file name
#define CPL_IO_FILE_MAX_NAME                     CPL_IO_FILE_MAX_NAME_MAP


/** This typed FString is guaranteed to be large enough to hold the
    maximum allowed file system name for the native OS/Platform
 */
typedef Cpl::Text::FString<CPL_IO_FILE_MAX_NAME> NameString;



/** This concrete class defines the interface for handling and manipulating
    entries in the System's File system.  
    
    The directory separator is ALWAYS  '/'.  If necessary it will be internally 
    converted to the OS/platform  specific delimiter. Directory paths can 
    contain drive designators, but ONLY if the native OS supports it.
 */
class Api
{
public:
    /// Standardized Directory separator
    static inline char directorySep() { return '/'; }

    /// Returns the native Directory separator
    static inline char nativeDirectorySep() { return CPL_IO_FILE_NATIVE_DIR_SEP; }


public:
    /** Extracts the drive letter from the complete name. The drive letter
        will NOT contain the drive separator character.
     */
    static inline void splitDrive( const char*         fsEntryName, 
                                   Cpl::Text::String&  drive, 
                                   char                driveSeparator=':'
                                 )                       
                                 { split( fsEntryName, 0, &drive, 0,0,0,0, directorySep(), '.', driveSeparator); }


    /// Extracts the full path (drive+path) from the complete name.
    static inline void splitFullpath( const char*          fsEntryName, 
                                      Cpl::Text::String&   fullpath, 
                                      char                 dirSeparator=directorySep(),
                                      char                 extensionSeparator='.',
                                      char                 driveSeparator=':'
                                    ) 
                                    { split( fsEntryName, &fullpath, 0,0,0,0,0, dirSeparator, extensionSeparator, driveSeparator ); }

    /// Extracts the path (no drive) from the complete name.
    static inline void splitPath( const char*          fsEntryName, 
                                  Cpl::Text::String&   path, 
                                  char                 dirSeparator=directorySep(),
                                  char                 extensionSeparator='.',
                                  char                 driveSeparator=':'
                                ) 
                                { split( fsEntryName, 0,0, &path, 0,0,0, dirSeparator, extensionSeparator, driveSeparator ); }


    /// Extracts the full file name (name+extension) from the complete name.
    static inline void splitFullname( const char*          fsEntryName, 
                                      Cpl::Text::String&   fullname, 
                                      char                 dirSeparator=directorySep(),
                                      char                 extensionSeparator='.',
                                      char                 driveSeparator=':'
                                    ) 
                                    { split( fsEntryName, 0,0,0, &fullname, 0,0, dirSeparator, extensionSeparator, driveSeparator ); }

    /// Extracts the file name from the complete name.
    static inline void splitName( const char*          fsEntryName, 
                                  Cpl::Text::String&   name, 
                                  char                 dirSeparator=directorySep(),
                                  char                 extensionSeparator='.',
                                  char                 driveSeparator=':'
                                ) 
                                { split( fsEntryName, 0,0,0,0, &name, 0, dirSeparator, extensionSeparator, driveSeparator ); }


    /// Extracts the file extension from the complete name.
    static inline void splitExtension( const char*         fsEntryName, 
                                       Cpl::Text::String&  extension, 
                                       char                dirSeparator=directorySep(),
                                       char                extensionSeparator='.',
                                       char                driveSeparator=':'
                                     ) 
                                     { split( fsEntryName, 0,0,0,0,0, &extension, dirSeparator, extensionSeparator, driveSeparator ); }



    /// Extracts the full path and name from the complete name.
    static inline void splitFullpathFullName( const char*          fsEntryName, 
                                              Cpl::Text::String&   fullpath, 
                                              Cpl::Text::String&   fullname,
                                              char                 dirSeparator=directorySep(),
                                              char                 extensionSeparator='.',
                                              char                 driveSeparator=':'
                                          ) 
                                          { split( fsEntryName, &fullpath, 0,0, &fullname, 0,0, dirSeparator, extensionSeparator, driveSeparator ); }


    /** This method breaks the specified File System Entry name into it's
        individual components.  If the specific component is not
        need, then specify 0 as the argument and it will be skipped.
        The fullPath argument contains the drive+path. Path only contains 
        the path.  The fullName argument contains the file name+extension.  
        Name only contains the file name.  On return, fullPath string (if 
        specified and drive exists) will contain the driver separator.  The 
        drive string will NOT 'include' the drive separator. The fullName 
        parameter will contain the extensionSeperator.  The name and
        extension will NOT 'include' the extensionSeperator. 

        Note: 'fullPath' and 'path' WILL contain the trailing directory
              separator.
     */
    static void split( const char*         fsEntryName,
                       Cpl::Text::String*  fullPath,
                       Cpl::Text::String*  drive,
                       Cpl::Text::String*  path,
                       Cpl::Text::String*  fullName,
                       Cpl::Text::String*  name,
                       Cpl::Text::String*  extension,
                       char                dirSeparator = directorySep(),
                       char                extensionSeparator = '.',
                       char                driveSeparator = ':'
                     );



public:
    /** This method expands the relative path name in 'relPath' to its 
        'absPath'.  Returns true if successful, else false if an error
        occurred (bad relative path, not enough buffer space to hold
        the complete path, etc.)
     */
    static bool canonicalPath( const char* relPath, Cpl::Text::String& absPath );

    /** This method returns the canonical/absolute path of the current
        working directory.  Returns true if successful, else false if
        an error occurred (not enough buffer space to hold the path, etc.).
     */
    static bool getCwd( Cpl::Text::String& cwd );


public:
    /** Returns the file system entry name in its in native format. The scope
        of the returned value is limited, i.e. the value needs to be 
        consumed BEFORE any other call to this interface and/or methods
        in the Cpl::Io::File* namespaces.  Note: This method IS thread safe
        in that each thread has it own internal storage that is used for
        the translation.
     */
    static const char* getNative( const char* fsEntryName );

    /** Returns the file system entry name in the 'Standard' format. The scope
        of the returned value is limited, i.e. the value needs to be 
        consumed BEFORE any other call to this interface and/or methods
        in the Cpl::Io::File* namespaces.  Note: This method IS thread safe
        in that each thread has it own internal storage that is used for
        the translation.
     */
    static const char* getStandard( const char* fsEntryName );

    /** This method converts the DOS directory separator to the UNIX
        directory separator.  The scope of the returned value is limited, i.e.
        the value needs to be consumed BEFORE any other call to this
        interface  and/or methods in the Cpl::Io::File* namespaces.  Note:
        This method IS thread safe in that each thread has it own internal
        storage that is used for the translation.
     */
    static const char* dos2unix( const char* fsEntryName );

    /** This method converts the UNIX directory separator to the
        DOS directory separator.  The scope of the returned value is limited,
        i.e. the value needs to be consumed BEFORE any other call to this
        interface  and/or methods in the Cpl::Io::File* namespaces.  Note: This
        method IS thread safe in that each thread has it own internal storage
        that is used for the translation.
     */
    static const char* unix2dos( const char* fsEntryName );


public:
    /** Returns true if the File Entry exists physically exists
        in the OS's file system.
     */
    static bool exists( const char* fsEntryName );

    /// Returns true if the File Entry is a file. 
    static bool isFile( const char* fsEntryName );

    /// Returns true if the File Entry is a directory
    static bool isDirectory( const char* fsEntryName );

    /// Return true if the user has read permission for the file system entry
    static bool isReadable( const char* fsEntryName );
            
    /// Return true if the user has write permission for the file system entry
    static bool isWriteable( const char* fsEntryName );

    /** Returns the size, in bytes, of the file.  If the File Entry is not
        a file or an error occurs, then 0 is returned.
     */
    static unsigned long size( const char* fsEntryName );              

    /** Returns the time/date the file entry was last modified. If an error
        occurred, then  ((time_t)-1) is returned.
     */
    static time_t timeModified( const char* fsEntryName );


public:
    /// This data structure defines status attributes for a file system entry
    struct Info
        {
        /// Size, in bytes, of the file entry
        unsigned long   m_size;

        /// Time the file entry was last modified/written
        time_t          m_mtime;

        /// True if the file entry is readable by the application
        bool            m_readable;

        /// True if the file entry is writable by the application
        bool            m_writeable;

        /// True if the file entry is a directory
        bool            m_isDir;

        /// True if the file entry is a file
        bool            m_isFile;
        };


    /** Returns information about the file system entry.  If there is any
        error, the function returns false; else true is returned.
     */
    static bool getInfo( const char* fsEntryName, Info& infoOut );


public:
    /** Creates the 'fileName' as an empty file.  The parent directory must 
        exist. Returns true if successful.
     */
    static bool createFile( const char* fileName );

    /** Creates the 'dirName' as a directory.  The parent directory must 
        exist. Returns true if successful.
     */
    static bool createDirectory( const char* dirName );


public:
    /** Renames the file system entry name in the OS's file system.  NOTE: This
        is NOT a general purpose 'move' that can change the path as well
        as the name.  This method only effects the entry's name ->no changes
        to its path is made. Returns true if successful.
     */
    static bool renameInPlace( const char* oldName, const char* newName );

    /** Moves and/or renames the specified file.  The method returns true if 
        successful; else false is returned.  When false is returned the state 
        of the 'srcName' and 'dstName' files is undetermined (more specifically
        the state of files of failed moveFile() command is platform specific).

        NOTE: This method can NOT be used to move directories.
     */
    static bool moveFile( const char* oldFileName, const char* newFileName );

    /** Deletes the file system entry from the OS's file system.  If the entry 
        is an directory, the directory must be empty before it is deleted.  
        Returns true if successful.
     */
    static bool remove( const char* fsEntryName );
        
    /** Copies the context of the 'srcName' file to the specified 'dstName'
        file.  Returns true if successful.  NOTE: Obviously this method will 
        fail if both names are NOT files.
     */
    static bool copyFile( const char* srcName, const char* dstName );

    /** Appends the context of the 'srcName' file to the specified 'dstName'
        file. Returns true if successful.  NOTE: Obviously this method will 
        fail if both names are NOT files.
     */
    static bool appendFile(  const char* srcName, const char* destFile );


public:
    /** This abstract class defines the client interface for walking
        the contents of a directory, i.e. defines the callback method for
        when walking/traversing the entries in a directory
     */
    class DirectoryWalker
    {
    public: 
        /// Virtual Destructor
        virtual ~DirectoryWalker(){}
    
    public: 
        /** This method is called once for ever item in the "traversee's"
            list.  The return code from the method is used by the traverser
            to continue the traversal (eCONTINUE), or abort the traversal 
            (eABORT).
         */
        virtual Cpl::Type::Traverser::Status_T item( const char* currentParent, 
                                                     const char* fsEntryName, 
                                                     Api::Info&  entryInfo ) = 0;
    };


    /** This method allows the caller to walk the contents of the 'dirToList' 
        directory (assuming the entry is a directory).  The default behavior is
        to list only the current directory.  The Caller can override the depth
        of the traversal.  Also, by default only the file name without its path
        is returned.  The caller can optionally have the traverse call only
        return the name of files (omitting any directory names found) OR names
        of directories (omitting any file names found).  Returns true when
        successfully and the entire traversal complete; else false is return if 
        their is an error (e.g. 'dirToList' is a file or does not exist) or the
        'walker' aborted the traversal.

        NOTE: The is no guaranteed order to the traversal, only that all items
              per the specified depth will be traversed.

        WARNING: On most platforms, the underlying OS calls dynamically 
                 allocate memory. The OS clean-up after itself but there still 
                 is the risk of failure do to lack of memory.  Memory failures 
                 are handled silently - but will not crash traversal.
       
        WARNING: Be careful when recursing into subdirectories.  There are 
                 typically OS limits on how many directories may be 'opened'.  
                 Extremely deep directory trees run the risk of exceeding these 
                 limits.

     */
    static bool walkDirectory( const char*      dirToList,
                               DirectoryWalker& callback, 
                               int              depth     =1, 
                               bool             filesOnly =false,
                               bool             dirsOnly  =false
                             );


protected:
    /// Helper method.  Returns an internal work buffer large enough to 'max path'
    static Cpl::Text::String& getWorkBuffer(void);

};



};      // end namespaces
};
};
#endif  // end header latch

