#pragma once

#include "vulkan/vulkan.h"
#include "vk_mem_alloc.h"
#include "vector"
#include "glm/glm.hpp"

namespace Engine {
struct Vertex
{
    glm::vec4 pos;
    glm::vec4 color;
    
    static VkVertexInputBindingDescription getBindingDescription(){
        
        VkVertexInputBindingDescription vertBindingDesc{};
        vertBindingDesc.binding = 0;
        vertBindingDesc.stride = sizeof(Vertex);
        vertBindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        
        return vertBindingDesc;
    }
    
    static std::vector<VkVertexInputAttributeDescription> getAttributeDescription(){
        
        std::vector<VkVertexInputAttributeDescription> vertAttribDesc(2);
        
        vertAttribDesc[0].binding = 0;
        vertAttribDesc[0].location = 0;
        vertAttribDesc[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        vertAttribDesc[0].offset = offsetof(Vertex, pos);
        
        vertAttribDesc[1].binding = 0;
        vertAttribDesc[1].location = 1;
        vertAttribDesc[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        vertAttribDesc[1].offset = offsetof(Vertex, color);
        
        return vertAttribDesc;
    }
};

class VulkanBuffer{
public:
    VulkanBuffer();
    ~VulkanBuffer();
    inline VkBuffer* GetBuffer(){return &m_buffer;}
        inline VmaAllocation* GetAllocation() {return &m_allocation;}
    void CreateBuffer(VkDevice& device, VmaAllocator& allocator, uint32_t size_in_bytes, VkBufferUsageFlags bufferUsageFlag, VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_CPU_TO_GPU);
    void CopyBuffer(VmaAllocator& allocator, void* buffer, uint32_t size_in_bytes);
    inline void DestroyBuffer(VmaAllocator& allocator) {
        vmaDestroyBuffer(allocator, m_buffer, m_allocation);
    }
private:
    VkBuffer m_buffer;
    VmaAllocation m_allocation;
};
}
