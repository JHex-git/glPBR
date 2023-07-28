cmake_minimum_required(VERSION 3.10)
project(glPBR)

SET(CMAKE_CXX_FLAGS_RELEASE "-O2")
SET(CMAKE_CXX_STANDARD 17)
set(RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

find_package(GLFW3 REQUIRED)
include_directories(src/)
add_library(glad_lib OBJECT src/opengl/glad.c)
add_library(shader_lib OBJECT src/shader/shader.cpp)
add_library(utility_lib OBJECT src/utility/stb_image.cpp)
add_executable(glPBR src/main.cpp)
target_link_libraries(glPBR glad_lib shader_lib utility_lib glfw)
