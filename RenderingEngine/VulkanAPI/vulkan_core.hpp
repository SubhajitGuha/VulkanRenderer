//
//  VulkanCore.hpp
//  vulkan_learning
//
//  Created by Subhajit on 15/06/24.
//
#pragma once
#include "../Window/window.hpp"
#include <vulkan/vulkan.h>
#include "vk_mem_alloc.h"
#include "vulkan_physical_device.hpp"
#include "vulkan_pipeline.hpp"
#include "vulkan_buffer.hpp"
#include <vector>
#include <glm/vec4.hpp>
#include <memory>
#include <stack>
#include <functional>
#include <fstream>

namespace Engine
{
    class Vulkan{
    public:
        Vulkan();
        ~Vulkan();
        void Init();
        void UpdateWindow();
        inline std::shared_ptr<Window> GetWindow(){return window;}
        inline uint32_t GetNumSwapChainImages(){return m_images.size();}
        inline VkImage GetImage(uint32_t index){return m_images[index];}
        //Gives the swapchain Image Extent
        inline VkExtent2D GetImageExtent() {return m_swapChainImageExtent;}
//        inline VkPipeline GetPipeline() {return m_pipeline->GetPipeline();}
//        inline VkPipelineLayout GetPipelineLayout() {return m_pipeline->GetPipelineLayout();}
        inline VkDevice& GetDevice() {return m_device;}
        inline VkBuffer* GetBuffer() {return m_vertex_buffer->GetBuffer();}
        inline VkQueue GetQueue() {return m_queue;}
        inline VkRenderPass& GetRenderPass(){return m_renderPass;}
        inline VmaAllocator& GetAllocator(){return m_vmaAllocator;}
        inline static void AddDescriptorSetLayout(VkDescriptorSetLayout& layout){ m_descriptorSetLayouts.push_back(layout);}
        void CreateCommandBuffer(uint32_t count, VkCommandBuffer *cmdBuffer);
        void BeginCommandBuffer(VkCommandBuffer buffer, VkCommandBufferUsageFlags flags);
        void EndCommandBuffer(VkCommandBuffer buffer);
        void BeginRenderPass(VkCommandBuffer& buffer, int imageIndex, VkClearValue& clearColor);
        void EndRenderPass(VkCommandBuffer& buffer);
        uint32_t AcquireNextImage();
        void SubmitAsync(VkCommandBuffer cmdBuffer);
        void Present(uint32_t imageIndex);
        static std::vector<char> ReadFile(const std::string& filename);
//        void Run();
    private:
        void cleanup();

        void CreateInstance(std::vector<const char*>& extensions);
        void CreateDebugCallback();
        void CreateSurface();
        void CreatePhysicalDevice();
        void CreateLogicalDevice();
        void CreateSwapChain();
        void CreateCommandBufferPool();
        void CreateImageView(VkDevice& device, VkImage& image, VkImageView& imageView, VkFormat format, VkImageViewType viewType, VkImageAspectFlags aspectFlags, uint32_t layerCnt, uint32_t mipLevel);
        void CreateQueue();
        void CreateSemaphores(VkSemaphore* semaphore);
        void CreateFences(VkFence* fence);
        void CreateRenderPass();
        void CreateGraphicsPipeline();
        void CreateFrameBuffers();
        void CreateMemoryAllocator();
        void CreateVertexBuffer();
    private:
        bool isDeviceSutable(VkPhysicalDevice& device);
    public:
        static std::vector<VkDescriptorSetLayout> m_descriptorSetLayouts;
    private:
        VkInstance VulkanInstance = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT m_debugMessanger = VK_NULL_HANDLE;
        VkSurfaceKHR m_surface;
        VkSwapchainKHR m_swapChain = VK_NULL_HANDLE;
        VkCommandPool m_cmdPool = VK_NULL_HANDLE;
        VkQueue m_queue = VK_NULL_HANDLE;
        VkRenderPass m_renderPass = VK_NULL_HANDLE;
        VkSemaphore m_presentCompleateSemaphore = VK_NULL_HANDLE;
        VkSemaphore m_renderCompleateSemaphore = VK_NULL_HANDLE;
        VkFence m_inFlightFence;
        VkFormat m_surfaceImageFormat;
        VkExtent2D m_swapChainImageExtent;
        VmaAllocator m_vmaAllocator;
        std::shared_ptr<VulkanBuffer> m_vertex_buffer;
        std::vector<VkFramebuffer> m_swapChainFramebuffers;
        std::vector<Vertex> vertices;

        VulkanPhysicalDevices m_physicalDevices;
        std::vector<VkImage> m_images;
        std::vector<VkImageView> m_imageViews;
        VkDevice m_device;
        uint32_t m_queueFamilyIndex;
        std::shared_ptr<Window> window;
        
        std::stack<std::function<void()>> m_deleationStack;
    };
}
