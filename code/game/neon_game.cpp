#include "neon_game.h"

#include "neon_font.h"
#include "neon_primitive_mesh.h"
#include "neon_mesh.h"

static game_state GameState = {};
static cg_common CgCommon = {};

void UpdateCamera(game_input *Input, camera *Camera, 
				  bool SetCamera = false, vec3 _Target = vec3i(0,0,0), 
				  r32 _Distance = 25.0f, r32 _Pitch = 0.0f, r32 _Yaw = 0.0f)
{
	auto Remap = [](vec2 MouseRel)
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
	
	static bool LockPitch = false, LockYaw = false;

	ImGui::Begin("Camera");
	ImGui::SliderFloat("Distance", &Camera->Distance, 0.0f, 500.0f);
	ImGui::SliderFloat("Target X", &Camera->Target.x, -300.0f, 300.0f);
	ImGui::SliderFloat("Target Y", &Camera->Target.y, -300.0f, 300.0f);
	ImGui::SliderFloat("Target Z", &Camera->Target.z, -300.0f, 300.0f);
	ImGui::Checkbox("Lock Pitch", &LockPitch);
	ImGui::SameLine();
	ImGui::Checkbox("Lock Yaw", &LockYaw);
	if(ImGui::Button("Reset Yaw/Pitch"))
	{
		Camera->Yaw = 0.0f;
		Camera->Pitch = 0.0f;
	}
	ImGui::SameLine();
	if(ImGui::Button("Reset Target"))
	{
		Camera->Target = vec3i(0, 150, 0);
	}
	ImGui::End();

	// Rotation
	if(Input->Mouse.Left.EndedDown && (Input->Mouse.xrel != 0 || Input->Mouse.yrel != 0))
	{
		vec2 MouseRel = Remap(vec2i(Input->Mouse.xrel, Input->Mouse.yrel));
		if(!LockPitch)
		{
			Camera->Pitch -= MouseRel.y;
		}
		if(!LockYaw)
		{
			Camera->Yaw -= MouseRel.x;
		}

		// TODO: These clamping are maybe wrong. FIX IT!!
		if(Camera->Yaw > 2 * M_PI)
		{
			Camera->Yaw = Camera->Yaw - (2 * M_PI);
		}
		if(Camera->Yaw < 0)
		{
			Camera->Yaw = 2 * M_PI - Camera->Yaw;
		}
		Camera->Pitch = Clamp(-M_PI / 2, Camera->Pitch, M_PI / 2);
	}

	// Zoom
	if(Input->Mouse.Middle.EndedDown && (Input->Mouse.yrel != 0))
	{
		Camera->Distance -= Input->Mouse.yrel;
	}

	Camera->P.x = Camera->Target.x + Camera->Distance * cosf(Camera->Pitch) * sinf(Camera->Yaw);
	Camera->P.y = Camera->Target.y + Camera->Distance * sin(Camera->Pitch);
	Camera->P.z = Camera->Target.z + Camera->Distance * cos(Camera->Pitch) * cos(Camera->Yaw);

	Camera->Matrix = LookAt(Camera->P, Camera->Target, vec3i(0, 1, 0));
	//Platform.Log("CamPos: (%f, %f, %f)", Camera->P.x, Camera->P.y, Camera->P.z);
	ImGui::Begin("Camera");
	ImGui::Text("Positon:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1, 0, 0, 1),"%.2f", Camera->P.x);
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0, 1, 0, 1),"%.2f", Camera->P.y);
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0, 0, 1, 1),"%.2f", Camera->P.z);
	ImGui::Text("Yaw: %f Pitch: %f", Camera->Yaw, Camera->Pitch);
	ImGui::End();
}

void CreateTilemap(tilemap *Tilemap, char const *_Filename, vec2 _MapSize, u32 _TileSize)
{
	strncpy(Tilemap->Filename, _Filename, sizeof(Tilemap->Filename));
	Tilemap->MapSize = _MapSize;
	Tilemap->Tileset.LoadFile(_Filename, texture_type::TEXTURE_2D, texture_filter::NEAREST, texture_wrap::CLAMP, true);
	Tilemap->Tileset.CreateRenderResource();
	Tilemap->TileSize = _TileSize;
}

DLLEXPORT
GAME_SETUP(GameSetup)
{
	Platform = _Platform;
	ImGui::SetCurrentContext(_ImGuiCtx);
	rndr::Init();

	// CgCommon
	CgCommon.Time = 0.00f;

	// GameState
	GameState.MetersToPixels = Platform.Width / 16.0f;
	GameState.PixelsToMeters = 1.0f / GameState.MetersToPixels;

	texture WhiteTexture;
	WhiteTexture.LoadFile("wonder.tga", texture_type::TEXTURE_2D, texture_filter::LINEAR, texture_wrap::CLAMP, true);
	GameState.WhiteTexture = rndr::MakeTexture(&WhiteTexture);
	texture WaterDisplacementTexture;
	WaterDisplacementTexture.LoadFile("displacement.tga", texture_type::TEXTURE_2D, texture_filter::NEAREST, texture_wrap::REPEAT, false);
	GameState.WaterDisplacementTexture = rndr::MakeTexture(&WaterDisplacementTexture);
	GameState.WaterShader = rndr::MakeShaderProgram("shaders/water_vs.glsl", "shaders/water_ps.glsl");
	GameState.WaterVertexBuffer = rndr::MakeBuffer(resource_type::VERTEX_BUFFER, MEGABYTE(1), true);
	GameState.Water = new render_cmd_list(MEGABYTE(2), GameState.WaterShader);
	GameState.Water->ViewMatrix = LookAt(vec3(0.0f), vec3i(0, 0, -1), vec3i(0, 1, 0));
	GameState.Water->ProjMatrix = Orthographic(0.0f, 16.0f, GameState.PixelsToMeters * Platform.Height, 0.0f, -10.0f, 10.0f);
	GameState.CommonConstBuffer = rndr::MakeBuffer(resource_type::CONSTANT_BUFFER, (u32)sizeof(cg_common), true);
	rndr::BufferData(GameState.CommonConstBuffer, 0, sizeof(cg_common), &CgCommon);
	rndr::BindBuffer(GameState.CommonConstBuffer, 0);
}

DLLEXPORT
GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
//-----------------------------------------------------------------------------
// Game Update
//-----------------------------------------------------------------------------
	CgCommon.Time = Input->Time / 20000;
	ImGui::Begin("Debug");
	ImGui::Text("CgCommon.Time = %f", CgCommon.Time);
	ImGui::End();

//-----------------------------------------------------------------------------
// Game Render
//-----------------------------------------------------------------------------
	rndr::Clear();

	ImGui::Begin("Debug");
	ImGui::Text("%0.2f ms/frame", 1000.0f * Input->FrameTime);
	ImGui::End();

	std::vector<vert_P1C1UV1> SpritesVertices;

	PushSprite(&SpritesVertices, vec3i(0, 0, 0), vec2i(9, 9), vec4(1, 1, 1, 1), vec4(0, 0, 1.0, 1.0));
	rndr::BufferData(GameState.WaterVertexBuffer, 0, sizeof(vert_P1C1UV1) * (u32)SpritesVertices.size(), &SpritesVertices.front());

	cmd::copy_const_buffer *CopyConstBuffer = GameState.Water->AddCommand<cmd::copy_const_buffer>(0, sizeof(cg_common));
	CopyConstBuffer->ConstantBuffer = GameState.CommonConstBuffer;
	CopyConstBuffer->Data = GetCmdPacket(CopyConstBuffer)->AuxMemory;
	memcpy(CopyConstBuffer->Data, &CgCommon, sizeof(cg_common));
	CopyConstBuffer->Size = sizeof(cg_common);

	cmd::draw *WaterSurface = GameState.Water->AppendCommand<cmd::draw>(CopyConstBuffer);
	WaterSurface->StartVertex = 0;
	WaterSurface->VertexBuffer = GameState.WaterVertexBuffer;
	WaterSurface->VertexFormat = vert_format::P1C1UV1;
	WaterSurface->StartVertex = 0;
	WaterSurface->VertexCount = (u32)SpritesVertices.size();
	WaterSurface->Textures[0] = GameState.WhiteTexture;
	WaterSurface->Textures[1] = GameState.WaterDisplacementTexture;

	GameState.Water->Sort();
	GameState.Water->Submit();
	GameState.Water->Flush();
}