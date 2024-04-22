#pragma once 

#include <vulkan/vulkan.h>

VkResult pickPhysicalDevice(VkPhysicalDevice* physicalDevice, VkInstance instance, VkSurfaceKHR surface);

VkResult createLogicalDevice(VkDevice* device, VkPhysicalDevice physicalDevice, VkQueue* graphicsQueue, VkQueue* presentQueue,VkSurfaceKHR surface);
