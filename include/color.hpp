#ifndef COLOR_H
#define COLOR_H

#include "common.hpp"
#include "vec3.hpp"
#include "interval.hpp"

using Color = Vec3;

inline double linear_To_Gamma(double linear_component) {
    if (linear_component > 0) {
        return sqrt(linear_component);
    }
    return 0;
}

void write_Color(std::ostream& out, const Color& pixel_Color) {
    auto r = pixel_Color.x();
    auto g = pixel_Color.y();
    auto b = pixel_Color.z();

    // Apply a linear to gamm transform for gamma 2
    r = linear_To_Gamma(r);
    g = linear_To_Gamma(g);
    b = linear_To_Gamma(b);

    // Translate the [0,1] component values to the byte range [0,255]
    static const Interval intensity(0.000, 0.999);
    int rbyte = int(256 * intensity.clamp(r));
    int gbyte = int(256 * intensity.clamp(g));
    int bbyte = int(256 * intensity.clamp(b));

    // Write out the pixel color components
    out << rbyte << " " << gbyte << " " << bbyte << "\n";
}

#endif