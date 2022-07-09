#!/usr/bin/env python3
"""Invokes NQBP's genfsm_base.py script.  To run 'GENFSM' copy this file to 
   your source directory.  Then edit the local script to generate one or more
   Finite State Machines (FSMs)
"""

import os
import sys

# Make sure the environment is properly set
#NQBP_BIN = os.environ.get('NQBP_BIN')
#if ( NQBP_BIN == None ):
#    sys.exit( "ERROR: The environment variable NQBP_BIN is not set!" )
#sys.path.append( NQBP_BIN )
sys.path.append( "C:\bin\testfsm" )

# Find the Package & Workspace root
import genfsm_base
sys.argv.append('')


###############################################################
# BEGIN EDITS HERE
###############################################################

# Generate FSM#1, where [1]:= Diagram name, [2]:= Namespace(s).
sys.argv[1] = 'bar'
genfsm_base.run( sys.argv )

# Generate FSM#2, where [1]:= Diagram name, [2]:= Namespace(s).
#sys.argv[1] = 'Example2'
#sys.argv[2] = 'Rte::Db::Record'
#genfsm_base.run( sys.argv )



