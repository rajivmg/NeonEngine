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

static u32 neon__COUNTER__ = 0; 
#define GEN_ID ++neon__COUNTER__

struct game_button_state
{
	bool EndedDown;
	int HalfTransitionCount;
};

struct game_mouse_state
{
	s32 x;
	s32 y;

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
	r32 FrameTime;
	game_mouse_state Mouse;
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

enum log_level
{
	INFO, WARN, ERR
};

struct read_file_result
{
	u32	 ContentSize;
	void *Content;
};

#define PLATFORM_READ_FILE(Name) read_file_result (Name)(char const *Filename)
typedef PLATFORM_READ_FILE(platform_read_file);
#define PLATFORM_WRITE_FILE(Name) void (Name)(char const *Filename, s32 BytesToWrite, void *FileContent)
typedef PLATFORM_WRITE_FILE(platform_write_file);
#define PLATFORM_FREE_FILE_MEMORY(Name) void (Name)(read_file_result *ReadFileResult)
typedef PLATFORM_FREE_FILE_MEMORY(platform_free_file_memory);
#define PLATFORM_LOG(Name) void (Name)(log_level Level, char const * Fmt, ...)
typedef PLATFORM_LOG(platform_log);

struct platform_t
{
	s32							Width;
	s32							Height;
	platform_read_file			*ReadFile;
	platform_write_file			*WriteFile;
	platform_free_file_memory	*FreeFileMemory;
	platform_log				*Log;
};

extern platform_t *Platform;

#include <imgui.h>

#define GAME_UPDATE_AND_RENDER(Name) void (Name)(game_controller_input *Input)
typedef GAME_UPDATE_AND_RENDER(game_update_and_render);

#define GAME_CODE_LOADED(Name) void (Name)(platform_t *_Platform, ImGuiContext *_ImGuiCtx)
typedef GAME_CODE_LOADED(game_code_loaded);

#define IMGUI_RENDER_DRAW_LISTS(Name) void (Name)(ImDrawData* draw_data)
typedef IMGUI_RENDER_DRAW_LISTS(imgui_render_draw_lists);

#define IMGUI_CREATE_DEVICE_OBJECTS(Name) bool (Name)()
typedef IMGUI_CREATE_DEVICE_OBJECTS(imgui_create_device_objects); 

#define IMGUI_INVALIDATE_DEVICE_OBJECTS(Name) void (Name)()
typedef IMGUI_INVALIDATE_DEVICE_OBJECTS(imgui_invalidate_device_objects);

#endif