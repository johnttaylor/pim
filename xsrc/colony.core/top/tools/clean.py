#!/usr/bin/env python3
"""
Clean script when publishing the package
===============================================================================
usage: clean <pkgroot> [<host>]

Notes:
    o Default host is "windows" 
    o Returns zero when all of the tests are succesful; else
      a non-zero value is returned.

"""

import sys
import os
import subprocess


#------------------------------------------------------------------------------
#
# This section can be deleted if NOT using the Outcast Bob tool.
# This method assumes that the file top/publish_clean_xxx.lst exists and it
# contains the list of test executable to run.
#
def _use_outcast():
    if ( len(sys.argv) < 2 ):
        exit( "ERROR: Missing <pkgroot> command argument" )

    # Trap host option
    publish = "publish_clean.lst"
    if ( len(sys.argv) > 2 and sys.argv[2] == 'linux' ):
        publish = "publish_clean_linux.lst"
    

    # Build arguments for the bob-the-builder
    cleanlist = os.path.join( sys.argv[1], "top", publish )
    path      = os.path.join( sys.argv[1], "tests" )
    cmd       = "bob.py build -v --path {} --file {}".format(path, cleanlist)

    # Invoke the build
    print("EXECUTING: " + cmd)
    p = subprocess.Popen( cmd, shell=True )
    r = p.communicate()
    if ( p.returncode != 0 ):
        exit( "ERROR: Clean failed" )


#------------------------------------------------------------------------------
# Uncomment out the following line if using the Outcast Bob build tool
_use_outcast()

# Delete this line if using the Outcast default namespace algorihtm
#print "Cleaning...."
