#ifndef RAY_H
#define RAY_H

#include "vec3.hpp"

class Ray {
public:
    // Default constructor, uninitialized origin and dir
    Ray() {}

    // Constructor, initializes ray with provided origin and direction
    Ray(const Point3& origin, const Vec3& direction) : orig(origin), dir(direction) {}

    // Getters for origin and direction of ray
    const Point3& origin() const { return orig; }
    const Vec3& direction() const { return dir; }
    
    // Returns the 3D point along the ray at "distance" t
    Point3 at(double t) const {
        return orig + t*dir;
    }

private:
    Point3 orig;
    Vec3 dir;
};

#endif