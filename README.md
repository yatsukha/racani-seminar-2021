# 3D Fractals using Ray Marching

![mandelbulb dynamic](videos/mandelbulb_dynamic.gif)

## Examples

See [videos](videos) subfolder for more examples.

## Usage

### Building

Program dependencies:
  * `OpenGL`
  * `GLM`
  * `GLFW`
  * `dl`, dynamic loader, likely provided by your compiler

Build tool dependencies:
  * `meson`
  * `ninja`
  * `gcc` with support for `-std=c++17` (or `clang`, untested)


```
$ meson -Dbuildtype=release release_build
$ cd release_build
$ ninja
```

This will output a `main.out` executable. To run, provide a shader from the [data/shaders](data/shaders) subfolder. Follow the instructions output by the executable to adjust various settings and the camera.

```
./main.out ../data/shaders/mandelbulb.glsl
```
