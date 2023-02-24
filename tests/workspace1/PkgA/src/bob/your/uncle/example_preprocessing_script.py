#!/usr/bin/python3
#---------------------------------------------------------------------------
# This is example python script that NQBP can/will call BEFORE invoking the
# compiler toolchain.  The script should be placed in every source directory 
# that NQBP builds that REQUIRES the pre-processing step.  
#
# In order to enable the pre-processing actions, the project's mytoolchain.py
# need to be updated with the following statements:
#
#   from nqbplib.my_globals import NQBP_PRE_PROCESS_SCRIPT
#   from nqbplib.my_globals import NQBP_PRE_PROCESS_SCRIPT_ARGS
#   NQBP_PRE_PROCESS_SCRIPT( "<preprocess-script.py>" )  # i.e the name of this script
#   NQBP_PRE_PROCESS_SCRIPT_ARGS("<arg7> <arg8> ...")    # project specific arguments pass to the script
#
# NOTE: The script is ALWAYS passed <arg1> ... <arg5> where:
#             <arg1>: build|clean         
#             <arg2>: verbose|terse
#             <arg3>: <workspace-dir>
#             <arg4>: <package-dir> 
#             <arg5>: <project-dir> 
#             <arg6>: <current-dir> 
#
# Since the <preprocess-script.py> script MUST be in every (desired) source 
# directory, users are encouraged that the <preprocess-script.py> ONLY be a 
# simple shell/wrapper that calls a singe instance of the 'actual script'
#---------------------------------------------------------------------------

# get definition of the Options structure
import os
import sys

# MAIN
if __name__ == '__main__':
    # Create path to the 'real' script
    script = os.path.join(sys.argv[4], "scripts", "example_preprocessing_base.py" )

    # Run the 'actual' pre-processing script
    sys.exit(os.system( script + " " + " ".join(sys.argv[1:])))
