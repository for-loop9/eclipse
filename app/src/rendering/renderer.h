#ifndef RENDERER_H
#define RENDERER_H

#include <cglm/struct.h>
#include <ignite.h>
#include <graphics/ig_dbuffer.h>
#include <stdalign.h>
#include "raytracer.h"
#include <stdalign.h>

typedef struct ImFont ImFont;

typedef enum {
	RENDERER_FONT_BIG = 0,
	RENDERER_FONT_BIG_BOLD,
	RENDERER_FONT_MED,
	RENDERER_FONT_MED_BOLD,
	RENDERER_FONT_SMALL,
	RENDERER_FONT_SMALL_BOLD,
	RENDERER_FONT_SMALL_MONO,
	RENDERER_FONT_SMALL_MONO_BOLD,
} renderer_font;

typedef struct sphere {
	alignas(16) vec3s pos;
	alignas(4) float r;
	alignas(16) vec3s col;
	alignas(4) float rot;
} sphere;

typedef struct renderer {
	ig_context* context;
	ImFont* fonts[8];

	raytracer* rtx;
	
	struct {
		alignas(16) vec3s cam_pos;
		alignas(16) mat4s inv_proj;
		alignas(16) mat4s inv_view;

		sphere spheres[3];

		alignas(4) float atmosphere_size;
		alignas(4) float density_falloff;
		alignas(4) int num_optical_depth_points;
		alignas(4) int num_in_scatter_points;

		alignas(16) vec3s scattering_coefficients;
	} global;

	vec3s cam_rot;

	ig_dbuffer* global_buffer;
	VkDescriptorSet* global_set;
} renderer;

typedef struct game game;

renderer* renderer_create(ig_context* context, ig_window* window);
void renderer_start_imgui_frame(renderer* renderer);
void renderer_flush(renderer* renderer, const ig_vec4* clear_color);
void renderer_destroy(renderer* renderer);

#endif