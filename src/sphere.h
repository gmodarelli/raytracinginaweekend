#ifndef SPHERE_H
#define SPHERE_H
#include "vec3.h"
#include "ray.h"
#include "aabb.h"

struct Sphere {
  enum Type { Static, Moving };
  Type type;
  vec3 center0;
  vec3 center1;
  float radius;
  float inverse_radius;
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
  float discriminant_squared = b * b - a * c;

  if (discriminant_squared > 0) {
      float discriminant = sqrt(discriminant_squared);

    float temp = (-b - discriminant) / a;
    if (temp < t_max && temp > t_min) {
      rec.t = temp;
      rec.p = ray_point_at_parameter(r, rec.t);
      rec.normal = (rec.p - center) * s.inverse_radius;
      return true;
    }

    temp = (-b + discriminant) / a;
    if (temp < t_max && temp > t_min) {
      rec.t = temp;
      rec.p = ray_point_at_parameter(r, rec.t);
      rec.normal = (rec.p - center) * s.inverse_radius;
      return true;
    }
  }

  return false;
}

bool sphere_bouding_box(const Sphere& s, float t0, float t1, AABB& box) {
    if (s.type == Sphere::Static) {
        box = make_aabb(s.center0 - make_vec3(s.radius, s.radius, s.radius), s.center0 + make_vec3(s.radius, s.radius, s.radius));
    } else if (s.type == Sphere::Moving) {
        AABB box0 = make_aabb(s.center0 - make_vec3(s.radius, s.radius, s.radius), s.center0 + make_vec3(s.radius, s.radius, s.radius));
        AABB box1 = make_aabb(s.center1 - make_vec3(s.radius, s.radius, s.radius), s.center1 + make_vec3(s.radius, s.radius, s.radius));
        box = surrounding_box(box0, box1);
    }

    return true;
}

#endif // SPHERE_H
