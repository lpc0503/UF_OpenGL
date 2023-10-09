cmake_minimum_required(VERSION 3.23)

project(stb_image)

add_library(${PROJECT_NAME} STATIC stb_image.cpp)
add_library(stb::image ALIAS stb_image)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})
