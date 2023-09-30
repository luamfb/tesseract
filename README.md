A simple 4D-hypercube (tesseract) visualizer using OpenGL.

## Building

One must first install the system-wide requisites:
    - [OpenGL](https://www.opengl.org/) headers
    - [GLFW](https://www.glfw.org/)

One must also install [GLAD](https://glad.dav1d.de/) and
[GLM](https://github.com/g-truc/glm) into the `glad` and `glm`
directories, respectively. Both are expected to be located in the
project root.

Then, simply run `make`.

## Running

The generated program will be called `bin`, and must be run without
arguments, from the project root (or else it will not find its shaders).
