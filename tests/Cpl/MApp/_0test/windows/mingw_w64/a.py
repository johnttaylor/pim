#!/usr/bin/python3
"""Launch's the RATT test script(s) for the unittest project
   
   usage: a.py [uut_exe_args]
"""

import os
import sys


# MAIN
if __name__ == '__main__':
    # Make sure the environment is properly set
    NQBP_BIN = os.environ.get('NQBP_BIN')
    if ( NQBP_BIN == None ):
        sys.exit( "ERROR: The environment variable NQBP_BIN is not set!" )
    sys.path.append( NQBP_BIN )
    from other import runratt_base

    # Housekeeping
    exeargs = ' '.join(sys.argv[1:])
    tsuite  = '../../my-tsuite.py'
    exe     = 'b.exe'
    host    = '--win'
    runratt_base.set_paths( "src/Cpl/MApp/_0test" )
    runratt_base.start_marker()

    # Run the test(s)
    result = 0
    if ( os.path.isdir( '_win32' ) ):
        result, testoutput = runratt_base.run_ratt( tsuite, '_win32', exe, host, exeargs )
        print( testoutput )
        print

    if ( os.path.isdir( '_win64' ) ):
        result, testoutput = runratt_base.run_ratt( tsuite, '_win64', exe, host, exeargs )
        print( testoutput )
        print

    if ( os.path.isdir( '_cpp11' ) and result == 0 ):
        result, testoutput = runratt_base.run_ratt( tsuite, '_cpp11', exe, host, exeargs )
        print( testoutput )
        print

    sys.exit( result )