#define VMA_IMPLEMENTATION
#include "vulkan_core.hpp"
#include "utils.h"
#include <iostream>

namespace Engine 
{
    std::vector<VkDescriptorSetLayout> Vulkan::m_descriptorSetLayouts;
    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT Severity,
        VkDebugUtilsMessageTypeFlagsEXT Type,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData)
    {
       std::cout<<"Debug callback: "<< pCallbackData->pMessage<< std::endl;
       std::cout<<"  Severity "<< GetDebugSeverityStr(Severity)<< std::endl;
       std::cout<<"  Type "<< GetDebugType(Type)<< std::endl;
       std::cout<<"  Objects "<< std::endl;

        for (uint32_t i = 0; i < pCallbackData->objectCount; i++) {
            std::cout<<pCallbackData->pObjects[i].objectHandle<<std::endl;
        }

        std::cout<<("\n");

        return VK_FALSE;  // The calling function should not be aborted
    }
    Vulkan::Vulkan(){
        
    }
    Vulkan::~Vulkan(){
        cleanup();
    }
    void Vulkan::Init(){
        window = std::make_shared<Engine::Window>();

        uint32_t extCount;
        std::vector<const char*> requiredExtensions;
        auto extensions = glfwGetRequiredInstanceExtensions(&extCount);
        
        std::cout<<"extension names\n";
        for (int i=0; i< extCount; i++){
            std::cout<< extensions[i]<<std::endl;
            requiredExtensions.emplace_back(extensions[i]);
        }
        //create the vertex buffer (triangle position and color val)
        vertices = {
            {{2.0,-0.5,0.0,10.0}, {1.0,0.0,0.0,1.0}},
            {{0.5,0.0,0.0,1.0}, {0.0,1.0,0.0,1.0}},
            {{-0.5,0.0,0.0,-10.0}, {0.0,0.0,1.0,1.0}},
        };
        m_vertex_buffer = std::make_shared<VulkanBuffer>();
        
        CreateInstance(requiredExtensions);
//        CreateDebugCallback();
        CreateSurface();
        CreatePhysicalDevice();
        CreateLogicalDevice();
        CreateMemoryAllocator();
        CreateVertexBuffer();
        CreateSwapChain();
        CreateCommandBufferPool();
        CreateRenderPass();
        CreateFrameBuffers();
//        CreateGraphicsPipeline();
        CreateQueue();
        CreateSemaphores(&m_presentCompleateSemaphore);
        CreateSemaphores(&m_renderCompleateSemaphore);
        CreateFences(&m_inFlightFence);
    }

    void Vulkan::CreateMemoryAllocator(){
        
        VmaVulkanFunctions vulkanFunctions = {};
        vulkanFunctions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
        vulkanFunctions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;
        
        VmaAllocatorCreateInfo allocatorInfo{};
        allocatorInfo.device = m_device;
        allocatorInfo.instance = VulkanInstance;
        allocatorInfo.physicalDevice = m_physicalDevices.Selected().m_physicalDevice;
        allocatorInfo.pVulkanFunctions = &vulkanFunctions;
        allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_2;
        allocatorInfo.flags = VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;
        
        vmaCreateAllocator(&allocatorInfo, &m_vmaAllocator);
    }

    void Vulkan::CreateInstance(std::vector<const char*>& extensions){
        
        //create validation layers
        std::vector<const char*> layers = {"VK_LAYER_KHRONOS_validation"};
        
        //add aditional extensions here
        #ifdef __APPLE__
            extensions.emplace_back("VK_MVK_macos_surface");
        #endif
        #ifdef _WIN32
            extensions.emplace_back("VK_MVK_win32_surface");
        #endif
        extensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
        extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME); //for debuging
        
        VkDebugUtilsMessengerCreateInfoEXT MessengerCreateInfo = {
                .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
                .pNext = NULL,
                .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
                .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
                .pfnUserCallback = &DebugCallback,
                .pUserData = NULL
            };
        
        VkApplicationInfo appInfo {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.apiVersion = VK_API_VERSION_1_3;
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pApplicationName = "Vulkan Renderer";
        appInfo.pEngineName = "vulkan Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
//        appInfo.pNext = &MessengerCreateInfo;
        
        VkInstanceCreateInfo pCreateInfo{};
        pCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        pCreateInfo.pApplicationInfo = &appInfo;
        pCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        pCreateInfo.ppEnabledExtensionNames = extensions.data();
        pCreateInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
        pCreateInfo.ppEnabledLayerNames = layers.data();
        pCreateInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

        VkResult result = vkCreateInstance(&pCreateInfo, nullptr, &VulkanInstance);
        if(result != VK_SUCCESS)
        {
            std::cout<<"Vulkan initiliazion failed with Err Code: "<< result <<std::endl;
        }
        else
            std::cout<<"instance created\n";
        
        m_deleationStack.push([&](){vkDestroyInstance(VulkanInstance, nullptr);});
    }

    void Vulkan::CreateDebugCallback(){
        
        VkDebugUtilsMessengerCreateInfoEXT MessengerCreateInfo = {
                .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
                .pNext = NULL,
                .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
                .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
                .pfnUserCallback = &DebugCallback,
                .pUserData = NULL
            };

            PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessenger = VK_NULL_HANDLE;
            vkCreateDebugUtilsMessenger = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(VulkanInstance, "vkCreateDebugUtilsMessengerEXT");
            if (!vkCreateDebugUtilsMessenger) {
                std::cerr<<("Cannot find address of vkCreateDebugUtilsMessenger\n");
                exit(1);
            }

            VkResult res = vkCreateDebugUtilsMessenger(VulkanInstance, &MessengerCreateInfo, NULL, &m_debugMessanger);
            if (res != VK_SUCCESS)
            {
                std::cerr<<"failed to create debug utils messanger";
                exit(1);
            }
            printf("Debug utils messenger created\n");
    }

    void Vulkan::CreateSurface()
    {
        auto result = glfwCreateWindowSurface(VulkanInstance, window->getWindow(), nullptr, &m_surface);
        if(result != VK_SUCCESS)
        {
            std::cerr<<"failed to create window surface Error code : "<< result<<std::endl;
            exit(1);
        }
        std::cout<<"window surface created successfully\n";
        
        m_deleationStack.push([&](){vkDestroySurfaceKHR(VulkanInstance, m_surface, nullptr);});
    }

    void Vulkan::CreatePhysicalDevice(){
        m_physicalDevices.Init(VulkanInstance, m_surface);
        m_queueFamilyIndex = m_physicalDevices.SelectDevice(VK_QUEUE_GRAPHICS_BIT, true);
    }

    bool Vulkan::isDeviceSutable(VkPhysicalDevice& device){
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
        
        return deviceProperties.deviceType == VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && deviceFeatures.geometryShader == VK_TRUE;
    }


    void Vulkan::CreateLogicalDevice(){
        float qPriorities[1] = {1.0f};
        
        VkDeviceQueueCreateInfo qInfo ={};
        qInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        qInfo.flags = 0;
        qInfo.pNext = nullptr;
        qInfo.pQueuePriorities = &qPriorities[0];
        qInfo.queueFamilyIndex = m_queueFamilyIndex;
        qInfo.queueCount = 1;
        
        std::vector<const char*> DevExts = {
                VK_KHR_SWAPCHAIN_EXTENSION_NAME,
                VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME,
                "VK_KHR_portability_subset"
            };
        
       
        VkPhysicalDeviceFeatures DeviceFeatures = { 0 };
            DeviceFeatures.geometryShader = VK_TRUE;
            DeviceFeatures.tessellationShader = VK_TRUE;
        
        VkDeviceCreateInfo dCreateInfo = {};
        dCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        dCreateInfo.flags = 0;
        dCreateInfo.pNext = nullptr;
        dCreateInfo.queueCreateInfoCount = 1;
        dCreateInfo.pQueueCreateInfos = &qInfo;
        dCreateInfo.enabledLayerCount = 0; // DEPRECATED
        dCreateInfo.ppEnabledLayerNames = NULL; // DEPRECATED
        dCreateInfo.enabledExtensionCount = (uint32_t)DevExts.size();
        dCreateInfo.ppEnabledExtensionNames = DevExts.data();
        dCreateInfo.pEnabledFeatures = &m_physicalDevices.Selected().m_deviceFeatures;
        
        auto res = vkCreateDevice(m_physicalDevices.Selected().m_physicalDevice, &dCreateInfo, nullptr, &m_device);
        if(res == VK_SUCCESS)
        {
            std::cout<<"logical device created"<<std::endl;
        }
        else{
            std::cout<<"failed to create a logical device ERROR code : "<<res<<std::endl;
        }
        
        m_deleationStack.push([&](){vkDestroyDevice(m_device, nullptr);});
    }

    void Vulkan::CreateSwapChain(){
        
        const VkSurfaceCapabilitiesKHR& surfaceCaps = m_physicalDevices.Selected().m_surfaceCaps;
        
        //get number of images that will be there in the swap chain
        auto requestedNumImages = surfaceCaps.minImageCount + 1; //for double buffering
        uint32_t finalImnageCnt;
        if(surfaceCaps.maxImageCount > 0 && (requestedNumImages > surfaceCaps.maxImageCount))
            finalImnageCnt = surfaceCaps.maxImageCount;
        else
            finalImnageCnt = requestedNumImages;
        
        //get suitable Image format for swap chain
        VkSurfaceFormatKHR surfaceFormat = m_physicalDevices.Selected().m_surfaceFormats[0];
        for (auto surfaceFmt : m_physicalDevices.Selected().m_surfaceFormats)
        {
            if(surfaceFmt.format == VkFormat::VK_FORMAT_R8G8B8A8_SRGB && 
               surfaceFmt.colorSpace == VkColorSpaceKHR::VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                surfaceFormat = surfaceFmt;
        }
        
        m_surfaceImageFormat = surfaceFormat.format;
        
        //define a presentation mode
        VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
        for (auto present_mode : m_physicalDevices.Selected().m_presentModes)
        {
            if (presentMode == VkPresentModeKHR::VK_PRESENT_MODE_MAILBOX_KHR)
                presentMode = present_mode;
        }
        
        m_swapChainImageExtent = surfaceCaps.currentExtent;
        
        VkSwapchainCreateInfoKHR createSCInfo={};
        createSCInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createSCInfo.pNext = nullptr;
        createSCInfo.flags = 0;
        createSCInfo.surface = m_surface;
        createSCInfo.minImageCount = finalImnageCnt;
        createSCInfo.imageFormat = surfaceFormat.format;
        createSCInfo.imageColorSpace = surfaceFormat.colorSpace;
        createSCInfo.imageArrayLayers = 1;
        createSCInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        createSCInfo.imageExtent = surfaceCaps.currentExtent;
        createSCInfo.preTransform = surfaceCaps.currentTransform;
        createSCInfo.presentMode = presentMode;
        createSCInfo.queueFamilyIndexCount = 1;
        createSCInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createSCInfo.pQueueFamilyIndices = &m_queueFamilyIndex;
        createSCInfo.compositeAlpha = VkCompositeAlphaFlagBitsKHR::VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createSCInfo.clipped = VK_TRUE;

        auto res = vkCreateSwapchainKHR(m_device, &createSCInfo, nullptr, &m_swapChain);
        if (res == VK_SUCCESS)
        {
            std::cout<<"Swapchain created Successfully\n";
        }
        else
            std::cout<<"failed to create SwapChain ERROR code:- "<< res<<std::endl;
        
        //create Image Views and Get SwapChain Images
        uint32_t numImages;
        vkGetSwapchainImagesKHR(m_device, m_swapChain, &numImages, nullptr);
        m_images.resize(numImages);
        m_imageViews.resize(numImages);
        vkGetSwapchainImagesKHR(m_device, m_swapChain, &numImages, m_images.data());
        
        for (int i=0 ; i<numImages  ; i++)
        {
            CreateImageView(m_device, m_images[i], m_imageViews[i], surfaceFormat.format, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT, 1, 1);
        }
        
        m_deleationStack.push([&](){vkDestroySwapchainKHR(m_device, m_swapChain, nullptr);});
    }

    void Vulkan::CreateImageView(VkDevice& device, VkImage& image, VkImageView& imageView, VkFormat format, VkImageViewType viewType, VkImageAspectFlags aspectFlags, uint32_t layerCnt, uint32_t mipLevel){
        
        VkImageViewCreateInfo imageViewInfo;
        imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewInfo.pNext = nullptr;
        imageViewInfo.image = image;
        imageViewInfo.components = {VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY,VK_COMPONENT_SWIZZLE_IDENTITY};
        imageViewInfo.flags = 0;
        imageViewInfo.viewType = viewType;
        imageViewInfo.format = format;
        VkImageSubresourceRange subResRange = {};
        subResRange.aspectMask = aspectFlags;
        subResRange.baseArrayLayer = 0;
        subResRange.baseMipLevel = 0;
        subResRange.layerCount = layerCnt;
        subResRange.levelCount = mipLevel;
        imageViewInfo.subresourceRange = subResRange;
        
        auto res = vkCreateImageView(m_device, &imageViewInfo, nullptr, &imageView);
        if (res != VK_SUCCESS)
            std::cout<< "failed to craete Image View ERROR code :- "<< res<< std::endl;
        else
            std::cout<< "Image View Created successfully\n";
        
        m_deleationStack.push([&](){for (int i=0; i<m_images.size(); i++)
        {
            vkDestroyImageView(m_device, m_imageViews[i], nullptr);
        }});
    }

    void Vulkan::CreateCommandBufferPool()
    {
        VkCommandPoolCreateInfo cmdPoolInfo;
        cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        cmdPoolInfo.flags = 0;
        cmdPoolInfo.pNext = nullptr;
        cmdPoolInfo.queueFamilyIndex = m_queueFamilyIndex;
       
        auto res = vkCreateCommandPool(m_device, &cmdPoolInfo, nullptr, &m_cmdPool);
        if(res != VK_SUCCESS)
            std::cout<<"failed to create a command buffer pool ERROR code :- "<<res<<std::endl;
        else
            std::cout<<"command pool buffer successfully created\n";
        
        m_deleationStack.push([&](){vkDestroyCommandPool(m_device, m_cmdPool, nullptr);});
    }

    void Vulkan::CreateCommandBuffer(uint32_t count, VkCommandBuffer *cmdBuffer)
    {
        VkCommandBufferAllocateInfo allocInfo;
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.pNext = nullptr;
        allocInfo.level = VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = count;
        allocInfo.commandPool = m_cmdPool;
        
        auto res = vkAllocateCommandBuffers(m_device, &allocInfo, cmdBuffer);
        
        if(res != VK_SUCCESS)
            std::cout<<"failed to create a command buffer ERROR code :- "<<res<<std::endl;
        else
            std::cout<<"command buffer successfully created\n";
    }

    void Vulkan::UpdateWindow(){
       
            window->update();
        
    }

    void Vulkan::BeginCommandBuffer(VkCommandBuffer buffer, VkCommandBufferUsageFlags usageFlags)
    {
        VkCommandBufferBeginInfo bufBeginInfo = {};
        bufBeginInfo.flags = usageFlags;
        bufBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        bufBeginInfo.pInheritanceInfo = nullptr;
        
        auto res = vkBeginCommandBuffer(buffer, &bufBeginInfo);
        if(res != VK_SUCCESS)
            std::cout<<"failed to create begin command buffer ERROR code"<<res<<std::endl;
        
    }

    void Vulkan::EndCommandBuffer(VkCommandBuffer buffer)
    {
        auto res = vkEndCommandBuffer(buffer);
        if(res != VK_SUCCESS)
            std::cout<<"failed to create end command buffer ERROR code"<<res<<std::endl;
    }

    void Vulkan::BeginRenderPass(VkCommandBuffer& buffer, int imageIndex, VkClearValue& clearColor){
        
        VkRenderPassBeginInfo rpInfo{};
        rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        rpInfo.clearValueCount = 1;
        rpInfo.pClearValues = &clearColor;
        rpInfo.framebuffer = m_swapChainFramebuffers[imageIndex];
        rpInfo.renderPass = m_renderPass;
        VkRect2D renderArea{};
        renderArea.offset = {0,0};
        renderArea.extent = m_swapChainImageExtent;
        rpInfo.renderArea = renderArea;
       
        vkCmdBeginRenderPass(buffer, &rpInfo, VK_SUBPASS_CONTENTS_INLINE);
    }

    void Vulkan::EndRenderPass(VkCommandBuffer& buffer){
        vkCmdEndRenderPass(buffer);
    }

    void Vulkan::CreateQueue()
    {
        vkGetDeviceQueue(m_device, m_queueFamilyIndex, 0, &m_queue);
    }

    void Vulkan::CreateSemaphores(VkSemaphore* semaphore)
    {
        VkSemaphoreCreateInfo semaphoreInfo = {};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        
        auto res = vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, semaphore);
        if(res == VK_SUCCESS)
        {
            std::cout<< "semaphore successfully created"<<std::endl;
        }
        else{
            std::cout<<"could not create a semaphore ERROR code:- "<< res<<std::endl;
        }
    }

    void Vulkan::CreateFences(VkFence* fence){
        
        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.pNext = nullptr;
        
        auto res = vkCreateFence(m_device, &fenceInfo, nullptr, fence);
        if(res == VK_SUCCESS)
            std::cout<<"fence created successfully\n";
        else
            std::cout<<"error in creating fence ERROR code:- "<<res<<std::endl;
    }

    void Vulkan::CreateFrameBuffers(){
        m_swapChainFramebuffers.resize(m_imageViews.size());
        auto imageExtent = m_physicalDevices.Selected().m_surfaceCaps.currentExtent;
        for (int i=0;i<m_swapChainFramebuffers.size();i++)
        {
            VkImageView attachment = m_imageViews[i];
            
            VkFramebufferCreateInfo fbInfo{};
            fbInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            fbInfo.renderPass = m_renderPass;
            fbInfo.width = imageExtent.width;
            fbInfo.height = imageExtent.height;
            fbInfo.attachmentCount = 1;
            fbInfo.pAttachments = &attachment;
            fbInfo.layers = 1;
            
            if(vkCreateFramebuffer(m_device, &fbInfo, nullptr, &m_swapChainFramebuffers[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create framebuffer!");
            }
        }
    }
    void Vulkan::CreateRenderPass(){
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = m_surfaceImageFormat;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;

        VkAttachmentReference colorAttachmentref{};
        colorAttachmentref.attachment = 0; //index of the VkAttachmentDescription array (as we use 1 value so the attachment index is 0)
        colorAttachmentref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        
        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount =1;
        subpass.pColorAttachments = &colorAttachmentref;
        
        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        
        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &colorAttachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;
        
        if (vkCreateRenderPass(m_device, &renderPassInfo, nullptr, &m_renderPass) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create render pass!");
        }
    }

    void Vulkan::CreateGraphicsPipeline(){
    }
    void Vulkan::CreateVertexBuffer(){
        m_vertex_buffer->CreateBuffer(m_device, m_vmaAllocator, sizeof(Vertex) * vertices.size(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
        m_vertex_buffer->CopyBuffer(m_vmaAllocator, vertices.data(), sizeof(Vertex) * vertices.size());
    }

    uint32_t Vulkan::AcquireNextImage(){
//        vkWaitForFences(m_device, 1, &m_inFlightFence, VK_TRUE, UINT64_MAX);
//        vkResetFences(m_device, 1, &m_inFlightFence);
        /*
         "vkAcquireNextImageKHR" signals the "m_presentCompleateSemaphore" semaphore
         once the acquiring of image is compleate.
         */
        uint32_t imageIndex = 0;
        auto res = vkAcquireNextImageKHR(m_device, m_swapChain, UINT64_MAX, m_presentCompleateSemaphore, nullptr, &imageIndex);
        if (res != VK_SUCCESS)
            std::cout<<"failed to acquire next Image ERROR code:- "<<res<<::std::endl;
        return imageIndex;
    }

    void Vulkan::SubmitAsync(VkCommandBuffer cmdBuffer)
    {
        /*
         "vkQueueSubmit" signals the "m_renderCompleateSemaphore" semaphore once
         all te instruction in the queue is compleated. "vkQueueSubmit" will not
         start executing unless the "m_presentCompleateSemaphore" is signaled (or until acquiring of Image is compleate)
         */
        VkPipelineStageFlags flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.pNext = nullptr;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &m_presentCompleateSemaphore;
        submitInfo.pSignalSemaphores = &m_renderCompleateSemaphore;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.commandBufferCount =1;
        submitInfo.pCommandBuffers = &cmdBuffer;
        submitInfo.pWaitDstStageMask = &flags;
        vkQueueSubmit(m_queue, 1, &submitInfo, VK_NULL_HANDLE);
    }

    void Vulkan::Present(uint32_t imageIndex)
    {
        /*
         "vkQueuePresentKHR" waits on "m_renderCompleateSemaphore" semaphore to be signalled befor executing or it waits for all the instruction in a queue to compleate execution.
         */
        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.pNext = nullptr;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &m_renderCompleateSemaphore;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &m_swapChain;
        presentInfo.pImageIndices = &imageIndex;
        
        auto res = vkQueuePresentKHR(m_queue, &presentInfo);
        if(res != VK_SUCCESS)
        {
            std::cout<<"error in presenting the queue ERROR code:- "<<res<<std::endl;
        }
    }

    void Vulkan::cleanup()
    {
//        PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessenger = VK_NULL_HANDLE;
//            vkDestroyDebugUtilsMessenger = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(VulkanInstance, "vkDestroyDebugUtilsMessengerEXT");
//            if (!vkDestroyDebugUtilsMessenger) {
//                std::cerr<<("Cannot find address of vkDestroyDebugUtilsMessengerEXT\n");
//                exit(1);
//            }
//        vkDestroyDebugUtilsMessenger(VulkanInstance, m_debugMessanger, NULL);
        m_vertex_buffer->DestroyBuffer(m_vmaAllocator);
        vkDestroySemaphore(m_device, m_presentCompleateSemaphore, nullptr);
        vkDestroySemaphore(m_device, m_renderCompleateSemaphore, nullptr);
        vkDestroyFence(m_device, m_inFlightFence, nullptr);
        vkDestroyCommandPool(m_device, m_cmdPool, nullptr);
        for (int i=0;i<m_swapChainFramebuffers.size();i++)
        {
            vkDestroyFramebuffer(m_device, m_swapChainFramebuffers[i], nullptr);
        }
        vkDestroyRenderPass(m_device, m_renderPass, nullptr);
        for (int i=0; i<m_imageViews.size(); i++)
        {
            vkDestroyImageView(m_device, m_imageViews[i], nullptr);
        }
        vkDestroySwapchainKHR(m_device, m_swapChain, nullptr);
        vmaDestroyAllocator(m_vmaAllocator);
        vkDestroyDevice(m_device, nullptr);
        vkDestroySurfaceKHR(VulkanInstance, m_surface, nullptr);
        vkDestroyInstance(VulkanInstance, nullptr);
//        while(!m_deleationStack.empty())
//        {
//            auto fn = m_deleationStack.top();
//            fn();
//            m_deleationStack.pop();
//        }
    }

std::vector<char> Vulkan::ReadFile(const std::string& filename) {
       std::ifstream file(filename, std::ios::ate | std::ios::binary);

       if (!file.is_open()) {
           throw std::runtime_error("failed to open file!");
       }

       size_t fileSize = (size_t) file.tellg();
       std::vector<char> buffer(fileSize);

       file.seekg(0);
       file.read(buffer.data(), fileSize);

       file.close();

       return buffer;
   }
}
