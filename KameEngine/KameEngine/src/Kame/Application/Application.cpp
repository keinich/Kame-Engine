#include "kmpch.h"

#include "Application.h"

// Kame
#include <Kame/Core/DebugUtils.h>
#include "Window.h"
#include "Kame/Game/Game.h"
#include <Kame/Graphics/GraphicsApi/GraphicsApi.h>

// TODO Remove
#include "KameEngine.h"

namespace Kame {

  Application* Application::_Instance = nullptr;

  void Application::Create() {
    KAME_ASSERT(!_Instance, "Instance is not null");
    _Instance = new Application();

    GraphicsApi::Create();
    startGlfw();
    startVulkan();
  }

  void Application::Destroy() {
    shutdownVulkan();
    shutdownGlfw();

    GraphicsApi::Destroy();

    KAME_ASSERT(_Instance, "Instance is null");

    KAME_ASSERT(
      _Instance->_WindowsByName.empty(),
      "All windows should be destroyed before destroying the application instance."
    );

    delete _Instance;
    _Instance = nullptr;
  }

  void Application::ReportLiveObjects() {}

  int Application::Run(Reference<Game> game) {

    game->Initialize();

    gameLoop();

    return 0;
  }

  Reference<Window> Application::GetOrCreateWindow(const std::wstring& windowName, int width, int height, bool vSync) {
    KAME_ASSERT(_Instance, "Application is null");
    WindowNameMap::iterator windowIt = _Instance->_WindowsByName.find(windowName);
    if (windowIt != _Instance->_WindowsByName.end()) {
      return windowIt->second;
    }

    Reference<Window> window = CreateReference<Window>();
    window->Create(width, height, vSync);
  }

}
