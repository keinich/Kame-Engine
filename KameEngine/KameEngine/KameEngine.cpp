// KameEngine.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "vulkan/vulkan.h"
#include <vector>

#define ASSERT_VULKAN(val)\
  if (val!= VK_SUCCESS) {\
    __debugbreak();\
  }

VkInstance instance;
VkDevice device;

void printStats(VkPhysicalDevice& device) {
  VkPhysicalDeviceProperties properties;
  vkGetPhysicalDeviceProperties(device, &properties);

  std::cout << "Name:             " << properties.deviceName << std::endl;
  uint32_t apiVer = properties.apiVersion;
  std::cout << "API Version:      "
    << VK_VERSION_MAJOR(apiVer) << "." << VK_VERSION_MINOR(apiVer) << "." << VK_VERSION_PATCH(apiVer)
    << std::endl;

  std::cout << "Driver Version:          " << properties.driverVersion << std::endl;
  std::cout << "Vendor ID:               " << properties.vendorID << std::endl;
  std::cout << "Device ID:               " << properties.deviceID << std::endl;
  std::cout << "Device Type:             " << properties.deviceType << std::endl;
  std::cout << "DiscreteQueuePriorities: " << properties.limits.discreteQueuePriorities << std::endl;

  VkPhysicalDeviceFeatures features;
  vkGetPhysicalDeviceFeatures(device, &features);

  std::cout << "Gemoetry Shader:  " << features.geometryShader << std::endl;

  VkPhysicalDeviceMemoryProperties memProps;
  vkGetPhysicalDeviceMemoryProperties(device, &memProps);

  for (int i = 0; i < memProps.memoryHeapCount; ++i) {
    std::cout << "Memory " << i << ":         " << memProps.memoryHeaps[i].size << std::endl;
  }

  uint32_t numberOfQueueFamilies = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &numberOfQueueFamilies, NULL);
  VkQueueFamilyProperties* familyProperties = new VkQueueFamilyProperties[numberOfQueueFamilies];
  vkGetPhysicalDeviceQueueFamilyProperties(device, &numberOfQueueFamilies, familyProperties);

  std::cout << std::endl;

  std::cout << "Number of Queue Families " << numberOfQueueFamilies << std::endl;
  for (int i = 0; i < numberOfQueueFamilies; i++) {
    std::cout << std::endl;
    std::cout << "Queue Family #" << i << std::endl;
    std::cout << "VK_QUEUE_GRAPHICS_BIT       " << ((familyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) << std::endl;
    std::cout << "VK_QUEUE_COMPUTE_BIT        " << ((familyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) != 0) << std::endl;
    std::cout << "VK_QUEUE_TRANSFER_BIT       " << ((familyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT) != 0) << std::endl;
    std::cout << "VK_QUEUE_SPARSE_BINDING_BIT " << ((familyProperties[i].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) != 0) << std::endl;
    std::cout << "Queue Count: " << familyProperties[i].queueCount << std::endl;
    std::cout << "Timestamp Valid Bits: " << familyProperties[i].timestampValidBits << std::endl;
    uint32_t width = familyProperties[i].minImageTransferGranularity.width;
    uint32_t height = familyProperties[i].minImageTransferGranularity.height;
    uint32_t depth= familyProperties[i].minImageTransferGranularity.depth;
    std::cout << "Min Image Timestamp Granularity: " << width << ", " << height << ", " << depth << std::endl;
  }

  std::cout << std::endl;

  delete[] familyProperties;
}

int main() {

  VkApplicationInfo appInfo;
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pNext = NULL; // Erweiterungen
  appInfo.pApplicationName = "Kame Sandbox";
  appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 0);
  appInfo.pEngineName = "Kame Engine";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_2;

  uint32_t numberOfLayers = 0;
  vkEnumerateInstanceLayerProperties(&numberOfLayers, NULL);
  VkLayerProperties* layerProperties = new VkLayerProperties[numberOfLayers];
  vkEnumerateInstanceLayerProperties(&numberOfLayers, layerProperties);

  std::cout << "Number of InstanceLayers: " << numberOfLayers << std::endl;
  for (int i = 0; i < numberOfLayers; ++i) {
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << "Name:                   " << layerProperties[i].layerName << std::endl;
    std::cout << "SpecVersion:            " << layerProperties[i].specVersion << std::endl;
    std::cout << "Implementation Version: " << layerProperties[i].implementationVersion << std::endl;
    std::cout << "Description:            " << layerProperties[i].description;
  }

  uint32_t numberOfExtensions = 0;
  vkEnumerateInstanceExtensionProperties(NULL, &numberOfExtensions, NULL);
  VkExtensionProperties* extensionProperties = new VkExtensionProperties[numberOfExtensions];
  vkEnumerateInstanceExtensionProperties(NULL, &numberOfExtensions, extensionProperties);
  
  std::cout << std::endl;
  std::cout << "Number of Extensions: " << numberOfExtensions << std::endl;
  for (int i = 0; i < numberOfExtensions; ++i) {
    std::cout << std::endl;
    std::cout << "Name: " << extensionProperties[i].extensionName << std::endl;
    std::cout << "Spec Version: " << extensionProperties[i].specVersion << std::endl;
  }  

  std::cout << std::endl;
  std::cout << std::endl;

  const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
  };

  VkInstanceCreateInfo instanceInfo;
  instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instanceInfo.pNext = NULL; // Erweiterungen
  instanceInfo.flags = 0; // noch keine Verwendung
  instanceInfo.pApplicationInfo = &appInfo;
  instanceInfo.enabledLayerCount = validationLayers.size();
  instanceInfo.ppEnabledLayerNames = validationLayers.data();
  instanceInfo.enabledExtensionCount = 0;
  instanceInfo.ppEnabledExtensionNames = NULL;

  VkResult result = vkCreateInstance(&instanceInfo, NULL, &instance);

  ASSERT_VULKAN(result);

  uint32_t physicalDeviceCount;
  result = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, NULL);
  ASSERT_VULKAN(result);

  std::cout << "Number of physical devices: " << physicalDeviceCount << std::endl;

  //VkPhysicalDevice* physicalDevices = new VkPhysicalDevice[physicalDeviceCount];
  std::vector<VkPhysicalDevice> physicalDevices;
  physicalDevices.resize(physicalDeviceCount);
  result = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data());
  ASSERT_VULKAN(result);

  for (int i = 0; i < physicalDeviceCount; ++i) {
    printStats(physicalDevices[i]);
  }

  float queuePrios[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  VkDeviceQueueCreateInfo deviceQueueCreateInfo;
  deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  deviceQueueCreateInfo.pNext = NULL;
  deviceQueueCreateInfo.flags = 0;
  deviceQueueCreateInfo.queueFamilyIndex = 0; //TODO eigentlich beste aussuchen
  deviceQueueCreateInfo.queueCount = 4; //TODO prüfen, ob 4 gehen
  deviceQueueCreateInfo.pQueuePriorities = queuePrios; // alle haben die gleiche Prio, sonst Array von floats

  VkPhysicalDeviceFeatures usedFeatures = {};
  VkDeviceCreateInfo deviceCreateInfo;
  deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  deviceCreateInfo.pNext = NULL;
  deviceCreateInfo.flags = 0;
  deviceCreateInfo.queueCreateInfoCount = 1;
  deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
  deviceCreateInfo.enabledLayerCount = 0;
  deviceCreateInfo.ppEnabledLayerNames = NULL;
  deviceCreateInfo.enabledExtensionCount = 0;
  deviceCreateInfo.ppEnabledExtensionNames = NULL;
  deviceCreateInfo.pEnabledFeatures = &usedFeatures;

  //TODO pick best device instead of first device
  result = vkCreateDevice(physicalDevices[0], &deviceCreateInfo, NULL, &device);
  ASSERT_VULKAN(result);

  vkDeviceWaitIdle(device);

  vkDestroyDevice(device, NULL);
  vkDestroyInstance(instance, NULL);

  delete[] layerProperties;
  delete[] extensionProperties;
}
