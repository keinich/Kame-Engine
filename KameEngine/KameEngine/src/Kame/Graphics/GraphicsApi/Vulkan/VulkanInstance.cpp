#include "kmpch.h"
#define VOLK_IMPLEMENTATION
#include "VulkanCommon.h"
#include "VulkanInstance.h"
#include "Kame/Engine.h"
#include <Kame/Graphics/GraphicsApi/Vulkan/VulkanException.h>

namespace Kame {

  namespace { // debug callbacks
#if defined(KAME_DEBUG) || defined(VKB_VALIDATION_LAYERS)

    VKAPI_ATTR VkBool32 VKAPI_CALL debugUtilsMessengerCallback(
      VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
      VkDebugUtilsMessageTypeFlagsEXT message_type,
      const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
      void* user_data
    ) {
      // Log debug messge
      if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        KM_CORE_INFO(
          "{} - {}: {}", callback_data
          ->messageIdNumber,
          callback_data->pMessageIdName,
          callback_data->pMessage
        );
      }
      else if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        KM_CORE_ERROR(
          "{} - {}: {}",
          callback_data->messageIdNumber,
          callback_data->pMessageIdName,
          callback_data->pMessage
        );
      }
      return VK_FALSE;
    }

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT /*type*/,
      uint64_t /*object*/, size_t /*location*/, int32_t /*message_code*/,
      const char* layer_prefix, const char* message, void* /*user_data*/) {
      if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) {
        KM_CORE_ERROR("{}: {}", layer_prefix, message);
      }
      else if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT) {
        KM_CORE_WARN("{}: {}", layer_prefix, message);
      }
      else if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) {
        KM_CORE_WARN("{}: {}", layer_prefix, message);
      }
      else {
        KM_CORE_INFO("{}: {}", layer_prefix, message);
      }
      return VK_FALSE;
    }
#endif

  }        // namespace

  VulkanInstance::VulkanInstance() {
    _VkInstance = VK_NULL_HANDLE;
  }

  VulkanInstance::~VulkanInstance() {}

  VulkanPhysicalDevice& VulkanInstance::GetBestPhysicalDevice() {
    KAME_ASSERT(!_PhysicalDevices.empty(), "No physical devices!");

    for (auto& gpu : _PhysicalDevices) {
      if (gpu->GetProperties().deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        return *gpu;
      }
    }

    KM_CORE_ERROR("No discrete physical device found!");
    return *_PhysicalDevices.at(0);
  }

  void VulkanInstance::Init(bool headless) {

    VkResult result = volkInitialize();
    ASSERT_VULKAN(result);

    // Extensions
    this->InitExtensions(headless);

    // Layers
    this->InitLayers();

    // Application Info
    VkApplicationInfo appInfo;
    FillApplicationInfo(appInfo);

    // Creating the VulkanInstace
    VkInstanceCreateInfo instanceInfo;
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pNext = nullptr; // Erweiterungen
    instanceInfo.flags = 0; // noch keine Verwendung
    instanceInfo.pApplicationInfo = &appInfo;
    instanceInfo.enabledLayerCount = _RequestedLayers.size();
    instanceInfo.ppEnabledLayerNames = _RequestedLayers.data();
    instanceInfo.enabledExtensionCount = _EnabledExtensions.size();
    instanceInfo.ppEnabledExtensionNames = _EnabledExtensions.data();

    result = vkCreateInstance(&instanceInfo, NULL, &_VkInstance);
    ASSERT_VULKAN(result);

#if defined(KAME_DEBUG) || defined(VKB_VALIDATION_LAYERS)
    const bool hasDebugUtils = this->IsEnabled(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    VkDebugUtilsMessengerCreateInfoEXT debug_utils_create_info = { VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
    VkDebugReportCallbackCreateInfoEXT debug_report_create_info = { VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT };
    if (hasDebugUtils) {
      debug_utils_create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
      debug_utils_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
      debug_utils_create_info.pfnUserCallback = debugUtilsMessengerCallback;

      instanceInfo.pNext = &debug_utils_create_info;
    }
    else {
      debug_report_create_info.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
      debug_report_create_info.pfnCallback = debugCallback;

      instanceInfo.pNext = &debug_report_create_info;
    }
#endif

    volkLoadInstance(_VkInstance);

#if defined(KAME_DEBUG) || defined(VKB_VALIDATION_LAYERS)
    if (hasDebugUtils) {
      result = vkCreateDebugUtilsMessengerEXT(_VkInstance, &debug_utils_create_info, nullptr, &_DebugUtilsMessenger);
      if (result != VK_SUCCESS) {
        throw VulkanException(result, "Could not create debug utils messenger");
      }
    }
    else {
      result = vkCreateDebugReportCallbackEXT(_VkInstance, &debug_report_create_info, nullptr, &_DebugReportCallback);
      if (result != VK_SUCCESS) {
        throw VulkanException(result, "Could not create debug report callback");
      }
    }
#endif

    QueryGpus();
  }

  void VulkanInstance::FillApplicationInfo(VkApplicationInfo& appInfo) {
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = nullptr; // Erweiterungen
    appInfo.pApplicationName = Engine::GetGameName();
    appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 0);
    appInfo.pEngineName = "Kame Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_2;
  }

  void VulkanInstance::InitExtensions(bool headless) {

    // Get the available Instance extensions
    uint32_t numberOfInstanceExtensions;
    VkResult result = vkEnumerateInstanceExtensionProperties(nullptr, &numberOfInstanceExtensions, nullptr);
    ASSERT_VULKAN(result);

    std::vector<VkExtensionProperties> availableInstanceExtensions(numberOfInstanceExtensions);
    result = vkEnumerateInstanceExtensionProperties(nullptr, &numberOfInstanceExtensions, availableInstanceExtensions.data());

    // Instance Extensions
#if defined(KAME_DEBUG)
    bool debugUtils = false;
    KM_CORE_INFO("{} InstanceExensions are available", numberOfInstanceExtensions);
    for (VkExtensionProperties& availableExtension : availableInstanceExtensions) {
      if (strcmp(availableExtension.extensionName, VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == 0) {
        debugUtils = true;
        KM_CORE_INFO("\t{} is available, enabling it", VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        _EnabledExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
      }
      KM_CORE_INFO("\t{} is available", availableExtension.extensionName);
    }
    if (!debugUtils) {
      _EnabledExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
    }
#endif

    // Headless surface extension
    if (headless) {
      bool headlessExtensionAvailable = false;
      for (VkExtensionProperties& availableExtension : availableInstanceExtensions) {
        if (strcmp(availableExtension.extensionName, VK_EXT_HEADLESS_SURFACE_EXTENSION_NAME) == 0) {
          headlessExtensionAvailable = true;
          KM_CORE_INFO("\t{} is available, enabling it", VK_EXT_HEADLESS_SURFACE_EXTENSION_NAME);
          _EnabledExtensions.push_back(VK_EXT_HEADLESS_SURFACE_EXTENSION_NAME);
        }
      }
      if (!headlessExtensionAvailable) {
        KM_CORE_WARN("\t{} is not available, disabling swapchain cration", VK_EXT_HEADLESS_SURFACE_EXTENSION_NAME);
      }
    }
    else {
      _EnabledExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
    }

    for (VkExtensionProperties& availableExtension : availableInstanceExtensions) {
      if (strcmp(availableExtension.extensionName, VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME) == 0) {
        KM_CORE_INFO("\t{} is available, enabling it", VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
        _EnabledExtensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
      }
    }

    // Check if all the required instance extensions are available
    uint32_t numberOfRequiredExtensions = 0;
    auto requiredExtensions = Engine::GetRequiredVulkanInstanceExtensions(&numberOfRequiredExtensions);
    bool requiredExtensionsMissing = false;
    for (int i = 0; i < numberOfRequiredExtensions; ++i) {
      const char* extension = requiredExtensions[i];
      bool extensionIsUnavailable = std::find_if(
        availableInstanceExtensions.begin(), availableInstanceExtensions.end(),
        [&extension](VkExtensionProperties availableExtension) {
          return strcmp(availableExtension.extensionName, extension) == 0;
        }
      ) == availableInstanceExtensions.end();
      if (extensionIsUnavailable) {
        KM_CORE_ERROR("Required extension {} is unavailable, cannot run", extension);
        requiredExtensionsMissing = true;
      }
      else {
        _EnabledExtensions.push_back(extension);
      }
    }
    if (requiredExtensionsMissing) {
      throw std::runtime_error("Required instance extensions are missing.");
    }
  }

  void VulkanInstance::InitLayers() {
    // Instance Layers
    uint32_t numberOfInstanceLayers;
    VkResult result = vkEnumerateInstanceLayerProperties(&numberOfInstanceLayers, nullptr);
    ASSERT_VULKAN(result);

    std::vector<VkLayerProperties> supportedValidationLayers(numberOfInstanceLayers);
    result = vkEnumerateInstanceLayerProperties(&numberOfInstanceLayers, supportedValidationLayers.data());
    ASSERT_VULKAN(result);

    const std::vector<const char*>& requiredValidationLayers = Engine::GetRequiredVulkanInstanceValidationLayers();
    std::vector<const char*> requestedValidationLayers(requiredValidationLayers);

#if KAME_DEBUG

    KM_CORE_INFO("{} InstanceLayers are available", numberOfInstanceLayers);
    for (const VkLayerProperties& layerName : supportedValidationLayers) {
      KM_CORE_INFO("\t{} is available", layerName.layerName);
    }

#endif

    if (ValidateLayers(requiredValidationLayers, supportedValidationLayers)) {
      KM_CORE_INFO("Enabled Validation Layers:");
      for (const char* const& layer : requestedValidationLayers) {
        KM_CORE_INFO(" \t{}", layer);
      }
    }
    else {
      throw std::runtime_error("Required validation layers are missing!");
    }

    _RequestedLayers = requestedValidationLayers;
  }

  void VulkanInstance::Shutdown() {

#if defined(KAME_DEBUG) || defined(VKB_VALIDATION_LAYERS)
    if (_DebugUtilsMessenger != VK_NULL_HANDLE) {
      vkDestroyDebugUtilsMessengerEXT(_VkInstance, _DebugUtilsMessenger, nullptr);
    }
    if (_DebugReportCallback != VK_NULL_HANDLE) {
      vkDestroyDebugReportCallbackEXT(_VkInstance, _DebugReportCallback, nullptr);
    }
#endif

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
      _PhysicalDevices.push_back(CreateNotCopyableReference<VulkanPhysicalDevice>(*this, physicalDevice));
    }

  }

  bool VulkanInstance::ValidateLayers(
    const std::vector<const char*>& requiredLayers,
    const std::vector<VkLayerProperties>& availableLayers
  ) {
    for (const char* layer : requiredLayers) {
      bool found = false;
      for (const VkLayerProperties& availableLayer : availableLayers) {
        if (strcmp(availableLayer.layerName, layer) == 0) {
          found = true;
          break;
        }
      }
      if (!found) {
        KM_CORE_ERROR("Validation Layer {} not found", layer);
        return false;
      }
    }
    return true;
  }

}