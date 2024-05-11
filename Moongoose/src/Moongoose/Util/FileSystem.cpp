#include "mgpch.h"
#include "FileSystem.h"

bool Moongoose::FileSystem::IsFileExist(const std::filesystem::path& path)
{
	return std::filesystem::exists(path);;
}

bool Moongoose::FileSystem::IsFileExist(const std::string& path)
{
	return std::filesystem::exists(std::filesystem::path(path));
}

bool Moongoose::FileSystem::MakeDir(const std::filesystem::path& directory)
{
	return std::filesystem::create_directories(directory);
}

bool Moongoose::FileSystem::MakeDir(const std::string& directory)
{
	return MakeDir(std::filesystem::path(directory));
}
