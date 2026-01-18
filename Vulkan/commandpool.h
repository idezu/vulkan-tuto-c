#pragma once

#include <vulkan/vulkan_core.h>
VkResult createCommandPool(VkCommandPool *commandPool, VkDevice device,
                           VkPhysicalDevice physicaleDevice,
                           VkSurfaceKHR surface);
