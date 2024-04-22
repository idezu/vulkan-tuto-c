#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

VkResult createSurface(VkSurfaceKHR* surface, VkInstance instance, GLFWwindow* window)
	{
		VkResult result = glfwCreateWindowSurface(instance,window,NULL,surface);
		if (result != VK_SUCCESS)
			{
				return result;
			}
		
		return VK_SUCCESS;
	}
