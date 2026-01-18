//
// Created by idezu on 17/01/2026.
//

#include "texture.h"

#include <stb_image.h>
#include <vulkan/vulkan_core.h>
#include <debug.h>
#include <math.h>
#include <string.h>

#include "buffer.h"
#include "commandbuffer.h"
#include "device.h"
#include "imageview.h"

VkResult generateMipmaps(VkImage image, VkFormat imageFormat, uint32_t texWidth, uint32_t texHeight, uint32_t mipLevels, VkQueue graphicsQueue, VkCommandPool pool, VkPhysicalDevice physicalDevice, VkDevice device) {

    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(physicalDevice, imageFormat, &formatProperties);

    if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
        error("texture image format does not support linear blitting\n");
        return VK_ERROR_FORMAT_NOT_SUPPORTED;
    }

    VkCommandBuffer commandBuffer;
    VkResult res = beginSingleTimeCommands(&commandBuffer, pool, device);
    CHECK(res);

    VkImageMemoryBarrier barrier = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .image = image,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseArrayLayer = 0,
            .layerCount = 1,
            .levelCount = 1,
        },
    };

    uint32_t mipWidth = texWidth;
    uint32_t mipHeight = texHeight;

    for (uint32_t i = 1; i < mipLevels; i++) {
        barrier.subresourceRange.baseMipLevel = i - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
            0, NULL,
            0, NULL,
            1, &barrier);

        VkImageBlit blit = {
            .srcOffsets = {
                {
                    0, 0, 0 ,
                },{
                    mipWidth, mipHeight, 1 ,
                }
            },
            .srcSubresource = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .mipLevel = i - 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
            .dstOffsets = {
                {
                    0, 0, 0
                }, {
                    mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1
                }
            },
            .dstSubresource = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .mipLevel = i,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
        };

        vkCmdBlitImage(commandBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, image ,VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1, &blit, VK_FILTER_LINEAR);

        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
            0, nullptr,
            0, nullptr,
            1, &barrier);
        if (mipWidth > 1) mipWidth /= 2;
        if (mipHeight > 1) mipHeight /= 2;
    }

    barrier.subresourceRange.baseMipLevel = mipLevels - 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(commandBuffer,
        VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
        0, nullptr,
        0, nullptr,
        1, &barrier);

    res = endSingleTimeCommands(commandBuffer, graphicsQueue, device, pool);
    CHECK(res);

    return VK_SUCCESS;
}

VkResult createTextureImage(VkImage* textureImage, VkDeviceMemory* textureImageMemory, uint32_t* mipLevels, VkDevice device, VkCommandPool pool, VkQueue graphicsQueue, VkPhysicalDevice physicalDevice) {
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load("textures/viking_room.png", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    *mipLevels = (uint32_t)log2((texWidth-texHeight>0)?texWidth:texHeight)+1;
    VkDeviceSize imageSize = texWidth * texHeight * 4;

    if (pixels == NULL) {
        error("failed to load texture image\n");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    VkResult res = createBuffer(
        &stagingBuffer,
        &stagingBufferMemory,
        imageSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
        | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, device, physicalDevice);
    CHECK(res);

    void* data;
    res = vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
    CHECK(res,"failed to map staging buffer to copy the image\n");
        memcpy(data, pixels, imageSize);
    vkUnmapMemory(device, stagingBufferMemory);
    stbi_image_free(pixels);

    res = createImage(textureImage,
        textureImageMemory,
        texWidth,
        texHeight,*mipLevels,VK_SAMPLE_COUNT_1_BIT,
        VK_FORMAT_R8G8B8A8_SRGB,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        device,
        physicalDevice);

    CHECK(res,"failed to create tex image\n");

    res = transitionImageLayout(*textureImage,VK_FORMAT_R8G8B8A8_SRGB,  VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, *mipLevels, device, pool, graphicsQueue);
    CHECK(res, "failed to transition image layout to transfer\n");

    res = copyBufferToImage(*textureImage,stagingBuffer, texWidth, texHeight, device, pool, graphicsQueue);
    CHECK(res,"failed to copy buffer to image\n");

    //res = transitionImageLayout(*textureImage,VK_FORMAT_R8G8B8A8_SRGB,  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, *mipLevels, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, device, pool, graphicsQueue);
    res = generateMipmaps(*textureImage, VK_FORMAT_R8G8B8A8_SRGB, texWidth, texHeight, *mipLevels, graphicsQueue, pool, physicalDevice, device);
    CHECK(res, "failed to generate mipmaps\n");

    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingBufferMemory, nullptr);

    return VK_SUCCESS;
}

VkResult createTextureImageView(VkImageView* textureImageView, uint32_t mipLevels, VkImage textureImage, VkDevice device) {
    VkResult res = createImageView(textureImageView, mipLevels, textureImage, VK_IMAGE_ASPECT_COLOR_BIT, VK_FORMAT_R8G8B8A8_SRGB, device);
    CHECK(res,"failed to create image view\n");

    return VK_SUCCESS;
}

VkResult createTextureSampler(VkSampler* textureSampler, VkDevice device, VkPhysicalDevice physicalDevice) {
    VkPhysicalDeviceProperties properties = {};
    vkGetPhysicalDeviceProperties(physicalDevice, &properties);


    VkSamplerCreateInfo samplerCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .magFilter = VK_FILTER_LINEAR,
        .minFilter = VK_FILTER_LINEAR,
        .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .anisotropyEnable =  VK_TRUE,
        .maxAnisotropy = properties.limits.maxSamplerAnisotropy,
        .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
        .unnormalizedCoordinates = VK_FALSE,
        .compareEnable = VK_FALSE,
        .compareOp = VK_COMPARE_OP_ALWAYS,
        .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
        .minLod = 0,
        .maxLod = VK_LOD_CLAMP_NONE,
    };

    VkResult res = vkCreateSampler(device, &samplerCreateInfo, nullptr, textureSampler);
    CHECK(res, "failed to create texture sampler\n");

    return VK_SUCCESS;
}