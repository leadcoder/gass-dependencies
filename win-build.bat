set INSTALL_ROOT=%cd%/gass-dep-install
set SOURCE_ROOT=%cd%
mkdir build
cd build
call "%VS140COMNTOOLS%vsvars32.bat"

rem set MSVC_VER=Visual Studio 14 2015 Win64
set MSVC_VER=Visual Studio 12 2013 Win64

mkdir tinyxml2
cd tinyxml2
cmake -DCMAKE_INSTALL_LIBDIR=lib -G"%MSVC_VER%" -DBUILD_SHARED_LIBS=OFF -DCMAKE_INSTALL_PREFIX:PATH=%INSTALL_ROOT%/tinyxml2/ %SOURCE_ROOT%/tinyxml2/
devenv.exe tinyxml2.sln /build "Debug" /project INSTALL /out "build_log_d.txt"
devenv.exe tinyxml2.sln /build "Release" /project INSTALL /out "build_log.txt"
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
