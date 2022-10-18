cmake_minimum_required(VERSION 3.23)

set(CMAKE_CXX_STANDARD 17)

project(project02)

add_executable(${PROJECT_NAME} main.cpp Renderer.cpp Renderer.h)

find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME}
    glad::glad
    imgui
    glfw
    OpenGL::GL
    glm::glm
    common::common
)

if(APPLE)
    target_link_libraries(${PROJECT_NAME}
            "-framework Cocoa"
            "-framework IOKit"
            "-framework CoreVideo"
            )
endif(APPLE)


