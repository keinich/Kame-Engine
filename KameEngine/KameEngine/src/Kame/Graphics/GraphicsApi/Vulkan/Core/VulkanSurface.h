#pragma once

#include <Kame/Graphics/GraphicsApi/Surface.h>
#include "../VulkanCommon.h"
#include <GLFW/glfw3.h>

namespace Kame {

  class VulkanSurface : public Surface {

  public:
    VulkanSurface();
    ~VulkanSurface();

    virtual void Destroy() override;

    virtual void CreateFromGlfwWindow(GLFWwindow* glfwWindow) override;

    inline virtual VkSurfaceKHR GetVkSurface() override { return _Handle; }

    inline VkSurfaceKHR GetHandle() { return _Handle; }

  private: // Fields
    VkSurfaceKHR _Handle;

  };

}