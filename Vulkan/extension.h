#pragma once

#include <vulkan/vulkan.h>

void getRequiredExtensions(uint32_t* extensionCount,char*** extensions);
uint8_t getDeviceExtention(const char*** extentions);
bool checkDeviceExtensionSupport(VkPhysicalDevice device);
