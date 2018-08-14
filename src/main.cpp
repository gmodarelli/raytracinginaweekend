#include <iostream>
#include "vec3.h"
#include "ray.h"
#include "sphere.h"
#include "float.h"
#include "camera.h"
#include "material.h"
#include "texture.h"
#include "util.h"
#include <limits>
#include <chrono>

const int max_depth = 10;
const int samples_per_pixel = 2;

vec3 trace(Sphere spheres[], Material materials[], int n, const ray& r, int depth, int& inoutRaycount) {
    ++inoutRaycount;
  hit_record rec;
  bool hit_anything = false;
  float closest_so_far = std::numeric_limits<float>::max();
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
    if (depth < max_depth && scatter(materials[sphere_hit_id], r, rec, attenuation, scattered)) {
      return attenuation * trace(spheres, materials, n, scattered, depth + 1, inoutRaycount);
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

void random_scene(int& spheres_count, Sphere* spheres, Material* materials, Texture* odd, Texture* even) {
  spheres[0].center0 = make_vec3(0, -1000, 0);
  spheres[0].radius = 1000;
  spheres[0].type = Sphere::Static;
  materials[0].type = Material::Lambert;
  materials[0].albedo = {Texture::Checker, make_vec3(0.0, 0.0, 0.0), odd, even};
  int max = 1;

  int i = 1;
  // TODO: find a better name
  vec3 limit = make_vec3(4, 0.2, 0);
  for (int a = -max; a < max; a++) {
    for (int b = -max; b < max; b++) {
      float choose_mat = random_float();
      vec3 center = make_vec3(a + 0.9 * random_float(), 0.2, b + 0.9 * random_float());
      if (length(center - limit) > 0.9) {
        i += 1;
        spheres[i].center0 = center;
        spheres[i].radius = 0.2;

        if (choose_mat < 0.8) { // diffuse
          spheres[i].type = Sphere::Moving;
          spheres[i].center1 = center + make_vec3(0, 0.5 * random_float(), 0);
          spheres[i].time0 = 0.0;
          spheres[i].time1 = 1.0;
          materials[i].type = Material::Lambert;
          materials[i].albedo = {Texture::Constant, make_vec3(random_float() * random_float(), random_float() * random_float(), random_float() * random_float()), nullptr, nullptr};
        }
        else if (choose_mat < 0.95) { // metal
          spheres[i].type = Sphere::Static;
          materials[i].type = Material::Metal;
          materials[i].albedo = {Texture::Constant, make_vec3(0.5 * (1.0 + random_float()), 0.5 * (1.0 + random_float()), 0.5 * (1.0 + random_float())), nullptr, nullptr};
          materials[i].fuzz = 0.5 * random_float();
        }
        else {
          spheres[i].type = Sphere::Static;
          materials[i].type = Material::Dielectric;
          materials[i].ref_idx = 1.5; 
        }
      }
    }
  }

  i += 1;
  spheres[i].center0 = make_vec3(0, 1, 0);
  spheres[i].radius = 1.0;
  spheres[i].type = Sphere::Static;
  materials[i].type = Material::Dielectric;
  materials[i].ref_idx = 1.5;

  i += 1;
  spheres[i].center0 = make_vec3(-4, 1, 0);
  spheres[i].radius = 1.0;
  spheres[i].type = Sphere::Static;
  materials[i].type = Material::Lambert;
  materials[i].albedo = {Texture::Constant, make_vec3(0.4, 0.2, 0.1), nullptr, nullptr};

  i += 1;
  spheres[i].center0 = make_vec3(4, 1, 0);
  spheres[i].radius = 1.0;
  spheres[i].type = Sphere::Static;
  materials[i].type = Material::Metal;
  materials[i].albedo = {Texture::Constant, make_vec3(0.7, 0.6, 0.5), nullptr, nullptr};
  materials[i].fuzz = 0.0;
  
  spheres_count = i;
}

int main() {
  int nx = 400;
  int ny = 200;

  std::cout << "P3\n" << nx << " " << ny << "\n255\n";

  vec3 lookfrom = {13, 2, 3};
  vec3 lookat = {0, 0, 0};
  vec3 up = {0, 1, 0};
  float dist_to_focus = 10.0;
  float aperture = 0.1;

  camera cam = default_camera(lookfrom, lookat, up, 20, float(nx) / float(ny), aperture, dist_to_focus, 0.0, 1.0);

  int spheres_count = 50;
  Sphere spheres[51];
  Material materials[51];

  Texture texture_odd = { Texture::Constant, make_vec3(0.2, 0.3, 0.1), nullptr, nullptr };
  Texture texture_even = { Texture::Constant, make_vec3(0.9, 0.9, 0.9), nullptr, nullptr };
  auto scene_t0 = std::chrono::high_resolution_clock::now();

  random_scene(spheres_count, spheres, materials, &texture_odd, &texture_even);

  auto scene_t1 = std::chrono::high_resolution_clock::now();

  auto scene_duration = std::chrono::duration_cast<std::chrono::microseconds>(scene_t1 - scene_t0);
  std::cerr << "Scene built in " << scene_duration.count() << "us" << std::endl;

  int rayCount = 0;
  auto t0 = std::chrono::high_resolution_clock::now();

  for (int j = ny - 1; j >= 0; j--) {
    for (int i = 0; i < nx; i++) {
      vec3 tmp_col = {0, 0, 0};
      for (int s = 0; s < samples_per_pixel; s++) {
        float u = float(i + random_float()) / float(nx);
        float v = float(j + random_float()) / float(ny);
        ray r = camera_get_ray(cam, u, v);
        tmp_col = tmp_col + trace(spheres, materials, spheres_count + 1, r, 0, rayCount);
      }

      tmp_col = tmp_col / float(samples_per_pixel);
      // Gamma correction
      vec3 col = {sqrt(tmp_col.x), sqrt(tmp_col.y), sqrt(tmp_col.z)};

      int ir = int(255.99 * col.x);
      int ig = int(255.99 * col.y);
      int ib = int(255.99 * col.z);

      std::cout << ir << " " << ig << " " << ib << "\n";
    }
  }

  auto t1 = std::chrono::high_resolution_clock::now();

  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);
  std::cerr << "Traced rays: " << rayCount << std::endl;
  std::cerr << "Duration: " << duration.count() << "ms" << std::endl;
  std::cerr << "MRays/s: " << (rayCount / duration.count() *  1.0e-3f) << std::endl;
}
