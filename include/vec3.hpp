#ifndef VEC3_H
#define VEC3_H

#include "common.hpp"

#include <cmath>
#include <iostream>

using std::sqrt;

class Vec3 {
public:
    // Array for X, Y, Z elements
    double e[3];

    // Default constructor, initializes X, Y, Z to 0
    Vec3() : e{0,0,0} {}
    // Constructor, initializes the vector with provided X, Y, Z values
    Vec3(double e0, double e1, double e2) : e{e0,e1,e2} {}

    // Getters for X, Y, Z components
    double x() const {return e[0];}
    double y() const {return e[1];}
    double z() const {return e[2];}

    // Negates the vector
    Vec3 operator-() const {return Vec3(-e[0], -e[1], -e[2]);}
    
    // Indexing operator (read-only)
    double operator[](int i) const { return e[i]; }
    
    // Indexing operator (read-write)
    double& operator[](int i) { return e[i]; }

    // Adds vector `v` to the current vector
    Vec3& operator+=(const Vec3& v) {
        e[0] += v.e[0];
        e[1] += v.e[1];
        e[2] += v.e[2];
        return *this;
    }

    // Multiplies the current vector by a scalar `t`
    Vec3& operator*=(double t) {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;
        return *this;
    }

    // Divides the current vector by a scalar `t`
    Vec3& operator/=(double t) {
        return *this *= 1/t;
    }

    // Returns the length (magnitude) of the vector
    double length() const {
        return sqrt(length_Squared());
    }

    // Returns the squared length of the vector
    double length_Squared() const {
        return e[0]*e[0] + e[1]*e[1] + e[2]*e[2];
    }

    // Returns true if the vector is approximately zero in all dimensions
    bool near_Zero() const {
        double s = 1e-8;
        return (fabs(e[0]) < s) && (fabs(e[1]) < s) && (fabs(e[2]) < s);
    }

    // Returns a random Vec3 with X, Y, Z components between [0, 1)
    static Vec3 random() {
        return Vec3(random_double(), random_double(), random_double());
    }

    // Returns a random Vec3 with X, Y, Z components between [min, max)
    static Vec3 random(double min, double max) {
        return Vec3(random_double(min,max), random_double(min,max), random_double(min,max));
    }
};

// point3 is just an alias for Vec3, useful for clarity when representing points
using Point3 = Vec3;

// Vector Utility Functions

// Output a Vec3 in the format "x y z"
inline std::ostream& operator<<(std::ostream& out, const Vec3& v) {
    return out << v.e[0] << " " << v.e[1] << " " << v.e[2];
}

// Adds two vectors `v` and `u`
inline Vec3 operator+(const Vec3& v, const Vec3& u) {
    return Vec3(v.e[0] + u.e[0], v.e[1] + u.e[1], v.e[2] + u.e[2]);
}

// Subtracts vector `u` from vector `v`
inline Vec3 operator-(const Vec3& v, const Vec3& u) {
    return Vec3(v.e[0] - u.e[0], v.e[1] - u.e[1], v.e[2] - u.e[2]);
}

// Element-wise multiplication of vectors `v` and `u`
inline Vec3 operator*(const Vec3& v, const Vec3& u) {
    return Vec3(v.e[0] * u.e[0], v.e[1] * u.e[1], v.e[2] * u.e[2]);
}

// Multiplies a vector `v` by a scalar `t`
inline Vec3 operator*(double t, const Vec3& v) {
    return Vec3(t*v.e[0], t*v.e[1], t*v.e[2]);
}
inline Vec3 operator*(const Vec3& v, double t) {
    return t * v;
}

// Divides a vector `v` by a scalar `t`
inline Vec3 operator/(const Vec3& v, double t) {
    return (1/t) * v;
}

// Returns the dot product of vectors `v` and `u`
inline double dot(const Vec3& v, const Vec3& u) {
    return u.e[0] * v.e[0]
        + u.e[1] * v.e[1]
        + u.e[2] * v.e[2];
}

// Returns the cross product of vectors `v` and `u`
inline Vec3 cross(const Vec3& v, const Vec3& u) {
    return Vec3(v.e[1] * u.e[2] - v.e[2] * u.e[1],
                v.e[2] * u.e[0] - v.e[0] * u.e[2],
                v.e[0] * u.e[1] - v.e[1] * u.e[0]);
}

// Returns the unit vector of vector `v`
inline Vec3 unit_Vector(const Vec3& v) {
    return v / v.length();
}

// Returns a random Vec3 inside a unit sphere (radius = 1)
inline Vec3 random_In_Unit_Sphere() {
    while (true) {
        Vec3 p = Vec3::random(-1,1);
        if (p.length_Squared() < 1) {
            return p;
        }
    }
}

// Returns a random Vec3 in the X-Y plane inside a unit disk
inline Vec3 random_In_Unit_Disk() {
    while (true) {
        Vec3 p = Vec3(random_double(-1,1), random_double(-1,1), 0);
        if (p.length_Squared() < 1) {
            return p;
        }
    }
}

// Returns a random unit vector on the surface of a unit sphere
inline Vec3 random_Unit_Vector() {
    return unit_Vector(random_In_Unit_Sphere());
}

// Returns a random unit vector within the hemisphere defined by a normal vector
inline Vec3 random_On_Hemisphere(const Vec3& normal) {
    Vec3 on_unit_sphere = random_Unit_Vector();
    if (dot(on_unit_sphere, normal) > 0.0) // in the same hemisphere as the normal
        return on_unit_sphere;
    else
        return -on_unit_sphere;
}

// Reflects vector `v` about a normal
// Reflection formula: v' = v - 2 * dot(v, normal) * normal
// Commonly used in reflection models for lighting.
inline Vec3 reflect(const Vec3& v, const Vec3& normal) {
    return v - 2*dot(v, normal)*normal;
}

// Refracts vector `uv` through a surface with normal `n` and ratio of refractive indices `etai_over_etat`
// Snell's law is used to calculate the direction of refraction.
// Perpendicular component: refracts as a proportion of `etai_over_etat`.
// Parallel component: adjusted to ensure the magnitude of the resulting vector remains correct.
inline Vec3 refract(const Vec3& uv, const Vec3& n, double etai_over_etat) {
    double cos_theta = fmin(dot(-uv, n), 1.0);  // Compute cos(theta)
    Vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);  // Perpendicular component of the refracted ray
    Vec3 r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.length_Squared())) * n;  // Parallel component
    return r_out_perp + r_out_parallel;
}

#endif
