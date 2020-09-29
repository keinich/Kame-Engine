#pragma once

namespace Kame {

  enum class GraphicsApiType {
    None = 0,
    Vulkan = 1,
    Dx12 = 2
  };

  class GraphicsApi {
    friend class VulkanApi;

  public:

    static void Create();
    static void Destroy();

  protected:

    GraphicsApi() {};
    ~GraphicsApi() {};

    virtual void Initialize() = 0;
    virtual void Shutdown() = 0;

  private: // Fields
    static GraphicsApi* _Instance;
    static GraphicsApiType _Type;

  };


}