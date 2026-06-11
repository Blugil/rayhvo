#ifndef RAYTRACER_H
#define RAYTRACER_H

#include <cmath>
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


// common headers

#include "color.h"
#include "interval.h"
#include "ray.h"
#include "vec3.h"

#endif
