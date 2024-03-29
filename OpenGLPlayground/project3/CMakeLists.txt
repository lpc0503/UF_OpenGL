cmake_minimum_required(VERSION 3.23)

set(CMAKE_CXX_STANDARD 17)

project(project03)

add_executable(${PROJECT_NAME}
        main.cpp
        Renderer.cpp
        Renderer.h
        Camera.cpp
        Camera.h
        Log.cpp
        Log.h
        OpenGLRenderAPI.cpp
        OpenGLRenderAPI.h
        Core.h
        Utils.cpp
        Utils.h
        Mesh.cpp
        Mesh.h
        Model.cpp
        Model.h
        Vertex.h
        debugbreak.h
        Entity.cpp
        Entity.h
        PNTriangle.cpp
        PNTriangle.h
        Texture.cpp
        Texture.h
        Application.h
        Application.cpp
        OpenGLApplication.h
        OpenGLApplication.cpp
        RenderAPI.cpp)

find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME}
    glad::glad
    imgui
    glfw
    OpenGL::GL
    fmt::fmt
    glm::glm
    tinyobjloader
    stb::image
    common::common
)

if(APPLE)
    target_link_libraries(${PROJECT_NAME}
            "-framework Cocoa"
            "-framework IOKit"
            "-framework CoreVideo"
            )
endif(APPLE)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:preprocessor")
endif()
