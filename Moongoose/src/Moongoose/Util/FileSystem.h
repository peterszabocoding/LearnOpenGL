#pragma once

#include <functional>
#include <filesystem>

namespace Moongoose {

	class FileSystem
	{
	public:
		static bool IsFileExist(const std::filesystem::path& path);
		static bool IsFileExist(const std::string& path);
	};

}
