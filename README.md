# OpenGL 3D Genome Visualization Project

## Overview
This repository contains the source code for a 3D genome visualization tool developed using OpenGL. The project focuses on rendering Hi-C data, which captures genome-wide chromatin interactions, in a three-dimensional space. This approach enhances the understanding of genomic structures, providing a more comprehensive view compared to traditional 2D representations.

## Features
- **3D Visualization**: Transform Hi-C data into intuitive and interactive 3D models.
- **Interactivity**: Zoom, rotate, and navigate through the 3D genomic structures.
- **Advanced Rendering**: Implement phong shading for realistic lighting and texture effects.
- **Instanced Rendering**: Efficiently render multiple instances of genomic data points.
- **Cross-Platform Compatibility**: Developed to be compatible with various operating systems.

## Installation

### Prerequisites
- [OpenGL](https://www.opengl.org/)
- [GLFW](https://www.glfw.org/)
- [GLEW](http://glew.sourceforge.net/)
- [GLM](https://glm.g-truc.net/)
- C++ Compiler (GCC or Visual Studio)

### Setup
1. Clone the repository:
   ```
   git clone https://github.com/your-username/OpenGL-3D-Genome-Visualization.git
   ```
2. Navigate to the cloned directory:
   ```
   cd OpenGL-3D-Genome-Visualization
   ```
3. Compile the source code (Example for GCC):
   ```
   g++ -o genome_visualizer main.cpp -lglfw -lGLEW -lGL
   ```

## Usage
Run the compiled executable to start the visualization tool. Use mouse and keyboard inputs to interact with the 3D model:
- **Zoom**: Scroll up/down to zoom in/out.
- **Rotate**: Click and drag to rotate the model.
- **Navigate**: Use arrow keys to move around the model.

## Contributing
Contributions to this project are welcome. Please follow these steps:
1. Fork the repository.
2. Create a new branch (`git checkout -b feature-branch`).
3. Make your changes and commit (`git commit -am 'Add some feature'`).
4. Push to the branch (`git push origin feature-branch`).
5. Create a new Pull Request.


## Acknowledgments
- Hi-C data processing scripts adapted from [GrapHi-C](https://github.com/kimmackay/GrapHi-C).
- Sphere model created using Blender.

## Contact
For any inquiries or issues, please open an issue on the repository or contact the project maintainer at `bqt031@usask.ca`.
