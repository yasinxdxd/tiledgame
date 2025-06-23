cmake_minimum_required(VERSION 3.16)
project(TiledGame)

set(EXTERN_DIR ${PROJECT_SOURCE_DIR}/extern)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Source files
file(GLOB_RECURSE SRC_FILES
    src/*.cpp
    src/*.cc
    ${EXTERN_DIR}/imgui/src/*.cpp
    ${EXTERN_DIR}/glad/src/glad.c
    ${EXTERN_DIR}/texteditor/*.cpp
)

# Include directories
include_directories(
    ./include
    ${EXTERN_DIR}/glad/include
    ${EXTERN_DIR}/glfw/include
    ${EXTERN_DIR}/stb/include
    ${EXTERN_DIR}/FreeType/include
    ${EXTERN_DIR}/glm/include
    ${EXTERN_DIR}/OBJ_Loader/include
    ${EXTERN_DIR}/3d_loaders/my_obj_loader/include
    ${EXTERN_DIR}/tinygltfloader/include
    ${EXTERN_DIR}/imgui/include
    ${EXTERN_DIR}/texteditor/
    ${EXTERN_DIR}/tile/
)

if(WIN32)
    include_directories(${EXTERN_DIR}/tile/libffi-mingw64/include)
endif()

# TiledGameTVM lib path
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(WIN32)
        set(LIBTILEDGAMETVMDIR ${EXTERN_DIR}/tile/build/Release)
    else()
        set(LIBTILEDGAMETVMDIR ${EXTERN_DIR}/tile/build.linux/Release)
    endif()

else()
    if(WIN32)
        set(LIBTILEDGAMETVMDIR ${EXTERN_DIR}/tile/build/Debug)
    else()
        set(LIBTILEDGAMETVMDIR ${EXTERN_DIR}/tile/build.linux/Debug)
    endif()
endif()

# Platform-specific library linking
if(WIN32)
    link_directories(
        ${EXTERN_DIR}/glfw/lib
        ${EXTERN_DIR}/FreeType/lib
        ${LIBTILEDGAMETVMDIR}
    )

    set(LIB_GLFW glfw3.lib)
    set(LIB_FREETYPE freetype.lib)

    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        set(LIB_TVM ${LIBTILEDGAMETVMDIR}/libTiledGameTVM.lib)
    else()
        set(LIB_TVM ${LIBTILEDGAMETVMDIR}/libTiledGameTVM_d.lib)
    endif()

else()  # UNIX / Linux
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GLFW REQUIRED glfw3)
    pkg_check_modules(FREETYPE2 REQUIRED freetype2)

    include_directories(${GLFW_INCLUDE_DIRS})
    include_directories(${FREETYPE2_INCLUDE_DIRS})
    link_directories(${GLFW_LIBRARY_DIRS})
    link_directories(${FREETYPE2_LIBRARY_DIRS})
    link_directories(${LIBTILEDGAMETVMDIR})

    set(LIB_GLFW ${GLFW_LIBRARIES})
    set(LIB_FREETYPE ${FREETYPE2_LIBRARIES})

    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        set(LIB_TVM ${LIBTILEDGAMETVMDIR}/libTiledGameTVM.so)
    else()
        set(LIB_TVM ${LIBTILEDGAMETVMDIR}/libTiledGameTVM_d.so)
    endif()
endif()


# Create the executable
add_executable(${PROJECT_NAME} ${SRC_FILES})

# Link libraries
target_link_libraries(${PROJECT_NAME}
    ${LIB_GLFW}
    ${LIB_FREETYPE}
    ${LIB_TVM}
    dl    # Required for libffi on Linux
    pthread
)
