#include <iostream>
#include "vec3.h"
#include "ray.h"
#include "sphere.h"
#include "float.h"
#include "camera.h"
#include "material.h"

vec3 color(sphere spheres[], int n, const ray& r, int depth) {
  hit_record rec;
  bool hit_anything = false;
  double closest_so_far = MAXFLOAT;
  float t_min = 0.001;
  for (int i = 0; i < n; i++) {
    if(sphere_hit(spheres[i], r, t_min, closest_so_far, rec)) {
      hit_anything = true;
      closest_so_far = rec.t;
    }
  }

  if (hit_anything) {
    ray scattered;
    vec3 attenuation;
    if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
      return attenuation * color(spheres, n, scattered, depth + 1);
    }
    else {
      return vec3(0, 0, 0);
    }
  } else {
    vec3 unit_direction = unit_vector(r.direction);
    float t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
  }
}

int main() {
  int nx = 400;
  int ny = 200;
  // Samples per pixel
  int ns = 100;
  std::cout << "P3\n" << nx << " " << ny << "\n255\n";

  vec3 lookfrom(3, 3, 2);
  vec3 lookat(0, 0, -1);
  float dist_to_focus = (lookfrom - lookat).length();
  float aperture = 2.0;

  camera cam = default_camera(lookfrom, lookat, vec3(0, 1, 0), 20, float(nx) / float(ny), aperture, dist_to_focus);

  sphere spheres[5];
  spheres[0].center = vec3(0, 0, -1);
  spheres[0].radius = 0.5;
  spheres[0].mat_ptr = new lambertian(vec3(0.1, 0.2, 0.5));
  spheres[1].center = vec3(0, -100.5, -1);
  spheres[1].radius = 100;
  spheres[1].mat_ptr = new lambertian(vec3(0.8, 0.8, 0.0));
  spheres[2].center = vec3(1, 0, -1);
  spheres[2].radius = 0.5;
  spheres[2].mat_ptr = new metal(vec3(0.8, 0.6, 0.2), 0.0);
  spheres[3].center = vec3(-1, 0, -1);
  spheres[3].radius = 0.5;
  spheres[3].mat_ptr = new dielectric(1.5);
  spheres[4].center = vec3(-1, 0, -1);
  spheres[4].radius = -0.45;
  spheres[4].mat_ptr = new dielectric(1.5);


  for (int j = ny - 1; j >= 0; j--) {
    for (int i = 0; i < nx; i++) {
      vec3 col(0, 0, 0);
      for (int s = 0; s < ns; s++) {
        float u = float(i + drand48()) / float(nx);
        float v = float(j + drand48()) / float(ny);
        ray r = camera_get_ray(cam, u, v);
        col += color(spheres, 5, r, 0);
      }

      col /= float(ns);
      // Gamma correction
      col = vec3(sqrt(col.r()), sqrt(col.g()), sqrt(col.b()));

      int ir = int(255.99 * col.r());
      int ig = int(255.99 * col.g());
      int ib = int(255.99 * col.b());

      std::cout << ir << " " << ig << " " << ib << "\n";
    }
  }
}
