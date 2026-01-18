#include "sync.h"
#include "debug.h"
#include <stddef.h>
#include <stdlib.h>
#include <vulkan/vulkan_core.h>

extern const int MAX_FRAMES_IN_FLIGHT;

VkResult createSyncObjects(VkSemaphore **imageAvailableSemaphore,
                           VkSemaphore **renderFinishedSemaphore,
                           VkFence **inFlightFence, VkDevice device,
                           uint32_t swapChainImageCount) {
  *imageAvailableSemaphore = malloc(MAX_FRAMES_IN_FLIGHT * sizeof(VkSemaphore));
  if (*imageAvailableSemaphore == NULL) {
    return VK_ERROR_MEMORY_MAP_FAILED;
  }
  *renderFinishedSemaphore = malloc(swapChainImageCount * sizeof(VkSemaphore));
  if (*renderFinishedSemaphore == NULL) {
    return VK_ERROR_MEMORY_MAP_FAILED;
  }
  *inFlightFence = malloc(MAX_FRAMES_IN_FLIGHT * sizeof(VkFence));
  if (*inFlightFence == NULL) {
    return VK_ERROR_MEMORY_MAP_FAILED;
  }
  VkSemaphoreCreateInfo semaphoreInfo = {
      .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
  };

  VkFenceCreateInfo fenceInfo = {
      .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
      .flags = VK_FENCE_CREATE_SIGNALED_BIT,
  };
  VkResult res;

  for (size_t i = 0; i < (size_t)MAX_FRAMES_IN_FLIGHT; i++) {

    res = vkCreateSemaphore(device, &semaphoreInfo, NULL,
                            (*imageAvailableSemaphore) + i);
    if (res != VK_SUCCESS) {
      goto ERROR_CREATE_SYNC_OBJECT;
    }
    res = vkCreateFence(device, &fenceInfo, NULL, *inFlightFence + i);
    if (res != VK_SUCCESS) {
      goto ERROR_CREATE_SYNC_OBJECT;
    }
  }
  for (size_t i = 0; i < swapChainImageCount; i++) {
    res = vkCreateSemaphore(device, &semaphoreInfo, NULL,
                            *renderFinishedSemaphore + i);
    if (res != VK_SUCCESS) {
      goto ERROR_CREATE_SYNC_OBJECT;
    }
  }
  return VK_SUCCESS;
ERROR_CREATE_SYNC_OBJECT:
  error("failed to create semaphores\n");
  return res;
}
