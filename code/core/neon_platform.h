#ifndef NEON_PLATFORM_H
#define NEON_PLATFORM_H

// Disable CRT security warnings
#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#   define _CRT_SECURE_NO_WARNINGS
#endif

// NOTE: DLLEXPORT for preventing name mangling, declspec(dllexport) allows the compiler to generate better dll code
#if defined(_MSC_VER)
#   define DLLEXPORT extern "C" __declspec(dllexport)
#else
#   define DLLEXPORT
#   error DLLEXPORT not implemented for this platform!
#endif

#include <cassert> // assert()
#include <cstdlib>

#include <cstdint>
typedef uint64_t    u64;
typedef uint32_t    u32;
typedef uint16_t    u16;
typedef uint8_t     u8;
typedef int64_t     s64;
typedef int32_t     s32;
typedef int16_t     s16;
typedef int8_t      s8;
typedef float       r32;
typedef double      r64;
typedef intptr_t    iptr;
typedef uintptr_t   uptr;

#define ASSERT(Exp) assert(Exp)
#define ARRAY_COUNT(Array) (sizeof(Array)/sizeof((Array)[0]))
#define KILOBYTE(X) 1024LL * (X)
#define MEGABYTE(X) 1024LL * KILOBYTE(X)
#define GIGABYTE(X) 1024LL * MEGABYTE(X)
#define FORCE_INLINE __forceinline
#define OFFSET_OF(TYPE, MEMBER) ((uintptr_t)&(((TYPE *)0)->MEMBER))
#define MALLOC(Size) Neon__Malloc((Size))
#define MALLOC_STRUCT(Type, Count) ((Type*)(MALLOC(sizeof(Type)*(Count))))
#define SAFE_FREE(x) { if(x) { free(x); (x) = nullptr; } }
#define SAFE_DELETE(x) { delete (x);    (x) = nullptr; }
#define SAFE_DELETEA(x) { delete[] (x); (x) = nullptr; }
#define INVALID_CODE_PATH ASSERT(!"Invalid code path!")
#define INVALID_DEFAULT_CASE default: {ASSERT(!"Invalid default case!");} break
#define MIN(A, B) (((A) < (B)) ? (A) : (B))
#define MAX(A, B) (((A) > (B)) ? (A) : (B))

static u32 neon__COUNTER__ = 0;
#define GEN_ID ++neon__COUNTER__

inline void *Neon__Malloc(size_t Size)
{
    void *Ptr = malloc(Size);
    ASSERT(Ptr);
    return Ptr;
}

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
        game_button_state Buttons[5];
        struct
        {
            game_button_state Up;
            game_button_state Down;
            game_button_state Left;
            game_button_state Right;
            game_button_state Jump;
        };
    };
};

struct game_input
{
    r64 Time; // In seconds
    r64 DeltaTime; // In seconds
    game_mouse_state Mouse;
    game_controller_input Controllers[1];
};

struct file_content
{
    bool NoError;
    s64 Size;
    void *Content;
};

#define PLATFORM_READ_FILE(Name) file_content (Name)(char const *Filename)
typedef PLATFORM_READ_FILE(platform_read_file);

#define PLATFORM_FREE_FILE_CONTENT(Name) void (Name)(file_content *FileContent)
typedef PLATFORM_FREE_FILE_CONTENT(platform_free_file_content);

#define PLATFORM_WRITE_FILE(Name) void (Name)(char const *Filename, u64 BytesToWrite, void *Content)
typedef PLATFORM_WRITE_FILE(platform_write_file);

#define PLATFORM_LOG(Name) void (Name)(char const *Format, ...)
typedef PLATFORM_LOG(platform_log);

#define PLATFORM_LOG_ERROR(Name) void (Name)(char const *Format, ...)
typedef PLATFORM_LOG_ERROR(platform_log_error);

struct platform
{
    s32 WindowWidth;
    s32 WindowHeight;
    platform_read_file *ReadFile;
    platform_free_file_content *FreeFileContent;
    platform_write_file *WriteFile;
    platform_log *Log;
    platform_log_error *LogError;
};
extern platform Platform;
#endif