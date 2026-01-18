#include <stdlib.h>

#include <vulkan/vulkan.h>

#include <debug.h>

#include "queue.h"
#include "extension.h"
#include "swapchain.h"

VkSampleCountFlagBits getMaxUsableSampleCount(VkPhysicalDevice physicalDevice) {
	VkPhysicalDeviceProperties physicalDeviceProperties;
	vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

	VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
	if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
	if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
	if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
	if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
	if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
	if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

	return VK_SAMPLE_COUNT_1_BIT;
}

VkFormat findSupportedFormat(VkFormat* candidates, uint32_t candidateCount, VkImageTiling tiling, VkFormatFeatureFlags features, VkPhysicalDevice physicalDevice) {
	for (uint32_t i = 0; i < candidateCount; i++) {
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(physicalDevice, candidates[i], &props);
		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
			return candidates[i];
		}
		if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
			return candidates[i];
		}
	}
	error("failed to find supported format\n");
	return VK_FORMAT_UNDEFINED;
}

VkFormat findDepthFormat(VkPhysicalDevice physicalDevice) {
	VkFormat formats[] = {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT};
	return findSupportedFormat(
		formats,
		sizeof(formats) / sizeof(formats[0]),
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT,
		physicalDevice
	);
}

bool hasStencilComponent(VkFormat format) {
	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, VkPhysicalDevice physicalDevice) {
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) != 0 && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}
	error("failed to find suitable memory type");
	__builtin_unreachable();
	return 0;
}

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
		VkPhysicalDeviceFeatures deviceFeatures = {0};
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

		return isQueueComplete(indices) && extensionsSupported && swapChainAdequate && deviceFeatures.samplerAnisotropy;
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
		VkPhysicalDeviceFeatures deviceFeatures = {
			.samplerAnisotropy = VK_TRUE,
			.sampleRateShading = VK_TRUE,
		};
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
