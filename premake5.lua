workspace "LearnOpenGL"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"]		=	"LearnOpenGL/vendor/GLFW/include"
IncludeDir["Glad"]		=	"LearnOpenGL/vendor/Glad/include"
IncludeDir["ImGui"]		=	"LearnOpenGL/vendor/imgui"
IncludeDir["GLM"]		=	"LearnOpenGL/vendor/glm"
IncludeDir["ASSIMP"]	=	"LearnOpenGL/vendor/ASSIMP/include"
IncludeDir["stb"]		=	"LearnOpenGL/vendor/stb/include"
IncludeDir["json"] 		= 	"LearnOpenGL/vendor/json/include"
IncludeDir["ImGuizmo"] 	= 	"LearnOpenGL-Editor/vendor/ImGuizmo"

group "Dependencies"
	include "LearnOpenGL/vendor/GLFW"
	include "LearnOpenGL/vendor/Glad"
	include "LearnOpenGL/vendor/imgui"

group "Engine"
	project "LearnOpenGL"
		location "LearnOpenGL"
		kind "StaticLib"
		language "C++"
		cppdialect "C++17"
		staticruntime "on"

		targetdir ("bin/" .. outputdir .. "/%{prj.name}")
		objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

		pchheader "pch.h"
		pchsource "%{prj.name}/src/pch.cpp"

		files
		{
			"%{prj.name}/src/**.h",
			"%{prj.name}/src/**.cpp",
			"%{prj.name}/vendor/glm/glm/**.hpp",
			"%{prj.name}/vendor/glm/glm/**.inl",
			"%{prj.name}/vendor/json/include/nlohmann/**.hpp",
		}

		includedirs
		{
			"%{prj.name}/src",
			"%{prj.name}/vendor/spdlog/include",
			"%{IncludeDir.GLFW}",
			"%{IncludeDir.Glad}",
			"%{IncludeDir.GLM}",
			"%{IncludeDir.ImGui}",
			"%{IncludeDir.ASSIMP}",
			"%{IncludeDir.stb}",
			"%{IncludeDir.json}"
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

	project "LearnOpenGL-Editor"
		location "LearnOpenGL-Editor"
		kind "ConsoleApp"
		language "C++"
		cppdialect "C++17"
		staticruntime "on"

		targetdir ("bin/" .. outputdir .. "/%{prj.name}")
		objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

		files
		{
			"%{prj.name}/src/**.h",
			"%{prj.name}/src/**.cpp",

			"LearnOpenGL-Editor/vendor/ImGuizmo/ImGuizmo.h",
			"LearnOpenGL-Editor/vendor/ImGuizmo/ImGuizmo.cpp"
		}

		includedirs
		{
			"LearnOpenGL/src",
			"LearnOpenGL/vendor",
			"LearnOpenGL-Editor/src",
			"LearnOpenGL/vendor/spdlog/include",
			"%{IncludeDir.GLM}",
			"%{IncludeDir.Glad}",
			"%{IncludeDir.ImGui}",
			"%{IncludeDir.ImGuizmo}"
		}

		links
		{
			"LearnOpenGL"
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