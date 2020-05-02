#!/usr/bin/python3
"""Invokes NQBP's xcode_base.py script.  Place 'xcode.py' in your current directory or in your command path
"""

from __future__ import absolute_import
import os
import sys

# Make sure the environment is properly set
NQBP_BIN = os.environ.get('NQBP_BIN')
if ( NQBP_BIN == None ):
    sys.exit( "ERROR: The environment variable NQBP_BIN is not set!" )
sys.path.append( NQBP_BIN )


# Run command
from other import xcode_base
xcode_base.run( sys.argv )
