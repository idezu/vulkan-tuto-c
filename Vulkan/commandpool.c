#include <debug.h>
#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

#include "queue.h"

#include "commandpool.h"

VkResult createCommandPool(VkCommandPool *commandPool, VkDevice device,
                           VkPhysicalDevice physicaleDevice,
                           VkSurfaceKHR surface) {
  QueueFamilyIndices queueFamilyIndices =
      findQueueFamilies(physicaleDevice, surface);

  VkCommandPoolCreateInfo poolInfo = {
      .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
      .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
      .queueFamilyIndex = queueFamilyIndices.graphicsFamily,
  };

  VkResult res = vkCreateCommandPool(device, &poolInfo, NULL, commandPool);

  if (res != VK_SUCCESS) {
    error("failed to create command pool\n");
    return res;
  }

  return VK_SUCCESS;
}
