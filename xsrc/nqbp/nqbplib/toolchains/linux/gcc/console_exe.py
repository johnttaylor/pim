#------------------------------------------------------------------------------
# TOOLCHAIN
#
#   Host:       Linux
#   Compiler:   gcc
#   Target:     Linux
#   Output:     
#------------------------------------------------------------------------------

import sys
from nqbplib import base
from nqbplib import utils

class ToolChain( base.ToolChain ):

    #--------------------------------------------------------------------------
    def __init__( self, exename, prjdir, build_variants, default_variant='release' ):
        base.ToolChain.__init__( self, exename, prjdir, build_variants, default_variant )
        self._ccname = 'GCC'
        
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

