cmake_minimum_required(VERSION 3.23)
project(OpenGLPlayGround)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(deps)
add_subdirectory(common)

add_subdirectory(tutorial01_first_window)
add_subdirectory(tutorial02_red_triangle)
add_subdirectory(misc05_picking)