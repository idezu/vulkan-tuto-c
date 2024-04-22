#include <stdlib.h>

#include <vulkan/vulkan.h>

#include <debug.h>

#include "File/binary.h"
#include "shadermodule.h"

VkResult createGraphicsPipeline(VkDevice device, VkExtent2D swapChainExtent)
	{
		char* vertShaderCode = NULL;
		char* fragShaderCode = NULL;

		VkShaderModule vertShaderMod = NULL;
		VkShaderModule fragShaderMod = NULL;

		debug("reading shaders bytecode\n");

		int vertSize = readFileB("shaders/vertex.spv",&vertShaderCode);
		int fragSize = readFileB("shaders/fragment.spv",&fragShaderCode);

		if (vertSize < 0 || fragSize < 0)
			{
				error("failed to read shaders");
				return VK_ERROR_MEMORY_MAP_FAILED;
			}
		

		createShaderModule(&vertShaderMod, vertShaderCode,vertSize,device);
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

		vkDestroyShaderModule(device, vertShaderMod, NULL);
		
		VkPipelineShaderStageCreateInfo fragShaderStageInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
			.module = fragShaderMod,
			.pName = "main",
		};

		vkDestroyShaderModule(device, fragShaderMod, NULL);

		VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

		VkDynamicState dynamicStates[] = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};

		debug("stating that the viewport and the scissor are dynamic\n");

		VkPipelineDynamicStateCreateInfo dynamicState =  {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
			.dynamicStateCount = sizeof(dynamicStates)/sizeof(VkDynamicState),
			.pDynamicStates = dynamicStates
		};

		VkPipelineVertexInputStateCreateInfo vertexInputInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
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

		VkRect2D scissor = {
			.offset = {0,0},
			.extent = swapChainExtent
		};

		debug("creating viewport state\n");

		VkPipelineViewportStateCreateInfo viewportState = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
			.viewportCount = 1,
			.pViewports = &viewport,
			.scissorCount = 1,
			.pScissors = &scissor,
		};

		return VK_SUCCESS;
	}
