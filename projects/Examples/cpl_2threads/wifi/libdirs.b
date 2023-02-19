# UART
src/Cpl/Io/Serial/RP2040/Uart
xsrc/pico-sdk/src/rp2_common/pico_stdio_uart

# OSAL support
src/Cpl/System/RP2040/_console

# Streams
src/Cpl/Io/Tcp/lwIP/Picow

# WIFI
xsrc/pico-sdk/src/rp2_common/pico_cyw43_arch 
xsrc/pico-sdk/src/rp2_common/cyw43_driver
xsrc/pico-sdk/lib/cyw43-driver/src < cyw43_ll.c cyw43_lwip.c cyw43_ctrl.c cyw43_stats.c
xsrc/pico-sdk/src/rp2_common/hardware_pio
xsrc/pico-sdk/src/rp2_common/hardware_spi
xsrc/pico-sdk/src/rp2_common/hardware_dma

# LWIP
xsrc/pico-sdk/lib/lwip/src/api
xsrc/pico-sdk/lib/lwip/src/core
xsrc/pico-sdk/lib/lwip/src/core/ipv4
xsrc/pico-sdk/lib/lwip/src/netif
xsrc/pico-sdk/src/rp2_common/pico_lwip