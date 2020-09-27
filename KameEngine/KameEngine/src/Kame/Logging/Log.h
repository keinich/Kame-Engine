#pragma once

//#include <Kame/Core.h>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace Kame {

  class Log {

  public:
    static void Init();

    inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
    inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

  private:
    static std::shared_ptr<spdlog::logger> s_CoreLogger;
    static std::shared_ptr<spdlog::logger> s_ClientLogger;

  };

}

// Core log macros
#define KM_CORE_FATAL(...)  ::Kame::Log::GetCoreLogger()->fatal(__VA_ARGS__)
#define KM_CORE_ERROR(...)  ::Kame::Log::GetCoreLogger()->error(__VA_ARGS__)
#define KM_CORE_WARN(...)   ::Kame::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define KM_CORE_INFO(...)   ::Kame::Log::GetCoreLogger()->info(__VA_ARGS__)
#define KM_CORE_TRACE(...)  ::Kame::Log::GetCoreLogger()->trace(__VA_ARGS__)

// Core log macros
#define KM_FATAL(...)       ::Kame::Log::GetClientLogger()->fatal(__VA_ARGS__)
#define KM_ERROR(...)       ::Kame::Log::GetClientLogger()->error(__VA_ARGS__)
#define KM_WARN(...)        ::Kame::Log::GetClientLogger()->warn(__VA_ARGS__)
#define KM_INFO(...)        ::Kame::Log::GetClientLogger()->info(__VA_ARGS__)
#define KM_TRACE(...)       ::Kame::Log::GetClientLogger()->trace(__VA_ARGS__)