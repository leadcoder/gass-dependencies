set INSTALL_ROOT=%cd%\gass-dep-install
set SOURCE_ROOT=%cd%

IF DEFINED MSVC_VER GOTO End
@ECHO OFF
CLS
ECHO 1.MSVC 2012
ECHO 2.MSVC 2013
ECHO 3.MSVC 2015
ECHO.

CHOICE /C 123 /M "Enter your choice:"

:: Note - list ERRORLEVELS in decreasing order
IF ERRORLEVEL 3 GOTO MSVC2015
IF ERRORLEVEL 2 GOTO MSVC2013
IF ERRORLEVEL 1 GOTO MSVC2012

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
xcopy x64\Release %INSTALL_ROOT%\tbb\lib\release /I /Y
xcopy x64\Debug %INSTALL_ROOT%\tbb\lib\debug /I /Y
cd %SOURCE_ROOT% 
xcopy tbb\include %INSTALL_ROOT%\tbb\include /I /Y /E

rem -----------------TinyXML-------------------

mkdir build
cd build

mkdir tinyxml2
cd tinyxml2
cmake -DCMAKE_INSTALL_LIBDIR=lib -G"%MSVC_VER%" -DBUILD_SHARED_LIBS=OFF -DCMAKE_INSTALL_PREFIX:PATH=%INSTALL_ROOT%/tinyxml2/ %SOURCE_ROOT%/tinyxml2/
devenv.exe tinyxml2.sln /build "Debug" /project INSTALL /out "build_log_d.txt"
devenv.exe tinyxml2.sln /build "Release" /project INSTALL /out "build_log.txt"
cd ..

pause
