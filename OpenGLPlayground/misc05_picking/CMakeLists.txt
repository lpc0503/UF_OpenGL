cmake_minimum_required(VERSION 3.23)

set(CMAKE_CXX_STANDARD 17)

project(misc05_picking)

add_executable(p1
        p1_source.cpp
        BezierCurve.cpp
        BezierCurve.h
        Geometry.h
        CatmullRomCurve.cpp
        CatmullRomCurve.h
        BSplineCurve.cpp BSplineCurve.h)

find_package(OpenGL REQUIRED)

target_link_libraries(p1
        glad::glad
        imgui
        glfw
        OpenGL::GL
        glm::glm
        common::common
        )

if(APPLE)
    target_link_libraries(p1
            "-framework Cocoa"
            "-framework IOKit"
            "-framework CoreVideo"
            )
endif(APPLE)


