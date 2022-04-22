#include "kmpch.h"
#include "VulkanRenderContext.h"

namespace Kame {

  VkFormat VulkanRenderContext::DEFAULT_VK_FORMAT = VK_FORMAT_R8G8B8A8_SRGB;

  VulkanRenderContext::VulkanRenderContext(
    VulkanDevice& device, VkSurfaceKHR surface, uint32_t window_width, uint32_t window_height
  ) {

  }

}
