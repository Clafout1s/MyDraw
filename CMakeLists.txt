cmake_minimum_required(VERSION 3.5)

project(MyDraw LANGUAGES C CXX)

add_subdirectory("include/glfw-3.4")
include_directories("include/glad/include")

set(gladC "include/glad/src/glad.c")

add_library(Rectangle rectangle.cpp rectangle.h point.cpp point.h color.h)

add_executable(App window.cpp ${gladC} vertexData.h)
add_executable(Test test.cpp ${gladC} vertexData.h)
target_link_libraries(App glfw Rectangle)
target_link_libraries(Test glfw Rectangle)
