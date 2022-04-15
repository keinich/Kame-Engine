#pragma once
#include <cstdint>

#include "VulkanCommon.h"
#include "VulkanCommandBuffer.h"

namespace Kame {

  class VulkanDevice;
  class VulkanRenderFrame;

  class VulkanCommandPool {

  public:

    VulkanCommandPool(
      VulkanDevice& device, uint32_t queueFamilyIndex,
      VulkanRenderFrame* renderFrame = nullptr,
      size_t threadIndex = 0,
      VulkanCommandBuffer::ResetMode resetMode = VulkanCommandBuffer::ResetMode::ResetPool
   );

    VulkanCommandPool(const VulkanCommandPool&) = delete;
    VulkanCommandPool(VulkanCommandPool&& other) = delete;

    ~VulkanCommandPool();

    VulkanCommandPool &operator=(const VulkanCommandPool &) = delete;
    VulkanCommandPool &operator=(VulkanCommandPool &&) = delete;

    VkCommandPool GetHandle() const;

  private: // Fields

    VulkanDevice& _Device;

    VkCommandPool _Handle{VK_NULL_HANDLE};
  };
}