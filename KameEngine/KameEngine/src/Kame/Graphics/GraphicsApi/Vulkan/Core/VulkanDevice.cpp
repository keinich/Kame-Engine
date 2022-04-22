#include "kmpch.h"
#include "Kame/Application/Platform.h"
#include <Kame/Application/Window.h>
#include "VulkanInstance.h"
#include "VulkanDevice.h"
#include "VulkanException.h"

__pragma(warning(push, 0))
#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>
__pragma(warning(pop))

namespace Kame {

  VulkanDevice::VulkanDevice() {}

  void VulkanDevice::Init(
    VulkanPhysicalDevice& gpu,
    std::unordered_map<const char*, bool> requestedExtensions
  ) {
    uint32_t numberOfQueueFamilies = gpu.GetQueueFamilyProperties().size();
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos(numberOfQueueFamilies, { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO });
    std::vector<std::vector<float>> queuePriorities(numberOfQueueFamilies);

    for (uint32_t i = 0U; i < numberOfQueueFamilies; ++i) {
      const VkQueueFamilyProperties& queueFamilyProperty = gpu.GetQueueFamilyProperties()[i];
      queuePriorities[i].resize(queueFamilyProperty.queueCount, 1.0f);
      VkDeviceQueueCreateInfo& queueCreateInfo = queueCreateInfos[i];

      queueCreateInfo.queueFamilyIndex = i;
      queueCreateInfo.queueCount = queueFamilyProperty.queueCount;
      queueCreateInfo.pQueuePriorities = queuePriorities[i].data();
    }

    InitExtensions(gpu, requestedExtensions);

    // Build the CreateInfo for the device
    bool getMemoryRequirementsSupported = IsExtensionSupported("VK_KHR_get_memory_requirements2");
    bool dedicatedAllocationSupported = IsExtensionSupported("VK_KHR_dedicated_allocation");

    VkDeviceCreateInfo createInfo{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
    createInfo.pNext = gpu.GetExtensionFeatureChain();

    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = queueCreateInfos.size();
    createInfo.enabledExtensionCount = _EnabledExtensions.size();
    createInfo.ppEnabledExtensionNames = _EnabledExtensions.data();
    createInfo.pEnabledFeatures = &(gpu.GetRequestedFeatures());

    //TODO get requested Features according to the game
    VkPhysicalDeviceFeatures usedFeatures1 = {};
    usedFeatures1.samplerAnisotropy = VK_TRUE;
    usedFeatures1.fillModeNonSolid = VK_TRUE;

    createInfo.pEnabledFeatures = &usedFeatures1;

    VkResult result = vkCreateDevice(gpu.GetHandle(), &createInfo, nullptr, &_Handle);
    ASSERT_VULKAN(result);

    // Create the queues
    _Queues.resize(numberOfQueueFamilies);

    for (uint32_t queueFamilyIndex = 0U; queueFamilyIndex < numberOfQueueFamilies; ++queueFamilyIndex) {
      const VkQueueFamilyProperties& queueFamilyProperty = gpu.GetQueueFamilyProperties()[queueFamilyIndex];

      Reference<Window> w = Kame::Platform::GetMainWindow();
      VkSurfaceKHR surface = w->GetSurface()->GetVkSurface();
      VkBool32 presentSupported = gpu.IsPresentSupported(surface, queueFamilyIndex);

      for (uint32_t queueIndex = 0; queueIndex < queueFamilyProperty.queueCount; queueIndex++) {
        _Queues[queueFamilyIndex].emplace_back(
          *this, queueFamilyIndex, queueFamilyProperty, presentSupported, queueIndex
        );
      }
    }

    CreateCommandAllocator(gpu, getMemoryRequirementsSupported, dedicatedAllocationSupported);

    _CommandPool = CreateNotCopyableReference<VulkanCommandPool>(
      *this,
      GetQueueByFlags(VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT, 0).GetFamilyIndex()
      );
    _FencePool = CreateNotCopyableReference<VulkanFencePool>(*this);

  }

  void VulkanDevice::Shutdown() {

    _CommandPool.reset();

    if (_MemoryAllocator != VK_NULL_HANDLE) {
      VmaStats stats;
      vmaCalculateStats(_MemoryAllocator, &stats);

      KM_CORE_INFO("Total device memory leaked: {} bytes.", stats.total.usedBytes);

      vmaDestroyAllocator(_MemoryAllocator);
    }

    if (_Handle != VK_NULL_HANDLE) {
      vkDestroyDevice(_Handle, nullptr);
    }
  }

  const VulkanQueue& VulkanDevice::GetQueue(uint32_t queueFamilyIndex, uint32_t queueIndex) {
    return _Queues[queueFamilyIndex][queueIndex];
  }

  const VulkanQueue& VulkanDevice::GetQueueByFlags(VkQueueFlags requiredQueueFlags, uint32_t queueIndex) {
    for (uint32_t queueFamilyIndex = 0U; queueFamilyIndex < _Queues.size(); ++queueFamilyIndex) {
      VulkanQueue const& firstQueue = _Queues[queueFamilyIndex][0];

      VkQueueFlags queueFlags = firstQueue.GetProperties().queueFlags;
      VkQueueFlags queueCount = firstQueue.GetProperties().queueCount;

      if (((queueFlags & requiredQueueFlags) == requiredQueueFlags) && queueIndex < queueCount) {
        return _Queues[queueFamilyIndex][queueIndex];
      }
    }
  }

  VulkanDevice::~VulkanDevice() {}

  bool VulkanDevice::IsExtensionSupported(const std::string& requestedExtension) {
    return std::find_if(
      _DeviceExtensions.begin(), _DeviceExtensions.end(),
      [requestedExtension](auto& deviceExtension) {
        return std::strcmp(deviceExtension.extensionName, requestedExtension.c_str()) == 0;
      }) != _DeviceExtensions.end();
  }

  void VulkanDevice::InitExtensions(VulkanPhysicalDevice& gpu, std::unordered_map<const char*, bool> requestedExtensions) {
    // Check extensions to enable Vma Dedicated Allocation
    uint32_t numberOfDeviceExtensions;
    VkResult result = vkEnumerateDeviceExtensionProperties(gpu.GetHandle(), nullptr, &numberOfDeviceExtensions, nullptr);
    ASSERT_VULKAN(result);
    _DeviceExtensions = std::vector<VkExtensionProperties>(numberOfDeviceExtensions);
    result = vkEnumerateDeviceExtensionProperties(gpu.GetHandle(), nullptr, &numberOfDeviceExtensions, _DeviceExtensions.data());

    // Log the supported extensions
    if (_DeviceExtensions.size() > 0) {
      KM_INFO("Device supports the following extensions:");
      for (auto& extensions : _DeviceExtensions) {
        KM_INFO("  \t{}", extensions.extensionName);
      }
    }

    // Dedicated Allocation Extensions
    bool getMemoryRequirementsSupported = IsExtensionSupported("VK_KHR_get_memory_requirements2");
    bool dedicatedAllocationSupported = IsExtensionSupported("VK_KHR_dedicated_allocation");

    if (getMemoryRequirementsSupported && dedicatedAllocationSupported) {
      _EnabledExtensions.push_back("VK_KHR_get_memory_requirements2");
      _EnabledExtensions.push_back("VK_KHR_dedicated_allocation");

      KM_INFO("Dedicated Allocation enabled");
    }

    if (IsExtensionSupported("VK_KHR_performance_query") && IsExtensionSupported("VK_EXT_host_query_reset")) {
      auto perfCounterFeatures = gpu.RequestExtensionFeatures<VkPhysicalDevicePerformanceQueryFeaturesKHR>(
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PERFORMANCE_QUERY_FEATURES_KHR
        );
      auto hostQueryResetFeatures = gpu.RequestExtensionFeatures<VkPhysicalDeviceHostQueryResetFeatures>(
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_QUERY_RESET_FEATURES
        );

      if (perfCounterFeatures.performanceCounterQueryPools && hostQueryResetFeatures.hostQueryReset) {
        _EnabledExtensions.push_back("VK_KHR_performance_query");
        _EnabledExtensions.push_back("VK_EXT_host_query_reset");
        KM_INFO("Performance query enabled");
      }
    }

    // Check tat extensions are supported before trying to create the device
    std::vector<const char*> unsopportedExtensions{};
    for (std::pair<const char* const, bool>& extension : requestedExtensions) {
      if (IsExtensionSupported(extension.first)) {
        _EnabledExtensions.emplace_back(extension.first);
      }
      else {
        unsopportedExtensions.emplace_back(extension.first);
      }
    }

    if (_EnabledExtensions.size() > 0) {
      KM_INFO("Device supports the following requested extensions:");
      for (const char*& extension : _EnabledExtensions) {
        KM_INFO(" \t{}", extension);
      }
    }

    if (unsopportedExtensions.size() > 0) {
      bool error = false;
      for (const char*& extension : unsopportedExtensions) {
        bool extensionIsOptional = requestedExtensions[extension];
        if (extensionIsOptional) {
          KM_WARN("Optional device extension {} not available, some features may be disabled", extension);
        }
        else {
          KM_ERROR("Required extension {} not available, cannot run", extension);
        }
        error = error || !extensionIsOptional;
      }

      if (error) {
        throw VulkanException(VK_ERROR_EXTENSION_NOT_PRESENT, "Extension not present");
      }
    }
  }

  bool VulkanDevice::IsEnabled(const char* extension) {
    return std::find_if(
      _EnabledExtensions.begin(), _EnabledExtensions.end(),
      [extension](const char* enabledExtension) {
        return std::strcmp(extension, enabledExtension) == 0;
      }
    ) != _EnabledExtensions.end();
  }

  void VulkanDevice::CreateCommandAllocator(
    VulkanPhysicalDevice& gpu, bool getMemoryRequirementsSupported, bool dedicatedAllocationSupported
  ) {
    VmaVulkanFunctions vmaVulkanFunc{};
    vmaVulkanFunc.vkAllocateMemory = vkAllocateMemory;
    vmaVulkanFunc.vkBindBufferMemory = vkBindBufferMemory;
    vmaVulkanFunc.vkBindImageMemory = vkBindImageMemory;
    vmaVulkanFunc.vkCreateBuffer = vkCreateBuffer;
    vmaVulkanFunc.vkCreateImage = vkCreateImage;
    vmaVulkanFunc.vkDestroyBuffer = vkDestroyBuffer;
    vmaVulkanFunc.vkDestroyImage = vkDestroyImage;
    vmaVulkanFunc.vkFlushMappedMemoryRanges = vkFlushMappedMemoryRanges;
    vmaVulkanFunc.vkFreeMemory = vkFreeMemory;
    vmaVulkanFunc.vkGetBufferMemoryRequirements = vkGetBufferMemoryRequirements;
    vmaVulkanFunc.vkGetImageMemoryRequirements = vkGetImageMemoryRequirements;
    vmaVulkanFunc.vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties;
    vmaVulkanFunc.vkGetPhysicalDeviceProperties = vkGetPhysicalDeviceProperties;
    vmaVulkanFunc.vkInvalidateMappedMemoryRanges = vkInvalidateMappedMemoryRanges;
    vmaVulkanFunc.vkMapMemory = vkMapMemory;
    vmaVulkanFunc.vkUnmapMemory = vkUnmapMemory;
    vmaVulkanFunc.vkCmdCopyBuffer = vkCmdCopyBuffer;

    VmaAllocatorCreateInfo allocatorInfo{};
    allocatorInfo.physicalDevice = gpu.GetHandle();
    allocatorInfo.device = _Handle;
    allocatorInfo.instance = gpu.GetVulkanInstance().GetVkInstance();

    if (getMemoryRequirementsSupported && dedicatedAllocationSupported) {
      allocatorInfo.flags |= VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT;
      vmaVulkanFunc.vkGetBufferMemoryRequirements2KHR = vkGetBufferMemoryRequirements2KHR;
      vmaVulkanFunc.vkGetImageMemoryRequirements2KHR = vkGetImageMemoryRequirements2KHR;
    }

    if (
      IsExtensionSupported(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME) &&
      IsEnabled(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME)
      ) {
      allocatorInfo.flags |= VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
    }

    allocatorInfo.pVulkanFunctions = &vmaVulkanFunc;

    VkResult result = vmaCreateAllocator(&allocatorInfo, &_MemoryAllocator);

    if (result != VK_SUCCESS) {
      throw VulkanException{ result, "Cannot create allocator" };
    }
        
  }

}

