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

#define BIT(x) (1 << x)
