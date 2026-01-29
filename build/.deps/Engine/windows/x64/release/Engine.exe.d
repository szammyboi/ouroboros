{
    files = {
        [[build\.objs\Engine\windows\x64\release\src\main.cpp.obj]],
        [[build\windows\x64\release\IMGUI.lib]],
        [[build\windows\x64\release\GLAD.lib]],
        [[build\windows\x64\release\SPDLOG.lib]],
        [[build\windows\x64\release\GLFW.lib]]
    },
    values = {
        [[C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.40.33807\bin\HostX64\x64\link.exe]],
        {
            "-nologo",
            "-dynamicbase",
            "-nxcompat",
            "-machine:x64",
            [[-libpath:build\windows\x64\release]],
            "/opt:ref",
            "/opt:icf",
            "opengl32.lib",
            "shell32.lib",
            "user32.lib",
            "GLFW.lib",
            "gdi32.lib",
            "SPDLOG.lib",
            "GLAD.lib",
            "IMGUI.lib"
        }
    }
}