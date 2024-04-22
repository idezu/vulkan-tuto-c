#pragma once 

#include <vulkan/vulkan.h>

#define QUEUE_FAMILY_EMPTY UINT32_MAX-1

typedef struct {
    uint32_t graphicsFamily;
    uint32_t presentFamily;
} QueueFamilyIndices;

bool isQueueComplete(QueueFamilyIndices indices);

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
