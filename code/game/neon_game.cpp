#include "neon_game.h"

static render_cmd_list *RenderCmdList;
using namespace Renderer;

extern "C" DLLEXPORT
GAME_CODE_LOADED(GameCodeLoaded)
{
	Platform = aPlatform;
	Renderer::Init();
	RenderCmdList = AllocRenderCmdList();
}

extern "C" DLLEXPORT
GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
	/*===================================
	=            Game Update            =
	===================================*/
	if(!Input->Mouse.Left.EndedDown && Input->Mouse.Left.HalfTransitionCount == 1)
	{
		// Platform->Log(INFO, "'Left' Mouse button pressed.");
	}

	/*===================================
	=            Game Render            =
	===================================*/
	local_persist bool FirstCall = false;
	local_persist font *Font = new font();
 	local_persist font *StyleFont = new font(); 
	local_persist u32 TPixelWallIndex;
	local_persist u32 SnakeHeadTexHandle;
	if(!FirstCall)
	{
		FirstCall = true;

		texture PixelWall;
		PixelWall.LoadFromFile("red_wall_1.tga");
		TPixelWallIndex = Renderer::UploadTexture(&PixelWall, TEXTURE_2D, NEAREST, REPEAT);
		PixelWall.FreeMemory();

		texture SnakeHead;
		SnakeHead.LoadFromFile("snake/snake_head_up.tga");
		SnakeHeadTexHandle = Renderer::UploadTexture(&SnakeHead, TEXTURE_2D, NEAREST, REPEAT);
		SnakeHead.FreeMemory();

 		Font->Load("font/Inconsolata/Inconsolata-Bold.ttf", 16);
 		StyleFont->Load("font/Neuton/Neuton-Regular.ttf", 64);
 	}

	RenderCmdClear(RenderCmdList);
	
	// debug grid
#if 0
	for(int i = 0; i <= 1280/32; ++i)
	{
		RenderCmdLine(RenderCmdList, vec3(32.0f * i, 0.0f, 0.1f), vec3(32.0f * i + 1.0f, 720.0f, 0.1f), vec4(1.0f, 1.0f, 0.0f, 0.4f), 0);
	}
	for(int i = 0; i<= 720/32; ++i)
	{
		RenderCmdLine(RenderCmdList, vec3(0.0f, 32.0f * i, 0.1f), vec3(1280.0f, 32.0f * i + 1.0f, 0.1f), vec4(1.0f, 1.0f, 0.0f, 0.4f), 0);
	}
#endif
	// debug text
	// RenderCmdText(RenderCmdList, vec3(5.0f, (r32)Platform->Height, 0.2f), vec4(1.0f, 1.0f, 1.0f, 0.7f), Font, 0,
			// "%.2fms Mouse(%d,%d)", Input->dTFrame * 1000, Input->Mouse.x, Input->Mouse.y); // multiply by 1000 to convert sec to ms

#if defined(DEBUG_BUILD)
	RenderCmdText(RenderCmdList, vec3(5.0f, (r32)Platform->Height, 0.2f), vec4(1.0f, 1.0f, 1.0f, 0.7f), Font, 0,
			"%s", "Debug Build");
#else
	RenderCmdText(RenderCmdList, vec3(5.0f, (r32)Platform->Height, 0.2f), vec4(1.0f, 1.0f, 1.0f, 0.7f), Font, 0,
			"%s", "Release Build");
#endif
	/*
	u32 MouseTileX = (u32)floorf(Input->Mouse.X/32.0f);
	u32 MouseTileY = (u32)floorf(Input->Mouse.Y/32.0f);
	RenderCmdText(RenderCmdList, vec3(185.0f, (r32)Platform->Height, 0.2f), vec4(1.0f, 1.0f, 1.0f, 0.7f), Font, 0,
			"Tile(%d,%d)", MouseTileY, MouseTileX); */
	// RenderCmdText(RenderCmdList, vec3(500.0f, (r32)Platform->Height, 0.2f), vec4(1.0f, 1.0f, 1.0f, 1.0f), StyleFont, 0,
	// 		"Neon");
	
	// tilemap
	u32 TileMap[11][18] =
	{
		{1, 1, 1,  1, 1, 1,  1, 1, 1,  1, 1, 1,  1, 1, 1,  1, 1, 1},
		{1, 0, 0,  0, 0, 0,  0, 0, 0,  0, 0, 0,  0, 0, 0,  0, 0, 1},
		{1, 0, 0,  0, 0, 0,  0, 0, 0,  0, 0, 0,  0, 0, 0,  0, 0, 1},
		{1, 0, 0,  0, 0, 0,  0, 0, 0,  0, 0, 0,  0, 0, 1,  1, 1, 1},
		{1, 0, 0,  0, 0, 0,  0, 0, 0,  0, 0, 0,  0, 0, 0,  0, 0, 1},
		{1, 0, 0,  0, 0, 0,  0, 0, 1,  1, 1, 1,  0, 0, 0,  0, 0, 1},
		{1, 0, 0,  0, 0, 0,  0, 0, 0,  0, 0, 0,  0, 0, 0,  0, 0, 1},
		{1, 0, 0,  0, 1, 1,  1, 0, 0,  0, 0, 0,  0, 0, 0,  0, 0, 1},
		{1, 0, 0,  0, 0, 0,  0, 0, 0,  0, 0, 0,  0, 0, 0,  0, 0, 1},
		{1, 0, 1,  0, 0, 0,  0, 0, 0,  0, 0, 0,  0, 0, 0,  0, 0, 1},
		{1, 1, 1,  1, 1, 1,  1, 1, 1,  1, 1, 1,  1, 1, 1,  1, 1, 1},
	};

	s32 TileWidth = 64;
	s32 TileHeight = 64;

	for(int Row = 10; Row>=0; --Row)
	{
		for(int Column = 0; Column<18; ++Column)
		{
			vec2 Min = vec2(Column * TileWidth, ((10-Row) * TileHeight));
			vec2 Max = vec2(TileWidth, TileHeight);

			if(TileMap[Row][Column] == 1)
			{
				RenderCmdColorQuad(RenderCmdList, vec3(Min, 0.0f), Max, vec4(0.7f, 0.1f, 0.3f, 1.0f), 0);
			}
			else
			{
				RenderCmdColorQuad(RenderCmdList, vec3(Min, 0.0f), Max, vec4(0.3f, 0.0f, 0.0f, 1.0f), 0);
			}
		}
	}

	static vec2 PlayerPos = vec2(128.0f, 128.0f);
	static r32 MetersToPixels = 64;
	vec2 dPlayer;

	if(Input->Up.EndedDown)
	{
		dPlayer = vec2(0.0f, 1.0f);
	}
	else if(Input->Down.EndedDown)
	{
		dPlayer = vec2(0.0f, -1.0f);
	}
	else if(Input->Left.EndedDown)
	{
		dPlayer = vec2(-1.0f, 0.0f);
	}
	else if(Input->Right.EndedDown)
	{
		dPlayer = vec2(1.0f, 0.0f);
	}
	dPlayer = dPlayer * 4.0f;
	PlayerPos = PlayerPos + dPlayer*(MetersToPixels*Input->dTFrame);

	// RenderCmdColorQuad(RenderCmdList, vec3(PlayerPos, 0.1f), vec2(64.0f, 64.0f), vec4(1.0f, 1.0f, 0.0f, 1.0f), 0);
	RenderCmdTextureQuad(RenderCmdList, vec3(PlayerPos, 0.1f), vec2(64.0f, 64.0f), vec4(0.0f, 0.0f, 1.0f, 1.0f),
						vec4(1.0f, 1.0f, 0.0f, 1.0f), SnakeHeadTexHandle, 0);
	
	// Draw
	DrawRenderCmdList(RenderCmdList);
}

level* AllocLevel(ivec2 LevelSize)
{
	level *Level = (level *)malloc(sizeof(level));

	return Level;
}