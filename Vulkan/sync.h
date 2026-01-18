#pragma once

#include <vulkan/vulkan_core.h>
VkResult createSyncObjects(VkSemaphore **imageAvailableSemaphore,
                           VkSemaphore **renderFinishedSemaphore,
                           VkFence **inFlightFence, VkDevice device,
                           uint32_t swapChainImageCount);
