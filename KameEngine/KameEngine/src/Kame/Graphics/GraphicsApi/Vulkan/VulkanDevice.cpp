#include "kmpch.h"
#include "VulkanDevice.h"
#include <Kame\Graphics\GraphicsApi\Vulkan\VulkanAPi.h>

namespace Kame {

  VulkanDevice::VulkanDevice(
    VulkanPhysicalDevice& gpu,
    VkSurfaceKHR surface,
    std::unordered_map<const char*, bool> reqeuestedExtensions
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

    float queuePrios[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    VkDeviceQueueCreateInfo deviceQueueCreateInfo;
    deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    deviceQueueCreateInfo.pNext = nullptr;
    deviceQueueCreateInfo.flags = 0;
    deviceQueueCreateInfo.queueFamilyIndex = 0; //TODO eigentlich beste aussuchen
    deviceQueueCreateInfo.queueCount = 1; //TODO pr�fen, ob 4 gehen
    deviceQueueCreateInfo.pQueuePriorities = queuePrios; // alle haben die gleiche Prio, sonst Array von floats

    VkPhysicalDeviceFeatures usedFeatures = {};
    usedFeatures.samplerAnisotropy = VK_TRUE;
    usedFeatures.fillModeNonSolid = VK_TRUE;

    const std::vector<const char*> deviceExtensions = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    VkDeviceCreateInfo deviceCreateInfo;
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pNext = nullptr;
    deviceCreateInfo.flags = 0;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
    deviceCreateInfo.enabledLayerCount = 0;
    deviceCreateInfo.ppEnabledLayerNames = nullptr;
    deviceCreateInfo.enabledExtensionCount = deviceExtensions.size();
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
    deviceCreateInfo.pEnabledFeatures = &usedFeatures;

    //TODO pick best device instead of first device
    result = vkCreateDevice(gpu.GetHandle(), &deviceCreateInfo, nullptr, &_Handle);
    ASSERT_VULKAN(result);
  }

  VulkanDevice::~VulkanDevice() {}

}

