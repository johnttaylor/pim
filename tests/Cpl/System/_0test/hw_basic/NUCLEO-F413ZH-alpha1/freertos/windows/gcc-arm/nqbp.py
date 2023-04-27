#!/usr/bin/python3
"""Invokes NQBP's mk.py script"""

import os
import sys

# MAIN
if __name__ == '__main__':
	# Make sure the environment is properly set
	NQBP_BIN = os.environ.get('NQBP_BIN')
	if ( NQBP_BIN == None ):
	    sys.exit( "ERROR: The environment variable NQBP_BIN is not set!" )
	sys.path.append( NQBP_BIN )

	# Find the Package & Workspace root
	from nqbplib import utils
	utils.set_pkg_and_wrkspace_roots(__file__)

	# Call into core/common scripts
	import mytoolchain
	from nqbplib import mk
	mk.build( sys.argv, mytoolchain.create() )
