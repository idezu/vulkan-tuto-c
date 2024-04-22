#pragma once

#include <vulkan/vulkan.h>

VkResult createImageViews(VkImageView** swapChainImageViews, VkImage* swapChainImages, uint16_t swapChainImagesCount, VkFormat swapChainImageFormat, VkDevice device);
