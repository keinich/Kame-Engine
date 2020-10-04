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
    _VulkanInstance = CreateNotCopyableReference<VulkanInstance>();
    _VulkanInstance->Initialize();
    startGlfw();
    startVulkan();
  }

  void VulkanApi::Shutdown() {
    shutdownVulkan();
    shutdownGlfw();
  }

}


