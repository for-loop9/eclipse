#ifndef RAYTRACER_H
#define RAYTRACER_H

#include "ignite.h"

typedef struct raytracer {
    ig_texture** tex;
    
    ig_texture* earth_tex;
    ig_texture* moon_tex;
    ig_texture* sun_tex;

    ig_texture* earth_normals;
    ig_texture* moon_normals;
    ig_texture* sun_normals;

    VkPipeline pipeline;
    VkDescriptorSet* desc_set;
} raytracer;

raytracer* raytracer_create(ig_context* ctx);
void raytracer_exec(raytracer* rtx, ig_context* ctx);
void raytracer_destroy(ig_context* ctx, raytracer* rtx);

#endif