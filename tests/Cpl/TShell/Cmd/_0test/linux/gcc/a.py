#!/usr/bin/python3
"""
Script to run unittests (leverages NQBP python code base)

"""

import os
import sys
import shutil
import subprocess
import filecmp

#------------------------------------------------------------------------------
def run_test( root, subdir ):
    utils.push_dir( subdir )
    basesrc = os.path.join( root, my_globals.NQBP_PKG_ROOT(), 'src', 'Cpl', 'TShell', 'Cmd', '_0test' )
    src     = os.path.join( basesrc, 'testinput.txt' )
    golden  = os.path.join( basesrc, 'expected-unix.txt' )
    shutil.copy( src, '.' ); 
    print( "Running unit test dir: {} ...".format( subdir ) )
    p = subprocess.Popen( f'cat {src}| b.out > out.txt', shell=True )
    p.communicate()
    if ( p.returncode != 0 ):
        utils.pop_dir()
        exit( "FAILED test" )
    print( "ALL Tests passed." )
    post_process_output( golden )
    utils.pop_dir()
        

def post_process_output( golden ):
    # Filter out line with runtime specific values
    with open( 'out.txt' ) as inf:
        with open( 'temp.out.txt', "w") as outf:  
            for line in inf:
                if ( 'LAST CHECKED LINE' in line ):
                    break
                outf.write( line )

    if ( not filecmp.cmp(golden,'temp.out.txt') ):
        utils.pop_dir()
        exit( "FAILED test - output does NOT match the expected values" )


#------------------------------------------------------------------------------
# Make sure the environment is properly set
NQBP_BIN = os.environ.get('NQBP_BIN')
if ( NQBP_BIN == None ):
    sys.exit( "ERROR: The environment variable NQBP_BIN is not set!" )
sys.path.append( NQBP_BIN )

# Find the Package & Workspace root
from nqbplib import utils
from nqbplib import my_globals
utils.set_pkg_and_wrkspace_roots(__file__)


#------------------------------------------------------------------------------
# MAIN ENTRY POINT....
#
if ( os.path.isdir( '_posix64' ) ):
    run_test( my_globals.NQBP_WORK_ROOT(), '_posix64' )
    print

if ( os.path.isdir( '_cpp11' ) ):
    run_test( my_globals.NQBP_WORK_ROOT(), '_cpp11' )
    print
