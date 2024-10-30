cmake_minimum_required(VERSION 3.23)

set(CMAKE_CXX_STANDARD 17)

project(Water)

set(ENGINE_SRC_FILES
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
        Texture.cpp
        Texture.h
        Application.h
        Application.cpp
        OpenGLApplication.h
        OpenGLApplication.cpp
        RenderAPI.cpp
        RenderAPI.h
        Water.cpp
        Water.h)
source_group(Engine FILES ${ENGINE_SRC_FILES})

set(APP_SRC_FILES
        Playground.cpp Playground.h)
source_group(App FILES ${APP_SRC_FILES})

add_executable(${PROJECT_NAME} ${ENGINE_SRC_FILES} ${APP_SRC_FILES})

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

    target_compile_definitions(${PROJECT_NAME} PUBLIC _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING)

    #set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
    set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")
    set_property( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Water )
endif()
