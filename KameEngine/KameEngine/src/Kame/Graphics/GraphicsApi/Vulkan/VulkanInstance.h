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

    bool IsEnabled(const char* extension) const {
      return std::find_if(_EnabledExtensions.begin(), _EnabledExtensions.end(),
        [extension](const char* enableExtension) {
          return strcmp(extension, enableExtension) == 0;
        }) != _EnabledExtensions.end();
    }

  private: // Methods
    void Initialize(bool headless);
    void Shutdown();

    void QueryGpus();
    bool ValidateLayers(
      const std::vector<const char*>& requiredLayers,
      const std::vector<VkLayerProperties>& availableLayers
    );

  private: // Fields
    VkInstance _VkInstance;

    std::vector<NotCopyableReference<VulkanPhysicalDevice>> _PhysicalDevices;

    std::vector<const char*> _EnabledExtensions;

  };

}