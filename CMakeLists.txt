# min version required
cmake_minimum_required(VERSION 3.8)

# name of project
project(NeonEngine)

if(WIN32)

	# set output folder
	set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
	set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
	set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

	set(EXECUTABLE_NAME "Neon" CACHE STRING "Output executable name")

	# set include and lib directories
	#include_directories(${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/code/dear-imgui)
	include_directories(${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/code)
	link_directories(${CMAKE_SOURCE_DIR}/lib/x64)
	
	# set compiler flags
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4201 /wd4100 /wd4189 /wd4505 /wd4127 /D_CRT_SECURE_NO_WARNINGS")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd /DDEBUG")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT /DNDEBUG")

	# copy dlls to executable's folder
	file(GLOB DLL_FILES "lib/x64/*.dll")
	file(COPY ${DLL_FILES} DESTINATION ${CMAKE_BINARY_DIR}/Release NO_SOURCE_PERMISSIONS)
	file(COPY ${DLL_FILES} DESTINATION ${CMAKE_BINARY_DIR}/Debug NO_SOURCE_PERMISSIONS)
	file(COPY ${DLL_FILES} DESTINATION ${CMAKE_BINARY_DIR}/RelWithDebInfo NO_SOURCE_PERMISSIONS)

	# set libs to link
	set(CORE_LIBS general SDL2main SDL2 opengl32 assimp-vc140-mt debug Box2DMTd optimized Box2DMT)

	file(GLOB IMGUI_SRC "code/dear-imgui/imgui.cpp" "code/dear-imgui/imgui_draw.cpp" "code/dear-imgui/imgui_demo.cpp")
	file(GLOB CORE_SRC "code/core/neon_*.cpp" "code/core/neon_*.h")
	file(GLOB NEON_SRC "code/core/neon.cpp" "code/core/neon.h")
	file(GLOB GAME_SRC "code/game/*.cpp" "code/game/*.h")
	add_executable(Neon ${NEON_SRC} ${CORE_SRC} ${GAME_SRC} ${IMGUI_SRC})
	target_link_libraries(Neon ${CORE_LIBS})
	set_target_properties(Neon PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/data")
	set_target_properties(Neon PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
	set_target_properties(Neon PROPERTIES OUTPUT_NAME ${EXECUTABLE_NAME})

endif()