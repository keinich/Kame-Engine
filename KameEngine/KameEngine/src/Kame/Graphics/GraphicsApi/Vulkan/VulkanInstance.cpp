#include "kmpch.h"
#define VOLK_IMPLEMENTATION
#include "VulkanCommon.h"
#include "VulkanInstance.h"
#include "Kame/Engine.h"

namespace Kame {

  VulkanInstance::VulkanInstance() {
    _VkInstance = VK_NULL_HANDLE;
  }

  VulkanInstance::~VulkanInstance() {}

  VulkanPhysicalDevice& VulkanInstance::GetBestPhysicalDevice() {
    return *(_PhysicalDevices.at(0));
  }

  void VulkanInstance::Initialize() {
    VkApplicationInfo appInfo;
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = nullptr; // Erweiterungen
    //appInfo.pApplicationName = "Kame Sandbox";
    appInfo.pApplicationName = Engine::GetGameName();
    appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 0);
    appInfo.pEngineName = "Kame Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_2;

    const std::vector<const char*> validationLayers = {
      "VK_LAYER_KHRONOS_validation"
    };

    uint32_t numberOfRequiredExtensions = 0;
    auto requiredExtensions = Engine::GetRequiredVulkanInstanceExtensions(&numberOfRequiredExtensions);

    VkInstanceCreateInfo instanceInfo;
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pNext = nullptr; // Erweiterungen
    instanceInfo.flags = 0; // noch keine Verwendung
    instanceInfo.pApplicationInfo = &appInfo;
    instanceInfo.enabledLayerCount = validationLayers.size();
    instanceInfo.ppEnabledLayerNames = validationLayers.data();
    instanceInfo.enabledExtensionCount = numberOfRequiredExtensions;
    instanceInfo.ppEnabledExtensionNames = requiredExtensions;

    VkResult result = volkInitialize();
    ASSERT_VULKAN(result);

    result = vkCreateInstance(&instanceInfo, NULL, &_VkInstance);
    ASSERT_VULKAN(result);

    volkLoadInstance(_VkInstance);

    QueryGpus();
  }
  void VulkanInstance::Shutdown() {
    vkDestroyInstance(_VkInstance, nullptr);
  }

  void VulkanInstance::QueryGpus() {
    uint32_t physicalDeviceCount;
    VkResult result = vkEnumeratePhysicalDevices(_VkInstance, &physicalDeviceCount, nullptr);
    ASSERT_VULKAN(result);

    std::cout << "Number of physical devices: " << physicalDeviceCount << std::endl;

    std::vector<VkPhysicalDevice> physicalDevices;
    physicalDevices.resize(physicalDeviceCount);

    result = vkEnumeratePhysicalDevices(_VkInstance, &physicalDeviceCount, physicalDevices.data());
    ASSERT_VULKAN(result);

    for each (VkPhysicalDevice physicalDevice in physicalDevices) {
      _PhysicalDevices.push_back(CreateNotCopyableReference<VulkanPhysicalDevice>(physicalDevice));
    }

  }

}