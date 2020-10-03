#include "kmpch.h"
#include "VulkanAPi.h"

// Kame

//TODO Remove
#include <old.h>

namespace Kame {

  void VulkanApi::Initialize() {
    _Instance = CreateNotCopyableReference<VulkanInstance>();
    startGlfw();
    startVulkan();
  }

  void VulkanApi::Shutdown() {
    shutdownVulkan();
    shutdownGlfw();
  }

}


