#pragma once

#include "Kame/Application/Window.h"

namespace Kame {

  enum class GraphicsApiType {
    None = 0,
    Vulkan = 1,
    Dx12 = 2
  };

  class GraphicsApi {
    friend class VulkanApi;

  public:

    /// <summary>
    /// Creates the singleton instance and prepares it so that windows can be created.
    /// </summary>
    static void Create();

    /// <summary>
    /// Destroys the singleton instance
    /// </summary>
    static void Destroy();

    /// <summary>
    /// Initializes the singleton instance after Platform and Main Window have been created.
    /// </summary>
    static void Init();

    /// <summary>
    /// Shots down the singleton instance before Platform and Main Window are destroyed
    /// </summary>
    static void Shutdown();

    static void CreateWindowSurface(Reference<Window> window);

  protected:

    GraphicsApi() {};
    ~GraphicsApi() {};

    virtual void InitInstance() = 0;
    virtual void ShutdownInstance() = 0;
    virtual void DestroyInstance() = 0;
    virtual void CreateWindowSurfaceInternal(Reference<Window> window) = 0;

  private: // Fields
    static GraphicsApi* _Instance;
    static GraphicsApiType _Type;

  };


}