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

vec3 reflect(const vec3& v, const vec3& n) {
  return v - 2 * dot(v, n) * n;
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

class metal : public material {
  public:
    metal(const vec3& a, float f) : albedo(a) {
      if (f < 1) fuzz = f;
      else fuzz = 1;
    }

    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) {
      vec3 reflected = reflect(unit_vector(r_in.direction), rec.normal);
      scattered.origin = rec.p;
      scattered.direction = reflected + fuzz * random_point_in_unit_sphere();
      attenuation = albedo;

      return (dot(scattered.direction, rec.normal) > 0);
    }

    vec3 albedo;
    float fuzz;
};

#endif // MATERIAL_H
