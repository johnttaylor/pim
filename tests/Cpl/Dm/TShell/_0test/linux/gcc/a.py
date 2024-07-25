#!/usr/bin/python3
"""
Script to run unittests (leverages NQBP python code base)

"""

import os
import sys
import shutil
import subprocess

#------------------------------------------------------------------------------
def run_test( subdir ):
    utils.push_dir( subdir )
    src = os.path.join( my_globals.NQBP_WORK_ROOT(), my_globals.NQBP_PKG_ROOT(), 'src', 'Cpl', 'Dm', 'TShell', '_0test', 'testinput.txt' )
    print( "Running unit test dir: {} ...".format( subdir ) )
    p = subprocess.Popen( 'cat {}| b.out'.format(src), shell=True )
    p.communicate()
    if ( p.returncode != 0 ):
        exit( "FAILED test" )
    utils.pop_dir()
        


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

if ( os.path.isdir( '_posix' ) ):
    run_test( '_posix' )
    print

if ( os.path.isdir( '_posix64' ) ):
    run_test( '_posix64' )
    print

if ( os.path.isdir( '_cpp11' ) ):
    run_test( '_cpp11' )
    print
