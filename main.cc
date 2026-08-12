#include "raytracer.h"


#include "bvh.h"
#include "camera.h"
#include "hittable_list.h"
#include "material.h"
#include "quad.h"
#include "sphere.h"
#include "texture.h"
#include "vec3.h"

#include <memory>

void many_spheres() {

  // world
  hittable_list world;

  auto checker = make_shared<checker_texture>(0.5, color(0.4,0.5,0.7), color(1,1,1));
  auto ground_material = std::make_shared<lambertian>(checker);
  world.add(make_shared<sphere>(point3(0,-1000, 0), 1000, ground_material));

  for (int a = -11; a < 11; a++) {
    for (int b = -11; b < 11; b++) {
      auto choose_mat = random_double();

      point3 center(a + 0.9*random_double(), 0.2, b+0.9*random_double());

      if ((center - point3(4,0.2,0)).length() > 0.9) {
        shared_ptr<material> sphere_material;

        if (choose_mat < 0.8) {
          auto albedo = color::random() * color::random();
          auto center2 = center + vec3(0,random_double(0, 0.5), 0);
          sphere_material = make_shared<lambertian>(albedo);
          world.add(make_shared<sphere>(center, center2, 0.2, sphere_material));
        }
        else if (choose_mat < 0.95) {
          auto albedo = color::random(0.5, 1);
          auto fuzz = random_double(0,0.5);
          sphere_material = make_shared<metal>(albedo, fuzz);
          world.add(make_shared<sphere>(center, 0.2, sphere_material));
        }
        else {
          sphere_material = make_shared<dielectric>(1.5);
          world.add(make_shared<sphere>(center, 0.2, sphere_material));
        }
      }
    }
  }

  auto glass = make_shared<dielectric>(1.5);
  world.add(make_shared<sphere>(point3(0,1,0), 1.0, glass));

  auto matte = make_shared<lambertian>(color(0.3,0.1,0.5));
  world.add(make_shared<sphere>(point3(-4,1,0), 1.0, matte));

  auto metalic = make_shared<metal>(color(0.5,0.9,0.2), 0.0);
  world.add(make_shared<sphere>(point3(4,1,0), 1.0, metalic));

  // the fruits of our labor
  world = hittable_list(make_shared<bvh_node>(world));

  camera cam;

  cam.aspect_ratio = 16.0 / 10.0;
  cam.image_width = 1500;
  cam.samples_per_pixel = 500;
  cam.max_depth = 50;
  cam.background = color(0.7,0.8,1.0);


  // image zoom
  cam.vfov = 20;
  cam.lookfrom = point3(13,2,3);
  cam.lookat = point3(0,0,0);
  cam.vup = vec3(0,1,0);

  cam.defocus_angle = 0.6;
  cam.focus_dist = 10.0;

  cam.render(world);

  
}


void checkered_spheres() {
  hittable_list world;

  auto checker = make_shared<checker_texture>(0.20, color(0.4,0.5,0.7), color(1,1,1));
  auto ground_material = std::make_shared<lambertian>(checker);


  world.add(make_shared<sphere>(point3(0,-10,0), 10, ground_material));
  world.add(make_shared<sphere>(point3(0,10,0), 10, ground_material));


  camera cam;

  cam.aspect_ratio = 16.0 / 10.0;
  cam.image_width = 400;
  cam.samples_per_pixel = 100;
  cam.max_depth = 50;
  cam.background = color(0.7,0.8,1.0);


  cam.vfov = 20;
  cam.lookfrom = point3(13, 2, 3);
  cam.lookat = point3(0,0,0);
  cam.vup = vec3(0,1,0);

  cam.defocus_angle = 0;
  cam.render(world);

}


void earth_texture() {
  auto earth_texture = make_shared<image_texture>("earthmap.jpg");
  auto earth_surface = make_shared<lambertian>(earth_texture);

  auto globe = make_shared<sphere>(point3(0,0,0), 2, earth_surface);

  camera cam;

  cam.aspect_ratio = 16.0 / 10.0;
  cam.image_width = 400;
  cam.samples_per_pixel = 100;
  cam.max_depth = 50;
  cam.background = color(0.7,0.8,1.0);


  cam.vfov = 20;
  cam.lookfrom = point3(0,0,12);
  cam.lookat = point3(0,0,0);
  cam.vup = vec3(0,1,0);

  cam.defocus_angle = 0;
  cam.render(hittable_list(globe));

}

void quads() {
  hittable_list world;

  auto earth_texture = make_shared<image_texture>("stare.jpeg");

  auto left_red = make_shared<lambertian>(color(1.0,0.1,0.1));
  auto back_green = make_shared<lambertian>(color(0.1,1.0,0.1));
  auto right_blue = make_shared<lambertian>(color(0.1,0.1,1.0));
  auto up_white = make_shared<lambertian>(color(1.0, 1.0, 1.0));
  auto down_black = make_shared<lambertian>(color(0.1,0.1,0.1));

  world.add(make_shared<quad>(point3(-3, -2, 5), vec3(0,0,-4), vec3(0,4,0), left_red));
  world.add(make_shared<quad>(point3(-2, -2, 0), vec3(4,0,0), vec3(0,4,0), back_green));
  world.add(make_shared<quad>(point3(3, -2, 1), vec3(0,0,4), vec3(0,4,0), right_blue));
  world.add(make_shared<quad>(point3(-2, 3, 1), vec3(4,0,0), vec3(0,0,4), up_white));
  world.add(make_shared<quad>(point3(-2, -3, 5), vec3(4,0,0), vec3(0,0,-4), down_black));

  camera cam;

  cam.aspect_ratio = 1.0;
  cam.image_width = 400;
  cam.samples_per_pixel = 100;
  cam.max_depth = 50;
  cam.background = color(0.7,0.8,1.0);

  cam.vfov = 80;
  cam.lookfrom = point3(0,0,9);
  cam.lookat = point3(0,0,0);
  cam.vup = vec3(0,1,0);

  cam.defocus_angle = 0;

  cam.render(world);

}

void simple_light() {
  hittable_list world;

  auto checker = make_shared<checker_texture>(0.20, color(0.4,0.5,0.7), color(1,1,1));
  auto sphere_material = std::make_shared<lambertian>(checker);
  world.add(make_shared<sphere>(point3(0,2,0), 2, sphere_material));


  auto matte = make_shared<lambertian>(color(0.1,0.2,0.3));
  world.add(make_shared<quad>(point3(-100,0, -100), vec3(200, 0, 0), vec3(0, 0, 200), matte));

  auto difflight = make_shared<diffuse_light>(color(4,4,4));
  auto light_source = make_shared<quad>(point3(3,1,-2), vec3(2,0,0), vec3(0,2,0), difflight);
  world.add(light_source);

  camera cam;

  cam.aspect_ratio = 16.0/10;
  cam.image_width = 1200;
  cam.samples_per_pixel = 500;
  cam.max_depth = 50;
  cam.background = color(0,0,0);

  cam.vfov = 20;
  cam.lookfrom = point3(26, 3, 6);
  cam.lookat = point3(0,2,0);
  cam.vup = vec3(0,1,0);

  cam.defocus_angle = 0;
  
  cam.render(world);
}

void cornell_box() {
  hittable_list world;

  auto white = make_shared<lambertian>(color(0.75,0.75,0.75));
  auto blue = make_shared<lambertian>(color(0.05,0.05,0.75));
  auto red = make_shared<lambertian>(color(0.75,0.05,0.05));
  auto light = make_shared<diffuse_light>(color(25,25,25));


  world.add(make_shared<quad>(point3(555,0,0), vec3(0,555,0), vec3(0,0,555), white));
  world.add(make_shared<quad>(point3(0,0,0), vec3(0,555,0), vec3(0,0,555), white));
  world.add(make_shared<quad>(point3(0,0,0), vec3(555,0,0), vec3(0,0,555), white));
  world.add(make_shared<quad>(point3(555,555,555), vec3(-555,0,0), vec3(0,0,-555), white));
  world.add(make_shared<quad>(point3(0,0,555), vec3(555,0,0), vec3(0,555,0), white));


  shared_ptr<hittable> box2 = box(point3(195,195,195), point3(360,360,360), red);
  //box2 = make_shared<rotate>(box2, 45, 45, 45);
  world.add(box2);

  world.add(make_shared<quad>(point3(343,554,332), vec3(-130,0,0), vec3(0,0,-150), light));


  camera cam;

  cam.aspect_ratio = 1.0;
  cam.image_width = 400;
  cam.samples_per_pixel = 100;
  cam.max_depth = 50;
  cam.background = color(0,0,0);

  cam.vfov = 35;
  cam.lookfrom = point3(278, 278, -800);
  cam.lookat = point3(278, 278, 0);
  cam.vup = vec3(0,1,0);

  cam.defocus_angle = 0;

  cam.render(world);
}

void final_scene(int image_width, int samples_per_pixel, int max_depth) {
    hittable_list boxes1;
    auto ground = make_shared<lambertian>(color(0.48, 0.83, 0.53));

    int boxes_per_side = 20;
    for (int i = 0; i < boxes_per_side; i++) {
        for (int j = 0; j < boxes_per_side; j++) {
            auto w = 100.0;
            auto x0 = -1000.0 + i*w;
            auto z0 = -1000.0 + j*w;
            auto y0 = 0.0;
            auto x1 = x0 + w;
            auto y1 = random_double(1,101);
            auto z1 = z0 + w;

            boxes1.add(box(point3(x0,y0,z0), point3(x1,y1,z1), ground));
        }
    }

    hittable_list world;

    world.add(make_shared<bvh_node>(boxes1));

    auto light = make_shared<diffuse_light>(color(7, 7, 7));
    world.add(make_shared<quad>(point3(123,554,147), vec3(300,0,0), vec3(0,0,265), light));

    auto center1 = point3(400, 400, 200);
    auto center2 = center1 + vec3(30,0,0);
    auto sphere_material = make_shared<lambertian>(color(0.7, 0.3, 0.1));
    world.add(make_shared<sphere>(center1, center2, 50, sphere_material));

    world.add(make_shared<sphere>(point3(260, 150, 45), 50, make_shared<dielectric>(1.5)));
    world.add(make_shared<sphere>(
        point3(0, 150, 145), 50, make_shared<metal>(color(0.8, 0.8, 0.9), 1.0)
    ));

    auto boundary = make_shared<sphere>(point3(360,150,145), 70, make_shared<dielectric>(1.5));
    world.add(boundary);
    boundary = make_shared<sphere>(point3(0,0,0), 5000, make_shared<dielectric>(1.5));

    auto emat = make_shared<lambertian>(make_shared<image_texture>("earthmap.jpg"));
    world.add(make_shared<sphere>(point3(400,200,400), 100, emat));

    hittable_list boxes2;
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    int ns = 1000;
    for (int j = 0; j < ns; j++) {
        boxes2.add(make_shared<sphere>(point3::random(0,165), 10, white));
    }
    camera cam;

    cam.aspect_ratio      = 1.0;
    cam.image_width       = image_width;
    cam.samples_per_pixel = samples_per_pixel;
    cam.max_depth         = max_depth;
    cam.background        = color(0,0,0);

    cam.vfov     = 40;
    cam.lookfrom = point3(478, 278, -600);
    cam.lookat   = point3(278, 278, 0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0;

    cam.render(world);
}

void rotating() {

  hittable_list world;
  auto white = make_shared<lambertian>(color(0.75,0.75,0.75));
  auto red = make_shared<lambertian>(color(0.75,0.05,0.05));
  auto green = make_shared<lambertian>(color(0.05,0.75,0.05));
  auto blue = make_shared<lambertian>(color(0.05,0.05,0.75));
  auto light = make_shared<diffuse_light>(color(25,25,25));


  world.add(make_shared<quad>(point3(555,0,0), vec3(0,555,0), vec3(0,0,555), blue)); // right wall
  world.add(make_shared<quad>(point3(0,0,0), vec3(0,555,0), vec3(0,0,555), blue)); // left wall
  world.add(make_shared<quad>(point3(0,0,0), vec3(555,0,0), vec3(0,0,555), green)); //bottom wall
  world.add(make_shared<quad>(point3(555,555,555), vec3(-555,0,0), vec3(0,0,-555), green)); // top wall
  world.add(make_shared<quad>(point3(0,0,555), vec3(555,0,0), vec3(0,555,0), white));// back wall


  world.add(make_shared<quad>(point3(343,554,332), vec3(-130,0,0), vec3(0,0,-150), light));

  shared_ptr<hittable> box2 = box(point3(0, 0, 0), point3(165, 165, 165), red);

  box2->translate(vec3(195, 195, 195));
  box2->rotate_axis(45, 45, 45, box2->bounding_box().centroid());
  world.add(box2);

  camera cam;

  cam.aspect_ratio = 1.0;
  cam.image_width = 400;
  cam.samples_per_pixel = 5000;
  cam.max_depth = 50;
  cam.background = color(0, 0, 0);

  cam.vfov = 40;
  cam.lookfrom = point3(278, 278, -800);
  cam.lookat = point3(278, 278, 0);
  cam.vup = vec3(0,1,0);

  cam.defocus_angle = 0;

  cam.render(world);
}


int main(int argc, char* argv[]) {

  auto scene = 8;

  if (argc >= 2) {
    scene = std::stoi(argv[1]);
  }

  switch(scene) {
    case 1:
      many_spheres();
      break;
    case 2:
      checkered_spheres();
      break;
    case 3:
      earth_texture();
      break;
    case 4:
      quads();
      break;
    case 5:
      simple_light();
      break;
    case 6:
      cornell_box();
      break;
    case 7:
      final_scene(1200, 10000, 40);
      break;
    case 8:
      rotating();
      break;
    default:
      rotating();
      break;
  }
  return 0;
}
