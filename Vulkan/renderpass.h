#pragma once

#include <vulkan/vulkan_core.h>
VkResult createRenderPass(VkRenderPass *renderPass, VkDevice device, VkSampleCountFlagBits msaaSamples,
                          VkFormat swapChainImageFormat, VkPhysicalDevice physicalDevice);
