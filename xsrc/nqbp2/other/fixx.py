#!/usr/bin/python3

"""
 
Recursively finds files and updates GIT's index with the execute permissions.
===============================================================================
usage: fixx [options] <dir>

Arguments:
    <dir>      Root directory to recursively search
                         
Options:
    -e EXT          File extension of the files to search-for/update. 
                    [Default: py]
    -v              Be verbose 
    -h, --help      Displays this information

Examples:
    ; Make sure all nqbp.py scripts are executable on a Linux host
    fixx projects
      
"""

import sys
import os
import fnmatch
import time

sys.path.append( os.path.dirname(__file__) + os.sep + ".." )

from nqbplib import utils
from nqbplib.docopt.docopt import docopt


FIXX_VERSION = "1.0"

#------------------------------------------------------------------------------
        
        
#------------------------------------------------------------------------------
#
def execute_test( target, ppath, args, arg_string ):
    utils.push_dir( os.path.split(target)[0] )
    reldir = target.replace(ppath,'')[1:]
    exe    = os.path.basename(target)
    cmd    = ".{}{} {}".format( os.path.sep, exe, arg_string )
    for n in range( int(args['--loop']) ):
        print( "= EXECUTING (#{}): {} {}".format( n+1, reldir, arg_string ) )
        result = utils.run_shell2( cmd, args['-v'], "** ERROR: Test failed (#{} {} {} )**".format(n+1, reldir, arg_string) )
    utils.pop_dir

#------------------------------------------------------------------------------
# BEGIN
if __name__ == '__main__':

    # Parse command line
    args = docopt(__doc__, version=FIXX_VERSION, options_first=True )
    utils.set_verbose_mode( args['-v'] )
    ppath   = args['<dir>']
    match   = f"*.{args['-e']}"
    verbose = args['-v']

    # Get superset of projects to build
    all = utils.walk_file_list( match, ppath )
    for f in all:
        cmd = f'git update-index --chmod=+x {f}'
        result = utils.run_shell2( cmd, False )
