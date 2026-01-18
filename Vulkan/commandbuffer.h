#pragma once

#include <vulkan/vulkan_core.h>
VkResult createCommandBuffer(VkCommandBuffer **commandBuffer,
                             VkCommandPool commandPool, VkDevice device);

VkResult recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex,
                             VkRenderPass renderPass,
                             VkFramebuffer *swapChainFramebuffers,
                             VkExtent2D swapChainExtent,
                             VkPipeline graphicsPipeline,
                             VkBuffer vertexBuffer,
                             VkBuffer indexBuffer,
                             VkDescriptorSet descriptorSet,
                             VkPipelineLayout pipelineLayout,
                             uint32_t indexBufferSize);

VkResult beginSingleTimeCommands(VkCommandBuffer* commandBuffer, VkCommandPool pool, VkDevice device);

VkResult endSingleTimeCommands(VkCommandBuffer commandBuffer, VkQueue graphicsQueue, VkDevice device, VkCommandPool commandPool);
