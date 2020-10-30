#pragma once

#include "VulkanCommon.h"

namespace Kame {

  class VulkanPhysicalDevice {
  public:
    VulkanPhysicalDevice(VkPhysicalDevice handle);
    ~VulkanPhysicalDevice();

    inline VkPhysicalDevice GetHandle() { return _Handle; }

    const VkPhysicalDeviceFeatures &GetFeatures() const;
    const VkPhysicalDeviceProperties GetProperties() const;
    const VkPhysicalDeviceMemoryProperties GetMemoryProperties() const;
    const std::vector<VkQueueFamilyProperties>& GetQueueFamilyProperties() const;

  private: // Methods
    void Initialize();

  private: // Fields
    VkPhysicalDevice _Handle;

    VkPhysicalDeviceFeatures _Features;
    VkPhysicalDeviceProperties _Properties;
    VkPhysicalDeviceMemoryProperties _MemoryProperties;
    std::vector<VkQueueFamilyProperties> _QueueFamilyProperties;
  };

}
