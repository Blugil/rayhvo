#ifndef RAYTRACER_H
#define RAYTRACER_H

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <memory>


// using statements

using std::make_shared;
using std::shared_ptr;


// constants

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535898;

// utility

inline double degrees_to_radians(double degrees) {
  return degrees * pi / 180;
}


inline double random_double() {
  return std::rand() / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max) {
  return min + (max-min)*random_double();
}

inline int random_int(int min, int max) {
  return int(random_double(min, max+1));
}


// common headers

#include "color.h"
#include "interval.h"
#include "ray.h"
#include "vec3.h"

#endif
