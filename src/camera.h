#ifndef CAMERA_H
#define CAMERA_H

#include "ray.h"
#include "util.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif // M_PI

struct camera {
  vec3 origin;
  vec3 lower_left_corner;
  vec3 horizontal;
  vec3 vertical;
  vec3 u;
  vec3 v;
  vec3 w;
  float lens_radius;
};

vec3 random_in_unit_disk() {
  vec3 p;
  do {
    p = 2.0 * make_vec3(random_float(), random_float(), 0) - make_vec3(1, 1, 0);
  } while (dot(p, p) >= 1.0);

  return p;
}

camera default_camera(vec3 lookfrom, vec3 lookat, vec3 vup, float vfov, float aspect, float aperture, float focus_dist) {
  vec3 u, v, w;
  // Converting degrees to radians
  float theta = vfov * M_PI / 180;
  float half_height = tan(theta / 2);
  float half_width = aspect * half_height;

  w = unit_vector(lookfrom - lookat);
  u = unit_vector(cross(vup, w));
  v = cross(w, u);

  camera cam = {
    lookfrom,
    lookfrom - u * focus_dist * half_width - half_height * focus_dist * v - focus_dist * w,
    2 * half_width * focus_dist * u,
    2 * half_height * focus_dist * v,
    u,
    v,
    w,
    aperture / 2
  };
  
  return cam;
}

ray camera_get_ray(const camera& cam, float s, float t) {
  ray r;

  vec3 rd = cam.lens_radius * random_in_unit_disk();
  vec3 offset = cam.u * rd.x + cam.v * rd.y;

  r.origin = cam.origin + offset;
  r.direction = cam.lower_left_corner + s * cam.horizontal + t * cam.vertical - cam.origin - offset;

  return r;
}
#endif // CAMERA_H
