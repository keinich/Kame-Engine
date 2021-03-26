#pragma once
#include <Kame/Graphics/GraphicsApi/Surface.h>

namespace Kame {

  class Window {

  public: // Methods
    Window();
    ~Window();

    Window(const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator=(const Window&) = delete;
    Window& operator=(Window&&) = delete;

    void Create(const std::string& name, int width, int height, bool vSync = true);
    void Destroy();
    virtual void DestroyInternal() = 0;

    virtual void CreateSurface(Reference<Surface> surface) = 0;
    virtual inline Reference<Surface> GetSurface() { return _Surface; };

  protected: // Fields

    Reference<Surface> _Surface;

  private: // Fields
    int _Width;
    int _Height;
    bool _VSync;
    bool _Created;
    std::string _Name;

  protected: // Methods
    virtual bool PlatformCreate(const std::string& name, int width, int height, bool vSync) = 0;
  };

}