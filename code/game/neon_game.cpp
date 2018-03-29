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
	static r32 MapDx = 0.0f, MapDy = 0.0f;

	if(Input->Mouse.Left.EndedDown && Input->Mouse.Left.HalfTransitionCount == 1)
	{
		 Platform.Log("'Left' Mouse button pressed.");
	}

	game_controller_input *Controller = &Input->Controllers[0];
	if(Controller->Left.EndedDown && Controller->Left.HalfTransitionCount == 1)
	{
		--MapDx;
	}
	if(Controller->Right.EndedDown && Controller->Right.HalfTransitionCount == 1)
	{
		++MapDx;
	}
	if(Controller->Up.EndedDown && Controller->Up.HalfTransitionCount == 1)
	{
		++MapDy;
	}
	if(Controller->Down.EndedDown && Controller->Down.HalfTransitionCount == 1)
	{
		--MapDy;
	}
	//-----------------------------------------------------------------------------
	// Game Render
	//-----------------------------------------------------------------------------
	rndr::Clear();

	local_persist render_cmd_list *CmdList = new render_cmd_list(MEGABYTE(1));
	local_persist render_cmd_list *SpriteCmdList = new render_cmd_list(MEGABYTE(1));
	local_persist render_cmd_list *HUDCmdList = new render_cmd_list(MEGABYTE(1));

	local_persist render_resource SpriteVertexBuffer;
	local_persist render_resource SpriteShader;
	
	local_persist font *DebugFont = new font;

	local_persist texture *DefaultTileTexture = new texture;
	local_persist texture *WhiteTexture = new texture;

	local_persist render_resource SuzanneVertexBuffer;
	local_persist render_resource SuzanneIndexBuffer;
	local_persist render_resource SuzanneShader;

	r32 MetersToPixels = Platform.Width / 20.0f;
	r32 PixelsToMeters = 1.0f / MetersToPixels;

	if(Input->Mouse.Left.EndedDown)
	{
		MapDx -= Input->Mouse.xrel * PixelsToMeters;
		MapDy -= Input->Mouse.yrel * PixelsToMeters;
	}

	local_persist bool OnceUponAGame = false;
	if(!OnceUponAGame)
	{
		OnceUponAGame = true;

		WhiteTexture->LoadFile("white_texture.tga", texture_type::TEXTURE_2D, texture_filter::NEAREST, texture_wrap::CLAMP, false);
		WhiteTexture->CreateRenderResource();

		SpriteCmdList->ViewMatrix = LookAt(vec3(MapDx, 0, 0), vec3(MapDx, 0, -1), vec3i(0, 1, 0));
		SpriteCmdList->ProjectionMatrix = Orthographic(0, 20.0f, PixelsToMeters * Platform.Height, 0, -1, 1);

		HUDCmdList->ViewMatrix = LookAt(vec3(0, 0, 0), vec3i(0, 0, -1), vec3i(0, 1, 0));
		HUDCmdList->ProjectionMatrix = Screenspace(Platform.Width, Platform.Height);

		DefaultTileTexture->LoadFile("default_tile.tga", texture_type::TEXTURE_2D, texture_filter::NEAREST, texture_wrap::CLAMP, true);
		DefaultTileTexture->CreateRenderResource();
		DefaultTileTexture->FreeContentMemory();

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
	SpriteCmdList->ViewMatrix = LookAt(vec3(MapDx, MapDy, 0), vec3(MapDx, MapDy, -1), vec3i(0, 1, 0));
	std::vector<vert_P1C1UV1> SpriteVertices;
	for(int I = 0; I < PixelsToMeters * Platform.Height; ++I)
	{
		for(int J = 0; J < 20; ++J)
		{
			PushSprite(&SpriteVertices, vec3i(J, I, -1), vec2i(1, 1), vec4i(1, 1, 1, 1), vec4i(0, 0, 1, 1));
		}
	}
	u32 DummyMapVertSize = (u32)SpriteVertices.size();
	u32 DebugTextStartVert = (u32)SpriteVertices.size();
	PushTextSprite(&SpriteVertices, DebugFont, vec3i(0, 720, 1), vec4i(1, 1, 0, 1), "%0.2f ms/frame", 1000.0f * Input->FrameTime);
	PushTextSprite(&SpriteVertices, DebugFont, vec3i(150, 720, 1), vec4i(1, 1, 1, 1), "Mouse xrel: %d, yrel: %d", Input->Mouse.xrel, Input->Mouse.yrel);
	u32 DebugTextVertCount = (u32)SpriteVertices.size() - DummyMapVertSize;

	rndr::VertexBufferData(SpriteVertexBuffer, 0, (u32)SpriteVertices.size() * sizeof(vert_P1C1UV1), &SpriteVertices.front());

	cmd::draw *DummyMapCmd = SpriteCmdList->AddCommand<cmd::draw>(1, 0);
	DummyMapCmd->VertexBuffer = SpriteVertexBuffer;
	DummyMapCmd->VertexFormat = vert_format::P1C1UV1;
	DummyMapCmd->StartVertex = 0;
	DummyMapCmd->VertexCount = DummyMapVertSize;
	DummyMapCmd->Textures[0] = DefaultTileTexture->RenderResource;
	DummyMapCmd->ShaderProgram = SpriteShader;

	cmd::draw *DebugTextCmd = HUDCmdList->AddCommand<cmd::draw>(2, 0);
	DebugTextCmd->VertexBuffer = SpriteVertexBuffer;
	DebugTextCmd->VertexFormat = vert_format::P1C1UV1;
	DebugTextCmd->StartVertex = DebugTextStartVert;
	DebugTextCmd->VertexCount = DebugTextVertCount;
	DebugTextCmd->Textures[0] = DebugFont->TextureAtlas.Texture.RenderResource;
	DebugTextCmd->ShaderProgram = SpriteShader;

	SpriteCmdList->Sort();
	SpriteCmdList->Submit();
	SpriteCmdList->Flush();

	HUDCmdList->Sort();
	HUDCmdList->Submit();
	HUDCmdList->Flush();

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