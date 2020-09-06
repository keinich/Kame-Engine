// KameEngine.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
// #include "vulkan/vulkan.h"
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include <vector>
#include <fstream>

#define ASSERT_VULKAN(val)\
  if (val!= VK_SUCCESS) {\
    __debugbreak();\
  }

VkInstance instance;
VkSurfaceKHR surface;
VkDevice device;
VkSwapchainKHR swapchain;
VkImageView* imageViews;
VkShaderModule shaderModuleVert;
VkShaderModule shaderModuleFrag;
uint32_t numberOfImagesInSwapchain = 0;
GLFWwindow* window;

const uint32_t WIDTH = 400;
const uint32_t HEIGHT = 300;

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
  vkGetPhysicalDeviceQueueFamilyProperties(device, &numberOfQueueFamilies, nullptr);
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
    uint32_t depth = familyProperties[i].minImageTransferGranularity.depth;
    std::cout << "Min Image Timestamp Granularity: " << width << ", " << height << ", " << depth << std::endl;
  }

  VkSurfaceCapabilitiesKHR surfaceCapabilities;
  std::cout << "Surface capabilities: " << std::endl;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &surfaceCapabilities);
  std::cout << "\t minImageCount:           " << surfaceCapabilities.minImageCount << std::endl;
  std::cout << "\t maxImageCount:           " << surfaceCapabilities.maxImageCount << std::endl;
  std::cout << "\t currentExtent:           " << surfaceCapabilities.currentExtent.width << "/" << surfaceCapabilities.currentExtent.height << std::endl;
  std::cout << "\t minImageExtent:          " << surfaceCapabilities.minImageExtent.width << "/" << surfaceCapabilities.minImageExtent.height << std::endl;
  std::cout << "\t maxImageExtent:          " << surfaceCapabilities.maxImageExtent.width << "/" << surfaceCapabilities.maxImageExtent.height << std::endl;
  std::cout << "\t maxImageArrayLayers:     " << surfaceCapabilities.maxImageArrayLayers << std::endl;
  std::cout << "\t supportedTransforms:     " << surfaceCapabilities.supportedTransforms << std::endl;
  std::cout << "\t currentTransform:        " << surfaceCapabilities.currentTransform << std::endl;
  std::cout << "\t supportedCompositeAlpha: " << surfaceCapabilities.supportedCompositeAlpha << std::endl;
  std::cout << "\t supportedUsageFlags:     " << surfaceCapabilities.supportedUsageFlags << std::endl;

  uint32_t numberOfFormats = 0;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &numberOfFormats, nullptr);
  VkSurfaceFormatKHR* surfaceFormats = new VkSurfaceFormatKHR[numberOfFormats];
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &numberOfFormats, surfaceFormats);

  std::cout << std::endl;

  std::cout << "Number of FOrmats: " << numberOfFormats << std::endl;
  for (int i = 0; i < numberOfFormats; ++i) {
    std::cout << "Format: " << surfaceFormats[i].format << std::endl;
  }

  uint32_t numberOfPresentationModes = 0;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &numberOfPresentationModes, nullptr);
  VkPresentModeKHR* presentModes = new VkPresentModeKHR[numberOfPresentationModes];
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &numberOfPresentationModes, presentModes);

  std::cout << std::endl;
  std::cout << "Number of Presentation Modes: " << numberOfPresentationModes;
  std::cout << std::endl;
  for (int i = 0; i < numberOfPresentationModes; ++i) {
    std::cout << "Supported presentation mode: " << presentModes[i] << std::endl;
  }

  std::cout << std::endl;
  delete[] familyProperties;
  delete[] surfaceFormats;
  delete[] presentModes;
}

std::vector<char> readFile(const std::string& filename) {
  std::ifstream file(filename, std::ios::binary | std::ios::ate);

  if (!file) {
    throw std::runtime_error("couldnt open file");
  }

  size_t fileSize = (size_t)file.tellg();
  std::vector<char> fileBuffer(fileSize);
  file.seekg(0);
  file.read(fileBuffer.data(), fileSize);
  file.close();
  return fileBuffer;
}

void startGlfw() {

  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  window = glfwCreateWindow(WIDTH, HEIGHT, "Kame Engine", nullptr, nullptr);

}

void createShaderModule(const std::vector<char>& code, VkShaderModule *shaderModule) {
  VkShaderModuleCreateInfo shaderCreateInfo;
  shaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  shaderCreateInfo.pNext = nullptr;
  shaderCreateInfo.flags = 0;
  shaderCreateInfo.codeSize = code.size();
  shaderCreateInfo.pCode = (uint32_t*)code.data();

  VkResult result = vkCreateShaderModule(device, &shaderCreateInfo, nullptr, shaderModule);
  ASSERT_VULKAN(result);
}

void startVulkan() {

  VkApplicationInfo appInfo;
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pNext = nullptr; // Erweiterungen
  appInfo.pApplicationName = "Kame Sandbox";
  appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 0);
  appInfo.pEngineName = "Kame Engine";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_2;

  uint32_t numberOfLayers = 0;
  vkEnumerateInstanceLayerProperties(&numberOfLayers, nullptr);
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
  vkEnumerateInstanceExtensionProperties(nullptr, &numberOfExtensions, nullptr);
  VkExtensionProperties* extensionProperties = new VkExtensionProperties[numberOfExtensions];
  vkEnumerateInstanceExtensionProperties(nullptr, &numberOfExtensions, extensionProperties);

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

  uint32_t numberOfGlfwExtensions = 0;
  auto glfwExtensions = glfwGetRequiredInstanceExtensions(&numberOfGlfwExtensions);

  const std::vector<const char*> usedExtensions = {
    "VK_KHR_surface"
  };

  VkInstanceCreateInfo instanceInfo;
  instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instanceInfo.pNext = nullptr; // Erweiterungen
  instanceInfo.flags = 0; // noch keine Verwendung
  instanceInfo.pApplicationInfo = &appInfo;
  instanceInfo.enabledLayerCount = validationLayers.size();
  instanceInfo.ppEnabledLayerNames = validationLayers.data();
  instanceInfo.enabledExtensionCount = numberOfGlfwExtensions;
  instanceInfo.ppEnabledExtensionNames = glfwExtensions;

  VkResult result = vkCreateInstance(&instanceInfo, NULL, &instance);
  ASSERT_VULKAN(result);

  result = glfwCreateWindowSurface(instance, window, nullptr, &surface);
  ASSERT_VULKAN(result);

  uint32_t physicalDeviceCount;
  result = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);
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
  deviceQueueCreateInfo.pNext = nullptr;
  deviceQueueCreateInfo.flags = 0;
  deviceQueueCreateInfo.queueFamilyIndex = 0; //TODO eigentlich beste aussuchen
  deviceQueueCreateInfo.queueCount = 1; //TODO prüfen, ob 4 gehen
  deviceQueueCreateInfo.pQueuePriorities = queuePrios; // alle haben die gleiche Prio, sonst Array von floats

  VkPhysicalDeviceFeatures usedFeatures = {};

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
  result = vkCreateDevice(physicalDevices[0], &deviceCreateInfo, nullptr, &device);
  ASSERT_VULKAN(result);

  VkQueue queue;
  vkGetDeviceQueue(device, 0, 0, &queue);

  VkBool32 surfaceSupport = false;
  result = vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevices[0], 0, surface, &surfaceSupport);
  ASSERT_VULKAN(result);

  if (!surfaceSupport) {
    std::cerr << "Surface nut supported!" << std::endl;
    __debugbreak;
  }

  VkSwapchainCreateInfoKHR swapChainCreateInfo;
  swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swapChainCreateInfo.pNext = nullptr;;
  swapChainCreateInfo.flags = 0;
  swapChainCreateInfo.surface = surface;
  swapChainCreateInfo.minImageCount = 3; //TODO check if valid
  swapChainCreateInfo.imageFormat = VK_FORMAT_B8G8R8A8_UNORM; //TODO check if valid
  swapChainCreateInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR; //TODO check if valid
  swapChainCreateInfo.imageExtent = VkExtent2D{ WIDTH, HEIGHT }; //TODO
  swapChainCreateInfo.imageArrayLayers = 1;
  swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE; // TODO civ
  swapChainCreateInfo.queueFamilyIndexCount = 0;
  swapChainCreateInfo.pQueueFamilyIndices = nullptr;
  swapChainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
  swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  swapChainCreateInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR; //TODO
  swapChainCreateInfo.clipped = VK_TRUE;
  swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

  result = vkCreateSwapchainKHR(device, &swapChainCreateInfo, nullptr, &swapchain);

  vkGetSwapchainImagesKHR(device, swapchain, &numberOfImagesInSwapchain, nullptr);
  VkImage* swapchainImages = new VkImage[numberOfImagesInSwapchain];
  result = vkGetSwapchainImagesKHR(device, swapchain, &numberOfImagesInSwapchain, swapchainImages);
  ASSERT_VULKAN(result);

  imageViews = new VkImageView[numberOfImagesInSwapchain];
  for (int i = 0; i < numberOfImagesInSwapchain; ++i) {
    VkImageViewCreateInfo imageViewCreateInfo;
    imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewCreateInfo.pNext = nullptr;
    imageViewCreateInfo.flags = 0;
    imageViewCreateInfo.image = swapchainImages[i];
    imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewCreateInfo.format = VK_FORMAT_B8G8R8A8_UNORM; //TODO civ
    imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    imageViewCreateInfo.subresourceRange.levelCount = 1;
    imageViewCreateInfo.subresourceRange.baseArrayLayer = 0; //Stereographische Texturen -> linkes und rechtes Auge unterschiedliche Texturen
    imageViewCreateInfo.subresourceRange.layerCount = 1;

    result = vkCreateImageView(device, &imageViewCreateInfo, nullptr, &imageViews[i]);
    ASSERT_VULKAN(result);
  }

  std::vector<char> shaderCodeVert = readFile("vert.spv");
  std::vector<char> shaderCodeFrag = readFile("frag.spv");
   
  createShaderModule(shaderCodeVert, &shaderModuleVert);
  createShaderModule(shaderCodeFrag, &shaderModuleFrag);

  delete[] swapchainImages;
  delete[] layerProperties;
  delete[] extensionProperties;
}

void gameLoop() {
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
  }
}

void shutdownVulkan() {
  vkDeviceWaitIdle(device);

  for (int i = 0; i < numberOfImagesInSwapchain; ++i) {
    vkDestroyImageView(device, imageViews[i], nullptr);
  }
  delete[] imageViews;
  vkDestroyShaderModule(device, shaderModuleFrag, nullptr);
  vkDestroyShaderModule(device, shaderModuleVert, nullptr);
  vkDestroySwapchainKHR(device, swapchain, nullptr);
  vkDestroyDevice(device, nullptr);
  vkDestroySurfaceKHR(instance, surface, nullptr);
  vkDestroyInstance(instance, nullptr);
}

void shutdownGlfw() {
  glfwDestroyWindow(window);
}

int main() {

  startGlfw();
  startVulkan();
  gameLoop();
  shutdownVulkan();
  shutdownGlfw();

}
