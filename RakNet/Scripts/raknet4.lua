project "raknet"
	configurations { "Debug", "Release" }
	kind "StaticLib"
	language "C++"
	targetprefix ""

	files { "../Source/**.cpp", 
			"../Source/**.h" }


if ( os.is("windows")) then

	objdir "./"
	flags { "NoRTTI", "No64BitChecks", "NoPCH" }
	
	includedirs
	{ 
		"../Source"
	}
	
	libdirs 
	{ 
	}
	
	configuration "Debug"
		targetname "../Lib/RakNet_d"
		defines { "_CRT_SECURE_NO_DEPRECATE","WIN32","_DEBUG","_RAKNET_DLL","_CRT_NONSTDC_NO_DEPRECATE"}
		links 
		{ 
			"ws2_32"
		}
		
	configuration "Release"
		targetname "../Lib/RakNet"
		defines { "WIN32", "_CRT_SECURE_NO_DEPRECATE","_RELEASE","_RAKNET_DLL", "_CRT_NONSTDC_NO_DEPRECATE"}
		links 
		{
			"ws2_32"
		}
		
else
	objdir "./"
	targetdir "../Lib"
	implibdir "../Lib"

	includedirs
	{ 
		"../Source",
	}
	
	libdirs 
	{ 
--		"../Lib/gnu",
	}
	
	configuration "Debug"
		targetname "raknet_d"
		links
		{
		}
		
	configuration "Release"
		targetname "raknet"
		links 	
		{
		}	
end

