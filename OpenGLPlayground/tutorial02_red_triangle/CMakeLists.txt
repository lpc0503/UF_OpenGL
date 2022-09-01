cmake_minimum_required(VERSION 3.23)

set(CMAKE_CXX_STANDARD 17)

project(tutorial02_red_triangle)

add_executable(${PROJECT_NAME} main.cpp)

find_package(OpenGL REQUIRED)

target_link_libraries(${PROJECT_NAME}
        glad::glad
        imgui
        glfw
        OpenGL::GL
        glm::glm
        common
        )
if(APPLE)
    target_link_libraries(${PROJECT_NAME}
            "-framework Cocoa"
            "-framework IOKit"
            "-framework CoreVideo"
            )
endif(APPLE)
