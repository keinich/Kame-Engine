#include "kmpch.h"
#include "Window.h"

namespace Kame {

  Window::Window() :
    _Width(0),
    _Height(0),
    _VSync(true),
    _Created(false) {}

  Window::~Window() {}

  void Window::Create(int width, int height, bool vSync) {
    _Width = width;
    _Height = height;
    _VSync = vSync;
    _Created = PlatformCreate();
    
  }
}

