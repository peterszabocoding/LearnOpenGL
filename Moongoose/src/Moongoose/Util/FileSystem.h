#pragma once

#include <functional>
#include <filesystem>

namespace Moongoose {

	class FileSystem
	{
	public:
		static bool IsFileExist(const std::filesystem::path& path);
		static bool IsFileExist(const std::string& path);

		static bool MakeDir(const std::filesystem::path& directory);
		static bool MakeDir(const std::string& directory);
	};

}
