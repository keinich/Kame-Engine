#pragma once

namespace Kame {

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
    static const char** GetRequiredVulkanInstanceExtensions(uint32_t* numberOfRequiredInstanceExtensions);    

  protected: // Methods
    Platform();
    ~Platform();

    static Platform* CreateInstance();
    void InitializeInstance();
    void ShutdownInstance();

  protected: // Fields
    static Platform* _Instance;
        
  };

}