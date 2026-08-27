#ifndef COLOR_H
#define COLOR_H

#include "interval.h"
#include "vec3.h"

using color = vec3;

inline double linear_to_gamma(double linear_component) {
  if (linear_component > 0) {
    return std::sqrt(linear_component);
  }

  return 0;
}

void write_color(std::ostream& out, const color& pixel_color) {
  // should never get here without already being in a normalized pixel channel format
  // since it takes binary pixel data from the below function
  uint32_t r = (uint8_t)pixel_color.x();
  uint32_t g = (uint8_t)pixel_color.y();
  uint32_t b = (uint8_t)pixel_color.z();
  
  out << r << ' ' << g << ' ' << b << '\n';
}

// might be useful later
uint32_t pack_color(const color pixel_color) {

  auto r = pixel_color.x();
  auto g = pixel_color.y();
  auto b = pixel_color.z();

  r = linear_to_gamma(r);
  g = linear_to_gamma(g);
  b = linear_to_gamma(b);

  static const interval intensity(0.000, 0.999);
  int rbyte = int(256 * intensity.clamp(r));
  int gbyte = int(256 * intensity.clamp(g));
  int bbyte = int(256 * intensity.clamp(b));

  //std::cout << "\npacked byte values: " << std::hex << rbyte << gbyte << bbyte << std::endl;

  uint32_t pack = 0;
  pack ^= ((rbyte << 16) & 0xFF0000);
  pack ^= (gbyte << 8) & 0xFF00;
  pack ^= (bbyte & 0xFF);
  pack &= 0xFFFFFF;

  //std::cout << "packed byte: " << std::hex << pack << std::endl;
  return pack;
}


#endif
