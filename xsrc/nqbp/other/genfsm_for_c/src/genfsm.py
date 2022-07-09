#!/usr/bin/python
"""Invokes NQBP's genfsm_base.py script.  To run 'GENFSM' copy this file to 
   your source directory.  Then edit the local script to generate one or more
   Finite State Machines (FSMs)
"""

from __future__ import absolute_import
import os
import sys

# Make sure the environment is properly set
#NQBP_BIN = os.environ.get('NQBP_BIN')
#if ( NQBP_BIN == None ):
#    sys.exit( "ERROR: The environment variable NQBP_BIN is not set!" )
#sys.path.append( NQBP_BIN )
sys.path.append( "C:/bin/testfsm" )

# Find the Package & Workspace root
import genfsm_base


###############################################################
# BEGIN EDITS HERE
###############################################################

# Generate FSM#1, where argv:= Diagram name,
sys.argv.append('bar')
genfsm_base.run( sys.argv )

# Generate FSM#1, where argv:= Diagram name,
sys.argv.append('example')
#genfsm_base.run( sys.argv )



