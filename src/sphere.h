#ifndef SPHERE_H
#define SPHERE_H
#include "vec3.h"
#include "ray.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif // M_PI

struct Spheres {
    vec3* center;
    float* radius;
    float* inverse_radius;
};

Spheres SpheresInit(int count) {
    Spheres spheres;
    spheres.center = new vec3[count];
    spheres.radius = new float[count];
    spheres.inverse_radius = new float[count];

    return spheres;
}

void sphere_uv(const vec3& normal, float& outU, float& outV) {
    float phi = atan2(normal.z, normal.x);
    float theta = asin(normal.y);
    outU = 1 - (phi + M_PI) / (2 * M_PI);
    outV = (theta + M_PI / 2) / M_PI;
}

bool hit_spheres(const Spheres& spheres, int spheres_count, const ray& r, float t_min, float t_max, hit_record& rec, int& outId) {
    int id = -1;
    float closest_so_far = t_max;

    for (int i = 0; i < spheres_count; i++) {
        vec3 oc = r.origin - spheres.center[i];
        float a = dot(r.direction, r.direction);
        float b = dot(oc, r.direction);
        float c = dot(oc, oc) - spheres.radius[i] * spheres.radius[i];
        float discriminant_squared = b * b - a * c;

        if (discriminant_squared > 0) {
            float discriminant = sqrt(discriminant_squared);

            float t = (-b - discriminant) / a;
            if (t <= t_min) {
                t = (-b + discriminant) / a;
            }

            if (t < closest_so_far && t > t_min) {
                closest_so_far = t;
                id = i;
            }
        }
    }

    if (id != -1) {
        rec.t = closest_so_far;
        rec.p = ray_point_at_parameter(r, rec.t);
        rec.normal = (rec.p - spheres.center[id]) * spheres.inverse_radius[id];
        sphere_uv(rec.normal, rec.u, rec.v);
        outId = id;
        return true;
    }

    return false;
}
#endif // SPHERE_H
