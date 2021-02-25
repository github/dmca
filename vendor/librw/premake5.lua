newoption {
	trigger		= "gfxlib",
	value       = "LIBRARY",
	description = "Choose a particular development library",
	default		= "glfw",
	allowed		= {
		{ "glfw",	"GLFW" },
		{ "sdl2",	"SDL2" },
	},
}

newoption {
	trigger     = "glfwdir64",
	value       = "PATH",
	description = "Directory of glfw",
	default     = "../glfw-3.3.2.bin.WIN64",
}

newoption {
	trigger     = "glfwdir32",
	value       = "PATH",
	description = "Directory of glfw",
	default     = "../glfw-3.3.2.bin.WIN32",
}

newoption {
	trigger     = "sdl2dir",
	value       = "PATH",
	description = "Directory of sdl2",
	default     = "../SDL2-2.0.8",
}

workspace "librw"
	location "build"
	language "C++"

	configurations { "Release", "Debug" }
	filter { "system:windows" }
		configurations { "ReleaseStatic" }
		platforms { "win-x86-null", "win-x86-gl3", "win-x86-d3d9",
			"win-amd64-null", "win-amd64-gl3", "win-amd64-d3d9" }
	filter { "system:linux" }
		platforms { "linux-x86-null", "linux-x86-gl3",
		"linux-amd64-null", "linux-amd64-gl3",
		"linux-arm-null", "linux-arm-gl3",
		"ps2" }
		if _OPTIONS["gfxlib"] == "sdl2" then
			includedirs { "/usr/include/SDL2" }
		end
	filter {}

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"
	filter "configurations:Release*"
		defines { "NDEBUG" }
		optimize "On"
	filter "configurations:ReleaseStatic"
		flags { "StaticRuntime" }

	filter { "platforms:*null" }
		defines { "RW_NULL" }
	filter { "platforms:*gl3" }
		defines { "RW_GL3" }
		if _OPTIONS["gfxlib"] == "sdl2" then
			defines { "LIBRW_SDL2" }
		end
	filter { "platforms:*d3d9" }
		defines { "RW_D3D9" }
	filter { "platforms:ps2" }
		defines { "RW_PS2" }
		toolset "gcc"
		gccprefix 'ee-'
		buildoptions { "-nostdlib", "-fno-common" }
		includedirs { "$(PS2SDK)/ee/include", "$(PS2SDK)/common/include" }
		optimize "Off"

	filter { "platforms:*amd64*" }
		architecture "x86_64"
	filter { "platforms:*x86*" }
		architecture "x86"
	filter { "platforms:*arm*" }
		architecture "ARM"

	filter { "platforms:win*" }
		system "windows"
	filter { "platforms:linux*" }
		system "linux"

	filter { "platforms:win*gl3" }
		includedirs { path.join(_OPTIONS["sdl2dir"], "include") }
	filter { "platforms:win-x86-gl3" }
		includedirs { path.join(_OPTIONS["glfwdir32"], "include") }
	filter { "platforms:win-amd64-gl3" }
		includedirs { path.join(_OPTIONS["glfwdir64"], "include") }

	filter "action:vs*"
		buildoptions { "/wd4996", "/wd4244" }

	filter { "platforms:win*gl3", "action:not vs*" }
		if _OPTIONS["gfxlib"] == "sdl2" then
			includedirs { "/mingw/include/SDL2" } -- TODO: Detect this properly
		end

	filter {}

	Libdir = "lib/%{cfg.platform}/%{cfg.buildcfg}"
	Bindir = "bin/%{cfg.platform}/%{cfg.buildcfg}"

project "librw"
	kind "StaticLib"
	targetname "rw"
	targetdir (Libdir)
	defines { "LODEPNG_NO_COMPILE_CPP" }
	files { "src/*.*" }
	files { "src/*/*.*" }
	filter { "platforms:*gl3" }
		files { "src/gl/glad/*.*" }

project "dumprwtree"
	kind "ConsoleApp"
	targetdir (Bindir)
	removeplatforms { "*gl3", "*d3d9", "ps2" }
	files { "tools/dumprwtree/*" }
	includedirs { "." }
	libdirs { Libdir }
	links { "librw" }

function findlibs()
	filter { "platforms:linux*gl3" }
		links { "GL" }
		if _OPTIONS["gfxlib"] == "glfw" then
			links { "glfw" }
		else
			links { "SDL2" }
		end
	filter { "platforms:win-amd64-gl3" }
		libdirs { path.join(_OPTIONS["glfwdir64"], "lib-vc2015") }
		libdirs { path.join(_OPTIONS["sdl2dir"], "lib/x64") }
	filter { "platforms:win-x86-gl3" }
		libdirs { path.join(_OPTIONS["glfwdir32"], "lib-vc2015") }
		libdirs { path.join(_OPTIONS["sdl2dir"], "lib/x86") }
	filter { "platforms:win*gl3" }
		links { "opengl32" }
		if _OPTIONS["gfxlib"] == "glfw" then
			links { "glfw3" }
		else
			links { "SDL2" }
		end
	filter { "platforms:*d3d9" }
		links { "gdi32", "d3d9" }
	filter { "platforms:*d3d9", "action:vs*" }
		links { "Xinput9_1_0" }
	filter {}
end

function skeleton()
	files { "skeleton/*.cpp", "skeleton/*.h" }
	files { "skeleton/imgui/*.cpp", "skeleton/imgui/*.h" }
	includedirs { "skeleton" }
end

function skeltool(dir)
	targetdir (Bindir)
	files { path.join("tools", dir, "*.cpp"),
	        path.join("tools", dir, "*.h") }
	vpaths {
		{["src"] = { path.join("tools", dir, "*") }},
		{["skeleton"] = { "skeleton/*" }},
	}
	skeleton()
	debugdir ( path.join("tools", dir) )
	includedirs { "." }
	libdirs { Libdir }
	links { "librw" }
	findlibs()
end

function vucode()
	filter "files:**.dsm"
		buildcommands {
			'cpp "%{file.relpath}" | dvp-as -o "%{cfg.objdir}/%{file.basename}.o"'
		}
		buildoutputs { '%{cfg.objdir}/%{file.basename}.o' }
	filter {}
end

project "playground"
	kind "WindowedApp"
	characterset ("MBCS")
	skeltool("playground")
	flags { "WinMain" }
	removeplatforms { "*null" }
	removeplatforms { "ps2" } -- for now

project "imguitest"
	kind "WindowedApp"
	characterset ("MBCS")
	skeltool("imguitest")
	flags { "WinMain" }
	removeplatforms { "*null" }
	removeplatforms { "ps2" }

project "lights"
	kind "WindowedApp"
	characterset ("MBCS")
	skeltool("lights")
	flags { "WinMain" }
	removeplatforms { "*null" }
	removeplatforms { "ps2" }

project "ska2anm"
	kind "ConsoleApp"
	characterset ("MBCS")
	targetdir (Bindir)
	files { path.join("tools/ska2anm", "*.cpp"),
	        path.join("tools/ska2anm", "*.h") }
	debugdir ( path.join("tools/ska2nm") )
	includedirs { "." }
	libdirs { Libdir }
	links { "librw" }
	findlibs()
	removeplatforms { "*gl3", "*d3d9", "*ps2" }

project "ps2test"
	kind "ConsoleApp"
	targetdir (Bindir)
	vucode()
	removeplatforms { "*gl3", "*d3d9", "*null" }
	targetextension '.elf'
	includedirs { "." }
	files { "tools/ps2test/*.cpp",
	        "tools/ps2test/vu/*.dsm",
	        "tools/ps2test/*.h" }
	libdirs { "$(PS2SDK)/ee/lib" }
	links { "librw" }

--project "ps2rastertest"
--	kind "ConsoleApp"
--	targetdir (Bindir)
--	removeplatforms { "*gl3", "*d3d9" }
--	files { "tools/ps2rastertest/*.cpp" }
--	includedirs { "." }
--	libdirs { Libdir }
--	links { "librw" }
