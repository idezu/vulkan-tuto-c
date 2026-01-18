#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

#include <vulkan/vk_platform.h>
#include <vulkan/vulkan.h>

#include <debug.h>
#include <vulkan/vulkan_core.h>

#include "device.h"
#include "File/binary.h"
#include "shadermodule.h"
#include "graphicspipeline.h"

#include <string.h>

#include "buffer.h"
#include "imageview.h"

VkResult createColorRessources(
  VkImage* colorImage,
  VkDeviceMemory* colorImageMemory,
  VkImageView* colorImageView,VkFormat swapChainImageFormat,VkExtent2D swapChainExtent,
  VkSampleCountFlagBits msaaSamples,VkDevice device, VkPhysicalDevice physicalDevice) {
    VkFormat colorFormat = swapChainImageFormat;

    VkResult res = createImage(colorImage, colorImageMemory, swapChainExtent.width, swapChainExtent.height, 1,
        msaaSamples, colorFormat, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, device, physicalDevice);
    CHECK(res,"Failed to create color ressources\n");

    res = createImageView(colorImageView, 1, *colorImage,
        VK_IMAGE_ASPECT_COLOR_BIT, colorFormat, device);
    CHECK(res, "failed to create color image view\n");

    return VK_SUCCESS;
}

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
    ) {
    VkFormat depthFormat = findDepthFormat(physicalDevice);
    if (depthFormat == VK_FORMAT_UNDEFINED) {
        return VK_ERROR_UNKNOWN;
    }
    VkResult res = createImage(
        depthImage,
        depthImageMemory,
        swapChainExtent.width,
        swapChainExtent.height,
        1,
        msaaSamples,
        depthFormat,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        device,
        physicalDevice
        );
    CHECK(res, "failed to create depth image\n");

    res = createImageView(depthImageView,1,*depthImage, VK_IMAGE_ASPECT_DEPTH_BIT, depthFormat, device);
    CHECK(res, "failed to create depth image view\n");

    res = transitionImageLayout(
        *depthImage,
        depthFormat,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,1,
        device,
        commandPool,
        graphicsQueue
        );
    CHECK(res, "failed to create depth image view\n");

    return VK_SUCCESS;
}

extern const int MAX_FRAMES_IN_FLIGHT;

VkResult createUniformBuffers(VkBuffer** uniformBuffers,VkDeviceMemory** uniformBuffersMemory, void** uniformBuffersMapped, VkDevice device, VkPhysicalDevice physicalDevice) {
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);
    info("ubo size is %lu\n",bufferSize);

    *uniformBuffers = malloc(MAX_FRAMES_IN_FLIGHT*sizeof(VkBuffer));
    if (*uniformBuffers == NULL) {
        error("failed to allocate uniform buffers\n");
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }
    *uniformBuffersMemory = malloc(MAX_FRAMES_IN_FLIGHT*sizeof(VkDeviceMemory));
    if (*uniformBuffersMemory == NULL) {
        error("failed to allocate uniform buffers memory\n");
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }
    *uniformBuffersMapped = malloc(MAX_FRAMES_IN_FLIGHT*sizeof(void*));
    if (*uniformBuffersMapped == NULL) {
        error("failed to allocate uniform buffers memory\n");
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    for (size_t i = 0; i < (size_t)MAX_FRAMES_IN_FLIGHT; i++) {
        VkResult res = createBuffer(*uniformBuffers+i,*uniformBuffersMemory+i,bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
                                    , VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
                                    ,device, physicalDevice);
        CHECK(res, "failed to allocate the %luth uniform buffer\n",i);
        res = vkMapMemory(device, (*uniformBuffersMemory)[i], 0, bufferSize, 0, ((void**)(*uniformBuffersMapped)) + i);
        CHECK(res, "failed to map the %luth uniform buffer\n",i);
    }
    return VK_SUCCESS;
}

VkResult createDescriptorSetLayout(VkDescriptorSetLayout* descriptorSetLayout, VkDevice device) {
    VkDescriptorSetLayoutBinding uboLayoutBinding = {
        .binding = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
    };

    VkDescriptorSetLayoutBinding samplerLayoutBinding = {
        .binding = 1,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
    };

    VkDescriptorSetLayoutBinding bindings[] = {
        uboLayoutBinding,
        samplerLayoutBinding,
    };


    VkDescriptorSetLayoutCreateInfo layoutInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = sizeof(bindings)/sizeof(bindings[0]),
        .pBindings = bindings,
    };

    VkResult res = vkCreateDescriptorSetLayout(device, &layoutInfo, NULL, descriptorSetLayout);
    CHECK(res, "failed to create the descriptor set layout\n");

    return VK_SUCCESS;
}

VkResult createDescriptorPool(VkDescriptorPool* descriptorPool, VkDevice device) {
    VkDescriptorPoolSize poolSize[] = {
        {
            .descriptorCount = MAX_FRAMES_IN_FLIGHT,
            .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        },
        {
            .descriptorCount = MAX_FRAMES_IN_FLIGHT,
            .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        }
    };

    VkDescriptorPoolCreateInfo poolInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .poolSizeCount = sizeof(poolSize)/sizeof(poolSize[0]),
        .pPoolSizes = poolSize,
        .maxSets = MAX_FRAMES_IN_FLIGHT,
    };

    VkResult res = vkCreateDescriptorPool(device,&poolInfo, NULL,descriptorPool);
    CHECK(res);

    return VK_SUCCESS;
}

VkResult createDescriptorSets(VkDescriptorSet** descriptorSets, VkDescriptorSetLayout descriptorSetLayout,
    VkDescriptorPool descriptorPool, VkDevice device, VkBuffer* uniformBuffers, VkImageView textureImageView,
    VkSampler textureSampler) {
    *descriptorSets = malloc(MAX_FRAMES_IN_FLIGHT*sizeof(VkDescriptorSet));
    if (*descriptorSets == NULL) {
        error("failed to allocate descriptor sets\n");
        return VK_ERROR_MEMORY_MAP_FAILED;
    }

    VkDescriptorSetLayout layouts[] = {
        descriptorSetLayout, descriptorSetLayout,
    };

    VkDescriptorSetAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = descriptorPool,
        .descriptorSetCount = MAX_FRAMES_IN_FLIGHT,
        .pSetLayouts = layouts,
    };

    VkResult res = vkAllocateDescriptorSets(device,&allocInfo, *descriptorSets);
    CHECK(res,"failed to allocate descriptor sets\n");

    for (size_t i = 0; i < (size_t)MAX_FRAMES_IN_FLIGHT; i++) {
        VkDescriptorBufferInfo bufferInfo = {
            .buffer = uniformBuffers[i],
            .offset = 0,
            .range = sizeof(UniformBufferObject),
        };

        VkDescriptorImageInfo imageInfo = {
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            .imageView = textureImageView,
            .sampler = textureSampler,
        };

        VkWriteDescriptorSet descriptorWrites[] = {
            {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet =  (*descriptorSets)[i],
                .dstBinding = 0,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .pBufferInfo = &bufferInfo,
            },
            {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet =  (*descriptorSets)[i],
                .dstBinding = 1,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .pImageInfo = &imageInfo,
            }
        };

        vkUpdateDescriptorSets(device, sizeof(descriptorWrites)/sizeof(descriptorWrites[0]),
            descriptorWrites, 0, NULL);
    }

    return VK_SUCCESS;
}

VkResult createVertexBuffer(VkBuffer* vertexBuffer, VkDeviceMemory* vertexBufferMemory, Vertex* vertices,
    uint32_t vertexBufferSize, VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool,
    VkQueue graphicsQueue) {

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    VkResult res = createBuffer(
        &stagingBuffer,
        &stagingBufferMemory,
        vertexBufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        device,
        physicalDevice
        );
    if (res != VK_SUCCESS) {
        error("failed to create staging buffer\n");
        return res;
    }

    void* data = NULL;
    res = vkMapMemory(device,stagingBufferMemory,0,vertexBufferSize,0,&data);
    if (res != VK_SUCCESS) {
        error("failed to map buffer memory\n");
        return res;
    }

    memcpy(data,vertices,vertexBufferSize);

    vkUnmapMemory(device,stagingBufferMemory);

    res = createBuffer(
        vertexBuffer,
        vertexBufferMemory,
        vertexBufferSize,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        device,
        physicalDevice
        );
    if (res != VK_SUCCESS) {
        error("failed to create vertex buffer\n");
        return res;
    }
    res = copyBuffer(stagingBuffer, *vertexBuffer, vertexBufferSize, commandPool, device, graphicsQueue);
    if (res != VK_SUCCESS) {
        error("failed to copy staging buffer to vertex buffer\n");
        return res;
    }
    vkFreeMemory(device,stagingBufferMemory,NULL);
    vkDestroyBuffer(device,stagingBuffer,NULL);
    return VK_SUCCESS;
}

VkResult createIndexBuffer(VkBuffer* indexBuffer, VkDeviceMemory* indexBufferMemory, uint32_t* indices,
    uint32_t indexBufferSize, VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool,
    VkQueue graphicsQueue) {
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    VkResult res = createBuffer(&stagingBuffer, &stagingBufferMemory, indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, device, physicalDevice);
    if (res != VK_SUCCESS) {
        error("failed to create staging buffer\n");
        return res;
    }

    void* data = NULL;
    res = vkMapMemory(device,stagingBufferMemory,0,indexBufferSize,0,&data);
    if (res != VK_SUCCESS) {
        error("failed to map buffer memory\n");
        return res;
    }

    memcpy(data,indices,indexBufferSize);

    vkUnmapMemory(device,stagingBufferMemory);

    res = createBuffer(indexBuffer, indexBufferMemory, indexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, device, physicalDevice);
    if (res != VK_SUCCESS) {
        error("failed to create vertex buffer\n");
        return res;
    }
    res = copyBuffer(stagingBuffer, *indexBuffer, indexBufferSize, commandPool, device, graphicsQueue);
    if (res != VK_SUCCESS) {
        error("failed to copy staging buffer to vertex buffer\n");
        return res;
    }
    vkFreeMemory(device,stagingBufferMemory,NULL);
    vkDestroyBuffer(device,stagingBuffer,NULL);
    return VK_SUCCESS;
}

VkResult createPipelineLayout(VkPipelineLayout *pipelineLayout,
                              VkDevice device, VkDescriptorSetLayout descriptorSetLayout) {
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 1,
        .pSetLayouts = &descriptorSetLayout,
    };
  VkResult res =
      vkCreatePipelineLayout(device, &pipelineLayoutInfo, NULL, pipelineLayout);

  if (res != VK_SUCCESS) {
    error("failed to create pipeline layout\n");
    return res;
  }
  return VK_SUCCESS;
}

VkResult createGraphicsPipeline(VkPipeline *graphicsPipeline,
                                VkPipelineLayout *pipelineLayout,
                                VkDevice device, VkExtent2D swapChainExtent,
                                VkRenderPass renderPass,
                                VkDescriptorSetLayout descriptorSetLayout, VkSampleCountFlagBits msaaSamples) {
  char *vertShaderCode = NULL;
  char *fragShaderCode = NULL;

  VkShaderModule vertShaderMod = NULL;
  VkShaderModule fragShaderMod = NULL;

  debug("reading shaders bytecode\n");

  int vertSize = readFileB("shaders/vertex.spv", &vertShaderCode);
  int fragSize = readFileB("shaders/fragment.spv", &fragShaderCode);

  if (vertSize < 0 || fragSize < 0) {
    error("failed to read shaders");
    return VK_ERROR_MEMORY_MAP_FAILED;
  }

  createShaderModule(&vertShaderMod, vertShaderCode, vertSize, device);
  createShaderModule(&fragShaderMod, fragShaderCode, fragSize, device);

  free(vertShaderCode);
  free(fragShaderCode);

  debug("creating shader module info\n");

  VkPipelineShaderStageCreateInfo vertShaderStageInfo = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .stage = VK_SHADER_STAGE_VERTEX_BIT,
      .module = vertShaderMod,
      .pName = "main",
  };

  VkPipelineShaderStageCreateInfo fragShaderStageInfo = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
      .module = fragShaderMod,
      .pName = "main",
  };

  VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo,
                                                    fragShaderStageInfo};

  VkDynamicState dynamicStates[] = {VK_DYNAMIC_STATE_VIEWPORT,
                                    VK_DYNAMIC_STATE_SCISSOR};

  debug("stating that the viewport and the scissor are dynamic\n");

  VkPipelineDynamicStateCreateInfo dynamicState = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
      .dynamicStateCount = sizeof(dynamicStates) / sizeof(VkDynamicState),
      .pDynamicStates = dynamicStates};

    VkVertexInputBindingDescription bindingDescription = {
        .binding = 0,
        .stride = sizeof(Vertex),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
    };

    VkVertexInputAttributeDescription attributeDescriptions[] = {
        {
            .binding = 0,
            .location = 0,
            .offset = offsetof(Vertex,pos),
            .format = VK_FORMAT_R32G32B32_SFLOAT,
        },
        {
            .binding = 0,
            .location = 1,
            .offset = offsetof(Vertex,color),
            .format = VK_FORMAT_R32G32B32_SFLOAT,
        },
        {
            .binding = 0,
            .location = 2,
            .format = VK_FORMAT_R32G32_SFLOAT,
            .offset = offsetof(Vertex,texCoord),
        },
    };

  VkPipelineVertexInputStateCreateInfo vertexInputInfo = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
      .vertexBindingDescriptionCount = 1,
      .pVertexBindingDescriptions = &bindingDescription,
      .vertexAttributeDescriptionCount = sizeof(attributeDescriptions) / sizeof(attributeDescriptions[0]),
      .pVertexAttributeDescriptions = attributeDescriptions,
  };

  VkPipelineInputAssemblyStateCreateInfo inputAssembly = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
      .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
      .primitiveRestartEnable = VK_FALSE,
  };

  VkViewport viewport = {
      .x = 0.0f,
      .y = 0.0f,
      .width = swapChainExtent.width,
      .height = swapChainExtent.height,
      .minDepth = 0.0f,
      .maxDepth = 1.0f,
  };

  VkRect2D scissor = {.offset = {0, 0}, .extent = swapChainExtent};

  debug("creating viewport state\n");

  VkPipelineViewportStateCreateInfo viewportState = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
      .viewportCount = 1,
      .pViewports = &viewport,
      .scissorCount = 1,
      .pScissors = &scissor,
  };

  VkPipelineRasterizationStateCreateInfo rasterizer = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
      .depthClampEnable = VK_FALSE,
      .rasterizerDiscardEnable = VK_FALSE,
      .polygonMode = VK_POLYGON_MODE_FILL,
      .lineWidth = 1.0f,
      .cullMode = VK_CULL_MODE_BACK_BIT,
      .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
      .depthBiasEnable = VK_FALSE,
  };

  VkPipelineMultisampleStateCreateInfo multisampling = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
      .rasterizationSamples = msaaSamples,
      .sampleShadingEnable = VK_TRUE,
      .minSampleShading = 0.2f,
  };

  VkPipelineColorBlendAttachmentState colorBlendAttachment = {
      .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
      .blendEnable = VK_TRUE,
      .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
      .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
      .colorBlendOp = VK_BLEND_OP_ADD,
      .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
      .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
      .alphaBlendOp = VK_BLEND_OP_ADD,
  };

  VkPipelineColorBlendStateCreateInfo colorBlending = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
      .logicOpEnable = VK_FALSE,
      .logicOp = VK_LOGIC_OP_COPY,
      .attachmentCount = 1,
      .pAttachments = &colorBlendAttachment,
  };

  VkResult res = createPipelineLayout(pipelineLayout, device, descriptorSetLayout);
  if (res != VK_SUCCESS) {
    return res;
  }

    VkPipelineDepthStencilStateCreateInfo depthStencil = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
      .depthTestEnable = VK_TRUE,
      .depthWriteEnable = VK_TRUE,
      .depthCompareOp = VK_COMPARE_OP_LESS,
      .depthBoundsTestEnable = VK_FALSE,
      .minDepthBounds = 0.0f,
      .maxDepthBounds = 1.0f,
      .stencilTestEnable = VK_FALSE,
      .front = {},
      .back = {},
  };

  VkGraphicsPipelineCreateInfo pipelineInfo = {
      .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
      .stageCount = 2,
      .pStages = shaderStages,
      .pVertexInputState = &vertexInputInfo,
      .pInputAssemblyState = &inputAssembly,
      .pViewportState = &viewportState,
      .pRasterizationState = &rasterizer,
      .pMultisampleState = &multisampling,
      .pColorBlendState = &colorBlending,
      .pDynamicState = &dynamicState,
      .pDepthStencilState = &depthStencil,
      .layout = *pipelineLayout,
      .renderPass = renderPass,
      .subpass = 0,
      .basePipelineHandle = NULL,
      .basePipelineIndex = -1,
  };

  res = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo,
                                  NULL, graphicsPipeline);
  if (res != VK_SUCCESS) {
    error("failed to create graphics pipeline\n");
    return res;
  }

  vkDestroyShaderModule(device, vertShaderMod, NULL);
  vkDestroyShaderModule(device, fragShaderMod, NULL);

  return VK_SUCCESS;
}
