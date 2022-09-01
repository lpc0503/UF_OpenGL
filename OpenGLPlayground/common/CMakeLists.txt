
cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)

project(common)

add_library(${PROJECT_NAME}
        controls.cpp
        controls.hpp
        objloader.cpp
        objloader.hpp
        quaternion_utils.cpp
        quaternion_utils.hpp
        shader.cpp
        shader.hpp
        tangentspace.cpp
        tangentspace.hpp
        text2D.cpp
        text2D.hpp
        texture.cpp
        texture.hpp
        vboindexer.cpp
        vboindexer.hpp
        )

add_library(common::common ALIAS common)
#target_link_libraries(glad::glad)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/)
