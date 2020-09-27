#pragma once

namespace Kame {

  class Window {

  public: // Methods
    Window();
    ~Window();

    void Create(int width, int height, bool vSync = true);


  private: // Fields
    int _Width;
    int _Height;
    bool _VSync;
    bool _Created;

  private: // Methods
    bool PlatformCreate();
  };

}