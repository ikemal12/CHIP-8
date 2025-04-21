cmake_minimum_required(VERSION 3.10)
project(chip8-emulator)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SDL2
find_package(SDL2 REQUIRED)

# Include directories
include_directories(
    include
    src
    ${SDL2_INCLUDE_DIRS}
)

# Source files
set(SOURCES
    src/main.cpp
    src/chip.cpp
    src/platform.cpp
    src/gl.c
)

# Create executable
add_executable(chip8-emulator ${SOURCES})

# Link libraries - exclude SDL2main
target_link_libraries(chip8-emulator
    ${SDL2_LIBRARIES}
)

# Windows-specific settings
if(WIN32)
    # Force console subsystem and prevent SDL main override
    set_target_properties(chip8-emulator PROPERTIES
        LINK_FLAGS "-mconsole -Wl,--undefined=SDL_main"
    )
endif()