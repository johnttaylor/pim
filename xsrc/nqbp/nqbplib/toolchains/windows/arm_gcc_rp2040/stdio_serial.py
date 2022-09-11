#------------------------------------------------------------------------------
# TOOLCHAIN
#
#   Host:       Windows
#   Compiler:   avr-gcc 
#   Target:     Raspberry Pi Pico
#   Output:     .BIN|ELF|UF2 file
#------------------------------------------------------------------------------

import sys, os
from nqbplib import base
from nqbplib import utils
from nqbplib import my_globals

#include "C:/_workspaces/raspberry/pico-sdk/src/boards/include/boards/pico.h"

#// based on PICO_RP2040_CONFIG_HEADER_FILES:

class ToolChain( base.ToolChain ):

    #--------------------------------------------------------------------------
    def __init__( self, exename, prjdir, build_variants, board, pico_root, default_variant='release', env_error=None ):
        base.ToolChain.__init__( self, exename, prjdir, build_variants, default_variant )
        self._ccname     = 'GCC Arm-Cortex (none-eabi) Compiler'
        self._cc         = 'arm-none-eabi-gcc' 
        self._asm        = 'arm-none-eabi-gcc' 
        self._ld         = 'arm-none-eabi-gcc' 
        self._ar         = 'arm-none-eabi-ar' 
        self._objcpy     = 'arm-none-eabi-objcopy' 
        self._objdmp     = 'arm-none-eabi-objdump' 
        self._printsz    = 'arm-none-eabi-size'
        self._pad_chksum = os.path.join( pico_root, 'xsrc', 'pico-sdk', 'src', 'rp2_common', 'boot_stage2', "pad_checksum.py" )
        self._elf2uf2    = os.path.join( pico_root, 'xsrc', 'pico-sdk', 'tools', 'elf2uf2.exe' )
        self._pioasm     = os.path.join( pico_root, 'xsrc', 'pico-sdk', 'tools', 'pioasm.exe' )
        self._asm_ext    = 'asm'    
        self._asm_ext2   = 'S'   

        self._clean_pkg_dirs.extend( ['_pico'] )

        # set the name of the linker output (not the final output)
        self._link_output = '-o ' + exename + '.elf'

        # Define paths
        sdk_src_path = os.path.join( pico_root, 'xsrc', 'pico-sdk', 'src' )
        self._base_release.inc = self._base_release.inc + \
                ' -I' + sdk_src_path + r'\common\pico_stdlib\include' + \
                ' -I' + sdk_src_path + r'\common\pico_base\include' + \
                ' -I' + sdk_src_path + r'\common\pico_sync\include' + \
                ' -I' + sdk_src_path + r'\common\pico_bit_opts\include' + \
                ' -I' + sdk_src_path + r'\common\pico_divider\include' + \
                ' -I' + sdk_src_path + r'\common\pico_binary_info\include' + \
                ' -I' + sdk_src_path + r'\common\pico_time\include' + \
                ' -I' + sdk_src_path + r'\common\pico_util\include' + \
                ' -I' + sdk_src_path + r'\rp2_common\hardware_gpio\include' + \
                ' -I' + sdk_src_path + r'\rp2_common\pico_platform\include' + \
                ' -I' + sdk_src_path + r'\rp2_common\hardware_base\include' + \
                ' -I' + sdk_src_path + r'\rp2_common\hardware_claim\include' + \
                ' -I' + sdk_src_path + r'\rp2_common\hardware_sync\include' + \
                ' -I' + sdk_src_path + r'\rp2_common\hardware_ireq\include' + \
                ' -I' + sdk_src_path + r'\rp2_common\hardware_timer\include' + \
                ' -I' + sdk_src_path + r'\rp2_common\hardware_uart\include' + \
                ' -I' + sdk_src_path + r'\rp2_common\hardware_divider\include' + \
                ' -I' + sdk_src_path + r'\rp2_common\pico_runtime\include' + \
                ' -I' + sdk_src_path + r'\rp2_common\hardware_clocks\include' + \
                ' -I' + sdk_src_path + r'\rp2_common\hardware_resets\include' + \
                ' -I' + sdk_src_path + r'\rp2_common\hardware_irq\include' + \
                ' -I' + sdk_src_path + r'\rp2_common\hardware_pll\include' + \
                ' -I' + sdk_src_path + r'\rp2_common\hardware_vreg\include' + \
                ' -I' + sdk_src_path + r'\rp2_common\hardware_watchdog\include' + \
                ' -I' + sdk_src_path + r'\rp2_common\hardware_xosc\include' + \
                ' -I' + sdk_src_path + r'\rp2_common\hardware_flash\include' + \
                ' -I' + sdk_src_path + r'\rp2_common\pico_printf\include' + \
                ' -I' + sdk_src_path + r'\rp2_common\pico_bootrom\include' + \
                ' -I' + sdk_src_path + r'\rp2_common\pico_double\include' + \
                ' -I' + sdk_src_path + r'\rp2_common\pico_int64_ops\include' + \
                ' -I' + sdk_src_path + r'\rp2_common\pico_float\include' + \
                ' -I' + sdk_src_path + r'\rp2_common\pico_malloc\include' + \
                ' -I' + sdk_src_path + r'\rp2_common\pico_mem_ops\include' + \
                ' -I' + sdk_src_path + r'\rp2_common\boot_stage2\include' + \
                ' -I' + sdk_src_path + r'\rp2_common\pico_stdio\include' + \
                ' -I' + sdk_src_path + r'\rp2_common\pico_stdio_uart\include' + \
                ' -I' + sdk_src_path + r'\rp2_common\pico_multicore\include' + \
                ' -I' + sdk_src_path + r'\rp2_common\pico_unique_id\include ' + \
                ' -I' + sdk_src_path + r'\rp2040\hardware_regs\include' + \
                ' -I' + sdk_src_path + r'\rp2040\hardware_structs\include'


        # Include the target board's header file
        board_header = os.path.join( pico_root, "xsrc", "pico-sdk", "src", "boards", "include", "boards", board + '.h' )

          
        # 
        mcu                             = '-mcpu=cortex-m0plus'
        common_flags                    = f' -O3 -DPICO_CONFIG_HEADER={board_header} {mcu} -mthumb -ffunction-sections -fdata-sections -Wno-stringop-truncation -DPICO_TARGET_NAME=\\"{exename}\\" -DPICO_BOARD=\\"{board}\\" -DLIB_PICO_BIT_OPS=1 -DLIB_PICO_BIT_OPS_PICO=1 -DLIB_PICO_DIVIDER=1 -DLIB_PICO_DIVIDER_HARDWARE=1 -DLIB_PICO_DOUBLE=1 -DLIB_PICO_DOUBLE_PICO=1 -DLIB_PICO_FLOAT=1 -DLIB_PICO_FLOAT_PICO=1 -DLIB_PICO_INT64_OPS=1 -DLIB_PICO_INT64_OPS_PICO=1 -DLIB_PICO_MALLOC=1 -DLIB_PICO_MEM_OPS=1 -DLIB_PICO_MEM_OPS_PICO=1 -DLIB_PICO_PLATFORM=1 -DLIB_PICO_PRINTF=1 -DLIB_PICO_PRINTF_PICO=1 -DLIB_PICO_RUNTIME=1 -DLIB_PICO_STANDARD_LINK=1 -DLIB_PICO_STDIO=1 -DLIB_PICO_STDLIB=1 -DLIB_PICO_SYNC=1 -DLIB_PICO_SYNC_CORE=1 -DLIB_PICO_SYNC_CRITICAL_SECTION=1 -DLIB_PICO_SYNC_MUTEX=1 -DLIB_PICO_SYNC_SEM=1 -DLIB_PICO_TIME=1 -DLIB_PICO_UTIL=1 -DPICO_BUILD=1 -DLIB_PICO_UNIQUE_ID=1'
        common_flags                    = common_flags + ' -DLIB_PICO_STDIO_UART=1'  
        self._base_release.cflags       = self._base_release.cflags + common_flags
        self._base_release.c_only_flags = self._base_release.c_only_flags + ' -std=gnu11'
        self._base_release.cppflags     = self._base_release.cppflags + ' -Wno-restrict -Wno-address-of-packed-member -Wno-class-memaccess -DPICO_CXX_ENABLE_EXCEPTIONS=0 -fno-threadsafe-statics -fno-rtti -fno-exceptions -fno-unwind-tables -fno-use-cxa-atexit'
        self._base_release.asmflags     = self._base_release.cflags 
        self._base_release.asminc       = self._base_release.asminc + self._base_release.inc + ' -I ' + sdk_src_path + r'\rp2_common\boot_stage2\asminclude'
        
        wrapper_funcs                   = '-Wl,--wrap=printf -Wl,--wrap=vprintf -Wl,--wrap=puts -Wl,--wrap=putchar -Wl,--wrap=getchar -Wl,--wrap=sprintf -Wl,--wrap=snprintf -Wl,--wrap=vsnprintf -Wl,--wrap=__clzsi2 -Wl,--wrap=__clzdi2 -Wl,--wrap=__ctzsi2 -Wl,--wrap=__ctzdi2 -Wl,--wrap=__popcountsi2 -Wl,--wrap=__popcountdi2 -Wl,--wrap=__clz -Wl,--wrap=__clzl -Wl,--wrap=__clzll -Wl,--wrap=__aeabi_idiv -Wl,--wrap=__aeabi_idivmod -Wl,--wrap=__aeabi_ldivmod -Wl,--wrap=__aeabi_uidiv -Wl,--wrap=__aeabi_uidivmod -Wl,--wrap=__aeabi_uldivmod -Wl,--wrap=__aeabi_dadd -Wl,--wrap=__aeabi_ddiv -Wl,--wrap=__aeabi_dmul -Wl,--wrap=__aeabi_drsub -Wl,--wrap=__aeabi_dsub -Wl,--wrap=__aeabi_cdcmpeq -Wl,--wrap=__aeabi_cdrcmple -Wl,--wrap=__aeabi_cdcmple -Wl,--wrap=__aeabi_dcmpeq -Wl,--wrap=__aeabi_dcmplt -Wl,--wrap=__aeabi_dcmple -Wl,--wrap=__aeabi_dcmpge -Wl,--wrap=__aeabi_dcmpgt -Wl,--wrap=__aeabi_dcmpun -Wl,--wrap=__aeabi_i2d -Wl,--wrap=__aeabi_l2d -Wl,--wrap=__aeabi_ui2d -Wl,--wrap=__aeabi_ul2d -Wl,--wrap=__aeabi_d2iz -Wl,--wrap=__aeabi_d2lz -Wl,--wrap=__aeabi_d2uiz -Wl,--wrap=__aeabi_d2ulz -Wl,--wrap=__aeabi_d2f -Wl,--wrap=sqrt -Wl,--wrap=cos -Wl,--wrap=sin -Wl,--wrap=tan -Wl,--wrap=atan2 -Wl,--wrap=exp -Wl,--wrap=log -Wl,--wrap=ldexp -Wl,--wrap=copysign -Wl,--wrap=trunc -Wl,--wrap=floor -Wl,--wrap=ceil -Wl,--wrap=round -Wl,--wrap=sincos -Wl,--wrap=asin -Wl,--wrap=acos -Wl,--wrap=atan -Wl,--wrap=sinh -Wl,--wrap=cosh -Wl,--wrap=tanh -Wl,--wrap=asinh -Wl,--wrap=acosh -Wl,--wrap=atanh -Wl,--wrap=exp2 -Wl,--wrap=log2 -Wl,--wrap=exp10 -Wl,--wrap=log10 -Wl,--wrap=pow -Wl,--wrap=powint -Wl,--wrap=hypot -Wl,--wrap=cbrt -Wl,--wrap=fmod -Wl,--wrap=drem -Wl,--wrap=remainder -Wl,--wrap=remquo -Wl,--wrap=expm1 -Wl,--wrap=log1p -Wl,--wrap=fma -Wl,--wrap=__aeabi_lmul -Wl,--wrap=__aeabi_fadd -Wl,--wrap=__aeabi_fdiv -Wl,--wrap=__aeabi_fmul -Wl,--wrap=__aeabi_frsub -Wl,--wrap=__aeabi_fsub -Wl,--wrap=__aeabi_cfcmpeq -Wl,--wrap=__aeabi_cfrcmple -Wl,--wrap=__aeabi_cfcmple -Wl,--wrap=__aeabi_fcmpeq -Wl,--wrap=__aeabi_fcmplt -Wl,--wrap=__aeabi_fcmple -Wl,--wrap=__aeabi_fcmpge -Wl,--wrap=__aeabi_fcmpgt -Wl,--wrap=__aeabi_fcmpun -Wl,--wrap=__aeabi_i2f -Wl,--wrap=__aeabi_l2f -Wl,--wrap=__aeabi_ui2f -Wl,--wrap=__aeabi_ul2f -Wl,--wrap=__aeabi_f2iz -Wl,--wrap=__aeabi_f2lz -Wl,--wrap=__aeabi_f2uiz -Wl,--wrap=__aeabi_f2ulz -Wl,--wrap=__aeabi_f2d -Wl,--wrap=sqrtf -Wl,--wrap=cosf -Wl,--wrap=sinf -Wl,--wrap=tanf -Wl,--wrap=atan2f -Wl,--wrap=expf -Wl,--wrap=logf -Wl,--wrap=ldexpf -Wl,--wrap=copysignf -Wl,--wrap=truncf -Wl,--wrap=floorf -Wl,--wrap=ceilf -Wl,--wrap=roundf -Wl,--wrap=sincosf -Wl,--wrap=asinf -Wl,--wrap=acosf -Wl,--wrap=atanf -Wl,--wrap=sinhf -Wl,--wrap=coshf -Wl,--wrap=tanhf -Wl,--wrap=asinhf -Wl,--wrap=acoshf -Wl,--wrap=atanhf -Wl,--wrap=exp2f -Wl,--wrap=log2f -Wl,--wrap=exp10f -Wl,--wrap=log10f -Wl,--wrap=powf -Wl,--wrap=powintf -Wl,--wrap=hypotf -Wl,--wrap=cbrtf -Wl,--wrap=fmodf -Wl,--wrap=dremf -Wl,--wrap=remainderf -Wl,--wrap=remquof -Wl,--wrap=expm1f -Wl,--wrap=log1pf -Wl,--wrap=fmaf -Wl,--wrap=malloc -Wl,--wrap=calloc -Wl,--wrap=realloc -Wl,--wrap=free -Wl,--wrap=memcpy -Wl,--wrap=memset -Wl,--wrap=__aeabi_memcpy -Wl,--wrap=__aeabi_memset -Wl,--wrap=__aeabi_memcpy4 -Wl,--wrap=__aeabi_memset4 -Wl,--wrap=__aeabi_memcpy8 -Wl,--wrap=__aeabi_memset8'
        linker_script                   = os.path.join( pico_root, 'xsrc', 'pico-sdk', "src", "rp2_common", "pico_standard_link", "memmap_default.ld" )
        self._base_release.linkflags    = f' -nostartfiles {mcu} -mthumb -Wl,--build-id=none --specs=nosys.specs -Wl,-z,max-page-size=4096 -fno-rtti -fno-exceptions -fno-unwind-tables -fno-use-cxa-atexit -Wl,--gc-sections {wrapper_funcs} -Wl,--script={linker_script} bs2_default_padded_checksummed.S  -Wl,-Map={exename}.elf.map' 
                                        
        boot_linker_script              = os.path.join( pico_root, 'xsrc', 'pico-sdk', "src", "rp2_common", "boot_stage2", "boot_stage2.ld" )
        boot_obj                        = r'..\xsrc\pico-sdk\src\rp2_common\boot_stage2\compile_time_choice.o'
        self._bootloader_link_flags     = f'{mcu} -O3 -DNDEBUG -Wl,--build-id=none --specs=nosys.specs -nostartfiles -Wl,--script={boot_linker_script} -Wl,-Map=bs2_default.elf.map {boot_obj} -o bs2_default.elf'   
        

        # 
        self._base_release.firstobjs    = r'_BUILT_DIR_.xsrc\pico-sdk\src\rp2_common\pico_standard_link'


        self._ar_options                = 'rcs ' + self._ar_library_name

        # Optimized options, flags, etc.
        self._optimized_release.linkflags  = self._optimized_release.linkflags + ' -DNDEBUG'

        # Debug options, flags, etc.
        self._debug_release.cflags     = self._debug_release.cflags + r' -DDEBUG -DPICO_CMAKE_BUILD_TYPE=\"Debug\"'
        self._debug_release.linkflags  = self._debug_release.linkflags + ' -DDEBUG'
        
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
        # switch to the build variant output directory
        vardir = '_' + self._bld
        utils.create_subdirectory( self._printer, '.', vardir )
        utils.push_dir( vardir )

        # Finish creating the second state boot loader
        self._printer.output("= Creating Boot2 (aka the second stage bootloader) ..." )
        bootld = f'{self._ld} {self._bootloader_link_flags}'
        if ( arguments['-v'] ):
            self._printer.output( bootld )
        if ( utils.run_shell(self._printer, bootld) ):
            self._printer.output("=")
            self._printer.output("= Build Failed: Failed to link Boot2" )
            self._printer.output("=")
            sys.exit(1)
        objcpy = f'{self._objcpy} -O binary bs2_default.elf bs2_default.bin'
        if ( arguments['-v'] ):
            self._printer.output( objcpy )
        if ( utils.run_shell(self._printer, objcpy) ):
            self._printer.output("=")
            self._printer.output("= Build Failed: Failed create the .BIN file for the Boot2" )
            self._printer.output("=")
            sys.exit(1)
        objdmp = f'{self._objdmp} -h bs2_default.elf > bs2_default.dis'
        if ( arguments['-v'] ):
            self._printer.output( objdmp )
        if ( utils.run_shell(self._printer, objdmp) ):
            self._printer.output("=")
            self._printer.output("= Build Failed: Failed create the disassembly file for the Boot2" )
            self._printer.output("=")
            sys.exit(1)
        objdmp = f'{self._objdmp} -d bs2_default.elf >> bs2_default.dis'
        if ( arguments['-v'] ):
            self._printer.output( objdmp )
        if ( utils.run_shell(self._printer, objdmp) ):
            self._printer.output("=")
            self._printer.output("= Build Failed: Failed create the disassembly file for the Boot2" )
            self._printer.output("=")
            sys.exit(1)
        script = f'{self._pad_chksum} -s 0xffffffff bs2_default.bin bs2_default_padded_checksummed.S'
        if ( arguments['-v'] ):
            self._printer.output( script )
        if ( utils.run_shell(self._printer, script) ):
            self._printer.output("=")
            self._printer.output("= Build Failed: Failed create final .S file for Boot2" )
            self._printer.output("=")
            sys.exit(1)


        # Run the linker
        utils.pop_dir()
        base.ToolChain.link(self, arguments, inf, local_external_setting, variant )

        # switch to the build variant output directory
        utils.push_dir( vardir )


        # Output Banner message
        self._printer.output("= Creating BIN file ..." )

        # construct objcopy command
        objcpy = f'{self._objcpy} -O binary {self._final_output_name + ".elf"} {self._final_output_name + ".bin"}'
                                          
        # Generate the .BIN file
        if ( arguments['-v'] ):
            self._printer.output( objcpy )
        if ( utils.run_shell(self._printer, objcpy) ):
            self._printer.output("=")
            self._printer.output("= Build Failed: Failed to create .BIN file from the .ELF" )
            self._printer.output("=")
            sys.exit(1)

        # Generate the .HEX file
        objcpy = f'{self._objcpy} -O ihex {self._final_output_name + ".elf"} {self._final_output_name + ".hex"}'
        if ( arguments['-v'] ):
            self._printer.output( objcpy )
        if ( utils.run_shell(self._printer, objcpy) ):
            self._printer.output("=")
            self._printer.output("= Build Failed: Failed to create .HEX file from the .ELF" )
            self._printer.output("=")
            sys.exit(1)

        # Generate dissambly file
        objdmp = f'{self._objdmp} -h {self._final_output_name + ".elf"} > {self._final_output_name + ".dis"}'
        if ( arguments['-v'] ):
            self._printer.output( objdmp )
        if ( utils.run_shell(self._printer, objdmp) ):
            self._printer.output("=")
            self._printer.output("= Build Failed: Failed create the disassembly file for the Application" )
            self._printer.output("=")
            sys.exit(1)
        objdmp = f'{self._objdmp} -d {self._final_output_name + ".elf"} >> {self._final_output_name + ".dis"}'
        if ( arguments['-v'] ):
            self._printer.output( objdmp )
        if ( utils.run_shell(self._printer, objdmp) ):
            self._printer.output("=")
            self._printer.output("= Build Failed: Failed create the disassembly file for the Application" )
            self._printer.output("=")
            sys.exit(1)

        # Generate the UF2file
        objcpy = f'{self._elf2uf2} {self._final_output_name + ".elf"} {self._final_output_name + ".uf2"}'
        if ( arguments['-v'] ):
            self._printer.output( objcpy )
        if ( utils.run_shell(self._printer, objcpy) ):
            self._printer.output("=")
            self._printer.output("= Build Failed: Failed to create .UF2 file from the .ELF" )
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

