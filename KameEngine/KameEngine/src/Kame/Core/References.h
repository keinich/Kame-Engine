#pragma once

#include <memory>

namespace Kame {

  template<typename T>
  using NotCopyableReference = std::unique_ptr<T>;
  template<typename T, typename ... Args>
  constexpr NotCopyableReference<T> CreateNotCopyableReference(Args&& ... args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
  }

  template<typename T>
  using Reference = std::shared_ptr<T>;
  template<typename T, typename ... Args>
  constexpr Reference<T> CreateReference(Args&& ... args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
  }
}