#include <stdio.h>
#include <stdlib.h>

#include <vulkan/vulkan.h>

#include <GLFW/glfw3.h>

#include "Error/error.h"
#include "extension.h"

#include <debug.h>

VkResult createInstance(VkInstance* instance)
	{
		#ifdef DEBUG
			if (!checkValidationLayerSupport())
				{
					error("check Failed\n");
					return VK_ERROR_LAYER_NOT_PRESENT;
				}
		#endif
		VkApplicationInfo appInfo = {
			.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			.pApplicationName = "Hello Triangle",
			.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
			.pEngineName = "No Engine",
			.engineVersion = VK_MAKE_VERSION(1, 0, 0),
			.apiVersion = VK_API_VERSION_1_0,
		};

		// checking for extentions support
		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, NULL);
		VkExtensionProperties* extensions = malloc(extensionCount*sizeof(VkExtensionProperties));
		if (extensions == NULL)
			{
				return VK_ERROR_MEMORY_MAP_FAILED;
			}
		
		vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, extensions);

		debug("available extentions: \n");

	#ifdef DEBUG

		for (size_t i = 0; i < extensionCount; i++)
			{
				printf("\t%s\n",extensions[i].extensionName);
			}
	#endif
		free(extensions);

		uint32_t RequiredExtensionCount = 0;
		char** RequiredExtensions = NULL;

		getRequiredExtensions(&RequiredExtensionCount,&RequiredExtensions);
		
		VkInstanceCreateInfo createInfo = {
			.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			.pApplicationInfo = &appInfo,
			.enabledExtensionCount = RequiredExtensionCount,
			.ppEnabledExtensionNames = (const char* const*)RequiredExtensions,
			.enabledLayerCount = 0,
		};

		#ifdef DEBUG
			const char** validationLayers = NULL;
			const int validationLayerCount = 0;
			getValidationLayers(&validationLayerCount,&validationLayers);
			createInfo.enabledLayerCount = validationLayerCount;
			createInfo.ppEnabledLayerNames = validationLayers;

			VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {0};
			populateDebugMessengerCreateInfo(&debugCreateInfo);
			createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
		#endif

		VkResult result = vkCreateInstance(&createInfo, NULL, instance);
		free(RequiredExtensions);
		return result;
	}
