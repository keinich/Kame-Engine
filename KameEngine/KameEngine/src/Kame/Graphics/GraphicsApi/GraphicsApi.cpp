#include "kmpch.h"
#include "GraphicsApi.h"

// Kame
#include "Kame/Core/DebugUtils.h"
#include "Vulkan/VulkanApi.h"

namespace Kame {

  GraphicsApi* GraphicsApi::_Instance = nullptr;
  GraphicsApiType GraphicsApi::_Type = GraphicsApiType::None;

  void Kame::GraphicsApi::Create() {
    KAME_ASSERT(!_Instance, "GraphicsApi already created!");

    _Type = GraphicsApiType::Vulkan;

    switch (_Type) {
    case GraphicsApiType::Vulkan:
      _Instance = VulkanApi::CreateInstance();
      _Instance->Initialize();
    }

  }

  void GraphicsApi::Destroy() {
    KAME_ASSERT(_Instance, "GraphicsApi cannot be destroyed becaust it is null");
    _Instance->Shutdown();
    delete _Instance;
    _Instance = nullptr;
  }
}

