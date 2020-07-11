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
    def __init__( self, exename, prjdir, build_variants, env_tools, env_support, env_cc_ver, env_bsp_ver, env_nfr_utils, default_variant='release', env_error=None, override_freertos_config=False ):
        base.ToolChain.__init__( self, exename, prjdir, build_variants, default_variant )
        self._ccname   = 'GCC Arm-Cortex M4 (no eabi) Compiler'
        self._cc       = os.path.join( env_tools, 'tools', 'gcc-arm-none-eabi', env_cc_ver, 'bin', 'arm-none-eabi-gcc' )
        self._asm      = os.path.join( env_tools, 'tools', 'gcc-arm-none-eabi', env_cc_ver, 'bin', 'arm-none-eabi-gcc' )
        self._ld       = os.path.join( env_tools, 'tools', 'gcc-arm-none-eabi', env_cc_ver, 'bin', 'arm-none-eabi-gcc' )
        self._ar       = os.path.join( env_tools, 'tools', 'gcc-arm-none-eabi', env_cc_ver, 'bin', 'arm-none-eabi-ar' )
        self._objcpy   = os.path.join( env_tools, 'tools', 'gcc-arm-none-eabi', env_cc_ver, 'bin', 'arm-none-eabi-objcopy' )
        self._nrfutil  = os.path.join( env_tools, 'hardware', 'nrf52', env_bsp_ver, 'tools', env_nfr_utils, 'binaries', 'win32', 'nrfutil.exe' )

        self._clean_pkg_dirs.extend( ['arduino', '_arduino'] )
        
        self._asm_ext  = 'asm'    
        self._asm_ext2 = 'S'   

        # Cache potential error for environment variables not set
        self._env_error = env_error;

        self._clean_list.extend( ('d') )

        # set the name of the linker output (not the final output)
        self._link_output = '-o ' + exename + '.elf'

        # Define paths
        nrf52_src_path         = os.path.join( my_globals.NQBP_WORK_ROOT(), env_support, 'arduino', 'hardware', 'nrf52', env_bsp_ver )
        sdk_src_path           = os.path.join( nrf52_src_path, 'cores', 'nRF5', 'SDK', 'components')
        freertos_src_path      = os.path.join( nrf52_src_path, 'cores', 'nRF5', 'freertos' )
        nffs_src_path          = os.path.join( nrf52_src_path, 'libraries', 'nffs', 'src' )
        self._base_release.inc = self._base_release.inc + \
                ' -I' + sdk_src_path + r'\toolchain' + \
                ' -I' + sdk_src_path + r'\toolchain\CMSIS\Include' + \
                ' -I' + sdk_src_path + r'\toolchain\gcc' + \
                ' -I' + sdk_src_path + r'\device' + \
                ' -I' + sdk_src_path + r'\drivers_nrf\delay' + \
                ' -I' + sdk_src_path + r'\softdevice\s132\headers' + \
                ' -I' + freertos_src_path + r'\source\include' + \
                ' -I' + freertos_src_path + r'\portable\GCC\nrf52' + \
                ' -I' + freertos_src_path + r'\portable\CMSIS\nrf52' + \
                ' -I' + nrf52_src_path + \
                ' -I' + nrf52_src_path + r'\cores\nRF5' + \
                ' -I' + nrf52_src_path + r'\variants\feather52' + \
                ' -I' + nrf52_src_path + r'\libraries\Bluefruit52Lib\src' + \
                ' -I' + nrf52_src_path + r'\libraries\Wire' + \
                ' -I' + nrf52_src_path + r'\libraries\Spi' + \
                ' -I' + nffs_src_path + \
                ' -I' + nffs_src_path + r'\fs\nffs\include' + \
                ' -I' + nffs_src_path + r'\fs\fs\include' + \
                ' -I' + nffs_src_path + r'\util\crc\include' + \
                ' -I' + nffs_src_path + r'\kernel\os\include' + \
                ' -I' + nffs_src_path + r'\kernel\os\include\os\arch\cortex_m4' + \
                ' -I' + nffs_src_path + r'\hw\hal\include' + \
                ' -I' + nffs_src_path + r'\sys\flash_map\include'
       
        if ( not override_freertos_config ):
            self._base_release.inc = self._base_release.inc + ' -I' + freertos_src_path + r'\config'


        # 
        common_flags                = ' -Os -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16'
        asm_and_compile_flags       = ' -DARDUINO_FEATHER52 -DARDUINO_ARCH_NRF52  -DNRF52 -DS132 -DNRF51_S132 -DUSE_LFXO'
        cpp_and_c_flags             = ' -MMD -DNRF5 -ffunction-sections -fdata-sections -nostdlib --param max-inline-insns-single=500'
        self._base_release.cflags       = self._base_release.cflags + common_flags + cpp_and_c_flags + asm_and_compile_flags + ' -g'
        self._base_release.c_only_flags = self._base_release.c_only_flags + ' -std=gnu11 -DSOFTDEVICE_PRESENT'
        self._base_release.cppflags     = self._base_release.cppflags + ' -std=gnu++11 -fno-threadsafe-statics -fno-rtti -fno-exceptions -DARDUINO_BSP_VERSION=\\"' + env_bsp_ver + '\\"'
        self._base_release.asmflags     = asm_and_compile_flags + ' -c -x assembler-with-cpp'

        linker_search_path1          = os.path.join(nrf52_src_path, 'variants', 'feather52' )
        linker_search_path2          = os.path.join(nrf52_src_path, 'cores', 'nRF5' )
        self._base_release.linklibs  = ' -Wl,--start-group -lm -Wl,--end-group'
        self._base_release.linkflags = common_flags + ' -Wl,--gc-sections -save-temps -L{} -L{} -Tfeather52_s132.ld -Wl,-Map,{}.map  -Wl,--unresolved-symbols=report-all -Wl,--warn-common -Wl,--warn-section-align --specs=nano.specs --specs=nosys.specs -Wl,--cref -Wl,--check-sections'.format(linker_search_path1, linker_search_path2, exename)

        self._ar_options      = 'rcs ' + self._ar_library_name

        self._debug_release.cflags    = self._debug_release.cflags + ' -DCFG_DEBUG=2'
        self._debug_release.asmflags  = self._debug_release.asmflags + ' -DCFG_DEBUG=2'
           
        self._optimized_release.cflags    = self._optimized_release.cflags + ' -DCFG_DEBUG=0'
        self._optimized_release.asmflags  = self._optimized_release.asmflags + ' -DCFG_DEBUG=0'


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
        self._printer.output("= Creating HEX file ..." )

        # construct objcopy command
        options = '-O ihex '
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

        # Output Banner message
        self._printer.output("= Creating the download (.ZIP) file ..." )

        # construct zip command
        options = 'dfu genpkg --dev-type 0x0052 --application'
        nrfutil = '{} {} {} {}'.format(  self._nrfutil,
                                        options,
                                        self._final_output_name + '.hex',
                                        self._final_output_name + '.zip'
                                     )
                                          
        # Generate the download file (aka the ZIP file)
        if ( arguments['-v'] ):
            self._printer.output( nrfutil )
        if ( utils.run_shell(self._printer, nrfutil) ):
            self._printer.output("=")
            self._printer.output("= Build Failed: Failed to create .ZIP file from the .HEX" )
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
