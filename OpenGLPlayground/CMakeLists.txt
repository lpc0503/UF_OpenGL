cmake_minimum_required(VERSION 3.23)
project(OpenGLPlayGround)
set(CMAKE_CXX_STANDARD 17)

set(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
set(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
set(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)

add_subdirectory(deps)
add_subdirectory(common)

add_subdirectory(project1)
add_subdirectory(project2)
add_subdirectory(project3)
