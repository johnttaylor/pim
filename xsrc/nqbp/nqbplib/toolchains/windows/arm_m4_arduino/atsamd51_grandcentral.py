#------------------------------------------------------------------------------
# TOOLCHAIN
#
#   Host:       Windows
#   Compiler:   avr-gcc 
#   Target:     Atmel SAM5D1-xxx / Ardunio
#   Output:     .BIN file
#------------------------------------------------------------------------------

import sys, os
from nqbplib import base
from nqbplib import utils
from nqbplib import my_globals

class ToolChain( base.ToolChain ):

    #--------------------------------------------------------------------------
    def __init__( self, exename, prjdir, build_variants, env_tools, env_support, env_cc_ver, env_bsp_ver, default_variant='release', env_error=None, override_freertos_config=False ):
        base.ToolChain.__init__( self, exename, prjdir, build_variants, default_variant )
        self._ccname   = 'GCC Arm-Cortex M4 (no eabi) Compiler'
        self._cc       = os.path.join( env_tools, 'tools', 'arm-none-eabi-gcc', env_cc_ver, 'bin', 'arm-none-eabi-gcc' )
        self._asm      = os.path.join( env_tools, 'tools', 'arm-none-eabi-gcc', env_cc_ver, 'bin', 'arm-none-eabi-gcc' )
        self._ld       = os.path.join( env_tools, 'tools', 'arm-none-eabi-gcc', env_cc_ver, 'bin', 'arm-none-eabi-gcc' )
        self._ar       = os.path.join( env_tools, 'tools', 'arm-none-eabi-gcc', env_cc_ver, 'bin', 'arm-none-eabi-ar' )
        self._objcpy   = os.path.join( env_tools, 'tools', 'arm-none-eabi-gcc', env_cc_ver, 'bin', 'arm-none-eabi-objcopy' )
        self._printsz  = os.path.join( env_tools, 'tools', 'arm-none-eabi-gcc', env_cc_ver, 'bin', 'arm-none-eabi-size' )

        self._asm_ext  = 'asm'    
        self._asm_ext2 = 'S'   

        # Cache potential error for environment variables not set
        self._env_error = env_error;

        self._clean_pkg_dirs.extend( ['arduino', '_arduino'] )

        # set the name of the linker output (not the final output)
        self._link_output = '-o ' + exename + '.elf'

        # Define paths
        samd_src_path         = os.path.join( my_globals.NQBP_WORK_ROOT(), env_support, 'arduino', 'hardware', 'samd', env_bsp_ver )
        sdk_src_path          = os.path.join( samd_src_path, 'cores', 'arduino' )
        arduino_src_path      = os.path.join( env_tools, "..", "arduino", 'tools')
        self._base_release.inc = self._base_release.inc + \
                ' -I' + sdk_src_path + \
                ' -I' + samd_src_path + r'\cores\arduino' + \
                ' -I' + samd_src_path + r'\cores\arduino\USB' + \
                ' -I' + samd_src_path + r'\variants\grand_central_m4' + \
                ' -I' + samd_src_path + r'\libraries\Wire' + \
                ' -I' + samd_src_path + r'\libraries\Spi' + \
                ' -I' + samd_src_path + r'\libraries\I2S' + \
                ' -I' + samd_src_path + r'\libraries\I2S' + \
                ' -I' + arduino_src_path + r'\CMSIS\4.5.0\CMSIS\Include' + \
                ' -I' + arduino_src_path + r'\CMSIS-ATMEL\1.2.0\CMSIS\Device\ATMEL' 

        #if ( not override_freertos_config ):
        #    self._base_release.inc = self._base_release.inc + ' -I' + freertos_src_path + r'\config'


        # 
        common_flags                    = ' -Os -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 --specs=nano.specs --specs=nosys.specs -u _printf_float '
        asm_and_compile_flags           = r' -D__FPU_PRESENT -DARM_MATH_CM4 -DARDUINO_ARCH_SAMD -D__SAMD51P20A__ -DARDUINO_GRAND_CENTRAL_M4 -DARDUINO_ARCH_SAMD -D__SAMD51P20A__ -DADAFRUIT_GRAND_CENTRAL_M4 -D__SAMD51__ -DUSB_VID=0x239A -DUSB_PID=0x8031 -DUSBCON -DUSB_CONFIG_POWER=100 "-DUSB_MANUFACTURER=\"Adafruit LLC\"" "-DUSB_PRODUCT=\"Adafruit Grand Central M4\"" '
        cpp_and_c_flags                 = ' -ffunction-sections -fdata-sections -nostdlib --param max-inline-insns-single=500'
        self._base_release.cflags       = self._base_release.cflags + common_flags + cpp_and_c_flags + asm_and_compile_flags + ' -g'
        self._base_release.c_only_flags = self._base_release.c_only_flags + ' -std=gnu11'
        self._base_release.cppflags     = self._base_release.cppflags + ' -std=gnu++11 -fno-threadsafe-statics -fno-rtti -fno-exceptions -DARDUINO_BSP_VERSION=\\"' + env_bsp_ver + '\\"'
        self._base_release.cppflags    += ' -Wno-restrict -Wno-address-of-packed-member -Wno-class-memaccess'
        self._base_release.asmflags     = asm_and_compile_flags + ' -c -x assembler-with-cpp'

        linker_search_path1          = os.path.join(samd_src_path, 'variants', 'grand_central_m4', "linker_scripts", "gcc" )
        linker_search_path2          = os.path.join(samd_src_path, 'variants', 'grand_central_m4' )
        #self._base_release.linklibs  = ' -Wl,--start-group -larm_cortexM4lf_math -lm -lstdc++ -Wl,--end-group'
        self._base_release.linklibs  = ' -Wl,--start-group -lm -lstdc++ -Wl,--end-group'
        self._base_release.linkflags = common_flags + ' -Wl,--gc-sections -save-temps -L{} -L{} -Wl,-Map,{}.map  -Wl,--unresolved-symbols=report-all -Wl,--warn-common -Wl,--warn-section-align -Wl,--cref -Wl,--check-sections'.format(linker_search_path1, linker_search_path2, exename)

        self._ar_options             = 'rcs ' + self._ar_library_name


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
        self._printer.output("= Creating BIN file ..." )

        # construct objcopy command
        options = '-O binary '
        objcpy = '{} {} {} {}'.format(  self._objcpy,
                                        options,
                                        self._final_output_name + '.elf',
                                        self._final_output_name + '.bin'
                                     )
                                          
        # Generate the .HEX file
        if ( arguments['-v'] ):
            self._printer.output( objcpy )
        if ( utils.run_shell(self._printer, objcpy) ):
            self._printer.output("=")
            self._printer.output("= Build Failed: Failed to create .BIN file from the .ELF" )
            self._printer.output("=")
            sys.exit(1)


        # Output Banner message
        self._printer.output("= Running Print Size..." )

        # construct zip command
        options = '--format=berkeley'
        printsz = '{} {} {}'.format( self._printsz,
                                        options,
                                        self._final_output_name + '.elf'
                                     )
                                          
        # Run the 'size' command
        if ( arguments['-v'] ):
            self._printer.output( printsz )
        utils.run_shell(self._printer, printsz)

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
