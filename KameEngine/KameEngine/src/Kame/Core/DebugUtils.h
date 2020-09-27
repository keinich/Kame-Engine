#pragma once

#include "Kame/Logging/Log.h"

#define KAME_ASSERT(x, ...) { if(!(x)) {KM_ERROR("Assertion failed: {0}", __VA_ARGS__); __debugbreak(); } }
