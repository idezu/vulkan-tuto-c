#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <GLFW/glfw3.h>
#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan.h>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <cglm/cglm.h>
#include <stb_image.h>

#include "Vulkan/Error/error.h"
#include "Vulkan/commandbuffer.h"
#include "Vulkan/commandpool.h"
#include "Vulkan/device.h"
#include "Vulkan/framebuffer.h"
#include "Vulkan/graphicspipeline.h"
#include "Vulkan/imageview.h"
#include "Vulkan/instance.h"
#include "Vulkan/model.h"
#include "Vulkan/renderpass.h"
#include "Vulkan/surface.h"
#include "Vulkan/swapchain.h"
#include "Vulkan/sync.h"

#include <debug.h>
#include <string.h>

#include "Vulkan/texture.h"

#define HEIGHT 600
#define WIDTH 800

typedef struct {
  GLFWwindow *window;
  VkInstance instance;
  VkDebugUtilsMessengerEXT debugMessenger;
  VkPhysicalDevice physicalDevice;
  VkDevice device;
  VkQueue graphicsQueue;
  VkQueue presentQueue;
  VkSurfaceKHR surface;
  VkSwapchainKHR swapChain;
  VkImage *swapChainImages;
  uint16_t swapChainImagesCount;
  VkFormat swapChainImageFormat;
  VkExtent2D swapChainExtent;
  VkImageView *swapChainImageViews;
  VkRenderPass renderPass;
  VkDescriptorSetLayout descriptorSetLayout;
  VkPipelineLayout pipelineLayout;
  VkPipeline graphicsPipeline;
  VkFramebuffer *swapChainFramebuffers;
  VkCommandPool commandPool;
  VkCommandBuffer *commandBuffer;
  VkSemaphore *imageAvailableSemaphore;
  VkSemaphore *renderFinishedSemaphore;
  VkFence *inFlightFence;
  VkBuffer vertexBuffer;
  VkDeviceMemory vertexBufferMemory;
  VkBuffer indexBuffer;
  VkDeviceMemory indexBufferMemory;
  VkBuffer* uniformBuffers;
  VkDeviceMemory* uniformBuffersMemory;
  void** uniformBuffersMapped;
  VkDescriptorPool descriptorPool;
  VkDescriptorSet* descriptorSets;
  uint32_t mipLevels;
  VkImage textureImage;
  VkDeviceMemory textureImageMemory;
  VkImageView textureImageView;
  VkSampler textureSampler;
  VkImage depthImage;
  VkDeviceMemory depthImageMemory;
  VkImageView depthImageView;
  Vertex* vertices;
  uint32_t vertexBufferSize;
  uint32_t* indices;
  uint32_t indexBufferSize;
  VkSampleCountFlagBits msaaSamples;
  VkImage colorImage;
  VkDeviceMemory colorImageMemory;
  VkImageView colorImageView;
} App;

bool framebufferResized = false;

static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
  (void)window;
  (void)width;
  (void)height;
  framebufferResized = true;
}

VkResult init(App *app) {
  if (glfwInit() != GLFW_TRUE) {
    return VK_ERROR_INITIALIZATION_FAILED;
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  // glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  debug("creating window\n");
  app->window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", NULL, NULL);

  glfwSetFramebufferSizeCallback(app->window, framebufferResizeCallback);

  debug("creating instance\n");
  VkResult result = createInstance(&app->instance);
  if (result != VK_SUCCESS) {
    return result;
  }
#ifdef DEBUG
  debug("setting up validation layers\n");
  result = setupDebugMessenger(&app->debugMessenger, app->instance);
  if (result != VK_SUCCESS) {
    return result;
  }
#endif
  debug("creating surface\n");
  result = createSurface(&app->surface, app->instance, app->window);
  if (result != VK_SUCCESS) {
    return result;
  }
  info("picking physical device\n");
  result =
      pickPhysicalDevice(&app->physicalDevice, app->instance, app->surface);
  if (result != VK_SUCCESS) {
    return result;
  }

  app->msaaSamples = getMaxUsableSampleCount(app->physicalDevice);

  debug("creating logical device\n");
  result = createLogicalDevice(&app->device, app->physicalDevice,
                               &app->graphicsQueue, &app->presentQueue,
                               app->surface);
  if (result != VK_SUCCESS) {
    return result;
  }
  debug("creating swapchain\n");
  result = createSwapChain(&app->swapChain, &app->swapChainImages,
                           &app->swapChainExtent, &app->swapChainImageFormat,
                           &app->swapChainImagesCount, app->surface,
                           app->device, app->physicalDevice, app->window);
  if (result != VK_SUCCESS) {
    return result;
  }
  debug("creating image views\n");
  result = createImageViews(&app->swapChainImageViews, app->swapChainImages,
                            app->swapChainImagesCount,
                            app->swapChainImageFormat, app->device);
  if (result != VK_SUCCESS) {
    return result;
  }

  info("building the render pass\n");

  result = createRenderPass(&app->renderPass, app->device, app->msaaSamples,
                            app->swapChainImageFormat,app->physicalDevice);
  if (result != VK_SUCCESS) {
    return result;
  }

  info("creating the descriptor set layout\n");

  result = createDescriptorSetLayout(&app->descriptorSetLayout,app->device);
  CHECK(result);

  info("building the graphic pipeline\n");
  result = createGraphicsPipeline(&app->graphicsPipeline, &app->pipelineLayout,
                                  app->device, app->swapChainExtent,
                                  app->renderPass,app->descriptorSetLayout, app->msaaSamples);
  if (result != VK_SUCCESS) {
    return result;
  }

  info("creating command pool\n");

  result = createCommandPool(&app->commandPool, app->device,
                             app->physicalDevice, app->surface);

  if (result != VK_SUCCESS) {
    return result;
  }

  info("creating  texture image\n");

  result = createTextureImage(&app->textureImage,&app->textureImageMemory, &app->mipLevels, app->device, app->commandPool, app->graphicsQueue, app->physicalDevice);
  CHECK(result);

  info("creating texture image views\n");

  result = createTextureImageView(&app->textureImageView, app->mipLevels, app->textureImage, app->device);
  CHECK(result);

  info("creating texture sampler\n");

  result = createTextureSampler(&app->textureSampler, app->device, app->physicalDevice);
  CHECK(result);

  info("loading model\n");
  result = loadModel(&app->vertices,&app->vertexBufferSize,&app->indices,&app->indexBufferSize);

  info("creating vertex buffer\n");

  result = createVertexBuffer(&app->vertexBuffer,&app->vertexBufferMemory,app->vertices, app->vertexBufferSize,
    app->device,app->physicalDevice,app->commandPool,app->graphicsQueue);

  if (result != VK_SUCCESS) {
    return result;
  }

  info("creating index buffer\n");

  result = createIndexBuffer(&app->indexBuffer,&app->indexBufferMemory, app->indices, app->indexBufferSize,
    app->device, app->physicalDevice,app->commandPool,app->graphicsQueue);

  if (result != VK_SUCCESS) {
    return result;
  }

  info("creating uniform buffers\n");
  result = createUniformBuffers(&app->uniformBuffers,&app->uniformBuffersMemory,(void**)&app->uniformBuffersMapped, app->device, app->physicalDevice);
  CHECK(result);

  info("creating descriptor pool\n");
  result = createDescriptorPool(&app->descriptorPool, app->device);
  CHECK(result);

  info("creating color ressources\n");

  result = createColorRessources(&app->colorImage, &app->colorImageMemory, &app->colorImageView, app->swapChainImageFormat, app->swapChainExtent,
    app->msaaSamples, app->device, app->physicalDevice);
  CHECK(result);

  info("creating depth ressources\n");
  result = createDepthRessources(&app->depthImage, &app->depthImageMemory, &app->depthImageView, app->swapChainExtent,
    app->msaaSamples, app->device, app->physicalDevice, app->commandPool, app->graphicsQueue);
  CHECK(result);

  info("creating framebuffers\n");
  result = createFramebufers(&app->swapChainFramebuffers, app->device,
                             app->renderPass, app->swapChainImageViews,
                             app->swapChainExtent, app->swapChainImagesCount, app->depthImageView, app->colorImageView);

  if (result != VK_SUCCESS) {
    return result;
  }

  info("creating descriptor sets\n");

  result = createDescriptorSets(&app->descriptorSets, app->descriptorSetLayout, app->descriptorPool, app->device,
    app->uniformBuffers, app->textureImageView, app->textureSampler);
  CHECK(result);

  info("creating command buffer\n");

  result =
      createCommandBuffer(&app->commandBuffer, app->commandPool, app->device);

  if (result != VK_SUCCESS) {
    return result;
  }

  info("creating sync objects\n");

  result = createSyncObjects(&app->imageAvailableSemaphore,
                             &app->renderFinishedSemaphore, &app->inFlightFence,
                             app->device, app->swapChainImagesCount);

  if (result != VK_SUCCESS) {
    return result;
  }

  return VK_SUCCESS;
}

const int MAX_FRAMES_IN_FLIGHT = 2;

void cleanupSwapChain(App* app) {
  vkDestroyImageView(app->device, app->colorImageView, NULL);
  vkDestroyImage(app->device, app->colorImage, NULL);
  vkFreeMemory(app->device, app->colorImageMemory, NULL);
  vkDestroyImageView(app->device, app->depthImageView, NULL);
  vkDestroyImage(app->device, app->depthImage, NULL);
  vkFreeMemory(app->device, app->depthImageMemory, NULL);
  for (size_t i = 0; i < app->swapChainImagesCount; i++) {
    vkDestroyFramebuffer(app->device, app->swapChainFramebuffers[i], NULL);
    vkDestroyImageView(app->device, app->swapChainImageViews[i], NULL);
  }
  vkDestroySwapchainKHR(app->device, app->swapChain, NULL);
}

VkResult recreateSwapchain(App* app) {
  vkDeviceWaitIdle(app->device);
  cleanupSwapChain(app);
  VkResult res = createSwapChain(&app->swapChain, &app->swapChainImages, &app->swapChainExtent,
                                 &app->swapChainImageFormat, &app->swapChainImagesCount,
                                 app->surface, app->device, app->physicalDevice, app->window);
  if (res != VK_SUCCESS) {
    return res;
  }
  res = createImageViews(&app->swapChainImageViews, app->swapChainImages, app->swapChainImagesCount,
    app->swapChainImageFormat, app->device);
  if (res != VK_SUCCESS) {
    return res;
  }
  res = createColorRessources(&app->colorImage, &app->colorImageMemory, &app->colorImageView, app->swapChainImageFormat, app->swapChainExtent,
    app->msaaSamples, app->device, app->physicalDevice);
  CHECK(res);
  res = createDepthRessources(&app->depthImage, &app->depthImageMemory, &app->depthImageView, app->swapChainExtent, app->msaaSamples,
    app->device, app->physicalDevice, app->commandPool, app->graphicsQueue);
  CHECK(res);
  res = createFramebufers(&app->swapChainFramebuffers, app->device, app->renderPass,
                          app->swapChainImageViews, app->swapChainExtent,
                          app->swapChainImagesCount, app->depthImageView, app->colorImageView);
  if (res != VK_SUCCESS) {
    return res;
  }

  return VK_SUCCESS;
}


void cleanup(App *app) {
  for (size_t i = 0; i < app->swapChainImagesCount; i++) {
    vkDestroySemaphore(app->device, app->renderFinishedSemaphore[i], NULL);
  }
  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    vkDestroySemaphore(app->device, app->imageAvailableSemaphore[i], NULL);
    vkDestroyFence(app->device, app->inFlightFence[i], NULL);
    vkDestroyBuffer(app->device, app->uniformBuffers[i], NULL);
    vkFreeMemory(app->device, app->uniformBuffersMemory[i], NULL);
  }
  free(app->renderFinishedSemaphore);
  free(app->imageAvailableSemaphore);
  free(app->inFlightFence);
  vkDestroyCommandPool(app->device, app->commandPool, NULL);
  vkDestroyPipeline(app->device, app->graphicsPipeline, NULL);
  vkDestroyPipelineLayout(app->device, app->pipelineLayout, NULL);
  vkDestroyRenderPass(app->device, app->renderPass, NULL);
  cleanupSwapChain(app);
  vkDestroySampler(app->device, app->textureSampler, NULL);
  vkDestroyImageView(app->device,app->textureImageView,NULL);
  vkDestroyImage(app->device, app->textureImage, NULL);
  vkFreeMemory(app->device, app->textureImageMemory, NULL);
  vkDestroyDescriptorPool(app->device, app->descriptorPool, NULL);
  vkDestroyDescriptorSetLayout(app->device, app->descriptorSetLayout, NULL);
  vkFreeMemory(app->device, app->vertexBufferMemory, NULL);
  vkDestroyBuffer(app->device, app->vertexBuffer, NULL);
  vkFreeMemory(app->device, app->indexBufferMemory, NULL);
  vkDestroyBuffer(app->device, app->indexBuffer, NULL);
  free(app->swapChainImageViews);
  free(app->swapChainFramebuffers);
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

void windowResized(App *app) {
  int width, height;
  glfwGetFramebufferSize(app->window,&width,&height);
  while (width == 0 || height == 0) {
    glfwGetFramebufferSize(app->window,&width,&height);
    glfwWaitEvents();
  }
}

static double startTime;

VkResult updateUniformBuffer(uint32_t currentFrame, App *app) {

  double currentTime = glfwGetTime();

  float time = currentTime - startTime;

  UniformBufferObject ubo = {};

  glm_mat4_identity(ubo.model);

  glm_rotate(ubo.model, time*glm_rad(90.0f),(vec3){0.0f,0.0f,1.0f});

  glm_lookat((vec3){2.0f,2.0f,2.0f},(vec3){0.0f,0.0f,0.0f},(vec3){0.0f,0.0f,1.0f},ubo.view);

  glm_perspective(glm_rad(45.0f), app->swapChainExtent.width/ (float) app->swapChainExtent.height, 0.1f,10.0f, ubo.proj);

  ubo.proj[1][1] *= -1;

  memcpy(((void**)app->uniformBuffersMapped)[currentFrame], &ubo, sizeof(ubo));

  return VK_SUCCESS;
}

VkResult drawFrame(App *app) {
  static uint32_t currentFrame = 0;

  VkResult res = vkWaitForFences(
      app->device, 1, &app->inFlightFence[currentFrame], VK_TRUE, UINT64_MAX);
  if (res != VK_SUCCESS) {
    return res;
  }
  uint32_t imageIndex;
  res = vkAcquireNextImageKHR(app->device, app->swapChain, UINT64_MAX,
                              app->imageAvailableSemaphore[currentFrame], NULL,
                              &imageIndex);
  if (res == VK_ERROR_OUT_OF_DATE_KHR) {
    res = recreateSwapchain(app);
    return res;
  }
  if (res != VK_SUCCESS && res != VK_SUBOPTIMAL_KHR) {
    error("failed to acquire the next image, %s\n", string_VkResult(res));
    return res;
  }

  res = vkResetFences(app->device, 1, &app->inFlightFence[currentFrame]);
  if (res != VK_SUCCESS) {
    return res;
  }

  res = vkResetCommandBuffer(app->commandBuffer[currentFrame], 0);

  res = recordCommandBuffer(app->commandBuffer[currentFrame], imageIndex,
                            app->renderPass, app->swapChainFramebuffers,
                            app->swapChainExtent, app->graphicsPipeline,app->vertexBuffer,app->indexBuffer,
                            app->descriptorSets[currentFrame],app->pipelineLayout, app->indexBufferSize);
  if (res != VK_SUCCESS) {
    return res;
  }

  VkSemaphore waitSemaphores[] = {app->imageAvailableSemaphore[currentFrame]};

  VkPipelineStageFlags waitStages[] = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

  VkSemaphore signalSemaphore[] = {app->renderFinishedSemaphore[imageIndex]};

  res = updateUniformBuffer(currentFrame, app);
  CHECK(res);

  VkSubmitInfo submitInfo = {
      .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
      .waitSemaphoreCount = 1,
      .pWaitSemaphores = waitSemaphores,
      .pWaitDstStageMask = waitStages,
      .commandBufferCount = 1,
      .pCommandBuffers = &app->commandBuffer[currentFrame],
      .signalSemaphoreCount = 1,
      .pSignalSemaphores = signalSemaphore,
  };

  res = vkQueueSubmit(app->graphicsQueue, 1, &submitInfo,
                      app->inFlightFence[currentFrame]);
  if (res != VK_SUCCESS) {
    error("failed to submit draw command buffer\n");
    return res;
  }

  VkSwapchainKHR swapChains[] = {app->swapChain};

  VkPresentInfoKHR presentInfo = {
      .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
      .waitSemaphoreCount = 1,
      .pWaitSemaphores = signalSemaphore,
      .swapchainCount = 1,
      .pSwapchains = swapChains,
      .pImageIndices = &imageIndex,
      .pResults = NULL,
  };

  res = vkQueuePresentKHR(app->presentQueue, &presentInfo);
  if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR || framebufferResized) {
    windowResized(app);
    res = recreateSwapchain(app);
  }
  if (res != VK_SUCCESS) {
    error("failed to present swap chain image\n");
    return res;
  }
  currentFrame++;
  currentFrame %= MAX_FRAMES_IN_FLIGHT;
  return VK_SUCCESS;
}

void loop(App *app) {
  while (!glfwWindowShouldClose(app->window)) {
    glfwPollEvents();
    VkResult res = drawFrame(app);
    if (res != VK_SUCCESS) {
      error("%s\n", string_VkResult(res));
      return;
    }
  }

  vkDeviceWaitIdle(app->device);
}

int main() {
  App VulkanApp = {0};
  debug("VulkanApp is %ld bytes\n", sizeof(VulkanApp));
  debug("initializing vulkan\n");
  VkResult result = init(&VulkanApp);
  if (result != VK_SUCCESS) {
    error("%s\n", string_VkResult(result));
    return result;
  }

  startTime = glfwGetTime();
  debug("entering the main loop\n");
  loop(&VulkanApp);

  debug("closing the program\n");
  cleanup(&VulkanApp);
}
