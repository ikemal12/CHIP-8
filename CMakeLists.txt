cmake_minimum_required(VERSION 3.10)
project(chip8-emulator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(SDL2 REQUIRED)

include_directories(
    include
    src
    ${SDL2_INCLUDE_DIRS}
)

set(SOURCES
    src/main.cpp
    src/chip.cpp
    src/platform.cpp
    src/gl.c
)

add_executable(chip8-emulator ${SOURCES})

target_link_libraries(chip8-emulator
    ${SDL2_LIBRARIES}
)

if(WIN32)
    set_target_properties(chip8-emulator PROPERTIES
        LINK_FLAGS "-mconsole -Wl,--undefined=SDL_main"
    )
endif()