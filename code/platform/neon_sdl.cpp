#include "neon_platform.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

#include <cstdio> // vsnprintf

#include "neon_sdl.h"

#include <imgui.h>

platform_t Platform;

static game_code GameCode = {};

// ImGui Data
static r64		imgui_Time = 0.0f;
static bool		imgui_MousePressed[3] = { false, false, false };
static r32		imgui_MouseWheel = 0.0f;
static bool 	imgui_FontTextureCreated = false;

PLATFORM_LOG(Log)
{
	va_list ArgList;
	va_start(ArgList, Format);

	SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, Format, ArgList);
	
	va_end(ArgList);
}

PLATFORM_LOG_ERROR(LogError)
{
	va_list ArgList;
	va_start(ArgList, Format);

	char Buffer[2048];
	vsnprintf(Buffer, 2048, Format, ArgList);
	SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, Format, ArgList);
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "ERROR", Buffer, 0);

	va_end(ArgList);

	// After logging error quit
	SDL_Event QuitE;
	QuitE.type = SDL_QUIT;
	SDL_PushEvent(&QuitE);
}

PLATFORM_READ_FILE(ReadFile)
{
	file_content Result = {};

	// Open file
	SDL_RWops *Fp = SDL_RWFromFile(Filename, "rb");
	
	// If file is opened
	if(Fp != 0)
	{
		// Seek the FP at end
		SDL_RWseek(Fp, 0, RW_SEEK_END);

		// Calculate file size
		Result.Size = (s64)SDL_RWtell(Fp);

		// Check if the file size is valid
		if(Result.Size == -1 || Result.Size <= 0)
		{
			Result.NoError = false;
		}

		// Set the FP to the start of the file
		SDL_RWseek(Fp, 0, RW_SEEK_SET);

		// Allocate the required memory
		Result.Content = (void *)malloc(sizeof(char) * Result.Size);
		if(Result.Content == nullptr)
		{
			Result.NoError = false;
		}

		// Read the file contents
		size_t ContentReadSize = SDL_RWread(Fp, Result.Content, sizeof(char), Result.Size);
		if(ContentReadSize != Result.Size)
		{
			Result.NoError = false;
		}

		// Close FP
		SDL_RWclose(Fp);

		Result.NoError = true;
	}
	// If file can't be opened
	else
	{
		Platform.Log("Can't open file %s for reading\n", Filename);
		Result.NoError = false;
	}

	return Result;
}

PLATFORM_FREE_FILE_CONTENT(FreeFileContent)
{
	SAFE_FREE(FileContent->Content);
}

PLATFORM_WRITE_FILE(WriteFile)
{
	// Open file for writing
	SDL_RWops *Fp = SDL_RWFromFile(Filename, "wb");

	// If file is opened
	if(Fp != 0)
	{
		size_t BytesWritten = SDL_RWwrite(Fp, Content, sizeof(u8), BytesToWrite);
		if(BytesWritten != (size_t)BytesToWrite)
		{
			Platform.Log("Can't write file %s completely\n", Filename);
		}

		SDL_RWclose(Fp);
	}
	// If file can't be opened
	else
	{
		Platform.Log("Can't open file %s for writing\n", Filename);
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
		Log(SDL_GetError());
	}

	assert(GameCode->Handle != nullptr);

	GameCode->GameUpdateAndRender = (game_update_and_render *)LoadFuncFromDLL(GameCode->Handle, "GameUpdateAndRender");
	GameCode->GameSetup = (game_setup *)LoadFuncFromDLL(GameCode->Handle, "GameSetup");
	GameCode->ImGui_RenderDrawLists = (imgui_render_draw_lists *)LoadFuncFromDLL(GameCode->Handle, "ImGui_RenderDrawLists");
	GameCode->ImGui_CreateDeviceObjects = (imgui_create_device_objects *)LoadFuncFromDLL(GameCode->Handle, "ImGui_CreateDeviceObjects");
	GameCode->ImGui_InvalidateDeviceObjects = (imgui_invalidate_device_objects *)LoadFuncFromDLL(GameCode->Handle, "ImGui_InvalidateDeviceObjects");
}

static void SDLProcessButtonState(game_button_state *NewState, bool IsDown)
{
	if(NewState->EndedDown != IsDown)
	{
		NewState->EndedDown = IsDown;
		++NewState->HalfTransitionCount;
	}
}

static void SDLProcessEvents(SDL_Event *Event, game_input *Input)
{
	game_controller_input *Controller = &Input->Controllers[0];

	switch(Event->type)
	{
		case SDL_KEYDOWN:
		case SDL_KEYUP:
		{
			if(!Event->key.repeat)
			{
				bool IsDown = (Event->key.state == SDL_PRESSED);
				switch(Event->key.keysym.sym)
				{
					case SDLK_w:
					case SDLK_UP:
					{
						SDLProcessButtonState(&Controller->Up, IsDown);
					} break;

					case SDLK_s:
					case SDLK_DOWN:
					{
						SDLProcessButtonState(&Controller->Down, IsDown);
					} break;

					case SDLK_a:
					case SDLK_LEFT:
					{
						SDLProcessButtonState(&Controller->Left, IsDown);
					} break;

					case SDLK_d:
					case SDLK_RIGHT:
					{
						SDLProcessButtonState(&Controller->Right, IsDown);
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
			Input->Mouse.x = Event->motion.x;
			Input->Mouse.y = Platform.Height - 1 - Event->motion.y;
			Input->Mouse.xrel = Event->motion.xrel;
			Input->Mouse.yrel = -Event->motion.yrel;
		} break;

		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
		{
			bool IsDown = (Event->button.state == SDL_PRESSED);
			switch(Event->button.button)
			{
				case SDL_BUTTON_LEFT:
				{
					SDLProcessButtonState(&Input->Mouse.Left, IsDown);
				} break;

				case SDL_BUTTON_MIDDLE:
				{
					SDLProcessButtonState(&Input->Mouse.Middle, IsDown);
				} break;

				case SDL_BUTTON_RIGHT:
				{
					SDLProcessButtonState(&Input->Mouse.Right, IsDown);
				} break;
			}
		} break;

		case SDL_WINDOWEVENT:
		{
			switch(Event->window.event)
			{
				case SDL_WINDOWEVENT_RESIZED:
				{
					Platform.Width = Event->window.data1;
					Platform.Height = Event->window.data2;
				} break;
			}

		} break;

	}
}

int main(int argc, char **argv)
{
	Platform.Log = &Log;
	Platform.LogError = &LogError;
	Platform.Width = 1280;
	Platform.Height = 720;

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
			SDL_WINDOWPOS_UNDEFINED, Platform.Width, Platform.Height,
			SDL_WINDOW_OPENGL);

		Platform.ReadFile = &ReadFile;
		Platform.WriteFile = &WriteFile;
		Platform.FreeFileContent = &FreeFileContent;

		if(Window)
		{
			SDL_GLContext GLContext = SDL_GL_CreateContext(Window);

			if(GLContext)
			{
				SDL_GL_SetSwapInterval(1);

				LoadGameCode(&GameCode);
				GameCode.GameSetup(Platform, ImGui::GetCurrentContext());

				// Setup Imgui binding
				ImGui_Init(Window, GameCode.ImGui_RenderDrawLists);

				SDL_Event Event;
				bool ShouldQuit = false;

				r32 Time = 0;
				r32 FrameTime = 0;

				u64 PrevCounter, CurrentCounter, CounterFrequency;
				CounterFrequency = SDL_GetPerformanceFrequency();

				PrevCounter = SDL_GetPerformanceCounter();

				game_input Input[2] = {};
				game_input *OldInput = &Input[0];
				game_input *NewInput = &Input[1];

				while(!ShouldQuit)
				{
					*NewInput = {};

					// Copy old keyboard input state
					game_controller_input *OldKeyboardController = &OldInput->Controllers[0];
					game_controller_input *NewKeyboardController = &NewInput->Controllers[0];
					for(int ButtonIndex = 0;
						ButtonIndex < ARRAY_COUNT(NewKeyboardController->Buttons);
						++ButtonIndex)
					{
						NewKeyboardController->Buttons[ButtonIndex].EndedDown =
							OldKeyboardController->Buttons[ButtonIndex].EndedDown;
					}

					// Copy old mouse input state
					NewInput->Mouse.x = OldInput->Mouse.x;
					NewInput->Mouse.y = OldInput->Mouse.y;
					for(int ButtonIndex = 0;
						ButtonIndex < ARRAY_COUNT(NewInput->Mouse.Buttons);
						++ButtonIndex)
					{
						NewInput->Mouse.Buttons[ButtonIndex].EndedDown =
							OldInput->Mouse.Buttons[ButtonIndex].EndedDown;
					}

					/*for(int ControllerIndex = 0; ControllerIndex < ARRAY_COUNT(NewInput->Controllers);
					++ControllerIndex)
					{

					}*/

					while(SDL_PollEvent(&Event))
					{
						if(Event.type == SDL_QUIT)
						{
							ShouldQuit = true;
						}
						else
						{
							ImGui_ProcessEvent(&Event); // Imgui process events
							SDLProcessEvents(&Event, NewInput);
						}
					}

					// All ImGui rendering after this line
					ImGui_NewFrame(Window);

					ImGui::ShowTestWindow();

					// Game running time
					NewInput->Time = SDL_GetTicks(); //(r32)(SDL_GetPerformanceCounter() / CounterFrequency);

					NewInput->FrameTime = FrameTime;

					// Simulate and render the game
					GameCode.GameUpdateAndRender(NewInput);
					
					// Render ImGui
					ImGui::Render();

					// Swap backbuffer
					SDL_GL_SwapWindow(Window);

					*OldInput = *NewInput;

					CurrentCounter = SDL_GetPerformanceCounter();
					FrameTime = (r32)((CurrentCounter - PrevCounter)) / CounterFrequency;
					PrevCounter = CurrentCounter;
				}
			}
			else
			{
				Platform.Log("Failed to create OpenGL context.\n");
			}
		}
		else
		{
			Platform.Log("Failed to create a window.\n");
		}

	}
	else
	{
		Platform.Log("%s\n", SDL_GetError());
	}

	ImGui_Shutdown();

	SDL_Quit();
	return 0;
}
