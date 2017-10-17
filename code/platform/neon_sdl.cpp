/*===============================================
=            NEON SDL Platform Layer            =
===============================================*/

#ifndef SDL_MAIN_HANDLED
#define SDL_MAIN_HANDLED 1
#endif

#include <SDL2/SDL.h>

#include "neon_platform.h"

#include <cstdio>

#if defined(WINDOWS)
#include <Windows.h>
#endif

#include "neon_sdl.h"

#include <imgui.h>
#include "imgui_impl.cpp"

platform_t *Platform;

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
		}
			break;
		default:
			SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, Fmt, Arguments);
			break;
	}

	va_end(Arguments);
}

PLATFORM_READ_FILE(ReadFile)
{
	read_file_result Result = {};
	//FILE *Fp;
	SDL_RWops *Fp = SDL_RWFromFile(Filename, "rb");
	//Fp = fopen(Filename, "rb");
	Assert(Fp != 0);
	if(Fp != 0)
	{
		Platform->Log(INFO, "File: %s read sucessfully.\n", Filename);
		//fseek(Fp, 0, SEEK_END);
		SDL_RWseek(Fp, 0, RW_SEEK_END);
		// long Temp = ftell(Fp);
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
		//fseek(Fp, 0, SEEK_SET);
		SDL_RWseek(Fp, 0, RW_SEEK_SET);
		// Allocate the required memory
		Result.Content = (void *)malloc(sizeof(char) * Result.ContentSize);
		if(Result.Content == 0)
		{
			Platform->Log(ERR, "Memory allocation to read %s failed.\n", Filename);
		}

		// Read the file contents
		// size_t ContentReadSize = fread(Result.Content, sizeof(char), Result.ContentSize, Fp);
		size_t ContentReadSize = SDL_RWread(Fp, Result.Content, sizeof(char), Result.ContentSize);
		if(ContentReadSize == 0 || ContentReadSize != Result.ContentSize)
		{
			Platform->Log(ERR, "File: %s is empty or read incompletely.\n", Filename);
		}
		// fclose(Fp);	
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
	// FILE *Fp;
	// Fp = fopen(Filename, "wb");
	SDL_RWops *Fp = SDL_RWFromFile(Filename, "wb");
	Assert(Fp != 0);
	if(Fp != 0)
	{
		// size_t BytesWritten = fwrite(FileContent, sizeof(u8), BytesToWrite, Fp);
		size_t BytesWritten = SDL_RWwrite(Fp, FileContent, sizeof(u8), BytesToWrite);
		if(BytesWritten != (size_t)BytesToWrite)
		{
			Platform->Log(WARN, "File: %s cannot be written completely.\n", Filename);
		}
		// fclose(Fp);
		SDL_RWclose(Fp);
		Platform->Log(INFO, "File: %s written sucessfully.\n", Filename);
	}
	else
	{ 
		Platform->Log(WARN, "File: %s cannot be opened for writing.\n");
	}	
}

void LoadGameCode(game_code *GameCode)
{	
	GameCode->Handle = SDL_LoadObject("neon_game.dll");
	
	if(GameCode->Handle == 0)
	{
		Log(ERR, SDL_GetError());
	}

	GameCode->GameUpdateAndRender = (game_update_and_render *)SDL_LoadFunction(GameCode->Handle, "GameUpdateAndRender");
	GameCode->GameCodeLoaded = (game_code_loaded *)SDL_LoadFunction(GameCode->Handle, "GameCodeLoaded");
}

void SDLProcessEvents(SDL_Event *Event, game_controller_input *Controller)
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
			// Controller->Mouse.Y = Event->motion.y;
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
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,3);
		// compatiblity profile for immediate mode debugging draws
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE); //SDL_GL_CONTEXT_PROFILE_CORE
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 2);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 1);

		// request SRGB enabled framebuffer
		SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 1);
		// create opengl debugging context
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

		SDL_Window *Window;
		Window = SDL_CreateWindow("Neon God",
									SDL_WINDOWPOS_UNDEFINED,
									SDL_WINDOWPOS_UNDEFINED, 1280, 720,
									SDL_WINDOW_OPENGL); //| SDL_WINDOW_FULLSCREEN);

		Platform->ReadFile = &ReadFile;
		Platform->WriteFile = &WriteFile;
		Platform->FreeFileMemory = &FreeFileMemory;

		if(Window)
		{
			SDL_GLContext GLContext = SDL_GL_CreateContext(Window);

			if(GLContext)
			{
				// Setup Imgui binding
				ImGui_Init(Window);

				int w, h;
				SDL_GL_GetDrawableSize(Window, &w, &h);

				game_code GameCode = {};
				
				LoadGameCode(&GameCode);
				GameCode.GameCodeLoaded(Platform);

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
					
					for(int ButtonIndex = 0; ButtonIndex < ArrayCount(NewInput.Buttons); ++ButtonIndex)
					{
						NewInput.Buttons[ButtonIndex].EndedDown = OldInput.Buttons[ButtonIndex].EndedDown;
					}

					NewInput.Mouse.x = OldInput.Mouse.x;
					NewInput.Mouse.y = OldInput.Mouse.y;

					for(int ButtonIndex = 0; ButtonIndex < ArrayCount(NewInput.Mouse.Buttons); ++ButtonIndex)
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
					// Imgui mark new frame start
					ImGui_NewFrame(Window);

					{
            			static float f = 0.0f;
            			ImGui::Text("Hello, world!");
            			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
           				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        			}

					PrevCounter = SDL_GetPerformanceCounter();
					NewInput.dTFrame = FrameTime;
					GameCode.GameUpdateAndRender(&NewInput);
					ImGui::Render();
					SDL_GL_SwapWindow(Window);
					
					OldInput = NewInput;	

					CurrentCounter = SDL_GetPerformanceCounter();
					FrameTime = (r32)((CurrentCounter - PrevCounter)) / CounterFrequency;
					char DebugCountString[100];
					sprintf(DebugCountString, "%f ms\n", FrameTime*1000.0);
					PrevCounter = CurrentCounter;

					#if defined(WINDOWS)
					OutputDebugString(DebugCountString);
					#endif
					// Platform->Log(INFO, DebugCountString);
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
