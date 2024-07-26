#------------------------------------------------------------------------------
# TOOLCHAIN
#
#   Host:       Windows
#   Compiler:   arm-none-eabi-gcc
#   Target:     STM32F4xxx Micro-controllers
#   Output:     .BIN|ELF|UF2 file
#------------------------------------------------------------------------------

import sys, os
from nqbplib import base
from nqbplib import utils
from nqbplib import my_globals



class ToolChain( base.ToolChain ):

    #--------------------------------------------------------------------------
    def __init__( self, exename, prjdir, build_variants, sdk_root, bsp_mx_root, freertos_root, linker_script, default_variant='release', env_error=None ):
        base.ToolChain.__init__( self, exename, prjdir, build_variants, default_variant )
        self._ccname     = 'GCC Arm-Cortex (none-eabi) Compiler'
        self._cc         = 'arm-none-eabi-gcc' 
        self._asm        = 'arm-none-eabi-gcc' 
        self._ld         = 'arm-none-eabi-gcc' 
        self._ar         = 'arm-none-eabi-ar' 
        self._objcpy     = 'arm-none-eabi-objcopy' 
        self._objdmp     = 'arm-none-eabi-objdump' 
        self._printsz    = 'arm-none-eabi-size'
        self._asm_ext    = 's'    
        self._asm_ext2   = 'S'   
        self._shell      = 'cmd.exe /C'
        self._rm         = 'del /f'
        self._os_sep     = '/' # Force unix directory separator (for using a response file with gcc on Windoze Host)

        self._clean_pkg_dirs.extend( ['_stm32'] )

        # Define paths
        self._base_release.inc = self._base_release.inc + \
                ' -I' + sdk_root + r'\Drivers\STM32F4xx_HAL_Driver\Inc' + \
                ' -I' + sdk_root + r'\Drivers\STM32F4xx_HAL_Driver\Inc\Legacy' + \
                ' -I' + sdk_root + r'\Drivers\CMSIS\Device\ST\STM32F4xx\Include' + \
                ' -I' + sdk_root + r'\Drivers\CMSIS\Include' + \
                ' -I' + sdk_root + r'\Middlewares\Third_Party\FreeRTOS\Source\CMSIS_RTOS' + \
                ' -I' + freertos_root + r'\Include' + \
                ' -I' + freertos_root + r'\portable\GCC\ARM_CM4F' + \
                ' -I' + bsp_mx_root + \
                ' -I' + bsp_mx_root + r'\Core\Inc'


        # 
        mcu                             = '-mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb'
        common_flags                    = f'{mcu} --specs=nano.specs --specs=nosys.specs -u _printf_float '
        cpp_and_c_flags                 = '-ffunction-sections -fdata-sections -nostdlib --param max-inline-insns-single=500 -DUSE_HAL_DRIVER -Wno-array-bounds -Wno-stringop-truncation -Wno-stringop-overflow'
        self._base_release.c_only_flags = self._base_release.c_only_flags + ' -std=gnu11'
        self._base_release.cflags       = self._base_release.cflags + common_flags + cpp_and_c_flags + ' -g'
        self._base_release.cppflags     = self._base_release.cppflags + ' -Wno-restrict -Wno-address-of-packed-member -Wno-class-memaccess  -fno-rtti -fno-exceptions -fno-unwind-tables  -fno-threadsafe-statics -fno-use-cxa-atexit'

        self._base_release.asmflags     = self._base_release.cflags + ' -x assembler-with-cpp'
        self._base_release.asminc       = self._base_release.asminc + self._base_release.inc
        self._base_release.exclude_clangd.extend(['-std=gnu11', 
                                                  '--param max-inline-insns-single=500',
                                                  '-mcpu=cortex-m4',
                                                  '-mfpu=fpv4-sp-d16',
                                                  '-mfloat-abi=hard',
                                                  '-mthumb',
                                                  '--specs=nano.specs'])
        
        linker_script                   = os.path.join( bsp_mx_root, linker_script )
        self._base_release.linkflags    = f' {common_flags} -T{linker_script}  -Wl,-Map={exename}.map -Wl,--gc-sections' 
        self._base_release.linklibs     = ' -Wl,--start-group -lc -lm -Wl,--end-group'                                        
        

        # Optimized options, flags, etc.
        self._optimized_release.linkflags  = self._optimized_release.linkflags + ' -DNDEBUG'

        # Debug options, flags, etc.
        self._debug_release.cflags     = self._debug_release.cflags + r' -g3 -DDEBUG'
        
 
   #--------------------------------------------------------------------------
    def link( self, arguments, inf, local_external_setting, variant ):
        # Run the linker
        base.ToolChain.link(self, arguments, inf, local_external_setting, variant, extra_inputs_list=None, outname=self._final_output_name + ".elf" )


        # Generate the .BIN file
        self._ninja_writer.build(
               outputs    = self._final_output_name + ".bin",
               rule       = 'objcpy_rule',
               inputs     = self._final_output_name + ".elf" ,
               variables  = {"objcpy_opts":'-O binary'} )
        self._ninja_writer.newline()

        # Generate the .HEX file
        self._ninja_writer.build(
               outputs    = self._final_output_name + ".hex",
               rule       = 'objcpy_rule',
               inputs     = self._final_output_name + ".elf",
               variables  = {"objcpy_opts":'-O ihex'} )
        self._ninja_writer.newline()
 
        # Generate list file
        self._ninja_writer.build(
               outputs    = self._final_output_name + ".list",
               rule       = 'objdmp_rule',
               inputs     = self._final_output_name + ".elf",
               variables  = {"objdmp_opts":'-h -S'} )
        self._ninja_writer.newline()

        # Run the 'size' command
        self._ninja_writer.rule( 
            name = 'print_size', 
            command = f'$shell {self._printsz} --format=berkeley {self._final_output_name+ ".elf"}', 
            description = "Generic Command: $cmd" )
        self._create_always_build_statments( "print_size", "dummy_printsize", impilicit_list=self._final_output_name+ ".elf" )
 
        return None
 
    def finalize( self, arguments, builtlibs, objfiles, local_external_setting, linkout=None ):
        self._ninja_writer.default( [self._final_output_name + ".bin", self._final_output_name + ".hex", self._final_output_name + ".list", "dummy_printsize_final"] )

    #--------------------------------------------------------------------------
    def get_asm_extensions(self):
        extlist = [ self._asm_ext, self._asm_ext2 ]
        return extlist


    # Because Windoze is pain!
    def _build_ar_rule( self ):
        self._win32_withrspfile_build_ar_rule()

    def _build_compile_rule( self ):
        self._build_withrspfile_compile_rule()

    def _build_assembly_rule( self ):
        self._build_withrspfile_assembly_rule()

    def _build_link_rule( self ):
        self._build_withrspfile_link_rule()