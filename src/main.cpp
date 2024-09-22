/**
*	Author: Cameron Cunningham
*	Initial commit date: 6/18/2024
*	Implementation of a simple software ray tracer
*   with SDL2 viewing window.
*	Adapted from Ray Tracing in One Weekend tutorial by
*	Peter Shirley, Trevor David Black, Steve Hollasch
*	Link: https://raytracing.github.io/
*/
#define SDL_MAIN_HANDLED    // Disables SDL's handling of main
#include "common.hpp"

#include "camera.hpp"
#include "hittable.hpp"
#include "hittable_list.hpp"
#include "material.hpp"
#include "sphere.hpp"


int main(int argc, char* argv[]) {

    std::cout << "Starting program...\n";
    

    Hittable_List world;

    auto material_ground = make_shared<Lambertian>(Color(0.9, 0.8, 0.3));
    auto material_center = make_shared<Lambertian>(Color(0.1, 0.5, 0.5));
    auto material_left   = make_shared<Dielectric>(1.50);
    auto material_bubble = make_shared<Dielectric>(1.00 / 1.50);
    auto material_right  = make_shared<Metal>(Color(0.8, 0.6, 0.2), 1.0);

    world.add(make_shared<Sphere>(Point3( 0.0, -50.5, -1.0), 50.0, material_ground));
    //world.add(make_shared<Sphere>(Point3( 0.0,    0.0, -1.2),   2.5, material_center));
    //world.add(make_shared<Sphere>(Point3(-1.0,    0.0, -1.0),   0.5, material_left));
    world.add(make_shared<Sphere>(Point3(-1.0,    0.0, -1.0),   0.4, material_bubble));
    world.add(make_shared<Sphere>(Point3( 1.0,    0.0, -1.0),   0.5, material_right));
    
    Camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 400;
    cam.samples_per_pixel = 1;      // For "real-time" rendering, set samples_per_pixel to 1 and max_depth to 2
    cam.max_depth = 2;              // NOTE: max_depth minimum is 2; if set to one, it only colors pixels that did not hit anything

    cam.vfov = 45;
    cam.lookfrom = Point3(0,0,0);
    cam.lookat - Point3(0,0,1);
    cam.vup = Vec3(0,1,0);

    cam.defocus_angle = 1.0;
    cam.focus_dist = 3.4;

    std::cout << "Starting SDL...\n";

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return -1;
    } else{
        std::cout << "SDL initialized successfully.\n";
    }

    SDL_Window* window = SDL_CreateWindow("Simple Ray Tracer",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        cam.image_width, int(cam.image_width/cam.aspect_ratio), SDL_WINDOW_SHOWN);

    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_Surface* surface = SDL_GetWindowSurface(window);

    std::cout << "Starting rendering...\n";

    cam.render(world, surface);

    // Wait for the window to close
    SDL_Event e;
    bool quit = false;
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            if (e.type == SDL_KEYDOWN){
                switch(e.key.keysym.sym){

                    // Change camera position
                    case SDLK_a:
                        cam.update_Camera_Position(Vec3(0, 0, -0.5));
                        break;
                    case SDLK_d:
                        cam.update_Camera_Position(Vec3(0, 0, 0.5));
                        break;
                    case SDLK_w:
                        cam.update_Camera_Position(Vec3(-0.5, 0, 0));
                        break;
                    case SDLK_s:
                        cam.update_Camera_Position(Vec3(0.5, 0, 0));
                        break;
                    case SDLK_SPACE:
                        cam.lookfrom += Point3(0, 0.1, 0);
                        cam.lookat += Point3(0, 0.1, 0);
                        break;
                    case SDLK_LSHIFT:
                        cam.lookfrom += Point3(0, -0.1, 0);
                        cam.lookat += Point3(0, -0.1, 0);
                        break;
                    
                    // TODO: Update lookat movement
                    // Change camera lookat
                    case SDLK_UP:
                        cam.lookat = cam.lookat + Point3(0, 0.1, 0);
                        break;
                    case SDLK_DOWN:
                        cam.lookat = cam.lookat + Point3(0, -0.1, 0);
                        break;
                    case SDLK_LEFT:
                        cam.lookat = cam.lookat + Point3(-0.1, 0, 0);
                        break;
                    case SDLK_RIGHT:
                        cam.lookat = cam.lookat + Point3(0.1, 0, 0);
                        break;
                }
            }
        }

        cam.render(world, surface);
    }

    std::cout << "Rendering complete...\n";

    // Clean up
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}