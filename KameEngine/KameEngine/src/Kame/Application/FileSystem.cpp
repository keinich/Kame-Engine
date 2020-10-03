#include "kmpch.h"
#include "FileSystem.h"

const std::string Kame::FileSystem::Path::GetPath(const Type type, const std::string& file) {
  // TODO
  switch (type) {
  case Kame::FileSystem::Path::Type::Assets:
    return "D:/Raftek/Kame2/KameEngine/KameEngine/Resources/" + file;
    break;
  case Kame::FileSystem::Path::Type::VulkanShaders:
    return "D:/Raftek/Kame2/KameEngine/KameEngine/src/Kame/Graphics/GraphicsApi/Vulkan/Shaders/" + file;
    break;
  }
}
