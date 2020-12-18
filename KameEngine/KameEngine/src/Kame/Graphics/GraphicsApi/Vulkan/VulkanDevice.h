#pragma once

#include <unordered_map>

#include "VulkanCommon.h"
#include "VulkanPhyiscalDevice.h"

namespace Kame {

  class VulkanDevice {
  public:
    VulkanDevice();
    VulkanDevice(const VulkanDevice&) = delete;
    VulkanDevice(VulkanDevice&&) = delete;
    ~VulkanDevice();
    VulkanDevice &operator=(const VulkanDevice&) = delete;
    VulkanDevice &operator=(VulkanDevice&&) = delete;

    void Initialize(VulkanPhysicalDevice& gpu, std::unordered_map<const char*, bool> reqeuestedExtensions = {});
    void Shutdown();

    inline VkDevice GetHandle() { return _Handle; }

  private: // Fields
    VkDevice _Handle{ VK_NULL_HANDLE };
    std::vector<VkExtensionProperties> _DeviceExtensions;
    std::vector<const char*> _EnabledExtensions{};

  private: // Methods
    bool IsExtensionSupported(const std::string& requestedExtension);

  };

}
