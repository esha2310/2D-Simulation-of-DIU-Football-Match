# 2D Simulation of DIU Football Match

An academic Computer Graphics project developed in **C++ with OpenGL/GLUT**.  
The simulation represents a DIU football-match environment with animated players, spectators,
moving buses and clouds, football interactions, geometric transformations, and interactive
day/night scene transitions.

## Key Features

- Custom **DDA Line Drawing Algorithm**
- Custom **Midpoint Circle Algorithm**
- OpenGL geometric transformations:
  - Translation
  - Rotation
  - Scaling
- Animated football players and ball movement
- Goal detection and spectator celebration
- Player transition logic
- Moving university students
- Animated buses with rotating wheels
- Moving clouds
- Interactive **day/night transition**
- Dynamic lighting/overlay effects
- DIU campus-inspired graphical environment

## Technologies

- C++
- OpenGL
- GLUT / FreeGLUT
- Computer Graphics Algorithms
- Geometric Transformations
- Animation Logic

## Source Code

The original project source supplied for this repository is kept in:

```text
src/main.cpp
```

## Controls

- `N` / `n` — switch toward night mode
- `D` / `d` — switch toward day mode
- `Space` — toggle day/night
- The on-screen **Make Day / Make Night** button can also be clicked.

## Build on Windows

### Option 1: MinGW + FreeGLUT

Install a C++ compiler and FreeGLUT, then compile with a command similar to:

```bash
g++ src/main.cpp -o diu_football_match.exe -lfreeglut -lopengl32 -lglu32
```

The exact include/library paths can vary depending on the FreeGLUT installation.

### Option 2: Code::Blocks / Visual Studio

1. Create a C++ project.
2. Add `src/main.cpp`.
3. Configure OpenGL and GLUT/FreeGLUT include/library paths.
4. Link the required OpenGL and GLUT libraries.
5. Build and run.

## Project Structure

```text
2D-Simulation-of-DIU-Football-Match/
├── src/
│   └── main.cpp
├── CMakeLists.txt
├── README.md
└── .gitignore
```

## Suggested CV Entry

**2D Simulation of DIU Football Match** [Code]  
Academic C++/OpenGL project using DDA and Midpoint Circle algorithms, geometric transformations,
animation logic, and interactive day-night scene transitions.

## Academic Context

This repository is intended for academic portfolio and demonstration purposes.
The code in `src/main.cpp` is based on the source file supplied for the original project.
