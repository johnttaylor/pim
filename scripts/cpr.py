#! /usr/bin/env python3
"""
 
cpr is essential a wrapper to the 'cp -r src/ dst/' shell command
================================================================================
usage: cpr [options] <srcroot> <srcdir> <dstroot> [<dstdir>]
       cpr [--help]

Arguments:
    <srcroot>      Specifies the parent directory to '<srcdir>'
    <srcdir>       Specified the source directory to be recursively copied to
                   '<dstroot>'
    <dstroot>      Specifies the parent directory for the destination directory
    <dstdir>       Optional name for the destination directory.  If not 
                   specified then the '<srcdir>' name is used
                         
Options:
    -v             Be verbose 
    -w             Disable warnings
    -h, --help     Displays this information

Examples:
    ; Copies the colony.core/src to pim/src.  Assumes that cpr is being invoked
    ; in the immediate parent directory to colony.core/ and pim/
    cpr colony.core src pim

      
"""

import sys
import os
import shutil

from docopt.docopt import docopt
import utils



#------------------------------------------------------------------------------
# Excludes certain files
def exclude_file( file ):
    gitdir = os.sep + ".git" + os.sep
    if ( gitdir in file ):
        return True
    if ( '__pycache__' in file ):
        return True
    if ( os.sep + 'gcc' + os.sep + 'xpkgs' in file ):
        return True
    if ( os.sep + 'vc12' + os.sep + 'xpkgs' in file ):
        return True
    if ( os.sep + 'mingw_w64' + os.sep + 'xpkgs' in file ):
        return True

    return False

#------------------------------------------------------------------------------
# BEGIN
if __name__ == '__main__':

    # Parse command line
    args = docopt(__doc__, version='0.0.1')
    
    # Set quite & verbose modes
    utils.set_quite_mode( args['-w'] )
    utils.set_verbose_mode( args['-v'] )
    
    # get the list of source files
    srcpath  = os.path.join( os.path.abspath(args['<srcroot>']), args['<srcdir>'] )
    utils.print_verbose( "srcpath = " + srcpath )
    utils.push_dir( srcpath )
    srcfiles = utils.walk_file_list( '*', '.' );
    utils.pop_dir()

    # setup the destination
    dstroot  = os.path.join( args['<dstroot>'] )
    dstdir   = args['<srcdir>'] if not args['<dstdir>'] else args['<dstdir>']
    dstpath  = os.path.join( dstroot, dstdir )
    utils.print_verbose( "dstroot = " + dstpath )
    if ( os.path.isfile( dstpath ) ):
        sys.exit( "ERROR: The Parent destination directory <{}> already exists as a FILE.".format( dstroot ) )
    if ( os.path.isfile( dstpath ) ):
        sys.exit( "ERROR: The destination directory <{}> already exists as a FILE.".format( dstdir ) )
    utils.mkdirs( dstpath )
    utils.push_dir( dstpath )

    # Copy the files
    num_warnings = 0
    num_files    = 0
    for h in srcfiles:
        # skip certain files (e.g. .git/ )
        if ( not exclude_file( h ) ):
            # housekeeping
            num_files = num_files + 1
            src       = os.path.join( srcpath, h )
            utils.print_verbose( "cp " + src + " TO " + h  )
            if ( os.path.exists( h ) ):
                utils.print_warning( "destination file - {} - already exists - the file will be overwritten".format( h ) )
                num_warnings = num_warnings + 1
        
            # copy the file
            d = os.path.split(h)[0]
            utils.mkdirs( d )
            try:
                shutil.copyfile( src, h )
            except:
                sys.exit( "ERROR: Failed to copy {} TO {} [{}]".format( src, h, sys.exc_info()[0] ) )

    # Restore original CWD
    utils.pop_dir()
    print( "Copied {} files with {} warnings".format( num_files, num_warnings ) )
        ## Package is local
        #if ( h.startswith(common_args['-w']) ):
        #    parts   = h.replace(common_args['-w'],'',1).split(os.sep)
        #    pkgpath = os.path.join( *parts[2:] )
        #    path    = os.path.join( common_args['-w'], OUTCAST_XINC_DIRNAME(), pkgpath )
        
        ## Package is external
        #else:
        #    parts   = h.replace(common_args['-p'],'',1).split(os.sep)
        #    pkgpath = os.path.join( *parts[4:] )
        #    path    = os.path.join( common_args['-w'], OUTCAST_XINC_DIRNAME(), pkgpath )

        
        #skip         = False
        #parent, file = os.path.split( path )
        #utils.mkdirs( parent )
        
        ## Generate a warning if exporting an non-existant header file
        #if ( not os.path.isfile(h) ):        
        #    utils.print_warning( "Attempted to mount non-existed file: {}".format( h ) )
            
        ## Export the file
        #else:
        #    # Trap the file already exists in the xinc/ directory
        #    if ( os.path.exists(path) ):
        #        try:
        #            linksrc = utils.get_pkg_source( path, srcpath, common_args['-w'], OUTCAST_LOCAL_PKG_SUFFIX() )
        #        except OSError as ex:
        #            exit( ex )
            
        #        prev = linksrc
        #        if ( os.sep not in prev ):
        #            prev = prev + os.sep + OUTCAST_LOCAL_PKG_SUFFIX()
                
                
        #        if ( not args['--override'] ):
        #            if ( prev == linksrc ):
        #                utils.print_warning( "Skipping mounting file: {}.  The current file is the same branch/version".format(prev) )
        #                skip = True
        #            else:
        #                exit( "ERROR: The file - {} - is already mounted ( {} ).  Mount failed".format(pkgpath, prev) )
        #        else:
        #            utils.print_warning( "The previous file mapping ( {} ) is being overridden.".format(prev) )
        #            umount._umount_file( path, linksrc )

        #    # Create symoblic for the header file
        #    try:
        #        if ( not skip ):
        #            symlinks.make_link( path, h )
        #            utils.print_verbose( "MOUNTED    " + pkgpath )
                
        #    except OSError as ex:
        #        exit( ex )

