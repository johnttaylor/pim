#!/usr/bin/python3
#---------------------------------------------------------------------------
# This is example of the 'single instance' NQBP pre-processing script
#       where <arg7> <arg8> ...  are project specific arguments passed to the
#                               <preprocess-script.py> script when it is
#                               executed.
# Usage:
#   example_preprocessing_base <a1> <a2> <a3> <a4> <a5> <a6> <prjarg1>
#
#   where:
#      <a1>: build|clean
#      <a2>: verbose|terse
#      <a3>: <workspace-dir>
#      <a4>: <package-dir>
#      <a5>: <project-dir>
#      <a6>: <current-dir>
#      <prjarg1>: <compiler> // mingw|mingw_64|vc12|etc.
#---------------------------------------------------------------------------

# get definition of the Options structure
import sys

# Do stuff...
print( "--> Example Pre-Processing Script" )
if (sys.argv[2] == 'verbose'):
    print( "=  ECHO: " + '  '.join(sys.argv) )
