#include <iostream>
#include "vec3.h"
#include "ray.h"
#include "sphere.h"
#include "float.h"
#include "camera.h"
#include "material.h"
#include "texture.h"
#include "util.h"
#include <chrono>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <stdio.h>
#include <stdlib.h>

const int image_width = 400;
const int image_height = 200;
const float inverse_image_width = 1.0f / (float)image_width;
const float inverse_image_height = 1.0f / (float)image_height;
const int max_depth = 50;
const int samples_per_pixel = 20;

const int imagesize = 3 * image_width * image_height;
unsigned char * image_buffer = (unsigned char *) malloc(imagesize);

const float minT = 0.001f;
const float maxT = 1.0e7f;

const int max_spheres = 50;
const vec3 light_center = {0.0f, 3.0f, 0.0f};
const float light_radius = 1.0f;
const float light_area = 4.0f * M_PI * light_radius;

vec3 trace_spheres(Spheres& spheres, Material materials[], int n, const ray& r, int depth, int& inoutRaycount) {
    ++inoutRaycount;
    hit_record rec;
    int hitId;

    if (hit_spheres(spheres, n, r, minT, maxT, rec, hitId)) {
        ray scattered;
        vec3 attenuation;
        vec3 emitted = material_emit(materials[hitId], rec.u, rec.v, rec.p);
        float pdf;
        if (depth < max_depth && material_scatter(materials[hitId], r, rec, attenuation, scattered, pdf)) {
            // Compute a 3D point on the lower hemisphere of the light source
            // The light source is the sphere at index 1 in the Spheres struct 
            // generated in the random_scene function.
            // The center of that sphere is (0.0f, 3.0f, 0.0f) and its radius 1.0f
            // For test sake, we'll point everything to the center of the sphere
            // and later we'll try with a point on its surface
            vec3 to_light = light_center - rec.p;
            float distance_squared = squared_length(to_light);
            vec3 to_light_unit = unit_vector(to_light);
            if (dot(to_light_unit, rec.normal) < 0)
                return emitted;

            float light_cosine = fabs(to_light_unit.y);
            if (light_cosine < 0.000001f)
                return emitted;

            pdf = distance_squared / (light_cosine * light_area);
            scattered.origin = rec.p;
            scattered.direction = to_light_unit;

            return emitted + attenuation * material_scattering_pdf(materials[hitId], r, rec, scattered) * trace_spheres(spheres, materials, n, scattered, depth + 1, inoutRaycount) / pdf;
        }
        else {
            return emitted;
        }
    } else {
        // vec3 unit_direction = unit_vector(r.direction);
        // float t = 0.5f * (unit_direction.y + 1.0f);
        // vec3 white = {1.0f, 1.0f, 1.0f};
        // vec3 blue = {0.5f, 0.7f, 1.0f};
        // return (1.0f - t) * white + t * blue;

        return {0, 0, 0};
    }
}

int random_scene(Spheres& spheres_soa, Material* materials, Texture* odd, Texture* even) {
    spheres_soa.center[0] = {0.0f, -1000.0f, 0.0f};
    spheres_soa.radius[0] = 1000.f;
    spheres_soa.inverse_radius[0] = 1.0f/1000.f;

    materials[0].type = Material::Lambert;
    materials[0].albedo = {Texture::Checker, {0.0f, 0.0f, 0.0f}, odd, even};

    spheres_soa.center[1] = light_center;
    spheres_soa.radius[1] = light_radius;
    spheres_soa.inverse_radius[1] = 1.0f/light_radius;

    materials[1].type = Material::Light;
    materials[1].albedo = {Texture::Constant, make_vec3(1.0f, 1.0f, 1.0f), nullptr, nullptr};

    int max = (int)sqrt(max_spheres) / 2;

    int i = 2;

    vec3 limit = {4.0f, 0.2f, 0.0f};
    for (int a = -max; a < max; a++) {
        for (int b = -max; b < max; b++) {
            float choose_mat = random_float();
            vec3 center = make_vec3(a + 0.9f * random_float(), 0.2f, b + 0.9f * random_float());
            if (length(center - limit) > 0.9f) {
                // if (choose_mat < 0.8f) { // diffuse
                    spheres_soa.center[i] = center;
                    spheres_soa.radius[i] = 0.2f;
                    spheres_soa.inverse_radius[i] = 1.0f/0.2f;

                    materials[i].type = Material::Lambert;
                    materials[i].albedo = {Texture::Constant, make_vec3(random_float() * random_float(), random_float() * random_float(), random_float() * random_float()), nullptr, nullptr};
                // }
                // else if (choose_mat < 0.95f) { // metal
                //    spheres_soa.center[i] = center;
                //    spheres_soa.radius[i] = 0.2f;
                //    spheres_soa.inverse_radius[i] = 1.0f/0.2f;

                    // materials[i].type = Material::Metal;
                    // materials[i].albedo = {Texture::Constant, make_vec3(0.5f * (1.0f + random_float()), 0.5f * (1.0f + random_float()), 0.5f * (1.0f + random_float())), nullptr, nullptr};
                    // materials[i].fuzz = 0.5f * random_float();
                // }
                //else {
                    // spheres_soa.center[i] = center;
                    // spheres_soa.radius[i] = 0.2f;
                    // spheres_soa.inverse_radius[i] = 1.0f/0.2f;

                    // materials[i].type = Material::Dielectric;
                    // materials[i].ref_idx = 1.5f; 
                // }

                i += 1;
            }
        }
    }

    // i += 1;
    // spheres_soa.center[i] = {0.0f, 1.0f, 0.0f};
    // spheres_soa.radius[i] = 1.0f;
    // spheres_soa.inverse_radius[i] = 1.0f;

    // materials[i].type = Material::Dielectric;
    // materials[i].ref_idx = 1.5;

    i += 1;
    spheres_soa.center[i] = {-4.0f, 1.0f, 0.0f};
    spheres_soa.radius[i] = 1.0f;
    spheres_soa.inverse_radius[i] = 1.0f;

    materials[i].type = Material::Lambert;
    materials[i].albedo = {Texture::Constant, make_vec3(0.4, 0.2, 0.1), nullptr, nullptr};

    // i += 1;
    // spheres_soa.center[i] = {4.0f, 1.0f, 0.0f};
    // spheres_soa.radius[i] = 1.0f;
    // spheres_soa.inverse_radius[i] = 1.0f;

    // materials[i].type = Material::Metal;
    // materials[i].albedo = {Texture::Constant, make_vec3(0.7f, 0.6f, 0.5f), nullptr, nullptr};
    // materials[i].fuzz = 0.0;


    // NOTE: i is an index
    return i + 1;
}

int main() {
    memset(image_buffer, 157, imagesize);

    vec3 lookfrom = {13, 5, 10};
    vec3 lookat = {0, 0, 0};
    vec3 up = {0, 1, 0};
    float dist_to_focus = 10.0;
    float aperture = 0.1;

    camera cam = default_camera(lookfrom, lookat, up, 20, float(image_width) / float(image_height), aperture, dist_to_focus);

    Material materials[max_spheres];
    Spheres spheres_soa = SpheresInit(max_spheres);

    Texture texture_odd = { Texture::Constant, make_vec3(0.2, 0.3, 0.1), nullptr, nullptr };
    Texture texture_even = { Texture::Constant, make_vec3(0.9, 0.9, 0.9), nullptr, nullptr };
    auto scene_t0 = std::chrono::high_resolution_clock::now();

    int spheres_count = random_scene(spheres_soa, materials, &texture_odd, &texture_even);
    std::cout << spheres_count << std::endl;

    auto scene_t1 = std::chrono::high_resolution_clock::now();

    auto scene_duration = std::chrono::duration_cast<std::chrono::microseconds>(scene_t1 - scene_t0);
    std::cout << "Scene built in " << scene_duration.count() << "us" << std::endl;

    int rayCount = 0;
    auto t0 = std::chrono::high_resolution_clock::now();

    for (int j = image_height - 1; j >= 0; j--) {
        for (int i = 0; i < image_width; i++) {
            vec3 tmp_col = {0, 0, 0};
            for (int s = 0; s < samples_per_pixel; s++) {
                float u = float(i + random_float()) * inverse_image_width;
                float v = float(j + random_float()) * inverse_image_height;
                ray r = camera_get_ray(cam, u, v);
                tmp_col = tmp_col + trace_spheres(spheres_soa, materials, spheres_count, r, 0, rayCount);
            }

            tmp_col = tmp_col / float(samples_per_pixel);
            // Gamma correction
            vec3 col = {sqrt(tmp_col.x), sqrt(tmp_col.y), sqrt(tmp_col.z)};

            int ir = int(255.99 * col.x);
            int ig = int(255.99 * col.y);
            int ib = int(255.99 * col.z);

            image_buffer[(i + (image_height - 1 - j) * image_width) * 3 + 0] = ir;
            image_buffer[(i + (image_height - 1 - j) * image_width) * 3 + 1] = ig;
            image_buffer[(i + (image_height - 1 - j) * image_width) * 3 + 2] = ib;
        }
    }

    auto t1 = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);
    std::cout << "Traced rays: " << rayCount << std::endl;
    std::cout << "Duration: " << duration.count() << "ms" << std::endl;
    std::cout << "MRays/s: " << (rayCount / duration.count() *  1.0e-3f) << std::endl;

    // TODO: Take the image path as param
    if (stbi_write_bmp("output.bmp", image_width, image_height, 3, (void *)image_buffer) != 0) {
        std::cout << "Image written to disk" << std::endl;
    } else {
        std::cerr << "Error!" << std::endl;
    }
    free(image_buffer);
}
