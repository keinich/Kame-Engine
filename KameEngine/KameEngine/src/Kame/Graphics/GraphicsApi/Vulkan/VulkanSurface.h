#pragma once

#include "VulkanCommon.h"
#include "../Surface.h"
#include <GLFW/glfw3.h>

namespace Kame {

  class VulkanSurface : public Surface {

  public:
    VulkanSurface();
    ~VulkanSurface();

    virtual void Destroy() override;

    virtual void CreateFromGlfwWindow(GLFWwindow* glfwWindow) override;

    inline VkSurfaceKHR GetHandle() { return _Handle; }

  private: // Fields
    VkSurfaceKHR _Handle;

  };

}