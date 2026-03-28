workspace("Softbody Simulation Workspace")
configurations({ "Debug", "Release" })
filter("configurations:Debug")
    symbols "On"
    optimize "Off"
filter("configurations:Release")
    optimize "On"
    symbols "Off"
location("build")
language("C++")
cppdialect("C++20")
architecture("x86_64")

--buildoptions { "-std=c++20" }

project("Softbody Simulation")
kind("ConsoleApp")
location("build/SoftbodySimulation")
files({
	"src/*.cpp",
	"src/*.c",
	"libs/GLAD/*.c",
	"include_dependencies/imgui/*.cpp"
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
