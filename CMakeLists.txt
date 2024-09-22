# Minimum CMake version
cmake_minimum_required(VERSION 3.12)

# Project name and version
project(SimpleRayTracer VERSION 0.2)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add source files
set(SOURCES
    src/main.cpp
)

# Prevent SDL2 from installing globally
set(SDL2_DISABLE_INSTALL ON)

# Add subdirectories
add_subdirectory(third_party/SDL2 third_party/stb_image)

# Include headers
include_directories(include third_party/SDL2/include third_party/stb_image)

link_directories(${PROJECT_SOURCE_DIR}/third_party/SDL2/build)

# Create the executable
add_executable(SimpleRayTracer ${SOURCES})


target_link_libraries(SimpleRayTracer SDL2d SDL2maind)

# Copy SDL2d.dll to the output directory (Debug)
add_custom_command(TARGET SimpleRayTracer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${PROJECT_SOURCE_DIR}/third_party/SDL2/build/Debug/SDL2d.dll
    $<TARGET_FILE_DIR:SimpleRayTracer>)




