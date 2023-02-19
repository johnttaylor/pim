add_library(pico_unicorn INTERFACE)

pico_generate_pio_header(pico_unicorn ${CMAKE_CURRENT_LIST_DIR}/pico_unicorn.pio)

target_sources(pico_unicorn INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/pico_unicorn.cpp
)

target_include_directories(pico_unicorn INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Pull in pico libraries that we need
target_link_libraries(pico_unicorn INTERFACE pico_stdlib hardware_pio hardware_dma)
