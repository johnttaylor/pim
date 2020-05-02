#!/usr/bin/env python3
"""
Package specific script that 'tests' the package content as part of the 
publish process.  

Usage: test <pkgroot> [<host>]

Notes:
    o Default host is "windows" 

Returns: If all of the test(s) are successful, then the script returns '0'; else
         a non-zero value is returned.
"""


import sys
import os
import subprocess


#------------------------------------------------------------------------------
#
# This section can be deleted if NOT using the Outcast Chuck tool.
# This method assumes that the file top/publish_test.lst exists and it
# contains the list of test(s) to execute.
#
def _use_outcast():
    # Begin 
    if ( len(sys.argv) < 2 ):
        exit( "ERROR: Missing <pkgroot> command argument" )

    # Trap host option
    publish = "publish_test.lst"
    if ( len(sys.argv) > 2 and sys.argv[2] == 'linux' ):
        publish = "publish_test_linux.lst"

    # Build arguments for the bob-the-builder
    testlist  = os.path.join( sys.argv[1], "top", publish )
    path      = os.path.join( sys.argv[1], "tests" )
    cmd       = "chuck.py -v --path {} --file {}".format(path, testlist)

    # Invoke the build
    print("EXECUTING: " + cmd)
    p = subprocess.Popen( cmd, shell=True )
    r = p.communicate()
    if ( p.returncode != 0 ):
        exit( "ERROR: Test(s) failed" )


#------------------------------------------------------------------------------
# Uncomment out the following line if using the Outcast Chuck test tool
_use_outcast()

# Delete this line if using the Outcast default namespace algorithm
#print "Testing...."
