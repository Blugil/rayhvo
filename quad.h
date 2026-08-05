#ifndef QUAD_H
#define QUAD_H

#include "hittable.h"
#include "hittable_list.h"
#include "material.h"
#include "raytracer.h"
#include "vec3.h"
#include <iostream>

class quad : public hittable {
  public:

    quad(const point3& Q, const vec3& u, const vec3& v, shared_ptr<material> mat) 
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
      if (!unit_interval.contains(a) || !unit_interval.contains(b))
        return false;


      rec.u = a;
      rec.v = b;
      return true;

    }

    void translate(const vec3& t_vector) {
      this->Q = Q + t_vector; 

      auto n = cross(u, v);
      this->normal = unit_vector(n);
      this->D = dot(normal, Q);
      this->w = n / dot(n, n);
      set_bounding_box();
    }

    void rotate(double alpha, double beta, double gamma) {
      this->Q = rotate_point(this->Q, gamma, beta, alpha);
      this->u = rotate_vector(this->u, gamma, beta, alpha);
      this->v = rotate_vector(this->v, gamma, beta, alpha);

      auto n = cross(u, v);
      this->normal = unit_vector(n);
      this->D = dot(normal, Q);
      this->w = n / dot(n, n);
      set_bounding_box();
    }

    void rotate_centroid_origin(double alpha, double beta, double gamma) {

      // find the centroid 
      auto x_center = ((this->bbox.x.max - this->bbox.x.min) / 2.0) + this->bbox.x.min;
      auto y_center = ((this->bbox.y.max - this->bbox.y.min) / 2.0) + this->bbox.y.min;
      auto z_center = ((this->bbox.z.max - this->bbox.z.min) / 2.0) + this->bbox.z.min;

      auto centroid = point3(-x_center, -y_center, -z_center);

      translate(centroid);
      rotate(alpha, beta, gamma);
      translate(-centroid);
    }


  private:
    point3 Q;
    vec3 u, v;
    vec3 w;
    shared_ptr<material> mat;
    aabb bbox;
    vec3 normal; 
    double D;

    void print_bounding_box() {
      std::clog << this->bbox.x.max << std::endl;
      std::clog << this->bbox.x.min << std::endl;
      std::clog << this->bbox.y.max << std::endl;
      std::clog << this->bbox.y.min << std::endl;
      std::clog << this->bbox.z.max << std::endl;
      std::clog << this->bbox.z.min << std::endl;

      auto x_center = (this->bbox.x.max - this->bbox.x.min) / 2.0;
      auto y_center = (this->bbox.y.max - this->bbox.y.min) / 2.0;
      auto z_center = (this->bbox.z.max - this->bbox.z.min) / 2.0;

      auto centroid = point3(x_center, y_center, z_center);

      std::clog << centroid.x() << " " << centroid.y() << " " << centroid.z() << std::endl;
    }

};


inline shared_ptr<hittable_list> box(const point3& a, const point3& b, shared_ptr<material> mat) {
  auto sides = make_shared<hittable_list>();

  auto min = point3(std::fmin(a.x(),b.x()), std::fmin(a.y(),b.y()), std::fmin(a.z(),b.z()));
  auto max = point3(std::fmax(a.x(), b.x()), std::fmax(a.y(), b.y()),std::fmax(a.z(), b.z()));

  auto dx = vec3(max.x() - min.x(), 0,0);
  auto dy = vec3(0, max.y() - min.y(), 0);
  auto dz = vec3(0,0,max.z() - min.z());

  auto blue = make_shared<lambertian>(color(0.150,0.01,1.00));
  auto green = make_shared<lambertian>(color(0.150,0.91,0.20));

  sides->add(make_shared<quad>(point3(min.x(), min.y(), max.z()),  dx,  dy, mat)); // front
  sides->add(make_shared<quad>(point3(max.x(), min.y(), max.z()), -dz,  dy, blue)); // right
  sides->add(make_shared<quad>(point3(max.x(), min.y(), min.z()), -dx,  dy, mat)); // back
  sides->add(make_shared<quad>(point3(min.x(), min.y(), min.z()),  dz,  dy, blue)); // left
  sides->add(make_shared<quad>(point3(min.x(), max.y(), max.z()),  dx, -dz, green)); // top
  sides->add(make_shared<quad>(point3(min.x(), min.y(), min.z()),  dx,  dz, green)); // bottom


  return sides;
}

#endif
