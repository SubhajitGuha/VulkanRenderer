#pragma once

#include "GLFW/glfw3.h"
#include <iostream>
#include <glm/vec4.hpp>
#include "VulkanAPI/vulkan_core.hpp"
#include <vector>
#include <memory>
#include <vulkan/vulkan.h>
#include "VulkanAPI/vulkan_buffer.hpp"
#include "VulkanAPI/vulkan_pipeline.hpp"
#include "Camera/Camera.hpp"
#include "Events/ApplicationEvent.h"
#include "Window/window.hpp"
#include "Events/ApplicationEvent.h"

class Application{
public:
    Application();
    ~Application();
    
    void Run();
    void OnEvent(Engine::Event& e);
    void RecordCommandBuffer();
    std::shared_ptr<Engine::Window> GetWindow(){return m_window;}
    static Application* Get(){return getApplication;}
private:
    void CreateDescriptorSets(VkDevice& m_device);
private:
    std::shared_ptr<Engine::Vulkan> vulkan_renderer;
    std::shared_ptr<Engine::VulkanPipeline>  m_pipeline;
    uint32_t m_numImages = 0;
    //create buffers for every image in the swap chin
    std::vector<Engine::VulkanBuffer> m_cameraProjViewBuffer; //a vulkan buffer that holdes the camera projection view matrix

    VkDescriptorSetLayout m_descSetLayout;
    VkDescriptorPool m_descriptorPool = VK_NULL_HANDLE;
    std::vector<VkCommandBuffer> m_cmdBuffers;
    std::vector<VkDescriptorSet> m_descriptorSets;
    std::shared_ptr<Engine::Camera> m_camera;
    std::shared_ptr<Engine::Window> m_window;
    static Application* getApplication;
    bool m_running = true;
};
