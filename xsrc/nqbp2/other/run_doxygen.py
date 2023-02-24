#!/usr/bin/python3
"""
This script runs 'doxygen' (to generate HTML documention of the package's
header files) on the package and checks for warnings.  Colony's coding 
standards require that doxygen execute without warning.

The script assumes that doxygen is in the command path.


Usage: doxygen

"""

import subprocess


#------------------------------------------------------------------------------
print("Running doxygen...")     

# run doxygen
cmd = "doxygen"
p   = subprocess.Popen( cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE )
r   = p.communicate()
if ( p.returncode != 0 ):
    exit( "ERROR: Doxygen failed to run.  Check if doxygen.exe is in your command path" )


# check for errors
if ( " warning: " in r[1].decode() ):
    print()
    print("*** Doxygen had one or more warnings! ***")
    print()
    print(r[1].decode() )
    exit(1)
    
print("Completed without warnings or errors.")