cmake_minimum_required(VERSION 3.5)

project(MyDraw LANGUAGES C CXX)

add_subdirectory("include/glfw-3.4")
include_directories("include/glad/include")

set(gladC "include/glad/src/glad.c")


add_executable(App window.cpp ${gladC} vertexData.h point.h)
target_link_libraries(App glfw)
