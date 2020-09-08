// KameEngine.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <kmpch.h>

#include <iostream>
// #include "vulkan/vulkan.h"
#define GLFW_INCLUDE_VULKAN
#include <glfw/glfw3.h>
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
VkFramebuffer* framebuffers;
VkShaderModule shaderModuleVert;
VkShaderModule shaderModuleFrag;
VkPipelineLayout pipelineLayout;
VkRenderPass renderPass;
VkPipeline pipeline;
VkCommandPool commandPool;
VkCommandBuffer* commandBuffers;
VkSemaphore semaphoreImageAvailable;
VkSemaphore semaphoreRenderingDone;
VkQueue queue;
uint32_t numberOfImagesInSwapchain = 0;
GLFWwindow* window;

const uint32_t WIDTH = 400;
const uint32_t HEIGHT = 300;
const VkFormat ourFormat = VK_FORMAT_B8G8R8A8_UNORM;

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
  swapChainCreateInfo.imageFormat = ourFormat; //TODO check if valid
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
    imageViewCreateInfo.format = ourFormat; //TODO civ
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

  std::vector<char> shaderCodeVert = readFile("D:/Raftek/Kame2/KameEngine/KameEngine/vert.spv");
  std::vector<char> shaderCodeFrag = readFile("D:/Raftek/Kame2/KameEngine/KameEngine/frag.spv");

  createShaderModule(shaderCodeVert, &shaderModuleVert);
  createShaderModule(shaderCodeFrag, &shaderModuleFrag);

  VkPipelineShaderStageCreateInfo shaderStageCreateInfoVert;
  shaderStageCreateInfoVert.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  shaderStageCreateInfoVert.pNext = nullptr;
  shaderStageCreateInfoVert.flags = 0;
  shaderStageCreateInfoVert.stage = VK_SHADER_STAGE_VERTEX_BIT;
  shaderStageCreateInfoVert.module = shaderModuleVert;
  shaderStageCreateInfoVert.pName = "main"; //Könnte eine andere Funktion sein, daher könnte es mehrere PipielineShaderStages für ein ShaderModule geben
  shaderStageCreateInfoVert.pSpecializationInfo = nullptr;

  VkPipelineShaderStageCreateInfo shaderStageCreateInfoFrag;
  shaderStageCreateInfoFrag.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  shaderStageCreateInfoFrag.pNext = nullptr;
  shaderStageCreateInfoFrag.flags = 0;
  shaderStageCreateInfoFrag.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  shaderStageCreateInfoFrag.module = shaderModuleFrag;
  shaderStageCreateInfoFrag.pName = "main"; //Könnte eine andere Funktion sein, daher könnte es mehrere PipielineShaderStages für ein ShaderModule geben
  shaderStageCreateInfoFrag.pSpecializationInfo = nullptr;

  VkPipelineShaderStageCreateInfo shaderStages[] = { shaderStageCreateInfoVert, shaderStageCreateInfoFrag };

  VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo;
  vertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputCreateInfo.pNext = nullptr;
  vertexInputCreateInfo.flags = 0;
  vertexInputCreateInfo.vertexBindingDescriptionCount = 0;
  vertexInputCreateInfo.pVertexBindingDescriptions = 0;
  vertexInputCreateInfo.vertexAttributeDescriptionCount = 0;
  vertexInputCreateInfo.pVertexAttributeDescriptions = 0;

  VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo;
  inputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssemblyCreateInfo.pNext = nullptr;
  inputAssemblyCreateInfo.flags = 0;
  inputAssemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;

  VkViewport viewport;
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = WIDTH;
  viewport.height = HEIGHT;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  VkRect2D scissor;
  scissor.offset = { 0,0 };
  scissor.extent = { WIDTH, HEIGHT };

  VkPipelineViewportStateCreateInfo viewportStateCreateInfo;
  viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportStateCreateInfo.pNext = nullptr;
  viewportStateCreateInfo.flags = 0;
  viewportStateCreateInfo.viewportCount = 1;
  viewportStateCreateInfo.pViewports = &viewport;
  viewportStateCreateInfo.scissorCount = 1;
  viewportStateCreateInfo.pScissors = &scissor;

  VkPipelineRasterizationStateCreateInfo rasterizationCreateInfo;
  rasterizationCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizationCreateInfo.pNext = nullptr;
  rasterizationCreateInfo.flags = 0;
  rasterizationCreateInfo.depthClampEnable = VK_FALSE;
  rasterizationCreateInfo.rasterizerDiscardEnable = VK_FALSE;
  rasterizationCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizationCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizationCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
  rasterizationCreateInfo.depthBiasEnable = VK_FALSE;
  rasterizationCreateInfo.depthBiasConstantFactor = 0.0f;
  rasterizationCreateInfo.depthBiasClamp = 0.0f;
  rasterizationCreateInfo.depthBiasSlopeFactor = 0.0f;
  rasterizationCreateInfo.lineWidth = 1.0f;

  VkPipelineMultisampleStateCreateInfo multisampleCreateInfo;
  multisampleCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampleCreateInfo.pNext = nullptr;
  multisampleCreateInfo.flags = 0;
  multisampleCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  multisampleCreateInfo.sampleShadingEnable = VK_FALSE;
  multisampleCreateInfo.minSampleShading = 1.0f;
  multisampleCreateInfo.pSampleMask = nullptr;
  multisampleCreateInfo.alphaToCoverageEnable = VK_FALSE;
  multisampleCreateInfo.alphaToOneEnable = VK_FALSE;

  VkPipelineColorBlendAttachmentState colorBlendAttachmentState;
  colorBlendAttachmentState.blendEnable = VK_TRUE;
  colorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
  colorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
  colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
  colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
  colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

  VkPipelineColorBlendStateCreateInfo colorBlendCreateInfo;
  colorBlendCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlendCreateInfo.pNext = nullptr;
  colorBlendCreateInfo.flags = 0;
  colorBlendCreateInfo.logicOpEnable = VK_FALSE;
  colorBlendCreateInfo.logicOp = VK_LOGIC_OP_NO_OP;
  colorBlendCreateInfo.attachmentCount = 1;
  colorBlendCreateInfo.pAttachments = &colorBlendAttachmentState;
  colorBlendCreateInfo.blendConstants[0] = 0.0f;
  colorBlendCreateInfo.blendConstants[1] = 0.0f;
  colorBlendCreateInfo.blendConstants[2] = 0.0f;
  colorBlendCreateInfo.blendConstants[3] = 0.0f;

  VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo;
  pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutCreateInfo.pNext = nullptr;
  pipelineLayoutCreateInfo.flags = 0;
  pipelineLayoutCreateInfo.setLayoutCount = 0;
  pipelineLayoutCreateInfo.pSetLayouts = nullptr;
  pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
  pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

  result = vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout);
  ASSERT_VULKAN(result);

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

  VkSubpassDescription subpassDescription;
  subpassDescription.flags = 0;
  subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpassDescription.inputAttachmentCount = 0;
  subpassDescription.pInputAttachments = nullptr;
  subpassDescription.colorAttachmentCount = 1;
  subpassDescription.pColorAttachments = &attachmentReference;
  subpassDescription.pResolveAttachments = nullptr;
  subpassDescription.pDepthStencilAttachment = nullptr;
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

  VkRenderPassCreateInfo renderPassCreateInfo;
  renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassCreateInfo.pNext = nullptr;
  renderPassCreateInfo.flags = 0;
  renderPassCreateInfo.attachmentCount = 1;
  renderPassCreateInfo.pAttachments = &attachmentDescription;
  renderPassCreateInfo.subpassCount = 1;
  renderPassCreateInfo.pSubpasses = &subpassDescription;
  renderPassCreateInfo.dependencyCount = 1;
  renderPassCreateInfo.pDependencies = &subpassDependency;

  result = vkCreateRenderPass(device, &renderPassCreateInfo, nullptr, &renderPass);
  ASSERT_VULKAN(result);

  VkGraphicsPipelineCreateInfo pipelineCreateInfo;
  pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineCreateInfo.pNext = nullptr;
  pipelineCreateInfo.flags = 0;
  pipelineCreateInfo.stageCount = 2;
  pipelineCreateInfo.pStages = shaderStages;
  pipelineCreateInfo.pVertexInputState = &vertexInputCreateInfo;
  pipelineCreateInfo.pInputAssemblyState = &inputAssemblyCreateInfo;
  pipelineCreateInfo.pTessellationState = nullptr;
  pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
  pipelineCreateInfo.pRasterizationState = &rasterizationCreateInfo;
  pipelineCreateInfo.pMultisampleState = &multisampleCreateInfo;
  pipelineCreateInfo.pDepthStencilState = nullptr;
  pipelineCreateInfo.pColorBlendState = &colorBlendCreateInfo;
  pipelineCreateInfo.pDynamicState = nullptr;
  pipelineCreateInfo.layout = pipelineLayout;
  pipelineCreateInfo.renderPass = renderPass;
  pipelineCreateInfo.subpass = 0;
  pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
  pipelineCreateInfo.basePipelineIndex = -1;

  result = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &pipeline);
  ASSERT_VULKAN(result);

  framebuffers = new VkFramebuffer[numberOfImagesInSwapchain];
  for (size_t i = 0; i < numberOfImagesInSwapchain; ++i) {
    VkFramebufferCreateInfo framebufferCreateInfo;
    framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferCreateInfo.pNext = nullptr;
    framebufferCreateInfo.flags = 0;
    framebufferCreateInfo.renderPass = renderPass;
    framebufferCreateInfo.attachmentCount = 1;
    framebufferCreateInfo.pAttachments = &(imageViews[i]);
    framebufferCreateInfo.width = WIDTH;
    framebufferCreateInfo.height = HEIGHT;
    framebufferCreateInfo.layers = 1;

    result = vkCreateFramebuffer(device, &framebufferCreateInfo, nullptr, &framebuffers[i]);
    ASSERT_VULKAN(result);
  }

  VkCommandPoolCreateInfo commandPoolCreateInfo;
  commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  commandPoolCreateInfo.pNext = nullptr;
  commandPoolCreateInfo.flags = 0;
  commandPoolCreateInfo.queueFamilyIndex = 0; //TODO civ

  result = vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &commandPool);
  ASSERT_VULKAN(result);

  VkCommandBufferAllocateInfo commandBufferAllocateInfo;
  commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  commandBufferAllocateInfo.pNext = nullptr;
  commandBufferAllocateInfo.commandPool = commandPool;
  commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  commandBufferAllocateInfo.commandBufferCount = numberOfImagesInSwapchain;

  commandBuffers = new VkCommandBuffer[numberOfImagesInSwapchain];
  result = vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, commandBuffers);
  ASSERT_VULKAN(result);

  VkCommandBufferBeginInfo commandBufferBeginInfo;
  commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  commandBufferBeginInfo.pNext = nullptr;
  commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
  commandBufferBeginInfo.pInheritanceInfo = nullptr;

  for (size_t i = 0; i < numberOfImagesInSwapchain; i++) {
    result = vkBeginCommandBuffer(commandBuffers[i], &commandBufferBeginInfo);
    ASSERT_VULKAN(result);

    VkRenderPassBeginInfo renderPassBeginInfo;
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.pNext = nullptr;
    renderPassBeginInfo.renderPass = renderPass;
    renderPassBeginInfo.framebuffer = framebuffers[i];
    renderPassBeginInfo.renderArea.offset = { 0, 0 };
    renderPassBeginInfo.renderArea.extent = { WIDTH, HEIGHT };
    VkClearValue clearValue = { 0.2f, 0.2f, 0.2f, 1.0f };
    renderPassBeginInfo.clearValueCount = 1;
    renderPassBeginInfo.pClearValues = &clearValue;

    vkCmdBeginRenderPass(commandBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

    vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);

    vkCmdEndRenderPass(commandBuffers[i]);

    result = vkEndCommandBuffer(commandBuffers[i]);
    ASSERT_VULKAN(result);
  }

  VkSemaphoreCreateInfo semaphoreCreateInfo;
  semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  semaphoreCreateInfo.pNext = nullptr;
  semaphoreCreateInfo.flags = 0;

  result = vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &semaphoreImageAvailable);
  ASSERT_VULKAN(result);
  result = vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &semaphoreRenderingDone);
  ASSERT_VULKAN(result);

  delete[] swapchainImages;
  delete[] layerProperties;
  delete[] extensionProperties;
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

void gameLoop() {
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    drawFrame();
  }
}

void shutdownVulkan() {
  vkDeviceWaitIdle(device);

  vkDestroySemaphore(device, semaphoreImageAvailable, nullptr);
  vkDestroySemaphore(device, semaphoreRenderingDone, nullptr);

  vkFreeCommandBuffers(device, commandPool, numberOfImagesInSwapchain, commandBuffers);
  delete[] commandBuffers;

  vkDestroyCommandPool(device, commandPool, nullptr);

  for (size_t i = 0; i < numberOfImagesInSwapchain; ++i) {
    vkDestroyFramebuffer(device, framebuffers[i], nullptr);
  }
  delete[] framebuffers;

  vkDestroyPipeline(device, pipeline, nullptr);
  vkDestroyRenderPass(device, renderPass, nullptr);

  for (int i = 0; i < numberOfImagesInSwapchain; ++i) {
    vkDestroyImageView(device, imageViews[i], nullptr);
  }
  delete[] imageViews;

  vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
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


