#pragma once

#include <vulkan/vulkan.h>

VkResult copyBufferToImage(VkImage image, VkBuffer buffer, uint32_t width, uint32_t height, VkDevice device, VkCommandPool pool, VkQueue graphicsQueue);

VkResult transitionImageLayout(VkImage image,VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels, VkDevice device, VkCommandPool pool, VkQueue graphicsQueue);
VkResult createImage(
    VkImage* image,
    VkDeviceMemory* imageMemory,
    uint32_t width,
    uint32_t height,
    uint32_t mipLevels,
    VkSampleCountFlagBits numSamples,
    VkFormat format,
    VkImageTiling tiling,
    VkImageUsageFlags usage,
    VkMemoryPropertyFlags properties,
    VkDevice device,
    VkPhysicalDevice physicalDevice);

VkResult createImageView(VkImageView* imageView, uint32_t mipLevels, VkImage image, VkImageAspectFlags aspectFlags, VkFormat format, VkDevice device);
VkResult createImageViews(VkImageView** swapChainImageViews, VkImage* swapChainImages, uint16_t swapChainImagesCount, VkFormat swapChainImageFormat, VkDevice device);
