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

# get definition of the Options structure
from nqbplib.base import BuildValues
from nqbplib.my_globals import NQBP_WORK_ROOT

# Get the location of the compiler toolchain
env_error = None
KPIT_ROOT = os.environ.get( 'KPIT_GNURX_ROOT' )
if ( KPIT_ROOT == None ):
    KPIT_ROOT = env_error = "KPIT_ROOT"
KPIT_VER = os.environ.get( 'KPIT_GNURX_VERSION' )
if ( KPIT_VER == None ):
    KPIT_VER = env_error = "KPIT_GNURX_VERSION"




#===================================================
# BEGIN EDITS/CUSTOMIZATIONS
#---------------------------------------------------

# Set the name for the final output item     
FINAL_OUTPUT_NAME = 'a.x'

# Set the name of the linker script
LINKER_SCRIPT = 'ldscript-rx62n'

# Set reset vector/funciton name
RESET_VECTOR_NAME = '_PowerON_Reset'


# Define paths to needed libraries
LIB_PATH1 = os.path.join( KPIT_ROOT, 'lib', 'gcc', 'rx-elf', KPIT_VER, '64-bit-double' )
LIB_PATH2 = os.path.join( KPIT_ROOT, 'rx-elf', 'lib', '64-bit-double' )
 
# Define include path for the above libraries
INC_PATH1 = os.path.join( KPIT_ROOT, 'lib' 'gcc', 'rx-elf', KPIT_VER, 'include' )
INC_PATH2 = os.path.join( KPIT_ROOT, 'rx-elf', 'include' )

# Define include path for the FreeRTOS include directory
INC_FREERTOS1 = os.path.join( NQBP_WORK_ROOT(), "xpkgs", "freertos", "src", "include" )
INC_FREERTOS2 = os.path.join( NQBP_WORK_ROOT(), "xpkgs", "freertos", "src", "portable", "GCC", "RX600" )


#
# For build config/variant: "Release"
#

# Set project specific 'base' (i.e always used) options
base_release = BuildValues()        # Do NOT comment out this line
base_release.cflags    = '-ffunction-sections -fdata-sections  -Wall -Werror -D__RX_LITTLE_ENDIAN__=1 -mlittle-endian-data -m64bit-doubles -mint-register=0 -DCPPAPP'
base_release.cppflags  = '-std=c++11 -fno-exceptions -fno-rtti'
base_release.asmflags  = base_release.cflags + ' -x assembler-with-cpp ' 
base_release.inc       = ' -I{} -I{} -I{} -I{}'.format( INC_PATH1, INC_PATH2, INC_FREERTOS1, INC_FREERTOS2 )
base_release.asminc    = base_release.inc

base_release.firstobjs  = '..\\xpkgs\\colony.bsp.renesas.rx\src\\Bsp\\Renesas\Rx\\u62n\\Yrdkr62n\\Gnurx\\_no_c++_exceptions\*.o '
base_release.lastobjs  = '..\\xpkgs\\colony.bsp.renesas.rx\src\\Bsp\\Renesas\Rx\\u62n\\Yrdkr62n\\Gnurx\\_libc\*.a '
base_release.linkflags = '-ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -Wl,--gc-sections -T ..\\{} -e {} -L {} -L {}'.format(LINKER_SCRIPT, RESET_VECTOR_NAME, LIB_PATH1, LIB_PATH2 )
                                    
                                    
# Set project specific 'optimized' options
optimzed_release = BuildValues()    # Do NOT comment out this line
optimzed_release.cflags   = ' -Os '
optimzed_release.asmflags = optimzed_release.cflags

# Set project specific 'debug' options
debug_release = BuildValues()       # Do NOT comment out this line
debug_release.cflags   = ' -g2 '
debug_release.asmflags = '-Wa,--gdwarf-2 '




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
# build varaints
build_variants = { 'target':release_opts,
#                  'xyz':xyz_opts,
                 }    

#---------------------------------------------------
# END EDITS/CUSTOMIZATIONS
#===================================================



# Capture project/build directory
prjdir = os.path.dirname(os.path.abspath(__file__))


# Select Module that contains the desired toolchain
from nqbplib.toolchains.windows.kpit_gnurx_elf.elf import ToolChain


# Function that instantiates an instance of the toolchain
def create():
    tc = ToolChain( FINAL_OUTPUT_NAME, prjdir, build_variants, "target", env_error )
    return tc 
