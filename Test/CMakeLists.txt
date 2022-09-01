cmake_minimum_required(VERSION 3.23)
project(Test)
set(CMAKE_CXX_STANDARD 17)

set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/imgui")

add_subdirectory(./deps/glad)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/imgui
)


find_package(OpenGL REQUIRED)
#add_subdirectory(./deps/glew-2.1.0)
add_subdirectory(./deps/glfw-3.3.8)

add_executable(Test
        main.cpp
        head.h
		imgui/imconfig.h
		imgui/imgui_tables.cpp
		imgui/imgui_demo.cpp
		imgui/imgui_draw.cpp
		imgui/imgui_impl_glfw.cpp
		imgui/imgui_impl_glfw.h
		imgui/imgui_impl_opengl3.cpp
		imgui/imgui_impl_opengl3.h
		imgui/imgui_internal.h
		imgui/imgui_widgets.cpp
		imgui/imgui.cpp
		imgui/imgui.h
		imgui/imstb_rectpack.h
		imgui/imstb_textedit.h
		imgui/imstb_truetype.h
		imgui/imgui_impl_opengl3_loader.h
        )

target_link_libraries(Test glfw)
target_link_libraries(Test OpenGL::GL)
#target_link_libraries(Test GLEW::GLEW)
target_link_libraries(Test glad::glad)
target_link_libraries(Test
    "-framework Cocoa"
    "-framework IOKit"
    "-framework CoreVideo"
    )