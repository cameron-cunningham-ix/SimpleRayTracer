#include <string>
#include <iostream>

#include "color.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "..\third_party\stb_image\stb_image.h"

class EnvironmentMap{
public:
    int width;              // EnvMap image width
    int height;             // EnvMap image height
    int channels;           // Number of channels (R,G,B, +- A)
    unsigned char* data;    // Image data

    // Constructor, load in the image file as the environment map
    // File should be .jpg format
    EnvironmentMap(const std::string& filename) {
        data = stbi_load(filename.c_str(), &width, &height, &channels, 0);
        if (!data){
            std::cout << "Failed to load environment map: " << filename << std::endl;
        }
    }

    // Deconstructor
    ~EnvironmentMap() {
        stbi_image_free(data);
    }

    // Sample the environment map given texture coordinates (u, v)
    Color sample(double u, double v) const {
        if (!data) {
            std::cout << "black\n";
            return Color(0, 0, 0);  // Return black if no image is loaded
        }

        // Map (u, v) to pixel coordinates
        int i = static_cast<int>(u * width);
        int j = static_cast<int>(v * height);

        // Clamp coordinates to image bounds
        if (i < 0) { i = 0; }
        if (i >= width) { i = width - 1; }
        if (j < 0) { j = 0; }
        if (j >= height) { j = height - 1; }

        // Calculate pixel color of image given index
        int index = (i + width*j) * channels;
        double r = data[index] / 255.999;
        double g = data[index + 1] / 255.999;
        double b = data[index + 2] / 255.999;

        return Color(r, g, b);
    }
};
