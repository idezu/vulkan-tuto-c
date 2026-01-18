#pragma once 

#include <vulkan/vulkan.h>

VkResult pickPhysicalDevice(VkPhysicalDevice* physicalDevice, VkInstance instance, VkSurfaceKHR surface);

VkResult createLogicalDevice(VkDevice* device, VkPhysicalDevice physicalDevice, VkQueue* graphicsQueue, VkQueue* presentQueue,VkSurfaceKHR surface);

uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, VkPhysicalDevice physicalDevice);

VkFormat findSupportedFormat(VkFormat* candidates, uint32_t candidateCount, VkImageTiling tiling, VkFormatFeatureFlags features, VkPhysicalDevice physicalDevice);

VkFormat findDepthFormat(VkPhysicalDevice physicalDevice);

VkSampleCountFlagBits getMaxUsableSampleCount(VkPhysicalDevice physicalDevice);

bool hasStencilComponent(VkFormat format);
