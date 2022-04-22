#pragma once

namespace Kame {

  class VulkanCommandBuffer {

  public:

    enum class ResetMode {
      ResetPool,
      ResetIndividually,
      AlwaysAllocate
    };

  };

}