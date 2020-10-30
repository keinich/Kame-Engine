#include "kmpch.h"
#include "VulkanPhyiscalDevice.h"

namespace Kame {

  VulkanPhysicalDevice::VulkanPhysicalDevice(VkPhysicalDevice handle) {
    _Handle = handle;

    vkGetPhysicalDeviceFeatures(_Handle, &_Features);
    vkGetPhysicalDeviceProperties(_Handle, &_Properties);
    vkGetPhysicalDeviceMemoryProperties(_Handle, &_MemoryProperties);

    uint32_t numberOfQueueFamilies = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(handle, &numberOfQueueFamilies, nullptr);
    _QueueFamilyProperties = std::vector< VkQueueFamilyProperties>(numberOfQueueFamilies);
    vkGetPhysicalDeviceQueueFamilyProperties(handle, &numberOfQueueFamilies, _QueueFamilyProperties.data());

  }

  VulkanPhysicalDevice::~VulkanPhysicalDevice() {}

  const VkPhysicalDeviceFeatures &VulkanPhysicalDevice::GetFeatures() const {
    return _Features;
  }

  const VkPhysicalDeviceProperties VulkanPhysicalDevice::GetProperties() const {
    return _Properties;
  }

  const VkPhysicalDeviceMemoryProperties VulkanPhysicalDevice::GetMemoryProperties() const{
    return _MemoryProperties;
  }

  const std::vector<VkQueueFamilyProperties>& VulkanPhysicalDevice::GetQueueFamilyProperties() const {
    return _QueueFamilyProperties;
  }

  void VulkanPhysicalDevice::Initialize() {

  }

}