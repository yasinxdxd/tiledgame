cmake_minimum_required(VERSION 3.16)
project(TiledGame)


set(EXTERN_DIR ${PROJECT_SOURCE_DIR}/extern)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
    ${EXTERN_DIR}/tile/libffi-mingw64/include
)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(LIBTILEDGAMETVMDIR ${EXTERN_DIR}/tile/build/Release)
else()
    set(LIBTILEDGAMETVMDIR ${EXTERN_DIR}/tile/build/Debug)
endif()

# Library directory
link_directories(
    ${EXTERN_DIR}/glfw/lib
    ${EXTERN_DIR}/FreeType/lib
    ${LIBTILEDGAMETVMDIR}
)

# Create the executable
add_executable(${PROJECT_NAME} ${SRC_FILES})

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(LIBTILEDGAMETVM "libTiledGameTVM.lib")
else()
    set(LIBTILEDGAMETVM "libTiledGameTVM_d.lib")
endif()

# Link with raylib (precompiled)
target_link_libraries(
    ${PROJECT_NAME} 
    glfw3.lib
    freetype.lib
    ${LIBTILEDGAMETVM}
)

