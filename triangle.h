#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "hittable.h"
#include "hittable_list.h"
#include "material.h"
#include "raytracer.h"
#include "vec3.h"
#include "bvh.h"
#include <iostream>
#include <memory>

class triangle : public hittable {
  public:

    triangle(const point3& Q, const vec3& u, const vec3& v, shared_ptr<material> mat) 
      : Q(Q), u(u), v(v), mat(mat)
    {

      auto n = cross(u, v);
      normal = unit_vector(n);
      D = dot(normal, Q);
      w = n / dot(n, n);
      set_bounding_box();
    }

    virtual void set_bounding_box() {
      auto bbox_diagonal1 = aabb(Q, Q + u + v);
      auto bbox_diagonal2 = aabb(Q + u, Q + v);
      bbox = aabb(bbox_diagonal1, bbox_diagonal2);
    }

    aabb bounding_box() const override { return bbox; }

    bool hit(const ray & r, interval ray_t, hit_record& rec) const override {

      auto denom = dot(normal, r.direction());

      if (std::fabs(denom) < 1e-8)
        return false;

      auto t = (D - dot(normal, r.origin())) / denom;
      if (!ray_t.contains(t))
        return false;

      auto intersection = r.at(t);

      vec3 planar_hitpt_vector = intersection - Q;
      auto alpha = dot(w, cross(planar_hitpt_vector, v));
      auto beta = dot(w, cross(u, planar_hitpt_vector));

      if (!is_interior(alpha, beta, rec))
        return false;

      rec.t = t;
      rec.p  = intersection;
      rec.mat = mat;
      rec.set_face_normal(r, normal);


      return true;
    }

    virtual bool is_interior(double a, double b, hit_record& rec) const {
      interval unit_interval = interval(0, 1);
      if (a < 0 || b < 0 || a + b > 1)
        return false;

      rec.u = a;
      rec.v = b;
      return true;

    }

    // translates to a point 
    void translate(const vec3& t_vector) override {
      this->Q = Q + t_vector; 

      auto n = cross(u, v);
      this->normal = unit_vector(n);
      this->D = dot(normal, Q);
      this->w = n / dot(n, n);

      set_bounding_box();
    }

    // rotate an item around an axis who's origin is defined by a point
    void rotate_axis(double x, double y, double z, const point3& p) override {
      translate(-p);

      this->Q = rotate_point(this->Q, z, y, x);
      this->u = rotate_vector(this->u, z, y, x);
      this->v = rotate_vector(this->v, z, y, x);

      auto n = cross(u, v);
      this->normal = unit_vector(n);
      this->D = dot(normal, Q);
      this->w = n / dot(n, n);
      set_bounding_box();

      translate(p);
    }

  private:
    point3 Q;
    vec3 u, v;
    vec3 w;
    shared_ptr<material> mat;
    aabb bbox;
    vec3 normal; 
    double D;

};


#endif
