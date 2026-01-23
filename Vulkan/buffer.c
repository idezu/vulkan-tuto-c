//
// Created by idezu on 16/01/2026.
//

#include <debug.h>
#include <vulkan/vulkan_core.h>

#include "commandbuffer.h"
#include "device.h"

#include "buffer.h"

VkResult createBuffer(VkBuffer *buffer, VkDeviceMemory *bufferMemory,
                      VkDeviceSize size, VkBufferUsageFlags usage,
                      VkMemoryPropertyFlags properties, VkDevice device,
                      VkPhysicalDevice physicalDevice) {
  VkBufferCreateInfo bufferCreateInfo = {
      .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
      .size = size,
      .usage = usage,
      .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
  };
  VkResult res = vkCreateBuffer(device, &bufferCreateInfo, nullptr, buffer);
  if (res != VK_SUCCESS) {
    error("failed to create buffer");
    return res;
  }

  VkMemoryRequirements memRequirements;

  vkGetBufferMemoryRequirements(device, *buffer, &memRequirements);

  VkMemoryAllocateInfo allocInfo = {
      .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
      .allocationSize = memRequirements.size,
      .memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits,
                                        properties, physicalDevice),
  };

  res = vkAllocateMemory(device, &allocInfo, nullptr, bufferMemory);
  if (res != VK_SUCCESS) {
    error("failed to allocate memory");
    return res;
  }

  vkBindBufferMemory(device, *buffer, *bufferMemory, 0);

  return VK_SUCCESS;
}

VkResult copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size,
                    VkCommandPool pool, VkDevice device,
                    VkQueue graphicsQueue) {
  VkCommandBuffer commandBuffer;
  VkResult res = beginSingleTimeCommands(&commandBuffer, pool, device);
  CHECK(res);

  VkBufferCopy copyRegion = {
      .size = size,
  };

  vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

  res = endSingleTimeCommands(commandBuffer, graphicsQueue, device, pool);
  CHECK(res);

  return VK_SUCCESS;
}
