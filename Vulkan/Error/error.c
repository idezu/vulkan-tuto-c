#include <stdlib.h>
#include <string.h>

#include <vulkan/vulkan.h>

#include <colors.h>

#define __err_log(level, color, str, ...)                                      \
  fprintf_color(stderr, COLOR_CYAN, "VALIDATION_LAYER");                       \
  fprintf(stderr, "::[");                                                      \
  fprintf_color(stderr, color, level);                                         \
  fprintf(stderr, "] ");                                                       \
  fprintf(stderr, str __VA_OPT__(, ) __VA_ARGS__)

/**
 * @brief a macro for printing debug
 */
#define debug(str, ...)                                                        \
  __err_log("Verbose", COLOR_MAGENTA, str __VA_OPT__(, ) __VA_ARGS__)

/**
 * @brief a macro for printing infos
 *
 */
#define info(str, ...)                                                         \
  __err_log("Info", COLOR_GREEN, str __VA_OPT__(, ) __VA_ARGS__)

/**
 * @brief a macro for printing warnings
 *
 */
#define warning(str, ...)                                                      \
  __err_log("Warning", COLOR_YELLOW, str __VA_OPT__(, ) __VA_ARGS__)

/**
 * @brief a macro for printing errors
 *
 */
#define error(str, ...)                                                        \
  __err_log("Error", COLOR_RED, str __VA_OPT__(, ) __VA_ARGS__)

const char *validationLayers[] = {
    "VK_LAYER_KHRONOS_validation",
    //"VK_LAYER_LUNARG_api_dump",
};

const int validationLayersCount =
    sizeof(validationLayers) / sizeof(validationLayers[0]);

void getValidationLayers(int *const count, const char ***layers) {
  *count = validationLayersCount;
  *layers = validationLayers;
}

bool checkValidationLayerSupport() {
  uint32_t layerCount = 0;
  vkEnumerateInstanceLayerProperties(&layerCount, NULL);

  VkLayerProperties *availableLayers =
      malloc(layerCount * sizeof(VkLayerProperties));
  if (availableLayers == NULL) {
    return false;
  }

  vkEnumerateInstanceLayerProperties(&layerCount, availableLayers);

  for (size_t i = 0; i < validationLayersCount; i++) {
    bool layerFound = true;
    for (size_t k = 0; k < layerCount; k++) {
      if (strcmp(availableLayers[k].layerName, validationLayers[i])) {
        layerFound = true;
      }
    }
    if (!layerFound) {
      return false;
    }
  }

  free(availableLayers);

  return true;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL
debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
              VkDebugUtilsMessageTypeFlagsEXT messageType,
              const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
              void *pUserData) {

  switch (messageSeverity) {
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
    debug("%s\n", pCallbackData->pMessage);
    break;

  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
    info("%s\n", pCallbackData->pMessage);
    break;

  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
    warning("%s\n", pCallbackData->pMessage);
    break;

  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
    error("%s\n", pCallbackData->pMessage);
    break;

  default:
    error("What ?\n");
    break;
  }

  return VK_FALSE;
}

VkResult CreateDebugUtilsMessengerEXT(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
    const VkAllocationCallbacks *pAllocator,
    VkDebugUtilsMessengerEXT *pDebugMessenger) {
  PFN_vkCreateDebugUtilsMessengerEXT func =
      (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
          instance, "vkCreateDebugUtilsMessengerEXT");
  if (func != NULL) {
    return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
  } else {
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
}

void populateDebugMessengerCreateInfo(
    VkDebugUtilsMessengerCreateInfoEXT *createInfo) {
  createInfo->sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  createInfo->messageSeverity =
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  createInfo->messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  createInfo->pfnUserCallback = debugCallback;
}

VkResult setupDebugMessenger(VkDebugUtilsMessengerEXT *debugMessenger,
                             VkInstance instance) {
#ifdef DEBUG
  VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
  populateDebugMessengerCreateInfo(&createInfo);
  if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr,
                                   debugMessenger) != VK_SUCCESS) {
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
  return VK_SUCCESS;
#else
  return VK_ERROR_EXTENSION_NOT_PRESENT;
#endif
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance,
                                   VkDebugUtilsMessengerEXT debugMessenger,
                                   const VkAllocationCallbacks *pAllocator) {
  PFN_vkDestroyDebugUtilsMessengerEXT func =
      (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
          instance, "vkDestroyDebugUtilsMessengerEXT");
  if (func != nullptr) {
    func(instance, debugMessenger, pAllocator);
  }
}
