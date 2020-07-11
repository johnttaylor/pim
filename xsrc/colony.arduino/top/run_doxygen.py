#!/usr/bin/env python
"""
This script runs 'doxygen' (to generate HTML documention of the package's
header files) on the package and checks for warnings.  Colony's coding 
standards require that doxygen execute without warning.

The script assumes that doxygen is in the command path.


Usage: doxygen

"""

import subprocess
import re

#------------------------------------------------------------------------------
def filter_warnings( output ):
    at_least_one = False
    lines = output.splitlines()
    for line in lines:
        # Filter auto generated FSM code
        if ( re.search( "^.*Fsm_.h", line ) or re.search( "^.*Fsm_ext_.h", line ) or re.search( "^.*Fsm_trace_.h", line )):
            continue
            
        # Filter
        if ( re.search( r"src/Cpl/Text/Frame/LineDecoder.h:.*warning: Found unknown command.*\\r", line ) ):
            continue
 
        # Filter
        if ( re.search( 'src/Cpl/TShell/Dac/Cmd/Command.h:.*warning: Unsupported xml/html tag <esc> found', line ) ):
            continue
            
        # Passed ALL filters
        print line
        at_least_one = True

    # Display the results of the filtering
    if ( at_least_one == False ):
        print "    All warnings are known warnings -->so you are good!"
        print
        exit(0)
    else:
        print
        exit(1)
        
#------------------------------------------------------------------------------
print "Running doxygen..."     

# run doxygen
cmd = "doxygen"
p   = subprocess.Popen( cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE )
r   = p.communicate()
if ( p.returncode != 0 ):
    exit( "ERROR: Doxygen failed to run.  Check if doxygen.exe is in your command path" )


# check for errors
if ( " warning: " in r[1] ):
    print
    print "*** Doxygen had one or more warnings! ***"
    filter_warnings( r[1] )
    
print "Completed without warnings or errors."
