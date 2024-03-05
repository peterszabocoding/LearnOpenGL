#pragma once

#ifdef MG_PLATFORM_WINDOWS
	#ifdef MG_BUILD_DLL
		#define MOONGOOSE_API __declspec(dllexport)
	#else
		#define MOONGOOSE_API __declspec(dllimport)
	#endif
#else
	#error MoonGoose only support Windows!
#endif