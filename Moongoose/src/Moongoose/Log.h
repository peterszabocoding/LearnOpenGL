#pragma once

#include "Core.h"
#include <spdlog/spdlog.h>
#include "spdlog/sinks/stdout_color_sinks.h"

namespace Moongoose 
{
	class MOONGOOSE_API Log {
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

#define LOG_CORE_TRACE(...)			Log::GetCoreLogger()->trace(__VA_ARGS__)
#define LOG_CORE_INFO(...)			Log::GetCoreLogger()->info(__VA_ARGS__)
#define LOG_CORE_WARN(...)			Log::GetCoreLogger()->warn(__VA_ARGS__)
#define LOG_CORE_ERROR(...)			Log::GetCoreLogger()->error(__VA_ARGS__)
#define LOG_CORE_FATAL(...)			Log::GetCoreLogger()->fatal(__VA_ARGS__)

#define LOG_APP_TRACE(...)			Log::GetClientLogger()->trace(__VA_ARGS__)
#define LOG_APP_INFO(...)			Log::GetClientLogger()->info(__VA_ARGS__)
#define LOG_APP_WARN(...)			Log::GetClientLogger()->warn(__VA_ARGS__)
#define LOG_APP_ERROR(...)			Log::GetClientLogger()->error(__VA_ARGS__)
#define LOG_APP_FATAL(...)			Log::GetClientLogger()->fatal(__VA_ARGS__)
