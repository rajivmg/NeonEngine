# min version required
cmake_minimum_required(VERSION 3.8)

# name of project
project(neon)

# set output folder
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

# set include and lib directories
include_directories(${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/include/imgui)
link_directories(${CMAKE_SOURCE_DIR}/lib/x64)

if(WIN32)
	
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	set(CMAKE_CXX_FLAGS "-wd4201 -wd4100 -wd4189 -wd4505 -wd4127")
	set(CMAKE_CXX_FLAGS_DEBUG "-MTd -DDEBUG_BUILD")
	set(CMAKE_CXX_FLAGS_RELEASE "-MT")
	
#	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
#	elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
#	endif()
	
	set(GAME_LAYER_LIBS debug freetypeMTd optimized freetypeMT general opengl32)
	set(PLATFORM_LAYER_LIBS general SDL2main SDL2 opengl32)

	MESSAGE(STATUS "GAME_LAYER_LIBS: " ${GAME_LAYER_LIBS})
	MESSAGE(STATUS "PLATFORM_LAYER_LIBS: " ${PLATFORM_LAYER_LIBS})

	file(GLOB DLL_FILES "lib/x64/*.dll")
	file(COPY ${DLL_FILES} DESTINATION ${CMAKE_BINARY_DIR}/Release NO_SOURCE_PERMISSIONS)
	file(COPY ${DLL_FILES} DESTINATION ${CMAKE_BINARY_DIR}/Debug NO_SOURCE_PERMISSIONS)

	add_executable(neon_sdl code/platform/neon_sdl.cpp)
	target_link_libraries(neon_sdl ${PLATFORM_LAYER_LIBS})

	add_library(neon_game SHARED code/game/neon_game.cpp)
	target_link_libraries(neon_game ${GAME_LAYER_LIBS})

	set_target_properties(neon_sdl PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/data")

endif()

