#------------------------------------------------------------------------------
# TOOLCHAIN
#
#   Host:       Linux
#   Compiler:   gcc
#   Target:     Linux
#   Output:     
#------------------------------------------------------------------------------

import os
import sys
from nqbplib import base
from nqbplib import utils

class ToolChain( base.ToolChain ):

    #--------------------------------------------------------------------------
    def __init__( self, exename, prjdir, build_variants, default_variant='release' ):
        base.ToolChain.__init__( self, exename, prjdir, build_variants, default_variant )
        self._ccname = 'GCC-ARM'

        # Check if there is specific path for the GCC toolchain
        gcc_bin = os.environ.get('NQBP_GCC-ARM_BIN')
        if ( gcc_bin != None ):
            self._ccname   = f'GCC @{gcc_bin}'
            self._cc       = os.path.join(gcc_bin, 'arm-linux-gnueabihf-gcc' )
            self._ld       = os.path.join(gcc_bin, 'arm-linux-gnueabihf-gcc' )
            self._asm      = os.path.join(gcc_bin, 'arm-linux-gnueabihf-as' )  
            self._ar       = os.path.join(gcc_bin, 'arm-linux-gnueabihf-ar' )  
            self._objcpy   = os.path.join(gcc_bin, 'arm-linux-gnueabihf-objcpy')
        else:
            self._cc       = 'arm-linux-gnueabihf-gcc'
            self._ld       = 'arm-linux-gnueabihf-gcc'
            self._asm      = 'arm-linux-gnueabihf-as'
            self._ar       = 'arm-linux-gnueabihf-ar'
            self._objcpy   = 'arm-linux-gnueabihf-objcpy'



        #
        # Build Config/Variant: "xyz"
        #
       
        # Disable the following 'string warnings' because they generate false-positives (i.e. issues with GCC itself)
        self._base_release.cflags = self._base_release.cflags + '-Wno-stringop-truncation -Wno-stringop-overflow '
        
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

