#include "kmpch.h"
#include "VulkanException.h"

#include "VulkanStringExtensions.h"

namespace Kame {

  VulkanException::VulkanException(VkResult result, const std::string& message) : std::runtime_error(message) {
    Result = result;
    _ErrorMessage = std::string(std::runtime_error::what()) + std::string{ " : " } + toString(result);
  }

  const char* VulkanException::what() const noexcept {
    return _ErrorMessage.c_str();
  }

}
