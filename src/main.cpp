#include <iostream>
#include "vec3.h"
#include "ray.h"
#include "sphere.h"
#include "float.h"
#include "camera.h"
#include "material.h"
#include "util.h"

vec3 color(sphere spheres[], Material materials[], int n, const ray& r, int depth) {
  hit_record rec;
  bool hit_anything = false;
  double closest_so_far = MAXFLOAT;
  float t_min = 0.001;
  int sphere_hit_id;
  for (int i = 0; i < n; i++) {
    if(sphere_hit(spheres[i], r, t_min, closest_so_far, rec)) {
      hit_anything = true;
      closest_so_far = rec.t;
      sphere_hit_id = i;
    }
  }

  if (hit_anything) {
    ray scattered;
    vec3 attenuation;
    if (depth < 50 && scatter(materials[sphere_hit_id], r, rec, attenuation, scattered)) {
      return attenuation * color(spheres, materials, n, scattered, depth + 1);
    }
    else {
      vec3 black = {0, 0, 0};
      return black;
    }
  } else {
    vec3 unit_direction = unit_vector(r.direction);
    float t = 0.5 * (unit_direction.y + 1.0);
    vec3 white = {1.0, 1.0, 1.0};
    vec3 blue = {0.5, 0.7, 1.0};
    return (1.0 - t) * white + t * blue;
  }
}

void random_scene(int& spheres_count, sphere* spheres, Material* materials) {
  spheres[0].center = make_vec3(0, -1000, 0);
  spheres[0].radius = 1000;
  materials[0].type = Material::Lambert;
  materials[0].albedo = make_vec3(0.5, 0.5, 0.5);
  int max = 3;

  int i = 1;
  // TODO: find a better name
  vec3 limit = make_vec3(4, 0.2, 0);
  for (int a = -max; a < max; a++) {
    for (int b = -max; b < max; b++) {
      float choose_mat = random_float();
      vec3 center = make_vec3(a + 0.9 * random_float(), 0.2, b + 0.9 * random_float());
      if (length(center - limit) > 0.9) {
        i += 1;
        spheres[i].center = center;
        spheres[i].radius = 0.2;

        if (choose_mat < 0.8) { // diffuse
          materials[i].type = Material::Lambert;
          materials[i].albedo = make_vec3(random_float() * random_float(), random_float() * random_float(), random_float() * random_float());
        }
        else if (choose_mat < 0.95) { // metal
          materials[i].type = Material::Metal;
          materials[i].albedo = make_vec3(0.5 * (1.0 + random_float()), 0.5 * (1.0 + random_float()), 0.5 * (1.0 + random_float()));
          materials[i].fuzz = 0.5 * random_float();
        }
        else {
          materials[i].type = Material::Dielectric;
          materials[i].ref_idx = 1.5; 
        }
      }
    }
  }

  i += 1;
  spheres[i].center = make_vec3(0, 1, 0);
  spheres[i].radius = 1.0;
  materials[i].type = Material::Dielectric;
  materials[i].ref_idx = 1.5;

  i += 1;
  spheres[i].center = make_vec3(-4, 1, 0);
  spheres[i].radius = 1.0;
  materials[i].type = Material::Lambert;
  vec3 albedo = {0.4, 0.2, 0.1};
  materials[i].albedo = albedo;

  i += 1;
  spheres[i].center = make_vec3(4, 1, 0);
  spheres[i].radius = 1.0;
  materials[i].type = Material::Metal;
  materials[i].albedo = make_vec3(0.7, 0.6, 0.5);
  materials[i].fuzz = 0.0;
  
  spheres_count = i;
}

int main() {
  int nx = 400;
  int ny = 200;
  // Samples per pixel
  int ns = 20;
  std::cout << "P3\n" << nx << " " << ny << "\n255\n";

  vec3 lookfrom = {13, 2, 3};
  vec3 lookat = {0, 0, 0};
  vec3 up = {0, 1, 0};
  float dist_to_focus = 10.0;
  float aperture = 0.1;

  camera cam = default_camera(lookfrom, lookat, up, 20, float(nx) / float(ny), aperture, dist_to_focus);

  int spheres_count = 50;
  sphere spheres[spheres_count + 1];
  Material materials[spheres_count + 1];
  random_scene(spheres_count, spheres, materials);

  for (int j = ny - 1; j >= 0; j--) {
    for (int i = 0; i < nx; i++) {
      vec3 tmp_col = {0, 0, 0};
      for (int s = 0; s < ns; s++) {
        float u = float(i + random_float()) / float(nx);
        float v = float(j + random_float()) / float(ny);
        ray r = camera_get_ray(cam, u, v);
        tmp_col = tmp_col + color(spheres, materials, spheres_count + 1, r, 0);
      }

      tmp_col = tmp_col / float(ns);
      // Gamma correction
      vec3 col = {sqrt(tmp_col.x), sqrt(tmp_col.y), sqrt(tmp_col.z)};

      int ir = int(255.99 * col.x);
      int ig = int(255.99 * col.y);
      int ib = int(255.99 * col.z);

      std::cout << ir << " " << ig << " " << ib << "\n";
    }
  }
}
