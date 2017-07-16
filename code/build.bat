@echo off

REM Source Dir
set SourceDir=..\neon\code\

REM set Sources=%SourceDir%neon_sdl.cpp 
REM 		^%SourceDir%neon_texture.cpp
REM 		%SourceDir%neon_renderer.cpp ^
REM			%SourceDir%neon_math.cpp ^

REM Build directory
set BuildDir=..\..\build

REM Comiler Settings
set Macros=-DDEBUG_BUILD=1 ^
	   	   -DDEVELOPER=1 ^
		   -D_CRT_SECURE_NO_WARNINGS

set DisabledWarnings=-wd4201 -wd4100 -wd4189 -wd4505 -wd4127
set CommonCompilerFlags=-MTd -nologo -Gm- -GR- -EHa- -Od -Oi -W4 %DisabledWarnings% %Macros% -FC -Z7 -I ../neon/include
set CommonLinkerFlags=-incremental:no -opt:ref opengl32.lib SDL2main.lib SDL2.lib freetypeMTd.lib

REM Build
IF NOT EXIST %BuildDir% mkdir %BuildDir%
pushd %BuildDir%

REM 64-bit
cl  %CommonCompilerFlags% %SourceDir%neon_game.cpp -LD /link -incremental:no -opt:ref /libpath:..\neon\lib\x64 freetypeMTd.lib opengl32.lib -EXPORT:GameCodeLoaded -EXPORT:GameUpdateAndRender
cl  %CommonCompilerFlags% %SourceDir%neon_sdl.cpp /link -incremental:no -opt:ref /libpath:..\neon\lib\x64 %CommonLinkerFlags%

popd
