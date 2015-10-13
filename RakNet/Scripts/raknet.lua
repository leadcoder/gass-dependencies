package.name = "raknet"
package.kind = "lib"
package.language = "c++"
package.targetprefix = ""

package.files = 
{ 
	matchrecursive( "../Source/*.cpp", 
			"../Source/*.h" ) 
}




if ( OS == "windows" ) then

	package.objdir = "./"
	package.config["Debug"].target = "../Lib/RakNet_d"
	package.config["Release"].target = "../Lib/RakNet"

	package.buildflags = { "no-rtti", "no-64bit-checks", "no-pch" }
	package.config["Debug"].defines = { "_CRT_SECURE_NO_DEPRECATE","WIN32","_DEBUG","_RAKNET_DLL","_CRT_NONSTDC_NO_DEPRECATE"}
	package.config["Release"].defines = { "WIN32", "_CRT_SECURE_NO_DEPRECATE","_RELEASE","_RAKNET_DLL", "_CRT_NONSTDC_NO_DEPRECATE"}

	package.config["Debug"].links = 
	{ 
		"ws2_32"
	}
	package.config["Release"].links = 
	{
		"ws2_32"
	}
	package.includepaths = 	
	{ 
		"../Source"
		
	}
	
	package.libpaths = 
	{ 
	}
else
	package.objdir = "./"
	package.bindir = "../Lib"
	package.libdir = "../Lib"

	package.config["Debug"].target = "raknet_d"
	package.config["Release"].target = "raknet"

	package.config["Debug"].links =
	{ 
		
	
	}
	package.config["Release"].links = 
	{
		
	}

	package.includepaths = 	
	{ 
		"../Source",
		
	}
	
	package.libpaths = 
	{ 
--		"../Lib/gnu",
		
	}
end

