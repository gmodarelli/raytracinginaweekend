#ifndef CAMERA_H
#define CAMERA_H

#include "ray.h"

struct camera {
  vec3 origin;
  vec3 lower_left_corner;
  vec3 horizontal;
  vec3 vertical;
};

camera default_camera() {
  camera cam = {
    vec3(0.0, 0.0, 0.0),
    vec3(-2.0, -1.0, -1.0),
    vec3(4.0, 0.0, 0.0),
    vec3(0.0, 2.0, 0.0)
  };
  
  return cam;
}

ray camera_get_ray(const camera& cam, float u, float v) {
  ray r;

  r.origin = cam.origin;
  r.direction = cam.lower_left_corner + u * cam.horizontal + v * cam.vertical - cam.origin;

  return r;
}
#endif // CAMERA_H
