# Project 1 B

# OpenGL

## Intro

OS : Mac OS

I built my own environment and write my own `CMakeLists.txt`.

I uploaded my whole project include every library that I use.

## Setting

Please set the working directory to `OpenGLPlayground/`.

If can't set the working direcotry, after building the project, copy the file `asset` to the directory that executable file exist.

for example:
`OpenGLPlayground/build/misc05_picking/asset`.

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

`./misc05_picking/p1`.

## Formulas

### Task 2

$$
c_{i,0} = \begin{case}
P_0 &quad \text{When i=0} \\
\dfrac{c_{i,1}+c_{i-1, 2}}} &quad \text{When i>0}
\end{cases}
$$

