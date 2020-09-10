workspace "Kame"
  architecture "x64"

  configurations {
    "Debug",
    "Release",
    "Dist"
  }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "KameEngine/vendor/GLFW/include"
--IncludeDir["Glad"] = "KameEngine/vendor/GLAD/include"
--IncludeDir["ImGui"] = "KameEngine/vendor/imgui"
IncludeDir["glm"] = "KameEngine/vendor/glm"
--IncludeDir["DirectXTex"] = "Kame/vendor/DirectXTex/DirectXTex"
IncludeDir["Vulkan"] = "KameEngine/vendor/vulkan/include"

LibDir = {}
--LibDir["GLFW"] = "KameEngine/vendor/glfw-3.3.2.bin.WIN64/lib-vc2019"
LibDir["Vulkan"] = "KameEngine/vendor/vulkan/lib"

include "KameEngine/vendor/GLFW"
--include "Kame/vendor/Glad"
--include "Kame/vendor/imgui"
--include "Kame/vendor/DirectXTex/DirectXTex"
-- include 

project "KameEngine"
  location "KameEngine"
  kind "StaticLib"
  language "C++"
  cppdialect "C++17"
  staticruntime "on"

  targetdir ("bin/" .. outputdir  .. "/%{prj.name}")
  objdir ("bin-int/" .. outputdir  .. "/%{prj.name}")

  pchheader "kmpch.h"
  pchsource "KameEngine/src/kmpch.cpp"

  files {
    "%{prj.name}/src/**.h",
    "%{prj.name}/src/**.cpp",
    "%{prj.name}/src/**.vert",
    "%{prj.name}/src/**.frag"
  }

  includedirs {
    "%{prj.name}/src",
    --"%{prj.name}/vendor/spdlog/include",
    "%{IncludeDir.GLFW}",
    --"%{IncludeDir.Glad}",
    --"%{IncludeDir.ImGui}",
    "%{IncludeDir.glm}",
    "%{IncludeDir.Vulkan}"
  }

  libdirs {
    "%{LibDir.Vulkan}"
  }
  
  links {
    "GLFW",
    --"Glad",
    --"ImGui",
    --"DirectXTex",
    "vulkan-1.lib"
  }

  filter "system:windows"
    systemversion "latest"

    defines {
      "KAME_PLATFORM_WINDOWS",
      "KAME_BUILD_DLL",
      "GLFW_INCLUDE_NONE",
      "KAME_PLATFORM_WIN32",
      "KAME_PLATFORM_VULKAN"
    }

  filter "configurations:Debug"
    defines "KAME_DEBUG"
    runtime "Debug"
    symbols "on"

  filter "configurations:Release"
    defines "KAME_RELEASE"
    runtime "Release"
    optimize "on"

  filter "configurations:Dist"
    defines "KAME_DIST"
    runtime "Release"
    optimize "on"

  filter { "files:**.hlsl" }
    shadermodel "6.0"
    shadervariablename "g_%{file.basename}"
    --shaderheaderfileoutput "D:/Raftek/Kame/Kame/src/Kame/Platform/DirectX12/Graphics/%{file.basename}.h"
    shaderheaderfileoutput "%{file.directory}/%{file.basename}.h"    
  filter { "files:**_PS.hlsl" }
    shadertype "Pixel"
  filter { "files:**_CS.hlsl" }
    shadertype "Compute"
  filter { "files:**_VS.hlsl" }
    shadertype "Vertex"

project "Sandbox"
  location "Sandbox"
  kind "ConsoleApp"
  language "C++"
  cppdialect "C++17"
  staticruntime "on"

  targetdir ("bin/" .. outputdir  .. "/%{prj.name}")
  objdir ("bin-int/" .. outputdir  .. "/%{prj.name}")

  files {
    "%{prj.name}/src/**.h",
    "%{prj.name}/src/**.cpp",
    "%{prj.name}/src/**.hlsl"
  }

  includedirs {
    --"KameEngine/vendor/spdlog/include",
    "KameEngine/src",
    "%{IncludeDir.GLFW}",
    "%{IncludeDir.glm}",
    "%{IncludeDir.Vulkan}"
  }

  links {
    "KameEngine"
  }

  filter "system:windows"
    systemversion "latest"

    defines {
      "KAME_PLATFORM_WINDOWS"
    }

  filter "configurations:Debug"
    defines "KAME_DEBUG"
    runtime "Debug"
    symbols "on"

  filter "configurations:Release"
    defines "KAME_RELEASE"
    runtime "Release"
    optimize "on"

  filter "configurations:Dist"
    defines "KAME_DIST"
    runtime "Release"
    optimize "on"

  filter { "files:**.hlsl" }
    shadermodel "6.0"
    shadervariablename "g_%{file.basename}"
    shaderheaderfileoutput "D:/Raftek/Kame/Sandbox/src/%{file.basename}.h"
  filter { "files:**_PS.hlsl" }
    shadertype "Pixel"
  filter { "files:**_CS.hlsl" }
    shadertype "Compute"
  filter { "files:**_VS.hlsl" }
    shadertype "Vertex"
