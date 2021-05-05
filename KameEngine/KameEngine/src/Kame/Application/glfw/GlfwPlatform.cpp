//TODO PLatform in premake!
//#ifdef KAME_PLATFORM_GLFW

#include "kmpch.h"
#include "GlfwPlatform.h"

// glfw
//#define GLFW_INCLUDE_VULKAN
#include <glfw/glfw3.h>

// Kame
#include <Kame/Application/Window.h>
#include "GlfwWindow.h"

namespace Kame {

  Platform* Platform::CreateInstance() {
    return new GlfwPaltform();
  }

  Platform::Type Platform::GetType() {
    return Type::Glfw;
  }

  void Platform::InitializeInstance() {
    glfwInit();
  }

  void Platform::ShutdownInstance() {
    glfwTerminate();
  }

  const char** Platform::GetRequiredVulkanInstanceExtensions(uint32_t* numberOfRequiredExtensions) {
    return glfwGetRequiredInstanceExtensions(numberOfRequiredExtensions);
  }

  Reference<Window> Platform::CreatePlatformWindow(const std::string& name, int width, int height, bool vSync) {
    Reference<Window> result = CreateReference<GlfwWindow>();
    result->Create(name, width, height, vSync);
    return result;
  }

}

//#endif