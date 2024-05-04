local proj_name = "NFSC-Internal"

workspace (proj_name)
    configurations { "Debug", "Release" }
    platforms { "x64", "x86" }

project (proj_name)
    kind "SharedLib"
    language "C++"
	characterset "MBCS"
	
	targetdir("$(SolutionDir)bin")
	objdir("$(SolutionDir)/obj/" .. proj_name)
	
	files { "src/**" }

	externalincludedirs {
	   "$(DXSDK_DIR)Include"
	}

    syslibdirs {
	   "$(DXSDK_DIR)Lib\\x86"
	}

	defines { "_CRT_SECURE_NO_WARNINGS" }
	linkoptions { "/NODEFAULTLIB:LIBCMT /LTCG:OFF" }
    optimize "Size"
    functionlevellinking "On"
    buildoptions { "/std:c++latest" }