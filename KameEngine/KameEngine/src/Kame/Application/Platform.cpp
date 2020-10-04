#include "kmpch.h"
#include "Platform.h"

namespace Kame {

  Platform* Platform::_Instance = nullptr;

  void Platform::Create() {
    KAME_ASSERT(!_Instance, "Platform Instance already created!");

    _Instance = CreateInstance();
    _Instance->Initialize();

  }
  void Platform::Destroy() {
    KAME_ASSERT(_Instance, "Platform Instance cannot be destroyed because it is null!");

    _Instance->Shutdown();

    delete _Instance;
    _Instance = nullptr;
  }
  Platform::Platform() {}

  Platform::~Platform() {}
}