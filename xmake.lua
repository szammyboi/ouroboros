add_rules {
	"mode.debug",
	"mode.release",
	"plugin.compile_commands.autoupdate"
}

target "Simulation"
	set_languages("c++17")
	add_defines "GLFW_INCLUDE_NONE"
	set_kind "binary"
	add_files {
		"./src/*.cpp",
		--"./src/**/*.cpp",
	}
	add_includedirs {
		"./src",
		"./vendor/glfw/include",
		"./vendor/glad/include",
		"./vendor/spdlog/include",
		--"./vendor/entt/single_include/",
		--"./vendor/glm",
		--"./vendor/stb",
		"./vendor/imgui",
		"./vendor",
	}
	add_deps {
		"GLFW",
		"SPDLOG",
		"GLAD",
		"IMGUI"
	}
	set_rundir "$(projectdir)"
	if is_plat("macosx") then
		add_frameworks {
			"OpenGL"
		}
	elseif is_plat("windows") then
		add_links {
			"opengl32",
			"shell32",
			"user32",
			"gdi32",
		}
		add_cxflags("/utf-8", {force=true})
	end

target "IMGUI"
	set_kind "static"
	add_defines "IMGUI_IMPL_OPENGL_LOADER_GLAD"
	add_includedirs {
		"./vendor/imgui",
		"./vendor/glad/include",
		"./vendor/glfw/include"
	}
	add_files {
		"./vendor/imgui/*.cpp",
		"./vendor/imgui/backends/imgui_impl_glfw.cpp",
		"./vendor/imgui/backends/imgui_impl_opengl3.cpp",
		"./vendor/imgui/misc/cpp/*.cpp"
	}

target "GLFW"
	set_kind "static"
	add_headerfiles {
		"./vendor/glfw/include/GLFW/glfw3.h",
		"./vendor/glfw/include/GLFW/glfw3native.h",
		"./vendor/glfw/src/*.h"
	}
	add_files {
		"./vendor/glfw/src/*.c",
	}

	if is_plat("macosx") then
		add_defines "_GLFW_COCOA"
		add_files "./vendor/glfw/src/*.m"
		add_mxflags "-fno-objc-arc"
		add_frameworks {
			"Cocoa",
			"CoreFoundation",
			"IOKit",
			"QuartzCore"
		}
	elseif is_plat("windows") then
		add_defines {
			"_GLFW_WIN32",
			"_CRT_SECURE_NO_WARNINGS"
		}
		add_links "gdi32"
	end

target "GLAD"
	set_kind "static"
	add_defines "GLAD_GLAPI_EXPORT"
	add_headerfiles {
		"./vendor/glad/include/glad/gl.h",
		"./vendor/glad/include/KHR/khrplatform.h"
	}
	add_includedirs "./vendor/glad/include"
	add_files "./vendor/glad/src/gl.c"

target "SPDLOG"
	set_kind "static"
	add_defines "SPDLOG_COMPILED_LIB"
	add_includedirs {
		"./vendor/spdlog/include",
		"./vendor/spdlog/include/spdlog"
	}
	add_files "./vendor/spdlog/src/*.cpp"
	if is_plat("windows") then
		add_cxflags("/utf-8", {force=true})
	end
