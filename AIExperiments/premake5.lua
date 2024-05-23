workspace "AIExperiments"
	architecture "x64"
	
	configurations
	{
		"Debug",
		"Release"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "AIExperiments"
	location "AIExperiments"
	kind "ConsoleApp"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"vendor/IMGUI/include",
		"vendor/IMGUI_SFML/include",
		"vendor/SFML/include"
	}

	libdirs 
	{ 
		"vendor/SFML/lib",  
	}

	defines
	{
		"SFML_STATIC"
	}

	filter "system:windows"
		cppdialect "C++20"
		systemversion "latest"

		defines
		{
			"GE_WIN"
		}

	filter "configurations:Debug"
		defines "GE_DEBUG"
		symbols "On"

		links
		{
			"sfml-graphics-s-d",
			"sfml-window-s-d",
			"sfml-system-s-d",
			"sfml-audio-s-d",
			"flac",
			"vorbisenc",
			"vorbisfile",
			"vorbis",
			"ogg",
			"openal32",
			"opengl32",
			"freetype",
			"winmm"
		}

	filter "configurations:Release"
		defines "GE_RELEASE"
		optimize "On"

		links
		{
			"sfml-graphics-s",
			"sfml-window-s",
			"sfml-system-s",
			"sfml-audio-s",
			"flac",
			"vorbisenc",
			"vorbisfile",
			"vorbis",
			"ogg",
			"openal32",
			"opengl32",
			"freetype",
			"winmm"
		}