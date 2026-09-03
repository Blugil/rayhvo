#ifndef MESH_H
#define MESH_H

#include "hittable.h"
#include "interval.h"
#include "material.h"
#include "raytracer.h"
#include "vec3.h"
#include "aabb.h"
#include "triangle.h"

#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>
#include <stdio.h>

class mesh : public hittable {

  std::vector<shared_ptr<triangle>> tris;

  public:

    // something must load in the object file (obj file)
    // obj file contains:
    //  - vertices
    //  - uv coordinates
    //  - vertex normals
    //  - faces
    // from here i think we only really need the vertices -> faces
    // each time we construct a face (per the obj file) we add a triangle to the tris vector
    //  - can ignore the texture coordinates and normals for now?
    // this triangle gets constructed
    // *the mesh should be optimized using a bvh accelerator

    mesh() {}
    mesh(char const* filename) {
      auto white = make_shared<lambertian>(color(0.75,0.75,0.75));
      if(!load_mesh(filename, white))
        exit(EXIT_FAILURE);

      for (const auto& tri : tris) {
        bbox = aabb(bbox, tri->bounding_box());
      }
    }
    mesh(char const* filename, shared_ptr<material> mat) {
      load_mesh(filename, mat);
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
      // feels wrong 
      hit_record temp_rec;
      bool hit_anything = false;
      auto closest_so_far = ray_t.max;
      // iterator over the objects 
      for (const auto& tri : tris) {
        if (tri->hit(r, interval(ray_t.min, closest_so_far), temp_rec)) {
          hit_anything = true;
          closest_so_far = temp_rec.t;
          rec = temp_rec;
        }
      }

      return hit_anything;
    }
    void translate(const vec3& v) override {}
    void rotate_axis(double x, double y, double z, const point3& p) override {}
    aabb bounding_box() const override { return bbox; }


  private:

    aabb bbox; 

    bool load_mesh(char const* filename, shared_ptr<material> mat) {

      std::vector<point3> vertices;
      std::ifstream obj_file(filename);

      // Enormously complex, I hope this is among the attrocities I look at fondly
      // as I become a more capable c++ programmer;
      // at least I can take solice in the fact that this bad code must be written by 
      // mortal hands.
      std::string line;
      while (std::getline(obj_file, line) ) {

        // grab the line leader
        size_t position ;
        position = line.find(" ");
        std::string option = line.substr(0, position);
        line.erase(0, position + 1);

        if (option == "v") {
          std::vector<double> temp;
          while((position = line.find(" ")) != std::string::npos) {
            std::string coord = line.substr(0, position);
            temp.push_back(std::strtod(coord.c_str(), NULL));
            line.erase(0, position + 1);
          }
          temp.push_back(std::strtod(line.c_str(), NULL));
          vertices.push_back(point3(temp[0], temp[1], temp[2]));
          temp.clear();
        }
        else if (option == "f") {
          std::vector<size_t> temp;
          while((position = line.find(" ")) != std::string::npos) {
            std::string coord = line.substr(0, position);
            temp.push_back(std::strtod(coord.c_str(), NULL));
            line.erase(0, position + 1);
          }
          temp.push_back(std::strtod(line.c_str(), NULL));
          // convert to 0 indexed array
          // counterclockwise rendered triangles should be created
          // in an index that works like this
          auto A = vertices[temp[0] - 1];
          auto B = vertices[temp[1] - 1];
          auto C = vertices[temp[2] - 1];
          temp.clear();

          tris.push_back(std::make_shared<triangle>(A, B, C, mat));
        }
        else if (option == "#") {
          continue;
        }
        else if (line == "") {
          continue;
        }
        // room for more thorough parsing of obj files
        // likely to get its own separate file if expanded beyond toy
        else {
          return false;
        }
      }

      return true;
    }

};



#endif 
