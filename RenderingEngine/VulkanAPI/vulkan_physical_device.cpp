#include "vulkan_physical_device.hpp"
//#include "utils.h"
#include <iostream>

namespace Engine
{
    VulkanPhysicalDevices::VulkanPhysicalDevices(){
        
    }

    VulkanPhysicalDevices::~VulkanPhysicalDevices(){
        
    }

    void VulkanPhysicalDevices::Init(const VkInstance& instance, const VkSurfaceKHR& surface){
        
        uint32_t numDevices = 0;
        vkEnumeratePhysicalDevices(instance, &numDevices, nullptr);
        
        std::cout<<"number of physical devices are = "<< numDevices<< std::endl;
        std::vector<VkPhysicalDevice> physicalDevices(numDevices);
        vkEnumeratePhysicalDevices(instance, &numDevices, physicalDevices.data());
        
        m_devices.resize(numDevices);
        
//        PhysicalDevice device;
        for (int i=0; i<numDevices; i++)
        {
            auto physicalDevice = physicalDevices[i];
            m_devices[i].m_physicalDevice = physicalDevice;
            
            VkPhysicalDeviceProperties deviceProperties;
            vkGetPhysicalDeviceProperties(m_devices[i].m_physicalDevice, &deviceProperties);
            m_devices[i].m_deviceProps = deviceProperties;
            
            vkGetPhysicalDeviceFeatures(m_devices[i].m_physicalDevice, &m_devices[i].m_deviceFeatures);
            
            uint32_t numQFamilies;
            vkGetPhysicalDeviceQueueFamilyProperties(m_devices[i].m_physicalDevice, &numQFamilies, nullptr);
            std::cout<<"number of queue families = "<< numQFamilies<< std::endl;
            
            m_devices[i].m_qFamilyProps.resize(numQFamilies);
            m_devices[i].m_qSupportPresent.resize(numQFamilies);
            
            vkGetPhysicalDeviceQueueFamilyProperties(m_devices[i].m_physicalDevice, &numQFamilies, m_devices[i].m_qFamilyProps.data());
            
            for (int q=0; q<numQFamilies;q++)
            {
                auto& qFamilyProp = m_devices[i].m_qFamilyProps[q];
                std::cout<<"family : "<<q<<" number of queues : "<<qFamilyProp.queueCount<<std::endl;
                
                auto flags = qFamilyProp.queueFlags;
                std::cout<<" Graphics : "<<((flags & VK_QUEUE_GRAPHICS_BIT)? "yes" : "no");
                std::cout<<" Compute : "<<((flags & VK_QUEUE_COMPUTE_BIT)? "yes" : "no");
                std::cout<<" Transfer : "<<((flags & VK_QUEUE_TRANSFER_BIT)? "yes" : "no");
                std::cout<<" Sparse Binding : "<<((flags & VK_QUEUE_SPARSE_BINDING_BIT)? "yes" : "no");
                std::cout<<std::endl;
                
                auto res = vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, q, surface, &(m_devices[i].m_qSupportPresent[q]));
                    std::cout<<"Queue family : "<< q<< " Supports Surface :"<< ((res == VK_SUCCESS)? "yes":"no")<<std::endl;
            }
            
            uint32_t numSurfaceFmt;
            auto res = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &numSurfaceFmt, nullptr);
            if(res!= VK_SUCCESS)
                std::cout<<"error in retriving device surface formats ERROR : "<< res<<std::endl;
            
            m_devices[i].m_surfaceFormats.resize(numSurfaceFmt);
            
            vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &numSurfaceFmt, m_devices[i].m_surfaceFormats.data());
            
            for (int j=0 ;j<numSurfaceFmt; j++)
            {
                auto fmt = m_devices[i].m_surfaceFormats[j];
                std::cout<<" Format : "<< fmt.format<< " Color space : "<< fmt.colorSpace<< std::endl;
            }
            std::cout<< std::endl;
            
            vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &m_devices[i].m_surfaceCaps);

            uint32_t numPresentModes;
            vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &numPresentModes, nullptr);
            
            m_devices[i].m_presentModes.resize(numPresentModes);
            
            vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &numPresentModes, m_devices[i].m_presentModes.data());
            
            vkGetPhysicalDeviceMemoryProperties(physicalDevice, &m_devices[i].m_memProps);
        }
    }

    uint32_t VulkanPhysicalDevices::SelectDevice(VkQueueFlags requiredQueueType, bool supportsPresent){
        int index = 0;
        for (auto device : m_devices)
        {
            int k=0;
            for(auto qFamilyProps: device.m_qFamilyProps)
            {
                if((qFamilyProps.queueFlags & requiredQueueType) && (device.m_qSupportPresent[k] == supportsPresent))
                {
                    m_deviceIdx = index;
                    return k;
                }
                k++;
            }
            index++;
        }
        
        std::cout<<"no device with supported queue type"<<std::endl;
        return 0;
    }

    const PhysicalDevice& VulkanPhysicalDevices::Selected(){
        if(m_deviceIdx == -1 || m_deviceIdx > m_devices.size())
        {
            std::cout<<"no valid devices found\n";
            exit(1);
        }
        return m_devices[m_deviceIdx];
    }

}
