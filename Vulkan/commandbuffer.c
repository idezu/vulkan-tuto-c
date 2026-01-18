#include "commandbuffer.h"
#include "debug.h"
#include <stdint.h>
#include <stdlib.h>
#include <vulkan/vulkan_core.h>

#include "graphicspipeline.h"
#include "../include/debug.h"

extern const int MAX_FRAMES_IN_FLIGHT;

VkResult createCommandBuffer(VkCommandBuffer **commandBuffer,
                             VkCommandPool commandPool, VkDevice device) {
  *commandBuffer = malloc(MAX_FRAMES_IN_FLIGHT * sizeof(VkCommandBuffer));
  if (*commandBuffer == NULL) {
    return VK_ERROR_MEMORY_MAP_FAILED;
  }
  VkCommandBufferAllocateInfo allocInfo = {
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      .commandPool = commandPool,
      .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
      .commandBufferCount = MAX_FRAMES_IN_FLIGHT,
  };

  VkResult res = vkAllocateCommandBuffers(device, &allocInfo, *commandBuffer);

  if (res != VK_SUCCESS) {
    error("failed to allocate command buffers\n");
    return res;
  }

  return VK_SUCCESS;
}

VkResult recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex,
                             VkRenderPass renderPass,
                             VkFramebuffer *swapChainFramebuffers,
                             VkExtent2D swapChainExtent,
                             VkPipeline graphicsPipeline,
                             VkBuffer vertexBuffer,
                             VkBuffer indexBuffer,
                             VkDescriptorSet descriptorSet,
                             VkPipelineLayout pipelineLayout,
                             uint32_t indexBufferSize) {
  VkCommandBufferBeginInfo beginInfo = {
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
      .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
  };

  VkResult res = vkBeginCommandBuffer(commandBuffer, &beginInfo);

  if (res != VK_SUCCESS) {
    error("failed to begin recording command buffers\n");
    return res;
  }

  VkClearValue clearValues[] = {
      {
          .color = {{0.0f, 0.0f, 0.0f, 0.0f}},
      },
      {
          .depthStencil = {1.0f,0}
      }
  };

  VkRenderPassBeginInfo renderPassInfo = {
      .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
      .renderPass = renderPass,
      .framebuffer = swapChainFramebuffers[imageIndex],
      .renderArea =
          {
              .offset = {0, 0},
              .extent = swapChainExtent,
          },
      .clearValueCount = sizeof(clearValues)/sizeof(clearValues[0]),
      .pClearValues = clearValues};

  vkCmdBeginRenderPass(commandBuffer, &renderPassInfo,
                       VK_SUBPASS_CONTENTS_INLINE);

  VkViewport viewport = {
      .x = 0.0f,
      .y = 0.0f,
      .width = swapChainExtent.width,
      .height = swapChainExtent.height,
      .minDepth = 0.0f,
      .maxDepth = 1.0f,
  };

  vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

  VkRect2D scissor = {
      .offset = {0, 0},
      .extent = swapChainExtent,
  };

  vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                    graphicsPipeline);

  VkBuffer vertexBuffers[] = {vertexBuffer};
  VkDeviceSize offsets[] = {0};
  vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

  vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, NULL);

  vkCmdDrawIndexed(commandBuffer, indexBufferSize/sizeof(uint32_t), 1, 0, 0, 0);

  vkCmdEndRenderPass(commandBuffer);

  res = vkEndCommandBuffer(commandBuffer);

  if (res != VK_SUCCESS) {
    error("failed to record command buffer\n");
    return res;
  }

  return VK_SUCCESS;
}

VkResult beginSingleTimeCommands(VkCommandBuffer* commandBuffer, VkCommandPool pool, VkDevice device) {
    VkCommandBufferAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandPool = pool,
        .commandBufferCount = 1,
    };

    VkResult res = vkAllocateCommandBuffers(device, &allocInfo, commandBuffer);
    CHECK(res,"failed to allocate the temporary command buffer\n");

    VkCommandBufferBeginInfo commandBufferBeginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    };

    res = vkBeginCommandBuffer(*commandBuffer, &commandBufferBeginInfo);
    CHECK(res,"failed to begin temporary command buffer\n");

    return VK_SUCCESS;
}

VkResult endSingleTimeCommands(VkCommandBuffer commandBuffer, VkQueue graphicsQueue, VkDevice device, VkCommandPool commandPool) {
    VkResult res = vkEndCommandBuffer(commandBuffer);
    CHECK(res,"failed to end temporary command buffer\n");

    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = &commandBuffer,
    };

    res = vkQueueSubmit(graphicsQueue, 1,  &submitInfo, VK_NULL_HANDLE);
    CHECK(res,"failed to submit command buffer\n");

    res = vkQueueWaitIdle(graphicsQueue);
    CHECK(res,"waiting for graphics queue failed\n");

    vkFreeCommandBuffers(device, commandPool,1,&commandBuffer);
    return VK_SUCCESS;
}