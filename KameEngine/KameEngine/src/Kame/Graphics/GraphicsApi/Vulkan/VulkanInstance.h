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


    void Init(bool headless);
    void FillApplicationInfo(VkApplicationInfo& appInfo);
    void Shutdown();

    void QueryGpus();
    void InitExtensions(bool headless);
    void InitLayers();

    bool ValidateLayers(
      const std::vector<const char*>& requiredLayers,
      const std::vector<VkLayerProperties>& availableLayers
    );

  private: // Fields
    VkInstance _VkInstance;

    std::vector<NotCopyableReference<VulkanPhysicalDevice>> _PhysicalDevices;

    std::vector<const char*> _EnabledExtensions;
    std::vector<const char*> _RequestedLayers;

#if defined(KAME_DEBUG) || defined(VKB_VALIDATION_LAYERS)
    /**
     * @brief Debug utils messenger callback for VK_EXT_Debug_Utils
     */
    VkDebugUtilsMessengerEXT _DebugUtilsMessenger{ VK_NULL_HANDLE };

    /**
     * @brief The debug report callback
     */
    VkDebugReportCallbackEXT _DebugReportCallback{ VK_NULL_HANDLE };
#endif

  };

}