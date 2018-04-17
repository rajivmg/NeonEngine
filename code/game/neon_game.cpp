#include "neon_game.h"

#include "neon_font.h"
#include "neon_primitive_mesh.h"
#include "neon_mesh.h"

struct camera
{
	vec3 Target;
	vec3 P;
	mat4 Matrix;
	r32 Distance; // Distance from the origin 
	r32 Pitch; // In radians
	r32 Yaw; // In radians 
};

static void UpdateCamera3D(game_input *Input, camera *Camera, bool SetCamera = false, vec3 _Target = vec3i(0,0,0), r32 _Distance = 25.0f, r32 _Pitch = DEG2RAD(00.0f), r32 _Yaw = DEG2RAD(0.0f))
{
	auto MPiToPi = [](vec2 MouseRel)
	{
		vec2 Center = vec2i(Platform.Width / 2, Platform.Height / 2);
		return vec2((2*M_PI/Platform.Width) * (MouseRel.x + Center.x) - M_PI,
					(2*M_PI/Platform.Height) * (MouseRel.y + Center.y) - M_PI);
	};

	if(SetCamera)
	{
		Camera->Target = _Target;
		Camera->Distance = _Distance;
		Camera->Pitch = _Pitch;
		Camera->Yaw = _Yaw;
	}
	if(Input->Mouse.Left.EndedDown && (Input->Mouse.xrel != 0 || Input->Mouse.yrel != 0))
	{
		vec2 RemapedRel = MPiToPi(vec2i(Input->Mouse.xrel, Input->Mouse.yrel));
		Camera->Pitch -= RemapedRel.y * 0.5f;
		Camera->Yaw -= RemapedRel.x * 0.5f;

		//Camera->Pitch = Clamp(0.0f, Camera->Pitch, 2*M_PI);
		//Camera->Yaw = Clamp(0.0f, Camera->Yaw, 2*M_PI);
	}
	if(Input->Mouse.Middle.EndedDown && (Input->Mouse.yrel != 0))
	{
		Camera->Distance -= Input->Mouse.yrel;
	}

	Camera->P.x = Camera->Target.x + Camera->Distance * cosf(Camera->Pitch) * sinf(Camera->Yaw);
	Camera->P.y = Camera->Target.y + Camera->Distance * sinf(Camera->Pitch) * sinf(Camera->Yaw);
	Camera->P.z = Camera->Target.z + Camera->Distance * cos(Camera->Yaw);

	Camera->Matrix = LookAt(Camera->P, Camera->Target, vec3i(0, 1, 0));
	Platform.Log("CamPos: (%f, %f, %f)", Camera->P.x, Camera->P.y, Camera->P.z);
}

/*
static void UpdateCamera3D(game_input *Input, camera *Camera, bool SetCamera = false, vec3 CameraPositon = vec3(0.0f, 0.0f, 1.0f))
{
	auto ArcballVector = [](s32 MouseRelX, s32 MouseRelY)
	{
		vec3 Point;

		// Radius based movement
		//r32 Radius = (r32)MIN(Platform.Width / 2, Platform.Height / 2);
		//vec2 Center = vec2i(Platform.Width / 2, Platform.Height / 2);
		//Point.x = (MouseRelX - Center.x) / Radius;
		//Point.y = (MouseRelY - Center.y) / Radius;
		//Point.z = 0.0f;

		Point.x = 1.0f * MouseRelX / Platform.Width  * 2 - 1.0f;
		Point.y = 1.0f * MouseRelY / Platform.Height * 2 - 1.0f;
		Point.z = 0.0f;

		r32 R = Point.x * Point.x + Point.y * Point.y;
		if(R > 1.0)
		{
			Point = Normalize(Point);
		}
		else
		{
			Point.z = sqrt(1.0f - R);
		}
		return Point;
	};

	if(SetCamera)
	{
		Camera->Target = vec3(0.0f);
		Camera->P = CameraPositon;
		Camera->Matrix = LookAt(Camera->P, Camera->Target, vec3i(0, 1, 0));
	}

	if(Input->Mouse.Left.EndedDown && (Input->Mouse.xrel != 0 || Input->Mouse.yrel != 0))
	{
		vec3 P0 = ArcballVector(Input->Mouse.x + Input->Mouse.xrel, Input->Mouse.y + Input->Mouse.yrel);
		vec3 P1 = ArcballVector(Input->Mouse.x, Input->Mouse.y);

		r32 CamRotAngle = acosf(MIN(1.0f, Dot(P0, P1)));
		vec3 CamRotAxis = Cross(P0, P1);

		vec4 CamPos;
		CamPos.xyz = Camera->P; CamPos.w = 1.0f;
		CamPos = Rotation(CamRotAxis, CamRotAngle) * CamPos; // CamRotAxis vector will be normalized by Rotation() implicitly.
		//assert(!isnan(CamPos.x) && !isnan(CamPos.y) && !isnan(CamPos.z));
		Camera->P = CamPos.xyz;
		Camera->Matrix = LookAt(Camera->P, Camera->Target, vec3i(0, 1, 0));

		Platform.Log("CamPos: (%f, %f, %f)", Camera->P.x, Camera->P.y, Camera->P.z);
	}
}
*/

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

	local_persist camera Camera;

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

		SuzanneTexture->LoadFile("models/lucy/lucy_angle_diffuse.tga", texture_type::TEXTURE_2D, texture_filter::NEAREST, texture_wrap::CLAMP, true);
		SuzanneTexture->CreateRenderResource();

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

		UpdateCamera3D(Input, &Camera, true, vec3i(0,150,0), 150.0f);
		//UpdateCamera3D(Input, &Camera, true, vec3i(0, 0, 150));
		CmdList->ViewMatrix = Camera.Matrix;
		CmdList->ProjectionMatrix = Perspective(DEG2RAD(105), (r32)Platform.Width / Platform.Height, 0.1f, 500.0f);

		std::vector<vert_P1C1UV1> SuzanneVertices;
		std::vector<vert_index> SuzanneIndices;
		PushMesh(&SuzanneVertices, &SuzanneIndices, "models/lucy/lucy_angle.obj", 0);
		//PushMesh(&SuzanneVertices, &SuzanneIndices, "models/suzanne.fbx", 0);
		SuzanneVertexBuffer = rndr::MakeVertexBuffer((u32)SuzanneVertices.size() * sizeof(vert_P1C1UV1), false);
		rndr::VertexBufferData(SuzanneVertexBuffer, 0, (u32)SuzanneVertices.size() * sizeof(vert_P1C1UV1), &SuzanneVertices.front());
		SuzanneIndexBuffer = rndr::MakeIndexBuffer((u32)SuzanneIndices.size() * sizeof(vert_index), false);
		rndr::IndexBufferData(SuzanneIndexBuffer, 0, (u32)SuzanneIndices.size() * sizeof(vert_index), &SuzanneIndices.front());
		SuzanneShader = rndr::MakeShaderProgram("shaders/model_vs.glsl", "shaders/model_ps.glsl");
		SuzanneIndexCount = (u32)SuzanneIndices.size();
		DebugVertexBuffer = rndr::MakeVertexBuffer(MEGABYTE(1), false);

		int BreakPoint = 0000;
	}
	SpriteCmdList->ViewMatrix = LookAt(vec3(MapDx, MapDy, 0), vec3(MapDx, MapDy, -1), vec3i(0, 1, 0));

	UpdateCamera3D(Input, &Camera);
	CmdList->ViewMatrix = Camera.Matrix;

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
	//PushTextSprite(&SpriteVertices, DebugFont, vec3i(150, 720, 1), vec4i(1, 1, 1, 1), "Mouse xrel: %d, yrel: %d", Input->Mouse.xrel, Input->Mouse.yrel);
	//PushTextSprite(&SpriteVertices, DebugFont, vec3i(350, 720, 1), vec4i(1, 1, 1, 1), "Mouse x: %d, y: %d", Input->Mouse.x, Input->Mouse.y);
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
	SuzanneDraw->IndexCount = SuzanneIndexCount;
	SuzanneDraw->Textures[0] = SuzanneTexture->RenderResource;
	SuzanneDraw->ShaderProgram = SuzanneShader;

	CmdList->Sort();
	CmdList->Submit();
	CmdList->Flush();
}