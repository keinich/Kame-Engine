//TODO PLatform in premake!
//#ifdef KAME_PLATFORM_GLFW
#pragma once

// System
#include <vector>

// Kame
#include "../Platform.h"

namespace Kame {

  class GlfwPaltform : public Platform {

  public:
    GlfwPaltform() {};
    ~GlfwPaltform() {};

  };

}

//#endif