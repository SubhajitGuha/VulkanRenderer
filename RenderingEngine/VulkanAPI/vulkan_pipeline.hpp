#pragma once
#include "vulkan/vulkan.h"
#include <vector>

namespace Engine {
class VulkanPipeline
{
public:
    VulkanPipeline() = default;
    ~VulkanPipeline() = default;
    void CreatePipeline(VkDevice& device, VkRenderPass& renderPass);
    void CreatePipelineLayout(VkDevice& device, std::vector<VkDescriptorSetLayout> descriptor_set_layouts);

    inline VkPipeline GetPipeline() {return m_pipeline;}
    inline VkPipelineLayout GetPipelineLayout() {return m_pipelineLayout;}
    void CleanUp(VkDevice& device);

private:
    VkShaderModule CreateShaderModule(VkDevice& device, const std::vector<char>& code);
private:
    VkPipeline m_pipeline;
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
};
}
