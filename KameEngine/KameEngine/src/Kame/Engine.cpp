#include "kmpch.h"

#include "Engine.h"

// Kame
#include <Kame/Core/DebugUtils.h>
#include "Application/Window.h"
#include "Kame/Game/Game.h"
#include <Kame/Graphics/GraphicsApi/GraphicsApi.h>
#include <Kame/Application/Platform.h>

// TODO Remove
#include "old.h"

namespace Kame {

  Engine* Engine::_Instance = nullptr;

  void Engine::Create(Reference<Game> game) {
    KAME_ASSERT(!_Instance, "Instance is not null");
    _Instance = new Engine();

    _Instance->_Game = game;
    
    Platform::Create();
    GraphicsApi::Create();
  }

  void Engine::Destroy() {

    GraphicsApi::Destroy();
    Platform::Destroy();

    KAME_ASSERT(_Instance, "Instance is null");

    KAME_ASSERT(
      _Instance->_WindowsByName.empty(),
      "All windows should be destroyed before destroying the application instance."
    );

    delete _Instance;
    _Instance = nullptr;
  }

  void Engine::ReportLiveObjects() {}

  int Engine::Run() {

    _Instance->_Game->Initialize();

    gameLoop();

    return 0;
  }

  Reference<Window> Engine::GetOrCreateWindow(const std::wstring& windowName, int width, int height, bool vSync) {
    KAME_ASSERT(_Instance, "Application is null");
    WindowNameMap::iterator windowIt = _Instance->_WindowsByName.find(windowName);
    if (windowIt != _Instance->_WindowsByName.end()) {
      return windowIt->second;
    }

    Reference<Window> window = CreateReference<Window>();
    window->Create(width, height, vSync);
  }

  const char* Engine::GetGameName() {
    KAME_ASSERT(_Instance, "Application is null");
    return _Instance->_Game->GetName();
  }

  const char** Engine::GetRequiredVulkanInstanceExtensions(uint32_t* numberOfExtensions) {
    return Platform::GetRequiredVulkanInstanceExtensions(numberOfExtensions);
  }

}
