#pragma once

#include <vulkan/vulkan.h>

VkResult createShaderModule( VkShaderModule* module, char* code, int codeSize, VkDevice device);