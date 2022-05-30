#---------------------------------------------------------------------------
# This python module is used to customize a supported toolchain for your 
# project specific settings.
#
# Notes:
#    - ONLY edit/add statements in the sections marked by BEGIN/END EDITS
#      markers.
#    - Maintain indentation level and use spaces (it's a python thing) 
#    - rvalues must be enclosed in quotes (single ' ' or double " ")
#    - The structure/class 'BuildValues' contains (at a minimum the
#      following data members.  Any member not specifically set defaults
#      to null/empty string
#            .inc 
#            .asminc
#            .cflags
#            .cppflags
#            .asmflags
#            .linkflags
#            .linklibs
#           
#---------------------------------------------------------------------------

# 
import os

# get definition of the Options strcuture
from nqbplib.base import BuildValues
from nqbplib.my_globals import NQBP_WORK_ROOT

# Get the location of the compiler toolchain
env_error = None
ARDUINO_TOOLS = os.environ.get( 'ARDUINO_TOOLS' )
if ( ARDUINO_TOOLS == None ):
    ARDUINO_TOOLS = env_error = "ARDUINO_TOOLS"
ARDUINO_COMPILER_VER = os.environ.get( 'ARDUINO_COMPILER_VER' )
if ( ARDUINO_COMPILER_VER == None ):
    ARDUINO_COMPILER_VER = env_error = "ARDUINO_COMPILER_VER"
ARDUINO_BSP_VER = os.environ.get( 'ARDUINO_BSP_VER' )
if ( ARDUINO_BSP_VER == None ):
    ARDUINO_BSP_VER = env_error = "ARDUINO_BSP_VER"
ARDUINO_NRF_UTIL_VER = os.environ.get( 'ARDUINO_NRF_UTIL_VER' )
if ( ARDUINO_NRF_UTIL_VER == None ):
    ARDUINO_NRF_UTIL_VER = env_error = "ARDUINO_NRF_UTIL_VER"

ARDUINO_SUPPORT = "colony.arduino"


#===================================================
# BEGIN EDITS/CUSTOMIZATIONS
#---------------------------------------------------

# Set the name for the final output item (with NO file extension)
FINAL_OUTPUT_NAME = 'blink'



#
# For build config/variant: "Release"
#

# Set project specific 'base' (i.e always used) options
base_release = BuildValues()        # Do NOT comment out this line
base_release.cflags = ' -Wall -DF_CPU=64000000 -DARDUINO=10802'


# Set project specific 'optimized' options
optimzed_release = BuildValues()    # Do NOT comment out this line

# Set project specific 'debug' options
debug_release = BuildValues()       # Do NOT comment out this line
#debug_release.cflags = '-D_MY_APP_DEBUG_SWITCH_'




#-------------------------------------------------
# ONLY edit this section if you are ADDING options
# for build configurations/variants OTHER than the
# 'release' build
#-------------------------------------------------

release_opts = { 'user_base':base_release, 
                 'user_optimized':optimzed_release, 
                 'user_debug':debug_release
               }
               
               
# Add new dictionary of for new build configuration options
#xyz_opts = { 'user_base':base_xyz, 
#             'user_optimized':optimzed_xyz, 
#             'user_debug':debug_xyz
#           }
  
        
# Add new variant option dictionary to # dictionary of 
# build variants
build_variants = { 'arduino':release_opts,
#                  'xyz':xyz_opts,
                 }    

#---------------------------------------------------
# END EDITS/CUSTOMIZATIONS
#===================================================



# Capture project/build directory
import os
prjdir = os.path.dirname(os.path.abspath(__file__))


# Select Module that contains the desired toolcahin
from nqbplib.toolchains.windows.arm_m4_arduino.nrf52_feather52 import ToolChain


# Function that instantiates an instance of the toolchain
def create():
    tc = ToolChain( FINAL_OUTPUT_NAME, prjdir, build_variants, ARDUINO_TOOLS, ARDUINO_SUPPORT, ARDUINO_COMPILER_VER, ARDUINO_BSP_VER, ARDUINO_NRF_UTIL_VER, "arduino", env_error )
    return tc 
