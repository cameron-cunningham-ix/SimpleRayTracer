#ifndef SPHERE_H
#define SPHERE_H

#include "common.hpp"
#include "hittable.hpp"

class Sphere : public Hittable {
public:
    // Constructor, initializes sphere with necessary center point, radius, and material
    Sphere(const Point3& center, double radius, shared_ptr<Material> mat) 
    : center(center), radius(fmax(0,radius)), mat(mat) {}

    // Calculates whether a ray 'r' has hit the sphere or not within the given interval
    // 'ray_t'. If it does, fills the Hit_Record with the details of the intersection
    bool hit(const Ray& r, Interval ray_t, Hit_Record& rec) const override {
        // oc is the vector from the ray's origin to the center of the sphere
        Vec3 oc = center - r.origin();  // Equivalent to (C - Q)

        // Quadratic coefficients for solving sphere-ray intersection
        double a = r.direction().length_Squared();
        double h = dot(r.direction(), oc);
        double c = oc.length_Squared() - radius*radius;
        
        double discriminant = h*h - a*c;
        // If the discriminant is negative, there's no real solutions and ray misses the sphere
        if (discriminant < 0) {
            return false;
        }

        // If discriminant is positive, two possible intersections
        double sqrtd = sqrt(discriminant);

        // Find the nearest root, ie smallest t value, that lies in the acceptable range
        double root = (h - sqrtd) / a;
        if (!ray_t.surrounds(root)) {   // If this root is not within range,
            root = (h + sqrtd) /a;      // try other root
            if (!ray_t.surrounds(root)) {
                // Neither root is in range of ray_t, so sphere is not hit
                return false;
            }
        }

        // Hit details
        rec.t = root;   // Parameter 't' of intersection
        rec.p = r.at(rec.t);    // Point on the sphere hit by the ray at "time" 't'
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