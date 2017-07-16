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

 		Font->Load("font/Inconsolata/Inconsolata-Bold.ttf", 16);

 		vec3 A(1, 2, 3), B(4, 5, 6);
 		vec3 CrossResult = A.Cross(B);
	}

	RenderCmdClear(&RenderCmdList);
	vec2 TextBoxDim = Font->GetTextDim("%.2fms", FrameTime);
	// RenderCmdColorQuad(&RenderCmdList, vec2(5.0f, Platform->Height - TextBoxDim.Y), TextBoxDim, vec4(0.0f, 0.0f, 0.0f, 0.8f));
	RenderCmdText(&RenderCmdList, Font, vec2(5, Platform->Height), vec4(1.0f, 1.0f, 1.0f, 0.1f),
			"%.2fms", FrameTime);
	RenderCmdText(&RenderCmdList, Font, vec2(10.0f + TextBoxDim.x, (r32)Platform->Height), vec4(1.0f, 1.0f, 1.0f, 0.7f),
				"Mouse(%d,%d)", Input->Mouse.X, Input->Mouse.Y);

	RenderCmdTextureQuad(&RenderCmdList, TPixelWallIndex, vec2(550, 350), vec2(16 * 16, 16 * 16), vec4(0.0, 0.0f, 8.0f, 8.0f),
						vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderCmdColorQuad(&RenderCmdList, vec2(1, 1), vec2(10, 10), vec4(1.0f, 0.0f, 0.0f, 1.0f));
	RenderCmdTextureQuad(&RenderCmdList, TIndex, vec2(10, 20), vec2(64, 64), 
						vec4(0.0f, 0.0f, 1.0f, 1.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderCmdTextureQuad(&RenderCmdList, TIndex, vec2(100, 120), vec2(64, 64), 
						vec4(0.0f, 0.0f, 1.0f, 1.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderCmdTextureQuad(&RenderCmdList, TChainIndex, vec2(150, 150), vec2(64, 64), vec4(0.0f, 0.0f, 1.0f, 1.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f));
	// RenderCmdColorQuad(&RenderCmdList, vec2(300, 350), vec2(100, 100), vec4(0.0f, 0.9f, 0.9f, 0.8f));
	RenderCmdColorQuad(&RenderCmdList, vec2(300, 350), vec2(100, 100), vec4(200/255.0f, 20/255.0f, 99/255.0f, 1.0f));
	// Draw
	DrawRenderCmdList(&RenderCmdList);
}
