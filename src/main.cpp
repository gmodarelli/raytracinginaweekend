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

void random_scene(int& spheres_count, sphere* spheres) {
  spheres[0].center = vec3(0, -1000, 0);
  spheres[0].radius = 1000;
  spheres[0].mat_ptr = new lambertian(vec3(0.5, 0.5, 0.5));

  int i = 1;
  for (int a = -11; a < 11; a++) {
    for (int b = -11; b < 11; b++) {
      float choose_mat = drand48();
      vec3 center(a + 0.9 * drand48(), 0.2, b + 0.9 * drand48());
      if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
        i += 1;
        spheres[i].center = center;
        spheres[i].radius = 0.2;

        if (choose_mat < 0.8) { // diffuse
          spheres[i].mat_ptr = new lambertian(vec3(drand48() * drand48(), drand48() * drand48(), drand48() * drand48()));
        }
        else if (choose_mat < 0.95) { // metal
          spheres[i].mat_ptr = new metal(vec3(0.5 * (1 + drand48()), 0.5 * (1 + drand48()), 0.5 * (1 + drand48())), 0.5 * drand48());
        }
        else {
          spheres[i].mat_ptr = new dielectric(1.5);
        }
      }
    }
  }

  i += 1;
  spheres[i].center = vec3(0, 1, 0);
  spheres[i].radius = 1.0;
  spheres[i].mat_ptr = new dielectric(1.5);

  i += 1;
  spheres[i].center = vec3(-4, 1, 0);
  spheres[i].radius = 1.0;
  spheres[i].mat_ptr = new lambertian(vec3(0.4, 0.2, 0.1));

  i += 1;
  spheres[i].center = vec3(4, 1, 0);
  spheres[i].radius = 1.0;
  spheres[i].mat_ptr = new metal(vec3(0.7, 0.6, 0.5), 0.0);
  
  spheres_count = i;
}

int main() {
  int nx = 1200;
  int ny = 800;
  // Samples per pixel
  int ns = 20;
  std::cout << "P3\n" << nx << " " << ny << "\n255\n";

  vec3 lookfrom(13, 2, 3);
  vec3 lookat(0, 0, 0);
  float dist_to_focus = 10.0;
  float aperture = 0.1;

  camera cam = default_camera(lookfrom, lookat, vec3(0, 1, 0), 20, float(nx) / float(ny), aperture, dist_to_focus);

  int spheres_count = 500;
  sphere spheres[spheres_count + 1];
  random_scene(spheres_count, spheres);

  for (int j = ny - 1; j >= 0; j--) {
    for (int i = 0; i < nx; i++) {
      vec3 col(0, 0, 0);
      for (int s = 0; s < ns; s++) {
        float u = float(i + drand48()) / float(nx);
        float v = float(j + drand48()) / float(ny);
        ray r = camera_get_ray(cam, u, v);
        col += color(spheres, spheres_count, r, 0);
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
