#include "kmpch.h"
#include "VulkanDevice.h"
#include "VulkanAPi.h"
#include "VulkanException.h"
#include "Kame/Application/Platform.h"

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

    //TODO Performance Query Extensions

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

    result = vkCreateDevice(gpu.GetHandle(), &createInfo, nullptr, &_Handle);
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

  }

  void VulkanDevice::Shutdown() {
    vkDestroyDevice(_Handle, nullptr);
  }

  VulkanDevice::~VulkanDevice() {}

  bool VulkanDevice::IsExtensionSupported(const std::string& requestedExtension) {
    return std::find_if(
      _DeviceExtensions.begin(), _DeviceExtensions.end(),
      [requestedExtension](auto& deviceExtension) {
        return std::strcmp(deviceExtension.extensionName, requestedExtension.c_str()) == 0;
      }) != _DeviceExtensions.end();
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

    if (result != VK_SUCCESS) 	{
      throw VulkanException{ result, "Cannot create allocator" };
    }
  }

}

