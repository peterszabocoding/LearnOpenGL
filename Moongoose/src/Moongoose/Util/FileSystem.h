#pragma once

#include <vector>
#include <filesystem>
#include <stack>

namespace Moongoose {

	class FileSystem
	{
	public:
		static std::string ReadFile(const std::filesystem::path& filepath);

		static bool IsFileExist(const std::filesystem::path& path);
		static bool IsFileExist(const std::string& path);

		static bool MakeDirectory(const std::filesystem::path& directory);
		static bool MakeDirectory(const std::string& directory);
		static bool IsDirectory(const std::filesystem::path& filepath);

		static bool DeleteSingleFile(const std::filesystem::path& filepath);
		static bool MoveSingleFile(const std::filesystem::path& filepath, const std::filesystem::path& dest);
		static bool CopySingleFile(const std::filesystem::path& filepath, const std::filesystem::path& dest);
		static bool RenameSingleFile(const std::filesystem::path& filepath, const std::string& newName);

		static std::vector<std::filesystem::path> GetFilesFromDirectory(const std::filesystem::path& directoryPath);
		static std::vector<std::filesystem::path> GetFilesFromDirectoryDeep(const std::filesystem::path& directoryPath);
		static std::vector<std::filesystem::path> GetFilesFromDirectoryDeep(const std::filesystem::path& directoryPath, const std::string& extensionFilter);
		static std::vector<std::filesystem::path> GetFilesByExtension(const std::filesystem::path& directoryPath, const std::string& extension);

	private:
		static void GetFilesFromDirectoryDeepInner(const std::filesystem::path& directoryPath, std::vector<std::filesystem::path> files, std::stack<std::filesystem::path> directories);
	};

}
