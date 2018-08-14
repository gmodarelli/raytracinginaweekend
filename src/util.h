#ifndef UTIL_H
#define UTIL_H

// NOTE: Taken from https://github.com/aras-p/ToyPathTracer/blob/01-initial/Cpp/Source/Maths.cpp
static uint32_t s_RndState = 1;
static uint32_t XorShift32() {
    uint32_t x = s_RndState;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 15;
    s_RndState = x;
    return x;
}

float random_float() {
    return (XorShift32() & 0xFFFFFF) / 16777216.0f;
}

#endif // UTIL_H
