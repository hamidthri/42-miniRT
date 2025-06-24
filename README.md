# miniRT

A 3D ray tracer implementation in C using miniLibX that generates photorealistic images from mathematical scene descriptions. This project demonstrates the fundamentals of ray tracing algorithms, 3D mathematics, and computer graphics programming.

## Overview

Ray tracing is a rendering technique that simulates the physical behavior of light to generate realistic images. Unlike rasterization (used in most real-time graphics), ray tracing traces the path of light rays backwards from the camera through each pixel to determine what the camera sees.

This implementation focuses on the core concepts:
- Ray-object intersection calculations
- Lighting models (Phong shading)
- 3D transformations and coordinate systems
- Scene parsing and memory management

## Features

### Geometric Objects
- **Spheres**: Perfect spherical objects with configurable diameter
- **Planes**: Infinite flat surfaces defined by a point and normal vector
- **Cylinders**: Finite cylindrical objects with configurable diameter and height

### Lighting System
- **Ambient Lighting**: Global illumination ensuring objects are never completely dark
- **Diffuse Lighting**: Directional lighting with realistic light falloff
- **Hard Shadows**: Sharp shadow casting from light sources
- **Multiple Light Sources**: Support for various positioned lights

### Camera System
- **Configurable Viewpoint**: Position camera anywhere in 3D space
- **Orientation Control**: Point camera in any direction using normalized vectors
- **Field of View**: Adjustable horizontal FOV (0-180 degrees)
- **Perspective Projection**: Realistic depth perception

### Scene Management
- **Custom File Format**: Human-readable `.rt` scene description files
- **Flexible Parsing**: Elements can appear in any order with flexible spacing
- **Error Handling**: Comprehensive validation with descriptive error messages
- **Object Transformations**: Translation and rotation support (except spheres)

## Installation & Usage

### Building the Project
```bash
# Clone and build
make

# Clean build files
make clean      # Remove object files
make fclean     # Remove object files and executable
make re         # Rebuild everything
```

### Running the Ray Tracer
```bash
./miniRT scenes/example.rt
```

The program will open a window displaying the rendered scene. Use ESC or the window close button to exit.

## Scene File Format

Scenes are defined using `.rt` files with a simple, human-readable format. Each line represents a scene element:

### Required Elements (One Each)
```bash
# Ambient Lighting
A 0.2 255,255,255
# A [ambient_ratio] [R,G,B_colors]

# Camera
C -50,0,20 0,0,1 70
# C [x,y,z_position] [x,y,z_orientation] [field_of_view]

# Light Source
L -40,0,30 0.7 255,255,255
# L [x,y,z_position] [brightness_ratio] [R,G,B_colors]
```

### Geometric Objects (Multiple Allowed)
```bash
# Plane
pl 0,0,0 0,1,0 255,0,225
# pl [x,y,z_point] [x,y,z_normal] [R,G,B_colors]

# Sphere
sp 0,0,20 20 255,0,0
# sp [x,y,z_center] [diameter] [R,G,B_colors]

# Cylinder
cy 50,0,20.6 0,0,1 14.2 21.42 10,0,255
# cy [x,y,z_center] [x,y,z_axis] [diameter] [height] [R,G,B_colors]
```

### Parameter Ranges
- **Coordinates**: Any real number
- **Ratios**: 0.0 to 1.0
- **Colors**: 0 to 255 (RGB)
- **Vectors**: -1.0 to 1.0 (normalized)
- **Field of View**: 0 to 180 degrees
- **Dimensions**: Positive real numbers

## Example Scene

```bash
# Basic scene with all elements
A 0.2 255,255,255
C -50,0,20 0,0,1 70
L -40,0,30 0.7 255,255,255

# Ground plane
pl 0,0,0 0,1,0 100,100,100

# Red sphere
sp 0,0,20 20 255,0,0

# Blue cylinder
cy 50,0,20.6 0,0,1 14.2 21.42 10,0,255
```

## Controls & Interface

- **ESC Key**: Exit program cleanly
- **Window Close Button**: Exit program cleanly
- **Window Management**: Supports minimization, focus switching, etc.

## Sample Outputs



## Technical Implementation

### Ray Tracing Algorithm
1. **Ray Generation**: Cast rays from camera through each pixel
2. **Intersection Testing**: Calculate ray-object intersections
3. **Closest Hit**: Determine nearest intersection point
4. **Lighting Calculation**: Apply Phong shading model
5. **Color Computation**: Combine ambient, diffuse, and shadow effects

### Mathematical Foundations
- **Vector Operations**: Dot product, cross product, normalization
- **Ray-Sphere Intersection**: Quadratic equation solving
- **Ray-Plane Intersection**: Line-plane intersection formula
- **Ray-Cylinder Intersection**: Complex geometric calculations
- **Coordinate Transformations**: 3D rotation and translation matrices

