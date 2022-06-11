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
from nqbplib.my_globals import NQBP_PKG_ROOT

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

ARDUINO_SUPPORT = NQBP_PKG_ROOT()

#===================================================
# BEGIN EDITS/CUSTOMIZATIONS
#---------------------------------------------------

# Set the name for the final output item (with NO file extension)
FINAL_OUTPUT_NAME = 'a'

# BSP directory that contains the vector table 
#bsp_objects = '_BUILT_DIR_.xpkgs/arduino/src/Bsp/Adafruit/grand_central_m4/gcc'

#
# For build config/variant: "Release"
#

# Set project specific 'base' (i.e always used) options
base_release = BuildValues()        # Do NOT comment out this line
base_release.cflags       = ' -DUSING_FREERTOS -DBUILD_OPT_PIN=6 -DBUILD_OPT_NUM_PIXELS=40 -DBUILD_OPT_NEO_TYPE=NEO_GRBW -Wall -DF_CPU=120000000L -DARDUINO=10810 -DVARIANT_QSPI_BAUD_DEFAULT=50000000 -DENABLE_CACHE '
#base_release.cflags      += '-DEXTERNAL_FLASH_USE_QSPI=1'
base_release.inc         += r' -I{}\src\Bsp\Adafruit\grand_central_m4\gcc\FreeRTOS\Source\Include'.format(NQBP_PKG_ROOT())
base_release.inc         += r' -I{}\src\Bsp\Adafruit\grand_central_m4\gcc\FreeRTOS\Source\portable\GCC\ARM_CM4F'.format(NQBP_PKG_ROOT())
base_release.inc         += r' -I{}\arduino\libraries\SdFat\src'.format( ARDUINO_SUPPORT )
base_release.inc         += r' -I{}\arduino\libraries\Adafruit_SPIFlash\src'.format( ARDUINO_SUPPORT )
base_release.inc         += r' -I{}\arduino\libraries\Adafruit_Zero_DMA_Library'.format( ARDUINO_SUPPORT )
base_release.linkflags    = r'-Tflash_without_bootloader.ld'
#base_release.firstobjs    = bsp_objects;

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


# Select Module that contains the desired toolchain
from nqbplib.toolchains.windows.arm_m4_arduino.atsamd51_grandcentral import ToolChain


# Function that instantiates an instance of the toolchain
def create():
    tc = ToolChain( FINAL_OUTPUT_NAME, prjdir, build_variants, ARDUINO_TOOLS, ARDUINO_SUPPORT, ARDUINO_COMPILER_VER, ARDUINO_BSP_VER, "arduino", env_error )
    return tc 
