# Platform
src/Cpl/Io/Serial/RP2040/Uart
src/Cpl/Io < Input.cpp Output.cpp

# OSAL
src/Cpl/System/RP2040
src/Cpl/System/RP2040/_fatalError
src/Cpl/System/RP2040/_shutdown
src/Cpl/System/RP2040/_console
src/Cpl/System/_trace

# BSP
src/Bsp/RP2040/Pico/gcc

# SDK-Multicore
xsrc/pico-sdk/src/rp2_common/pico_multicore

# Core SDK
xsrc/pico-sdk/src/common/pico_sync
xsrc/pico-sdk/src/common/pico_time
xsrc/pico-sdk/src/common/pico_util
xsrc/pico-sdk/src/rp2_common/boot_stage2 < compile_time_choice.S
xsrc/pico-sdk/src/rp2_common/pico_stdlib
xsrc/pico-sdk/src/rp2_common/hardware_gpio
xsrc/pico-sdk/src/rp2_common/hardware_claim
xsrc/pico-sdk/src/rp2_common/pico_platform
xsrc/pico-sdk/src/rp2_common/hardware_sync
xsrc/pico-sdk/src/rp2_common/hardware_irq
xsrc/pico-sdk/src/rp2_common/hardware_timer
xsrc/pico-sdk/src/rp2_common/hardware_uart
xsrc/pico-sdk/src/rp2_common/hardware_divider
xsrc/pico-sdk/src/rp2_common/pico_runtime
xsrc/pico-sdk/src/rp2_common/hardware_clocks
xsrc/pico-sdk/src/rp2_common/hardware_pll
xsrc/pico-sdk/src/rp2_common/hardware_vreg
xsrc/pico-sdk/src/rp2_common/hardware_watchdog
xsrc/pico-sdk/src/rp2_common/hardware_xosc
xsrc/pico-sdk/src/rp2_common/hardware_flash
xsrc/pico-sdk/src/rp2_common/pico_printf
xsrc/pico-sdk/src/rp2_common/pico_bit_ops
xsrc/pico-sdk/src/rp2_common/pico_bootrom
xsrc/pico-sdk/src/rp2_common/pico_divider
xsrc/pico-sdk/src/rp2_common/pico_double > double_none.S
xsrc/pico-sdk/src/rp2_common/pico_int64_ops
xsrc/pico-sdk/src/rp2_common/pico_float > float_none.S
xsrc/pico-sdk/src/rp2_common/pico_malloc
xsrc/pico-sdk/src/rp2_common/pico_mem_ops
xsrc/pico-sdk/src/rp2_common/pico_standard_link
xsrc/pico-sdk/src/rp2_common/pico_stdio
xsrc/pico-sdk/src/rp2_common/pico_unique_id
