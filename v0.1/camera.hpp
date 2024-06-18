#ifndef CAMERA_H
#define CAMERA_H

#include "common.hpp"
#include "hittable.hpp"
#include "material.hpp"

class Camera {
public:
    // Image
    double aspect_ratio = 1.0;   // Ratio of image width over height
    int image_width = 100;       // Rendered image width in pixel count
    int samples_per_pixel = 10;  // Count of random samples per pixel
    int max_depth = 10;          // Maximum number of ray bounces into scene

    double vfov = 90;   // Vertical view angle (field of view)
    Point3 lookfrom = Point3(0,0,0);    // Point camera is looking from
    Point3 lookat = Point3(0,0,-1);     // Point camera is looking to
    Vec3 vup = Vec3(0,1,0);             // Camera-relative "up" direction

    double defocus_angle = 0;    // Variation angle of rays through each pixel
    double focus_dist = 10;     // Distance from camera lookfrom point to plane of perfect focus

    void render(const Hittable& world) {
        initialize();

        std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";

        for (int j = 0; j < image_height; j++) {
            std::clog << "\rScanlines remaining: " << (image_height - j) << " " << std::flush;

            for (int i = 0; i < image_width; i++) {
                Color pixel_color(0,0,0);

                for (int sample = 0; sample < samples_per_pixel; sample++) {
                    Ray r = get_Ray(i,j);
                    pixel_color += ray_Color(r, max_depth, world);
                }
                
                write_Color(std::cout, pixel_samples_scale * pixel_color);
        }
    }

    std::clog << "\rDone.                       \n";

    }

    int get_Image_Height() {
        return image_height;
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

        // Calculate the vectors across the horizontal and down the vertical viewport edges.
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

    Vec3 sample_Square() const {
        // Returns the vector to a random point in the [-.5,-.5]-[+.5,+.5] unit square
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

    Point3 defocus_Disk_Sample() const {
        // returns a random point in the camera defocus disk
        auto p = random_In_Unit_Disk();
        return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    }

    Ray get_Ray_Consistent(int i, int j, int samples){
        // Construct a camera ray originating from the origin and directed at a consistent sampled
        // point around the pixel location i, j based on the # of samples_per_pixel
        Vec3 offset;
        if (samples % 2 == 0){
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

    Color ray_Color(const Ray& r, int depth, const Hittable& world) const {
        // If we've exceeded the ray bounce limit, no more light is gathered
        if (depth <= 0) {
            return Color(0,0,0);
        }

        Hit_Record rec;

        if (world.hit(r, Interval(0.001, infinity), rec)) {
            Ray scattered;
            Color attenuation;
            if (rec.mat->scatter(r, rec, attenuation, scattered))
                return attenuation * ray_Color(scattered, depth-1, world);
            return Color(0,0,0);
        }

        // Simple gradient
        // linear interpolation - lerp - between two values
        // blendedValue = (1 - a)*startValue + a*endValue
        // with a going from 0 to 1

        Vec3 unit_direction = unit_Vector(r.direction());
        auto a = 0.5*(unit_direction.y() + 1.0);
        return (1.0-a)*Color(1.0, 1.0, 1.0) + a*Color(0.5,0.7,1.0);
    }
};

#endif