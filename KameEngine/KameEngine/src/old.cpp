// KameEngine.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <kmpch.h>

#include <iostream>
// #include "vulkan/vulkan.h"
#include <VulkanUtils.h>
#include "EasyImage.h"
#include <DepthImage.h>
#include <Vertex.h>
#include <Mesh.h>
#include <vector>
#include <fstream>
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
#include <Pipeline.h>
#include <MeshHelper.h>

#include <Kame/Engine.h>
#include <Kame/Application/FileSystem.h>
#include <Kame/Graphics/GraphicsApi/Vulkan/VulkanAPi.h>
#include <Kame/Application/Platform.h>
#include <Kame/Graphics/GraphicsApi/Vulkan/VulkanSurface.h>

VkInstance instance;
//std::vector<VkPhysicalDevice> physicalDevices;
VkSurfaceKHR surface;
VkDevice device;
VkSwapchainKHR swapchain = VK_NULL_HANDLE;
VkImageView* imageViews;
VkFramebuffer* framebuffers;
VkShaderModule shaderModuleVert;
VkShaderModule shaderModuleFrag;
Pipeline pipeline;
Pipeline pipelineWireframe;
VkRenderPass renderPass;
VkCommandPool commandPool;
VkCommandBuffer* commandBuffers;
VkSemaphore semaphoreImageAvailable;
VkSemaphore semaphoreRenderingDone;
VkQueue queue;
VkBuffer vertexBuffer;
VkBuffer indexBuffer;
VkDeviceMemory vertexBufferDeviceMemory;
VkDeviceMemory indexBufferDeviceMemory;
VkBuffer uniformBuffer;
VkDeviceMemory uniformBufferMemory;
uint32_t numberOfImagesInSwapchain = 0;
GLFWwindow* window;

uint32_t width = 400;
uint32_t height = 300;
//const VkFormat ourFormat = VK_FORMAT_B8G8R8A8_UNORM;
const VkFormat ourFormat = VK_FORMAT_B8G8R8A8_SRGB;

struct UniformBufferObject {
  glm::mat4 model;
  glm::mat4 view;
  glm::mat4 projection;
  glm::vec3 lightPosition;
};

UniformBufferObject ubo;

VkDescriptorSetLayout descriptorSetLayout;
VkDescriptorPool descriptorPool;
VkDescriptorSet descriptorSet;

EasyImage waterfallImage;
EasyImage wallTexture;
EasyImage wallNormalTexture;
DepthImage depthImage;
Mesh dragonMesh;


std::vector<Vertex> vertices = {
  /*Vertex({-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}),
  Vertex({ 0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}),
  Vertex({ 0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}),
  Vertex({-0.5f,  0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}),

  Vertex({-0.5f, -0.5f, -1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}),
  Vertex({ 0.5f, -0.5f, -1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}),
  Vertex({ 0.5f,  0.5f, -1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}),
  Vertex({-0.5f,  0.5f, -1.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f})*/
};

//std::vector<Vertex> vertices = {
//  Vertex({-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}),
//  Vertex({ 0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}),
//  Vertex({-0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}),
//  Vertex({ 0.5f, -0.5f}, {0.0f, 1.0f, 1.0f})
//};

std::vector<uint32_t> indices = {
  0, 1, 2, 2, 3, 0,
  4, 5, 6, 6, 7, 4
};

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

  std::cout << "Number of Surface Formats: " << numberOfFormats << std::endl;
  for (int i = 0; i < numberOfFormats; ++i) {
    std::cout << "Format: " << surfaceFormats[i].format << std::endl;
    std::cout << "ColorSpace: " << surfaceFormats[i].colorSpace << std::endl;
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

void recreateSwapchain();

void onWindowResized(GLFWwindow* window, int w, int h) {
  if (w == 0 || h == 0) return; // Do nothing

  VkSurfaceCapabilitiesKHR surfaceCapabilities;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(Kame::VulkanApi::Get()->GetInstance().GetBestPhysicalDevice().GetHandle(), surface, &surfaceCapabilities);

  if (w > surfaceCapabilities.maxImageExtent.width) w = surfaceCapabilities.maxImageExtent.width;
  if (h > surfaceCapabilities.maxImageExtent.height) h = surfaceCapabilities.maxImageExtent.height;

  width = w;
  height = h;
  recreateSwapchain();
}

void startGlfw() {

  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  //glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  window = glfwCreateWindow(width, height, "Kame Engine", nullptr, nullptr);
  glfwSetWindowSizeCallback(window, onWindowResized);
}

void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule) {
  VkShaderModuleCreateInfo shaderCreateInfo;
  shaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  shaderCreateInfo.pNext = nullptr;
  shaderCreateInfo.flags = 0;
  shaderCreateInfo.codeSize = code.size();
  shaderCreateInfo.pCode = (uint32_t*)code.data();

  VkResult result = vkCreateShaderModule(device, &shaderCreateInfo, nullptr, shaderModule);
  ASSERT_VULKAN(result);
}

void createInstance() {

  instance = Kame::VulkanApi::Get()->GetVkInstance();
  return;

  VkApplicationInfo appInfo;
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pNext = nullptr; // Erweiterungen
  //appInfo.pApplicationName = "Kame Sandbox";
  appInfo.pApplicationName = Kame::Engine::GetGameName();
  appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 0);
  appInfo.pEngineName = "Kame Engine";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_2;

  const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
  };

  uint32_t numberOfGlfwExtensions = 0;
  auto glfwExtensions = glfwGetRequiredInstanceExtensions(&numberOfGlfwExtensions);

  VkInstanceCreateInfo instanceInfo;
  instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instanceInfo.pNext = nullptr; // Erweiterungen
  instanceInfo.flags = 0; // noch keine Verwendung
  instanceInfo.pApplicationInfo = &appInfo;
  instanceInfo.enabledLayerCount = validationLayers.size();
  instanceInfo.ppEnabledLayerNames = validationLayers.data();
  instanceInfo.enabledExtensionCount = numberOfGlfwExtensions;
  instanceInfo.ppEnabledExtensionNames = glfwExtensions;

  VkResult result = volkInitialize();
  ASSERT_VULKAN(result);

  result = vkCreateInstance(&instanceInfo, NULL, &instance);
  ASSERT_VULKAN(result);

  volkLoadInstance(instance);
}

void printInstanceLayers() {
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

  delete[] layerProperties;
}

void printInstanceExtensions() {
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

  delete[] extensionProperties;
}

void createGlfwWindowSurface() {

  //Kame::VulkanSurface* vulkanSurface = (Kame::VulkanSurface *) Kame::Platform::GetMainWindow()->GetSurface().get();
  //surface = vulkanSurface->GetHandle();
  //return;
  VkResult result = glfwCreateWindowSurface(instance, window, nullptr, &surface);
  ASSERT_VULKAN(result);
}

//std::vector<VkPhysicalDevice> getAllPhysicalDevices() {
//  uint32_t physicalDeviceCount;
//  VkResult result = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);
//  ASSERT_VULKAN(result);
//
//  std::cout << "Number of physical devices: " << physicalDeviceCount << std::endl;
//
//  std::vector<VkPhysicalDevice> physicalDevices;
//  physicalDevices.resize(physicalDeviceCount);
//
//  result = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data());
//  ASSERT_VULKAN(result);
//
//  return physicalDevices;
//}

void printStatsOfAllPhysicalDevices() {

  //for (int i = 0; i < physicalDevices.size(); ++i) {
  //  printStats(physicalDevices[i]);
  //}
}

void createLogicalDevice() {

  device = Kame::VulkanApi::Get()->GetVkDevice();
  return;
  float queuePrios[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  VkDeviceQueueCreateInfo deviceQueueCreateInfo;
  deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  deviceQueueCreateInfo.pNext = nullptr;
  deviceQueueCreateInfo.flags = 0;
  deviceQueueCreateInfo.queueFamilyIndex = 0; //TODO eigentlich beste aussuchen
  deviceQueueCreateInfo.queueCount = 1; //TODO prüfen, ob 4 gehen
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
  VkResult result = vkCreateDevice(Kame::VulkanApi::Get()->GetInstance().GetBestPhysicalDevice().GetHandle(), &deviceCreateInfo, nullptr, &device);
  ASSERT_VULKAN(result);
}

void createQueue() {
  vkGetDeviceQueue(device, 0, 0, &queue);
}

void checkSurfaceSupport() {
  VkBool32 surfaceSupport = false;
  VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(Kame::VulkanApi::Get()->GetInstance().GetBestPhysicalDevice().GetHandle(), 0, surface, &surfaceSupport);
  ASSERT_VULKAN(result);

  if (!surfaceSupport) {
    std::cerr << "Surface nut supported!" << std::endl;
    __debugbreak;
  }
}

void createSwapchain() {
  VkSwapchainCreateInfoKHR swapChainCreateInfo;
  swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swapChainCreateInfo.pNext = nullptr;;
  swapChainCreateInfo.flags = 0;
  swapChainCreateInfo.surface = surface;
  swapChainCreateInfo.minImageCount = 3; //TODO check if valid
  swapChainCreateInfo.imageFormat = ourFormat; //TODO check if valid
  swapChainCreateInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR; //TODO check if valid
  swapChainCreateInfo.imageExtent = VkExtent2D{ width, height }; //TODO
  swapChainCreateInfo.imageArrayLayers = 1;
  swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE; // TODO civ
  swapChainCreateInfo.queueFamilyIndexCount = 0;
  swapChainCreateInfo.pQueueFamilyIndices = nullptr;
  swapChainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
  swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  swapChainCreateInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR; //TODO
  swapChainCreateInfo.clipped = VK_TRUE;
  swapChainCreateInfo.oldSwapchain = swapchain;

  VkResult result = vkCreateSwapchainKHR(device, &swapChainCreateInfo, nullptr, &swapchain);
  ASSERT_VULKAN(result);
}

void createImageViews() {
  vkGetSwapchainImagesKHR(device, swapchain, &numberOfImagesInSwapchain, nullptr);
  VkImage* swapchainImages = new VkImage[numberOfImagesInSwapchain];
  VkResult result = vkGetSwapchainImagesKHR(device, swapchain, &numberOfImagesInSwapchain, swapchainImages);
  ASSERT_VULKAN(result);

  imageViews = new VkImageView[numberOfImagesInSwapchain];
  for (int i = 0; i < numberOfImagesInSwapchain; ++i) {
    createImageView(device, swapchainImages[i], ourFormat, VK_IMAGE_ASPECT_COLOR_BIT, imageViews[i]);
  }

  delete[] swapchainImages;
}

void createRenderPass() {
  VkAttachmentDescription attachmentDescription;
  attachmentDescription.flags = 0;
  attachmentDescription.format = ourFormat;
  attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
  attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference attachmentReference;
  attachmentReference.attachment = 0;
  attachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentDescription depthAttachment = DepthImage::getDepthAttachment(Kame::VulkanApi::Get()->GetInstance().GetBestPhysicalDevice().GetHandle());

  VkAttachmentReference depthAttachmentReference;
  depthAttachmentReference.attachment = 1;
  depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpassDescription;
  subpassDescription.flags = 0;
  subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpassDescription.inputAttachmentCount = 0;
  subpassDescription.pInputAttachments = nullptr;
  subpassDescription.colorAttachmentCount = 1;
  subpassDescription.pColorAttachments = &attachmentReference;
  subpassDescription.pResolveAttachments = nullptr;
  subpassDescription.pDepthStencilAttachment = &depthAttachmentReference;
  subpassDescription.preserveAttachmentCount = 0;
  subpassDescription.pPreserveAttachments = nullptr;

  VkSubpassDependency subpassDependency;
  subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  subpassDependency.dstSubpass = 0;
  subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  subpassDependency.srcAccessMask = 0;
  subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  subpassDependency.dependencyFlags = 0;

  std::vector<VkAttachmentDescription> attachments;
  attachments.push_back(attachmentDescription);
  attachments.push_back(depthAttachment);

  VkRenderPassCreateInfo renderPassCreateInfo;
  renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassCreateInfo.pNext = nullptr;
  renderPassCreateInfo.flags = 0;
  renderPassCreateInfo.attachmentCount = attachments.size();
  renderPassCreateInfo.pAttachments = attachments.data();
  renderPassCreateInfo.subpassCount = 1;
  renderPassCreateInfo.pSubpasses = &subpassDescription;
  renderPassCreateInfo.dependencyCount = 1;
  renderPassCreateInfo.pDependencies = &subpassDependency;

  VkResult result = vkCreateRenderPass(device, &renderPassCreateInfo, nullptr, &renderPass);
  ASSERT_VULKAN(result);
}

void createDescriptorSetLayout() {
  VkDescriptorSetLayoutBinding descriptorSetLayoutBinding;
  descriptorSetLayoutBinding.binding = 0;
  descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  descriptorSetLayoutBinding.descriptorCount = 1;
  descriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  descriptorSetLayoutBinding.pImmutableSamplers = nullptr;

  VkDescriptorSetLayoutBinding samplerDescriptorSetLayoutBinding;
  samplerDescriptorSetLayoutBinding.binding = 1;
  samplerDescriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  samplerDescriptorSetLayoutBinding.descriptorCount = 1;
  samplerDescriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
  samplerDescriptorSetLayoutBinding.pImmutableSamplers = nullptr;

  VkDescriptorSetLayoutBinding samplerNormalDescriptorSetLayoutBinding;
  samplerNormalDescriptorSetLayoutBinding.binding = 2;
  samplerNormalDescriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  samplerNormalDescriptorSetLayoutBinding.descriptorCount = 1;
  samplerNormalDescriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
  samplerNormalDescriptorSetLayoutBinding.pImmutableSamplers = nullptr;

  std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;
  descriptorSetLayoutBindings.push_back(descriptorSetLayoutBinding);
  descriptorSetLayoutBindings.push_back(samplerDescriptorSetLayoutBinding);
  descriptorSetLayoutBindings.push_back(samplerNormalDescriptorSetLayoutBinding);

  VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo;
  descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  descriptorSetLayoutCreateInfo.pNext = nullptr;
  descriptorSetLayoutCreateInfo.flags = 0;
  descriptorSetLayoutCreateInfo.bindingCount = descriptorSetLayoutBindings.size();
  descriptorSetLayoutCreateInfo.pBindings = descriptorSetLayoutBindings.data();

  VkResult result = vkCreateDescriptorSetLayout(device, &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout);
  ASSERT_VULKAN(result);
}

void createPipeline() {
  std::vector<char> shaderCodeVert = readFile(Kame::FileSystem::Path::GetPath(Kame::FileSystem::Path::Type::VulkanShaders, "shader.vert.spv").c_str());
  std::vector<char> shaderCodeFrag = readFile(Kame::FileSystem::Path::GetPath(Kame::FileSystem::Path::Type::VulkanShaders, "shader.frag.spv").c_str());

  createShaderModule(shaderCodeVert, &shaderModuleVert);
  createShaderModule(shaderCodeFrag, &shaderModuleFrag);

  pipeline.ini(shaderModuleVert, shaderModuleFrag, width, height);
  pipeline.create(device, renderPass, descriptorSetLayout);

  pipelineWireframe.ini(shaderModuleVert, shaderModuleFrag, width, height);
  pipelineWireframe.setPolygonMode(VK_POLYGON_MODE_LINE);
  pipelineWireframe.create(device, renderPass, descriptorSetLayout);

}

void createFramebuffers() {
  framebuffers = new VkFramebuffer[numberOfImagesInSwapchain];
  for (size_t i = 0; i < numberOfImagesInSwapchain; ++i) {

    std::vector<VkImageView> attachmentViews;
    attachmentViews.push_back(imageViews[i]);
    attachmentViews.push_back(depthImage.getImageView());

    VkFramebufferCreateInfo framebufferCreateInfo;
    framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferCreateInfo.pNext = nullptr;
    framebufferCreateInfo.flags = 0;
    framebufferCreateInfo.renderPass = renderPass;
    framebufferCreateInfo.attachmentCount = attachmentViews.size();
    framebufferCreateInfo.pAttachments = attachmentViews.data();
    framebufferCreateInfo.width = width;
    framebufferCreateInfo.height = height;
    framebufferCreateInfo.layers = 1;

    VkResult result = vkCreateFramebuffer(device, &framebufferCreateInfo, nullptr, &framebuffers[i]);
    ASSERT_VULKAN(result);
  }
}

void createCommandPool() {
  VkCommandPoolCreateInfo commandPoolCreateInfo;
  commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  commandPoolCreateInfo.pNext = nullptr;
  commandPoolCreateInfo.flags = 0;
  commandPoolCreateInfo.queueFamilyIndex = 0; //TODO civ

  VkResult result = vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &commandPool);
  ASSERT_VULKAN(result);
}

void createDepthImage() {
  depthImage.create(device, Kame::VulkanApi::Get()->GetInstance().GetBestPhysicalDevice().GetHandle(), commandPool, queue, width, height);
}

void createCommandBuffers() {
  VkCommandBufferAllocateInfo commandBufferAllocateInfo;
  commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  commandBufferAllocateInfo.pNext = nullptr;
  commandBufferAllocateInfo.commandPool = commandPool;
  commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  //commandBufferAllocateInfo.commandBufferCount = numberOfImagesInSwapchain;
  commandBufferAllocateInfo.commandBufferCount = 3; // TODO : wo kommt die 3 her?

  //commandBuffers = new VkCommandBuffer[numberOfImagesInSwapchain];
  commandBuffers = new VkCommandBuffer[3];
  VkResult result = vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, commandBuffers);
  ASSERT_VULKAN(result);
}



void loadTexture() {
  //waterfallImage.load("D:/Raftek/Kame2/KameEngine/KameEngine/Resources/Waterfalls.png");
  waterfallImage.load(
    Kame::FileSystem::Path::GetPath(Kame::FileSystem::Path::Type::Assets, "Waterfalls.png").c_str()
  );
  std::cout << waterfallImage.getWidth() << std::endl;
  std::cout << waterfallImage.getHeight() << std::endl;
  std::cout << waterfallImage.getSizeInBytes() << std::endl;

  waterfallImage.upload(device, Kame::VulkanApi::Get()->GetInstance().GetBestPhysicalDevice().GetHandle(), commandPool, queue);

  wallTexture.load(Kame::FileSystem::Path::GetPath(Kame::FileSystem::Path::Type::Assets, "154.JPG").c_str());
  wallTexture.upload(device, Kame::VulkanApi::Get()->GetInstance().GetBestPhysicalDevice().GetHandle(), commandPool, queue);

  wallNormalTexture.load(Kame::FileSystem::Path::GetPath(Kame::FileSystem::Path::Type::Assets, "154_norm.JPG").c_str());
  wallNormalTexture.upload(device, Kame::VulkanApi::Get()->GetInstance().GetBestPhysicalDevice().GetHandle(), commandPool, queue);

}

void loadMesh() {
  dragonMesh.create(Kame::FileSystem::Path::GetPath(Kame::FileSystem::Path::Type::Assets, "dragon.obj").c_str());
  vertices = dragonMesh.getVertices();
  indices = dragonMesh.getIndices();

  //vertices = getQuadVertices();
  //indices = getQuadIndices();
}

void createVertexBuffer() {
  createAndUploadBuffer(device, Kame::VulkanApi::Get()->GetInstance().GetBestPhysicalDevice().GetHandle(), queue, commandPool, vertices, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, vertexBuffer, vertexBufferDeviceMemory);
}

void createIndexBuffer() {
  createAndUploadBuffer(device, Kame::VulkanApi::Get()->GetInstance().GetBestPhysicalDevice().GetHandle(), queue, commandPool, indices, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, indexBuffer, indexBufferDeviceMemory);
}

void createUniformBuffer() {
  VkDeviceSize bufferSize = sizeof(ubo);
  createBuffer(
    device,
    Kame::VulkanApi::Get()->GetInstance().GetBestPhysicalDevice().GetHandle(),
    bufferSize,
    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
    uniformBuffer,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    uniformBufferMemory
  );
}

void createDescriptorPool() {
  VkDescriptorPoolSize descriptorPoolSize;
  descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  descriptorPoolSize.descriptorCount = 1;

  VkDescriptorPoolSize samplerPoolSize;
  samplerPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  samplerPoolSize.descriptorCount = 1;

  std::vector<VkDescriptorPoolSize> descriptorPoolSizes;
  descriptorPoolSizes.push_back(descriptorPoolSize);
  descriptorPoolSizes.push_back(samplerPoolSize);

  VkDescriptorPoolCreateInfo descriptorPoolCreateInfo;
  descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  descriptorPoolCreateInfo.pNext = nullptr;
  descriptorPoolCreateInfo.flags = 0;
  descriptorPoolCreateInfo.maxSets = 1;
  descriptorPoolCreateInfo.poolSizeCount = descriptorPoolSizes.size();
  descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSizes.data();

  VkResult result = vkCreateDescriptorPool(device, &descriptorPoolCreateInfo, nullptr, &descriptorPool);
  ASSERT_VULKAN(result);
}

void createDescriptorSet() {
  VkDescriptorSetAllocateInfo descriptorSetAllocateInfo;
  descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  descriptorSetAllocateInfo.pNext = nullptr;
  descriptorSetAllocateInfo.descriptorPool = descriptorPool;
  descriptorSetAllocateInfo.descriptorSetCount = 1;
  descriptorSetAllocateInfo.pSetLayouts = &descriptorSetLayout;

  VkResult result = vkAllocateDescriptorSets(device, &descriptorSetAllocateInfo, &descriptorSet);
  ASSERT_VULKAN(result);

  VkDescriptorBufferInfo descriptorBufferInfo;
  descriptorBufferInfo.buffer = uniformBuffer;
  descriptorBufferInfo.offset = 0;
  descriptorBufferInfo.range = sizeof(ubo);

  VkWriteDescriptorSet descriptorWrite;
  descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  descriptorWrite.pNext = nullptr;
  descriptorWrite.dstSet = descriptorSet;
  descriptorWrite.dstBinding = 0;
  descriptorWrite.dstArrayElement = 0;
  descriptorWrite.descriptorCount = 1;
  descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  descriptorWrite.pImageInfo = nullptr;
  descriptorWrite.pBufferInfo = &descriptorBufferInfo;
  descriptorWrite.pTexelBufferView = nullptr;

  VkDescriptorImageInfo descriptorImageInfo;
  descriptorImageInfo.sampler = wallTexture.getSampler();
  descriptorImageInfo.imageView = wallTexture.getImageView();
  descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

  VkDescriptorImageInfo descriptorImageInfoNormal;
  descriptorImageInfoNormal.sampler = wallNormalTexture.getSampler();
  descriptorImageInfoNormal.imageView = wallNormalTexture.getImageView();
  descriptorImageInfoNormal.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

  std::vector<VkDescriptorImageInfo> descriptorImageInfos = { descriptorImageInfo, descriptorImageInfoNormal };

  VkWriteDescriptorSet descriptorSampler;
  descriptorSampler.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  descriptorSampler.pNext = nullptr;
  descriptorSampler.dstSet = descriptorSet;
  descriptorSampler.dstBinding = 1;
  descriptorSampler.dstArrayElement = 0;
  descriptorSampler.descriptorCount = descriptorImageInfos.size();
  descriptorSampler.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  descriptorSampler.pImageInfo = descriptorImageInfos.data();
  descriptorSampler.pBufferInfo = nullptr;
  descriptorSampler.pTexelBufferView = nullptr;

  std::vector<VkWriteDescriptorSet> writeDescriptorSets;
  writeDescriptorSets.push_back(descriptorWrite);
  writeDescriptorSets.push_back(descriptorSampler);

  vkUpdateDescriptorSets(device, writeDescriptorSets.size(), writeDescriptorSets.data(), 0, nullptr);
}

void recordCommandBuffers() {
  VkCommandBufferBeginInfo commandBufferBeginInfo;
  commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  commandBufferBeginInfo.pNext = nullptr;
  commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
  commandBufferBeginInfo.pInheritanceInfo = nullptr;

  for (size_t i = 0; i < numberOfImagesInSwapchain; i++) {
    VkResult result = vkBeginCommandBuffer(commandBuffers[i], &commandBufferBeginInfo);
    ASSERT_VULKAN(result);

    VkRenderPassBeginInfo renderPassBeginInfo;
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.pNext = nullptr;
    renderPassBeginInfo.renderPass = renderPass;
    renderPassBeginInfo.framebuffer = framebuffers[i];
    renderPassBeginInfo.renderArea.offset = { 0, 0 };
    renderPassBeginInfo.renderArea.extent = { width, height };

    VkClearValue clearValue = { 0.2f, 0.2f, 0.2f, 1.0f };
    VkClearValue depthClearValue = { 1.0f, 0 };
    std::vector<VkClearValue> clearValues;
    clearValues.push_back(clearValue);
    clearValues.push_back(depthClearValue);
    renderPassBeginInfo.clearValueCount = clearValues.size();
    renderPassBeginInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkBool32 usePhong = VK_TRUE;
    vkCmdPushConstants(commandBuffers[i], pipeline.getVkPipelineLayout(), VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(usePhong), &usePhong);

    vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.getVkPipeline());

    VkViewport viewport;
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = width / 2;
    viewport.height = height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffers[i], 0, 1, &viewport);

    VkRect2D scissor;
    scissor.offset = { 0,0 };
    scissor.extent = { width, height };
    vkCmdSetScissor(commandBuffers[i], 0, 1, &scissor);

    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, &vertexBuffer, offsets);
    vkCmdBindIndexBuffer(commandBuffers[i], indexBuffer, 0, VK_INDEX_TYPE_UINT32);

    vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.getVkPipelineLayout(), 0, 1, &descriptorSet, 0, nullptr);

    vkCmdDrawIndexed(commandBuffers[i], indices.size(), 1, 0, 0, 0);



    vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineWireframe.getVkPipeline());

    viewport.x = width / 2;
    vkCmdSetViewport(commandBuffers[i], 0, 1, &viewport);

    usePhong = VK_FALSE;
    vkCmdPushConstants(commandBuffers[i], pipelineWireframe.getVkPipelineLayout(), VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(usePhong), &usePhong);

    vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, &vertexBuffer, offsets);
    vkCmdBindIndexBuffer(commandBuffers[i], indexBuffer, 0, VK_INDEX_TYPE_UINT32);

    vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineWireframe.getVkPipelineLayout(), 0, 1, &descriptorSet, 0, nullptr);

    vkCmdDrawIndexed(commandBuffers[i], indices.size(), 1, 0, 0, 0);

    vkCmdEndRenderPass(commandBuffers[i]);

    result = vkEndCommandBuffer(commandBuffers[i]);
    ASSERT_VULKAN(result);
  }
}

void createSemaphores() {
  VkSemaphoreCreateInfo semaphoreCreateInfo;
  semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  semaphoreCreateInfo.pNext = nullptr;
  semaphoreCreateInfo.flags = 0;

  VkResult result = vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &semaphoreImageAvailable);
  ASSERT_VULKAN(result);
  result = vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &semaphoreRenderingDone);
  ASSERT_VULKAN(result);
}

void startVulkan() {

  createInstance(); //Done
  //
  //physicalDevices = getAllPhysicalDevices();
  printInstanceLayers();
  printInstanceExtensions();

  createLogicalDevice();
  createQueue();


  createRenderPass();
  createDescriptorSetLayout();
  createPipeline();
  createCommandPool();
  createDepthImage();

  createCommandBuffers();
  loadTexture();
  loadMesh();
  createVertexBuffer();
  createIndexBuffer();
  createUniformBuffer();
  createDescriptorPool();
  createDescriptorSet();


  createGlfwWindowSurface();
  checkSurfaceSupport();

  createSwapchain();
  createImageViews();
  createFramebuffers();

  printStatsOfAllPhysicalDevices();


  recordCommandBuffers();
  createSemaphores();
}

void recreateSwapchain() {
  vkDeviceWaitIdle(device);

  depthImage.destroy();

  vkFreeCommandBuffers(device, commandPool, numberOfImagesInSwapchain, commandBuffers);
  delete[] commandBuffers;

  for (size_t i = 0; i < numberOfImagesInSwapchain; ++i) {
    vkDestroyFramebuffer(device, framebuffers[i], nullptr);
  }
  delete[] framebuffers;

  vkDestroyRenderPass(device, renderPass, nullptr);

  for (int i = 0; i < numberOfImagesInSwapchain; ++i) {
    vkDestroyImageView(device, imageViews[i], nullptr);
  }
  delete[] imageViews;

  VkSwapchainKHR oldSwapchain = swapchain;

  createSwapchain();
  createImageViews();
  createRenderPass();
  createDepthImage();
  createFramebuffers();
  createCommandBuffers();
  recordCommandBuffers();

  vkDestroySwapchainKHR(device, oldSwapchain, nullptr);

}

void drawFrame() {
  uint32_t imageIndex;
  vkAcquireNextImageKHR(
    device, swapchain, std::numeric_limits<uint64_t>::max(), semaphoreImageAvailable, VK_NULL_HANDLE, &imageIndex
  );

  VkSubmitInfo submitInfo;
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.pNext = nullptr;
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = &semaphoreImageAvailable;
  VkPipelineStageFlags waitStageMask[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
  submitInfo.pWaitDstStageMask = waitStageMask;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffers[imageIndex];
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = &semaphoreRenderingDone;

  VkResult result = vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
  ASSERT_VULKAN(result);

  VkPresentInfoKHR presentInfo;
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.pNext = nullptr;
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = &semaphoreRenderingDone;
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = &swapchain;
  presentInfo.pImageIndices = &imageIndex;
  presentInfo.pResults = nullptr;

  result = vkQueuePresentKHR(queue, &presentInfo);
  ASSERT_VULKAN(result);
}

void updateMVP() {

  static auto gameStartTime = std::chrono::high_resolution_clock::now();
  auto frameTime = std::chrono::high_resolution_clock::now();

  float timeSinceStart = std::chrono::duration_cast<std::chrono::milliseconds>(frameTime - gameStartTime).count() / 1000.0f;

  glm::vec3 offset = glm::vec3(timeSinceStart * 1.0f, 0.0f, 0.0f);

  glm::mat4 model = glm::mat4(1);
  model = glm::translate(model, glm::vec3(0, 0, -0.2));
  model = glm::translate(model, offset);
  model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
  model = glm::rotate(model, timeSinceStart * glm::radians(30.0f), glm::vec3(0.0f, 0.0f, 1.0f));

  glm::mat4 view = glm::lookAt(
    glm::vec3(1.0f, 1.0f, 1.0f) + offset,
    glm::vec3(0.0f, 0.0f, 0.0f) + offset,
    glm::vec3(0.0f, 0.0f, 1.0f)
  );
  glm::mat4 projection = glm::perspective(glm::radians(60.0f), ((float)width * 0.5f) / (float)height, 0.01f, 10.0f);
  projection[1][1] *= -1;

  float rotationSpeed = 0.0f;
  ubo.lightPosition = glm::vec4(offset, 0.0f) + glm::rotate(
    glm::mat4(1),
    timeSinceStart * glm::radians(rotationSpeed),
    glm::vec3(0.0f, 0.0f, 1.0f)
  ) * glm::vec4(0, 3, 1, 0);

  ubo.model = model;
  ubo.view = view;
  ubo.projection = projection;

  void* data;
  vkMapMemory(device, uniformBufferMemory, 0, sizeof(ubo), 0, &data);
  memcpy(data, &ubo, sizeof(ubo));
  vkUnmapMemory(device, uniformBufferMemory);

}

void gameLoop() {
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    updateMVP();

    drawFrame();
  }
}

void shutdownVulkan() {
  vkDeviceWaitIdle(device);

  depthImage.destroy();

  vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
  vkDestroyDescriptorPool(device, descriptorPool, nullptr);
  vkFreeMemory(device, uniformBufferMemory, nullptr);
  vkDestroyBuffer(device, uniformBuffer, nullptr);

  vkFreeMemory(device, indexBufferDeviceMemory, nullptr);
  vkDestroyBuffer(device, indexBuffer, nullptr);

  vkFreeMemory(device, vertexBufferDeviceMemory, nullptr);
  vkDestroyBuffer(device, vertexBuffer, nullptr);

  waterfallImage.destroy();
  wallTexture.destroy();
  wallNormalTexture.destroy();

  vkDestroySemaphore(device, semaphoreImageAvailable, nullptr);
  vkDestroySemaphore(device, semaphoreRenderingDone, nullptr);

  vkFreeCommandBuffers(device, commandPool, numberOfImagesInSwapchain, commandBuffers);
  delete[] commandBuffers;

  vkDestroyCommandPool(device, commandPool, nullptr);

  for (size_t i = 0; i < numberOfImagesInSwapchain; ++i) {
    vkDestroyFramebuffer(device, framebuffers[i], nullptr);
  }
  delete[] framebuffers;

  pipeline.destroy();
  pipelineWireframe.destroy();

  vkDestroyRenderPass(device, renderPass, nullptr);

  for (int i = 0; i < numberOfImagesInSwapchain; ++i) {
    vkDestroyImageView(device, imageViews[i], nullptr);
  }
  delete[] imageViews;

  vkDestroyShaderModule(device, shaderModuleFrag, nullptr);
  vkDestroyShaderModule(device, shaderModuleVert, nullptr);
  vkDestroySwapchainKHR(device, swapchain, nullptr);
  //vkDestroyDevice(device, nullptr);
  vkDestroySurfaceKHR(instance, surface, nullptr);
  //vkDestroyInstance(instance, nullptr);
}

void shutdownGlfw() {
  glfwDestroyWindow(window);
  glfwTerminate();
}


