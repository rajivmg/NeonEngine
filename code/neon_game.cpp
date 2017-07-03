#include "neon_game.h"

void InitGameResources()
{
	GameResources.dbgQuadBatch.Init(TEXTURE_QUAD, KILOBYTE(64));
	GameResources.dbgQuadBatch.SetShader(RenderResources.TQuadShader);

	GameResources.dbgLineBatch.Init(KILOBYTE(32));
	GameResources.dbgLineBatch.SetShader(RenderResources.CQuadShader);

	GameResources.dbgUITextBatch.SetFont(RenderResources.HUDFont, KILOBYTE(32));
}

extern "C"
GAME_CODE_LOADED(GameCodeLoaded)
{
	Platform = aPlatform;
	InitRenderer();

	InitGameResources();
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

	//==================================//
	
	ClearBackBuffer();
	
	/*===================================
	=            Game Render            =
	===================================*/
	local_persist texture EngineLogo;
	local_persist b32 ShaderAndTextureSet = false;
	local_persist texture_quad EngineLogoQuad = {}; 
	local_persist color_quad CQuad1 = {};

	if(!ShaderAndTextureSet)
	{
		EngineLogo.LoadFromFile("vortex.tga");
		GameResources.dbgQuadBatch.SetTexture(&EngineLogo);
		EngineLogo.FreeMemory();

 		EngineLogoQuad = TextureQuad(vec2(608, 296), vec2(64, 64));

		ShaderAndTextureSet = true;
	}

	GameResources.dbgQuadBatch.PushQuad(&EngineLogoQuad);

	local_persist int Counter = 0;
	// if(Counter == 15)
	{	
		GameResources.dbgUITextBatch.Flush();
		GameResources.dbgUITextBatch.PushText(vec2(10, Platform->Height - 40), vec4(1.0f, 1.0f, 1.0f, 1.0f),
				"Frame time(ms): %.2f\nA quick brown fox jumps over the lazy dog.", FrameTime);
		GameResources.dbgUITextBatch.PushText(vec2(10, Platform->Height - 100), vec4(1.0f, 1.0f, 1.0f, 1.0f),
				"Mouse X: %d\nMouse Y: %d", Input->Mouse.X, Input->Mouse.Y);
		Counter = 0;
	}
	// Counter++;

	GameResources.dbgQuadBatch.Draw();
	GameResources.dbgLineBatch.Draw();
	GameResources.dbgUITextBatch.Draw();
}
