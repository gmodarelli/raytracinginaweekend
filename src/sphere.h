#ifndef SPHERE_H
#define SPHERE_H
#include "vec3.h"
#include "ray.h"

struct Sphere {
  enum Type { Static, Moving };
  Type type;
  vec3 center0;
  vec3 center1;
  float radius;
  float time0;
  float time1;
};


vec3 moving_sphere_center(const Sphere& sphere, float time) {
  return sphere.center0 + ((time - sphere.time0) / (sphere.time1 - sphere.time0)) * (sphere.center1 - sphere.center0);
}

bool sphere_hit(const Sphere& s, const ray& r, float t_min, float t_max, hit_record& rec) {
  vec3 center;

  if (s.type == Sphere::Static) {
    center = s.center0;
  } else if (s.type == Sphere::Moving) {
    center = moving_sphere_center(s, r.time);
  }

  vec3 oc = r.origin - center;
  float a = dot(r.direction, r.direction);
  float b = dot(oc, r.direction);
  float c = dot(oc, oc) - s.radius * s.radius;
  float discriminant = b * b - a * c;

  if (discriminant > 0) {
    float temp = (-b - sqrt(discriminant)) / a;
    if (temp < t_max && temp > t_min) {
      rec.t = temp;
      rec.p = ray_point_at_parameter(r, rec.t);
      rec.normal = (rec.p - center) / s.radius;
      return true;
    }

    temp = (-b + sqrt(discriminant)) / a;
    if (temp < t_max && temp > t_min) {
      rec.t = temp;
      rec.p = ray_point_at_parameter(r, rec.t);
      rec.normal = (rec.p - center) / s.radius;
      return true;
    }
  }

  return false;
}

#endif // SPHERE_H
