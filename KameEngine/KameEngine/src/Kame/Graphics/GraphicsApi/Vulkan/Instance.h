#pragma once

namespace Kame {

  class VulkanInstance {
  public:
    VulkanInstance();
    ~VulkanInstance();

  private:
    VkInstance _VkInstance;
  };

}