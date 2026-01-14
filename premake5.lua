workspace("Softbody Simulation Workspace")
configurations({ "Debug", "Release" })
location("build")
language("C++")
cppdialect("C++23")
architecture("x86_64")

project("Softbody Simulation")
kind("ConsoleApp")
location("build/SoftbodySimulation")
files({
	"src/*.cpp",
	"src/*.c",
	"libs/GLAD/*.c",
})
includedirs("include")
includedirs("include_dependencies")
libdirs({ "libs/GLFW" })
links("glfw3")

filter("system:linux")
links({ "dl", "pthread", "X11" }) -- GLFW-Abhängigkeiten
filter({})

filter({ "system:windows" })
links({ "OpenGL32" })
filter({ "system:not windows" })
links({ "GL" })
