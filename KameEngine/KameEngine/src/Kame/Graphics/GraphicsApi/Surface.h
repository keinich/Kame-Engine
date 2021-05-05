#pragma once

#include "Kame/Graphics/GraphicsApi/Vulkan/VulkanCommon.h"
#include <GLFW/glfw3.h>

namespace Kame {

  class Surface {

  public:
    Surface() {};
    ~Surface() {};

    virtual void Destroy() = 0;

    virtual void CreateFromGlfwWindow(GLFWwindow* glfwWindow) = 0;

    virtual VkSurfaceKHR GetVkSurface() = 0;
  };

}