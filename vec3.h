#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <iostream>
#include "raytracer.h"

class vec3 {
  public:
    double e[3];

    vec3() : e{0,0,0} {}
    vec3(double e0, double e1, double e2) : e{e0, e1, e2} {}

    double x() const { return e[0]; }
    double y() const { return e[1]; }
    double z() const { return e[2]; }


    vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
    double operator[](int i) const {return e[i]; }
    double& operator[](int i) { return e[i]; }

    vec3& operator+=(const vec3& v) {
      e[0] += v.e[0];
      e[1] += v.e[1];
      e[2] += v.e[2];
      return *this; 
    }

    vec3& operator*=(double t) {
      e[0] *= t;
      e[1] *= t;
      e[2] *= t;
      return *this; 
    }

    vec3& operator/=(double t) {
      return *this *= (1/t);
    }

    double length() const {
      return std::sqrt(length_squared());
    }

    double length_squared() const {
      return (e[0] * e[0]) + (e[1] * e[1]) + (e[2] * e[2]);
    }

    bool near_zero() const {
      auto s = 1e-8;
      return (std::fabs(e[0]) < s) &&(std::fabs(e[1]) < s) && (std::fabs(e[2]) < s);
    }

    static vec3 random() {
      return vec3(random_double(), random_double(), random_double());
    }

    static vec3 random(double min, double max) {
      return vec3(random_double(min,max), random_double(min,max), random_double(min,max));
    }
};

using point3 = vec3;


// vector utility functions

//for printing out vectors to the out stream for the ppm format
inline std::ostream& operator<<(std::ostream& out, const vec3& v) {
  return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

inline vec3 operator+(const vec3& u, const vec3& v) {
  return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

inline vec3 operator-(const vec3& u, const vec3& v) {
  return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline vec3 operator*(const vec3& u, const vec3& v) {
  return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

inline vec3 operator*(double t, const vec3& u) {
  return vec3(u.e[0] * t, u.e[1] * t, u.e[2] * t);
}

inline vec3 operator*(const vec3& u, double t) {
  return t * u;
}

inline vec3 operator/(const vec3& u, double t) {
  // classic method of reducing cpu division time
  // reduces cycle count of division utility operations
  return (1/t) * u;
}

inline double dot(const vec3& u, const vec3& v) {
  return u.e[0] * v.e[0] + u.e[1] * v.e[1] + u.e[2] * v.e[2];
}

inline vec3 cross(const vec3& u, const vec3& v) {
  return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
              u.e[2] * v.e[0] - u.e[0] * v.e[2], 
              u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

inline vec3 unit_vector(const vec3& u) {
  return u / u.length();
}

inline vec3 random_in_unit_disk() {
  while (true) {
    auto p = vec3(random_double(-1,1), random_double(-1, 1), 0);
    if (p.length_squared() < 1) {
      return p;
    }
  }
}

inline vec3 random_unit_vector() {
  while (true) {
    auto p = vec3::random(-1, 1);
    auto len_sq = p.length_squared();
    if (1e-160 < len_sq && len_sq <= 1) {
      return p / sqrt(len_sq);
    }
  }
}

inline vec3 random_on_hemisphere(const vec3& normal) {
  vec3 on_unit_sphere = random_unit_vector();
  if (dot(on_unit_sphere, normal) > 0.0) {
    return on_unit_sphere;
  }
  else {
    return -on_unit_sphere;
  }
}

inline vec3 reflect(const vec3& v, const vec3&n) {
  return v - 2*dot(v,n)*n;
}

inline vec3 refract(const vec3& uv, const vec3& n, double etai_over_etat) {
  auto cos_theta = std::fmin(dot(-uv, n), 1.0);
  vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
  vec3 r_out_parallel = -std::sqrt(std::fabs(1.0 - r_out_perp.length_squared())) * n;
  return r_out_perp + r_out_parallel;
}

inline vec3 rotate_vector(vec3& u, double alpha, double beta, double gamma) {

  auto alpha_r = degrees_to_radians(alpha);
  auto beta_r = degrees_to_radians(beta);
  auto gamma_r = degrees_to_radians(gamma);

  auto cos_a = std::cos(alpha_r);
  auto cos_b = std::cos(beta_r);
  auto cos_g = std::cos(gamma_r);

  auto sin_a = std::sin(alpha_r);
  auto sin_b = std::sin(beta_r);
  auto sin_g = std::sin(gamma_r);


  auto x = ((cos_a * cos_b) * u.x()) 
    + ((cos_a * sin_b * sin_g - sin_a * cos_g) * u.y())
    + ((cos_a * sin_b * cos_g + sin_a * sin_g) * u.z());

  auto y = ((sin_a * cos_b) * u.x()) 
    + ((sin_a * sin_b * sin_g + cos_a * cos_g) * u.y())
    + ((sin_a * sin_b * cos_g - cos_a * sin_g) * u.z());

  auto z = ((-sin_b) * u.x())
    + ((cos_b * sin_g) * u.y())
    + ((cos_b * cos_g) * u.z());


  return vec3(x, y, z);
}

inline point3 rotate_point(point3& p, double alpha, double beta, double gamma) {

  auto alpha_r = degrees_to_radians(alpha);
  auto beta_r = degrees_to_radians(beta);
  auto gamma_r = degrees_to_radians(gamma);

  auto cos_a = std::cos(alpha_r);
  auto cos_b = std::cos(beta_r);
  auto cos_g = std::cos(gamma_r);

  auto sin_a = std::sin(alpha_r);
  auto sin_b = std::sin(beta_r);
  auto sin_g = std::sin(gamma_r);

  auto x = ((cos_a * cos_b) * p.x()) 
    + ((cos_a * sin_b * sin_g - sin_a * cos_g) * p.y())
    + ((cos_a * sin_b * cos_g + sin_a * sin_g) * p.z());

  auto y = ((sin_a * cos_b) * p.x()) 
    + ((sin_a * sin_b * sin_g + cos_a * cos_g) * p.y())
    + ((sin_a * sin_b * cos_g - cos_a * sin_g) * p.z());

  auto z = ((-sin_b) * p.x())
    + ((cos_b * sin_g) * p.y())
    + ((cos_b * cos_g) * p.z());


  return point3(x, y, z);
}



#endif
