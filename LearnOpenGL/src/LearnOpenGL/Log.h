#pragma once

#include "Core.h"
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include "spdlog/sinks/stdout_color_sinks.h"

namespace LearnOpenGL 
{
	class LEARNOPENGL_API Log {
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() {
			return s_CoreLogger;
		}

		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() {
			return s_ClientLogger;
		}

	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
};

#define LOG_CORE_TRACE(...)			LearnOpenGL::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define LOG_CORE_INFO(...)			LearnOpenGL::Log::GetCoreLogger()->info(__VA_ARGS__)
#define LOG_CORE_WARN(...)			LearnOpenGL::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define LOG_CORE_ERROR(...)			LearnOpenGL::Log::GetCoreLogger()->error(__VA_ARGS__)
#define LOG_CORE_FATAL(...)			LearnOpenGL::Log::GetCoreLogger()->fatal(__VA_ARGS__)

#define LOG_APP_TRACE(...)			LearnOpenGL::Log::GetClientLogger()->trace(__VA_ARGS__)
#define LOG_APP_INFO(...)			LearnOpenGL::Log::GetClientLogger()->info(__VA_ARGS__)
#define LOG_APP_WARN(...)			LearnOpenGL::Log::GetClientLogger()->warn(__VA_ARGS__)
#define LOG_APP_ERROR(...)			LearnOpenGL::Log::GetClientLogger()->error(__VA_ARGS__)
#define LOG_APP_FATAL(...)			LearnOpenGL::Log::GetClientLogger()->fatal(__VA_ARGS__)
