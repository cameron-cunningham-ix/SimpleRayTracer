#ifndef SPHERE_H
#define SPHERE_H

#include "common.hpp"
#include "hittable.hpp"

class Sphere : public Hittable {
public:
    Sphere(const Point3& center, double radius, shared_ptr<Material> mat) 
    : center(center), radius(fmax(0,radius)), mat(mat) {}

    bool hit(const Ray& r, Interval ray_t, Hit_Record& rec) const override {
        Vec3 oc = center - r.origin();  // Equivalent to (C - Q)
        auto a = r.direction().length_Squared();
        auto h = dot(r.direction(), oc);
        auto c = oc.length_Squared() - radius*radius;
        
        auto discriminant = h*h - a*c;
        if (discriminant < 0) {
            return false;
        }

        auto sqrtd = sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range
        auto root = (h - sqrtd) / a;
        if (!ray_t.surrounds(root)) {
            root = (h + sqrtd) /a;
            if (!ray_t.surrounds(root)) {
                return false;
            }
        }

        rec.t = root;
        rec.p = r.at(rec.t);
        Vec3 outward_normal = (rec.p - center) / radius;
        rec.set_Face_Normal(r, outward_normal);
        rec.mat = mat;

        return true;
    }

private:
    Point3 center;
    double radius;
    shared_ptr<Material> mat;
};

#endif