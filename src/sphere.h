#ifndef SPHERE_H
#define SPHERE_H
#include "vec3.h"
#include "ray.h"

class material;

struct sphere {
  vec3 center;
  float radius;
  material *mat_ptr;
};

bool sphere_hit(const sphere& s, const ray& r, float t_min, float t_max, hit_record& rec) {
  vec3 oc = r.origin - s.center;
  float a = dot(r.direction, r.direction);
  float b = dot(oc, r.direction);
  float c = dot(oc, oc) - s.radius * s.radius;
  float discriminant = b * b - a * c;

  if (discriminant > 0) {
    float temp = (-b - sqrt(discriminant)) / a;
    if (temp < t_max && temp > t_min) {
      rec.t = temp;
      rec.p = ray_point_at_parameter(r, rec.t);
      rec.normal = (rec.p - s.center) / s.radius;
      rec.mat_ptr = s.mat_ptr;
      return true;
    }

    temp = (-b + sqrt(discriminant)) / a;
    if (temp < t_max && temp > t_min) {
      rec.t = temp;
      rec.p = ray_point_at_parameter(r, rec.t);
      rec.normal = (rec.p - s.center) / s.radius;
      rec.mat_ptr = s.mat_ptr;
      return true;
    }
  }

  return false;
}

#endif // SPHERE_H
