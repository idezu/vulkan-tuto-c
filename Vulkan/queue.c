#include <stdlib.h>

#include <vulkan/vulkan.h>

#include "queue.h"

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
	{
		QueueFamilyIndices indices = {
			QUEUE_FAMILY_EMPTY,
			QUEUE_FAMILY_EMPTY
		};

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, NULL);

		VkQueueFamilyProperties*  queueFamilies = malloc(queueFamilyCount*sizeof(VkQueueFamilyProperties));

		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies);

		for (size_t i = 0; i < queueFamilyCount; i++)
			{
				if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT && indices.graphicsFamily == QUEUE_FAMILY_EMPTY)
					{
						indices.graphicsFamily = i;
					}
				VkBool32 presentSupport = false;
				vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
				if (presentSupport && indices.graphicsFamily != i && indices.presentFamily == QUEUE_FAMILY_EMPTY)
					{
						indices.presentFamily = i;
					}
				
				if (isQueueComplete(indices))
					{
						break;
					}
			}
		free(queueFamilies);
		return indices;
	}

bool isQueueComplete(QueueFamilyIndices indices)
	{
		return indices.graphicsFamily != QUEUE_FAMILY_EMPTY && indices.presentFamily != QUEUE_FAMILY_EMPTY;
	}
