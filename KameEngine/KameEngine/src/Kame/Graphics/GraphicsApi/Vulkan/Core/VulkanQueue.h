#pragma once
#include <cstdint>
#include "../VulkanCommon.h"

namespace Kame {

  class VulkanDevice;

  class VulkanQueue {

  public:
    VulkanQueue(VulkanDevice& device, uint32_t familyIndex, VkQueueFamilyProperties properties, VkBool32 canPresent, uint32_t index);
    VulkanQueue(const VulkanQueue&) = default;
     VulkanQueue(VulkanQueue&& other) noexcept;

    VulkanQueue& operator=(const VulkanQueue&) = delete;
    VulkanQueue& operator=(VulkanQueue&&) = delete;

    const VulkanDevice& GetDevice() const;
    VkQueue GetHandle() const;

    uint32_t GetFamilyIndex() const;
    uint32_t GetIndex() const;
    VkQueueFamilyProperties GetProperties() const;

    VkBool32 CanPresent() const;

  private: // Fields
    VulkanDevice& _Device;
    VkQueue _Handle{ VK_NULL_HANDLE };

    uint32_t _FamilyIndex{ 0 };
    uint32_t _Index{ 0 };

    VkBool32 _CanPresent{ VK_FALSE };

    VkQueueFamilyProperties _Properties{};
  };

}