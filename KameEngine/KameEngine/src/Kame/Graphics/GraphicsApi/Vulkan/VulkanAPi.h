#pragma once

// Kame
#include <Kame/Core/References.h>
#include "../GraphicsApi.h"
#include "VulkanInstance.h"

namespace Kame {

  class VulkanInstance;

  class VulkanApi : public GraphicsApi {
    friend class GraphicsApi;
  public:

  protected:
    VulkanApi() {};
    ~VulkanApi() {};

    virtual void Initialize() override;
    virtual void Shutdown() override;

  private: // Fields
    NotCopyableReference<VulkanInstance> _Instance;
  };

}