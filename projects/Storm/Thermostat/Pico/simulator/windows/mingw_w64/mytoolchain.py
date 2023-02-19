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

import os

# get definition of the Options strcuture
from nqbplib.base import BuildValues
from nqbplib.my_globals import NQBP_PKG_ROOT


#===================================================
# BEGIN EDITS/CUSTOMIZATIONS
#---------------------------------------------------

# Set the name for the final output item
FINAL_OUTPUT_NAME = 'thermostat-sim.exe'

#
# For build config/variant: "Release"
#

# PICO-SDK Header paths
sdk_src_path  = os.path.join( NQBP_PKG_ROOT(), 'xsrc', 'pico-sdk', 'src' )
sdk_inc       = f' -I{os.path.join(sdk_src_path,"rp2_common","include")}' + \
                f' -I{os.path.join(sdk_src_path,"common","pico_stdlib", "include")}' + \
                f' -I{os.path.join(sdk_src_path,"common","pico_base", "include")}' + \
                f' -I{os.path.join(sdk_src_path,"rp2040","hardware_regs", "include")}' + \
                f' -I{os.path.join(sdk_src_path,"rp2040","hardware_structs", "include")}' + \
                f' -I{os.path.join(sdk_src_path,"rp2_common","pico_stdio", "include")}' + \
                f' -I{os.path.join(sdk_src_path,"rp2_common","hardware_uart", "include")}' + \
                f' -I{os.path.join(sdk_src_path,"rp2_common","hardware_irq", "include")}' + \
                f' -I{os.path.join(sdk_src_path,"rp2_common","hardware_gpio", "include")}' + \
                f' -I{os.path.join(sdk_src_path,"rp2_common","hardware_base", "include")}' + \
                f' -I{os.path.join(sdk_src_path,"rp2_common","hardware_timer", "include")}' + \
                f' -I{os.path.join(sdk_src_path,"rp2_common","hardware_spi", "include")}' + \
                f' -I{os.path.join(sdk_src_path,"rp2_common","hardware_i2c", "include")}' + \
                f' -I{os.path.join(sdk_src_path,"rp2_common","pico_stdio_uart", "include")}' + \
                f' -I{os.path.join(sdk_src_path,"common","pico_time", "include")}' + \
                f' -I{os.path.join(sdk_src_path,"rp2_common","pico_platform", "include")}'

# Additional Header paths for PIMORONI supplied code
pimoroni_src_path = os.path.join( NQBP_PKG_ROOT(), 'xsrc', 'pimoroni-pico' )
pimoroni_inc      = f' -I{pimoroni_src_path}' + \
                    f' -I{os.path.join(pimoroni_src_path,"common")}' +\
                    f' -I{os.path.join(pimoroni_src_path,"libraries","pico_display")}' +\
                    f' -I{os.path.join(pimoroni_src_path,"libraries","pico_graphcis")}' +\
                    f' -I{os.path.join(pimoroni_src_path,"libraries","pico_bitmap_fonts")}' +\
                    f' -I{os.path.join(pimoroni_src_path,"libraries","pico_hershey_fonts")}' 

# Magic defines need to be able to compiler the PICO/pimoroni header files
extra_cflags = '-DLIB_PICO_BIT_OPS=1 -DLIB_PICO_BIT_OPS_PICO=1 -DLIB_PICO_DIVIDER=1 -DLIB_PICO_DIVIDER_HARDWARE=1 -DLIB_PICO_DOUBLE=1 -DLIB_PICO_DOUBLE_PICO=1 -DLIB_PICO_FLOAT=1 -DLIB_PICO_FLOAT_PICO=1 -DLIB_PICO_INT64_OPS=1 -DLIB_PICO_INT64_OPS_PICO=1 -DLIB_PICO_MALLOC=1 -DLIB_PICO_MEM_OPS=1 -DLIB_PICO_MEM_OPS_PICO=1 -DLIB_PICO_PLATFORM=1 -DLIB_PICO_PRINTF=1 -DLIB_PICO_PRINTF_PICO=1 -DLIB_PICO_RUNTIME=1 -DLIB_PICO_STANDARD_LINK=1 -DLIB_PICO_STDIO=1 -DLIB_PICO_STDLIB=1 -DLIB_PICO_SYNC=1 -DLIB_PICO_SYNC_CORE=1 -DLIB_PICO_SYNC_CRITICAL_SECTION=1 -DLIB_PICO_SYNC_MUTEX=1 -DLIB_PICO_SYNC_SEM=1 -DLIB_PICO_TIME=1 -DLIB_PICO_UTIL=1 -DPICO_BUILD=1 -DLIB_PICO_UNIQUE_ID=1 -DLIB_PICO_STDIO_UART=1 -DPICO_BOARD=\\"pico\\" -DPICO_STACK_SIZE=2048 -DPICO_COPY_TO_RAM=0 -DPICO_CXX_ENABLE_EXCEPTIONS=0 -DPICO_NO_FLASH=0 -DPICO_NO_HARDWARE=0 -DPICO_ON_DEVICE=1 -DPICO_USE_BLOCKED_RAM=0 -DPICO_DEFAULT_UART_INSTANCE=uart0'

# Set project specific 'base' (i.e always used) options
base_release           = BuildValues()        # Do NOT comment out this line
base_release.cflags    = f'-Wno-attributes -m32 -D__unused= -D__always_inline=__attribute__((always_inline)) {extra_cflags}'
base_release.cppflags   = f'-std=gnu++17'
base_release.inc       = f'{sdk_inc}{pimoroni_inc}'
base_release.linkflags = '-m32 -fprofile-arcs'
base_release.linklibs  = '-lgcov -lws2_32'

# Set project specific 'optimized' options
optimzed_release           = BuildValues()    # Do NOT comment out this line
optimzed_release.cflags    = '-O3'
optimzed_release.linklibs  = '-lstdc++'

# Set project specific 'debug' options
debug_release           = BuildValues()       # Do NOT comment out this line
debug_release.linklibs  = '-lstdc++'



# 
# For build config/variant: "cpp11"
# (note: uses same internal toolchain options as the 'Release' variant, 
#        only the 'User' options will/are different)
#

# Construct option structs
base_cpp11     = BuildValues()  
optimzed_cpp11 = BuildValues()
debug_cpp11    = BuildValues()

# Set 'base' options
base_cpp11.cflags     = '-m64 -std=c++11 -Wall -Werror -x c++ -DCATCH_CONFIG_FAST_COMPILE'
base_cpp11.linkflags  = '-m64'
base_cpp11.linklibs   = '-lws2_32'

# Set 'Optimized' options
optimzed_cpp11.cflags    = '-O3'
optimzed_cpp11.linklibs  = '-lstdc++'

# Set 'debug' options
debug_cpp11.linklibs  = '-lstdc++'


#
# For build config/variant: "win64"
# (note: uses same internal toolchain options as the 'Release' variant,
#        only the 'User' options will/are different)
#

# Construct option structs
base_win64     = BuildValues()
optimzed_win64 = BuildValues()
debug_win64    = BuildValues()

# Set 'base' options
base_win64.cflags     = '-m64 -std=c++11 -Wall -Werror -x c++ -DCATCH_CONFIG_FAST_COMPILE'
base_win64.linkflags  = '-m64'
base_win64.linklibs   = '-lws2_32'

# Set 'Optimized' options
optimzed_win64.cflags    = '-O3'
optimzed_win64.linklibs  = '-lstdc++'

# Set 'debug' options
debug_win64.linklibs  = '-lstdc++'

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
cpp11_opts = { 'user_base':base_cpp11, 
               'user_optimized':optimzed_cpp11, 
               'user_debug':debug_cpp11
             }
  
# Add new dictionary of for new build configuration options
win64_opts = { 'user_base':base_win64,
               'user_optimized':optimzed_win64,
               'user_debug':debug_win64
             }
               
        
# Add new variant option dictionary to # dictionary of 
# build variants
build_variants = { 'win32':release_opts,
                   'win64':win64_opts,
                   'cpp11':cpp11_opts,
                 }    


#---------------------------------------------------
# END EDITS/CUSTOMIZATIONS
#===================================================



# Capture project/build directory
import os
prjdir = os.path.dirname(os.path.abspath(__file__))


# Select Module that contains the desired toolchain
from nqbplib.toolchains.windows.mingw_w64.console_exe import ToolChain


# Function that instantiates an instance of the toolchain
def create():
    tc = ToolChain( FINAL_OUTPUT_NAME, prjdir, build_variants, "win32" )
    return tc 
