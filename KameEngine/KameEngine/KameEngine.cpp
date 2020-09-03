// KameEngine.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "vulkan/vulkan.h"

#define ASSERT_VULKAN(val)\
  if (val!= VK_SUCCESS) {\
    __debugbreak();\
  }

VkInstance instance;

void printStats(VkPhysicalDevice& device) {
  VkPhysicalDeviceProperties properties;
  vkGetPhysicalDeviceProperties(device, &properties);

  std::cout << "Name:             " << properties.deviceName << std::endl;
  uint32_t apiVer = properties.apiVersion;
  std::cout << "API Version:      "
    << VK_VERSION_MAJOR(apiVer) << "." << VK_VERSION_MINOR(apiVer) << "." << VK_VERSION_PATCH(apiVer)
    << std::endl;

  std::cout << "Driver Version:   " << properties.driverVersion << std::endl;
  std::cout << "Vendor ID:        " << properties.vendorID << std::endl;
  std::cout << "Device ID:        " << properties.deviceID << std::endl;
  std::cout << "Device Type:      " << properties.deviceType << std::endl;

  VkPhysicalDeviceFeatures features;
  vkGetPhysicalDeviceFeatures(device, &features);

  std::cout << "Gemoetry Shader:  " << features.geometryShader << std::endl;

  VkPhysicalDeviceMemoryProperties memProps;
  vkGetPhysicalDeviceMemoryProperties(device, &memProps);

  std::cout << std::endl;

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

  VkInstanceCreateInfo instanceInfo;
  instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instanceInfo.pNext = NULL; // Erweiterungen
  instanceInfo.flags = 0; // noch keine Verwendung
  instanceInfo.pApplicationInfo = &appInfo;
  instanceInfo.enabledLayerCount = 0;
  instanceInfo.ppEnabledLayerNames = NULL;
  instanceInfo.enabledExtensionCount = 0;
  instanceInfo.ppEnabledExtensionNames = NULL;

  VkResult result = vkCreateInstance(&instanceInfo, NULL, &instance);

  ASSERT_VULKAN(result);

  uint32_t physicalDeviceCount;
  result = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, NULL);
  ASSERT_VULKAN(result);

  std::cout << "Number of physical devices: " << physicalDeviceCount << std::endl;

  VkPhysicalDevice* physicalDevices = new VkPhysicalDevice[physicalDeviceCount];
  result = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices);
  ASSERT_VULKAN(result);

  for (int i = 0; i < physicalDeviceCount; ++i) {
    printStats(physicalDevices[i]);
  }

}
