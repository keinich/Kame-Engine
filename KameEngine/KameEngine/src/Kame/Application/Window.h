#pragma once

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
    virtual void Destroy() = 0;

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