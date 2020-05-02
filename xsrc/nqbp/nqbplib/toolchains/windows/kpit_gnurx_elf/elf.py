#------------------------------------------------------------------------------
# TOOLCHAIN
#
#   Host:       Windows
#   Compiler:   KPit Cummins - GNU RX-ELF (default is to use newlib)
#   Target:     Renesas RX
#   Output:     .MOT file
#------------------------------------------------------------------------------

import sys
import os
from nqbplib import base
from nqbplib import utils

class ToolChain( base.ToolChain ):

    #--------------------------------------------------------------------------
    def __init__( self, exename, prjdir, build_variants, default_variant='release', env_error=None ):
        base.ToolChain.__init__( self, exename, prjdir, build_variants, default_variant )
        self._ccname   = 'KPit Cummins - GNU RX-ELF'
        self._cc       = 'rx-elf-gcc'  
        self._ld       = 'rx-elf-gcc'  
        self._asm      = 'rx-elf-gcc'   
        self._ar       = 'rx-elf-ar'   
        self._objcpy   = 'rx-elf-objcpy'

        self._asm_ext  = 'asm'    
        self._asm_ext2 = 'S'    
        
        self._env_error = env_error;
        
        self._clean_list.extend( ('x', 'mot') )
        
        # Get root of the final output name
        exename_base = os.path.splitext(exename)[0] 

        # 
        self._base_release.cflags   = self._base_release.cflags + ' -mcpu=rx600 -Wa,-alhs=ME_CC_BASE_FILENAME.lst '
        self._base_release.asmflags = self._base_release.cflags

        self._base_release.linklibs  = ' -Wl,--start-group -lm -lc -lgcc -lstdc++ -Wl,--end-group '
        self._base_release.linkflags = ' -nostartfiles -Wl,-Map={}.map '.format(exename_base)

        self._debug_release.cflags   = self._debug_release.cflags + ' -g2 -D DEBUG'
        self._debug_release.asmflags = self._debug_release.cflags
           
        self._optimized_release.cflags    = self._optimized_release.cflags + ' -fno-function-cse -funit-at-a-time -falign-jumps -D RELEASE'
        self._optimized_release.asmflags  = self._optimized_release.cflags
        self._optimized_release.linkflags = '-Wl,--no-keep-memory -Wl,--strip-debug'

        
        #
        # Build Config/Variant: "xyz"
        #
       
        # Common/base options, flags, etc.
        
        #self._base_xyz = self._base_release.copy()
        #self._base_xyz.cflags  = '-c -DBUILD_TIME_UTC={:d}'.format(self._build_time_utc)
        
        # Optimized options, flags, etc.
        #self._optimized_xyz = self._optimized_release.copy()
        
        # Debug options, flags, etc.
        #self._debug_xyz = self._debug_release.copy()
        
        # Create new build variant - but ONLY if it DOES NOT already exist
        #if ( not self._bld_variants.has_key('xyz') ):
        #    self._bld_variants['xyz'] = { 'nop':'none' }
            
        # Add to dictionary of options for the 'xyz' build variant
        #self._bld_variants['xyz']['base']      = self._base_xyz
        #self._bld_variants['xyz']['optimized'] = self._optimized_xyz
        #self._bld_variants['xyz']['debug']     = self._debug_xyz

    #--------------------------------------------------------------------------
    def get_asm_extensions(self):
        extlist = [ self._asm_ext, self._asm_ext2 ]
        return extlist
        

    #--------------------------------------------------------------------------
    def validate_cc( self ):
        if ( self._env_error != None ):
            exit( "ERROR: The {} environment variable is not set.".format( self._env_error) )
        
        return base.ToolChain.validate_cc(self)

