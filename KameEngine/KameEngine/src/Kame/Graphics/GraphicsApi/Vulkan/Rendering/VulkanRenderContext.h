#pragma once

#include "..\VulkanCommon.h"
#include "..\Core\VulkanDevice.h"

namespace Kame {

  class VulkanRenderContext {

  public:

    static VkFormat DEFAULT_VK_FORMAT;

    VulkanRenderContext(
      VulkanDevice& device, VkSurfaceKHR surface, uint32_t window_width, uint32_t window_height
    );

    virtual ~VulkanRenderContext() = default;

    VulkanRenderContext(const VulkanRenderContext&) = delete;
    VulkanRenderContext(VulkanRenderContext&&) = delete;

    VulkanRenderContext& operator=(const VulkanRenderContext&) = delete;
    VulkanRenderContext& operator=(VulkanRenderContext&&) = delete;
  };

}