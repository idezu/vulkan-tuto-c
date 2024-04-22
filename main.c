#include <stdint.h>
#include <stdlib.h>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <vulkan/vk_enum_string_helper.h>

#include "Vulkan/device.h"
#include "Vulkan/Error/error.h"
#include "Vulkan/imageview.h"
#include "Vulkan/instance.h"
#include "Vulkan/surface.h"
#include "Vulkan/swapchain.h"
#include "Vulkan/graphicspipeline.h"

#include <debug.h>

#define HEIGHT 600
#define WIDTH 800

typedef struct {
	GLFWwindow* window;
	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;
	VkPhysicalDevice physicalDevice;
	VkDevice device;
	VkQueue graphicsQueue;
	VkQueue presentQueue;
	VkSurfaceKHR surface;
	VkSwapchainKHR swapChain;
	VkImage* swapChainImages;
	uint16_t swapChainImagesCount;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	VkImageView* swapChainImageViews;
} App;

VkResult init(App* app)
	{
		if (glfwInit() != GLFW_TRUE)
			{
				return VK_ERROR_INITIALIZATION_FAILED;
			}

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		debug("creating window\n");
		app->window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", NULL, NULL);

		debug("creating instance\n");
		VkResult result = createInstance(&app->instance);
		if(result != VK_SUCCESS)
			{
				return result;
			}
		#ifdef DEBUG
			debug("setting up validation layers\n");
			result = setupDebugMessenger(&app->debugMessenger,app->instance);
			if(result != VK_SUCCESS)
				{
					return result;
				}
		#endif
		debug("creating surface\n");
		result = createSurface(&app->surface,app->instance,app->window);
		if (result != VK_SUCCESS)
			{
				return result;
			}
		info("picking physical device\n");
		result = pickPhysicalDevice(&app->physicalDevice,app->instance,app->surface);
		if (result != VK_SUCCESS)
			{
				return result;
			}
		debug("creating logical device\n");
		result = createLogicalDevice(&app->device,app->physicalDevice,&app->graphicsQueue,&app->presentQueue,app->surface);
		if (result != VK_SUCCESS)
			{
				return result;
			}
		debug("creating swapchain\n");
		result = createSwapChain(&app->swapChain,&app->swapChainImages,&app->swapChainExtent,&app->swapChainImageFormat,&app->swapChainImagesCount,app->surface,app->device,app->physicalDevice,app->window);
		if (result != VK_SUCCESS)
			{
				return result;
			}
		debug("creating image views\n");
		result = createImageViews(&app->swapChainImageViews, app->swapChainImages,app->swapChainImagesCount,app->swapChainImageFormat, app->device);
		if (result != VK_SUCCESS)
			{
				return result;
			}
		info("building the graphic pipeline\n");
		result = createGraphicsPipeline(app->device,app->swapChainExtent);
		if (result != VK_SUCCESS)
			{
				return result;
			}
		
		return VK_SUCCESS;
	}
void cleanup(App* app)
	{
		for (int i = 0; i<app->swapChainImagesCount ;i++ )
			{
				vkDestroyImageView(app->device,app->swapChainImageViews[i],NULL);
			}
		free(app->swapChainImageViews);
		vkDestroySwapchainKHR(app->device, app->swapChain, NULL);
		free(app->swapChainImages);
		vkDestroyDevice(app->device, NULL);
		#ifdef DEBUG
			DestroyDebugUtilsMessengerEXT(app->instance, app->debugMessenger, NULL);
			app->debugMessenger = NULL;
		#endif
		vkDestroySurfaceKHR(app->instance, app->surface, NULL);
		vkDestroyInstance(app->instance, NULL);
		app->instance = NULL;
		glfwDestroyWindow(app->window);
		app->window = NULL;
   		glfwTerminate();
	}

void loop(App* app)
	{
		while (!glfwWindowShouldClose(app->window)) 
			{
				glfwPollEvents();
			}
	}

int main() 
	{
		App VulkanApp = {0};
		debug("VulkanApp is %ld bytes\n",sizeof(VulkanApp));
		debug("initializing vulkan\n");
		VkResult result = init(&VulkanApp);
		if (result != VK_SUCCESS) 
			{
				error("%s\n",string_VkResult(result));
				return  result;
			}

		debug("entering the main loop\n");
		loop(&VulkanApp);


		debug("closing the program\n");
		cleanup(&VulkanApp);
	}
