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
c_{i, 0}=
\begin{cases}
0 & \quad \text{When i=0}\\ 
\dfrac{c_{i, 1} + c_{i-1, 2}}{2} & \quad \text{When i>0}
\end{cases}
$$

$$
c_{i,3} = c_{i+1,0} = P_i
$$

### Task 3

$$
c_{i, 1}=
\begin{cases}
c_{i, 0} + \dfrac{P_{i+1} - P_{i-1}}{6} & \quad \text{Where $c_{i, 0} = P_i$}
\end{cases}
$$

$$
c_{i, 2}=
\begin{cases}
c_{i, 3} + \dfrac{P_{i} - P_{i+2}}{6} & \quad \text{Where $c_{i, 3} = c_{i+1, 0} = P_{i+1}$}
\end{cases}
$$

### File explanation

* asset
    * shader
        * All shader
* common
    * All wrapped render utils
* deps
    * All library
* misc05_picking
    * BSplineCurve
        * BSplineCurve object, calculate the subdivide point
    * BezierCurve
        * BezierCurve object, calculate the Bezier point
    * CatmullRomCurve
        * Catmull object, calculate the CatmullRomCurve using the de casteljau's algorithm.
    * Geometry
        * Point and Vertice declaration
    * p1_cource
        * Main function
