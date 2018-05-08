#ifndef NEON_PLATFORM_H
#define NEON_PLATFORM_H

// Disable CRT security warnings
#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#	define _CRT_SECURE_NO_WARNINGS
#endif

// DLLEXPORT for preventing name mangling, declspec(dllexport) allows the compiler to generate better code
#if defined(_MSC_VER)
#	define DLLEXPORT extern "C" __declspec(dllexport)
#else
#	define DLLEXPORT
#	error DLLEXPORT not implemented for this platform!
#endif

// assert functions
#include <cassert>

#include <cstdint>
typedef uint64_t	u64;
typedef uint32_t	u32;
typedef uint16_t	u16;
typedef uint8_t		u8;
typedef int64_t		s64;
typedef int32_t		s32;
typedef int16_t		s16;
typedef int8_t		s8;
typedef float 		r32;
typedef double		r64;

#define ARRAY_COUNT(Array) (sizeof(Array)/sizeof((Array)[0]))
#define KILOBYTE(X) 1024LL * (X)
#define MEGABYTE(X) 1024LL * KILOBYTE(X)
#define GIGABYTE(X) 1024LL * MEGABYTE(X)
#define FORCE_INLINE __forceinline
#define OFFSET_OF(TYPE, MEMBER) (uintptr_t)&(((TYPE *)0)->MEMBER)
#define SAFE_FREE(x) { if(x) { free(x);	(x) = nullptr; } }
#define SAFE_DELETE(x) { delete (x);	(x) = nullptr; }
#define SAFE_DELETEA(x) { delete[] (x);	(x) = nullptr; }
#define INVALID_CODE_PATH assert(!"Invalid code path!")
#define INVALID_DEFAULT_CASE default: {assert(!"Invalid default case!");} break
#define MIN(A, B) ((A < B) ? (A) : (B))
#define MAX(A, B) ((A > B) ? (A) : (B))

static u32 neon__COUNTER__ = 0; 
#define GEN_ID ++neon__COUNTER__

#define local_persist static

struct game_button_state
{
	bool EndedDown;
	int HalfTransitionCount;
};

struct game_mouse_state
{
	s32 x;
	s32 y;
	s32 xrel;
	s32 yrel;

	union
	{
		game_button_state Buttons[3];
		struct
		{
			game_button_state Left;
			game_button_state Middle;
			game_button_state Right;
		};
	};	
};

struct game_controller_input
{
	union
	{
		game_button_state Buttons[4];
		struct
		{
			game_button_state Up;
			game_button_state Down;
			game_button_state Left;
			game_button_state Right;
		};
	};
};

struct game_input
{
	r32 Time;								//
	r32 FrameTime;							// In seconds
	game_mouse_state Mouse;
	game_controller_input Controllers[1];
};

struct file_content
{
	bool NoError;
	s64	Size;
	void *Content;
};

#define PLATFORM_READ_FILE(Name) file_content (Name)(char const *Filename)
typedef PLATFORM_READ_FILE(platform_read_file);

#define PLATFORM_FREE_FILE_CONTENT(Name) void (Name)(file_content *FileContent)
typedef PLATFORM_FREE_FILE_CONTENT(platform_free_file_content);

#define PLATFORM_WRITE_FILE(Name) void (Name)(char const *Filename, s32 BytesToWrite, void *Content)
typedef PLATFORM_WRITE_FILE(platform_write_file);

#define PLATFORM_LOG(Name) void (Name)(char const *Format, ...)
typedef PLATFORM_LOG(platform_log);

#define PLATFORM_LOG_ERROR(Name) void (Name)(char const *Format, ...)
typedef PLATFORM_LOG_ERROR(platform_log_error);

struct platform_t
{
	s32	Width;
	s32	Height;
	platform_read_file *ReadFile;
	platform_free_file_content *FreeFileContent;
	platform_write_file	*WriteFile;
	platform_log *Log;
	platform_log_error *LogError;
};

extern platform_t Platform;

#include <imgui.h>

#define GAME_SETUP(Name) void (Name)(platform_t _Platform, ImGuiContext *_ImGuiCtx)
typedef GAME_SETUP(game_setup);

#define GAME_UPDATE_AND_RENDER(Name) void (Name)(game_input *Input)
typedef GAME_UPDATE_AND_RENDER(game_update_and_render);

#define IMGUI_RENDER_DRAW_LISTS(Name) void (Name)(ImDrawData* draw_data)
typedef IMGUI_RENDER_DRAW_LISTS(imgui_render_draw_lists);

#define IMGUI_CREATE_DEVICE_OBJECTS(Name) bool (Name)()
typedef IMGUI_CREATE_DEVICE_OBJECTS(imgui_create_device_objects); 

#define IMGUI_INVALIDATE_DEVICE_OBJECTS(Name) void (Name)()
typedef IMGUI_INVALIDATE_DEVICE_OBJECTS(imgui_invalidate_device_objects);

#endif