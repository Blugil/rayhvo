#ifndef HITTABLE_H
#define HITTABLE_H

#include "interval.h"
#include "aabb.h"

class material;

class hit_record {
  public:
    point3 p;
    vec3 normal;
    shared_ptr<material> mat;
    double t;
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

#endif 
