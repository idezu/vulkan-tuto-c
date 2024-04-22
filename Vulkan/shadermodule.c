#include <vulkan/vulkan.h>

VkResult createShaderModule( VkShaderModule* module, char* code, int codeSize, VkDevice device)
	{
		VkShaderModuleCreateInfo createInfo = {
			.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			.codeSize = codeSize,
			.pCode = (uint32_t*) code,
		};

		VkResult result = vkCreateShaderModule(device, &createInfo, NULL, module);
		if (result != VK_SUCCESS)
			{
				return result;
			}
		return result;
	}