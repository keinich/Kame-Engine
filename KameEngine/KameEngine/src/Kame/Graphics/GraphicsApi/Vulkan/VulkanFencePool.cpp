#include "kmpch.h"
#include "VulkanFencePool.h"

#include "VulkanDevice.h"

namespace Kame {

  VulkanFencePool::VulkanFencePool(VulkanDevice& device) :
    _Device{ device } {

  }

  VulkanFencePool::~VulkanFencePool() {

    Wait();
    Reset();

    for (VkFence fence : _Fences) {
      vkDestroyFence(_Device.GetHandle(), fence, nullptr);
    }

    _Fences.clear();

  }

  VkFence VulkanFencePool::RequestFence() {
    
    if (_ActiveFenceCount < _Fences.size()) {
      return _Fences.at(_ActiveFenceCount++);
    }

    VkFence fence{ VK_NULL_HANDLE };

    VkFenceCreateInfo createInfo{ VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };

    VkResult result = vkCreateFence(_Device.GetHandle(), &createInfo, nullptr, &fence);
    ASSERT_VULKAN(result);

    _Fences.push_back(fence);
    _ActiveFenceCount++;

    return _Fences.back();

  }

  VkResult VulkanFencePool::Wait(uint32_t timeout) const {
    
    if (_ActiveFenceCount < 1 || _Fences.empty()) {
      return VK_SUCCESS;
    }

    return vkWaitForFences(_Device.GetHandle(), _ActiveFenceCount, _Fences.data(), true, timeout);

  }

  VkResult VulkanFencePool::Reset() {
    
    if (_ActiveFenceCount < 1 || _Fences.empty()) {
      return VK_SUCCESS;
    }

    VkResult result = vkResetFences(_Device.GetHandle(), _ActiveFenceCount, _Fences.data());
    ASSERT_VULKAN(result);

    _ActiveFenceCount = 0;

    return VK_SUCCESS;

  }

}
