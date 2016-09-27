project.name = "RakNet"
package = newpackage()
package.name = "RakNet"
package.kind = "lib"
package.language = "c++"
package.targetprefix = ""

package.config["Debug"].target = "RakNetLibStaticDebug"
package.config["Release"].target = "RakNetLibStatic"

package.libdir = "Lib"
package.bindir = "Lib"

package.files = { matchrecursive( "Source/*.cpp", "Source/*.h") }

table.insert( package.defines, "_CRT_SECURE_NO_WARNINGS" )
package.includepaths = { "Source" }

package.links = { "ws2_32" }