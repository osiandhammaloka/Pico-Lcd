add_library(pico_display INTERFACE)

#Temporarily overwrote behaviour of SDK
set(PICO_CXX_ENABLE_RTTI, 1)

target_sources(pico_display INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/display.cpp
)

target_include_directories(pico_display INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(pico_display INTERFACE
    pico_stdlib
    hardware_i2c
    hardware_gpio
)
