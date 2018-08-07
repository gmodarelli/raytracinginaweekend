#ifndef VEC3_H
#define VEC3_H

#include <math.h>
#include <stdlib.h>
#include <iostream>

class vec3 {
  public:
    // Constructors
    vec3() {}
    vec3(float e0, float e1, float e2) { e[0] = e0; e[1] = e1; e[2] = e2; }

    // Position and color components getters
    inline float x() const { return e[0]; }
    inline float y() const { return e[1]; }
    inline float z() const { return e[2]; }
    inline float r() const { return e[0]; }
    inline float g() const { return e[1]; }
    inline float b() const { return e[2]; }

    inline vec3& operator+=(const vec3 &v2);
    inline vec3& operator/=(const float t);

    inline float length() const { return sqrt(e[0] * e[0] + e[1] * e[1] + e[2] * e[2]); }
    inline float squared_length() const { return e[0] * e[0] + e[1] * e[1] + e[2] * e[2]; }
    // TODO: Why is this function declared here
    // and defined down in the same file?
    inline void make_unit_vector();

    float e[3];
};

inline void vec3::make_unit_vector() {
  float k = 1.0 / length();
  e[0] *= k;
  e[1] *= k;
  e[2] *= k;
}

inline vec3 operator+(const vec3 &v1, const vec3 &v2) {
  return vec3(v1.e[0] + v2.e[0], v1.e[1] + v2.e[1], v1.e[2] + v2.e[2]);
}

inline vec3 operator-(const vec3 &v1, const vec3 &v2) {
  return vec3(v1.e[0] - v2.e[0], v1.e[1] - v2.e[1], v1.e[2] - v2.e[2]);
}

inline vec3 operator*(const vec3 &v1, const vec3 &v2) {
  return vec3(v1.e[0] * v2.e[0], v1.e[1] * v2.e[1], v1.e[2] * v2.e[2]);
}

inline vec3 operator/(const vec3 &v1, const vec3 &v2) {
  return vec3(v1.e[0] / v2.e[0], v1.e[1] / v2.e[1], v1.e[2] / v2.e[2]);
}

inline vec3 operator*(const vec3 &v, const float t) {
  return vec3(v.e[0] * t, v.e[1] * t, v.e[2] * t);
}

inline vec3 operator*(const float t, const vec3 &v) {
  return vec3(v.e[0] * t, v.e[1] * t, v.e[2] * t);
}

inline vec3 operator/(const vec3 &v, const float t) {
  return vec3(v.e[0] / t, v.e[1] / t, v.e[2] / t);
}

inline vec3& vec3::operator+=(const vec3 &v){
    e[0]  += v.e[0];
    e[1]  += v.e[1];
    e[2]  += v.e[2];
    return *this;
}

inline vec3& vec3::operator/=(const float t) {
    float k = 1.0/t;
    
    e[0]  *= k;
    e[1]  *= k;
    e[2]  *= k;
    return *this;
}

inline float dot(const vec3 &v1, const vec3 &v2) {
  return v1.e[0] * v2.e[0] + v1.e[1] * v2.e[1] + v1.e[2] * v2.e[2];
}

inline vec3 cross(const vec3 &v1, const vec3 &v2) {
  return vec3( (v1.e[1] * v2.e[2] - v1.e[2] * v2.e[1]),
              -(v1.e[0] * v2.e[2] - v1.e[2] * v2.e[0]),
               (v1.e[0] * v2.e[1] - v1.e[1] * v2.e[0]) );
}

inline vec3 unit_vector(vec3 v) {
  return v / v.length();
}

inline std::istream& operator<<(std::istream &is, vec3 &v) {
  is >> v.e[0] >> v.e[1] >> v.e[2];
  return is;
}

inline std::ostream& operator<<(std::ostream &os, const vec3 &v) {
  os << v.e[0] << " " << v.e[1] << " " << v.e[2];
  return os;
}

#endif // VEC3_H
