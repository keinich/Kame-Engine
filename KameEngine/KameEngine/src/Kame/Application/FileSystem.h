#pragma once

namespace Kame {

  namespace FileSystem {

    namespace Path {

      enum class Type {
        Assets = 0,
        VulkanShaders = 1
      };

      const std::string GetPath(const Type type, const std::string& file = "");

    }
    
  };

}