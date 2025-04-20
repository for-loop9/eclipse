#include "gui.h"

void gui_start(gui* gui, renderer* r) {
	gui->compute_tex[0] = igImplVulkan_AddTexture(r->context->nearest_sampler, r->rtx->tex[0]->view, VK_IMAGE_LAYOUT_GENERAL);
	gui->compute_tex[1] = igImplVulkan_AddTexture(r->context->nearest_sampler, r->rtx->tex[1]->view, VK_IMAGE_LAYOUT_GENERAL);

	gui->camera.dist = 20;
	gui->camera.lx = 0;
	gui->camera.ly = 0;
	gui->camera.offset = (vec3s) { 0, 0, 0 };

	// r->global.cam_pos = (vec3s) { 0, 0, 3.7 };

	r->global.spheres[0] = (sphere) { .pos = { 0, 0, 0 }, .r = 3.67, .col = { 1, 1, 1 }, .rot = 0.5f };
	r->global.spheres[1] = (sphere) { .pos = { 220, 0, 0 }, .r = 1, .col = { 1, 1, 1 }, .rot = 0.5f };
	r->global.spheres[2] = (sphere) { .pos = { -86295, 0, 0 }, .r = 400, .col = { 1, 1, 0 }, .rot = 0.5f };
}

void gui_draw(gui* gui, renderer* r, const ig_window* window, const ig_keyboard* keyboard, const ig_mouse* mouse, bool* close_requested) {
	static float px = 0;
	gui->camera.dist -= 1.1f * mouse->dwheel;
	r->global.spheres[1].rot += 0.0001f;

	// if (gui->camera.dist <= dx) gui->camera.dist = dx;
	
	r->global.cam_pos.x = gui->camera.dist * cosf(gui->camera.lx) * sinf(gui->camera.ly);
	r->global.cam_pos.y = gui->camera.dist * sinf(gui->camera.lx);
	r->global.cam_pos.z = gui->camera.dist * cosf(gui->camera.lx) * cosf(gui->camera.ly);

	glm_vec3_add((float*) &r->global.cam_pos, (float*) &gui->camera.offset, (float*) &r->global.cam_pos);

	r->cam_rot.x = gui->camera.lx;
	r->cam_rot.y = -gui->camera.ly;

	if (ig_window_mouse_button_down((ig_window*) window, GLFW_MOUSE_BUTTON_LEFT)) {
		if (ig_window_keyboard_key_down((ig_window*) window, GLFW_KEY_LEFT_SHIFT)) {
			vec3s right;
			right.x = cosf(r->cam_rot.y);
			right.y = 0;
			right.z = sinf(r->cam_rot.y);

			vec3s up;
			up.x = sinf(r->cam_rot.x) * sinf(r->cam_rot.y);
			up.y = cosf(r->cam_rot.x);
			up.z = -sinf(r->cam_rot.x) * cosf(-r->cam_rot.y);

			gui->camera.offset.x -= right.x * (float) mouse->delta.x * 0.001f * gui->camera.dist;
			gui->camera.offset.y -= right.y * (float) mouse->delta.x * 0.001f * gui->camera.dist;
			gui->camera.offset.z -= right.z * (float) mouse->delta.x * 0.001f * gui->camera.dist;

			gui->camera.offset.x += up.x * (float) mouse->delta.y * 0.001f * gui->camera.dist;
			gui->camera.offset.y += up.y * (float) mouse->delta.y * 0.001f * gui->camera.dist;
			gui->camera.offset.z += up.z * (float) mouse->delta.y * 0.001f * gui->camera.dist;
		} else {
			gui->camera.ly -= mouse->delta.x * 0.004f;
			gui->camera.lx += mouse->delta.y * 0.004f;
		}
	}
	
	ImGuiStyle* style = igGetStyle();

	igPushStyleVar_Float(ImGuiStyleVar_WindowRounding, 12);
	igPushStyleVar_Float(ImGuiStyleVar_FrameRounding, 12);
	igBegin("poppup", NULL, ImGuiWindowFlags_NoTitleBar);

	if (igButton("earth", (ImVec2) {})) {
		gui->camera.dist = 20;
		gui->camera.offset = (vec3s) { 0, 0, 0 };
	}
	if (igButton("moon", (ImVec2) {})) {
		gui->camera.dist = 5;
		gui->camera.offset = (vec3s) { 220, 0, 0 };
	}
	if (igButton("sun", (ImVec2) {})) {
		gui->camera.dist = 800;
		gui->camera.offset = (vec3s) { -86295, 0, 0 };
	}

	igSeparatorText("atmosphere");

	igDragFloat("atmosphere size", &r->global.atmosphere_size, 0.001f, 0.01f, 0.5f, "%.2f", ImGuiSliderFlags_None);
	igDragInt("in scatter points", &r->global.num_in_scatter_points, 1, 1, 10, "%d", ImGuiSliderFlags_None);
	igDragInt("optical points", &r->global.num_optical_depth_points, 1, 1, 10, "%d", ImGuiSliderFlags_None);
	igDragFloat("density falloff", &r->global.density_falloff, 0.01f, -3, 3, "%.2f", ImGuiSliderFlags_None);

	// igDragFloat3("cam_pos", (float*) &r->global.cam_pos, 0.01f, -100, 100, "%.2f", ImGuiSliderFlags_None);

    ImDrawList* draw_list = igGetBackgroundDrawList(NULL);
    
	ImDrawList_AddImage(draw_list, gui->compute_tex[r->context->frame_idx], (ImVec2) { 0, 0 }, (ImVec2) { r->rtx->tex[0]->dim.x, r->rtx->tex[0]->dim.y }, (ImVec2) { 0, 1 }, (ImVec2) { 1, 0 }, igColorConvertFloat4ToU32((ImVec4) { 1, 1, 1, 1 }));

	igSetCursorPosY(igGetWindowHeight() - 50 - style->FramePadding.y);
	if (igButton("Save", (ImVec2) { igGetWindowWidth() / 2 - style->WindowPadding.x, 50 })) {
	}
	igSameLine(0, style->FramePadding.x / 2);
	if (igButton("Cancel", (ImVec2) { -1, 50 })) {
		*close_requested = true;
	}
	igEnd();
	igPopStyleVar(2);
}

void gui_end(gui* gui) {
}