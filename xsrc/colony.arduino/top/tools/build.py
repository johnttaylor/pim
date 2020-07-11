#!/usr/bin/env python
"""
Package specific script that 'builds' the package content as part of the 
publish process.  

Usage: build <pkgroot> [<host>]

Notes:
    o Default host is "windows" 

Returns: If the build is succesful, then the script returns '0'; else
         a non-zero value is returned.
"""

import sys
import os
import subprocess


#------------------------------------------------------------------------------
#
# This section can be deleted if NOT using the Outcast Bob tool.
# This method assumes that the file top/publish_build_xxx.lst exists and it
# contains the list of projects to build.
#
def _use_outcast():
    if ( len(sys.argv) < 2 ):
        exit( "ERROR: Missing <pkgroot> command argument" )

    # Trap host option
    publish = "publish_build.lst"
    if ( len(sys.argv) > 2 and sys.argv[2] == 'linux' ):
        publish = "publish_build_linux.lst"
    
    # Build arguments for the bob-the-builder
    buildlist = os.path.join( sys.argv[1], "top", publish )
    path      = os.path.join( sys.argv[1], "tests" )
    cmd       = "bob.py build -v --path {} --file {}".format(path, buildlist)

    # Invoke the build
    print "EXECUTING: " + cmd
    p = subprocess.Popen( cmd, shell=True )
    r = p.communicate()
    if ( p.returncode != 0 ):
        exit( "ERROR: Build(s) failed" )


#------------------------------------------------------------------------------
# Uncomment out the following line if using the Outcast Bob build tool
_use_outcast()

# Delete this line if using the Outcast default namespace algorihtm
#print "Building...."
