rem Following env vars has to be set: 
rem BOOST_HOME for mygui pagedgeom skyx and hydrax
rem FREETYPE_HOME for mygui
rem OGRE_HOME for pagedgeom skyx and hydrax

set INSTALL_ROOT=%cd%\gass-dep-install
set SOURCE_ROOT=%cd%

rem call "%VS140COMNTOOLS%vsvars32.bat"
rem set MSVC_VER=Visual Studio 12 2013 Win64


IF DEFINED MSVC_VER GOTO End
@ECHO OFF
CLS
ECHO 1.MSVC 2010
ECHO 2.MSVC 2012
ECHO 3.MSVC 2013
ECHO 4.MSVC 2015
ECHO.

CHOICE /C 123 /M "Enter your choice:"

:: Note - list ERRORLEVELS in decreasing order
IF ERRORLEVEL 4 GOTO MSVC2015
IF ERRORLEVEL 3 GOTO MSVC2013
IF ERRORLEVEL 2 GOTO MSVC2012
IF ERRORLEVEL 1 GOTO MSVC2010

:MSVC2010
	call "%VS100COMNTOOLS%vsvars32.bat"
	set MSVC_VER=Visual Studio 10 Win64
GOTO End

:MSVC2012
	call "%VS110COMNTOOLS%vsvars32.bat"
	set MSVC_VER=Visual Studio 11 Win64
GOTO End

:MSVC2013
	call "%VS120COMNTOOLS%vsvars32.bat"
	set MSVC_VER=Visual Studio 12 2013 Win64
GOTO End
	
:MSVC2015
	call "%VS140COMNTOOLS%vsvars32.bat"
	set MSVC_VER=Visual Studio 14 2015 Win64
GOTO End
:End
ECHO Selected: %MSVC_VER%
@ECHO ON


rem -----------------TBB-------------------
cd tbb\build\vs2012
devenv.exe makefile.sln /upgrade
devenv.exe makefile.sln /build "Release|x64"  /out "build_log.txt"
devenv.exe makefile.sln /build "Debug|x64"  /out "build_log_d.txt"

rem deploy
xcopy x64\Release %INSTALL_ROOT%\tbb\lib /I /Y
xcopy x64\Debug %INSTALL_ROOT%\tbb\lib /I /Y
cd %SOURCE_ROOT% 
xcopy tbb\include %INSTALL_ROOT%\tbb\include /I /Y /E

mkdir build
cd build

mkdir tinyxml2
cd tinyxml2
cmake -DCMAKE_INSTALL_LIBDIR=lib -G"%MSVC_VER%" -DBUILD_SHARED_LIBS=OFF -DCMAKE_INSTALL_PREFIX:PATH=%INSTALL_ROOT%/tinyxml2/ %SOURCE_ROOT%/tinyxml2/
devenv.exe tinyxml2.sln /build "Debug" /project INSTALL /out "build_log_d.txt"
devenv.exe tinyxml2.sln /build "Release" /project INSTALL /out "build_log.txt"
cd ..

rem prereq env: freetype_home
mkdir mygui
cd mygui

cmake -DCMAKE_INSTALL_LIBDIR=lib -G"%MSVC_VER%" -DMYGUI_BUILD_DEMOS=FALSE -DCMAKE_INSTALL_PREFIX:PATH=%INSTALL_ROOT%/MyGUI_3.2.0/ %SOURCE_ROOT%/MyGUI_3.2.0/
devenv.exe MyGUI.sln /build "Debug" /project INSTALL /out "build_log_d.txt"
devenv.exe MyGUI.sln /build "Release" /project INSTALL /out "build_log.txt"
cd ..

mkdir raknet
cd raknet

cmake -DCMAKE_INSTALL_LIBDIR=lib -G"%MSVC_VER%" -DCMAKE_INSTALL_PREFIX:PATH=%INSTALL_ROOT%/raknet/ %SOURCE_ROOT%/raknet/
devenv.exe RakNetStaticLib.sln /build "Debug" /project INSTALL /out "build_log_d.txt"
devenv.exe RakNetStaticLib.sln /build "Release" /project INSTALL /out "build_log.txt"
cd ..

mkdir ODE
cd ODE
cmake -G"%MSVC_VER%" -DBUILD_SHARED_LIBS=OFF -DCMAKE_INSTALL_PREFIX:PATH=%INSTALL_ROOT%/ODE/ %SOURCE_ROOT%/ODE/
devenv.exe ODE.sln /build "Debug" /project INSTALL /out "build_log_d.txt"
devenv.exe ODE.sln /build "Release" /project INSTALL /out "build_log.txt"
cd ..

mkdir angelscript
cd angelscript
cmake -G"%MSVC_VER%" -DCMAKE_INSTALL_PREFIX:PATH=%INSTALL_ROOT%/angelscript/ %SOURCE_ROOT%/angelscript_2.28.0/angelscript/projects/cmake
devenv.exe angelscript.sln /build "Debug" /project INSTALL /out "build_log_d.txt"
devenv.exe angelscript.sln /build "Release" /project INSTALL /out "build_log.txt"
cd ..


mkdir skyx
cd skyx
cmake -G"%MSVC_VER%" -DBoost_USE_STATIC_LIBS=ON -DSKYX_BUILD_SAMPLES=OFF -DCMAKE_MODULE_PATH=%OGRE_HOME%/cmake/modules -DCMAKE_INSTALL_PREFIX:PATH=%INSTALL_ROOT%/SkyX-v0.4/ %SOURCE_ROOT%/SkyX-v0.4
devenv.exe skyx.sln /build "Debug" /project INSTALL /out "build_log_d.txt"
devenv.exe skyx.sln /build "Release" /project INSTALL /out "build_log.txt"
cd ..


mkdir Hydrax
cd Hydrax
cmake -G"%MSVC_VER%" -DBoost_USE_STATIC_LIBS=ON  -DCMAKE_MODULE_PATH=%OGRE_HOME%/cmake/modules -DCMAKE_INSTALL_PREFIX:PATH=%INSTALL_ROOT%/Hydrax-v0.5.1/ %SOURCE_ROOT%/Hydrax-v0.5.1
devenv.exe Hydrax.sln /build "Debug" /project INSTALL /out "build_log_d.txt"
devenv.exe Hydrax.sln /build "Release" /project INSTALL /out "build_log.txt"
cd ..

mkdir PagedGeometry
cd PagedGeometry
cmake -G"%MSVC_VER%" -DCMAKE_MODULE_PATH=%OGRE_HOME%/cmake/modules -DCMAKE_INSTALL_PREFIX:PATH=%INSTALL_ROOT%/PagedGeometry-1.1.1/ %SOURCE_ROOT%/PagedGeometry-1.1.1
devenv.exe PagedGeometry.sln /build "Debug" /project INSTALL /out "build_log_d.txt"
devenv.exe PagedGeometry.sln /build "Release" /project INSTALL /out "build_log.txt"
cd..

pause
