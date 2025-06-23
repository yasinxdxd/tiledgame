cmake_minimum_required(VERSION 3.16)
project(TiledGameTVM C)

# Set C standard
set(CMAKE_C_STANDARD 11)

# Detect build type and set output name + flags
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(OUTPUT_NAME "TiledGameTVM")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")
    set(OUTPUT_DIR "${CMAKE_BINARY_DIR}/Release")
else()
    set(OUTPUT_NAME "TiledGameTVM_d")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
    set(OUTPUT_DIR "${CMAKE_BINARY_DIR}/Debug")
endif()

# Sources
file(GLOB SRC_FILES
    ./*.c
    ./common/*.c
)

# Include common headers
include_directories(
    ./
    ../stb/include
)

# Platform-specific includes and libs
if(WIN32)
    include_directories(libffi-mingw64/include)
    link_directories(libffi-mingw64/lib)
elseif(UNIX)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(LIBFFI REQUIRED libffi)
    include_directories(${LIBFFI_INCLUDE_DIRS})
    link_directories(${LIBFFI_LIBRARY_DIRS})
endif()

# Build shared library
add_library(${OUTPUT_NAME} SHARED ${SRC_FILES})

# Set output directories
set_target_properties(${OUTPUT_NAME} PROPERTIES
    OUTPUT_NAME ${OUTPUT_NAME}
    RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR}
    ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR}
)

# Link libraries
if(WIN32)
    target_link_libraries(${OUTPUT_NAME} libffi.a)
elseif(UNIX)
    target_link_libraries(${OUTPUT_NAME} ${LIBFFI_LIBRARIES})
endif()
