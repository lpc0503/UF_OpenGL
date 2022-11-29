# Project 2

# OpenGL

## Important

I run my *same* code on two different enviroment and it comes out extremely different action.
So if the behavior is not correct on your device, uncomment line 445 in file `pi_source.cpp`, maybe it will be correct XD.

## Intro

OS : Mac OS

I write several class to maintain my code, so there is a bunch of file in `misc05_picking` and some change in `CMakeList.txt`

## Setting

Replace the whole `misc05_picking` file with uploaded `misc05_picking`, or copy **ALL** things inside to `misc05_picking`.
Replace the `CMakeLists.txt`

I use `tinyobjloader` for loading `.obj` files, this library **is mention on the website** so I use it.
Replace the `external` file or copy `tinyobjloader` to `external/`

Shaders are in the misc05_picking file, so if replace the whole file, do not need to replace the shader again.

## App Usage

For the project, it ask us to use key to move the camara. I did implement this, I also implement mouse event,
you can use mouse to move the camara with pressing right mouse and move. Also, you can use mouse scroll to scroll the screen.

### File explanation

* misc05_picking
    * Mesh
        * Store the mesh data.
    * Model
        * Load the `.obj` file and store the data to Mesh.
    * Renderer/RendererAPI
        * Wrapped OpenGL state and method so that help me easier to render objects.
    * Vertex
        * Vertex data structer
    * p1_cource
        * Main function
