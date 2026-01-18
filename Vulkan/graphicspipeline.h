#pragma once

#include <vulkan/vulkan_core.h>

#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <cglm/cglm.h>

typedef struct {
    vec3 pos;
    vec3 color;
    vec2 texCoord;
} Vertex;

typedef struct {
    mat4 model;
    mat4 view;
    mat4 proj;
} UniformBufferObject;

#define VERTEX_ATTRIBUTE_COUNT 3

VkResult createDepthRessources(
    VkImage* depthImage,
    VkDeviceMemory* depthImageMemory,
    VkImageView* depthImageView,
    VkExtent2D swapChainExtent,
    VkSampleCountFlagBits msaaSamples,
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    VkCommandPool commandPool,
    VkQueue graphicsQueue
    );

VkResult createGraphicsPipeline(VkPipeline *graphicsPipeline,
                                VkPipelineLayout *pipelineLayout,
                                VkDevice device, VkExtent2D swapChainExtent,
                                VkRenderPass renderPass,
                                VkDescriptorSetLayout descriptorSetLayout,
                                VkSampleCountFlagBits msaaSamples);

VkResult createVertexBuffer(VkBuffer* vertexBuffer, VkDeviceMemory* vertexBufferMemory, Vertex* vertices,
    uint32_t vertexBufferSize, VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool,
    VkQueue graphicsQueue);
VkResult createIndexBuffer(VkBuffer* indexBuffer, VkDeviceMemory* indexBufferMemory, uint32_t* indices,
    uint32_t indexBufferSize, VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool,
    VkQueue graphicsQueue);
uint32_t getVertexBufferSize();
uint32_t getIndexBufferSize();
VkResult createDescriptorSetLayout(VkDescriptorSetLayout* descriptorSetLayout, VkDevice device);
VkResult createUniformBuffers(VkBuffer** uniformBuffers,VkDeviceMemory** uniformBuffersMemory, void** uniformBuffersMapped, VkDevice device, VkPhysicalDevice physicalDevice);
VkResult createDescriptorPool(VkDescriptorPool* descriptorPool, VkDevice device);
VkResult createDescriptorSets(VkDescriptorSet** descriptorSets, VkDescriptorSetLayout descriptorSetLayout,
    VkDescriptorPool descriptorPool, VkDevice device, VkBuffer* uniformBuffers, VkImageView textureImageView,
    VkSampler textureSampler);

VkResult createColorRessources(
  VkImage* colorImage,
  VkDeviceMemory* colorImageMemory,
  VkImageView* colorImageView,VkFormat swapChainImageFormat,VkExtent2D swapChainExtent,
  VkSampleCountFlagBits msaaSamples,VkDevice device, VkPhysicalDevice physicalDevice);