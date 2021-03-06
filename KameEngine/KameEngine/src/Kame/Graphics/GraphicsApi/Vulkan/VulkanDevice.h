#pragma once

#include <unordered_map>

#include "VulkanCommon.h"

#include <vk_mem_alloc.h>
#include "VulkanPhyiscalDevice.h"
#include "VulkanQueue.h"

namespace Kame {

  class VulkanDevice {
  public:
    VulkanDevice();
    VulkanDevice(const VulkanDevice&) = delete;
    VulkanDevice(VulkanDevice&&) = delete;
    ~VulkanDevice();
    VulkanDevice& operator=(const VulkanDevice&) = delete;
    VulkanDevice& operator=(VulkanDevice&&) = delete;

    void Init(VulkanPhysicalDevice& gpu, std::unordered_map<const char*, bool> reqeuestedExtensions = {});
    void Shutdown();

    inline VkDevice GetHandle() { return _Handle; }

    const VulkanQueue& GetQueue(uint32_t queueFamilyIndex, uint32_t queueIndex);

  private: // Fields
    VkDevice _Handle{ VK_NULL_HANDLE };
    std::vector<VkExtensionProperties> _DeviceExtensions;
    std::vector<const char*> _EnabledExtensions{};

    std::vector<std::vector<VulkanQueue>> _Queues;

    VmaAllocator _MemoryAllocator{ VK_NULL_HANDLE };

  private: // Methods
    bool IsExtensionSupported(const std::string& requestedExtension);
    bool IsEnabled(const char* extension);

    void CreateCommandAllocator(VulkanPhysicalDevice& gpu, bool getMemoryRequirementsSupported, bool dedicatedAllocationSupported);

  };

}
