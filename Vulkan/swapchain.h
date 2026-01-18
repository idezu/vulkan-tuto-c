#pragma once

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

typedef struct {
  VkSurfaceCapabilitiesKHR capabilities;
  VkSurfaceFormatKHR *formats;
  uint8_t formatCount;
  VkPresentModeKHR *presentModes;
  uint8_t presentModeCount;
} SwapChainSupportDetails;

SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device,
                                              VkSurfaceKHR surface);
VkResult createSwapChain(VkSwapchainKHR *swapChain, VkImage **swapChainImages,
                         VkExtent2D *swapChainExtent,
                         VkFormat *swapChainImageFormat,
                         uint16_t *swapChainImageCount, VkSurfaceKHR surface,
                         VkDevice device, VkPhysicalDevice physicalDevice,
                         GLFWwindow *window);
