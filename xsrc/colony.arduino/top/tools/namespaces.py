#!/usr/bin/env python
"""
Package specific script that updates the 'pkg.namespaces' file with all of the
namespaces 'owned' by the package.  This script is called as part of the
publish package process.

Note: Package owner can opt for updating the 'pkg.namespaces' file as a
      manual process by having this script do nothing and simply 
      return '0'

Usage: namespaces <pkgroot>

Returns: If the update was successful then the script returns '0'; else
         a non-zero value is returned.
"""

import sys
import os
import subprocess

#------------------------------------------------------------------------------
#
# This section can be deleted if NOT using Outcast's default namespace algorithm
#
def _use_outcast():
    # Begin 
    if ( len(sys.argv) < 2 ):
        exit( "ERROR: Missing <pkgroot> command argument" )

    
    # Build arguments for the bob-the-builder
    pkgname = os.path.basename( sys.argv[1] )
    cmd       = "orc.py -v namespaces --update {}".format( pkgname )
    
    # Invoke the build
    print "EXECUTING: " + cmd
    p = subprocess.Popen( cmd, shell=True )
    r = p.communicate()
    if ( p.returncode != 0 ):
        exit( "ERROR: Update of pkg.namespaces file failed" )
        

#------------------------------------------------------------------------------
# 

# Uncomment out the following line if using Outcast's default namespace
# algorihtm 
_use_outcast()



