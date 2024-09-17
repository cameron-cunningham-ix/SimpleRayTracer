# Minimum CMake version
cmake_minimum_required(VERSION 3.12)

# Project name and version
project(SimpleRayTracer VERSION 0.2)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add source files (update this list with your actual files)
set(SOURCES
    src/main.cpp
)

# Include directories (update this list with your actual include paths)
include_directories(include)

# Prevent SDL2 from installing globally
set(SDL2_DISABLE_INSTALL ON)

# Add SDL2 as a subdirectory (point this to where SDL2 CMakeLists.txt is located)
add_subdirectory(third_party/SDL2)

# Include SDL2 headers
include_directories(third_party/SDL2/include)

# Create the executable
add_executable(SimpleRayTracer ${SOURCES})

# Link SDL2 to your project
target_link_libraries(SimpleRayTracer SDL2 SDL2main)
