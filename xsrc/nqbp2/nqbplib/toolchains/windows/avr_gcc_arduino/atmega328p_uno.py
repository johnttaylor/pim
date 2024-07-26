#------------------------------------------------------------------------------
# TOOLCHAIN
#
#   Host:       Windows
#   Compiler:   avr-gcc 
#   Target:     atmega-xxx / Ardunio
#   Output:     .HEX file
#------------------------------------------------------------------------------

import sys, os
from nqbplib import base
from nqbplib import utils
from nqbplib import my_globals

class ToolChain( base.ToolChain ):

    #--------------------------------------------------------------------------
    def __init__( self, exename, prjdir, build_variants, env_tools, env_support, env_cc_ver, env_bsp_ver, default_variant='release', env_error=None ):
        base.ToolChain.__init__( self, exename, prjdir, build_variants, default_variant )
        self._ccname   = 'AVR-GCC-ATMega328p Ardunio'
        self._cc       = os.path.join( env_tools, env_cc_ver, 'hardware', 'tools', 'avr', 'bin', 'avr-gcc' )
        self._asm      = os.path.join( env_tools, env_cc_ver, 'hardware', 'tools', 'avr', 'bin', 'avr-gcc' )
        self._ld       = os.path.join( env_tools, env_cc_ver, 'hardware', 'tools', 'avr', 'bin', 'avr-gcc' )
        self._ar       = os.path.join( env_tools, env_cc_ver, 'hardware', 'tools', 'avr', 'bin', 'avr-gcc-ar' )
        self._objcpy   = os.path.join( env_tools, env_cc_ver, 'hardware', 'tools', 'avr', 'bin', 'avr-objcopy' )
        self._printsz  = os.path.join( env_tools, env_cc_ver, 'hardware', 'tools', 'avr', 'bin', 'avr-size' )

        self._asm_ext  = 'asm'    
        self._asm_ext2 = 'S'   

        self._shell      = 'cmd.exe /C'
        self._rm         = 'del /f /q'

        # Cache potential error for environment variables not set
        self._env_error = env_error;

        self._clean_pkg_dirs.extend( ['arduino', '_arduino'] )

        # set the name of the linker output (not the final output)
        self._link_output = '-o'

        # Define paths
        core_path     = os.path.join(my_globals.NQBP_WORK_ROOT(), env_support, 'arduino', 'hardware', 'avr', env_bsp_ver, 'cores', 'arduino' )
        hardware_path = os.path.join(my_globals.NQBP_WORK_ROOT(), env_support, 'arduino', 'hardware', 'avr', env_bsp_ver, 'variants', 'standard' )
        self._base_release.inc = self._base_release.inc + " -I{} -I{} ".format(core_path, hardware_path) 

        # 
        common_flags                = ' -mmcu=atmega328p -Os'
        link_and_compile_flags      = ' -flto'
        asm_and_compile_flags       = ' -MMD -DARDUINO_AVR_UNO -DARDUINO_ARCH_AVR'

        self._base_release.cflags        = self._base_release.cflags + common_flags + link_and_compile_flags + asm_and_compile_flags + ' -ffunction-sections -fdata-sections'
        self._base_release.c_only_flags  = self._base_release.c_only_flags + ' -std=gnu11 -fno-fat-lto-objects'
        self._base_release.cppflags      = self._base_release.cppflags + ' -std=gnu++11 -fpermissive -fno-exceptions -fno-rtti -fno-threadsafe-statics'
        self._base_release.asmflags      = common_flags + asm_and_compile_flags + ' -c -x assembler-with-cpp'
        self._base_release.exclude_clangd.append('-std=gnu11')

        self._base_release.linklibs      = ' -Wl,--start-group -lm -Wl,--end-group'
        self._base_release.linkflags     = common_flags + link_and_compile_flags + ' -fuse-linker-plugin -Wl,--gc-sections'

        self._ar_options      = 'rcs'

        self._debug_release.cflags   = self._debug_release.cflags + ' -DCFG_DEBUG=2'
        self._debug_release.asmflags = self._debug_release.asmflags + ' -DCFG_DEBUG=2'
  
        self._optimized_release.cflags    = self._optimized_release.cflags + ' -DCFG_DEBUG=0'
        self._optimized_release.asmflags  = self._optimized_release.asmflags + ' -DCFG_DEBUG=2'



    #--------------------------------------------------------------------------
    def link( self, arguments, inf, local_external_setting, variant ):
        # Run the linker
        base.ToolChain.link(self, arguments, inf, local_external_setting, variant, outname=self._final_output_name + ".elf" )

        # Generate the .EEP file
        self._ninja_writer.build(
               outputs    = self._final_output_name + ".eep",
               rule       = 'objcpy_rule',
               inputs     = self._final_output_name + ".elf" ,
               variables  = {"objcpy_opts":'-O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0'} )
        self._ninja_writer.newline()

        # Generate the .HEX file
        self._ninja_writer.build(
               outputs    = self._final_output_name + ".hex",
               rule       = 'objcpy_rule',
               inputs     = self._final_output_name + ".elf" ,
               variables  = {"objcpy_opts":'-O ihex -R .eeprom'} )
        self._ninja_writer.newline()

        # Run the 'size' command
        self._ninja_writer.rule( 
            name = 'print_size', 
            command = f'$shell {self._printsz} --format=berkeley {self._final_output_name+".elf"}', 
            description = "Generic Command: $cmd" )
        self._create_always_build_statments( "print_size", "dummy_printsize", impilicit_list=self._final_output_name+ ".bin" )

    
    def finalize( self, arguments, builtlibs, objfiles, local_external_setting, linkout=None ):
        self._ninja_writer.default( [self._final_output_name + ".hex", self._final_output_name + ".eep","dummy_printsize_final"] )
       
        

    #--------------------------------------------------------------------------
    def get_asm_extensions(self):
        extlist = [ self._asm_ext, self._asm_ext2 ]
        return extlist

    #--------------------------------------------------------------------------
    def validate_cc( self ):
        if ( self._env_error != None ):
            exit( "ERROR: The {} environment variable is not set.".format( self._env_error) )
        
        return base.ToolChain.validate_cc(self)

    #--------------------------------------------------------------------------
    # Because Windoze is pain!
    def _build_ar_rule( self ):
        self._ninja_writer.rule( 
            name = 'ar', 
            command = 'cmd.exe /C "$rm $out 1>nul 2>nul && $ar ${aropts} ${arout}${out} $in"', 
            description = "Archiving Directory: $out" )
        self._ninja_writer.newline()