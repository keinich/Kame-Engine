#pragma once

#include "VulkanCommon.h"

namespace Kame {

  class VulkanPhysicalDevice {
  public:
    VulkanPhysicalDevice(VkPhysicalDevice handle);
    ~VulkanPhysicalDevice();

    inline VkPhysicalDevice GetHandle() { return _Handle; }

    VkPhysicalDeviceFeatures GetFeatures();
    VkPhysicalDeviceProperties GetProperties();
    VkPhysicalDeviceMemoryProperties GetMemoryProperties();

  private: // Methods
    void Initialize();

  private: // Fields
    VkPhysicalDevice _Handle;

    VkPhysicalDeviceFeatures _Features;
    VkPhysicalDeviceProperties _Properties;
    VkPhysicalDeviceMemoryProperties _MemoryProperties;
  };

}
