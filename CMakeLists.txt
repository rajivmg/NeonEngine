# min version required
cmake_minimum_required(VERSION 3.8)

# name of project
project(NeonEngine)

# set output folder
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

# set include and lib directories
include_directories(${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/code/imgui)
link_directories(${CMAKE_SOURCE_DIR}/lib/x64)

if(WIN32)
    
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4201 /wd4100 /wd4189 /wd4505 /wd4127")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd /DDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT /DNDEBUG")

#   MESSAGE(STATUS "CXX_FLAGS: " ${CMAKE_CXX_FLAGS})
#   MESSAGE(STATUS "DEBUG_FLAGS: " ${CMAKE_CXX_FLAGS_DEBUG})
#   MESSAGE(STATUS "RELEASE_FLAGS: " ${CMAKE_CXX_FLAGS_RELEASE})

    set(GAME_LAYER_LIBS debug freetypeMTd optimized freetypeMT general opengl32 assimp-vc140-mt)
    set(PLATFORM_LAYER_LIBS general SDL2main SDL2 opengl32)

    file(GLOB DLL_FILES "lib/x64/*.dll")
    file(COPY ${DLL_FILES} DESTINATION ${CMAKE_BINARY_DIR}/Release NO_SOURCE_PERMISSIONS)
    file(COPY ${DLL_FILES} DESTINATION ${CMAKE_BINARY_DIR}/Debug NO_SOURCE_PERMISSIONS)

    file(GLOB IMGUI_SRC "code/imgui/imgui.cpp" "code/imgui/imgui_draw.cpp" "code/imgui/imgui_demo.cpp")

# ===Game===
    file(GLOB GAME_SRC "code/game/*.cpp" "code/game/*.h" ${IMGUI_SRC})
    add_library(Game SHARED ${GAME_SRC})
    target_link_libraries(Game ${GAME_LAYER_LIBS})
    set_target_properties(Game PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")

# ===Neon===    
    file(GLOB PLATFORM_SRC "code/platform/*.cpp" "code/platform/*.h" ${IMGUI_SRC})
    add_executable(Neon ${PLATFORM_SRC})
    target_link_libraries(Neon ${PLATFORM_LAYER_LIBS})
    set_target_properties(Neon PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/data")
    set_target_properties(Neon PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")

    # Make Neon depend on Game so we don't need to manually build Game everytime
    add_dependencies(Neon Game)
endif()