#include "kmpch.h"

#include "../Window.h"

// glfw
#define GLFW_INCLUDE_VULKAN
#include <glfw/glfw3.h>
#include "GlfwWindow.h"

namespace Kame {

  bool GlfwWindow::PlatformCreate(const std::string& name, int width, int height, bool vSync) {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    //glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    _Handle = glfwCreateWindow(width, height, "Kame Engine", nullptr, nullptr);
    //glfwSetWindowSizeCallback(_Handle, onWindowResized);

    return true;
  }

  void GlfwWindow::DestroyInternal() {
    glfwDestroyWindow(_Handle);
  }

  void GlfwWindow::CreateSurface(Reference<Surface> surface) {
    surface->CreateFromGlfwWindow(_Handle);
    _Surface = surface;
  }

  GlfwWindow::GlfwWindow() : Window() {
    _Handle = nullptr;
  }
  GlfwWindow::~GlfwWindow() {}
}