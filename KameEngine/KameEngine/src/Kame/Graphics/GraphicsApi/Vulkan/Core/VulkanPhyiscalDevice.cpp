#include "kmpch.h"
#include "VulkanPhyiscalDevice.h"

#include "VulkanInstance.h"

namespace Kame {

  VulkanPhysicalDevice::VulkanPhysicalDevice(VulkanInstance& instance, VkPhysicalDevice handle) :
    _Instance(instance) {
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

  const VkPhysicalDeviceFeatures& VulkanPhysicalDevice::GetFeatures() const {
    return _Features;
  }

  const VkPhysicalDeviceProperties VulkanPhysicalDevice::GetProperties() const {
    return _Properties;
  }

  const VkPhysicalDeviceMemoryProperties VulkanPhysicalDevice::GetMemoryProperties() const {
    return _MemoryProperties;
  }

  const std::vector<VkQueueFamilyProperties>& VulkanPhysicalDevice::GetQueueFamilyProperties() const {
    return _QueueFamilyProperties;
  }

  VkBool32 VulkanPhysicalDevice::IsPresentSupported(VkSurfaceKHR surface, int queueFamilyIndex) {
    VkBool32 result{ VK_FALSE };

    if (surface == VK_NULL_HANDLE) return false;

    VkResult r = vkGetPhysicalDeviceSurfaceSupportKHR(_Handle, queueFamilyIndex, surface, &result);
    ASSERT_VULKAN(r);

    return result;
  }

  void* VulkanPhysicalDevice::GetExtensionFeatureChain() const {
    return _LastRequestedExtensionFeature;
  }

  const VkPhysicalDeviceFeatures VulkanPhysicalDevice::GetRequestedFeatures() const {
    return _RequestedFeatures;
  }

  void VulkanPhysicalDevice::Initialize() {

  }

}