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
from nqbplib.my_globals import NQBP_PRE_PROCESS_SCRIPT


#===================================================
# BEGIN EDITS/CUSTOMIZATIONS
#---------------------------------------------------

# Set the name for the final output item (with NO file extension)
FINAL_OUTPUT_NAME = 'pico-display'

# This is used for running the PIO Assembler in a given driver directory (e.g drivers\st7789)
NQBP_PRE_PROCESS_SCRIPT( "nqbp-preprocess.py" )

#
# For build config/variant: "Release"
#
 
# Additional PICO-SDK Header paths
sdk_src_path  = os.path.join( NQBP_PKG_ROOT(), 'xsrc', 'pico-sdk', 'src' )
sdk_inc       = f' -I{os.path.join(sdk_src_path,"rp2_common","hardware_i2c","include")}' +\
                f' -I{os.path.join(sdk_src_path,"rp2_common","hardware_spi","include")}' +\
                f' -I{os.path.join(sdk_src_path,"rp2_common","hardware_pwm","include")}' +\
                f' -I{os.path.join(sdk_src_path,"rp2_common","hardware_pio","include")}' +\
                f' -I{os.path.join(sdk_src_path,"rp2_common","hardware_dma","include")}' 

# Additional Header paths for PIMORONI supplied code
pimoroni_src_path = os.path.join( NQBP_PKG_ROOT(), 'xsrc', 'pimoroni-pico' )
pimoroni_inc      = f' -I{pimoroni_src_path}' + \
                    f' -I{os.path.join(pimoroni_src_path,"common")}' +\
                    f' -I{os.path.join(pimoroni_src_path,"drivers","rgbled")}' +\
                    f' -I{os.path.join(pimoroni_src_path,"drivers","st7789")}' +\
                    f' -I{os.path.join(pimoroni_src_path,"libraries","pico_display")}' +\
                    f' -I{os.path.join(pimoroni_src_path,"libraries","pico_graphcis")}' +\
                    f' -I{os.path.join(pimoroni_src_path,"libraries","pico_bitmap_fonts")}' +\
                    f' -I{os.path.join(pimoroni_src_path,"libraries","pico_hershey_fonts")}' 

# Set project specific 'base' (i.e always used) options
base_release = BuildValues()        # Do NOT comment out this line
common_flags           = ' -DPICO_STACK_SIZE=2048 -DPICO_COPY_TO_RAM=0 -DPICO_CXX_ENABLE_EXCEPTIONS=0 -DPICO_NO_FLASH=0 -DPICO_NO_HARDWARE=0 -DPICO_ON_DEVICE=1 -DPICO_USE_BLOCKED_RAM=0 '
base_release.cflags    = f' -Wall {common_flags}'
base_release.cppflags  = ' -std=gnu++17'
base_release.asmflags  = f' {common_flags}'
base_release.inc       = f'{sdk_inc}{pimoroni_inc}'


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
build_variants = { 'pico':release_opts,
#                  'xyz':xyz_opts,
                 }    

#---------------------------------------------------
# END EDITS/CUSTOMIZATIONS
#===================================================



# Capture project/build directory
import os
prjdir = os.path.dirname(os.path.abspath(__file__))


# Select Module that contains the desired toolchain
from nqbplib.toolchains.windows.arm_gcc_rp2040.stdio_serial import ToolChain


# Function that instantiates an instance of the toolchain
def create():
    tc = ToolChain( FINAL_OUTPUT_NAME, prjdir, build_variants, "pico", NQBP_PKG_ROOT(), "pico"  )
    return tc 
