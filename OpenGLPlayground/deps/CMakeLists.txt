cmake_minimum_required(VERSION 3.10)

project(deps)

add_subdirectory(glm)
if(WIN32)
    add_subdirectory(glad-win)
else()
    add_subdirectory(glad)
endif()
add_subdirectory(glfw-3.3.8)
add_subdirectory(imgui)
add_subdirectory(fmt)
add_subdirectory(tinyobjloader)
