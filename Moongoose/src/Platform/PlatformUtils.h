#pragma once

#include <string>
#include <functional>

namespace Moongoose {

	class FileDialogs
	{
	public:
		static std::string OpenFile(const char* filter);
		static void OpenFile(const char* filter, const std::function<void(const std::string& filePath)>& onComplete);
		static std::string SaveFile(const char* filter);
	};

}
