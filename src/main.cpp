#include <iostream>
#include "vec3.h"
#include "ray.h"
#include "sphere.h"
#include "float.h"
#include "camera.h"

vec3 random_point_in_unit_sphere() {
  vec3 p;
  do {
    p = 2.0 * vec3(drand48(), drand48(), drand48()) - vec3(1, 1, 1);
  } while (p.squared_length() >= 1.0);

  return p;
}

vec3 color(sphere spheres[], int n, const ray& r) {
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
    vec3 target = rec.p + rec.normal + random_point_in_unit_sphere();
    ray reflected_ray = {
      rec.p,
      target - rec.p
    };

    return 0.5 * color(spheres, n, reflected_ray);
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

  camera cam = default_camera();

  sphere spheres[2];
  spheres[0].center = vec3(0, 0, -1);
  spheres[0].radius = 0.5;
  spheres[1].center = vec3(0, -100.5, -1);
  spheres[1].radius = 100;

  for (int j = ny - 1; j >= 0; j--) {
    for (int i = 0; i < nx; i++) {
      vec3 col(0, 0, 0);
      for (int s = 0; s < ns; s++) {
        float u = float(i + drand48()) / float(nx);
        float v = float(j + drand48()) / float(ny);
        ray r = camera_get_ray(cam, u, v);
        col += color(spheres, 2, r);
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
