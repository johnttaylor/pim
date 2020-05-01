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
    --filter WCARD Optional wildcard to use when copying directory content
                   [Default: *].
    --ex SRCDIR    Excludes the specified source directory from the copy
                   operation.
    --ex2 SRCDIR   Sames as --ex (i.e. adds additional exclude criteria)
    --ex3 SRCDIR   Sames as --ex (i.e. adds additional exclude criteria)
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
    srcfiles = utils.walk_file_list( args['--filter'], '.' );
    utils.pop_dir()

    # exclude directories
    if ( args['--ex'] ):
        newlist = []
        for s in srcfiles:
            exclude = False
            if ( os.sep + args['--ex'] + os.sep in s  ):
                exclude = True
            if ( args['--ex2'] != None and os.sep + args['--ex2'] + os.sep in s  ):
                exclude = True
            if ( args['--ex3'] != None and os.sep + args['--ex3'] + os.sep in s  ):
                exclude = True
                
            if ( not exclude ):
                newlist.append( s )

        srcfiles = newlist

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
        