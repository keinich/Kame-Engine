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

    Log::Init();
    Platform::Create();
    GraphicsApi::Create();

    // TODO game settings to get resolution
    Platform::CreateMainWindow(800, 600);
    startGlfw();
    GraphicsApi::Init();
  }

  void Engine::Destroy() {

    GraphicsApi::Shutdown();
    Platform::DestroyRenderWindow("Main Window");
    GraphicsApi::Destroy();
    Platform::Destroy();

    KAME_ASSERT(_Instance, "Instance is null");

    delete _Instance;
    _Instance = nullptr;
  }

  void Engine::ReportLiveObjects() {}

  int Engine::Run() {

    _Instance->_Game->Initialize();

    gameLoop();

    return 0;
  }

#pragma region " Vulkan specific stuff "

  const char* Engine::GetGameName() {
    KAME_ASSERT(_Instance, "Application is null");
    return _Instance->_Game->GetName();
  }

  const char** Engine::GetRequiredVulkanInstanceExtensions(uint32_t* numberOfExtensions) {
    return Platform::GetRequiredVulkanInstanceExtensions(numberOfExtensions);
  }

  const std::vector<const char*> Engine::GetRequiredVulkanInstanceValidationLayers() {
    return {
      "VK_LAYER_KHRONOS_validation"
    };
  }

#pragma endregion 

}
