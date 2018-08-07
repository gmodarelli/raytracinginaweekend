#ifndef RAY_H
#define RAY_H
#include "vec3.h"

struct ray {
  vec3 origin;
  vec3 direction;
};

// This function returns a point along the 3D line defined by the ray,
// starting from its m_Origin towards its m_Direction.
// Positive values of t will return points "in front" of the ray, while
// negative values will return points "behind" the ray.
const vec3 ray_point_at_parameter(const ray& ray, float t) {
  return ray.origin + t * ray.direction; 
}

#endif // RAY_H
