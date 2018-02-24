#include "neon_game.h"

#include "neon_font.h"
#include "neon_primitive_mesh.h"

DLLEXPORT
GAME_SETUP(GameSetup)
{
	Platform = _Platform;
	ImGui::SetCurrentContext(_ImGuiCtx);
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

	static bool FirstCall = false;
	static texture *DefaultTexture;
	static font *DebugFont = new font;
	static font *NeutonFont = new font;
	static render_cmd_list *RenderCmdList = new render_cmd_list(MEGABYTE(5));
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
		NeutonFont->Load("fonts/Neuton/Neuton-Regular.ttf", 32);

		vec3 va = vec3i(1, 1, 1);
	}

	std::vector<vert_P1UV1C1> TextVertices;
	PushTextSprite(&TextVertices, DebugFont, vec3i(0, 720, 1), vec4i(1, 1, 1, 1), "%0.3f ms/frame", 1000.0f * Input->FrameTime);
	PushTextSprite(&TextVertices, DebugFont, vec3i(0, 16, 1), vec4i(1, 1, 1, 1), "%s @ %s", __DATE__,  __TIME__);
	static render_resource TextVertexBuffer = rndr::MakeVertexBuffer(1000 * sizeof(vert_P1UV1C1));
	rndr::VertexBufferData(TextVertexBuffer, 0, (u32)TextVertices.size() * sizeof(vert_P1UV1C1), &TextVertices.front());

	cmd::udraw *TextC = RenderCmdList->AddCommand<cmd::udraw>(10, 0);
	TextC->VertexBuffer = TextVertexBuffer;
	TextC->VertexFormat = vert_format::P1UV1C1;
	TextC->StartVertex = 0;
	TextC->VertexCount = (u32)TextVertices.size();
	TextC->Textures[0] = DebugFont->TextureAtlas.Texture.RenderResource;
	TextC->ShaderProgram = BasicShader;

	RenderCmdList->Sort();
	RenderCmdList->Submit();
	RenderCmdList->Flush();
}