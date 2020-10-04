#pragma once

namespace Kame {

  class Platform {

  public:
    enum class Type {
      Glfw,
      Win32
    };

    static void Create();
    static void Destroy();

    Type GetType();
    static const char** GetRequiredVulkanInstanceExtensions(uint32_t* numberOfRequiredInstanceExtensions);    

  protected: // Methods
    Platform();
    ~Platform();

    static Platform* CreateInstance();
    void Initialize();
    void Shutdown();

  protected: //
    static Platform* _Instance;
  };

}