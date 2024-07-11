#include "vulkan_buffer.hpp"

namespace Engine {

VulkanBuffer::VulkanBuffer(){
    
    vertices = {
        {{0.0,-0.5,0.0,1.0}, {0.5,0.8,0.0,1.0}},
        {{0.5,0.0,0.0,1.0}, {0.0,1.0,0.6,1.0}},
        {{-0.5,0.0,0.0,1.0}, {0.0,0.0,1.0,1.0}},
    };
}
VulkanBuffer::~VulkanBuffer(){
    
}
void VulkanBuffer::CreateVertexBuffer(VkDevice& device, VmaAllocator& allocator){
    
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.size = sizeof(vertices[0]) * vertices.size();
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    
    if (vkCreateBuffer(device, &bufferInfo, nullptr, &m_buffer) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create vertex buffer!");
    }
    
    VmaAllocationCreateInfo info{};
    info.usage = VmaMemoryUsage::VMA_MEMORY_USAGE_CPU_TO_GPU;

    if(vmaCreateBuffer(allocator, &bufferInfo, &info, &m_buffer, &m_allocation, nullptr) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create vertex buffer and allocate memory for it");
    }
    
    //get the memory spot in the gpu so that we can copy the data
    void* data;
    vmaMapMemory(allocator, m_allocation, &data);
    memcpy(data, vertices.data(), sizeof(vertices[0]) * vertices.size());
    vmaUnmapMemory(allocator, m_allocation);
}

}
