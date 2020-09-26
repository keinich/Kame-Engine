#pragma once

//System
#include <string>
#include <map>
#include <memory>

//Kame
#include <Kame/Core/References.h>

//#ifdef KAME_PLATFORM_WIN32
#define KAME_NATIVE_WINDOW HWND
namespace Kame {
  class Win32Window;
}
#define KAME_WINDOW Win32Window
//#endif

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
  };

}