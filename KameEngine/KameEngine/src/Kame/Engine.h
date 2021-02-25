#pragma once

//System
#include <string>
#include <map>
#include <memory>
#include <vector>

//Kame
#include <Kame/Core/References.h>

namespace Kame {

  class Input;
  class Platform;

  class Engine {

    friend class Game;

  public:
    static void Create(Reference<Game>);
    static void Destroy();

    static void ReportLiveObjects();

    static int Run();

    static const char* GetGameName();

    // Vulkan specific stuff, that is required by the game the engine is running
    // TODO get this out of the game settings!
    static const char** GetRequiredVulkanInstanceExtensions(uint32_t* numberOfExtensions);
    static const std::vector<const char*> GetRequiredVulkanInstanceValidationLayers();

  private:
    static Engine* _Instance;

    Reference<Game> _Game;
  };

}