#include "framebuffer.h"
#include "debug.h"
#include <stddef.h>
#include <stdlib.h>
#include <vulkan/vulkan_core.h>

VkResult createFramebufers(VkFramebuffer **swapchainFramebuffers,
                           VkDevice device, VkRenderPass renderPass,
                           VkImageView *const swapchainImageViews,
                           VkExtent2D swapchainExtent,
                           uint16_t swapchainImageViewCount,
                           VkImageView depthImageView, VkImageView colorImageView) {
  *swapchainFramebuffers =
      calloc(swapchainImageViewCount, sizeof(VkFramebuffer));
  for (size_t i = 0; i < swapchainImageViewCount; i++) {
    VkImageView attachments[] = { colorImageView, depthImageView, swapchainImageViews[i], };

    VkFramebufferCreateInfo framebufferInfo = {
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .renderPass = renderPass,
        .attachmentCount = sizeof(attachments) / sizeof(attachments[0]),
        .pAttachments = attachments,
        .width = swapchainExtent.width,
        .height = swapchainExtent.height,
        .layers = 1,
    };

    VkResult res = vkCreateFramebuffer(device, &framebufferInfo, NULL,
                                       *swapchainFramebuffers + i);
    if (res) {
      error("failed to create framebuffer\n");
      return res;
    }
  }
  return VK_SUCCESS;
}
