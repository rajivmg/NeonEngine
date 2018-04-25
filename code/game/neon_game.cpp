#include "neon_game.h"

#include "neon_font.h"
#include "neon_primitive_mesh.h"
#include "neon_mesh.h"

struct camera
{
	// TODO: Merge projection and view matrix in one??
	vec3	P;			// Position of the camera
	vec3	Target;		// Target of the camera
	r32		Distance;	// Distance of the camera from target
	r32		Pitch;		// Pitch of the camera
	r32		Yaw;		// Yaw of the camera
	mat4	Matrix;		// Camera matrix
};

static
void UpdateCamera3D(game_input *Input, camera *Camera, 
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
	
	static bool LockPitch = true, LockYaw = false;

	ImGui::Begin("Camera");
	ImGui::Checkbox("Lock Pitch", &LockPitch);
	ImGui::SameLine();
	ImGui::Checkbox("Lock Yaw", &LockYaw);
	if(ImGui::Button("Reset Camera"))
	{
		Camera->Yaw = 0.0f;
		Camera->Pitch = 0.0f;
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
	ImGui::End();
}

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

	local_persist render_cmd_list *CmdList;
	local_persist render_cmd_list *SpriteCmdList;
	local_persist render_cmd_list *HUDCmdList;

	local_persist render_resource SpriteVertexBuffer;
	local_persist render_resource SpriteShader;
	
	local_persist font *DebugFont = new font;

	local_persist texture *DefaultTileTexture = new texture;
	local_persist texture *WhiteTexture = new texture;
	local_persist texture *LucyDiffuseTexture= new texture;

	local_persist render_resource VertexBuffer;
	local_persist render_resource IndexBuffer;
	local_persist render_resource Shader;
	local_persist u32 IndicesCount;

	local_persist render_resource DebugVertexBuffer;

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

		LucyDiffuseTexture->LoadFile("models/lucy/lucy_diffuse.tga", texture_type::TEXTURE_2D, texture_filter::NEAREST, texture_wrap::CLAMP, true);
		//LucyDiffuseTexture->LoadFile("models/lucy/stone_texture.tga", texture_type::TEXTURE_2D, texture_filter::NEAREST, texture_wrap::CLAMP, true);
		LucyDiffuseTexture->CreateRenderResource();

		// Setup shader
		Shader = rndr::MakeShaderProgram("shaders/pbr_vs.glsl", "shaders/pbr_ps.glsl");
		SpriteShader = rndr::MakeShaderProgram("shaders/basic_vs.glsl", "shaders/basic_ps.glsl");

		SpriteCmdList = new render_cmd_list(MEGABYTE(1), SpriteShader);
		HUDCmdList = new render_cmd_list(MEGABYTE(1), SpriteShader);
		CmdList = new render_cmd_list(MEGABYTE(1), Shader);

		SpriteCmdList->ViewMatrix = LookAt(vec3(MapDx, 0, 0), vec3(MapDx, 0, -1), vec3i(0, 1, 0));
		SpriteCmdList->ProjectionMatrix = Orthographic(0, 20.0f, PixelsToMeters * Platform.Height, 0, -1, 1);

		HUDCmdList->ViewMatrix = LookAt(vec3(0, 0, 0), vec3i(0, 0, -1), vec3i(0, 1, 0));
		HUDCmdList->ProjectionMatrix = Screenspace(Platform.Width, Platform.Height);

		DefaultTileTexture->LoadFile("default_tile.tga", texture_type::TEXTURE_2D, texture_filter::NEAREST, texture_wrap::CLAMP, true);
		DefaultTileTexture->CreateRenderResource();
		DefaultTileTexture->FreeContentMemory();

		DebugFont->Load("fonts/Inconsolata/Inconsolata-Regular.ttf", 16);

		SpriteVertexBuffer = rndr::MakeVertexBuffer(MEGABYTE(1), false);

		UpdateCamera3D(Input, &Camera, true, vec3i(0, 150, 0), 150.0f, DEG2RAD(0), DEG2RAD(45));
		CmdList->ViewMatrix = Camera.Matrix;
		CmdList->ProjectionMatrix = Perspective(DEG2RAD(105), (r32)Platform.Width / Platform.Height, 0.1f, 500.0f);

		// Load Model
		std::vector<vert_P1N1UV1> LucyVertices;
		std::vector<vert_index> LucyIndices;
		PushMesh(&LucyVertices, &LucyIndices, "models/lucy/lucy.fbx", 0);
		
		// Setup buffers
		VertexBuffer = rndr::MakeVertexBuffer((u32)LucyVertices.size() * sizeof(vert_P1N1UV1), false);
		rndr::VertexBufferData(VertexBuffer, 0, (u32)LucyVertices.size() * sizeof(vert_P1N1UV1), &LucyVertices.front());

		IndexBuffer = rndr::MakeIndexBuffer((u32)LucyIndices.size() * sizeof(vert_index), false);
		IndicesCount = (u32)LucyIndices.size(); // Count of indices to draw
		rndr::IndexBufferData(IndexBuffer, 0, (u32)LucyIndices.size() * sizeof(vert_index), &LucyIndices.front());

		DebugVertexBuffer = rndr::MakeVertexBuffer(MEGABYTE(1), true);

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
	PushTextSprite(&SpriteVertices, DebugFont, vec3i(150, 720, 1), vec4i(1, 1, 1, 1), "Pitch: %f, Yaw: %f", Camera.Pitch, Camera.Yaw);
	u32 DebugTextVertCount = (u32)SpriteVertices.size() - DummyMapVertSize;

	rndr::VertexBufferData(SpriteVertexBuffer, 0, (u32)SpriteVertices.size() * sizeof(vert_P1C1UV1), &SpriteVertices.front());

	cmd::draw *DummyMapCmd = SpriteCmdList->AddCommand<cmd::draw>(1);
	DummyMapCmd->VertexBuffer = SpriteVertexBuffer;
	DummyMapCmd->VertexFormat = vert_format::P1C1UV1;
	DummyMapCmd->StartVertex = 0;
	DummyMapCmd->VertexCount = DummyMapVertSize;
	DummyMapCmd->Textures[0] = DefaultTileTexture->RenderResource;

	cmd::draw *DebugTextCmd = HUDCmdList->AddCommand<cmd::draw>(2);
	DebugTextCmd->VertexBuffer = SpriteVertexBuffer;
	DebugTextCmd->VertexFormat = vert_format::P1C1UV1;
	DebugTextCmd->StartVertex = DebugTextStartVert;
	DebugTextCmd->VertexCount = DebugTextVertCount;
	DebugTextCmd->Textures[0] = DebugFont->TextureAtlas.Texture.RenderResource;

	SpriteCmdList->Sort();
	SpriteCmdList->Submit();
	SpriteCmdList->Flush();

	HUDCmdList->Sort();
	HUDCmdList->Submit();
	HUDCmdList->Flush();

	cmd::draw_indexed *Lucy = CmdList->AddCommand<cmd::draw_indexed>(2);
	Lucy->VertexBuffer = VertexBuffer;
	Lucy->VertexFormat = vert_format::P1N1UV1;
	Lucy->IndexBuffer = IndexBuffer;
	Lucy->IndexCount = IndicesCount;
	Lucy->Textures[0] = LucyDiffuseTexture->RenderResource;

	CmdList->Sort();
	CmdList->Submit();
	CmdList->Flush();
}