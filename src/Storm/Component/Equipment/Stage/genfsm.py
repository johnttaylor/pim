#!/usr/bin/python3
"""Invokes NQBP's genfsm_base.py script.  To run 'GENFSM' copy this file to 
   your source directory.  Then edit the local script to generate one or more
   Finite State Machines (FSMs)
"""

import os
import sys

# Make sure the environment is properly set
NQBP_BIN = os.environ.get('NQBP_BIN')
if ( NQBP_BIN == None ):
    sys.exit( "ERROR: The environment variable NQBP_BIN is not set!" )
sys.path.append( NQBP_BIN )

# Find the Package & Workspace root
from other import genfsm_base
sys.argv.append('')
sys.argv.append('')
sys.argv.append('')


###############################################################
# BEGIN EDITS HERE
###############################################################

# Generate FSM#1
sys.argv[1] = '-d 8'
sys.argv[2] = 'Fsm'
sys.argv[3] = 'Storm::Component::Equipment::Stage'
genfsm_base.run( sys.argv )
