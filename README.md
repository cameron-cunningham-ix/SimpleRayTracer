Ray Tracer - "Ray Tracing in One Weekend" with SDL

This project is an implementation of the ray tracer described in Ray Tracing in One Weekend by Peter Shirley. The ray tracer renders an image by simulating the behavior of light rays in a 3D scene and is displayed in real-time through an SDL2 window as it is progressively generated.

Features

    Ray tracing algorithm based on Ray Tracing in One Weekend
    Real-time rendering using SDL2 to display the image as it is created
    Support for spheres, camera, and simple materials like lambertian, metal, and dielectric
    Multithreaded rendering for performance improvements
    CMake build system for cross-platform development

Getting Started
Prerequisites

Make sure you have the following installed:

    C++17 compatible compiler
    CMake

Building the Project

Clone the repository:

    git clone https://github.com/cameron-cunningham-ix/SimpleRayTracer.git
    cd SimpleRayTracer

Create a build directory and navigate into it:

    mkdir build
    cd build

Run CMake to generate the build files:

    cmake ..

Build the project:

    cmake --build .

Running the Ray Tracer

After building the project, you can run the ray tracer from the build directory:

    Debug/SimpleRayTracer.exe

Once launched, the SDL window will open and start displaying the image as it’s progressively rendered.

Project Structure

    src/ - Contains the source files for the ray tracer
    include/ - Header files for the ray tracer
    CMakeLists.txt - CMake build configuration
    third_party/ - External libraries and dependencies (SDL2)

Future Improvements

    Add support for more complex geometric shapes
    Implement texture mapping and lighting models
    Optimize ray tracing performance with acceleration structures (e.g., BVH)
    Expand material support (e.g., emissive surfaces)

Acknowledgments

    Peter Shirley for his excellent Ray Tracing in One Weekend series
    The SDL team for the cross-platform development library
