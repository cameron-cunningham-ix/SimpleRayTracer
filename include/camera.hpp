#ifndef CAMERA_H
#define CAMERA_H

#include "..\third_party\SDL2\include\SDL.h"

#include "common.hpp"
#include "hittable.hpp"
#include "material.hpp"
#include "environmentmap.hpp"

#include <atomic>
#include <thread>
#include <mutex>
#include <vector>

class Camera {
public:
    // Image
    double aspect_ratio = 1.0;   // Ratio of image width over height
    int image_width = 100;       // Rendered image width in pixel count
    int samples_per_pixel = 10;  // Count of random samples per pixel
    int max_depth = 10;          // Maximum number of ray bounces into scene

    double vfov = 90;                   // Vertical view angle (field of view)
    Point3 lookfrom = Point3(0,0,-1);    // Point camera is looking from
    Point3 lookat = Point3(0,0,1);     // Point camera is looking to
    Vec3 vup = Vec3(0,1,0);             // Camera-relative "up" direction

    double defocus_angle = 0;    // Variation angle of rays through each pixel
    double focus_dist = 10;     // Distance from camera lookfrom point to plane of perfect focus

    // Initialize public camera settings for 'real-time' rendering
    void init_Real_Time_Settings(){
        aspect_ratio = 16.0 / 9.0;
        image_width = 400;
        samples_per_pixel = 2;      // For "real-time" rendering, set samples_per_pixel to 2 and max_depth to 4
        max_depth = 4;              // NOTE: max_depth absolute minimum is 2; if set to one, it only colors pixels that did not hit anything
                                    // max_depth = 3 gets rid of some important reflections as well
        vfov = 45;
        lookfrom = Point3(0,0,-1);
        lookat = Point3(0,0,1);
        vup = Vec3(0,1,0);

        defocus_angle = 1.0;
        focus_dist = 3.4;
    }

    // Initialize custom camera settings
    void init_Custom_Settings() {
        std::string input;

        std::cout << "Default settings:\n"
                << "Aspect Ratio: 16.0 / 9.0\n"
                << "Image width: 800 px\n"
                << "Samples per pixel: 50\n"
                << "Max bounce depth: 20\n"
                << "Vertical Field of View: 45 degrees\n"
                << "Defocus Angle: 1.0\n"
                << "Focus Distance: 3.4\n\n"
                << "Hit ENTER for default settings, enter A to change default settings: ";
        std::cin.ignore();  // Ignores any leftover input from before
        std::getline(std::cin, input);  // Use getline to capture empty input
        
        // Use default settings
        if (input.empty()) {
            aspect_ratio = 16.0 / 9.0;
            image_width = 800;
            samples_per_pixel = 50;
            max_depth = 20;
            vfov = 45;
            defocus_angle = 1.0;
            focus_dist = 3.4;
        } 
        // Custom settings
        else if (input == "A") {
            // Aspect Ratio Width
            double aspr_width;
            std::cout << "Enter Aspect Ratio Width (e.g., 16.0 in 16.0 / 9.0): ";
            std::cin >> aspr_width;

            // Aspect Ratio Height
            double aspr_height;
            std::cout << "Enter Aspect Ratio Height (e.g., 9.0 in 16.0 / 9.0): ";
            std::cin >> aspr_height;

            aspect_ratio = aspr_width / aspr_height;

            // Image Width
            std::cout << "Enter Image Width (px): ";
            std::cin >> image_width;

            // Samples Per Pixel
            std::cout << "Enter Samples Per Pixel (default is 50): ";
            std::cin >> samples_per_pixel;

            // Max Bounce Depth
            std::cout << "Enter Max Bounce Depth (default is 20): ";
            std::cin >> max_depth;

            // Vertical Field of View (vfov)
            std::cout << "Enter Vertical Field of View (degrees): ";
            std::cin >> vfov;

            // Defocus Angle
            std::cout << "Enter Defocus Angle (default is 1.0): ";
            std::cin >> defocus_angle;

            // Focus Distance
            std::cout << "Enter Focus Distance (default is 3.4): ";
            std::cin >> focus_dist;
        } 
        else {
            std::cout << "\nInvalid input. Please try again.\n";
        }
    }

    void render(const Hittable& world, SDL_Surface* surface, const EnvironmentMap* envmap, std::atomic<bool>& rendering_complete) {
        initialize();

        // Determine the number of threads to use based on hardware
        const int num_threads = std::thread::hardware_concurrency();
        std::vector<std::thread> threads;
        
        // Mutex to protect access to the shared SDL surface
        std::mutex surface_mutex;

        // Lambda function to render a section of the image
        auto render_section = [&](int start_row, int end_row) {
            for (int j = start_row; j < end_row; j++) {
                for (int i = 0; i < image_width; i++) {
                    Color pixel_color(0, 0, 0);
                    // Calculate current pixel color
                    for (int sample = 0; sample < samples_per_pixel; sample++) {
                        Ray r = get_Ray(i, j);
                        pixel_color += ray_Color(r, max_depth, world, envmap);
                    }

                    // Convert the color to SDL format
                    Uint32 color = SDL_MapRGB(surface->format,
                        static_cast<Uint8>(255.999 * pixel_samples_scale * pixel_color.x()),
                        static_cast<Uint8>(255.999 * pixel_samples_scale * pixel_color.y()),
                        static_cast<Uint8>(255.999 * pixel_samples_scale * pixel_color.z()));

                    // Lock the mutex before accessing the shared surface
                    std::lock_guard<std::mutex> lock(surface_mutex);
                    Uint32* pixels = (Uint32*)surface->pixels;
                    pixels[j * image_width + i] = color;
                }
            }
        };

        // Divide the image into sections and assign each to a thread
        int rows_per_thread = image_height / num_threads;
        for (int i = 0; i < num_threads; i++) {
            int start_row = i * rows_per_thread;
            // Ensure the last thread covers any remaining rows
            int end_row = (i == num_threads - 1) ? image_height : (i + 1) * rows_per_thread;
            threads.emplace_back(render_section, start_row, end_row);
        }

        // Wait for all threads to complete
        for (auto& thread : threads) {
            thread.join();
        }

        // Signal that rendering is complete
        rendering_complete.store(true);
    }


    int get_Image_Height() {
        return image_height;
    }

    // Alter the camera position
    // move_by component values correspond to speed in that direction relative to camera view
    void update_Camera_Position(Vec3 move_by) {
        // Calculate direction camera is looking in
        Vec3 direction = unit_Vector(lookat - lookfrom);
        Vec3 right = unit_Vector(cross(direction, vup));
        //printf("direction: %f, %f, %f\n", direction.x(), direction.y(), direction.z());
        //printf("right: %f, %f, %f\n", right.x(), right.y(), right.z());
        
        // Move camera left / right relative to orientation
        if (move_by.x() != 0) {
            lookfrom += right * move_by.x();
            lookat += right * move_by.x();
        }
        // Move camera foward / backward relative to direction
        if (move_by.z() != 0) {
            lookfrom += direction * move_by.z();
            lookat += direction * move_by.z();
        }
        // Move camera up / down relative to direction
        if (move_by.y() != 0) {
            lookfrom += vup * move_by.y();
            lookat += vup * move_by.y();
        }
        //printf("lookfrom: %f, %f, %f\n\n\n", lookfrom.x(), lookfrom.y(), lookfrom.z());
    }

    // TODO: NOT DONE YET
    // There's still some unwanted behaviour when pitching up / down
    void update_Camera_Direction(double delta_yaw, double delta_pitch) {
        // Calculate direction camera is looking in
        Vec3 direction = lookat - lookfrom;

        // Rotate around Y-axis for yaw (l-r rotation)
        double cos_yaw = cos(delta_yaw);
        double sin_yaw = sin(delta_yaw);

        // Apply rotation matrix
        //
        // [  cos yaw, 0, sin yaw ]
        // |  0,       1, 0       |
        // [ -sin yaw, 0, cos yaw ]
        Vec3 rotated_dir_yaw = Vec3(
            cos_yaw * direction.x() + sin_yaw * direction.z(),
            direction.y(),
            -sin_yaw * direction.x() + cos_yaw * direction.z()
        );

        // Rotate around right vector for pitch (up-down rotation)
        Vec3 right = unit_Vector(cross(rotated_dir_yaw, vup));
        double cos_pitch = cos(delta_pitch);
        double sin_pitch = sin(delta_pitch);

        // Apply rotation matrix
        //
        // [  1,         0, 0          ]
        // |  0, cos pitch, -sin pitch |
        // [  0, sin pitch, cos pitch  ]
        Vec3 rotated_dir_pit = Vec3(
            rotated_dir_yaw.x(),
            cos_pitch * rotated_dir_yaw.y() - sin_pitch * rotated_dir_yaw.z(),
            sin_pitch * rotated_dir_yaw.y() + cos_pitch * rotated_dir_yaw.z()
        );

        lookat = lookfrom + rotated_dir_pit;

        //printf("lookat: %f, %f, %f\n\n", lookat.x(), lookat.y(), lookat.z());
    }

    

private:
    int image_height;           // Rendered image height
    double pixel_samples_scale; // Color scale factor for a sum of pixel samples
    Point3 center;              // Camera center
    Point3 pixel00_loc;         // Location of pixel 0,0
    Vec3 pixel_delta_u;         // Offset to pixel to the right
    Vec3 pixel_delta_v;         // Offset to the pixel below
    Vec3 u, v, w;               // Camera frame basis vectors
    Vec3 defocus_disk_u;        // Defocus disk horizontal radius
    Vec3 defocus_disk_v;        // Defocus disk vertical radius

    // Initialize the private camera settings
    void initialize() {
        // Calculate image height and make sure that it's at least 1
        image_height = int(image_width/aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;

        pixel_samples_scale = 1.0 / samples_per_pixel;

        center = lookfrom;

        // Determine viewport dimensions
        auto theta = degrees_to_radians(vfov);
        auto h = tan(theta/2);
        auto viewport_height = 2 * h * focus_dist;
        auto viewport_width = viewport_height * (double(image_width)/image_height);

        // Calculate the u,v,w unit basis vectors for the camera coordinate frame
        w = unit_Vector(lookfrom - lookat);
        u = unit_Vector(cross(vup, w));
        v = cross(w, u);

        // Calculate the vectors across the horizontal and down the vertical viewport edges
        auto viewport_u = viewport_width * u;       // Vector accross viewport horizontal edge
        auto viewport_v = viewport_height * -v;     // Vector down viewport vertical edge

        // Calculate hori. and vert. delta vectors from pixel to pixel
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // Calculate the location of the upper left pixel
        auto viewport_upper_left = center - (focus_dist * w) - viewport_u/2 - viewport_v/2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

        // Calculate the camera defocus disk basis vectors
        auto defocus_radius = focus_dist * tan(degrees_to_radians(defocus_angle / 2));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;
    }

    // Returns the vector to a random point in the [-.5,-.5]-[+.5,+.5] unit square
    Vec3 sample_Square() const {    
        return Vec3(random_double() - 0.5, random_double() - 0.5, 0);
    }

    Ray get_Ray(int i, int j) const {
        // Construct a camera ray originating from the defocus disk and directed at randomly sampled
        // point around the pixel location i, j

        auto offset = sample_Square();
        auto pixel_sample = pixel00_loc
                            + ((i + offset.x()) * pixel_delta_u)
                            + ((j + offset.y()) * pixel_delta_v);
        
        auto ray_origin = (defocus_angle <= 0) ? center : defocus_Disk_Sample();
        auto ray_direction = pixel_sample - ray_origin;

        return Ray(ray_origin, ray_direction);
    }

    // Returns a random point in the camera defocus disk
    Point3 defocus_Disk_Sample() const {    
        auto p = random_In_Unit_Disk();
        return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    }

    Ray get_Ray_Consistent(int i, int j, int samples) {    
        // Construct a camera ray originating from the origin and directed at a consistent sampled
        // point around the pixel location i, j based on the # of samples_per_pixel
        Vec3 offset;
        if (samples % 2 == 0) {
            // Bottom left to top right
            offset = Vec3(samples/samples_per_pixel - 0.5, samples/samples_per_pixel - 0.5, 0);
        } else {
            // Top left to bottom right
            offset = Vec3(samples/samples_per_pixel - 0.5, -samples/samples_per_pixel + 0.5, 0);
        }

        offset = Vec3(samples/samples_per_pixel - 0.5, samples/samples_per_pixel - 0.5, 0);
        auto pixel_sample = pixel00_loc
                            + ((i + offset.x()) * pixel_delta_u)
                            + ((j + offset.y()) * pixel_delta_v);

        auto ray_origin = center;
        auto ray_direction = pixel_sample - ray_origin;

        return Ray(ray_origin, ray_direction);
    }

    Color ray_Color(const Ray& r, int depth, const Hittable& world, const EnvironmentMap* envmap = nullptr) const {
        // If we've exceeded the ray bounce limit, no more light is gathered
        if (depth <= 0) {
            return Color(0,0,0);
        }

        Hit_Record rec;

        if (world.hit(r, Interval(0.001, infinity), rec)) {
            Ray scattered;
            Color attenuation;
            if (rec.mat->scatter(r, rec, attenuation, scattered)) {
                return attenuation * ray_Color(scattered, depth-1, world, envmap);
            }
            return Color(0,0,0);
        }

        // Get the unit vector of the ray
        Vec3 unit_direction = unit_Vector(r.direction());
        // If an environment map was provided
        if (envmap) {
            // Map the direction of the ray to (u, v) texture coordinates
            // Environment map images use spherical coordinates
            double u = 0.5 + atan2(unit_direction.z(), unit_direction.x()) / (2*pi);
            double v = 0.5 - asin(unit_direction.y()) / pi;

            return envmap->sample(u, v);
        }
        else {
            // Simple gradient
            // linear interpolation - lerp - between two values
            // blendedValue = (1 - a)*startValue + a*endValue
            // with a going from 0 to 1

            auto a = 0.5*(unit_direction.y() + 1.0);
            return (1.0-a)*Color(1.0, 1.0, 1.0) + a*Color(0.5,0.7,1.0);
        }
    }
};

#endif