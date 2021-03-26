#pragma once

#include <GLFW/glfw3.h>

namespace Kame {

  class Surface {

  public:
    Surface() {};
    ~Surface() {};

    virtual void Destroy() = 0;

    virtual void CreateFromGlfwWindow(GLFWwindow* glfwWindow) = 0;

  };

}