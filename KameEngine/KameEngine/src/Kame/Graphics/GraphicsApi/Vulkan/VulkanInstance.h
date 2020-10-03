#pragma once

#include "VulkanCommon.h"

namespace Kame {

  class VulkanInstance {
  public:
    VulkanInstance();
    ~VulkanInstance();

  private: // Methods
    void Create();
    void Destroy();

  private: // Fields
    VkInstance _VkInstance;
  };

}