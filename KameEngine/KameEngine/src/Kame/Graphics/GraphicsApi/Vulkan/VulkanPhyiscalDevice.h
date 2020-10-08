#pragma once

#include "VulkanCommon.h"

namespace Kame {

  class VulkanPhysicalDevice {
  public:
    VulkanPhysicalDevice(VkPhysicalDevice handle);
    ~VulkanPhysicalDevice();

    inline VkPhysicalDevice GetHandle() { return _Handle; }

  private: // Methods
    void Initialize();

  private: // Fields
    VkPhysicalDevice _Handle;
  };

}
