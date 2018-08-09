#ifndef VEC3_H
#define VEC3_H

#include <math.h>
#include <stdlib.h>
#include <iostream>

struct vec3 {
  float x;
  float y;
  float z;
};

vec3 make_vec3(float x, float y, float z) {
  vec3 vec = {x, y, z};
  return vec;
}

inline float length(vec3 v) {
  return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

inline float squared_length(vec3 v) {
  return v.x * v.x + v.y * v.y + v.z * v.z;
}

inline vec3 operator+(const vec3 &v1, const vec3 &v2) {
  return {v1.x + v2.x, v1.y + v2.y, v1.z + v2.z};
}

inline vec3 operator-(const vec3 &v1, const vec3 &v2) {
  return {v1.x - v2.x, v1.y - v2.y, v1.z - v2.z};
}

inline vec3 operator*(const vec3 &v1, const vec3 &v2) {
  return {v1.x * v2.x, v1.y * v2.y, v1.z * v2.z};
}

inline vec3 operator/(const vec3 &v1, const vec3 &v2) {
  return {v1.x / v2.x, v1.y / v2.y, v1.z / v2.z};
}

inline vec3 operator*(const vec3 &v, const float t) {
  return {v.x * t, v.y * t, v.z * t};
}

inline vec3 operator*(const float t, const vec3 &v) {
  return {v.x * t, v.y * t, v.z * t};
}

inline vec3 operator/(const vec3 &v, const float t) {
  return {v.x / t, v.y / t, v.z / t};
}

inline float dot(const vec3 &v1, const vec3 &v2) {
  return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

inline vec3 cross(const vec3 &v1, const vec3 &v2) {
  return { (v1.y * v2.z - v1.z * v2.y),
          -(v1.x * v2.z - v1.z * v2.x),
           (v1.x * v2.y - v1.y * v2.x) };
}

inline vec3 unit_vector(vec3 v) {
  return v / length(v);
}

inline std::istream& operator<<(std::istream &is, vec3 &v) {
  is >> v.x >> v.y >> v.z;
  return is;
}

inline std::ostream& operator<<(std::ostream &os, const vec3 &v) {
  os << v.x << " " << v.y << " " << v.z;
  return os;
}

#endif // VEC3_H
