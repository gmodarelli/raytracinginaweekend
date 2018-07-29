#ifndef RAY_H
#define RAY_H
#include "vec3.h"

class ray
{
  public:
    ray() {}
    ray(const vec3& origin, const vec3& direction) {
      m_Origin = origin; 
      m_Direction = direction;
    }

    vec3 origin() const { return m_Origin; }
    vec3 direction() const { return m_Direction; }
    // This function returns a point along the 3D line defined by the ray,
    // starting from its m_Origin towards its m_Direction.
    // Positive values of t will return points "in front" of the ray, while
    // negative values will return points "behind" the ray.
    vec3 point_at_parameter(float t) const { return m_Origin + t * m_Direction; }

    vec3 m_Origin;
    vec3 m_Direction;
};
#endif // RAY_H
