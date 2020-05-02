#!/usr/bin/python3
"""Launch's the RATT test script(s) for the unittest project"""

import os
import sys
import subprocess

###
def run_shell( cmd ):
    p = subprocess.Popen( cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE )
    r = p.communicate()
    r0 = '' if r[0] == None else r[0].decode()
    r1 = '' if r[1] == None else r[1].decode()
    return (p.returncode, "{} {}".format(r0,r1))

# MAIN
if __name__ == '__main__':
    
    # Additional ratt options
    options = ' '.join(sys.argv[1:])

    # Get paths to important stuff...
    result, wrk_root = run_shell( 'orc --qry-w' )
    if ( result != 0 ):
        sys.exit( "Outcast Environment not configured (cannot execute 'orc.py')" )
    result, pkg_root = run_shell( 'orc --qry-p' )
    if ( result != 0 ):
        sys.exit( "Outcast Environment not configured (cannot execute 'orc.py')" )
    wrk_root = wrk_root.strip()
    pkg_root = pkg_root.strip()

    testdir = os.path.join( pkg_root,  "src", "Storm", "Thermostat", "_0test" )
    ratt    = os.path.join( wrk_root, "ratt", "bin",  "ratt.py" )

    # Run the test suite(s)
    cmd = "{} {} --input test_suite.ratt --path1 {} --win _win32\\b.exe".format( ratt, options, testdir )
    result, testoutput = run_shell( cmd  )
    print( testoutput )
    sys.exit( result )
