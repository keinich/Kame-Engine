#pragma once

#include <vector>

#include "VulkanCommon.h"

namespace Kame {

  class VulkanDevice;

  class VulkanFencePool {

  public:

    VulkanFencePool(VulkanDevice& device);

    VulkanFencePool(const VulkanFencePool&) = delete;
    VulkanFencePool(VulkanFencePool&& other) = delete;

    ~VulkanFencePool();

    VulkanFencePool& operator=(const VulkanFencePool&) = delete;
    VulkanFencePool& operator=(VulkanFencePool&&) = delete;

    VkFence RequestFence();

    VkResult Wait(uint32_t timeout = std::numeric_limits<uint32_t>::max()) const;

    VkResult Reset();

  private: // Fields

    VulkanDevice& _Device;

    std::vector<VkFence> _Fences;

    uint32_t _ActiveFenceCount{ 0 };

  };

}