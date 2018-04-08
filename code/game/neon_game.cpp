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
	local_persist texture *SuzanneTexture = new texture;

	local_persist render_resource SuzanneVertexBuffer;
	local_persist render_resource SuzanneIndexBuffer;
	local_persist render_resource SuzanneShader;

	local_persist render_resource DebugVertexBuffer;

	local_persist u32 SuzanneIndexCount;

	r32 MetersToPixels = Platform.Width / 20.0f;
	r32 PixelsToMeters = 1.0f / MetersToPixels;

	//if(Input->Mouse.Left.EndedDown)
	//{
	//	MapDx -= Input->Mouse.xrel * PixelsToMeters;
	//	MapDy -= Input->Mouse.yrel * PixelsToMeters;
	//}

	local_persist bool OnceUponAGame = false;
	if(!OnceUponAGame)
	{
		OnceUponAGame = true;

		WhiteTexture->LoadFile("white_texture.tga", texture_type::TEXTURE_2D, texture_filter::NEAREST, texture_wrap::CLAMP, false);
		WhiteTexture->CreateRenderResource();

		//SuzanneTexture->LoadFile("dwarf/dwarf_diffuse.tga", texture_type::TEXTURE_2D, texture_filter::NEAREST, texture_wrap::CLAMP, true);
		//SuzanneTexture->CreateRenderResource();

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

		CmdList->ViewMatrix = LookAt(vec3i(0, 0, 2), vec3i(0, 0, 0), vec3i(0, 1, 0));
		CmdList->ProjectionMatrix = Perspective(DEG2RAD(105), (r32)Platform.Width / Platform.Height, 0.1f, 100.0f);

		std::vector<vert_P1C1UV1> SuzanneVertices;
		std::vector<vert_index> SuzanneIndices;
		//PushMesh(&SuzanneVertices, &SuzanneIndices, "dwarf/dwarf.fbx", 0);
		PushMesh(&SuzanneVertices, &SuzanneIndices, "models/suzanne.fbx", 0);
		SuzanneVertexBuffer = rndr::MakeVertexBuffer((u32)SuzanneVertices.size() * sizeof(vert_P1N1UV1), false);
		rndr::VertexBufferData(SuzanneVertexBuffer, 0, (u32)SuzanneVertices.size() * sizeof(vert_P1N1UV1), &SuzanneVertices.front());
		SuzanneIndexBuffer = rndr::MakeIndexBuffer((u32)SuzanneIndices.size() * sizeof(vert_index), false);
		rndr::IndexBufferData(SuzanneIndexBuffer, 0, (u32)SuzanneIndices.size() * sizeof(vert_index), &SuzanneIndices.front());
		SuzanneShader = rndr::MakeShaderProgram("shaders/model_vs.glsl", "shaders/model_ps.glsl");
		SuzanneIndexCount = SuzanneIndices.size();
		DebugVertexBuffer = rndr::MakeVertexBuffer(MEGABYTE(1), false);

		int BreakPoint = 0000;
	}
	SpriteCmdList->ViewMatrix = LookAt(vec3(MapDx, MapDy, 0), vec3(MapDx, MapDy, -1), vec3i(0, 1, 0));
	
	// Arcball Start
	auto ArcballPoint = [](s32 MouseX, s32 MouseY)
	{
		r32 Radius = (r32)MIN(Platform.Width / 2, Platform.Height / 2);
		vec2 Center = vec2i(Platform.Width / 2, Platform.Height / 2);
		vec3 Point;
		Point.x = (MouseX - Center.x) / Radius;
		Point.y = (MouseY - Center.y) / Radius;
		Point.z = 0.0f;
		r32 R = Point.x * Point.x + Point.y * Point.y;
		if(R > 1.0)
		{
			Point = Normalize(Point);
		}
		else
		{
			Point.z = sqrt(1.0 - R);
		}
		return Point;
	};

	r32 CamRotAngle = 0;
	vec3 CamRotAxis = vec3i(0, 0, 0);
	static vec4 CamPos = vec4i(0, 0, 1, 0);

	if(Input->Mouse.Left.EndedDown && (Input->Mouse.xrel != 0 || Input->Mouse.yrel != 0))
	{
		vec3 P0 = ArcballPoint(Input->Mouse.x + Input->Mouse.xrel, Input->Mouse.y + Input->Mouse.yrel);
		vec3 P1 = ArcballPoint(Input->Mouse.x, Input->Mouse.y);

		CamRotAngle = acosf(MIN(1.0f, Dot(P0, P1)));
		CamRotAxis = Cross(P0, P1);
		//CamRotAxis = Normalize(CamRotAxis); // CamRotAxis vector will be normalized in implicitly in Rotation()

		CamPos = Rotation(CamRotAxis, CamRotAngle) * vec4(CamPos.x, CamPos.y, CamPos.z, 1.0f);
		CmdList->ViewMatrix = LookAt(CamPos.xyz, vec3i(0, 0, 0), vec3i(0, 1, 0));
		//CmdList->ViewMatrix = LookAt(vec3i(0, 0, 1), vec3i(0, 0, 0), vec3i(0, 1, 0)) * Rotation(CamRotAxis, CamRotAngle);
		/*Platform.Log("Cam RotAxis: (%f, %f, %f) RotAngle: %f", CamRotAxis.x, CamRotAxis.y, CamRotAxis.z, CamRotAngle);*/
		Platform.Log("CamPos: (%f, %f, %f)", CamPos.x, CamPos.y, CamPos.z);
	}
	// Arcball end

	vec3 AP = ArcballPoint(Input->Mouse.x, Input->Mouse.y);
	std::vector<vert_P1C1UV1> DebugSpriteVertices;
	r32 Radius = (r32)MIN(Platform.Width / 2, Platform.Height / 2);

	for(int A = 0; A <= 360; ++A)
	{
		vec2 CP = vec2(cosf(DEG2RAD(A)), sinf(DEG2RAD(A)));
		PushSprite(&DebugSpriteVertices, vec3((Platform.Width / 2) + CP.x*Radius - 2.0f, (Platform.Height / 2) + CP.y*Radius - 2.0f, 1.0f), vec2i(4, 4), vec4i(1, 1, 0, 1), vec4i(0, 0, 1, 1));
	}

	PushSprite(&DebugSpriteVertices, vec3((Platform.Width/2) + AP.x*Radius - 4.0f, (Platform.Height/2) + AP.y*Radius - 4.0f, 1.0f), vec2i(8, 8), vec4i(1, 0, 0, 1), vec4i(0, 0, 1, 1));
	rndr::VertexBufferData(DebugVertexBuffer, 0, DebugSpriteVertices.size() * sizeof(vert_P1C1UV1), &DebugSpriteVertices.front());
	//

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
	PushTextSprite(&SpriteVertices, DebugFont, vec3i(350, 720, 1), vec4i(1, 1, 1, 1), "Mouse x: %d, y: %d", Input->Mouse.x, Input->Mouse.y);
	u32 DebugTextVertCount = (u32)SpriteVertices.size() - DummyMapVertSize;

	rndr::VertexBufferData(SpriteVertexBuffer, 0, (u32)SpriteVertices.size() * sizeof(vert_P1C1UV1), &SpriteVertices.front());

	cmd::draw *DummyMapCmd = SpriteCmdList->AddCommand<cmd::draw>(1, 0);
	DummyMapCmd->VertexBuffer = SpriteVertexBuffer;
	DummyMapCmd->VertexFormat = vert_format::P1C1UV1;
	DummyMapCmd->StartVertex = 0;
	DummyMapCmd->VertexCount = DummyMapVertSize;
	DummyMapCmd->Textures[0] = DefaultTileTexture->RenderResource;
	DummyMapCmd->ShaderProgram = SpriteShader;

	cmd::draw *DebugSpriteCmd = HUDCmdList->AddCommand<cmd::draw>(2, 0);
	DebugSpriteCmd->VertexBuffer = DebugVertexBuffer;
	DebugSpriteCmd->VertexFormat = vert_format::P1C1UV1;
	DebugSpriteCmd->StartVertex = 0;
	DebugSpriteCmd->VertexCount = DebugSpriteVertices.size();
	DebugSpriteCmd->Textures[0] = WhiteTexture->RenderResource;
	DebugSpriteCmd->ShaderProgram = SpriteShader;

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
	SuzanneDraw->IndexCount = SuzanneIndexCount;
	//SuzanneDraw->Textures[0] = SuzanneTexture->RenderResource;
	SuzanneDraw->Textures[0] = WhiteTexture->RenderResource;
	SuzanneDraw->ShaderProgram = SuzanneShader;

	CmdList->Sort();
	CmdList->Submit();
	CmdList->Flush();
}