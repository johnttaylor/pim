#! /usr/bin/env python3
"""
 
replace is used to modify a set of files in place
================================================================================
usage: replace [options] <rootdir> <file-pattern> <original> <new>...
       replace [--help]

Arguments:
    <rootdir>      Specifies the parent directory to recursively search for
                   'file-pattern'
    <filepattern>  Filename or pattern (using Python wildcards) of file(s) to
                   modify
    <original>     Original string to be modified
    <dstdir>       New string value to replace 'original' with
                         
Options:
    -c MAX         Number of times - on a single line - the original string
                   is replaced [Default: 1].
    --sol          Requires that the first 'original' string only matches lines
                   where 'original' is the start of the lines.
    --ex SRCDIR    Excludes the specified source directory from the replace
                   operation.
    --ex2 SRCDIR   Sames as --ex (i.e. adds additional exclude criteria)
    --ex3 SRCDIR   Sames as --ex (i.e. adds additional exclude criteria)
    -v             Be verbose 
    -w             Disable warnings
    -h, --help     Displays this information

Examples:
    ; Replaces all occurrences of 'arduino/hardware/' in libdirs.b files
    ; with 'xsrc/colony.arduino/hardware under the tests/ directory
    replace tests libdirs.b arduino/hardware/ src/colony.arduino/hardware/

      
"""

import sys
import os
import shutil

from docopt.docopt import docopt
import utils



#------------------------------------------------------------------------------
# Excludes certain files
def replace( file, pairs, max=1, sol=True ):
    tmpfile       = file + ".tmp"
    replaced      = False
    with open( file ) as inf:
        with open( tmpfile, "w") as outf:  
            for line in inf:
                stripped = line.strip()

                # Replace pairs
                first = True
                for p in pairs:
                    if ( first ):
                        if ( stripped.startswith( p[0] ) ):
                            line = line.replace( p[0], p[1], 1 )
                            replaced = True

                        first = False
                    else:
                        if ( p[0] in line ):
                            line = line.replace( p[0], p[1], max )
                            replaced = True
                
                outf.write( line )
    
    if ( replaced ):
        os.remove( file )
        os.rename( tmpfile, file )
    else:
        os.remove( tmpfile )

    return replaced

# Convert the command line args to pairs of original/new strings
def convert_to_replacement_pairs( args ):
    # Get the first/required pair
    pairs = []
    pair  = (args['<original>'], args['<new>'][0] )
    pairs.append( pair )

    # Get remaining pairs
    remaining = len(args['<new>']) - 1
    index     = 1
    while( remaining > 1 ):
        pair = (args['<new>'][index], args['<new>'][index+1])
        pairs.append( pair )
        remaining = remaining - 2
        index     = index + 2

    if ( remaining != 0 ):
        utils.print_warning( "Invalid number of original/new strings: " + args['<original>'] + " " +  " ".join( args['<new>']) )

    return pairs

#------------------------------------------------------------------------------
# BEGIN
if __name__ == '__main__':

    # Parse command line
    args = docopt(__doc__, version='0.0.1')

    # Set quite & verbose modes
    utils.set_quite_mode( args['-w'] )
    utils.set_verbose_mode( args['-v'] )
    
    # get the list of source files
    srcpath  = os.path.abspath(args['<rootdir>'])
    utils.print_verbose( "srcpath = " + srcpath )
    utils.push_dir( srcpath )
    srcfiles = utils.walk_file_list( args['<file-pattern>'], '.' );

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

    
    # housekeeping
    pairs = convert_to_replacement_pairs( args )
    
    # Replace file contents
    num_warnings = 0
    num_files    = 0
    num_replaced = 0
    for h in srcfiles:

        # housekeeping
        num_files = num_files + 1
        src       = os.path.join( srcpath, h )
        utils.print_verbose( "replace: " + src )
        if ( not os.path.exists( h ) ):
            utils.print_warning( "file - {} - does not exist - the file will be skipped".format( h ) )
            num_warnings = num_warnings + 1
        
        # Replace content
        if ( replace( h, pairs, args['-c'], args['--sol'] ) ):
            num_replaced = num_replaced + 1

    # Restore original CWD
    utils.pop_dir()
    print( "Replaced {} files (of total files {}) with {} warnings".format( num_replaced, num_files, num_warnings ) )
        