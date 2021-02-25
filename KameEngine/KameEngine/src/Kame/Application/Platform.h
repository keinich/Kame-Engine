#pragma once

namespace Kame {

  class Window;

  class Platform {

  public:
    enum class Type {
      Glfw,
      Win32
    };

    /// <summary>
    /// Creates and initializes the singleton instance.
    /// </summary>
    static void Create();

    /// <summary>
    /// Shuts down and destroys the singleton instance.
    /// </summary>
    static void Destroy();

    Type GetType();

    static void CreateMainWindow(int width, int height, bool vSync = true);
    static const Reference<Window> GetMainWindow();

    static Reference<Window> CreateRenderWindow(const std::string& name, int width, int height, bool vSync = true);
    static void DestroyRenderWindow(const std::string& name);

    static const Reference<Window> GetRenderWindow(const std::string& name);

    static const char** GetRequiredVulkanInstanceExtensions(uint32_t* numberOfRequiredInstanceExtensions);

  protected: // Methods
    Platform();
    ~Platform();

    static Platform* CreateInstance();
    void InitializeInstance();
    void ShutdownInstance();

    Reference<Window> CreatePlatformWindow(const std::string& name, int width, int height, bool vSync = true);

  protected: // Fields
    static Platform* _Instance;

    std::map<std::string, Reference<Window>> _WindowsByName;
  };

}