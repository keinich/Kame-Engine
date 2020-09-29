#pragma once

// Kame
#include "../GraphicsApi.h"

namespace Kame {

  class VulkanApi : public GraphicsApi {
    friend class GraphicsApi;
  public:

  protected:
    VulkanApi() {};
    ~VulkanApi() {};

    virtual void Initialize() override;
    virtual void Shutdown() override;
  };

}