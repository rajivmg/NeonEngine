#include "neon_game.h"

#include "neon_font.h"
#include "neon_opengl.h"

#include "neon_primitive_mesh.h"

DLLEXPORT
GAME_CODE_LOADED(GameCodeLoaded)
{
	Platform = aPlatform;
	ImGui::SetCurrentContext(aImGuiCtx);
	rndr::Init();
}

DLLEXPORT
GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
	//-----------------------------------------------------------------------------
	// Game Update
	//-----------------------------------------------------------------------------
	if(!Input->Mouse.Left.EndedDown && Input->Mouse.Left.HalfTransitionCount == 1)
	{
		 //Platform->Log(INFO, "'Left' Mouse button pressed.");
	}

	//-----------------------------------------------------------------------------
	// Game Render
	//-----------------------------------------------------------------------------
	rndr::Clear();

	if(ImGui::Begin("Debug Info"))
	{
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
	ImGui::End();

	static bool FirstCall = false;
	static u32  DefaultTex;
	static texture *DefaultTexture;
	static font *DebugFont = new font;
	static font *BigFont = new font;
	static font *ParaFont = new font;
	static u32 Sky;
	static render_cmd_list a(KILOBYTE(5));
	static render_resource TestProg;
	static render_resource TestVB;

	if(!FirstCall)
	{
		FirstCall = true;
		DefaultTexture = new texture;
		DefaultTexture->LoadFile("uv_texture.tga", texture_type::TEXTURE_2D, texture_filter::LINEAR, texture_wrap::CLAMP, true);
		DefaultTexture->HwGammaCorrection = true;
		DefaultTexture->CreateRenderResource();
		DefaultTexture->FreeContentMemory();

		DebugFont->Load("font/Inconsolata/Inconsolata-Regular.ttf", 16);
		BigFont->Load("font/Neuton/Neuton-Regular.ttf", 48);

		std::vector<vert_POS3UV2COLOR4> *VertexData = new std::vector<vert_POS3UV2COLOR4>;
		PushSpriteQuad(VertexData, vec3(50.0f, 50.0f, 1.0f), vec2(400.0f, 400.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f), vec4(0.0f, 0.0f, 1.0f, 1.0f));

		PushSpriteQuad(VertexData, vec3(500.0f, 50.0f, 1.0f), vec2((r32)BigFont->TextureAtlas.Texture.Width, (r32)BigFont->TextureAtlas.Texture.Height), vec4(1.0f, 1.0f, 1.0f, 1.0f), vec4(0.0f, 0.0f, 1.0f, 1.0f));

		TestVB = rndr::MakeVertexBuffer(MEGABYTE(1));
		rndr::VertexBufferData(TestVB, 0, (u32)VertexData->size() * sizeof(vert_POS3UV2COLOR4), &VertexData->front());

		TestProg = rndr::MakeShaderProgram("basic_vs.glsl", "basic_ps.glsl");
		
		SafeDelete(VertexData);

		vec3 va = vec3i(1, 1, 1);
		vec3 vb = va + 2.0f;
		vec3 vc = 2.0f + vb;
		vc += 2.0f;
		vc *= vb;
		vc /= 3.0f;
		vc = va + vec3(1.0f);
		vc = -vb;
		vc *= vec3(-1.0f);
		vc = vc == vec3(2.0f);
		vec3 vd = vec3i(1, 2, 3);
		vec3 ve = vec3i(1, 5, 7);
		vec3 vf = Cross(vd, ve);
		int a; a = 5;
	}

	cmd::udraw *d = a.AddCommand<cmd::udraw>(10, 0);
	d->Texture = DefaultTexture;
	d->VertexFormat = vert_format::POS3UV2COLOR4;
	d->StartVertex = 0;
	d->VertexCount = 6;
	d->VertexBuffer = TestVB;
	d->ShaderProgram = TestProg;
	
	cmd::udraw *Quad2 = a.AddCommand<cmd::udraw>(50, 0);
	Quad2->VertexBuffer = TestVB;
	Quad2->VertexFormat = vert_format::POS3UV2COLOR4;
	Quad2->StartVertex	= 6;
	Quad2->VertexCount = 6;
	Quad2->Texture = &(BigFont->TextureAtlas.Texture);
	Quad2->ShaderProgram = TestProg;

	//cmd::udraw *c = a.AppendCommand<cmd::udraw>(d, 0);
	//c->Texture.ResourceHandle = 777;

	a.Sort();
	a.Submit();
	a.Flush();

/*
	RenderCmdClear(RenderCmdList);
	
// Debug grid
#if 0
	for(int i = 0; i <= 1280/64; ++i)
	{
		RenderCmdLine(RenderCmdList, vec3(64.0f * i, 0.0f, 0.1f), vec3(64.0f * i + 1.0f, 720.0f, 0.1f), vec4(1.0f, 1.0f, 0.0f, 0.4f), 0);
	}
	for(int i = 0; i<= 720/64; ++i)
	{
		RenderCmdLine(RenderCmdList, vec3(0.0f, 64.0f * i, 0.1f), vec3(1280.0f, 64.0f * i + 1.0f, 0.1f), vec4(1.0f, 1.0f, 0.0f, 0.4f), 0);
	}
#endif

#if defined(DEBUG)
	RenderCmdText(RenderCmdList, vec3(5.0f, (r32)Platform->Height, 0.2f), vec4(1.0f, 1.0f, 1.0f, 1.0f), DebugFont, RT0,
			"%s", "Debug Build");
#else
	RenderCmdText(RenderCmdList, vec3(5.0f, (r32)Platform->Height, 0.2f), vec4(1.0f, 1.0f, 1.0f, 0.7f), DebugFont, RT0,
			"%s", "Release Build");
#endif

	RenderCmdText(RenderCmdList, vec3(100.f, 500.f, 0.2f), vec4(1.0f, 1.0f, 1.0f, 1.0f), BigFont, RT0, "%s", "Hello Rajiv!");
	
	r32 TileSize = 64.0f;
	for(int i = 0; i <= Platform->Width / TileSize; ++i)
	{
		for(int j = 0; j <= Platform->Height / TileSize; ++j)
		{
			RenderCmdTextureQuad(RenderCmdList, vec3(i*TileSize, j*TileSize, 0.2f), vec2((r32)TileSize, (r32)TileSize), vec4(0.0f, 0.0f, 1.0f, 1.0f),
				vec4(1.0f, 1.0f, 1.0f, 1.0f), DefaultTexture->Idx, RT0);
		}
	}

	// Draw
	DrawRenderCmdList(RenderCmdList);
*/
}

//level* AllocLevel(ivec2 LevelSize)
//{
//	level *Level = (level *)malloc(sizeof(level));
//
//	return Level;
//}