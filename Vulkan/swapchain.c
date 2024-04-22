#include <stdlib.h>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "swapchain.h"
#include "queue.h"
#include "Math/math.h"

SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface) 
	{
		SwapChainSupportDetails details = {0};

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

		if (formatCount != 0) 
			{
				details.formats = malloc(sizeof(VkSurfaceFormatKHR)*formatCount);
				details.formatCount = formatCount;
				vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats);
			}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

		if (presentModeCount != 0) 
			{
				details.presentModes = malloc(presentModeCount*sizeof(VkPresentModeKHR));
				details.presentModeCount = presentModeCount;
				vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes);
			}
		return details;
	}

VkSurfaceFormatKHR chooseSwapSurfaceFormat(const VkSurfaceFormatKHR* availableFormats,uint16_t availableFormatCount)
	{
		for (size_t i = 0; i < availableFormatCount; i++)
			{
				if (availableFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB && availableFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) 
					{
						return availableFormats[i];
					}
			}
		return *availableFormats;
	}

VkPresentModeKHR chooseSwapPresentMode(const VkPresentModeKHR* availablePresentModes, uint16_t availablePresentModeCount)
	{
		for (size_t i = 0; i < availablePresentModeCount; i++)
			{
				if (availablePresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
					{
						return availablePresentModes[i];
					}
				
			}
		

		return VK_PRESENT_MODE_FIFO_KHR;
	}

VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR* capabilities, GLFWwindow* window)
	{
		if (capabilities->currentExtent.width != UINT32_MAX) 
			{
				return capabilities->currentExtent;
			} 
		else 
			{
				int width, height;
				glfwGetFramebufferSize(window, &width, &height);

				VkExtent2D actualExtent = {
					width,
					height
				};

				actualExtent.width = clamp(actualExtent.width, capabilities->minImageExtent.width, capabilities->maxImageExtent.width);
				actualExtent.height = clamp(actualExtent.height, capabilities->minImageExtent.height, capabilities->maxImageExtent.height);

				return actualExtent;
			}
	}

VkResult createSwapChain(VkSwapchainKHR* swapChain, VkImage** swapChainImages, VkExtent2D* swapChainExtent, VkFormat* swapChainImageFormat,uint16_t* swapChainImageCount, VkSurfaceKHR surface,
				VkDevice device, VkPhysicalDevice physicalDevice, GLFWwindow* window)	
	{
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice,surface);

		VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats,swapChainSupport.formatCount);
		VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes,swapChainSupport.presentModeCount);
		VkExtent2D extent = chooseSwapExtent(&swapChainSupport.capabilities,window);

		free(swapChainSupport.formats);
		free(swapChainSupport.presentModes);
		uint32_t imageCount = swapChainSupport.capabilities.minImageCount +1;
		if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) 
			{
				imageCount = swapChainSupport.capabilities.maxImageCount;
			}
		VkSwapchainCreateInfoKHR createInfo = {
			.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			.surface = surface,
			.minImageCount = imageCount,
			.imageFormat = surfaceFormat.format,
			.imageColorSpace = surfaceFormat.colorSpace,
			.imageExtent = extent,
			.imageArrayLayers = 1,
			.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
			.preTransform = swapChainSupport.capabilities.currentTransform,
			.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			.presentMode = presentMode,
			.clipped = VK_TRUE,
			.oldSwapchain = VK_NULL_HANDLE,

		};
		QueueFamilyIndices indices = findQueueFamilies(physicalDevice,surface);
		uint32_t queueFamilyIndices[] = {indices.graphicsFamily, indices.presentFamily};

		if (indices.graphicsFamily != indices.presentFamily) 
			{
				createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
				createInfo.queueFamilyIndexCount = 2;
				createInfo.pQueueFamilyIndices = queueFamilyIndices;
			} 
		else 
			{
				createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
				createInfo.queueFamilyIndexCount = 0; // Optional
				createInfo.pQueueFamilyIndices = nullptr; // Optional
			}
		VkResult result = vkCreateSwapchainKHR(device, &createInfo, nullptr, swapChain);
		if (result != VK_SUCCESS)
			{
				return result;
			}
		
		vkGetSwapchainImagesKHR(device, *swapChain, &imageCount, NULL);
		*swapChainImages = malloc(imageCount*sizeof(VkImage));
		if (*swapChainImages == NULL)
			{
				return VK_ERROR_MEMORY_MAP_FAILED;
			}
		
		vkGetSwapchainImagesKHR(device, *swapChain, &imageCount, *swapChainImages);

		*swapChainImageCount = imageCount;
		*swapChainImageFormat = surfaceFormat.format;
		*swapChainExtent = extent;
		return VK_SUCCESS;
	}
