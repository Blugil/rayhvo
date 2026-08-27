#ifndef CAMERA_H
#define CAMERA_H

#include "color.h"
#include "hittable.h"
#include "hittable_list.h"
#include "material.h"
#include "raytracer.h"
#include "vec3.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "external/stb_image_write.h"

#include <ostream>
#include <string>
#include <thread>

enum image_save {
  STDOUT = 0,
  PNG = 1,
  JPG = 2,
};

class camera {
  public:
    double aspect_ratio = 1.0;
    int image_width = 100;
    int samples_per_pixel = 10;
    int max_depth = 10;
    color background;
    int color_channel = 3; //rgb, may eventually explore rgba for future so leaving this here

    double vfov = 90.0;
    point3 lookfrom = point3(0,0,0);
    point3 lookat = point3(0,0,-1);
    vec3 vup = vec3(0,1,0);

    double defocus_angle = 0;
    double focus_dist = 10;

    bool threaded = false;
    int num_threads = 0;

    image_save ftype = PNG;
    char const *file_n = nullptr;

    void render(const hittable& world) {
      initialize();

      char *image = new char[image_width * color_channel * image_height]();

      if (num_threads > 1 && threaded) {

        int sys_threads = std::thread::hardware_concurrency();
        if (sys_threads < num_threads) 
          std::clog << "Selected thread count: " 
            << num_threads 
            << " is too high, defaulting to: " 
            << sys_threads - 1 
            << std::endl;

        num_threads = sys_threads < num_threads ? sys_threads - 1: num_threads; 
        std::clog << "threads: " << num_threads << std::endl;
        std::vector<std::thread> threads(num_threads);

        // the main render loop
        for (int line = 0; line < image_height;) {
          std::clog << "\rScanlines remaining: " << (image_height - line) << ' ' << std::flush;
          for (size_t t_idx = 0; t_idx < num_threads && line < image_height; ++line, ++t_idx) {
            // threads render out a single line of the image to be saved to the image buffer
            threads.emplace_back([this, line, &image, &world](){
              for (int row = 0; row < image_width; ++row) {
                color pixel_color(0,0,0);
                for (int sample = 0; sample < samples_per_pixel; ++sample) {
                  ray r = get_ray(row, line);
                  pixel_color += ray_color(r, max_depth, world);
                }
                uint32_t pixel = pack_color(pixel_samples_scale * pixel_color);
                size_t idx = (line * image_width + row) * color_channel;
                image[idx + 0] = ((pixel >> 16) & 0xFF); // r
                image[idx + 1] = ((pixel >> 8) & 0xFF);  // g
                image[idx + 2] = (pixel & 0xFF);         // b
              }
            });
          }
          for (auto& t : threads) {
            if (t.joinable()) t.join();
          }
          threads.clear();
        }
        std::clog << "\rDone.               \n";
      }

      // original single-threaded version of the application
      else {
        for (int line = 0; line < image_height; ++line) {
          std::clog << "\rScanlines remaining: " << (image_height - line) << ' ' << std::flush;
          for (int row = 0; row < image_width; ++row) {
            color pixel_color(0,0,0);
            for (int sample = 0; sample < samples_per_pixel; ++sample) {
              ray r = get_ray(row, line);
              pixel_color += ray_color(r, max_depth, world);
            }

            uint32_t pixel = pack_color(pixel_samples_scale * pixel_color);
            size_t idx = (line * image_width + row) * color_channel;
            image[idx + 0] = ((pixel >> 16) & 0xFF); // r
            image[idx + 1] = ((pixel >> 8) & 0xFF);  // g
            image[idx + 2] = (pixel & 0xFF);         // b
          }
        }
      }
      // write to file code, right now we just dump into the stdout like normal
      write_image_to_file((char *)file_n, image);
      std::clog << "\rDone.               \n";

      // gotta clean up after myself
      delete[] image;
    }


  private:

    int image_height;
    double pixel_samples_scale;
    point3 camera_center;
    point3 pixel00_loc;
    vec3 pixel_delta_u;
    vec3 pixel_delta_v;
    vec3 u, v, w;
    vec3 defocus_disk_u;
    vec3 defocus_disk_v;

    void initialize() {

      image_height = int(image_width / aspect_ratio);
      image_height = (image_height < 1) ? 1 : image_height;

      pixel_samples_scale = 1.0 / samples_per_pixel;

      camera_center = lookfrom;

      auto theta = degrees_to_radians(vfov);
      auto h = std::tan(theta/2);
      auto viewport_height = 2 * h * focus_dist;
      auto viewport_width = viewport_height * (double(image_width) / image_height);

      w = unit_vector(lookfrom - lookat);
      u = unit_vector(cross(vup, w));
      v = cross(w, u);

      auto viewport_u = viewport_width * u;
      auto viewport_v = viewport_height * -v;

      pixel_delta_u = viewport_u / image_width;
      pixel_delta_v = viewport_v / image_height;

      auto viewport_upper_left = camera_center - (focus_dist * w) - (viewport_u/2) - (viewport_v/2);
      pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

      auto defocus_radius = focus_dist * std::tan(degrees_to_radians(defocus_angle / 2));
      defocus_disk_u = u * defocus_radius;
      defocus_disk_v = v * defocus_radius;

    }

    int write_image_to_file(char* filename, char* bpixels) {

      //TODO
      switch(ftype) {
        case STDOUT:
          // localized stdout from the binary array in ppm format
          std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
          for (size_t line = 0; line < image_height; ++line) {
            for (size_t row = 0; row < image_width; ++row) {
              size_t idx = (line * image_width + row) * color_channel;
              const color pixel_color = color(bpixels[idx + 0], bpixels[idx + 1], bpixels[idx + 2]);
              write_color(std::cout, pixel_color);
            }
          }
          return 0;
          break;
        case PNG:
          //png stuff
          return stbi_write_png(filename, image_width, image_height, color_channel, bpixels, 0); // 0 stride bytes
          break;
        case JPG:
          //jpg stuff for later
        default:
          //i imagine the original stdio redirect technique goes here
          std::clog << "No output type selected, please use as directed:" << std::endl;
          fprintf(stderr, "nUsage: [-s scenes (1-9)] [-t nthreads] [-j (jpg) / -p (png) / -o (stdout) \n");
          return 1;
          break;
      }

      return 1;
    }

    ray get_ray(int i, int j) const {
      auto offset = sample_square();
      auto pixel_sample = pixel00_loc 
                        + ((i + offset.x()) * pixel_delta_u) 
                        + ((j + offset.y()) * pixel_delta_v);

      auto ray_origin = (defocus_angle <= 0) ? camera_center : defocus_disk_sample();
      auto ray_direction = pixel_sample - ray_origin;

      auto ray_time = random_double();

      return ray(ray_origin, ray_direction, ray_time);
    }

    vec3 sample_square() const {
      return vec3(random_double() - 0.5, random_double() - 0.5, 0);
    }

    point3 defocus_disk_sample() const {
      auto p = random_in_unit_disk();
      return camera_center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    }

    // option for a sample disk function, which is used in the github
    
    color ray_color(const ray& r, int depth, const hittable& world) const {
      if (depth <= 0) {
        return color(0,0,0);
      }
      hit_record rec;

      if(!world.hit(r, interval(0.001, infinity), rec))
        return background;

      ray scattered;
      color attenuation;
      color color_from_emission = rec.mat->emitted(rec.u, rec.v, rec.p);

      if(!rec.mat->scatter(r, rec, attenuation, scattered)) {
        return color_from_emission;
      }

      color color_from_scatter = attenuation * ray_color(scattered, depth-1, world);
      return color_from_emission + color_from_scatter;
    }
};

#endif
