#include "kmpch.h"
#include "VulkanPhyiscalDevice.h"

namespace Kame {

  VulkanPhysicalDevice::VulkanPhysicalDevice(VkPhysicalDevice handle) {
    _Handle = handle;

    vkGetPhysicalDeviceFeatures(_Handle, &_Features);
    vkGetPhysicalDeviceProperties(_Handle, &_Properties);
    vkGetPhysicalDeviceMemoryProperties(_Handle, &_MemoryProperties);
  }

  VulkanPhysicalDevice::~VulkanPhysicalDevice() {}

  VkPhysicalDeviceFeatures VulkanPhysicalDevice::GetFeatures() {
    return _Features;
  }

  VkPhysicalDeviceProperties VulkanPhysicalDevice::GetProperties() {
    return _Properties;
  }

  VkPhysicalDeviceMemoryProperties VulkanPhysicalDevice::GetMemoryProperties() {
    return _MemoryProperties;
  }

  void VulkanPhysicalDevice::Initialize() {

  }

}