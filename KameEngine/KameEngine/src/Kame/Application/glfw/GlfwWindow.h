#pragma once

#include "../Window.h"

// glfw
//#include <GLFW/glfw3.h>

namespace Kame {

  class GlfwWindow : public Window {
  public:
    GlfwWindow();
    ~GlfwWindow();

    GlfwWindow(const GlfwWindow&) = delete;
    GlfwWindow(GlfwWindow&&) = delete;
    GlfwWindow& operator=(const GlfwWindow&) = delete;
    GlfwWindow& operator=(GlfwWindow&&) = delete;

    virtual void DestroyInternal() override;

    virtual void CreateSurface(Reference<Surface> surface) override;

  private: // Fields
    GLFWwindow* _Handle;

  protected: // Methods
    virtual bool PlatformCreate(const std::string& name, int width, int height, bool vSync) override;
  };

}