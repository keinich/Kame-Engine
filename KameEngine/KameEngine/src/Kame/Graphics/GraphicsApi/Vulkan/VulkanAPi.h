#pragma once

// Kame
#include <Kame/Core/References.h>
#include "../GraphicsApi.h"
#include "Core/VulkanInstance.h"
#include "Core/VulkanDevice.h"

namespace Kame {

  class VulkanInstance;

  class VulkanApi : public GraphicsApi {
    friend class GraphicsApi;

  public: // Methods

    inline static VulkanApi* Get() { return _Instance; };
    static VulkanApi* CreateInstance();

    virtual void DestroyInstance() override;

    inline VulkanInstance& GetInstance() { return *_VulkanInstance; }
    inline VkInstance GetVkInstance() { return _VulkanInstance->GetVkInstance(); }

    inline VulkanDevice& GetDevice() { return *_VulkanDevice; }
    inline VkDevice GetVkDevice() { return _VulkanDevice->GetHandle(); }

    void AddDeviceExtension(const char* extension, bool optional = false);
    const std::unordered_map<const char*, bool> GetDeviceExtensions();

  protected:
    VulkanApi() {};
    ~VulkanApi() {};

    virtual void InitInstance() override;
    virtual void ShutdownInstance() override;

    virtual void CreateWindowSurfaceInternal(Reference<Window> window) override;

  private: // Fields
    static VulkanApi* _Instance;

    NotCopyableReference<VulkanInstance> _VulkanInstance;
    NotCopyableReference<VulkanDevice> _VulkanDevice;
      
    std::unordered_map<const char*, bool> _DeviceExtensions;
  };

}