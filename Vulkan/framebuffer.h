#pragma once

#include <stdint.h>
#include <vulkan/vulkan_core.h>
VkResult createFramebufers(VkFramebuffer **swapchainFramebuffers,
                           VkDevice device, VkRenderPass renderPass,
                           VkImageView *const swapchainImageViews,
                           VkExtent2D swapchainExtent,
                           uint16_t swapchainImageViewCount,
                           VkImageView depthImageView, VkImageView colorImageView);
