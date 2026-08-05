#ifndef HITTABLE_H
#define HITTABLE_H

#include "interval.h"
#include "aabb.h"
#include "raytracer.h"
#include "vec3.h"
#include <memory>

class material;

class hit_record {
  public:
    point3 p;
    vec3 normal;
    shared_ptr<material> mat;
    double t;
    double u;
    double v;
    bool front_face;


    void set_face_normal(const ray& r, const vec3& outward_normal) {
      //outward normal should have unit length (assumed)
      //sets the hit record normal vector
      

      // if the dotproduct of the two vectors is less than zero than we're on the outside
      // otherwise we're inside
      front_face = dot(r.direction(), outward_normal) < 0;
      normal = front_face ? outward_normal : -outward_normal;
    }

};

// abstract class for a hittable object
class hittable {
  public:
    virtual ~hittable() = default;

    virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;

    virtual aabb bounding_box() const = 0;
};

class translate : public hittable {
  public: 

    translate(shared_ptr<hittable> object, const vec3& offset) : object(object), offset(offset) {
      bbox = object->bounding_box() + offset;
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
      ray offset_r(r.origin() - offset, r.direction(), r.time());

      if (!object->hit(offset_r, ray_t, rec)) {
        return false;
      }

      rec.p += offset;
      return true;
    }

    aabb bounding_box() const override { return bbox; }

  private:
    shared_ptr<hittable> object;
    vec3 offset;
    aabb bbox;

};

class rotate : public hittable {
  public: 

    rotate(shared_ptr<hittable> object, double x, double y, double z) 
      : object(object), alpha(z), beta(y), gamma(x) {

      bbox = object->bounding_box();

      point3 min(infinity, infinity, infinity);
      point3 max(-infinity, -infinity, -infinity);


      // this rotates the bounding box but it seems kinda unintuitive
      // i understand its necessary but just kinda fails to help me understand
      // can't we just recalc the bounding box on a rotation?
      // either way this class is being extended to handle rotations in any direction
      // rather than just the y axis
      for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
          for (int k = 0; k < 2; k++) {
            // grabs the min or max
            auto x = i*bbox.x.max + (1-i)*bbox.x.min;
            auto y = j*bbox.y.max + (1-j)*bbox.y.min;
            auto z = k*bbox.z.max + (1-k)*bbox.z.min;

            auto tester = vec3(x,y,z);
            tester = rotate_vector(tester, alpha, beta, gamma);


            for (int c = 0; c < 3; c++) {
              min[c] = std::fmin(min[c], tester[c]);
              max[c] = std::fmax(max[c], tester[c]);
            }
          }
        }
      }

      bbox = aabb(min, max);
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
      auto origin = r.origin();
      origin = rotate_point(origin, -alpha, -beta, -gamma);

      auto direction = r.direction();
      direction = rotate_vector(direction, -alpha, -beta, -gamma);

      ray rotated_r(origin, direction, r.time());

      if (!object->hit(rotated_r, ray_t, rec)) {
        return false;
      }

      rec.p = rotate_point(rec.p, alpha, beta, gamma);
      rec.normal = rotate_vector(rec.normal, alpha, beta, gamma);

      return true;

    }

    aabb bounding_box() const override { return bbox; }


  private:
    double alpha, beta, gamma;
    shared_ptr<hittable> object;
    aabb bbox;
};

#endif 
