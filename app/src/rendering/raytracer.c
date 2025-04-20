#include "raytracer.h"

raytracer* raytracer_create(ig_context* ctx) {
	raytracer* r = malloc(sizeof(raytracer));
	r->tex = malloc(ctx->fif * sizeof(ig_texture*));

	for (int i = 0; i < ctx->fif; i++) {
		r->tex[i] = malloc(sizeof(ig_texture));

		r->tex[i]->dim.x = ctx->default_frame.resolution.x;
		r->tex[i]->dim.y = ctx->default_frame.resolution.y;

		vmaCreateImage(ctx->allocator, &(VkImageCreateInfo) {
			.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			.pNext = NULL,
			.flags = 0,
			.imageType = VK_IMAGE_TYPE_2D,
			.format = VK_FORMAT_R8G8B8A8_UNORM,
			.extent = { .width = ctx->default_frame.resolution.x, .height = ctx->default_frame.resolution.y, .depth = 1 },
			.mipLevels = 1,
			.arrayLayers = 1,
			.samples = VK_SAMPLE_COUNT_1_BIT,
			.tiling = VK_IMAGE_TILING_OPTIMAL,
			.usage = VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
			.queueFamilyIndexCount = 0,
			.pQueueFamilyIndices = NULL,
			.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
		}, &(VmaAllocationCreateInfo) {
			.flags = 0,
			.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
			.requiredFlags = 0,
			.preferredFlags = 0,
			.memoryTypeBits = 0,
			.pool = VK_NULL_HANDLE,
			.pUserData = NULL,
			.priority = 0
		}, &r->tex[i]->image, &r->tex[i]->memory, NULL);

		vkCreateImageView(ctx->device, &(VkImageViewCreateInfo) {
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.pNext = NULL,
			.flags = 0,
			.image = r->tex[i]->image,
			.viewType = VK_IMAGE_VIEW_TYPE_2D,
			.format = VK_FORMAT_R8G8B8A8_UNORM,
			.components = {
				.r = VK_COMPONENT_SWIZZLE_IDENTITY,
				.g = VK_COMPONENT_SWIZZLE_IDENTITY,
				.b = VK_COMPONENT_SWIZZLE_IDENTITY,
				.a = VK_COMPONENT_SWIZZLE_IDENTITY
			},
			.subresourceRange = {
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1
			}
		}, NULL, &r->tex[i]->view);
	}

	VkShaderModule shader = ig_context_create_shader_from_file(ctx, "app/res/shaders/test.spv");

	vkCreateComputePipelines(ctx->device, VK_NULL_HANDLE, 1, &(VkComputePipelineCreateInfo) {
		.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
		.pNext = NULL,
		.stage = (VkPipelineShaderStageCreateInfo) {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.pNext = NULL,
			.stage = VK_SHADER_STAGE_COMPUTE_BIT,
			.module = shader,
			.pName = "main",
			.pSpecializationInfo = NULL
		},
		.layout = ctx->standard_layout,
		.basePipelineHandle = VK_NULL_HANDLE,
		.basePipelineIndex = 0
	}, NULL, &r->pipeline);

	vkDestroyShaderModule(ctx->device, shader, NULL);
	
	vkResetCommandBuffer(ctx->transfer_cmd_buffer, 0);
	vkBeginCommandBuffer(ctx->transfer_cmd_buffer, &(VkCommandBufferBeginInfo) {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.pNext = NULL,
		.flags = 0,
		.pInheritanceInfo = NULL
	});

	for (int i = 0; i < ctx->fif; i++) {
		vkCmdPipelineBarrier(ctx->transfer_cmd_buffer,
			VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 0, 0, NULL, 0, NULL, 1, &(VkImageMemoryBarrier) {
				.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
				.pNext = NULL,
				.srcAccessMask = VK_ACCESS_NONE,
				.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_SHADER_READ_BIT,
				.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
				.newLayout = VK_IMAGE_LAYOUT_GENERAL,
				.srcQueueFamilyIndex = ctx->queue_family,
				.dstQueueFamilyIndex = ctx->queue_family,
				.image = r->tex[i]->image,
				.subresourceRange = {
					.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
					.baseMipLevel = 0,
					.levelCount = 1,
					.baseArrayLayer = 0,
					.layerCount = 1
				}
			});
	}
	vkEndCommandBuffer(ctx->transfer_cmd_buffer);
	
	vkResetFences(ctx->device, 1, &ctx->transfer_fence);
	vkQueueSubmit(ctx->queue, 1, &(VkSubmitInfo) {
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.pNext = NULL,
		.waitSemaphoreCount = 0,
		.pWaitSemaphores = NULL,
		.pWaitDstStageMask = NULL,
		.commandBufferCount = 1,
		.pCommandBuffers = &ctx->transfer_cmd_buffer,
		.signalSemaphoreCount = 0,
		.pSignalSemaphores = NULL
	}, ctx->transfer_fence);

	vkWaitForFences(ctx->device, 1, &ctx->transfer_fence, VK_TRUE, UINT64_MAX);

	r->desc_set = malloc(ctx->fif * sizeof(VkDescriptorSet));

	vkAllocateDescriptorSets(ctx->device, &(VkDescriptorSetAllocateInfo) {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.pNext = NULL,
		.descriptorPool = ctx->descriptor_pool,
		.descriptorSetCount = ctx->fif,
		.pSetLayouts = (VkDescriptorSetLayout[]) { ctx->texture_layout, ctx->texture_layout }
	}, r->desc_set);

	r->earth_tex = ig_context_texture_create_from_file(ctx, "app/res/textures/earth.png");
	r->moon_tex = ig_context_texture_create_from_file(ctx, "app/res/textures/moon.png");
	r->sun_tex = ig_context_texture_create_from_file(ctx, "app/res/textures/sun.jpg");

	r->earth_normals = ig_context_texture_create_from_file(ctx, "app/res/textures/earth_normals.jpg");
	r->moon_normals = ig_context_texture_create_from_file(ctx, "app/res/textures/moon_normals.png");
	r->sun_normals = ig_context_texture_create_from_file(ctx, "app/res/textures/sun.jpg");

	for (int i = 0; i < ctx->fif; i++) {
		vkUpdateDescriptorSets(ctx->device, 1, &(VkWriteDescriptorSet) {
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.pNext = NULL,
			.dstSet = r->desc_set[i],
			.dstBinding = 0,
			.dstArrayElement = 0,
			.descriptorCount = 1,
			.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
			.pImageInfo = &(VkDescriptorImageInfo) {
				.imageView = r->tex[i]->view,
				.imageLayout = VK_IMAGE_LAYOUT_GENERAL
			},
			.pBufferInfo = NULL,
			.pTexelBufferView = NULL
		}, 0, NULL);

		vkUpdateDescriptorSets(ctx->device, 1, &(VkWriteDescriptorSet) {
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.pNext = NULL,
			.dstSet = r->desc_set[i],
			.dstBinding = 1,
			.dstArrayElement = 0,
			.descriptorCount = 1,
			.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			.pImageInfo = &(VkDescriptorImageInfo) {
				.imageView = r->earth_tex->view,
				.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				.sampler = ctx->linear_sampler
			},
			.pBufferInfo = NULL,
			.pTexelBufferView = NULL
		}, 0, NULL);

		vkUpdateDescriptorSets(ctx->device, 1, &(VkWriteDescriptorSet) {
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.pNext = NULL,
			.dstSet = r->desc_set[i],
			.dstBinding = 1,
			.dstArrayElement = 1,
			.descriptorCount = 1,
			.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			.pImageInfo = &(VkDescriptorImageInfo) {
				.imageView = r->moon_tex->view,
				.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				.sampler = ctx->linear_sampler
			},
			.pBufferInfo = NULL,
			.pTexelBufferView = NULL
		}, 0, NULL);

		vkUpdateDescriptorSets(ctx->device, 1, &(VkWriteDescriptorSet) {
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.pNext = NULL,
			.dstSet = r->desc_set[i],
			.dstBinding = 1,
			.dstArrayElement = 2,
			.descriptorCount = 1,
			.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			.pImageInfo = &(VkDescriptorImageInfo) {
				.imageView = r->sun_tex->view,
				.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				.sampler = ctx->linear_sampler
			},
			.pBufferInfo = NULL,
			.pTexelBufferView = NULL
		}, 0, NULL);

		vkUpdateDescriptorSets(ctx->device, 1, &(VkWriteDescriptorSet) {
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.pNext = NULL,
			.dstSet = r->desc_set[i],
			.dstBinding = 2,
			.dstArrayElement = 0,
			.descriptorCount = 1,
			.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			.pImageInfo = &(VkDescriptorImageInfo) {
				.imageView = r->earth_normals->view,
				.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				.sampler = ctx->linear_sampler
			},
			.pBufferInfo = NULL,
			.pTexelBufferView = NULL
		}, 0, NULL);

		vkUpdateDescriptorSets(ctx->device, 1, &(VkWriteDescriptorSet) {
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.pNext = NULL,
			.dstSet = r->desc_set[i],
			.dstBinding = 2,
			.dstArrayElement = 1,
			.descriptorCount = 1,
			.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			.pImageInfo = &(VkDescriptorImageInfo) {
				.imageView = r->moon_normals->view,
				.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				.sampler = ctx->linear_sampler
			},
			.pBufferInfo = NULL,
			.pTexelBufferView = NULL
		}, 0, NULL);

		vkUpdateDescriptorSets(ctx->device, 1, &(VkWriteDescriptorSet) {
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.pNext = NULL,
			.dstSet = r->desc_set[i],
			.dstBinding = 2,
			.dstArrayElement = 2,
			.descriptorCount = 1,
			.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			.pImageInfo = &(VkDescriptorImageInfo) {
				.imageView = r->sun_normals->view,
				.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				.sampler = ctx->linear_sampler
			},
			.pBufferInfo = NULL,
			.pTexelBufferView = NULL
		}, 0, NULL);
	}

	return r;
}

void raytracer_exec(raytracer* rtx, ig_context* ctx) {
	_ig_frame* frame = ctx->frames + ctx->frame_idx;

	vkCmdBindDescriptorSets(frame->cmd_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, ctx->standard_layout, 1, 1, &rtx->desc_set[ctx->frame_idx], 0, NULL);
	vkCmdBindPipeline(frame->cmd_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, rtx->pipeline);
	vkCmdDispatch(frame->cmd_buffer, rtx->tex[0]->dim.x / 16, rtx->tex[0]->dim.y / 16, 1);
}

void raytracer_destroy(ig_context* ctx, raytracer* rtx) {
	ig_context_texture_destroy(ctx, rtx->earth_tex);
	ig_context_texture_destroy(ctx, rtx->moon_tex);
	ig_context_texture_destroy(ctx, rtx->sun_tex);

	ig_context_texture_destroy(ctx, rtx->earth_normals);
	ig_context_texture_destroy(ctx, rtx->moon_normals);
	ig_context_texture_destroy(ctx, rtx->sun_normals);
	
	free(rtx->desc_set);
	vkDestroyPipeline(ctx->device, rtx->pipeline, NULL);
	for (int i = 0; i < ctx->fif; i++) {
		ig_context_texture_destroy(ctx, rtx->tex[i]);
	}
	free(rtx->tex);
	free(rtx);
}