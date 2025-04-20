#include "renderer.h"
#include <graphics/ig_buffer.h>
#include <stdlib.h>
#include <string.h>

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "../external/cimgui/cimgui.h"

void dark_theme(ImGuiStyle* style) {
	style->FrameRounding = 4;
	style->FramePadding.x = style->FramePadding.y = 8;
	style->WindowRounding = 4;
	style->FrameBorderSize = 1;
	style->GrabRounding = 2;
	style->ScrollbarRounding = 0;

	style->Colors[ImGuiCol_Button] = (ImVec4) { 0.345f, 0.345f, 0.345f, 1 };
	style->Colors[ImGuiCol_ButtonHovered] = (ImVec4) { 0.345f * 1.2f, 0.345f * 1.2f, 0.345f * 1.2f, 1 };
	style->Colors[ImGuiCol_ButtonActive] = (ImVec4) { 0.345f * 0.8f, 0.345f * 0.8f, 0.345f * 0.8f, 1 };
	style->Colors[ImGuiCol_SliderGrab] = (ImVec4) { 0.392f, 0.392f, 0.392f, 1 };
	style->Colors[ImGuiCol_SliderGrabActive] = (ImVec4) { 0.392f * 1.3f, 0.392f * 1.3f, 0.392f * 1.3f, 1 };
	style->Colors[ImGuiCol_WindowBg] = (ImVec4) { 0.22f, 0.22f, 0.22f, 1 };
	style->Colors[ImGuiCol_Border] = style->Colors[ImGuiCol_TableBorderLight] = style->Colors[ImGuiCol_TableBorderStrong] = (ImVec4) { 0.047f, 0.047f, 0.047f, 1 };
	style->Colors[ImGuiCol_TabDimmedSelected] = (ImVec4) { 0.239f, 0.239f, 0.239f, 1 };
	style->Colors[ImGuiCol_TabSelected] = (ImVec4) { 0.239f, 0.239f, 0.239f, 1 };
	style->Colors[ImGuiCol_TabHovered] = (ImVec4) { 0.239f * 1.4f, 0.239f * 1.4f, 0.239f * 1.4f, 1 };
	style->Colors[ImGuiCol_TabDimmedSelectedOverline] = (ImVec4) {};
	style->Colors[ImGuiCol_TitleBg] = style->Colors[ImGuiCol_TabDimmed] = style->Colors[ImGuiCol_Tab] = (ImVec4) { 0.157f, 0.157f, 0.157f, 1 };
	style->Colors[ImGuiCol_TitleBgActive] = (ImVec4) { 0.157f, 0.157f, 0.157f, 1 };
	style->Colors[ImGuiCol_Header] = (ImVec4) { 0.22f, 0.22f, 0.22f, 1 };
	style->Colors[ImGuiCol_HeaderHovered] = (ImVec4) { 0.271f, 0.271f, 0.271f, 1 };
	style->Colors[ImGuiCol_HeaderActive] = (ImVec4) { 0.149f, 0.353f, 0.541f, 1 };

	style->Colors[ImGuiCol_FrameBg] = (ImVec4) { 0.165f, 0.165f, 0.165f, 1 };
	style->Colors[ImGuiCol_FrameBgHovered] = (ImVec4) { 0.22f, 0.22f, 0.22f, 1 };
	style->Colors[ImGuiCol_FrameBgActive] = (ImVec4) { 0.165f * 0.8f, 0.165f * 0.8f, 0.165f * 0.8f, 1 };
	style->Colors[ImGuiCol_CheckMark] = (ImVec4) { 0.845f, 0.845f, 0.845f, 1 };
}

void imgui_init(ig_context* context, ig_window* window, renderer* renderer) {
	ImGui_ImplVulkan_InitInfo vk_imgui_init_info = {
		.Instance = context->instance,
		.PhysicalDevice = context->gpu,
		.Device = context->device,
		.QueueFamily = context->queue_family,
		.Queue = context->queue,
		.DescriptorPool = context->descriptor_pool,
		.RenderPass = context->default_frame.render_pass,
		.MinImageCount = context->fif,
		.ImageCount = context->fif,
		.MSAASamples = VK_SAMPLE_COUNT_1_BIT,
		.Subpass = 0
	};
	
	igCreateContext(NULL);
	ImGuiIO* io = igGetIO();
	io->IniFilename = "settings.ini";
	io->LogFilename = NULL;
	io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	// igStyleColorsLight(NULL);
	dark_theme(igGetStyle());
	igImplGlfw_InitForVulkan(window->native_handle, true);
	igImplVulkan_Init(&vk_imgui_init_info);

	renderer->fonts[RENDERER_FONT_SMALL] = ImFontAtlas_AddFontFromFileTTF(io->Fonts, "app/res/fonts/calibri-regular.ttf", 16, NULL, NULL);
	renderer->fonts[RENDERER_FONT_SMALL_BOLD] = ImFontAtlas_AddFontFromFileTTF(io->Fonts, "app/res/fonts/calibri-bold.ttf", 16, NULL, NULL);
	renderer->fonts[RENDERER_FONT_SMALL_MONO] = ImFontAtlas_AddFontFromFileTTF(io->Fonts, "app/res/fonts/jetbrains-regular.ttf", 20, NULL, NULL);
	renderer->fonts[RENDERER_FONT_SMALL_MONO_BOLD] = ImFontAtlas_AddFontFromFileTTF(io->Fonts, "app/res/fonts/jetbrains-bold.ttf", 20, NULL, NULL);
	renderer->fonts[RENDERER_FONT_MED] = ImFontAtlas_AddFontFromFileTTF(io->Fonts, "app/res/fonts/calibri-regular.ttf", 20, NULL, NULL);
	renderer->fonts[RENDERER_FONT_MED_BOLD] = ImFontAtlas_AddFontFromFileTTF(io->Fonts, "app/res/fonts/calibri-bold.ttf", 20, NULL, NULL);
	renderer->fonts[RENDERER_FONT_BIG] = ImFontAtlas_AddFontFromFileTTF(io->Fonts, "app/res/fonts/calibri-regular.ttf", 24, NULL, NULL);
	renderer->fonts[RENDERER_FONT_BIG_BOLD] = ImFontAtlas_AddFontFromFileTTF(io->Fonts, "app/res/fonts/calibri-bold.ttf", 24, NULL, NULL);

	igImplVulkan_CreateFontsTexture();
}

void imgui_render(_ig_frame* frame) {
	igRender();
	ImDrawData* draw_data = igGetDrawData();
	igImplVulkan_RenderDrawData(draw_data, frame->cmd_buffer, VK_NULL_HANDLE);
}

void imgui_destroy() {
	igImplVulkan_Shutdown();
    igImplGlfw_Shutdown();
    igDestroyContext(NULL);
}

renderer* renderer_create(ig_context* context, ig_window* window) {
	renderer* r = malloc(sizeof(renderer));
	r->context = context;
	imgui_init(context, window, r);

	r->rtx = raytracer_create(context);

	r->cam_rot = (vec3s) { 0, 0, 0 };
	r->global.cam_pos = (vec3s) { 0, 0, 2 };
	r->global.atmosphere_size = 0.1f;
	r->global.density_falloff = 3;
	r->global.num_in_scatter_points = 5;
	r->global.num_optical_depth_points = 5;

	vec3s wavelengths = { 700, 530, 440 };
	float scatter_strength = 10;

	r->global.scattering_coefficients.x = powf(400 / wavelengths.x, 4) * scatter_strength;
	r->global.scattering_coefficients.y = powf(400 / wavelengths.y, 4) * scatter_strength;
	r->global.scattering_coefficients.z = powf(400 / wavelengths.z, 4) * scatter_strength;

	r->global_buffer = ig_context_dbuffer_create(context, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, &r->global, sizeof(r->global));

	r->global_set = malloc(context->fif * sizeof(VkDescriptorSet));

	vkAllocateDescriptorSets(context->device, &(VkDescriptorSetAllocateInfo) {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.pNext = NULL,
		.descriptorPool = context->descriptor_pool,
		.descriptorSetCount = context->fif,
		.pSetLayouts = (VkDescriptorSetLayout[]) { context->global_layout, context->global_layout }
	}, r->global_set);

	for (int i = 0; i < context->fif; i++) {
		vkUpdateDescriptorSets(context->device, 1, &(VkWriteDescriptorSet) {
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.pNext = NULL,
			.dstSet = r->global_set[i],
			.dstBinding = 1,
			.dstArrayElement = 0,
			.descriptorCount = 1,
			.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.pBufferInfo = &(VkDescriptorBufferInfo) {
				.offset = 0,
				.buffer = r->global_buffer[i].buffer,
				.range = VK_WHOLE_SIZE
			},
			.pTexelBufferView = NULL
		}, 0, NULL);
	}

	return r;
}

void renderer_start_imgui_frame(renderer* renderer) {
	igImplVulkan_NewFrame();
	igImplGlfw_NewFrame();
	igNewFrame();
}

void renderer_flush(renderer* renderer, const ig_vec4* clear_color) {
	_ig_frame* frame = renderer->context->frames + renderer->context->frame_idx;

	glm_mat4_identity((vec4*) &renderer->global.inv_proj);
	glm_perspective(glm_rad(45), (float) renderer->rtx->tex[0]->dim.x / (float) renderer->rtx->tex[0]->dim.y, 0.01f, 100000, (vec4*) &renderer->global.inv_proj);
	glm_mat4_inv((vec4*) &renderer->global.inv_proj, (vec4*) &renderer->global.inv_proj);

	glm_mat4_identity((vec4*) &renderer->global.inv_view);
	glm_rotate((vec4*) &renderer->global.inv_view, renderer->cam_rot.x, (float*) &(vec3) { 1, 0, 0 });
	glm_rotate((vec4*) &renderer->global.inv_view, renderer->cam_rot.y, (float*) &(vec3) { 0, 1, 0 });
	glm_rotate((vec4*) &renderer->global.inv_view, renderer->cam_rot.z, (float*) &(vec3) { 0, 0, 1 });
	glm_translate((vec4*) &renderer->global.inv_view, (float*) &renderer->global.cam_pos);
	glm_mat4_inv((vec4*) &renderer->global.inv_view, (vec4*) &renderer->global.inv_view);

	memcpy(renderer->global_buffer[renderer->context->frame_idx].data, &renderer->global, sizeof(renderer->global));
	vkCmdBindDescriptorSets(frame->cmd_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, renderer->context->standard_layout, 0, 1, &renderer->global_set[renderer->context->frame_idx], 0, NULL);

	raytracer_exec(renderer->rtx, renderer->context);

	vkCmdBeginRenderPass(frame->cmd_buffer, &(VkRenderPassBeginInfo) {
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.pNext = NULL,
		.renderPass = renderer->context->default_frame.render_pass,
		.framebuffer = renderer->context->default_frame.framebuffer,
		.renderArea = {
			.offset = { .x = 0, .y = 0 },
			.extent = {
				.width = renderer->context->default_frame.resolution.x,
				.height = renderer->context->default_frame.resolution.y
			}
		},
		.clearValueCount = 2,
		.pClearValues = (VkClearValue[]) {
			{ .color = { .float32 = { clear_color->x, clear_color->y, clear_color->z, clear_color->w } } },
			{ .depthStencil = { .depth = 0, .stencil = 0 }}
		}
	}, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindVertexBuffers(frame->cmd_buffer, 0, 1, &renderer->context->quad_buffer->buffer, (VkDeviceSize[]) { 0 });
	imgui_render(frame);
	vkCmdEndRenderPass(frame->cmd_buffer);
}

void renderer_destroy(renderer* renderer) {
	free(renderer->global_set);
	ig_context_dbuffer_destroy(renderer->context, renderer->global_buffer);
	raytracer_destroy(renderer->context, renderer->rtx);

	imgui_destroy();
	free(renderer);
}
