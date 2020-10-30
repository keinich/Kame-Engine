#pragma once

#include <unordered_map>

#include "VulkanCommon.h"
#include "VulkanPhyiscalDevice.h"

namespace Kame {

  class VulkanDevice {
  public:
    VulkanDevice(VulkanPhysicalDevice& gpu, VkSurfaceKHR surface, std::unordered_map<const char*, bool> reqeuestedExtensions = {});
    VulkanDevice(const VulkanDevice&) = delete;
    VulkanDevice(VulkanDevice&&) = delete;
    ~VulkanDevice();
    VulkanDevice &operator=(const VulkanDevice&) = delete;
    VulkanDevice &operator=(VulkanDevice&&) = delete;

  private: // Fields
    VkDevice _Handle{ VK_NULL_HANDLE };
    std::vector<VkExtensionProperties> _DeviceExtensions;

  };

}
