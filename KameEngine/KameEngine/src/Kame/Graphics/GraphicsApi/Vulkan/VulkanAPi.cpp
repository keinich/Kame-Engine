#include "kmpch.h"
#include "VulkanAPi.h"

// Kame

//TODO Remove
#include <old.h>

namespace Kame {

  VulkanApi* VulkanApi::_Instance = nullptr;

  VulkanApi* VulkanApi::CreateInstance() {
    KAME_ASSERT(!_Instance, "VulkanApi cannot be created because it is not null");

    _Instance = new VulkanApi();
    return _Instance;
  }

  void VulkanApi::Initialize() {

    //TODO add extensions according to the game!
    AddDeviceExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

    _VulkanInstance = CreateNotCopyableReference<VulkanInstance>();
    _VulkanInstance->Initialize(true);
    _VulkanDevice = CreateNotCopyableReference<VulkanDevice>();
    _VulkanDevice->Initialize(_VulkanInstance->GetBestPhysicalDevice(), GetDeviceExtensions());
    
    startGlfw();
    startVulkan();
  }

  void VulkanApi::Shutdown() {
    shutdownVulkan();
    _VulkanDevice->Shutdown();
    _VulkanInstance->Shutdown();
    shutdownGlfw();
  }

  void VulkanApi::AddDeviceExtension(const char* extension, bool optional) {
    _DeviceExtensions[extension] = optional;
  }

  const std::unordered_map<const char*, bool> VulkanApi::GetDeviceExtensions() {
    return _DeviceExtensions;
  }

}


