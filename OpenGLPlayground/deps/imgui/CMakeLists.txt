cmake_minimum_required(VERSION 3.10)

project(imgui)

add_library(imgui STATIC
    imconfig.h
    imgui_tables.cpp
    imgui_demo.cpp
    imgui_draw.cpp
    imgui_impl_glfw.cpp
    imgui_impl_glfw.h
    imgui_impl_opengl3.cpp
    imgui_impl_opengl3.h
    imgui_internal.h
    imgui_widgets.cpp
    imgui.cpp
    imgui.h
    imstb_rectpack.h
    imstb_textedit.h
    imstb_truetype.h
    imgui_impl_opengl3_loader.h
)
target_include_directories(imgui PUBLIC .)
target_link_libraries(imgui glfw)
add_library(imgui::imgui ALIAS imgui)

