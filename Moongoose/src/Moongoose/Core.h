#pragma once

#ifdef MG_PLATFORM_WINDOWS
	#ifdef MG_BUILD_DLL
		#define MOONGOOSE_API __declspec(dllexport)
	#else
		#define MOONGOOSE_API __declspec(dllimport)
	#endif
#else
	#error Moongoose only support Windows!
#endif

#ifdef MG_ENABLE_ASSERTS
#define MG_ASSERT(x, ...) { if(!(x)) { LOG_APP_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#define MG_CORE_ASSERT(x, ...) { if(!(x)) { LOG_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
#define MG_ASSERT(x, ...)
#define MG_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)
