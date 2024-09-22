#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "hittable.hpp"
#include <vector>

// Hittable_List stores a list of objects that can be hit by rays
class Hittable_List : public Hittable {
public:
    // Vector of shared_ptrs to Hittable objects, automatically handles memory management
    // 
    std::vector<shared_ptr<Hittable>> objects;

    // Default constructor, initializes with an empty list
    Hittable_List() {}
    // Constructor, adds an object to the list
    Hittable_List(shared_ptr<Hittable> object) { add(object); }

    // Clear the list of objects
    void clear() { objects.clear(); }

    // Adds object onto the list
    void add(shared_ptr<Hittable> object) {
        objects.push_back(object);
    }

    // Check if any object in the list is hit by ray r
    // ray_t is the valid interval for the ray. rec stores details if hit
    // Returns true if any object is hit
    bool hit(const Ray& r, Interval ray_t, Hit_Record& rec) const override {
        Hit_Record temp_rec;
        bool hit_anything = false;
        auto closest_so_far = ray_t.max;

        for (const auto& object : objects) {
            // If this object is hit and it's closer than previous hits, update hit record
            if (object->hit(r, Interval(ray_t.min, closest_so_far), temp_rec)) {
                hit_anything = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }

        return hit_anything;
    }
};

#endif