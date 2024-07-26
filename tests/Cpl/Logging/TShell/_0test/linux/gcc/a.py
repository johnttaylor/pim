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
    basesrc = os.path.join( root, my_globals.NQBP_PKG_ROOT(), 'src', 'Cpl', 'Logging', 'TShell', '_0test' )
    src     = os.path.join( basesrc, 'testinput.txt' )
    golden  = os.path.join( basesrc, 'expected.txt' )
    shutil.copy( src, '.' ); 
    print( "Running unit test dir: {} ...".format( subdir ) )
    p = subprocess.Popen( f'cat {src}| b.out > out.txt', shell=True )
    p.communicate()
    if ( p.returncode != 0 ):
        utils.pop_dir()
        exit( "FAILED test" )
    post_process_output( golden )
    print( "ALL Tests passed." )
    utils.pop_dir()
        

def post_process_output( golden ):
    if ( not compare_files(golden,'out.txt') ):
        utils.pop_dir()
        exit( "FAILED test - output does NOT match the expected values" )

def compare_files( file1, file2 ):
    line1 = line2 = True
    with open(file1, 'r') as f1, open(file2, 'r') as f2:
        while line1 and line2:
            line1 = f1.readline().replace(' ','') # remove whitespace since it varies based 32 vs 64 bit builds
            line2 = f2.readline().replace(' ','')
            if line1 != line2:
                return False
    return True


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
    run_test( my_globals.NQBP_WORK_ROOT(), '_posix' )
    print

if ( os.path.isdir( '_posix64' ) ):
    run_test( my_globals.NQBP_WORK_ROOT(), '_posix64' )
    print
