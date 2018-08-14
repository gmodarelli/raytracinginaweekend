#ifndef UTIL_H
#define UTIL_H

#include <random>

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dis(0.0, 1.0);

float random_float() {
  return static_cast<float>(dis(gen));
}

#endif // UTIL_H
