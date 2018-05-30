#include "neon_game.h"

#include "neon_mesh.h"

static game_state GameState = {};

void GenFloorTiles(room *Room, u32 *RoomMap)
{
	Room->FloorTilesCount = Room->SizeX * Room->SizeY;
	Room->FloorTiles = (floor_tile *)malloc(sizeof(floor_tile) * Room->FloorTilesCount);
	
	for(u32 Y = 0; Y < Room->SizeY; ++Y)
	{
		for(u32 X = 0; X < Room->SizeX; ++X)
		{
			floor_tile *FloorTile = (Room->FloorTiles + X + Y * Room->SizeX);
			FloorTile->P = vec3i(X, Y, 0);
		}
	}
}

void GenRoom(room *Room)
{
	/**
	 * Empty Space	: 0
	 * Block		: 1
	 * Enemy		: 2
	 * Portal		: 3
	 * Player		: 4
	 **/

	Room->SizeX = 7;
	Room->SizeY = 7;

	static u32 Map[49] =
	{
		1, 1, 1, 1, 1, 1, 1,
		1, 3, 0, 0, 0, 0, 1,
		1, 0, 1, 2, 0, 0, 1,
		1, 0, 0, 1, 0, 0, 1,
		1, 0, 0, 0, 0, 0, 1,
		1, 0, 4, 1, 0, 0, 1,
		1, 1, 1, 1, 1, 1, 1,
	};

	u32 Tiles[49];

	// Copy 2D array upside-down. Output origin lower-left 
	for(u32 Y = 0; Y < Room->SizeY; ++Y)
	{
		for(u32 X = 0; X < Room->SizeX; ++X)
		{
			*(Tiles + X + Y * Room->SizeX) = *(Map + X + (Room->SizeY - Y  - 1) * Room->SizeX);
		}
	}

	GenFloorTiles(Room, Tiles);

	// Gen Slot Map
	Room->SlotMap = (u32 *)malloc(Room->SizeX * Room->SizeY * sizeof(u32));
	memset(Room->SlotMap, 0, Room->SizeX * Room->SizeY * sizeof(u32));

	Room->EntityCount = 0;
	// Fill Entities array
	// origin lower-left 
	for(u32 Y = 0; Y < Room->SizeX; ++Y)
	{
		for(u32 X = 0; X < Room->SizeX; ++X)
		{
			switch(*(Tiles + X + Y * Room->SizeX))
			{
				case 1:
				{
					assert(Room->EntityCount < ARRAY_COUNT(room::Entities));
					// Room->Entities[Room->EntityCount] = {}; // Zero initialization is implict 
					Room->Entities[Room->EntityCount].Type = entity_type::Entity_Block;
					Room->Entities[Room->EntityCount].P = vec3i(X, Y, 1);
					Room->Entities[Room->EntityCount].X = X;
					Room->Entities[Room->EntityCount].Y = Y;
					Room->Entities[Room->EntityCount].IsMoving = false;
					Room->Entities[Room->EntityCount].Collide = true;

					*(Room->SlotMap + X + Y * Room->SizeX) = true;

					++Room->EntityCount;
				} break;

				case 2:
				{
					assert(Room->EntityCount < ARRAY_COUNT(room::Entities));
					Room->Entities[Room->EntityCount].Type = entity_type::Entity_Enemy1;
					Room->Entities[Room->EntityCount].P = vec3i(X, Y, 2);
					Room->Entities[Room->EntityCount].X = X;
					Room->Entities[Room->EntityCount].Y = Y;
					Room->Entities[Room->EntityCount].IsMoving = false;
					Room->Entities[Room->EntityCount].Collide = true;

					*(Room->SlotMap + X + Y * Room->SizeX) = true;

					++Room->EntityCount;
				} break;

				case 3:
				{
					assert(Room->EntityCount < ARRAY_COUNT(room::Entities));
					Room->Entities[Room->EntityCount].Type = entity_type::Entity_Portal;
					Room->Entities[Room->EntityCount].P = vec3i(X, Y, 3);
					Room->Entities[Room->EntityCount].X = X;
					Room->Entities[Room->EntityCount].Y = Y;
					Room->Entities[Room->EntityCount].IsMoving = false;
					Room->Entities[Room->EntityCount].Collide = false;

					*(Room->SlotMap + X + Y * Room->SizeX) = false;

					++Room->EntityCount;
				} break;

				case 4:
				{
					assert(Room->EntityCount < ARRAY_COUNT(room::Entities));
					Room->Entities[Room->EntityCount].Type = entity_type::Entity_Player;
					Room->Entities[Room->EntityCount].P = vec3i(X, Y, 4);
					Room->Entities[Room->EntityCount].X = X;
					Room->Entities[Room->EntityCount].Y = Y;
					Room->Entities[Room->EntityCount].IsMoving = false;
					Room->Entities[Room->EntityCount].Collide = true;

					*(Room->SlotMap + X + Y * Room->SizeX) = true;

					Room->Player = &Room->Entities[Room->EntityCount];

					++Room->EntityCount;
				} break;
			}
		}
	}
}

inline
bool MoveEntity(entity *Entity, vec3 EntityNewP)
{
	if(!Entity->IsMoving)
	{

	}
	return true;
}

void SimRoom(game_input *Input, room *Room)
{
	game_controller_input *Controller = &Input->Controllers[0];

	auto IsTileEmptyForPlayer = [Room](vec3 P)
	{
		bool IsEmpty = true;
		// Check player collision
		for(u32 I = 0; I < Room->EntityCount; ++I)
		{
			entity *Entity = &Room->Entities[I];
			if(Entity->Collide && Entity->Type != entity_type::Entity_Player)
			{
				if(Entity->P.x == P.x && Entity->P.y == P.y)
				{
					IsEmpty = false;
					break;
				}
			}
		}

		return IsEmpty;
	};
	static bool CheckPlayerNewP = false;
	static vec3 PlayerLastP, PlayerNewP;
	static r32 PlayerMoveStartTime;
	if(!Room->Player->IsMoving)
	{
		if(Controller->Up.EndedDown && Controller->Up.HalfTransitionCount == 1)
		{
			PlayerLastP = Room->Player->P;
			PlayerNewP = Room->Player->P;

			PlayerNewP.y += 1;

			CheckPlayerNewP = true;
		}
		if(Controller->Down.EndedDown && Controller->Down.HalfTransitionCount == 1)
		{
			PlayerLastP = Room->Player->P;
			PlayerNewP = Room->Player->P;

			PlayerNewP.y -= 1;

			CheckPlayerNewP = true;
		}
		if(Controller->Left.EndedDown && Controller->Left.HalfTransitionCount == 1)
		{
			PlayerLastP = Room->Player->P;
			PlayerNewP = Room->Player->P;

			PlayerNewP.x -= 1;

			CheckPlayerNewP = true;
		}
		if(Controller->Right.EndedDown && Controller->Right.HalfTransitionCount == 1)
		{
			PlayerLastP = Room->Player->P;
			PlayerNewP = Room->Player->P;

			PlayerNewP.x += 1;

			CheckPlayerNewP = true;
		}
	}

	if(CheckPlayerNewP)
	{
		if(IsTileEmptyForPlayer(PlayerNewP))
		{
			Room->Player->X = (u32)PlayerNewP.x;
			Room->Player->Y = (u32)PlayerNewP.y;
			Room->Player->IsMoving = true;
			PlayerMoveStartTime = Input->Time;
		}
		else
		{
			Room->Player->IsMoving = false;
		}

		CheckPlayerNewP = false;
	}

	if(Room->Player->IsMoving)
	{
		r32 Perc = (Input->Time - PlayerMoveStartTime) / 190.0f; //225
		if(Perc >= 1.0f)
		{
			Perc = Clamp(0.0f, Perc, 1.0f);
			Room->Player->IsMoving = false;
		}
		Room->Player->P = Lerp(PlayerLastP, Perc, PlayerNewP);
	}
	// 
}

DLLEXPORT
GAME_SETUP(GameSetup)
{
	// Set pointers and init renderer
	Platform = _Platform;
	ImGui::SetCurrentContext(_ImGuiCtx);
	rndr::Init();

	// GameState
	GenRoom(&GameState.Room);

	u32 SizeY = GameState.Room.SizeY;
	r32 TilesPerCol = (r32)SizeY;
	GameState.MetersToPixels = Platform.Height / TilesPerCol;
	GameState.PixelsToMeters = 1.0f / GameState.MetersToPixels;

	GameState.RoomCenterOffset = vec3(((Platform.Width * GameState.PixelsToMeters) - GameState.Room.SizeX) / 2, 0, 0);

	InitFont(&GameState.DebugFont, "fonts/Inconsolata/Inconsolata-Regular.ttf", 20);

	bitmap WhiteBitmap;
	LoadBitmap(&WhiteBitmap, "sprites/white_texture.tga");
	GameState.WhiteTexture = rndr::MakeTexture(&WhiteBitmap, texture_type::TEXTURE_2D, texture_filter::NEAREST, texture_wrap::CLAMP, true);

	GameState.SpriteShader = rndr::MakeShaderProgram("shaders/sprite_vs.glsl", "shaders/sprite_ps.glsl");
	GameState.TilesVertexBuffer = rndr::MakeBuffer(resource_type::VERTEX_BUFFER, MEGABYTE(1), true);

	GameState.TileRenderList = new render_cmd_list(MEGABYTE(1), GameState.SpriteShader);
	GameState.TileRenderList->ViewMatrix = LookAt(vec3(0.0f), vec3i(0, 0, -1), vec3i(0, 1, 0));
	GameState.TileRenderList->ProjMatrix = Orthographic(0, GameState.PixelsToMeters * Platform.Width, TilesPerCol, 0, -10, 0);

	GameState.TextShader = rndr::MakeShaderProgram("shaders/sprite_vs.glsl", "shaders/sprite_ps.glsl");
	GameState.DebugTextVertexBuffer = rndr::MakeBuffer(resource_type::VERTEX_BUFFER, MEGABYTE(1), true);
	GameState.DebugTextCmdList = new render_cmd_list(MEGABYTE(1), GameState.TextShader);
	GameState.DebugTextCmdList->ViewMatrix = LookAt(vec3(0.0f), vec3i(0, 0, -1), vec3i(0, 1, 0));
	GameState.DebugTextCmdList->ProjMatrix = Orthographic(0.0f, (r32)Platform.Width, (r32)Platform.Height, 0.0f, -1.0f, 1.0f);
}

DLLEXPORT
GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
//-----------------------------------------------------------------------------
// Game Update
//-----------------------------------------------------------------------------

	SimRoom(Input, &GameState.Room);

//-----------------------------------------------------------------------------
// Game Render
//-----------------------------------------------------------------------------
	rndr::Clear();

	ImGui::Begin("Debug");
	ImGui::Text("%0.2f ms/frame", 1000.0f * Input->FrameTime);
	ImGui::End();

	std::vector<vert_P1C1UV1> TilesVertices;
	std::vector<vert_P1C1UV1> DebugTextVertices;

	for(u32 I = 0; I < GameState.Room.EntityCount; ++I)
	{
			entity *Entity = GameState.Room.Entities + I;
			switch(Entity->Type)
			{
				case entity_type::Entity_Block:
				{
					PushSprite(&TilesVertices, Entity->P + GameState.RoomCenterOffset, vec2i(1, 1), vec4(0.525f, 0.509f, 0.529f, 1.0f), vec4(0, 0, 1, 1));
				} break;

				case entity_type::Entity_Enemy1:
				{
					PushSprite(&TilesVertices, Entity->P + GameState.RoomCenterOffset, vec2i(1, 1), vec4(0.9f, 0.0f, 0.0f, 1.0f), vec4(0, 0, 1, 1));
				} break;

				case entity_type::Entity_Portal:
				{
					PushSprite(&TilesVertices, Entity->P + GameState.RoomCenterOffset, vec2i(1, 1), vec4(0.5f, 0.0f, 1.0f, 1.0f), vec4(0, 0, 1, 1));
				} break;

				case entity_type::Entity_Player:
				{
					PushSprite(&TilesVertices, Entity->P + GameState.RoomCenterOffset, vec2i(1, 1), vec4(0.0f, 0.9f, 0.0f, 1.0f), vec4(0, 0, 1, 1));
				} break;
			}
	}

	// Render FloorTiles
	for(u32 I = 0; I < GameState.Room.FloorTilesCount; ++I)
	{
		PushSprite(&TilesVertices, GameState.Room.FloorTiles[I].P + GameState.RoomCenterOffset, vec2i(1, 1), vec4(0.7f, 0.5f, 0.5, 1.0f), vec4(0, 0, 1, 1));
	}

	//vec3 P = vec3((r32)PlayerP.x, (r32)PlayerP.y, 0.0f);
	//vec4 Color = vec4(0.0f, 0.9f, 0.0f, 1.0f);
	//PushSprite(&TilesVertices, P, vec2i(1, 1), Color, vec4(0, 0, 1, 1));	

	//vec3 TextP = vec3(P.x * GameState.MetersToPixels + 0.4f * GameState.MetersToPixels,
	//	P.y * GameState.MetersToPixels + GameState.MetersToPixels,
	//	0.0f);
	//PushTextSprite(&DebugTextVertices, DebugFont, TextP, vec4i(1, 1, 1, 1), "@");
	//TextP.x -= 0.2f * GameState.MetersToPixels;
	//TextP.y -= 0.5f * GameState.MetersToPixels;
	//PushTextSprite(&DebugTextVertices, DebugFont, TextP, vec4i(1, 1, 1, 1), "HP: 3");

	rndr::BufferData(GameState.TilesVertexBuffer, 0, (u32)(sizeof(vert_P1C1UV1) * TilesVertices.size()), &TilesVertices.front());

	cmd::draw *TilesDrawCmd = GameState.TileRenderList->AddCommand<cmd::draw>(0);
	TilesDrawCmd->VertexBuffer = GameState.TilesVertexBuffer;
	TilesDrawCmd->VertexFormat = vert_format::P1C1UV1;
	TilesDrawCmd->StartVertex = 0;
	TilesDrawCmd->VertexCount = (u32)TilesVertices.size();
	TilesDrawCmd->Textures[0] = GameState.WhiteTexture;

	GameState.TileRenderList->Sort();
	GameState.TileRenderList->Submit();
	GameState.TileRenderList->Flush();
	
	PushTextSprite(&DebugTextVertices, &GameState.DebugFont, vec3i(0, 720, 1), vec4i(1, 1, 0, 1), "%0.2f ms/frame", 1000.0f * Input->FrameTime);
	rndr::BufferData(GameState.DebugTextVertexBuffer, 0, (u32)sizeof(vert_P1C1UV1) * (u32)DebugTextVertices.size(), &DebugTextVertices.front());

	cmd::draw *DebugTextCmd = GameState.DebugTextCmdList->AddCommand<cmd::draw>(0);
	DebugTextCmd->VertexBuffer = GameState.DebugTextVertexBuffer;
	DebugTextCmd->VertexFormat = vert_format::P1C1UV1;
	DebugTextCmd->StartVertex = 0;
	DebugTextCmd->VertexCount = (u32)DebugTextVertices.size();
	DebugTextCmd->Textures[0] = GameState.DebugFont.FontTexture;
	

	GameState.DebugTextCmdList->Sort();
	GameState.DebugTextCmdList->Submit();
	GameState.DebugTextCmdList->Flush();
}