#!/usr/bin/env python
"""
Test script when publishing the package
===============================================================================
usage: test <pkgroot>
    
Notes:
    o Returns zero when all of the tests are succesful; else
      a non-zero value is returned.

"""

import sys
import os
import subprocess


#------------------------------------------------------------------------------
# Begin 
if ( len(sys.argv) < 2 ):
    exit( "ERROR: Missing <pkgroot> command argument" )


# Build arguments for the bob-the-builder
cleanlist = os.path.join( sys.argv[1], "top", "publish_test.lst" )
path      = os.path.join( sys.argv[1], "tests", "workspace1")
cmd       = "bob.py build -v --path {} --file {}".format(path, cleanlist)

# Invoke the build
print "EXECUTING: " + cmd
p = subprocess.Popen( cmd, shell=True )
r = p.communicate()
if ( p.returncode != 0 ):
    exit( "ERROR: Test(s) failed" )
