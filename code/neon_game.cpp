#include "neon_game.h"

static render_cmd_list RenderCmdList;

extern "C"
GAME_CODE_LOADED(GameCodeLoaded)
{
	Platform = aPlatform;
	Renderer::Init();
	AllocRenderCmdList(&RenderCmdList);
}

extern "C"
GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
	/*===================================
	=            Game Update            =
	===================================*/
	if(Input->Up.EndedDown && Input->Up.HalfTransitionCount == 1)
	{
		Platform->Log(INFO,"'Up' pressed.\n");
	}

	if(!Input->Mouse.Left.EndedDown && Input->Mouse.Left.HalfTransitionCount == 1)
	{
		Platform->Log(INFO, "'Left' Mouse button pressed.");
	}

	/*===================================
	=            Game Render            =
	===================================*/
	local_persist bool FirstCall = false;
	local_persist color_quad CQuad1 = {};
	local_persist texture EngineLogo;
	local_persist font *Font = new font();
	local_persist u32 TIndex;
	local_persist u32 TChainIndex;
	local_persist u32 TPixelWallIndex;
	local_persist u32 TCharacterIndex;
	if(!FirstCall)
	{
		FirstCall = true;

		EngineLogo.LoadFromFile("vortex.tga");
		TIndex = EngineLogo.UploadToGPU();
		
		texture ChainArmor;
		ChainArmor.LoadFromFile("chain_mail2.tga");
		TChainIndex = ChainArmor.UploadToGPU();
		ChainArmor.FreeMemory();

		texture PixelWall;
		PixelWall.LoadFromFile("red_wall_1.tga");
		TPixelWallIndex = PixelWall.UploadToGPU();
		PixelWall.FreeMemory();

		texture Character;
		Character.LoadFromFile("character_main.tga");
		TCharacterIndex = Character.UploadToGPU();
		Character.FreeMemory();

 		Font->Load("font/Inconsolata/Inconsolata-Bold.ttf", 16);

 		vec3 A(1, 2, 3), B(4, 5, 6);
 		vec3 CrossResult = A.Cross(B);
 		// MeshFromFile("model/suzanne.fbx");
	}

	RenderCmdClear(&RenderCmdList);
	vec2 TextBoxDim = Font->GetTextDim("%.2fms", FrameTime);
	// RenderCmdColorQuad(&RenderCmdList, vec2(5.0f, Platform->Height - TextBoxDim.Y), TextBoxDim, vec4(0.0f, 0.0f, 0.0f, 0.8f));
	RenderCmdText(&RenderCmdList, Font, vec3(5.0f, (r32)Platform->Height, 0.0f), vec4(1.0f, 1.0f, 1.0f, 0.2f),
			"%.2fms", FrameTime);
	RenderCmdText(&RenderCmdList, Font, vec3(10.0f + TextBoxDim.x, (r32)Platform->Height, 0.0f), vec4(1.0f, 1.0f, 1.0f, 0.6f),
				"Mouse(%d,%d)", Input->Mouse.X, Input->Mouse.Y);

	RenderCmdTextureQuad(&RenderCmdList, vec3(550, 350, 0), vec2(16 * 16, 16 * 16), vec4(0.0, 0.0f, 8.0f, 8.0f),
						TPixelWallIndex, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderCmdTextureQuad(&RenderCmdList, vec3(300, 460, 0), vec2(19 * 4, 46 * 4), vec4(0.0, 0.0f, 1.0f, 1.0f),
						TCharacterIndex, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderCmdColorQuad(&RenderCmdList, vec3(1, 1, 0), vec2(10, 10), vec4(1.0f, 0.0f, 0.0f, 1.0f));
	RenderCmdTextureQuad(&RenderCmdList, vec3(10, 20, 0), vec2(64, 64), 
						vec4(0.0f, 0.0f, 1.0f, 1.0f), TIndex, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderCmdTextureQuad(&RenderCmdList, vec3(100, 120, 0), vec2(64, 64), 
						vec4(0.0f, 0.0f, 1.0f, 1.0f), TIndex, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderCmdTextureQuad(&RenderCmdList, vec3(150, 150, 0), vec2(64, 64), vec4(0.0f, 0.0f, 1.0f, 1.0f),
						TChainIndex, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderCmdColorQuad(&RenderCmdList, vec3(300, 350, 0), vec2(100, 100), vec4(200/255.0f, 20/255.0f, 99/255.0f, 1.0f));
	
	// Draw
	DrawRenderCmdList(&RenderCmdList);
}
