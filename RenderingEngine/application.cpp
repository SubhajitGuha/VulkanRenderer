
#include "application.hpp"
#include "Window/window.hpp"

using namespace Engine;

Application::Application(){
    
    vulkan_renderer = std::make_shared<Vulkan>();
    vulkan_renderer->Init();
    
    m_numImages = vulkan_renderer->GetNumSwapChainImages();
    m_cmdBuffers.resize(m_numImages);
    
    vulkan_renderer->CreateCommandBuffer(m_numImages, m_cmdBuffers.data());
    RecordCommandBuffer();
}
Application::~Application(){
    
}

void Application::RecordCommandBuffer()
{
    VkClearColorValue clearColor = {0.0,1.0,0.0,1.0};
    VkClearValue clearVal = {0.1,0.1,0.1,1.0};
    VkImageSubresourceRange imageRange = {};
    imageRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageRange.baseArrayLayer = 0;
    imageRange.layerCount = 1;
    imageRange.baseMipLevel = 0;
    imageRange.levelCount = 1;
    
    for (int i=0 ;i<m_numImages; i++)
    {
        vulkan_renderer->BeginCommandBuffer(m_cmdBuffers[i], VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);
        vulkan_renderer->BeginRenderPass(m_cmdBuffers[i], i, clearVal);
        vkCmdBindPipeline(m_cmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, vulkan_renderer->GetPipeline());
        
        VkDeviceSize offset = 0;
        vkCmdBindVertexBuffers(m_cmdBuffers[i], 0, 1, vulkan_renderer->GetBuffer(), &offset);
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(vulkan_renderer->GetImageExtent().width);
        viewport.height = static_cast<float>(vulkan_renderer->GetImageExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(m_cmdBuffers[i], 0, 1, &viewport);
        
        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = vulkan_renderer->GetImageExtent();
        vkCmdSetScissor(m_cmdBuffers[i], 0, 1, &scissor);
        vkCmdDraw(m_cmdBuffers[i], 3, 1, 0, 0);
//        vkCmdClearColorImage(m_cmdBuffers[i], vulkan_renderer->GetImage(i), VK_IMAGE_LAYOUT_GENERAL, &clearColor, 1, &imageRange);
        
        vulkan_renderer->EndRenderPass(m_cmdBuffers[i]);
        vulkan_renderer->EndCommandBuffer(m_cmdBuffers[i]);
    }
    std::cout<<"command buffers recorded\n";
}

void Application::Run(){
    auto window = vulkan_renderer->GetWindow();
    while(!window->is_windowClosd())
    {
        window->update();
        uint32_t imgIndex = vulkan_renderer->AcquireNextImage();
        vulkan_renderer->SubmitAsync(m_cmdBuffers[imgIndex]);
        vulkan_renderer->Present(imgIndex);
        vkQueueWaitIdle(vulkan_renderer->GetQueue());

    }
    vkDeviceWaitIdle(vulkan_renderer->GetDevice());
}
