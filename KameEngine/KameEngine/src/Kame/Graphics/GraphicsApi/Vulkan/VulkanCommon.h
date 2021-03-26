#pragma once

//#define VK_NO_PROTOTYPES
//#define VOLK_IMPLEMENTATION
#include "volk.h"

#define ASSERT_VULKAN(val)\
  if (val!= VK_SUCCESS) {\
    __debugbreak();\
  }
