/**
*	Author: Cameron Cunningham
*	Initial commit date: 6/18/2024
*	Implementation of a simple software ray tracer,
*	fully taken from Ray Tracing in One Weekend tutorial by
*	Peter Shirley, Trevor David Black, Steve Hollasch
*	Link: https://raytracing.github.io/
*/
#include "common.hpp"

#include "camera.hpp"
#include "hittable.hpp"
#include "hittable_list.hpp"
#include "material.hpp"
#include "sphere.hpp"

int main() {

    Hittable_List world;

    auto material_ground = make_shared<Lambertian>(Color(0.9, 0.8, 0.3));
    auto material_center = make_shared<Lambertian>(Color(0.1, 0.5, 0.5));
    auto material_left   = make_shared<Dielectric>(1.50);
    auto material_bubble = make_shared<Dielectric>(1.00 / 1.50);
    auto material_right  = make_shared<Metal>(Color(0.8, 0.6, 0.2), 1.0);

    world.add(make_shared<Sphere>(Point3( 0.0, -100.5, -1.0), 100.0, material_ground));
    world.add(make_shared<Sphere>(Point3( 0.0,    0.0, -1.2),   2.5, material_center));
    world.add(make_shared<Sphere>(Point3(-1.0,    0.0, -1.0),   0.5, material_left));
    world.add(make_shared<Sphere>(Point3(-1.0,    0.0, -1.0),   0.4, material_bubble));
    world.add(make_shared<Sphere>(Point3( 1.0,    0.0, -1.0),   0.5, material_right));
    
    Camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 400;
    cam.samples_per_pixel = 200;
    cam.max_depth = 50;

    cam.vfov = 45;
    cam.lookfrom = Point3(-2,2,1);
    cam.lookat - Point3(0,0,-1);
    cam.vup = Vec3(0,1,0);

    cam.defocus_angle = 10.0;
    cam.focus_dist = 3.4;

    cam.render(world);
}