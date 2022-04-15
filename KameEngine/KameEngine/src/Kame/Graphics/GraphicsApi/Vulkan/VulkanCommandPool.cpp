#include "kmpch.h"
#include "VulkanCommandPool.h"

#include "VulkanDevice.h"

namespace Kame {

  VulkanCommandPool::VulkanCommandPool(
    VulkanDevice& device, uint32_t queueFamilyIndex,
    VulkanRenderFrame* renderFrame, size_t threadIndex, VulkanCommandBuffer::ResetMode resetMode
  ) :
    _Device{ device } {

    VkCommandPoolCreateFlags flags;
    switch (resetMode) {
    case VulkanCommandBuffer::ResetMode::ResetIndividually:
    case VulkanCommandBuffer::ResetMode::AlwaysAllocate:
      flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
      break;
    case VulkanCommandBuffer::ResetMode::ResetPool:
    default:
      flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
      break;
    }

    VkCommandPoolCreateInfo commandPoolCreateInfo;
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.pNext = nullptr;
    commandPoolCreateInfo.flags = flags;
    commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndex;

    VkResult result = vkCreateCommandPool(device.GetHandle(), &commandPoolCreateInfo, nullptr, &_Handle);
    ASSERT_VULKAN(result);
  }

  VulkanCommandPool::~VulkanCommandPool() {

    if (_Handle != VK_NULL_HANDLE) {
      vkDestroyCommandPool(_Device.GetHandle(), _Handle, nullptr);
    }

  }

  VkCommandPool VulkanCommandPool::GetHandle() const {
    return _Handle;
  }

}