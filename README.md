[![Run on Repl.it](https://repl.it/badge/github/calvang/ray-casting-demo)](https://repl.it/github/calvang/ray-casting-demo)

# Ray Casting Demo
  
A simple maze demo utilizing ray casting. The desktop application uses OpenGL and C++ while the web application utilizes WebGL and Typescript. This was inspired by 
3DSage's video on a simple ray casting algorithm and was written to experiment with different game mechanics with ray casting.

## Controls

The direction keys are mapped to the WASD format (A and D are for turning), and `L` is held to sprint.

## Run

### C++

To build from the source code, run:
 `cd c++ && make maze; ./maze`
Or simple run the executable:
`./maze`

Similarly, `sample.cpp` and `sampleLarge.cpp` can be built or run for testing.
`sample.cpp` is a generic template, and `sampleLarge` is a scalable and
customizable reimplementation.
