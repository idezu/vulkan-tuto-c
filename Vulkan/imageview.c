#include <stdint.h>
#include <stdlib.h>

#include <debug.h>

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vk_enum_string_helper.h>

#include "commandbuffer.h"
#include "device.h"

VkResult copyBufferToImage(VkImage image, VkBuffer buffer, uint32_t width, uint32_t height, VkDevice device, VkCommandPool pool, VkQueue graphicsQueue) {
    VkCommandBuffer commandBuffer;
    VkResult res = beginSingleTimeCommands(&commandBuffer, pool, device);
    CHECK(res,"failed to begin command buffer to copy buffer to image\n");

    VkBufferImageCopy copyRegion = {
        .bufferOffset = 0,
        .bufferRowLength = 0,
        .bufferImageHeight = 0,
        .imageSubresource = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .mipLevel = 0,
            .baseArrayLayer = 0,
            .layerCount = 1,
        },
        .imageOffset = {0,0,0},
        .imageExtent = {
            width,
            height,
            1,
        },
    };

    vkCmdCopyBufferToImage(
        commandBuffer,
        buffer,
        image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &copyRegion
    );

    res = endSingleTimeCommands(commandBuffer, graphicsQueue, device, pool);
    CHECK(res,"failed to end command buffer to copy buffer to image\n");
    return VK_SUCCESS;
}

VkResult transitionImageLayout(VkImage image,VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels, VkDevice device, VkCommandPool pool, VkQueue graphicsQueue) {
    VkCommandBuffer commandBuffer;
    VkResult res = beginSingleTimeCommands(&commandBuffer, pool, device);
    CHECK(res,"failed to begin command buffer to transition layout\n");

    VkImageMemoryBarrier barrier = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .oldLayout = oldLayout,
        .newLayout = newLayout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = image,
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = mipLevels,
            .baseArrayLayer = 0,
            .layerCount = 1,
        },
        .srcAccessMask = 0,
        .dstAccessMask = 0,
    };

    VkPipelineStageFlags sourceStage = 0;
    VkPipelineStageFlags destinationStage = 0;

    if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

        if (hasStencilComponent(format)) {
            barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }
    } else {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    }

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    } else {
        error("unsupported layout transition from %s to %s\n",string_VkImageLayout(oldLayout),string_VkImageLayout(newLayout));
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage, destinationStage,
        0,
        0, NULL,
        0, NULL,
        1, &barrier
        );

    res = endSingleTimeCommands(commandBuffer, graphicsQueue, device, pool);
    CHECK(res,"failed to end command buffer to transition layout\n");

    return VK_SUCCESS;
}

VkResult createImage(
    VkImage* image,
    VkDeviceMemory* imageMemory,
    uint32_t width,
    uint32_t height,
    uint32_t mipLevels,
    VkSampleCountFlagBits numSamples,
    VkFormat format,
    VkImageTiling tiling,
    VkImageUsageFlags usage,
    VkMemoryPropertyFlags properties,
    VkDevice device,
    VkPhysicalDevice physicalDevice) {
    VkImageCreateInfo imageInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = VK_IMAGE_TYPE_2D,
        .extent = {
            .width = width,
            .height = height,
            .depth = 1,
        },
        .mipLevels = mipLevels,
        .arrayLayers = 1,
        .format = format,
        .tiling = tiling,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .usage = usage,
        .samples = numSamples,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };
    VkResult res = vkCreateImage(device, &imageInfo, NULL, image);
    CHECK(res, "failed to create image\n");

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device, *image, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memRequirements.size,
        .memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits,properties,physicalDevice),
    };

    res = vkAllocateMemory(device, &allocInfo, NULL, imageMemory);
    CHECK(res,"failed to allocate image memory\n");

    res = vkBindImageMemory(device, *image, *imageMemory, 0);
    CHECK(res,"failed to bind image memory\n");

    return VK_SUCCESS;
}

VkResult createImageView(VkImageView* imageView, uint32_t mipLevels, VkImage image, VkImageAspectFlags aspectFlags, VkFormat format, VkDevice device) {
	VkImageViewCreateInfo viewInfo = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.image = image,
		.viewType = VK_IMAGE_VIEW_TYPE_2D,
		.format = format,
		.subresourceRange = {
			.aspectMask = aspectFlags,
			.baseMipLevel = 0,
			.levelCount = mipLevels,
			.baseArrayLayer = 0,
			.layerCount = 1
		},
	};

	VkResult res = vkCreateImageView(device, &viewInfo, NULL, imageView);
	CHECK(res,"failed to create image view\n");

	return VK_SUCCESS;
}

VkResult createImageViews(VkImageView** swapChainImageViews, VkImage* swapChainImages, uint16_t swapChainImagesCount, VkFormat swapChainImageFormat, VkDevice device)
	{
		*swapChainImageViews = malloc(swapChainImagesCount*sizeof(VkImageView));

		for (size_t i = 0; i < swapChainImagesCount ; i++)
			{
				VkResult res = createImageView(*swapChainImageViews + i, VK_REMAINING_MIP_LEVELS, swapChainImages[i], VK_IMAGE_ASPECT_COLOR_BIT, swapChainImageFormat, device);
				CHECK(res);
			}
		return VK_SUCCESS;
	}
