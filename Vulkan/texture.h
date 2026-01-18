//
// Created by idezu on 17/01/2026.
//

#ifndef VULKAN_TUTO_C_TEXTURE_H
#define VULKAN_TUTO_C_TEXTURE_H

#include <vulkan/vulkan_core.h>

VkResult createTextureImage(VkImage* textureImage, VkDeviceMemory* textureImageMemory, uint32_t* mipLevels, VkDevice device, VkCommandPool pool, VkQueue graphicsQueue, VkPhysicalDevice physicalDevice);
VkResult createTextureImageView(VkImageView* textureImageView, uint32_t mipLevels, VkImage textureImage, VkDevice device);
VkResult createTextureSampler(VkSampler* textureSampler, VkDevice device, VkPhysicalDevice physicalDevice);

#endif //VULKAN_TUTO_C_TEXTURE_H