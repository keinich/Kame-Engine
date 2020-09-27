#pragma once

//System
#include <string>
#include <map>
#include <memory>

//Kame
#include <Kame/Core/References.h>

namespace Kame {
    
  class Game;
  class Window;
  class Input;

  using WindowPtr = std::shared_ptr<Window>;
  using WindowNameMap = std::map< std::wstring, WindowPtr >;

  class Application {

  public:
    static void Create();
    static void Destroy();

    static void ReportLiveObjects();

    static int Run(Reference<Game> game);

    static Reference<Window> GetOrCreateWindow(const std::wstring& windowName, int width, int height, bool vSync = true);

  private:
    static Application* _Instance;

    using WindowNameMap = std::map<std::wstring, Reference<Window>>;
    WindowNameMap _WindowsByName;
  };

}