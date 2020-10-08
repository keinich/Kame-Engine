#include "kmpch.h"
#include "VulkanPhyiscalDevice.h"

namespace Kame {

  VulkanPhysicalDevice::VulkanPhysicalDevice(VkPhysicalDevice handle) {
    _Handle = handle;
  }

  VulkanPhysicalDevice::~VulkanPhysicalDevice() {}

  void VulkanPhysicalDevice::Initialize() {

  }

}