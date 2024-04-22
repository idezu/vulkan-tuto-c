#include <stdint.h>
#include <stdlib.h>

#include <debug.h>

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

VkResult createImageViews(VkImageView** swapChainImageViews, VkImage* swapChainImages, uint16_t swapChainImagesCount, VkFormat swapChainImageFormat, VkDevice device)
	{
		*swapChainImageViews = malloc(swapChainImagesCount*sizeof(VkImageView));

		for (size_t i = 0; i < swapChainImagesCount ; i++)
			{
				VkImageViewCreateInfo createInfo = {
					.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
					.image = swapChainImages[i],
					.viewType = VK_IMAGE_VIEW_TYPE_2D,
					.format = swapChainImageFormat,
					.components = (VkComponentMapping){
						.r = VK_COMPONENT_SWIZZLE_IDENTITY,
						.g = VK_COMPONENT_SWIZZLE_IDENTITY,
						.b = VK_COMPONENT_SWIZZLE_IDENTITY,
						.a = VK_COMPONENT_SWIZZLE_IDENTITY,
					},
					.subresourceRange = (VkImageSubresourceRange){
						.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
						.baseArrayLayer = 0,
						.baseMipLevel = 0,
						.layerCount = 1,
						.levelCount = 1
					}
				};
				VkResult result = vkCreateImageView(device,&createInfo,NULL,&(*swapChainImageViews)[i]);
				if (result != VK_SUCCESS)
					{
						return result;
					}
			}
		return VK_SUCCESS;
	}
