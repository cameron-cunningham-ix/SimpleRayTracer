#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.hpp"
#include "common.hpp"

class Material;

class Hit_Record {
public:
    Point3 p;                   // The 3D point that the object was hit at
    Vec3 normal;                // Normal vector of the object at the point p
    shared_ptr<Material> mat;   // The material of the object that was hit
    double t;                   // Parameter t at which the ray hit the object
    bool front_face;            // Whether the ray hit a front facing side or not

    // Sets the hit record normal vector
    // NOTE: The parameter 'outward_normal' is assumed to have unit length
    void set_Face_Normal(const Ray& r, const Vec3& outward_normal) {
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class Hittable {
public:
    virtual ~Hittable() = default;

    virtual bool hit(const Ray& r, Interval ray_t, Hit_Record& rec) const = 0;
};


#endif