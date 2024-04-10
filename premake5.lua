workspace "Moongoose"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"]		=	"Moongoose/vendor/GLFW/include"
IncludeDir["Glad"]		=	"Moongoose/vendor/Glad/include"
IncludeDir["ImGui"]		=	"Moongoose/vendor/imgui"
IncludeDir["GLM"]		=	"Moongoose/vendor/glm"
IncludeDir["ASSIMP"]	=	"Moongoose/vendor/ASSIMP/include"

group "Dependencies"
	include "Moongoose/vendor/GLFW"
	include "Moongoose/vendor/Glad"
	include "Moongoose/vendor/imgui"

group "Engine"
	project "Moongoose"
		location "Moongoose"
		kind "StaticLib"
		language "C++"
		cppdialect "C++17"
		staticruntime "on"

		targetdir ("bin/" .. outputdir .. "/%{prj.name}")
		objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

		pchheader "mgpch.h"
		pchsource "Moongoose/src/mgpch.cpp"

		files
		{
			"%{prj.name}/src/**.h",
			"%{prj.name}/src/**.cpp",
			"%{prj.name}/vendor/glm/glm/**.hpp",
			"%{prj.name}/vendor/glm/glm/**.inl"
		}

		includedirs
		{
			"%{prj.name}/src",
			"%{prj.name}/vendor/spdlog/include",
			"%{IncludeDir.GLFW}",
			"%{IncludeDir.Glad}",
			"%{IncludeDir.GLM}",
			"%{IncludeDir.ImGui}",
			"%{IncludeDir.ASSIMP}"
		}

		libdirs
		{
			"%{prj.name}/vendor/ASSIMP/lib/Debug"
		}

		links 
		{ 
			"GLFW",
			"Glad",
			"ImGui",
			"opengl32.lib",
			"assimp-vc143-mtd.lib"
		}

		filter "system:windows"
			staticruntime "On"
			systemversion "latest"

			defines
			{
				"MG_PLATFORM_WINDOWS",
				"MG_BUILD_DLL",
				"GLFW_INCLUDE_NONE"
			}

		filter "configurations:Debug"
			buildoptions "/MDd"
			symbols "On"

			defines
			{
				"MG_DEBUG",
				"MG_ENABLE_ASSERTS"
			}

		filter "configurations:Release"
			defines "MG_RELEASE"
			buildoptions "/MD"
			optimize "On"

		filter "configurations:Dist"
			defines "MG_DIST"
			buildoptions "/MD"
			optimize "On"

	project "Moongoose-Editor"
		location "Moongoose-Editor"
		kind "ConsoleApp"
		language "C++"
		cppdialect "C++17"
		staticruntime "on"

		targetdir ("bin/" .. outputdir .. "/%{prj.name}")
		objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

		files
		{
			"%{prj.name}/src/**.h",
			"%{prj.name}/src/**.cpp"
		}

		includedirs
		{
			"Moongoose/vendor/spdlog/include",
			"Moongoose/src",
			"Moongoose/vendor",
			"%{IncludeDir.GLM}",
			"%{IncludeDir.Glad}"
		}

		links
		{
			"Moongoose"
		}

		filter "system:windows"
			staticruntime "On"
			systemversion "latest"
			defines
			{
				"MG_PLATFORM_WINDOWS"
			}

		filter "configurations:Debug"
			buildoptions "/MDd"
			symbols "on"
			defines 
			{
				"MG_DEBUG",
				"MG_ENABLE_ASSERTS"
			}

		filter "configurations:Release"
			defines "MG_RELEASE"
			buildoptions "/MD"
			optimize "on"

		filter "configurations:Dist"
			defines "MG_DIST"
			buildoptions "/MD"
			optimize "on"