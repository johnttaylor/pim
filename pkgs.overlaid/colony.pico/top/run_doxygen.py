#!/usr/bin/env python3
"""
This script runs 'doxygen' (to generate HTML documentation of the package's
header files) on the package and checks for warnings.  Colony's coding 
standards require that doxygen execute without warning.

The script assumes that doxygen is in the command path.


Usage: doxygen

"""

import subprocess
import re
import shutil
import os

#------------------------------------------------------------------------------
def filter_warnings( output ):
    at_least_one = False
    lines = output.splitlines()
    for line in lines:
        
        # Skip empty lines
        line.strip()
        if ( line == "" ):
            continue

        # Filter
        if ( re.search( r"^.*error.*inline_dotgraph_.*\.dot", line ) ):
            continue

        # Filter
        if ( re.search( r"^.*src/Cpl/Type/enum.h:.*warning:.*", line ) ):
            continue

        # Filter
        if ( re.search( r"^.*src/.*warning: argument .* of command @param is not found in the argument list of .*BETTER_ENUM.*", line ) ):
            continue

        # Passed ALL filters
        print( line )
        at_least_one = True

    # Display the results of the filtering
    if ( at_least_one == False ):
        print( "    All warnings are known warnings -->so you are good!")
        print()
        exit(0)
    else:
        print()
        exit(1)
        
#------------------------------------------------------------------------------
print( "Running doxygen..." )    

# run doxygen
cmd = "doxygen"
p   = subprocess.Popen( cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE )
r   = p.communicate()
if ( p.returncode != 0 ):
    exit( "ERROR: Doxygen failed to run.  Check if doxygen.exe is in your command path" )

# delete the HTML files - only keep the Windows Help (.chm) file
#path = os.path.join( '..', 'docs', 'html' )
#shutil.rmtree( path, ignore_errors=True  )

# check for errors
if ( " warning: " in r[1].decode() ):
    print()
    print( "*** Doxygen had one or more warnings! ***" )
    filter_warnings( r[1].decode() )
    
print( "Completed without warnings or errors." )
