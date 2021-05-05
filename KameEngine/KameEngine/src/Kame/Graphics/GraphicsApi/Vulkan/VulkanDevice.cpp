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

    CreateCommandAllocator();

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

  void VulkanDevice::CreateCommandAllocator() {
    VmaVulkanFunctions vma_vulkan_func{};
    vma_vulkan_func.vkAllocateMemory = vkAllocateMemory;
    vma_vulkan_func.vkBindBufferMemory = vkBindBufferMemory;
    vma_vulkan_func.vkBindImageMemory = vkBindImageMemory;
    vma_vulkan_func.vkCreateBuffer = vkCreateBuffer;
    vma_vulkan_func.vkCreateImage = vkCreateImage;
    vma_vulkan_func.vkDestroyBuffer = vkDestroyBuffer;
    vma_vulkan_func.vkDestroyImage = vkDestroyImage;
    vma_vulkan_func.vkFlushMappedMemoryRanges = vkFlushMappedMemoryRanges;
    vma_vulkan_func.vkFreeMemory = vkFreeMemory;
    vma_vulkan_func.vkGetBufferMemoryRequirements = vkGetBufferMemoryRequirements;
    vma_vulkan_func.vkGetImageMemoryRequirements = vkGetImageMemoryRequirements;
    vma_vulkan_func.vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties;
    vma_vulkan_func.vkGetPhysicalDeviceProperties = vkGetPhysicalDeviceProperties;
    vma_vulkan_func.vkInvalidateMappedMemoryRanges = vkInvalidateMappedMemoryRanges;
    vma_vulkan_func.vkMapMemory = vkMapMemory;
    vma_vulkan_func.vkUnmapMemory = vkUnmapMemory;
    vma_vulkan_func.vkCmdCopyBuffer = vkCmdCopyBuffer;

    //VmaAllocatorCreateInfo allocator_info{};
    //allocator_info.physicalDevice = gpu.get_handle();
    //allocator_info.device = handle;
    //allocator_info.instance = gpu.get_instance().get_handle();

    //if (can_get_memory_requirements && has_dedicated_allocation) 	{
    //  allocator_info.flags |= VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT;
    //  vma_vulkan_func.vkGetBufferMemoryRequirements2KHR = vkGetBufferMemoryRequirements2KHR;
    //  vma_vulkan_func.vkGetImageMemoryRequirements2KHR = vkGetImageMemoryRequirements2KHR;
    //}

    //if (is_extension_supported(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME) && is_enabled(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME)) 	{
    //  allocator_info.flags |= VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
    //}

    //allocator_info.pVulkanFunctions = &vma_vulkan_func;

    //result = vmaCreateAllocator(&allocator_info, &memory_allocator);

    //if (result != VK_SUCCESS) 	{
    //  throw VulkanException{ result, "Cannot create allocator" };
    //}
  }

}

