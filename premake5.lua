workspace "ROTTR-DRM-Loader"
	architecture "x64"
	configurations {"Debug", "Release"}

project "ROTTR-DRM-Loader"
	kind "SharedLib"

	files {
		"*.hpp",
		"*.cpp",
		"vendor/minhook/src/**",
		"vendor/minhook/include/*.h"
	}
	
	language "C++"

	filter "configurations:Debug"
		defines { "DEBUG", "_DEBUG" }
		symbols "On"

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"
		symbols "On"
