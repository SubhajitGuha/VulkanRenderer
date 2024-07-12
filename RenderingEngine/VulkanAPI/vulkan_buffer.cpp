#include "vulkan_buffer.hpp"

namespace Engine {

    VulkanBuffer::VulkanBuffer(){
    }
    VulkanBuffer::~VulkanBuffer(){
        
    }
    void VulkanBuffer::CreateBuffer(VkDevice& device, VmaAllocator& allocator, uint32_t size, VkBufferUsageFlags bufferUsageFlag, VmaMemoryUsage memoryUsage){
        
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.size = size;
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.usage = bufferUsageFlag;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        
        VmaAllocationCreateInfo info{};
        info.usage = memoryUsage;

        if(vmaCreateBuffer(allocator, &bufferInfo, &info, &m_buffer, &m_allocation, nullptr) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create vertex buffer and allocate memory for it");
        }
    }

    void VulkanBuffer::CopyBuffer(VmaAllocator& allocator, void* buffer, uint32_t size){
        //get the memory spot in the gpu so that we can copy the data
        void* data;
        vmaMapMemory(allocator, m_allocation, &data);
        memcpy(data, buffer, size);
        vmaUnmapMemory(allocator, m_allocation);
    }
}
