#include "kmpch.h"
#include "Platform.h"

// Kame
#include "Window.h"
#include "Kame/Graphics/GraphicsApi/GraphicsApi.h"

namespace Kame {

  Platform* Platform::_Instance = nullptr;

  void Platform::Create() {
    KAME_ASSERT(!_Instance, "Platform Instance already created!");

    _Instance = CreateInstance();
    _Instance->InitializeInstance();

  }
  void Platform::Destroy() {
    KAME_ASSERT(_Instance, "Platform Instance cannot be destroyed because it is null!");

    //DestroyRenderWindow("Main Window");

    KAME_ASSERT(
      _Instance->_WindowsByName.empty(),
      "All windows should be destroyed before destroying the application instance."
    );

    _Instance->ShutdownInstance();

    delete _Instance;
    _Instance = nullptr;
  }

  Platform::Platform() {}

  Platform::~Platform() {}

  void Platform::CreateMainWindow(int width, int height, bool vSync) {
    Platform* me = _Instance;
    std::map<std::string, Reference<Window>>::iterator windowIterator = me->_WindowsByName.find("Main Window");
    if (windowIterator != me->_WindowsByName.end()) {
      KAME_ASSERT(false, "Main Window was already created!");
    }

    Reference<Window> window = me->CreateRenderWindow("Main Window", width, height, vSync);
    GraphicsApi::CreateWindowSurface(window);

    me->_WindowsByName.insert(std::map<std::string, Reference<Window>>::value_type("Main Window", window));

  }

  const Reference<Window> Platform::GetMainWindow() {
    return GetRenderWindow("Main Window");
  }
  Reference<Window> Platform::CreateRenderWindow(const std::string& name, int width, int height, bool vSync) {
    return _Instance->CreatePlatformWindow(name, width, height, vSync);
  }

  void Platform::DestroyRenderWindow(const std::string& name) {
    Platform* me = _Instance;
    std::map<std::string, Reference<Window>>::iterator windowIter = me->_WindowsByName.find(name);
    if (windowIter != me->_WindowsByName.end()) {
      windowIter->second->Destroy();
      me->_WindowsByName.erase(windowIter);
    }
  }

  const Reference<Window> Platform::GetRenderWindow(const std::string& name) {
    Reference<Window> window;
    std::map<std::string, Reference<Window>>::iterator iter = _Instance->_WindowsByName.find(name);
    if (iter != _Instance->_WindowsByName.end()) {
      window = iter->second;
    }
    return window;
  }
}