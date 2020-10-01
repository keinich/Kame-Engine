#include "kmpch.h"
#include "VulkanAPi.h"

//TODO Remove
#include <old.h>

namespace Kame {

  void VulkanApi::Initialize() {
    startGlfw();
    startVulkan();
  }

  void VulkanApi::Shutdown() {
    shutdownVulkan();
    shutdownGlfw();
  }

}


