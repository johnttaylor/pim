#!/usr/bin/python3
"""Invokes NQBP's avr_dude_base.py script.  Place 'avr-dude.py' in your current directory or in your command path
"""

import os
import sys

# Make sure the environment is properly set
NQBP_BIN = os.environ.get('NQBP_BIN')
if ( NQBP_BIN == None ):
    sys.exit( "ERROR: The environment variable NQBP_BIN is not set!" )
sys.path.append( NQBP_BIN )


# Run command
from other import avr_dude_base
avr_dude_base.run( sys.argv )
