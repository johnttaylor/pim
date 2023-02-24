#------------------------------------------------------------------------------
# TOOLCHAIN
#
#   Host:       Windows
#   Compiler:   Mingw_w64
#   Target:     Windows
#   Output:     64 bit Console application
#------------------------------------------------------------------------------

import sys
from nqbplib import base
from nqbplib import utils


class ToolChain( base.ToolChain ):
    def __init__( self, exename, prjdir, build_variants, default_variant='release' ):
        base.ToolChain.__init__( self, exename, prjdir, build_variants, default_variant )
        self._ccname = 'Mingw_W64'
        
        # more stuff to clean
        self._clean_list.extend( ['xml'] )

        # Disable the following 'string warnings' because they generate false-positives (i.e. issues with GCC itself)
        self._base_release.cflags = self._base_release.cflags + '-Wno-stringop-truncation -Wno-stringop-overflow '

        # Force ANSI standard printfs/scanf
        self._base_release.cflags = self._base_release.cflags + '-D__USE_MINGW_ANSI_STDIO=1'

        # Turn off ALL optimization on the debug build
        self._debug_release.cflags   = self._debug_release.cflags + ' -O0'
        
        # statically link my C/C++ libraries
        #self._base_release.linkflags  = self._base_release.linkflags + ' -static -static-libgcc -static-libstdc++ '
 
    #--------------------------------------------------------------------------
    def validate_cc( self ):
       t = base.ToolChain.validate_cc(self)
       if ( not '64' in t[1].decode() ):
           utils.output( "ERROR: Incorrect build of GCC (target does NOT equal 64Bit version)" )
           sys.exit(1)

       return t

    #--------------------------------------------------------------------------
    def _build_ar_rule( self ):
        self._win32_build_ar_rule()
