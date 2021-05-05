#pragma once

#include "VulkanCommon.h"

namespace Kame {

  class VulkanInstance;

  class VulkanPhysicalDevice {

  public:

    VulkanPhysicalDevice(VulkanInstance& instance, VkPhysicalDevice handle);
    ~VulkanPhysicalDevice();

    inline VkPhysicalDevice GetHandle() { return _Handle; }

    const VkPhysicalDeviceFeatures& GetFeatures() const;
    const VkPhysicalDeviceProperties GetProperties() const;
    const VkPhysicalDeviceMemoryProperties GetMemoryProperties() const;
    const std::vector<VkQueueFamilyProperties>& GetQueueFamilyProperties() const;

    VkBool32 IsPresentSupported(VkSurfaceKHR surface, int queueFamilyIndex);

    void* GetExtensionFeatureChain() const;
    const VkPhysicalDeviceFeatures GetRequestedFeatures() const;

    template <typename T>
    T& RequestExtensionFeatures(VkStructureType type) {
      
      // This needs to be enabled
      if (!_Instance->IsEnabled(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME)) {
        throw std::runtime_error(
          "Couldnt request feature from device as" +
          std::string(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME) +
          "isn't enabled!"
        );
      }

      auto extensionFeaturesIt = _ExtensionFeatures.find(type);
      if (extensionFeaturesIt != _ExtensionFeatures.end()) {
        return *static_cast<T*>(extensionFeaturesIt->second.get());
      }

      VkPhysicalDeviceFeatures2KHR physicalDeviceFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2_KHR };
      T extension(type);
      physicalDeviceFeatures.pNext = &extension;
      vkGetPhysicalDeviceFeatures2KHR(_Handle, &physicalDeviceFeatures);

      _ExtensionFeatures.insert({ type, std::make_shared<T>(extension) });

      T* extensionPtr = static_cast<T*>(_ExtensionFeatures.find(type)->second.get());

      if (_LastRequestedExtensionFeature) {
        extensionPtr->pNext = _LastRequestedExtensionFeature;
      }
      _LastRequestedExtensionFeature = extensionPtr;

      return *extensionPtr;
    }

  private: // Methods
    void Initialize();

  private: // Fields
    VkPhysicalDevice _Handle;
    VulkanInstance& _Instance;

    VkPhysicalDeviceFeatures _Features;
    VkPhysicalDeviceProperties _Properties;
    VkPhysicalDeviceMemoryProperties _MemoryProperties;
    std::vector<VkQueueFamilyProperties> _QueueFamilyProperties;

    void* _LastRequestedExtensionFeature{ nullptr };
    std::map<VkStructureType, std::shared_ptr<void>> _ExtensionFeatures;

    VkPhysicalDeviceFeatures _RequestedFeatures{};
  };

}
