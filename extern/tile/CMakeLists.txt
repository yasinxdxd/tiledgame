cmake_minimum_required(VERSION 3.16)
project(TiledGameTVM)

# C++ standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -static-libgcc -static-libstdc++")

# Source files
file(GLOB_RECURSE SRC_FILES
    ./*.c
)

# Include directories
include_directories(
    ./
    libffi-mingw64/include
    ../stb/include
)

# Library directory
link_directories(
    libffi-mingw64/lib
)

# Create the executable
add_library(${PROJECT_NAME} SHARED ${SRC_FILES})

# Link with raylib (precompiled)
target_link_libraries(
    ${PROJECT_NAME} 
    libffi.a
)

