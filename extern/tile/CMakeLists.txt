cmake_minimum_required(VERSION 3.16)
project(TiledGameTVM C)

# Force C standard
set(CMAKE_C_STANDARD 11)

# Detect Release/Debug and set flags
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(OUTPUT_NAME "TiledGameTVM")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -static-libgcc -static-libstdc++ -O3")
    set(OUTPUT_DIR "${CMAKE_BINARY_DIR}/Release")
else()
    set(OUTPUT_NAME "TiledGameTVM_d")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -static-libgcc -static-libstdc++ -g")
    set(OUTPUT_DIR "${CMAKE_BINARY_DIR}/Debug")
endif()

# Sources
file(GLOB_RECURSE SRC_FILES
    ./*.c
)

# Includes
include_directories(
    ./
    libffi-mingw64/include
    ../stb/include
)

# Link directories
link_directories(
    libffi-mingw64/lib
)

# Build shared library
add_library(${OUTPUT_NAME} SHARED ${SRC_FILES})

# Set output names explicitly
set_target_properties(${OUTPUT_NAME} PROPERTIES
    OUTPUT_NAME ${OUTPUT_NAME}
    RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR}    # DLL
    ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIR}    # Static lib (if needed)
    LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR}    # .lib or .a
)

# Link libffi (MinGW)
target_link_libraries(
    ${OUTPUT_NAME}
    libffi.a
)
