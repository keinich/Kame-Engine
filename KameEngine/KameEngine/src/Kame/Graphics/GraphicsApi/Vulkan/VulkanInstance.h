#pragma once

#include "VulkanCommon.h"
#include "VulkanPhyiscalDevice.h"

namespace Kame {

  class VulkanPhysicalDevice;

  class VulkanInstance {
  public:
    friend class VulkanApi;

    VulkanInstance();
    ~VulkanInstance();

    inline VkInstance GetVkInstance() { return _VkInstance; }

    VulkanPhysicalDevice& GetBestPhysicalDevice();

  private: // Methods
    void Initialize();
    void Shutdown();

    void QueryGpus();

  private: // Fields
    VkInstance _VkInstance;

    std::vector<NotCopyableReference<VulkanPhysicalDevice>> _PhysicalDevices;

  };

}