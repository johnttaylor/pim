#!/usr/bin/env python3
"""
This script runs 'doxygen' (to generate HTML documentation of the package's
header files) on the package and checks for warnings.  Colony's coding 
standards require that doxygen execute without warning.

The script assumes that doxygen is in the command path.


Usage: doxygen [<buildtype> <buildNumber>] [debug]

"""

import subprocess
import re
import shutil
import os
import sys

#------------------------------------------------------------------------------
def filter_warnings( output ):
    at_least_one = False
    lines = output.splitlines()
    for line in lines:
        # Filter
        if ( re.search( r"^.*src/Cpl/Type/enum.h:.*warning:.*", line ) ):
            continue

        # Filter
        if ( re.search( r"^.*src/.*warning: argument .* of command @param is not found in the argument list of .*BETTER_ENUM.*", line ) ):
            continue

        # Filter
        #if ( re.search( r"src/Cpl/Text/Frame/LineDecoder.h:.*warning: Found unknown command.*\\r", line ) ):
        #    continue
 
        # Filter
        #if ( re.search( 'src/Cpl/TShell/Dac/Cmd/Command.h:.*warning: Unsupported xml/html tag <esc> found', line ) ):
        #    continue
            
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

# File name MUST match the base file name that is set for CHM_FILE in the 'Doxyfile' file
filename  = "colony.core-library"

# set the build info/number
buildtype = 'private'
buildnum  = 0
debug     = False
if ( len(sys.argv) > 3 and sys.argv[3] == 'debug' ):
    debug = True
if ( len(sys.argv) > 2 ):
    buildtype = sys.argv[1]
    buildnum  = sys.argv[2]

buildInfo = f"Build: {buildnum}-{buildtype}"
if ( debug ):
    print( "= buildInfo: ", buildInfo )

# Create a temporary config file that includes the build info
cfgfile = "Doxyfile.tmp"
indata  = ""
with open ("Doxyfile", "r") as inf:
    indata=inf.read()
indata = indata.replace('$$$PROJECT_NUMBER$$$',buildInfo)
if ( debug ):
    print("= Config File...")
with open (cfgfile, "w") as outf:
    outf.writelines(indata)
    if ( debug ):
        print(indata)
        
# run doxygen
cmd = "doxygen " + cfgfile
if ( debug ):
    print("= Command: ", cmd )
p   = subprocess.Popen( cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE )
r   = p.communicate()
if ( debug ):
    print( "= Return code: ", p.returncode)
    print( "= Output: " )
    print( r[0].decode() )
    print( r[1].decode() )
if ( p.returncode != 0 ):
    exit( "ERROR: Doxygen failed to run.  Check if doxygen.exe is in your command path" )

# Remove temporary config file
try:
    os.remove(cfgfile)
except:
    pass

# delete the HTML files - only keep the Windows Help (.chm) file
path = os.path.join( '..', 'docs', 'html' )
shutil.rmtree( path, ignore_errors=True  )


# check for errors (note: filter_warnings exits on an error)
if ( "warning: " in r[1].decode() or "error: " in r[1].decode() ):
    print()
    print( "*** Doxygen had one or more warnings! ***" )
    filter_warnings( r[1].decode() )
    
else:
    print( "Completed without warnings or errors." )

# Rename help file output to include the build info
if ( len(sys.argv) > 2 ):
    outfile = os.path.join( '..', 'docs', f'{filename}.chm' )
    newfile = os.path.join( '..', 'docs', f'{filename}_{buildnum}-{buildtype}.chm' )
    shutil.move( outfile, newfile )