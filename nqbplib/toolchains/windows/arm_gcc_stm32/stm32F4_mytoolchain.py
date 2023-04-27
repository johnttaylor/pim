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
from nqbplib.my_globals import NQBP_PKG_ROOT


#===================================================
# BEGIN EDITS/CUSTOMIZATIONS
#---------------------------------------------------

# Set the name for the final output item (with NO file extension)
FINAL_OUTPUT_NAME = 'blink'


# Path to SDK and the ST CubeMX generated BSP files
bsp_mx        = os.path.join( "src", "Bsp", "ST", "NUCLEO-F413ZH", "alpha1", "MX" )
sdk_root      = os.path.join( NQBP_PKG_ROOT(), "xsrc", "STCubeF4-SDK")
bsp_mx_root   = os.path.join( NQBP_PKG_ROOT(), bsp_mx )
freertos_root = os.path.join( NQBP_PKG_ROOT(), "xsrc", "FreeRTOS")
sysview_root  = os.path.join( NQBP_PKG_ROOT(), "src", "Bsp", "ST", "NUCLEO-F413ZH", "alpha1", "SeggerSysView" )
sysview_root  = sysview_root.replace("\\", "/")

#
# For build config/variant: "Release"
#
 

# Set project specific 'base' (i.e always used) options
base_release = BuildValues()        # Do NOT comment out this line
target_flags             = '-DUSE_STM32F4XX_NUCLEO_144 -DSTM32F413xx'
base_release.cflags      = f' -Wall {target_flags}'
base_release.cppflags    = ' -std=gnu++11'
base_release.asmflags    = f' {target_flags}'
base_release.firstobjs   = f'_BUILT_DIR_.{bsp_mx}/Core/Src'
base_release.firstobjs   = base_release.firstobjs + f' {bsp_mx}/../stdio.o'
base_release.lastobjs    = base_release.lastobjs + f' {bsp_mx}/../syscalls.o' 

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
build_variants = { 'stm32':release_opts,
#                  'xyz':xyz_opts,
                 }    

#---------------------------------------------------
# END EDITS/CUSTOMIZATIONS
#===================================================



# Capture project/build directory
import os
prjdir = os.path.dirname(os.path.abspath(__file__))


# Select Module that contains the desired toolchain
from nqbplib.toolchains.windows.arm_gcc_stm32.stm32F4 import ToolChain


# Function that instantiates an instance of the toolchain
def create():
    lscript  = 'STM32F413ZHTx_FLASH.ld'
    tc = ToolChain( FINAL_OUTPUT_NAME, prjdir, build_variants, sdk_root, bsp_mx_root, freertos_root, lscript, "stm32" )
    return tc 
