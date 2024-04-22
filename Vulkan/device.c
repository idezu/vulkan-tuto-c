#include <stdlib.h>

#include <vulkan/vulkan.h>

#include <debug.h>

#include "queue.h"
#include "extension.h"
#include "swapchain.h"

bool isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface) 
	{
		QueueFamilyIndices indices = findQueueFamilies(device,surface);

		bool extensionsSupported = checkDeviceExtensionSupport(device);

		bool swapChainAdequate = false;
		if (extensionsSupported) 
			{
				SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device,surface);
				swapChainAdequate = swapChainSupport.formats!=NULL && swapChainSupport.presentModes!=NULL;
				free(swapChainSupport.formats);
				free(swapChainSupport.presentModes);
			}

		return isQueueComplete(indices) && extensionsSupported && swapChainAdequate;
	}

VkResult pickPhysicalDevice(VkPhysicalDevice* physicalDevice, VkInstance instance, VkSurfaceKHR surface)
	{
		uint32_t deviceCount = 0;
		VkResult result = vkEnumeratePhysicalDevices(instance, &deviceCount, NULL);
		if (deviceCount <= 0)
			{
				return VK_ERROR_UNKNOWN;
			}
		if (result != VK_SUCCESS)
			{
				return result;
			}
		VkPhysicalDevice* devices = malloc(deviceCount*sizeof(VkPhysicalDevice));
		if (devices == NULL)
			{
				return VK_ERROR_MEMORY_MAP_FAILED;
			}
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices);
		for (size_t i = 0; i < deviceCount; i++)
			{
				if (isDeviceSuitable(devices[i],surface))
					{
						*physicalDevice = devices[i];
						free(devices);
						return VK_SUCCESS;
					}
			}
		free(devices);
		return VK_ERROR_UNKNOWN;
	}

VkResult createLogicalDevice(VkDevice* device, VkPhysicalDevice physicalDevice, VkQueue* graphicsQueue, VkQueue* presentQueue,VkSurfaceKHR surface)	
	{
		QueueFamilyIndices indices = findQueueFamilies(physicalDevice,surface);

		if (!isQueueComplete(indices))
			{
				error("didn't found all families\n");
				return VK_ERROR_UNKNOWN;
			}
		

		float queuePriority = 1.0f;

		VkDeviceQueueCreateInfo queueCreateInfos[] = {
			{
				.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
				.queueFamilyIndex = indices.graphicsFamily,
				.queueCount = 1,
				.pQueuePriorities = &queuePriority,
			},
			{
				.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
				.queueFamilyIndex = indices.presentFamily,
				.queueCount = 1,
				.pQueuePriorities = &queuePriority,
			}
		};
		VkPhysicalDeviceFeatures deviceFeatures = {0};
		const char** deviceExtentions;
		VkDeviceCreateInfo createInfo = {
			.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			.pQueueCreateInfos = queueCreateInfos,
			.queueCreateInfoCount = sizeof(queueCreateInfos)/sizeof(VkDeviceQueueCreateInfo),
			.pEnabledFeatures = &deviceFeatures,
			.enabledExtensionCount = getDeviceExtention(&deviceExtentions),
			.ppEnabledExtensionNames = deviceExtentions,
		};
		VkResult result = vkCreateDevice(physicalDevice, &createInfo, NULL, device);
		if (result != VK_SUCCESS)
			{
				return result;
			}
		vkGetDeviceQueue(*device, indices.graphicsFamily, 0, graphicsQueue);
		vkGetDeviceQueue(*device, indices.presentFamily, 0, presentQueue);
		return VK_SUCCESS;
	}
