#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.hpp"
#include "common.hpp"

class Material;

class Hit_Record {
public:
    Point3 p;
    Vec3 normal;
    shared_ptr<Material> mat;
    double t;
    bool front_face;

    void set_Face_Normal(const Ray& r, const Vec3& outward_normal) {
        // Sets the hit record normal vector
        // NOTE: The parameter 'outward_normal' is assumed to have unit length

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