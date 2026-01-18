//
// Created by idezu on 16/01/2026.
//

#ifndef VULKAN_TUTO_C_BUFFER_H
#define VULKAN_TUTO_C_BUFFER_H

#include <vulkan/vulkan_core.h>

VkResult createBuffer(VkBuffer* buffer, VkDeviceMemory* bufferMemory, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkDevice device, VkPhysicalDevice physicalDevice);

VkResult copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkCommandPool pool, VkDevice device, VkQueue graphicsQueue);

#endif //VULKAN_TUTO_C_BUFFER_H