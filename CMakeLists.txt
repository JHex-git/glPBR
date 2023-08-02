cmake_minimum_required(VERSION 3.10)
project(glPBR)

SET(CMAKE_CXX_FLAGS_RELEASE "-O2")
SET(CMAKE_CXX_STANDARD 17)
set(RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

find_package(GLFW3 REQUIRED)
find_package(assimp REQUIRED)
include_directories(src/)
include_directories(${ASSIMP_INCLUDE_DIRS})
list(GET ASSIMP_INCLUDE_DIRS 0 ASSIMP_INCLUDE_DIR)
add_subdirectory(src/object3ds)
add_library(glad_lib OBJECT src/opengl/glad.c)
add_library(cameras_lib OBJECT src/cameras/camera.cpp)
add_library(shader_lib OBJECT src/shader/shader.cpp)
add_library(utility_lib OBJECT src/utility/stb_image.cpp)
add_executable(glPBR src/main.cpp)
target_link_libraries(glPBR glad_lib cameras_lib shader_lib utility_lib object3ds_lib glfw ${ASSIMP_LIBRARIES})


MACRO (COPY_GNU_DLL trgt libname)
ADD_CUSTOM_COMMAND (
    TARGET ${trgt}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND}
    ARGS -E copy_if_different "${ASSIMP_INCLUDE_DIR}/../bin/${libname}.dll" ${RUNTIME_OUTPUT_DIRECTORY}
)
ENDMACRO (COPY_GNU_DLL)

COPY_GNU_DLL(glPBR libassimp-5)