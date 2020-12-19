#include "kmpch.h"
#include "VulkanQueue.h"

#include "VulkanDevice.h"

namespace Kame {

  VulkanQueue::VulkanQueue(
    VulkanDevice& device,
    uint32_t familyIndex,
    VkQueueFamilyProperties properties,
    VkBool32 canPresent,
    uint32_t index
  ) :
    _Device(device),
    _FamilyIndex(familyIndex),
    _Index(index),
    _CanPresent(canPresent),
    _Properties(properties) {
    vkGetDeviceQueue(device.GetHandle(), familyIndex, index, &_Handle);
  }

  VulkanQueue::VulkanQueue(VulkanQueue&& other) noexcept:
    _Device(other._Device),
    _Handle(other._Handle),
    _FamilyIndex(other._FamilyIndex),
    _Index(other._Index),
    _CanPresent(other._CanPresent),
    _Properties(other._Properties) {
    other._Handle = VK_NULL_HANDLE;
    other._FamilyIndex = {};
    other._Properties = {};
    other._CanPresent = VK_FALSE;
    other._Index = 0;
  }

  const VulkanDevice& VulkanQueue::GetDevice() const {
    return _Device;
  }

  VkQueue VulkanQueue::GetHandle() const {
    return _Handle;
  }

  uint32_t VulkanQueue::GetFamilyIndex() const {
    return _FamilyIndex;
  }

  uint32_t VulkanQueue::GetIndex() const {
    return _Index;
  }

  VkQueueFamilyProperties VulkanQueue::GetProperties() const {
    return _Properties;
  }

  VkBool32 VulkanQueue::CanPresent() const {
    return _CanPresent;
  }

}