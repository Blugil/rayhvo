#ifndef CAMERA_H
#define CAMERA_H

#include "hittable.h"
#include "hittable_list.h"
#include "material.h"
#include "raytracer.h"
#include "vec3.h"

#include "external/stb_image_write.h"


#include <ostream>
#include <thread>

enum image_save {
  NONE = 0,
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

    image_save ftype = NONE;

    void render(const hittable& world) {
      initialize();

      std::vector<char> image(image_width * color_channel * image_height);

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

        std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
        // the main render loop
        for (int line = 0; line < image_height;) {

          std::clog << "\rScanlines remaining: " << (image_height - line) << ' ' << std::flush;
          for (size_t t_idx = 0; t_idx < num_threads && line < image_height; ++line, ++t_idx) {
            // threads render out a single line of the image to be saved to the image buffer
            threads.emplace_back([this, line, &image, &world](){
              for (int i = 0; i < image_width; ++i) {
                color pixel_color(0,0,0);
                for (int sample = 0; sample < samples_per_pixel; ++sample) {
                  ray r = get_ray(i, line);
                  pixel_color += ray_color(r, max_depth, world);
                }
                image[line * image_width + 0] = (char)pixel_color.x();
                image[line * image_width + 1] = (char)pixel_color.y();
                image[line * image_width + 2] = (char)pixel_color.z();
                //image[line * image_width + i] = pixel_color;
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
        std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
        for (int line = 0; line < image_height; ++line) {
          std::clog << "\rScanlines remaining: " << (image_height - line) << ' ' << std::flush;
          for (int i = 0; i < image_width; ++i) {
            color pixel_color(0,0,0);
            for (int sample = 0; sample < samples_per_pixel; ++sample) {
              ray r = get_ray(i, line);
              pixel_color += ray_color(r, max_depth, world);
            }
            image[line * image_width + 0] = (char)pixel_color.x();
            image[line * image_width + 1] = (char)pixel_color.y();
            image[line * image_width + 2] = (char)pixel_color.z();
            //image[line * image_width + i] = pixel_color;
          }
        }
      }
      // write to file code, right now we just dump into the stdout like normal
      /*
      for (auto pixel_color : image) {
        write_color(std::cout, pixel_samples_scale * pixel_color);
      }
      */
      std::clog << "\rDone.               \n";

      image.clear();
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

      
      //calculate the horizontal and vertical delta vectors from pixel to pixel 
      pixel_delta_u = viewport_u / image_width;
      pixel_delta_v = viewport_v / image_height;

      auto viewport_upper_left = camera_center - (focus_dist * w) - (viewport_u/2) - (viewport_v/2);
      pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

      auto defocus_radius = focus_dist * std::tan(degrees_to_radians(defocus_angle / 2));
      defocus_disk_u = u * defocus_radius;
      defocus_disk_v = v * defocus_radius;

    }

    int write_image_to_file(char* filename, const std::vector<char> pixels) {

      //TODO
      //
      //std_image functions want an array of pixel channels instead of packed 32bit ints
      //can probably just manually save each entry into the char array, with "comp" being # channels
      //start with 3 channel (no alpha channel)
      //CLI flags take -p (as save to png) with a filename attachment 
      
      switch(ftype) {
        case PNG:
          //png stuff
          
          break;
        case JPG:
          //jpg stuff for later
          break;
        default:
          //i imagine the original stdio redirect technique goes here
          break;
      }


      return 1;
    }

    /*
    void render_line(size_t idx, int line, std::vector<color>& scanlines, const hittable& world) {
      for (int i = 0; i < image_width; i++) {
        color pixel_color(0,0,0);
        for (int sample = 0; sample < samples_per_pixel; sample++) {
          ray r = get_ray(i, line);
          pixel_color += ray_color(r, max_depth, world);
        }
        //std::clog << pixel_color.x() << pixel_color.y() << pixel_color.z() << std::endl;
        scanlines[idx * image_width + i] = pixel_color;
      }
    }
    */

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
