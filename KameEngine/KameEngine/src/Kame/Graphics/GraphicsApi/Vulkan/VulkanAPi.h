#pragma once

// Kame
#include <Kame/Core/References.h>
#include "../GraphicsApi.h"
#include "VulkanInstance.h"

namespace Kame {

  class VulkanInstance;

  class VulkanApi : public GraphicsApi {
    friend class GraphicsApi;
  public: // Methods

    inline static VulkanApi* Get() { return _Instance; };
    static VulkanApi* CreateInstance();

    inline VkInstance GetVkInstance() { return _VulkanInstance->GetVkInstance(); }

  protected:
    VulkanApi() {};
    ~VulkanApi() {};

    virtual void Initialize() override;
    virtual void Shutdown() override;

  private: // Fields
    static VulkanApi* _Instance;

    NotCopyableReference<VulkanInstance> _VulkanInstance;
  };

}