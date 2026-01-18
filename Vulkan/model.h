//
// Created by idezu on 17/01/2026.
//

#ifndef VULKAN_TUTO_C_MODEL_H
#define VULKAN_TUTO_C_MODEL_H
#include <vulkan/vulkan_core.h>

#include "graphicspipeline.h"

VkResult loadModel(Vertex** vertices, uint32_t* vertexBufferSize, uint32_t** indices, uint32_t* indexBufferSize);

#endif //VULKAN_TUTO_C_MODEL_H