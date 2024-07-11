//#pragma once
#include <vulkan/vulkan.h>
#include <vector>

namespace Engine {

struct PhysicalDevice{
    VkPhysicalDevice m_physicalDevice;
    VkPhysicalDeviceProperties m_deviceProps;
    VkPhysicalDeviceFeatures m_deviceFeatures;
    std::vector<VkQueueFamilyProperties> m_qFamilyProps;
    std::vector<VkBool32> m_qSupportPresent;
    std::vector<VkSurfaceFormatKHR> m_surfaceFormats;
    VkSurfaceCapabilitiesKHR m_surfaceCaps;
    VkPhysicalDeviceMemoryProperties m_memProps;
    std::vector<VkPresentModeKHR> m_presentModes;
};

class VulkanPhysicalDevices{
public:
    VulkanPhysicalDevices();
    ~VulkanPhysicalDevices();
    
    void Init(const VkInstance& instance, const VkSurfaceKHR& surface);
    uint32_t SelectDevice(VkQueueFlags requiredQueueType, bool supportsPresent);
    const PhysicalDevice& Selected();
private:
    std::vector<PhysicalDevice> m_devices;
    int m_deviceIdx = -1;
};
}
