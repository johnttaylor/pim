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

        self._asm_ext  = 'asm'    
        self._asm_ext2 = 'S'   

        # Cache potential error for environment variables not set
        self._env_error = env_error;

        self._clean_pkg_dirs.extend( ['arduino', '_arduino'] )

        # set the name of the linker output (not the final output)
        self._link_output = '-o ' + exename + '.elf'

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

        self._base_release.linklibs      = ' -Wl,--start-group -lm -Wl,--end-group'
        self._base_release.linkflags     = common_flags + link_and_compile_flags + ' -fuse-linker-plugin -Wl,--gc-sections'

        self._ar_options      = 'rcs ' + self._ar_library_name

        self._debug_release.cflags   = self._debug_release.cflags + ' -DCFG_DEBUG=2'
        self._debug_release.asmflags = self._debug_release.asmflags + ' -DCFG_DEBUG=2'
  
        self._optimized_release.cflags    = self._optimized_release.cflags + ' -DCFG_DEBUG=0'
        self._optimized_release.asmflags  = self._optimized_release.asmflags + ' -DCFG_DEBUG=2'



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
    def link( self, arguments, inf, local_external_setting, variant ):
        # Run the linker
        base.ToolChain.link(self, arguments, inf, local_external_setting, variant )

        # switch to the build variant output directory
        vardir = '_' + self._bld
        utils.push_dir( vardir )

        # Output Banner message
        self._printer.output("= Creating EEPROM (eep) file ..." )

        # construct objcopy command
        options = '-O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0'
        objcpy = '{} {} {} {}'.format(  self._objcpy,
                                        options,
                                        self._final_output_name + '.elf',
                                        self._final_output_name + '.eep'
                                     )
                                          
        # Generate the .HEX file
        if ( arguments['-v'] ):
            self._printer.output( objcpy )
        if ( utils.run_shell(self._printer, objcpy) ):
            self._printer.output("=")
            self._printer.output("= Build Failed: Failed to create .EEP file from the .ELF" )
            self._printer.output("=")
            sys.exit(1)

        # Output Banner message
        self._printer.output("= Creating HEX file ..." )

        # construct objcopy command
        options = '-O ihex -R .eeprom'
        objcpy = '{} {} {} {}'.format(  self._objcpy,
                                        options,
                                        self._final_output_name + '.elf',
                                        self._final_output_name + '.hex'
                                     )
                                          
        # Generate the .HEX file
        if ( arguments['-v'] ):
            self._printer.output( objcpy )
        if ( utils.run_shell(self._printer, objcpy) ):
            self._printer.output("=")
            self._printer.output("= Build Failed: Failed to create .HEX file from the .ELF" )
            self._printer.output("=")
            sys.exit(1)

        # Return to project dir
        utils.pop_dir()
        

    #--------------------------------------------------------------------------
    def get_asm_extensions(self):
        extlist = [ self._asm_ext, self._asm_ext2 ]
        return extlist

    #--------------------------------------------------------------------------
    def validate_cc( self ):
        if ( self._env_error != None ):
            exit( "ERROR: The {} environment variable is not set.".format( self._env_error) )
        
        return base.ToolChain.validate_cc(self)
