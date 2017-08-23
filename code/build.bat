@echo off

REM Build type [Debug=0/Release=1]
set BUILDTYPE=0

REM Source Dir
REM ==========
set SourceDir=..\code\

REM Include Dir
REM ===========
set IncludeDir=..\include

REM Build directory
REM ===============
set BuildDir=..\build

REM Comiler Settings
REM ================
set Macros=-DDEBUG_BUILD=1 ^
	   	   -DDEVELOPER=1 ^
		   -D_CRT_SECURE_NO_WARNINGS

set DisabledWarnings=-wd4201 -wd4100 -wd4189 -wd4505 -wd4127
set CommonCompilerFlags=-nologo -Gm- -GR- -EHsc- -Oi -W4 -FC -Z7 %DisabledWarnings% %Macros% -I %IncludeDir%

REM Start Building
REM ==============

IF NOT EXIST %BuildDir% mkdir %BuildDir%
pushd %BuildDir%

IF %BUILDTYPE%==0 (
echo ::Debug mode build
cl  %CommonCompilerFlags% -MTd -Od %SourceDir%neon_game.cpp -LD /link -incremental:no -opt:ref /libpath:..\lib\x64 freetypeMTd.lib opengl32.lib assimp-vc140-mt.lib -EXPORT:GameCodeLoaded -EXPORT:GameUpdateAndRender
cl  %CommonCompilerFlags% -MTd -Od %SourceDir%neon_sdl.cpp /link -incremental:no -opt:ref /libpath:..\lib\x64 SDL2main.lib SDL2.lib
) ElSE (
echo ::Release mode build
cl  %CommonCompilerFlags% -MT -O2 %SourceDir%neon_game.cpp -LD /link -incremental:no -opt:ref /libpath:..\lib\x64 freetypeMT.lib opengl32.lib assimp-vc140-mt.lib -EXPORT:GameCodeLoaded -EXPORT:GameUpdateAndRender
cl  %CommonCompilerFlags% -MT -O2 %SourceDir%neon_sdl.cpp /link -incremental:no -opt:ref /libpath:..\lib\x64 SDL2main.lib SDL2.lib
)


popd
