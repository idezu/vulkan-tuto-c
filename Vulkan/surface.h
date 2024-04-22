#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

VkResult createSurface(VkSurfaceKHR* surface, VkInstance instance, GLFWwindow* window);
