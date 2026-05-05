# Core3D

Core3D is a lightweight 3D renderer written in C using SDL2.  
It loads 3D models in OBJ format, applies transformations, and renders them in real time using a simple projection pipeline.

## Features

- OBJ file parsing (vertices and triangular faces)
- Multiple models support
- Basic transformation system (4x4 matrices)
- Perspective projection
- Real-time rendering using SDL2
- Wireframe visualization
- Export of transformed models back to OBJ

## Technologies

- C
- SDL2
- Custom dynamic arrays (vectors)
- Basic linear algebra (matrix transformations)

## Project Structure
├── main.c
├── Makefile
└── README.md

## Build

Make sure you have SDL2 installed. Then run Makefile.

## Usage example

./Core3D input.obj output.obj


### Arguments

* `input.obj` — path to input OBJ file
* `output.obj` (optional) — file to save transformed model

If output file is not specified, the program writes to:

```
a.obj
```

## Controls

* Close window to exit the program

## Rendering Pipeline

1. Load models from OBJ file
2. Store vertices and polygons
3. Apply transformation matrices
4. Perform perspective projection
5. Draw edges using SDL2

## Limitations

* Only triangular faces are supported
* No lighting or shading (wireframe only)
* No depth buffer (simple visibility)
* Limited OBJ format support (no textures, normals partially ignored)

## Example

```bash
./Core3D cube.obj result.obj
```

## Future Improvements

* Z-buffer implementation
* Camera movement and controls
* Lighting and shading (Phong, Gouraud)
* Support for textures and normals
* Scene graph system

