#ifndef SDL_MAIN_HANDLED
#define SDL_MAIN_HANDLED 1
#endif

#include "neon_platform.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

#include <cstdio>

#if defined(_MSC_VER)
#include <Windows.h>
#endif

#include "neon_sdl.h"

#include <imgui.h>

platform_t *Platform;

static game_code GameCode = {};

// ImGui Data
static r64		imgui_Time = 0.0f;
static bool		imgui_MousePressed[3] = { false, false, false };
static r32		imgui_MouseWheel = 0.0f;
static bool 	imgui_FontTextureCreated = false;

PLATFORM_LOG(Log)
{
	va_list Arguments;
	va_start(Arguments, Fmt);

	switch(Level)
	{
		case INFO:
			SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, Fmt, Arguments);
			break;
		
		case WARN:
			SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_WARN, Fmt, Arguments);
			break;

		case ERR:
		{
			SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, Fmt, Arguments);
			char Temp[200];
			vsnprintf(Temp, 200, Fmt, Arguments);
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", Temp, 0);

			SDL_Event QuitEvent;
			QuitEvent.type = SDL_QUIT;
			SDL_PushEvent(&QuitEvent);
		} break;

		default:
			SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, Fmt, Arguments);
			break;
	}

	va_end(Arguments);
}

PLATFORM_READ_FILE(ReadFile)
{
	read_file_result Result = {};
	SDL_RWops *Fp = SDL_RWFromFile(Filename, "rb");
	assert(Fp != 0);
	if(Fp != 0)
	{
		Platform->Log(INFO, "File: %s read sucessfully.\n", Filename);
		SDL_RWseek(Fp, 0, RW_SEEK_END);
		s64 Temp = (s64)SDL_RWtell(Fp);
		if(Temp != -1L && Temp >= 0)
		{
			Result.ContentSize = (u32)Temp;
		}
		else
		{
			Platform->Log(ERR, "File: %s size is zero.\n", Filename);
		}

		// Set the position indicator to the start of the file
		SDL_RWseek(Fp, 0, RW_SEEK_SET);
		// Allocate the required memory
		Result.Content = (void *)malloc(sizeof(char) * Result.ContentSize);
		if(Result.Content == 0)
		{
			Platform->Log(ERR, "Memory allocation to read %s failed.\n", Filename);
		}

		// Read the file contents
		size_t ContentReadSize = SDL_RWread(Fp, Result.Content, sizeof(char), Result.ContentSize);
		if(ContentReadSize == 0 || ContentReadSize != Result.ContentSize)
		{
			Platform->Log(ERR, "File: %s is empty or read incompletely.\n", Filename);
		}

		SDL_RWclose(Fp);
	}
	else
	{
		Platform->Log(ERR, "File: %s not found or cannot be opened for read.\n", Filename);
	}
	return Result;
}

PLATFORM_FREE_FILE_MEMORY(FreeFileMemory)
{
	free(ReadFileResult->Content);
}

PLATFORM_WRITE_FILE(WriteFile)
{
	SDL_RWops *Fp = SDL_RWFromFile(Filename, "wb");
	assert(Fp != 0);
	if(Fp != 0)
	{
		size_t BytesWritten = SDL_RWwrite(Fp, FileContent, sizeof(u8), BytesToWrite);
		if(BytesWritten != (size_t)BytesToWrite)
		{
			Platform->Log(WARN, "File: %s cannot be written completely.\n", Filename);
		}
		SDL_RWclose(Fp);
		Platform->Log(INFO, "File: %s written sucessfully.\n", Filename);
	}
	else
	{
		Platform->Log(WARN, "File: %s cannot be opened for writing.\n");
	}
}

static const char* ImGui_GetClipboardText(void*)
{
	return SDL_GetClipboardText();
}

static void ImGui_SetClipboardText(void*, const char* text)
{
	SDL_SetClipboardText(text);
}

static bool ImGui_ProcessEvent(SDL_Event* event)
{
	ImGuiIO& io = ImGui::GetIO();
	switch(event->type)
	{
		case SDL_MOUSEWHEEL:
		{
			if(event->wheel.y > 0)
				imgui_MouseWheel = 1;
			if(event->wheel.y < 0)
				imgui_MouseWheel = -1;
			return true;
		}
		case SDL_MOUSEBUTTONDOWN:
		{
			if(event->button.button == SDL_BUTTON_LEFT) imgui_MousePressed[0] = true;
			if(event->button.button == SDL_BUTTON_RIGHT) imgui_MousePressed[1] = true;
			if(event->button.button == SDL_BUTTON_MIDDLE) imgui_MousePressed[2] = true;
			return true;
		}
		case SDL_TEXTINPUT:
		{
			io.AddInputCharactersUTF8(event->text.text);
			return true;
		}
		case SDL_KEYDOWN:
		case SDL_KEYUP:
		{
			int key = event->key.keysym.sym & ~SDLK_SCANCODE_MASK;
			io.KeysDown[key] = (event->type == SDL_KEYDOWN);
			io.KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
			io.KeyCtrl = ((SDL_GetModState() & KMOD_CTRL) != 0);
			io.KeyAlt = ((SDL_GetModState() & KMOD_ALT) != 0);
			io.KeySuper = ((SDL_GetModState() & KMOD_GUI) != 0);
			return true;
		}
	}
	return false;
}

static bool ImGui_Init(SDL_Window* window, imgui_render_draw_lists *ImGui_RenderDrawListsFn)
{
	ImGuiIO& io = ImGui::GetIO();
	io.KeyMap[ImGuiKey_Tab] = SDLK_TAB;                     // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
	io.KeyMap[ImGuiKey_LeftArrow] = SDL_SCANCODE_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = SDL_SCANCODE_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = SDL_SCANCODE_UP;
	io.KeyMap[ImGuiKey_DownArrow] = SDL_SCANCODE_DOWN;
	io.KeyMap[ImGuiKey_PageUp] = SDL_SCANCODE_PAGEUP;
	io.KeyMap[ImGuiKey_PageDown] = SDL_SCANCODE_PAGEDOWN;
	io.KeyMap[ImGuiKey_Home] = SDL_SCANCODE_HOME;
	io.KeyMap[ImGuiKey_End] = SDL_SCANCODE_END;
	io.KeyMap[ImGuiKey_Delete] = SDLK_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = SDLK_BACKSPACE;
	io.KeyMap[ImGuiKey_Enter] = SDLK_RETURN;
	io.KeyMap[ImGuiKey_Escape] = SDLK_ESCAPE;
	io.KeyMap[ImGuiKey_A] = SDLK_a;
	io.KeyMap[ImGuiKey_C] = SDLK_c;
	io.KeyMap[ImGuiKey_V] = SDLK_v;
	io.KeyMap[ImGuiKey_X] = SDLK_x;
	io.KeyMap[ImGuiKey_Y] = SDLK_y;
	io.KeyMap[ImGuiKey_Z] = SDLK_z;

	io.RenderDrawListsFn = ImGui_RenderDrawListsFn;   // Alternatively you can set this to NULL and call ImGui::GetDrawData() after ImGui::Render() to get the same ImDrawData pointer.
	io.SetClipboardTextFn = ImGui_SetClipboardText;
	io.GetClipboardTextFn = ImGui_GetClipboardText;
	io.ClipboardUserData = NULL;

#ifdef _WIN32
	SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version);
	SDL_GetWindowWMInfo(window, &wmInfo);
	io.ImeWindowHandle = wmInfo.info.win.window;
#else
	(void)window;
#endif

	return true;
}

static void ImGui_Shutdown()
{
	GameCode.ImGui_InvalidateDeviceObjects();
	ImGui::Shutdown();
}

static void ImGui_NewFrame(SDL_Window* window)
{
	if(!imgui_FontTextureCreated)
	{
		GameCode.ImGui_CreateDeviceObjects();
		imgui_FontTextureCreated = true;
	}

	ImGuiIO& io = ImGui::GetIO();

	// Setup display size (every frame to accommodate for window resizing)
	int w, h;
	int display_w, display_h;
	SDL_GetWindowSize(window, &w, &h);
	SDL_GL_GetDrawableSize(window, &display_w, &display_h);
	io.DisplaySize = ImVec2((float)w, (float)h);
	io.DisplayFramebufferScale = ImVec2(w > 0 ? ((float)display_w / w) : 0, h > 0 ? ((float)display_h / h) : 0);

	// Setup time step
	Uint32	time = SDL_GetTicks();
	double current_time = time / 1000.0;
	io.DeltaTime = imgui_Time > 0.0 ? (float)(current_time - imgui_Time) : (float)(1.0f / 60.0f);
	imgui_Time = current_time;

	// Setup inputs
	// (we already got mouse wheel, keyboard keys & characters from SDL_PollEvent())
	int mx, my;
	Uint32 mouseMask = SDL_GetMouseState(&mx, &my);
	if(SDL_GetWindowFlags(window) & SDL_WINDOW_MOUSE_FOCUS)
		io.MousePos = ImVec2((float)mx, (float)my);   // Mouse position, in pixels (set to -1,-1 if no mouse / on another screen, etc.)
	else
		io.MousePos = ImVec2(-1.0f, -1.0f);

	io.MouseDown[0] = imgui_MousePressed[0] || (mouseMask & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;		// If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
	io.MouseDown[1] = imgui_MousePressed[1] || (mouseMask & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0;
	io.MouseDown[2] = imgui_MousePressed[2] || (mouseMask & SDL_BUTTON(SDL_BUTTON_MIDDLE)) != 0;
	imgui_MousePressed[0] = imgui_MousePressed[1] = imgui_MousePressed[2] = false;

	io.MouseWheel = imgui_MouseWheel;
	imgui_MouseWheel = 0.0f;

	// Hide OS mouse cursor if ImGui is drawing it
	SDL_ShowCursor(io.MouseDrawCursor ? 0 : 1);

	// Start the frame
	ImGui::NewFrame();
}

static void* LoadFuncFromDLL(void *GameCodeHandle, char const *FuncName)
{
	void *Result = SDL_LoadFunction(GameCodeHandle, FuncName);
	assert(Result != nullptr);
	return Result;
}

static void LoadGameCode(game_code *GameCode)
{
	GameCode->Handle = SDL_LoadObject("Game.dll");
	
	if(!GameCode->Handle)
	{
		Log(ERR, SDL_GetError());
	}

	assert(GameCode->Handle != nullptr);

	GameCode->GameUpdateAndRender = (game_update_and_render *)LoadFuncFromDLL(GameCode->Handle, "GameUpdateAndRender");
	GameCode->GameCodeLoaded = (game_code_loaded *)LoadFuncFromDLL(GameCode->Handle, "GameCodeLoaded");
	GameCode->ImGui_RenderDrawLists = (imgui_render_draw_lists *)LoadFuncFromDLL(GameCode->Handle, "ImGui_RenderDrawLists");
	GameCode->ImGui_CreateDeviceObjects = (imgui_create_device_objects *)LoadFuncFromDLL(GameCode->Handle, "ImGui_CreateDeviceObjects");
	GameCode->ImGui_InvalidateDeviceObjects = (imgui_invalidate_device_objects *)LoadFuncFromDLL(GameCode->Handle, "ImGui_InvalidateDeviceObjects");
}

static void SDLProcessEvents(SDL_Event *Event, game_controller_input *Controller)
{
	switch(Event->type)
	{
		case SDL_KEYDOWN:
		case SDL_KEYUP:
		{
			if(!Event->key.repeat)
			{
				switch(Event->key.keysym.sym)
				{
					case SDLK_UP:
					{
						Controller->Up.EndedDown = Event->key.state == SDL_PRESSED ? true : false;
						++(Controller->Up.HalfTransitionCount);
					} break;

					case SDLK_DOWN:
					{
						Controller->Down.EndedDown = Event->key.state == SDL_PRESSED ? true : false;
						++(Controller->Down.HalfTransitionCount);
					} break;

					case SDLK_LEFT:
					{
						Controller->Left.EndedDown = Event->key.state == SDL_PRESSED ? true : false;
						++(Controller->Left.HalfTransitionCount);
					} break;

					case SDLK_RIGHT:
					{
						Controller->Right.EndedDown = Event->key.state == SDL_PRESSED ? true : false;
						++(Controller->Right.HalfTransitionCount);
					} break;

					case SDLK_ESCAPE:
					{
						SDL_Event QuitEvent;
						QuitEvent.type = SDL_QUIT;
						SDL_PushEvent(&QuitEvent);
					} break;
				}
			}
		} break;

		case SDL_MOUSEMOTION:
		{
			Controller->Mouse.x = Event->motion.x;
			Controller->Mouse.y = Platform->Height - 1 - Event->motion.y;
		} break;

		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
		{
			switch(Event->button.button)
			{
				case SDL_BUTTON_LEFT:
				{
					Controller->Mouse.Left.EndedDown = (Event->button.state == SDL_PRESSED) ? true : false;
					++(Controller->Mouse.Left.HalfTransitionCount);
				} break;

				case SDL_BUTTON_MIDDLE:
				{
					Controller->Mouse.Middle.EndedDown = (Event->button.state == SDL_PRESSED) ? true : false;
					++(Controller->Mouse.Middle.HalfTransitionCount);
				} break;

				case SDL_BUTTON_RIGHT:
				{
					Controller->Mouse.Right.EndedDown = (Event->button.state == SDL_PRESSED) ? true : false;
					++(Controller->Mouse.Right.HalfTransitionCount);
				} break;
			}
		} break;

		case SDL_WINDOWEVENT:
		{
			switch(Event->window.event)
			{
				case SDL_WINDOWEVENT_RESIZED:
				{
					Platform->Width = Event->window.data1;
					Platform->Height = Event->window.data2;
				} break;
			}

		} break;

	}
}

int main(int argc, char **argv)
{
	Platform = (platform_t *)malloc(sizeof(platform_t));
	Platform->Log = &Log;
	Platform->Width = 1280;
	Platform->Height = 720;

	if(SDL_Init(SDL_INIT_VIDEO) == 0)
	{
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 2);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS,1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4); // TODO: Replace MSAA with FXAA 

		// Request SRGB enabled framebuffer
		SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 1);

#if defined(DEBUG)
		// Create opengl debugging context
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif

		SDL_Window *Window;
		Window = SDL_CreateWindow("Neon",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED, Platform->Width, Platform->Height,
			SDL_WINDOW_OPENGL);

		Platform->ReadFile = &ReadFile;
		Platform->WriteFile = &WriteFile;
		Platform->FreeFileMemory = &FreeFileMemory;

		if(Window)
		{
			SDL_GLContext GLContext = SDL_GL_CreateContext(Window);

			if(GLContext)
			{
				SDL_GL_SetSwapInterval(1);

				LoadGameCode(&GameCode);
				GameCode.GameCodeLoaded(Platform, ImGui::GetCurrentContext());

				// Setup Imgui binding
				ImGui_Init(Window, GameCode.ImGui_RenderDrawLists);

				SDL_Event Event;
				bool ShouldQuit = false;

				r32 FrameTime = 0;

				u64 PrevCounter, CurrentCounter, CounterFrequency;
				CounterFrequency = SDL_GetPerformanceFrequency();

				PrevCounter = SDL_GetPerformanceCounter();

				game_controller_input OldInput = {};
				game_controller_input NewInput = {};

				while(!ShouldQuit)
				{
					NewInput = {};

					for(int ButtonIndex = 0; ButtonIndex < ARRAY_COUNT(NewInput.Buttons); ++ButtonIndex)
					{
						NewInput.Buttons[ButtonIndex].EndedDown = OldInput.Buttons[ButtonIndex].EndedDown;
					}

					NewInput.Mouse.x = OldInput.Mouse.x;
					NewInput.Mouse.y = OldInput.Mouse.y;

					for(int ButtonIndex = 0; ButtonIndex < ARRAY_COUNT(NewInput.Mouse.Buttons); ++ButtonIndex)
					{
						NewInput.Mouse.Buttons[ButtonIndex].EndedDown = OldInput.Mouse.Buttons[ButtonIndex].EndedDown;
					}

					while(SDL_PollEvent(&Event))
					{
						if(Event.type == SDL_QUIT)
						{
							ShouldQuit = true;
						}
						else
						{
							ImGui_ProcessEvent(&Event); // Imgui process events
							SDLProcessEvents(&Event, &NewInput);
						}
					}

					// All ImGui rendering after this line
					ImGui_NewFrame(Window);

					ImGui::ShowTestWindow();

					NewInput.dTFrame = FrameTime;

					// Simulate and render the game
					GameCode.GameUpdateAndRender(&NewInput);
					
					// Render ImGui
					ImGui::Render();

					// Swap backbuffer
					SDL_GL_SwapWindow(Window);

					OldInput = NewInput;

					CurrentCounter = SDL_GetPerformanceCounter();
					FrameTime = (r32)((CurrentCounter - PrevCounter)) / CounterFrequency;
					PrevCounter = CurrentCounter;

#if defined(_MSC_VER)
					char DebugCountString[100];
					sprintf(DebugCountString, "%f ms\n", FrameTime*1000.0);
					OutputDebugString(DebugCountString);
#endif
				}
			}
			else
			{
				Platform->Log(ERR, "Failed to create OpenGL context.\n");
			}
		}
		else
		{
			Platform->Log(ERR, "Failed to create a window.\n");
		}

	}
	else
	{
		Platform->Log(ERR, "%s\n", SDL_GetError());
	}

	ImGui_Shutdown();

	SDL_Quit();
	return 0;
}
