#------------------------------------------------------------------------------
# TOOLCHAIN
#
#   Host:       Windows
#   Compiler:   VC++ 12 32bit (aka Visual Studio 2013)
#   Target:     Windows
#   Output:     32 bit Console application
#------------------------------------------------------------------------------

import sys, os
from nqbplib import base
from nqbplib import utils

class ToolChain( base.ToolChain ):

    #--------------------------------------------------------------------------
    def __init__( self, exename, prjdir, build_variants, default_variant='release' ):
        base.ToolChain.__init__( self, exename, prjdir, build_variants, default_variant )
        
        
        # Tools
        self._ccname   = 'VC++ 12, 32bit (Visual Studio 2013)'
        self._cc       = 'cl'  
        self._ld       = 'link'  
        self._asm      = 'masm'   
        self._ar       = 'lib'   
        self._objcpy   = ''
        
        self._obj_ext  = 'obj'    
        self._asm_ext  = 's'    
        
        self._echo_cc  = False 
        self._echo_asm = True
        
        self._validate_cc_options = ''
        
        self._cflag_symdef               = '/D '
        self._asmflag_symdef             = '/D '
        self._cflag_symvalue_delimiter   = '"'
        self._asmflag_symvalue_delimiter = '"'

        self._ar_library_name = 'library.lib'
        self._ar_options      = '/NOLOGO /OUT:' + self._ar_library_name
        
        self._link_lib_prefix       = ''
        self._linker_libgroup_start = ''
        self._linker_libgroup_end   = ''
        
        self._final_output_name = exename
        self._link_output       = '/OUT:' + exename
        
        
        #
        # Build Config/Variant: "release"
        #
        
        # Common/base options, flags, etc.
        # Notes:
        #   o Order of the header file include path should be in the 
        #     following order (i.e. first directory searched)
        #       - Current directory
        #       - Package source directory
        #       - Project directory
        #       - Workspace Public Include directory
        #

        # Orignal VC12 flags
        # self._base_release.cflags    = '/FS /nologo  /D "_CRT_SECURE_NO_WARNINGS" /D "_CONSOLE" /D "_MBCS" /Gm /Zi /FD /D "WIN32" /D "WIN32_LEAN_AND_MEAN" /c /D "BUILD_TIME_UTC={:d}"'.format(self._build_time_utc)

        # Flags compatible with Visual Studio 19
        self._base_release.cflags    = '/FS /nologo  /D "_CRT_SECURE_NO_WARNINGS" /D "_CONSOLE" /D "_MBCS" /Zi /FD /D "WIN32" /D "WIN32_LEAN_AND_MEAN" /c /D "BUILD_TIME_UTC={:d}"'.format(self._build_time_utc)
        self._base_release.linkflags = '/nologo /subsystem:console /pdb:{}.pdb /machine:X86'.format( os.path.splitext(exename)[0] )
        self._base_release.linklibs  = '' #kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib'
        
        # Optimized options, flags, etc.
        self._optimized_release.cflags    = '/D "NDEBUG"'
        self._optimized_release.linkflags = '/nologo /subsystem:console /pdb:{}.pdb /machine:X86'.format( os.path.splitext(exename)[0] )
        
        # Debug options, flags, etc.
        self._debug_release.cflags    = '/FS /Zi /Od /D "_DEBUG" /RTCs'
        self._debug_release.linkflags = '/debug /NODEFAULTLIB:LIBCMT '
        self._debug_release.linklibs  = 'libcmtd.lib'

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

