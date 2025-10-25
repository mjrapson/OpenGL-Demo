cmake_minimum_required (VERSION 3.25)

project(OpenGLDemo)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)

add_subdirectory(3rd)
add_subdirectory(src)
