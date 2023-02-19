#!/usr/bin/python3
""" Runs the PIO Assembler for .pio files in the directory"""

import os, sys


# MAIN
if __name__ == '__main__':
    NQBP_PKG_ROOT = os.environ.get('NQBP_PKG_ROOT')
    if ( NQBP_PKG_ROOT == None ):
        sys.exit( "ERROR: The environment variable NQBP_PKG_ROOT is not set!" )


    script_dir = os.path.join( NQBP_PKG_ROOT, "src", "Cpl", "System", "RP2040" );
    print(script_dir)
    sys.path.append( script_dir )
    from _pyscripts import nqbp_preprocess_base_piosam
    nqbp_preprocess_base_piosam.run( sys.argv )
