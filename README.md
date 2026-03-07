# OpenGL Airport

A 3D airport simulation and rendering project built with **C++** and **OpenGL**, showcasing real-time graphics, interactive camera control, and detailed airport modeling.

---

## Table of Contents

- [Project Overview](#project-overview)
- [Features](#features)
- [Technical Details](#technical-details)
- [Design & Architecture](#design--architecture)
- [Tech Stack](#tech-stack)
- [Setup and Installation](#setup-and-installation)
- [Running the Project](#running-the-project)
- [Learning Goals](#learning-goals)
- [Key Features](#key-features)

---

## Project Overview

This project visualizes a 3D airport environment including runways, terminals, and airplanes. It allows **interactive camera control**, real-time rendering, and demonstrates key concepts in **computer graphics and OpenGL**. The simulation is designed for learning graphics programming and portfolio showcase.

---

## Features

- Real-time 3D rendering of an airport scene
- Interactive camera controls (orbit, pan, zoom)
- Lighting and shading effects for realism
- Airport environment including runways, planes, and terminal buildings
- Efficient rendering using modern OpenGL techniques
- Shader-based visualization for detailed graphics

---

## Technical Details

- **Rendering:** OpenGL 3.3+
- **Shaders:** GLSL (vertex & fragment shaders)
- **Math Library:** GLM for vector and matrix operations
- **Windowing & Input:** GLFW
- **Models & Textures:** OBJ/PNG assets for airport objects
- **Animation:** Simple object movements (planes, lights)

---

## Design & Architecture

- **Modular structure:** Separate classes for Camera, Scene, and Renderer
- **Shader management:** Centralized shader compilation and use
- **Camera abstraction:** Orbit and fly-style camera controls
- **Scene graph:** Optional organization of airport objects for efficient rendering



---

## Tech Stack

| Component        | Technology |
| ---------------- | ---------- |
| Language         | C++        |
| Graphics API     | OpenGL     |
| Windowing        | GLFW       |
| Math Library     | GLM        |
| Shaders          | GLSL       |

---

## Setup and Installation

Clone the repository:

```bash
git clone https://github.com/GeorgiEmanuel/OpenGL-Airport.git
cd OpenGL-Airport
```
Build the project
```bash
mkdir build
cd build
cmake ..
make
```

## Running the Project
```bash
./OpenGL-Airport
```
Use keyboard and mouse to navigate the 3D airport scene

## Key Features 
3D Airport Scene: Runways, terminals, and planes

Interactive Camera: Orbit, zoom, and pan through the environment

Lighting & Shading: Realistic scene illumination

Animated Objects: Moving planes and dynamic elements

<img width="1004" height="539" alt="image" src="https://github.com/user-attachments/assets/2d38b0e3-87d4-4170-a263-a28c97808455" />
<img width="1005" height="635" alt="image" src="https://github.com/user-attachments/assets/21456c99-f0e8-4a3e-b087-b0f0a064e809" />


## Learning Goals
* Learn modern OpenGL 3D rendering and shader programming
* Understand camera and projection transformations
* Implement lighting, shading, and textures in OpenGL
* Apply modular C++ design for graphics applications
