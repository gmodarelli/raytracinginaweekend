#ifndef MATERIAL_H
#define MATERIAL_H

#include "ray.h"


vec3 random_point_in_unit_sphere() {
  vec3 p;
  do {
    p = 2.0 * vec3(drand48(), drand48(), drand48()) - vec3(1, 1, 1);
  } while (p.squared_length() >= 1.0);

  return p;
}

// TODO: Replace abstract class and virtual function with a struct somehow

class material {
  public:
    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) = 0;
};

class lambertian : public material {
  public:
    lambertian(const vec3& a) : albedo(a) {}

    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) {
      vec3 target = rec.p + rec.normal + random_point_in_unit_sphere();
      scattered.origin = rec.p;
      scattered.direction = target - rec.p;
      attenuation = albedo;
      return true;
    }

    vec3 albedo;
};

#endif // MATERIAL_H
