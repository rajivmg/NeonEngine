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
	static texture *DefaultTexture;
	static font *DebugFont = new font;
	static font *NeutonFont = new font;
	static render_cmd_list *BackbufferList = new render_cmd_list(MEGABYTE(5));
	static render_resource BasicShader;

	if(!FirstCall)
	{
		FirstCall = true;

		DefaultTexture = new texture;
		DefaultTexture->LoadFile("uv_texture.tga", texture_type::TEXTURE_2D, texture_filter::LINEAR, texture_wrap::CLAMP, true);
		DefaultTexture->CreateRenderResource();
		DefaultTexture->FreeContentMemory();

		BasicShader = rndr::MakeShaderProgram("shaders/basic_vs.glsl", "shaders/basic_ps.glsl");

		DebugFont->Load("fonts/Inconsolata/Inconsolata-Regular.ttf", 16);
		NeutonFont->Load("fonts/Neuton/Neuton-Regular.ttf", 48);

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

	std::vector<vert_P1UV1C1> TextVertices;
	PushTextSprite(&TextVertices, NeutonFont, vec3i(100, 500, 1), vec4i(1, 1, 0, 1), "Neon Text Rendering\nSucessful!");

	static render_resource TextVertexBuffer = rndr::MakeVertexBuffer((u32)TextVertices.size() * sizeof(vert_P1UV1C1));
	rndr::VertexBufferData(TextVertexBuffer, 0, (u32)TextVertices.size() * sizeof(vert_P1UV1C1), &TextVertices.front());

	cmd::udraw *TextC = BackbufferList->AddCommand<cmd::udraw>(10, 0);
	TextC->VertexBuffer = TextVertexBuffer;
	TextC->VertexFormat = vert_format::P1UV1C1;
	TextC->StartVertex = 0;
	TextC->VertexCount = (u32)TextVertices.size();
	TextC->Texture = &NeutonFont->TextureAtlas.Texture;
	TextC->ShaderProgram = BasicShader;

	BackbufferList->Sort();
	BackbufferList->Submit();
	BackbufferList->Flush();

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