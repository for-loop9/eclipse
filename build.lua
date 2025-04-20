require "ecc/ecc"

workspace "project"
	location "build/makefiles"
	configurations { "debug" }
	architecture "x86_64"

	project "glfw"
		location "build/makefiles"
		kind "staticlib"
		toolset "gcc"
		language "c"
		objdir "build/bin-int/"
		targetdir "build/bin/"

		files {
			"glfw/src/context.c",
			"glfw/src/init.c",
			"glfw/src/input.c",
			"glfw/src/monitor.c",
			"glfw/src/platform.c",
			"glfw/src/vulkan.c",
			"glfw/src/window.c",
			"glfw/src/egl_context.c",
			"glfw/src/osmesa_context.c",
			"glfw/src/null_init.c",
			"glfw/src/null_monitor.c",
			"glfw/src/null_window.c",
			"glfw/src/null_joystick.c",
		}

		defines { 
			"_GLFW_X11",
		}

		files {
			"glfw/src/posix_module.c",
			"glfw/src/posix_time.c",
			"glfw/src/posix_thread.c",
			"glfw/src/xkb_unicode.c",
			"glfw/src/linux_joystick.c",

			"glfw/src/x11_init.c",
			"glfw/src/linux_joystick.c",
			"glfw/src/x11_monitor.c",
			"glfw/src/x11_window.c",
			"glfw/src/glx_context.c",
			"glfw/src/posix_poll.c"
		}
	
		optimize "off"
		symbols "on"
	
	project "ignite"
		location "build/makefiles"
		kind "staticlib"
		toolset "gcc"
		language "c"
		objdir "build/bin-int/"
		targetdir "build/bin/"

		defines {
			"GLFW_INCLUDE_NONE",
			"GLFW_INCLUDE_VULKAN"
		}

		files { "ignite/src/**.c", "ignite/src/**.cpp" }
		includedirs {"glfw/include" }

		optimize "off"
		symbols "on"

	project "app"
		location "build/makefiles"
		kind "consoleapp"
		toolset "gcc"
		language "c"
		objdir "build/bin-int"
		targetdir "build/bin"

		files { "app/src/**.c", "app/src/**.cpp" }
		includedirs { "ignite/src", "glfw/include", "app/include" }

		defines {
			"GLFW_INCLUDE_NONE",
			"GLFW_INCLUDE_VULKAN"
		}

		links { "ignite", "glfw", "vulkan", "stdc++", "m" }

		optimize "off"
		symbols "on"
