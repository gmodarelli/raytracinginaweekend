#ifndef AABB_H
#define AABB_H

#include "vec3.h"
#include "ray.h"

struct AABB {
    vec3 min;
    vec3 max;
};

AABB make_aabb(vec3 min, vec3 max) {
    AABB box;
    box.min = min;
    box.max = max;

    return box;
}

inline bool aabb_hit_coord(float min, float max, float origin, float direction, float tmin, float tmax) {
    float invD = 1.0 / direction;
    float t0 = (min - origin) * invD;
    float t1 = (max - origin) * invD;

    if (invD < 0.0f) std::swap(t0, t1);

    tmin = t0 > tmin ? t0 : tmin;
    tmax = t1 < tmax ? t1 : tmax;

    if (tmax <= tmin) return false;

    return true;
}

bool aabb_hit(const AABB& aabb, const ray& r, float tmin, float tmax) {
    if(!aabb_hit_coord(aabb.min.x, aabb.max.x, r.origin.x, r.direction.x, tmin, tmax)) return false;
    if(!aabb_hit_coord(aabb.min.y, aabb.max.y, r.origin.y, r.direction.y, tmin, tmax)) return false;
    if(!aabb_hit_coord(aabb.min.z, aabb.max.z, r.origin.z, r.direction.z, tmin, tmax)) return false;

    return true;
}

AABB surrounding_box(AABB box0, AABB box1) {
    vec3 small = make_vec3(fmin(box0.min.x, box1.min.x),
                            fmin(box0.min.y, box1.min.y),
                            fmin(box0.min.z, box1.min.z));

    vec3 big = make_vec3(fmax(box0.max.x, box1.max.x),
                            fmax(box0.max.y, box1.max.y),
                            fmax(box0.max.z, box1.max.z));

    return make_aabb(small, big);
};

#endif // AABB_H
