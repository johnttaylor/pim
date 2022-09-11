#!/usr/bin/python3
"""Invokes NQBP's f4_base.py script.  Place 'f4.py' in your current directory or in your command path
"""

import os
import sys

# Make sure the environment is properly set
NQBP_BIN = os.environ.get('NQBP_BIN')
if ( NQBP_BIN == None ):
    sys.exit( "ERROR: The environment variable NQBP_BIN is not set!" )
sys.path.append( NQBP_BIN )

# Modify the following lines to override the default --local/--global values!
#============================================================================
### BEGIN EDIT

default_local_path  = '.'
default_global_path = '.'
default_root_path   = '.'
#default_local_path  = '/resources/f4'
#default_global_path = '//colon.core/resources/f4'
#default_root_path   = '/src'

### END EDIT
#============================================================================

# Find the Package & Workspace root
from other import f4_base
f4_base.run( sys.argv, default_local_path, default_global_path, default_root_path )
