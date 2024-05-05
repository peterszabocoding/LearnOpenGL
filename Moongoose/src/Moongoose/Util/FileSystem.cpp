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
