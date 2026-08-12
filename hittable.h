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
    virtual void translate(const vec3& v) = 0;
    virtual void rotate_axis(double x, double y, double z, const point3& p) = 0;
    //virtual void rotate_centroid(double x, double y, double z) = 0;

    virtual aabb bounding_box() const = 0;
};

#endif 
