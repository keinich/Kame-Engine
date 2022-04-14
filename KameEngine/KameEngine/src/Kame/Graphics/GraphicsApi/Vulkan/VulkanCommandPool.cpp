#include "kmpch.h"
#include "VulkanCommandPool.h"

#include "VulkanDevice.h"

namespace Kame {

  VulkanCommandPool::VulkanCommandPool(
    VulkanDevice& device, uint32_t queueFamilyIndex,
    VulkanRenderFrame* renderFrame, size_t threadIndex
  ) :
    _Device{ device } {

    VkCommandPoolCreateInfo commandPoolCreateInfo;
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.pNext = nullptr;
    commandPoolCreateInfo.flags = 0;
    commandPoolCreateInfo.queueFamilyIndex = 0; //TODO civ

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