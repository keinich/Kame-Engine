#pragma once

#include <stdexcept>

#include "../VulkanCommon.h"

namespace Kame{

  class VulkanException : public std::runtime_error {
  public: // Methods
    VulkanException(VkResult result, const std::string& message = "Vulkan error");

    const char* what() const noexcept override;

  public: // Properties
    VkResult Result;

  private:
    std::string _ErrorMessage;
  };

}