/**
*	Author: Cameron Cunningham
*	Initial commit date: 6/18/2024
*	Implementation of a simple software ray tracer
*   with SDL2 viewing window.
*	Adapted from Ray Tracing in One Weekend tutorial by
*	Peter Shirley, Trevor David Black, Steve Hollasch
*	Link: https://raytracing.github.io/
*
*   BRANCH: MAJOR REFACTOR
*   FOCUS: 1. INCLUDE PROPER DOCUMENTATION FOR EVERYTHING
*   2. REFACTOR THREADING
*       - RENDERING STARTING IN MAIN THREAD AND CREATING FUTURES
*       MAY BE CAUSING THE SLOWDOWN IN SINGLE RENDER OPTION
*       - SWITCH TO STD::THREAD
*
*
*/
#define SDL_MAIN_HANDLED    // Disables SDL's handling of main
#include "common.hpp"
#include "camera.hpp"
#include "hittable.hpp"
#include "hittable_list.hpp"
#include "material.hpp"
#include "sphere.hpp"

#include <string>
#include <atomic>
#include <thread>

// Atomic flag to signal when rendering is complete
// Using atomic ensures thread-safe access without explicit locking
std::atomic<bool> rendering_complete(false);
// Atomic flag to signal when a frame should be rendered
std::atomic<bool> should_render(true);

int main(int argc, char* argv[]) {

    std::cout << "Starting program...\n";

    // Worldspace setup
    Hittable_List world;

    auto material_ground = make_shared<Lambertian>(Color(0.9, 0.8, 0.3));
    auto material_center = make_shared<Lambertian>(Color(0.1, 0.5, 0.5));
    auto material_left   = make_shared<Dielectric>(1.50);
    auto material_bubble = make_shared<Dielectric>(1.00 / 1.50);
    auto material_right  = make_shared<Metal>(Color(0.8, 0.6, 0.2), 0.1);

    world.add(make_shared<Sphere>(Point3( 0.0, -50.5, 1.0), 50.0, material_ground));
    //world.add(make_shared<Sphere>(Point3(-1.0,    0.0, -1.0),   0.5, material_left));
    world.add(make_shared<Sphere>(Point3(-1.0,    0.0, 1.0),   0.4, material_bubble));
    world.add(make_shared<Sphere>(Point3( 1.0,    0.0, 1.0),   0.5, material_right));
    
    EnvironmentMap envmap("..\\include\\hdr\\texturify_court.jpg");

    std::cout << "\nCamera Settings\n'Real-time' rendering (interactive): Enter A\n"
            << "(CURRENTLY BUGGY!) Single render with settings: Enter B\n"
            << "Input: ";

    std::string input;
    bool real_time_rendering = true;

    std::cin >> input;
    while (input.compare("A") && input.compare("B")){
        std::cout << "\nInvalid input"
                << "\n'Real-time' rendering (interactive): Enter A:\nSingle render with settings: Enter B\n"
                << "Input: ";
        std::cin >> input;
    }
    
    // Create and initialize Camera object
    Camera cam;

    if (input == "A"){
        cam.init_Real_Time_Settings();
        std::cout << "Starting rendering...\n"
                << "Use WASD to move camera position,\nuse arrow keys to move camera direction\n"
                << "Hit ESCAPE to close the program.\n";
    }
    else if (input == "B"){
        real_time_rendering = false;
        cam.init_Custom_Settings();

        std::cout << "Hit ESCAPE to close the program.\n";
    }


    std::cout << "Starting SDL...\n";

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("Simple Ray Tracer",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        cam.image_width, int(cam.image_width/cam.aspect_ratio), SDL_WINDOW_SHOWN);

    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_Surface* surface = SDL_GetWindowSurface(window);
    SDL_RaiseWindow(window);

    // Thread object
    // This allows the main thread to remain responsive for SDL events
    std::thread render_thread;

    // Wait for the window to close
    SDL_Event e;
    bool quit = false;

    while (!quit) {
        // Start a new render if needed
        if (!render_thread.joinable() && should_render.load()) {
            render_thread = std::thread(&Camera::render, &cam, std::ref(world),
                surface, &envmap, std::ref(rendering_complete));
        }

        // Handle SDL events
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
                should_render.store(false);
            }
            if (e.type == SDL_KEYDOWN){
                switch(e.key.keysym.sym){
                    case SDLK_ESCAPE:
                        quit = true;
                        break;
                    // Change camera position
                    case SDLK_a:
                        cam.update_Camera_Position(Vec3(-0.5, 0, 0));
                        break;
                    case SDLK_d:
                        cam.update_Camera_Position(Vec3(0.5, 0, 0));
                        break;
                    case SDLK_w:
                        cam.update_Camera_Position(Vec3(0, 0, 0.5));
                        break;
                    case SDLK_s:
                        cam.update_Camera_Position(Vec3(0, 0, -0.5));
                        break;
                    case SDLK_SPACE:
                        cam.update_Camera_Position(Vec3(0, 0.5, 0));
                        break;
                    case SDLK_LSHIFT:
                        cam.update_Camera_Position(Vec3(0, -0.5, 0));
                        break;
                    
                    // Change camera direction
                    case SDLK_UP:
                        cam.update_Camera_Direction(0, -0.1);
                        break;
                    case SDLK_DOWN:
                        cam.update_Camera_Direction(0, 0.1);
                        break;
                    case SDLK_LEFT:
                        cam.update_Camera_Direction(0.1, 0);
                        break;
                    case SDLK_RIGHT:
                        cam.update_Camera_Direction(-0.1, 0);
                        break;
                }
            }
        }

        // Update the window periodically
        // This ensures the user sees the progress of the render
        SDL_UpdateWindowSurface(window);
        SDL_Delay(16);  // Cap at roughly 60 FPS for smooth updates

        // Check if rendering is complete
        if (rendering_complete.load()) {
            //std::cout << "Frame complete...\n";

            // Ensure rendering thread is finished for this frame
            if (render_thread.joinable()) {
                render_thread.join();
            }

            if (!real_time_rendering){
                should_render.store(false);
            } 
            else {
                // Reset for next frame in real-time mode
                rendering_complete.store(false);
                should_render.store(true);
            }
        }
        
    }

    // Ensure rendering thread is finished before cleaning up
    // This is crucial to prevent accessing destroyed resources
    if (render_thread.joinable()) {
        render_thread.join();
    }

    std::cout << "Rendering complete.\n";

    // Clean up
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}