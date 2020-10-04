#pragma once

#include "VulkanCommon.h"

namespace Kame {

  class VulkanInstance {
  public:
    friend class VulkanApi;

    VulkanInstance();
    ~VulkanInstance();

    inline VkInstance GetVkInstance() { return _VkInstance; }

  private: // Methods
    void Initialize();
    void Shutdown();

  private: // Fields
    VkInstance _VkInstance;
  };

}