
#include "application.hpp"
#include "Window/window.hpp"
#include "glm/glm.hpp"
#include <functional>

#define BIND_FN(x) std::bind(&Application::x, this, std::placeholders::_1)
using namespace Engine;

Application::Application(){
    m_window = std::make_shared<Window>("Vulkan Renderer", 1920, 1080);
    m_window->SetCallbackEvent(BIND_FN(OnEvent));
    
    m_camera = Camera::GetCamera(CameraType::SCENE_CAMERA);
    m_camera->SetViewportSize(1920/1080.0);
    m_camera->SetCameraPosition({0.0,0.0,-10});
    vulkan_renderer = std::make_shared<Vulkan>(m_window);

    
    m_numImages = vulkan_renderer->GetNumSwapChainImages();
    m_cmdBuffers.resize(m_numImages);
    m_cameraProjViewBuffer.resize(m_numImages);
    
    vulkan_renderer->CreateCommandBuffer(m_numImages, m_cmdBuffers.data());
    CreateDescriptorSets(vulkan_renderer->GetDevice());
    
    m_pipeline = std::make_shared<VulkanPipeline>();
    m_pipeline->CreatePipelineLayout(vulkan_renderer->GetDevice(), {m_descSetLayout});
    m_pipeline->CreatePipeline(vulkan_renderer->GetDevice(), vulkan_renderer->GetRenderPass());
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
        
        vkCmdBindPipeline(m_cmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetPipeline());
        vkCmdBindDescriptorSets(m_cmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetPipelineLayout(), 0, 1, &m_descriptorSets[i], 0, nullptr);
        
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

void Application::CreateDescriptorSets(VkDevice& m_device)
{
    VkDescriptorSetLayoutBinding dsLayoutBinding{};
    dsLayoutBinding.binding = 0;
    dsLayoutBinding.descriptorCount = 1;
    dsLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    dsLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT; //use it from the vertex shader
    
    VkDescriptorSetLayoutCreateInfo dsLayoutInfo{};
    dsLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    dsLayoutInfo.pNext = nullptr;
    dsLayoutInfo.flags = 0;
    dsLayoutInfo.bindingCount = 1;
    dsLayoutInfo.pBindings = &dsLayoutBinding;
    
    auto res = vkCreateDescriptorSetLayout(m_device, &dsLayoutInfo, nullptr, &m_descSetLayout);
    
    if (res == VK_SUCCESS)
        std::cout<<"Descriptor set Created!!"<<std::endl;
    else
        std::cout<<"Failed to create Descriptor Set ERROR code:- "<< res<<std::endl;
    
    //create a descriptor pool that will hold 10 uniform buffers
    std::vector<VkDescriptorPoolSize> sizes = {
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 10}
    };
    
    VkDescriptorPoolCreateInfo dpInfo{};
    dpInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    dpInfo.flags = 0;
    dpInfo.maxSets = 10;
    dpInfo.pPoolSizes = sizes.data();
    dpInfo.poolSizeCount = (uint32_t)sizes.size();
    
    vkCreateDescriptorPool(m_device, &dpInfo, nullptr, &m_descriptorPool);
    
    //create descriptor sets for each image in the swap chain.
    m_descriptorSets.resize(m_numImages);
    for (int i=0; i< m_descriptorSets.size(); i++){
        //create the camera buffers for every images
        m_cameraProjViewBuffer[i].CreateBuffer(m_device, vulkan_renderer->GetAllocator(), sizeof(glm::mat4x4), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
        
        VkDescriptorSetAllocateInfo dsAllocInfo{};
        dsAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        dsAllocInfo.pNext = nullptr;
        dsAllocInfo.descriptorPool = m_descriptorPool;
        dsAllocInfo.descriptorSetCount = 1;
        dsAllocInfo.pSetLayouts = &m_descSetLayout;
        
        vkAllocateDescriptorSets(m_device, &dsAllocInfo, &m_descriptorSets[i]);
        
        VkDescriptorBufferInfo descBufferInfo{};
        descBufferInfo.buffer =  *m_cameraProjViewBuffer[i].GetBuffer();
        descBufferInfo.offset = 0;
        descBufferInfo.range = sizeof(glm::mat4x4);
        
        VkWriteDescriptorSet writeSet{};
        writeSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeSet.pNext = nullptr;
        writeSet.descriptorCount  = 1; //here the descriptor count is the number of elemnts in "pBufferInfo" array.
        writeSet.dstSet = m_descriptorSets[i];
        writeSet.dstBinding = 0;
        writeSet.pBufferInfo = &descBufferInfo;
        writeSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        
        vkUpdateDescriptorSets(m_device, 1, &writeSet, 0, nullptr);
    }
}

void Application::OnEvent(Event& e)
{
    EventDispatcher dispatch(e);
    dispatch.Dispatch<WindowCloseEvent>([&](WindowCloseEvent close_event){
        m_running = false;
        return true;
    });
}

void Application::Run(){
    while(m_running)
    {
        m_window->update();
        m_camera->OnUpdate();
        uint32_t imgIndex = vulkan_renderer->AcquireNextImage();
        
        glm::mat4x4 projViewMat = m_camera->GetProjectionView(); //get projection view matrix
        m_cameraProjViewBuffer[imgIndex].CopyBuffer(vulkan_renderer->GetAllocator(), &projViewMat[0][0], sizeof(glm::mat4x4)); //update the vulkan buffer
        
        vulkan_renderer->SubmitAsync(m_cmdBuffers[imgIndex]);
        vulkan_renderer->Present(imgIndex);
        vkQueueWaitIdle(vulkan_renderer->GetQueue());

    }
    vkDeviceWaitIdle(vulkan_renderer->GetDevice());
    for (auto val: m_cameraProjViewBuffer)
    {
        val.DestroyBuffer(vulkan_renderer->GetAllocator());
    }
}
