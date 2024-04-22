#include <stdlib.h>
#include <string.h>

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

#include "Error/error.h"

const char* deviceExtensions[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

uint8_t getDeviceExtention(const char*** extentions)
	{
		*extentions=deviceExtensions;
		return sizeof(deviceExtensions)/sizeof(char*);
	}

bool checkDeviceExtensionSupport(VkPhysicalDevice device) 
	{
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, NULL, &extensionCount, NULL);

		VkExtensionProperties* availableExtensions = malloc(extensionCount*sizeof(VkExtensionProperties));

		vkEnumerateDeviceExtensionProperties(device, NULL, &extensionCount, availableExtensions);

		uint8_t deviceExtensionCount = sizeof(deviceExtensions)/sizeof(const char*);

		for (size_t i = 0; i < deviceExtensionCount; i++)
			{
				bool extentionFound = false;
				for (size_t k = 0; k < extensionCount; k++)
					{
						if (strcmp(availableExtensions[k].extensionName,deviceExtensions[i]))
						{
							extentionFound = true;
							break;
						}
					}
				if (!extentionFound)
					{
						return false;
					}
			}
		
		free(availableExtensions);

		return true;
	}

void getRequiredExtensions(uint32_t* extensionCount,char*** extensions)
	{
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		#ifdef DEBUG
			*extensions = malloc((glfwExtensionCount+1)*sizeof(uintptr_t));
			memcpy(*extensions,glfwExtensions,glfwExtensionCount*sizeof(uintptr_t));
			(*extensions)[glfwExtensionCount] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
			*extensionCount = glfwExtensionCount+1;
		#else
			*extensions = malloc(glfwExtensionCount*sizeof(uintptr_t));
			memcpy(*extensions,glfwExtensions,glfwExtensionCount*sizeof(uintptr_t));
			*extensionCount = glfwExtensionCount;
		#endif
	}
