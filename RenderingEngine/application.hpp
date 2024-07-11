#pragma once

#include "GLFW/glfw3.h"
#include <iostream>
#include <glm/vec4.hpp>
#include "VulkanAPI/vulkan_core.hpp"
#include <vector>
#include <memory>
#include <vulkan/vulkan.h>

class Application{
public:
    Application();
    ~Application();
    
    void Run();
    void RecordCommandBuffer();
private:
    std::shared_ptr<Engine::Vulkan> vulkan_renderer;
    uint32_t m_numImages = 0;
    std::vector<VkCommandBuffer> m_cmdBuffers;
};
