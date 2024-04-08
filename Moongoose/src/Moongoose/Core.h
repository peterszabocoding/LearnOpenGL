#pragma once

#include <memory>

#ifdef MG_PLATFORM_WINDOWS
	#ifdef MG_BUILD_DLL
		#define MOONGOOSE_API __declspec(dllexport)
	#else
		#define MOONGOOSE_API __declspec(dllimport)
	#endif
#else
	#error Moongoose only support Windows!
#endif

#define ASSERT_STRINGIFY_MACRO(x) #x

#ifdef MG_ENABLE_ASSERTS
#define MG_ASSERT(x, ...) { if(!(x)) { LOG_APP_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#define MG_CORE_ASSERT(x, ...) { if(!(x)) { LOG_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
#define MG_ASSERT(x, ...)
#define MG_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

template<typename T>
using Scope = std::unique_ptr<T>;

template<typename T>
using Ref = std::shared_ptr<T>;

template<class T>
using Scope = std::unique_ptr<T>;

template<class T>
using Ref = std::shared_ptr<T>;

template<typename T, typename ... Args>
constexpr Scope<T> CreateScope(Args&& ... args)
{
	return std::make_unique<T>(std::forward<Args>(args)...);
}

template<typename T, typename... Args>
constexpr Ref<T> CreateRef(Args&&... args)
{
	return std::make_shared<T>(std::forward<Args>(args)...);
}

template <typename E>
constexpr auto to_underlying(E e) noexcept
{
	return static_cast<std::underlying_type_t<E>>(e);
}