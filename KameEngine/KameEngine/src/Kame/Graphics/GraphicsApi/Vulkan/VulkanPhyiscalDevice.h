#pragma once

#include "VulkanCommon.h"

namespace Kame {

  class VulkanInstance;

  class VulkanPhysicalDevice {
  public:
    VulkanPhysicalDevice(VulkanInstance& instance, VkPhysicalDevice handle);
    ~VulkanPhysicalDevice();

    inline VkPhysicalDevice GetHandle() { return _Handle; }

    const VkPhysicalDeviceFeatures &GetFeatures() const;
    const VkPhysicalDeviceProperties GetProperties() const;
    const VkPhysicalDeviceMemoryProperties GetMemoryProperties() const;
    const std::vector<VkQueueFamilyProperties>& GetQueueFamilyProperties() const;

    void* GetExtensionFeatureChain() const;

    template <typename T>
    T& RequestExtensionFeatures(VkStructureType type) {
      if (!_Instance->is_)
    }

  private: // Methods
    void Initialize();

  private: // Fields
    VkPhysicalDevice _Handle;
    VulkanInstance& _Instance;

    VkPhysicalDeviceFeatures _Features;
    VkPhysicalDeviceProperties _Properties;
    VkPhysicalDeviceMemoryProperties _MemoryProperties;
    std::vector<VkQueueFamilyProperties> _QueueFamilyProperties;

    void* _LastRequestedExtensionFeature{ nullptr };
  
  };

}
