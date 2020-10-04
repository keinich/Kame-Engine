//TODO PLatform in premake!
//#ifdef KAME_PLATFORM_GLFW

#include "kmpch.h"
#include "GlfwPlatform.h"

// glfw
#define GLFW_INCLUDE_VULKAN
#include <glfw/glfw3.h>

namespace Kame {

  Platform* Platform::CreateInstance() {
    return new GlfwPaltform();
  }

  Platform::Type Platform::GetType() {
    return Type::Glfw;
  }

  void Platform::Initialize() {
    glfwInit();
  }

  void Platform::Shutdown() {
    glfwTerminate();
  }

  const char** Platform::GetRequiredVulkanInstanceExtensions(uint32_t* numberOfRequiredExtensions) {
    return glfwGetRequiredInstanceExtensions(numberOfRequiredExtensions);
  }

}

//#endif