#include "neon_game.h"

#include "neon_font.h"
#include "neon_primitive_mesh.h"
#include "neon_mesh.h"

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

	local_persist render_cmd_list *CmdList = new render_cmd_list(MEGABYTE(1));
	local_persist render_cmd_list *SpriteCmdList = new render_cmd_list(MEGABYTE(1));
	local_persist texture *DummyMapTexture = new texture;
	local_persist render_resource SpriteVertexBuffer;
	local_persist render_resource SpriteShader;
	local_persist font *DebugFont = new font;

	local_persist texture *WhiteTexture = new texture;
	local_persist render_resource SuzanneVertexBuffer;
	local_persist render_resource SuzanneIndexBuffer;
	local_persist render_resource SuzanneShader;

	local_persist bool OnceUponAGame = false;
	if(!OnceUponAGame)
	{
		OnceUponAGame = true;

		WhiteTexture->LoadFile("white_texture.tga", texture_type::TEXTURE_2D, texture_filter::NEAREST, texture_wrap::CLAMP, false);
		WhiteTexture->CreateRenderResource();

		SpriteCmdList->ViewMatrix = LookAt(vec3(0, 0, 0), vec3i(0, 0, -1), vec3i(0, 1, 0));//Mat4Identity();
		SpriteCmdList->ProjectionMatrix = Screenspace(Platform.Width, Platform.Height);

		DummyMapTexture->LoadFile("map.tga", texture_type::TEXTURE_2D, texture_filter::LINEAR, texture_wrap::CLAMP, true);
		DummyMapTexture->CreateRenderResource();
		DummyMapTexture->FreeContentMemory();

		SpriteShader = rndr::MakeShaderProgram("shaders/basic_vs.glsl", "shaders/basic_ps.glsl");

		DebugFont->Load("fonts/Inconsolata/Inconsolata-Regular.ttf", 16);

		SpriteVertexBuffer = rndr::MakeVertexBuffer(MEGABYTE(1), false);

		CmdList->ViewMatrix = LookAt(vec3i(1, 1, 1), vec3i(0, 0, 0), vec3i(0, 1, 0));
		CmdList->ProjectionMatrix = Perspective(DEG2RAD(105), (r32)Platform.Width / Platform.Height, 0.1f, 100.0f);

		std::vector<vert_P1C1UV1> SuzanneVertices;
		std::vector<vert_index> SuzanneIndices;
		PushMesh(&SuzanneVertices, &SuzanneIndices, "models/suzanne.fbx", 0);
		SuzanneVertexBuffer = rndr::MakeVertexBuffer((u32)SuzanneVertices.size() * sizeof(vert_P1N1UV1), false);
		rndr::VertexBufferData(SuzanneVertexBuffer, 0, (u32)SuzanneVertices.size() * sizeof(vert_P1N1UV1), &SuzanneVertices.front());
		SuzanneIndexBuffer = rndr::MakeIndexBuffer((u32)SuzanneIndices.size() * sizeof(vert_index), false);
		rndr::IndexBufferData(SuzanneIndexBuffer, 0, (u32)SuzanneIndices.size() * sizeof(vert_index), &SuzanneIndices.front());
		SuzanneShader = rndr::MakeShaderProgram("shaders/model_vs.glsl", "shaders/model_ps.glsl");

		int BreakPoint = 0000;
	}

	//r32 MetersToPixel = Platform.Width / 20.0f;
	//r32 PixelsToMeter = 1.0f / MetersToPixel;

	std::vector<vert_P1C1UV1> SpriteVertices;
	PushSprite(&SpriteVertices, vec3i(0, 0, -1), vec2i(1280, 720), vec4i(1, 1, 1, 1), vec4i(0, 0, 1, 1));
	u32 DummyMapVertSize = (u32)SpriteVertices.size();
	u32 DebugTextStartVert = (u32)SpriteVertices.size();
	PushTextSprite(&SpriteVertices, DebugFont, vec3i(0, 720, 1), vec4i(1, 1, 0, 1), "%0.2f ms/frame", 1000.0f * Input->FrameTime);
	//PushTextSprite(&SpriteVertices, DebugFont, vec3i(0, 16, 1), vec4i(1, 1, 1, 1), "%s @ %s", __DATE__,  __TIME__);
	if(Input->Up.EndedDown)
	{
		PushTextSprite(&SpriteVertices, DebugFont, vec3i(150, 720, 1), vec4i(1, 1, 1, 1), "Input UP ended down, half transition count %d", Input->Up.HalfTransitionCount);
	}
	u32 DebugTextVertCount = (u32)SpriteVertices.size() - DummyMapVertSize;

	rndr::VertexBufferData(SpriteVertexBuffer, 0, (u32)SpriteVertices.size() * sizeof(vert_P1C1UV1), &SpriteVertices.front());

	cmd::draw *DummyMapCmd = SpriteCmdList->AddCommand<cmd::draw>(1, 0);
	DummyMapCmd->VertexBuffer = SpriteVertexBuffer;
	DummyMapCmd->VertexFormat = vert_format::P1C1UV1;
	DummyMapCmd->StartVertex = 0;
	DummyMapCmd->VertexCount = DummyMapVertSize;
	DummyMapCmd->Textures[0] = DummyMapTexture->RenderResource;
	DummyMapCmd->ShaderProgram = SpriteShader;

	cmd::draw *DebugTextCmd = SpriteCmdList->AddCommand<cmd::draw>(2, 0);
	DebugTextCmd->VertexBuffer = SpriteVertexBuffer;
	DebugTextCmd->VertexFormat = vert_format::P1C1UV1;
	DebugTextCmd->StartVertex = DebugTextStartVert;
	DebugTextCmd->VertexCount = DebugTextVertCount;
	DebugTextCmd->Textures[0] = DebugFont->TextureAtlas.Texture.RenderResource;
	DebugTextCmd->ShaderProgram = SpriteShader;

	SpriteCmdList->Sort();
	SpriteCmdList->Submit();
	SpriteCmdList->Flush();

	cmd::draw_indexed *SuzanneDraw = CmdList->AddCommand<cmd::draw_indexed>(1, 0);
	SuzanneDraw->VertexBuffer = SuzanneVertexBuffer;
	SuzanneDraw->VertexFormat = vert_format::P1C1UV1;
	SuzanneDraw->IndexBuffer = SuzanneIndexBuffer;
	SuzanneDraw->IndexCount = 2904;
	SuzanneDraw->Textures[0] = WhiteTexture->RenderResource;
	SuzanneDraw->ShaderProgram = SuzanneShader;

	CmdList->Sort();
	//CmdList->Submit();
	CmdList->Flush();
}