#ifndef MATERIAL_H
#define MATERIAL_H

#include "ray.h"
#include "util.h"
#include "texture.h"

vec3 random_point_in_unit_sphere() {
  vec3 p;
  do {
    p = 2.0 * make_vec3(random_float(), random_float(), random_float()) - make_vec3(1, 1, 1);
  } while (squared_length(p) >= 1.0);

  return p;
}

vec3 reflect(const vec3& v, const vec3& n) {
  return v - 2 * dot(v, n) * n;
}

bool refract(const vec3& v, const vec3& n, float ni_over_nt, vec3& refracted) {
  vec3 uv = unit_vector(v);
  float dt = dot(uv, n);
  float discriminant = 1.0 - ni_over_nt * ni_over_nt * (1 - dt * dt);
  if (discriminant > 0) {
    refracted = ni_over_nt * (uv - n * dt) - n * sqrt(discriminant);
    return true;
  }

  return false;
}

float schlick(float cosine, float ref_idx) {
  float r0 = (1 - ref_idx) / (1 + ref_idx);
  r0 = r0 * r0;
  return r0 + (1 - r0) * pow((1 - cosine), 5);
}

struct Material {
  enum Type { Lambert, Metal, Dielectric, Light };
  Type type;
  Texture albedo;
  float ref_idx;
  float fuzz;
};

vec3 material_emit(const Material& mat, float u, float v, const vec3& p) {
    if (mat.type == Material::Light) {
        return texture_value(mat.albedo, u, v, p);
    } else {
        return {0, 0, 0};
    }
}

bool material_scatter(const Material& mat, const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) {
  if (mat.type == Material::Lambert) {
    vec3 target = rec.p + rec.normal + random_point_in_unit_sphere();
    scattered.origin = rec.p;
    scattered.direction = target - rec.p;
    attenuation = texture_value(mat.albedo, rec.u, rec.v, rec.p);

    return true;
  }
  else if (mat.type == Material::Metal) {
    vec3 reflected = reflect(unit_vector(r_in.direction), rec.normal);
    scattered.origin = rec.p;
    scattered.direction = reflected + mat.fuzz * random_point_in_unit_sphere();
    attenuation = texture_value(mat.albedo, rec.u, rec.v, rec.p);
    float result = dot(scattered.direction, rec.normal);
    return (result > 0);
  }
  else if (mat.type == Material::Dielectric) {
    vec3 outward_normal;
    vec3 reflected = reflect(r_in.direction, rec.normal);
    float ni_over_nt;
    attenuation = make_vec3(1.0, 1.0, 1.0);
    vec3 refracted;
    float reflect_prob;
    float cosine;

    if(dot(r_in.direction, rec.normal) > 0) {
      outward_normal = make_vec3(-(rec.normal.x), -(rec.normal.y), -(rec.normal.z));
      ni_over_nt = mat.ref_idx;
      cosine = mat.ref_idx * dot(r_in.direction, rec.normal) / length(r_in.direction);
    }
    else {
      outward_normal = rec.normal;
      ni_over_nt = 1.0 / mat.ref_idx;
      cosine = -dot(r_in.direction, rec.normal) / length(r_in.direction);
    }

    if (refract(r_in.direction, outward_normal, ni_over_nt, refracted)) {
      reflect_prob = schlick(cosine, mat.ref_idx);
    }
    else {
      reflect_prob = 1.0;
    }

    if (random_float() < reflect_prob) {
      scattered.origin = rec.p;
      scattered.direction = reflected;
    }
    else {
      scattered.origin = rec.p;
      scattered.direction = refracted;
    }

    return true;
  }

  return false;
}

#endif // MATERIAL_H
