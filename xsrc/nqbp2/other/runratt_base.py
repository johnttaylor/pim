#!/usr/bin/python3
""" Utilities and helper functions used when running RATT test scripts for
    automated/CI testing
"""

import subprocess
import time
import os
import sys


_dirstack = []
testsdir3 = ""
testsdir2 = ""
testsdir1 = ""
rattexe   = ""
pathopts  = ""

start_time = int(time.time())

#-------------------------------------------------
def set_paths( testcases_dir,
               testsuite_dir = os.getcwd(),
               library_scripts ="scripts/colony.core/ratt" ):
    """ Sets the various paths.  All paths are specified relative
        to the package's root directory.
        
        Note: the directory separator will be normalized for the Host platform.

        testsuite_dir    is passed to RATT using --path3
        testcases_dir    is passed to RATT using --path2
        library1_scripts is passed to RATT using --path1
    """
    global testsdir1, testsdir2, testsdir3, rattexe
    
    pkg_root   = os.environ.get('NQBP_PKG_ROOT')
    xpkgs_root = os.environ.get('NQBP_XPKGS_ROOT')
    testsdir3  = os.path.join( pkg_root,  standardize_dir_sep(testsuite_dir) )
    testsdir2  = os.path.join( pkg_root,  standardize_dir_sep(testcases_dir) )
    testsdir1  = os.path.join( pkg_root,  standardize_dir_sep(library_scripts) )
    rattexe    = os.path.join( xpkgs_root, "ratt", "bin",  "ratt.py" )

#
def run_ratt( tsuite_name, uut_dir, uut_exe, exehost, exeargs="", rattxargs="" ):
    push_dir( uut_dir )
    
    cmd = f'{rattexe} --log --input {standardize_dir_sep(tsuite_name)} --path1 {testsdir1} --path2 {testsdir2} --path3 {testsdir3} {rattxargs} {exehost} {uut_exe} {exeargs}' 
    print( cmd )
    result, testoutput = run_shell( cmd  )
    pop_dir()
    return result, testoutput


def start_marker():
    print("START Time:", time.strftime("%a, %d %b %Y %H:%M:%S", time.localtime(start_time)))

def end_marker():
    end_time = time.time() 
    elasped  = int(end_time - start_time) 
    mm       = (elasped // 60) % 60
    hhh      = elasped // (60*60)
    ss       = elasped % 60

    print("END Time:", time.strftime("%a, %d %b %Y %H:%M:%S", time.localtime(end_time)))
    print('Elapsed Time: {:02d} {:02d}:{:02d}'.format(hhh, mm, ss) )

#----------------------------------------------------------------------
def standardize_dir_sep( pathinfo, os_sep=os.sep  ):
    return pathinfo.replace( '/', os_sep).replace( '\\', os_sep )

def strip_drive_letter( path ):
    r = path.split( ":", 1 )
    if ( len(r) == 1 ):
        return r
    return r[1]

def push_dir( newDir ):
    global _dirstack
    _dirstack.append( os.getcwd() )
    os.chdir( newDir )
    
    
def pop_dir():
    global _dirstack
    os.chdir( _dirstack.pop() )

def run_shell( cmd ):
    p = subprocess.Popen( cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE )
    r = p.communicate()
    r0 = '' if r[0] == None else r[0].decode()
    r1 = '' if r[1] == None else r[1].decode()
    return (p.returncode, f"{r0} {r1}")
