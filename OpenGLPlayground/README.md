# Project 0

# Intro

OS : Mac OS

I built my own enviroment and wirte my own `CMakeLists.txt`, because the GUI library this tutorial use can't run properly on my device, so I change the GUI library.

I uploaded my whole project include every library that I use.

## Setting

Please set the working directory to `OpenGLPlayground/`.

If can't set the working direcotry, after building the project, copy the file `asset` to the directory that executable file exist.

for example:
`OpenGLPlayground/build/tutorial02_red_triangle/asset`.

## Build

Set the working ditectory to `OpenGLPlayground/` and build the project in your IDE.

OR

Open the terminal, `cd` to `OpenGLPlayground/`.

Under `OpenGLPlayground/`

```
mkdir build && cd build
cmake ..
make -j4
```

## Exececute

Run the project in your IDE directly

OR

In the terminal.

Under `OpenGLPlayground/build`.

`./tutorialfile/executable`.

ex:

`./misc05_picking.p1`.