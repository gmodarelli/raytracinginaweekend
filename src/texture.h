#ifndef TEXTURE_H
#define TEXTURE_H

struct Texture {
    enum Type { Constant, Checker };
    Type type;
    vec3 color;
    Texture* odd;
    Texture* even;
};

vec3 texture_value(const Texture& tex, float u, float v, const vec3& p) {
    if (tex.type == Texture::Constant) {
        return tex.color;
    } else if (tex.type == Texture::Checker) {
        float sines = sin(10 * p.x) * sin(10 * p.y) * sin(10 * p.z);
        if (sines < 0) {
            return tex.odd->color;
        } else {
            return tex.even->color;
        }
    }
}

#endif // TEXTURE_H
