#ifndef NEON_GAME_H
#define NEON_GAME_H

#include "../platform/neon_platform.h"

#include "neon_renderer.h"
#include "neon_math.h"
#include "neon_texture.h"

platform_t Platform;

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

struct floor_tile
{
	vec3 P;
};

enum class entity_type
{
	Entity_None,
	Entity_Block,
	Entity_Enemy1,
	Entity_Portal,
	Entity_Player
};

struct entity
{
	entity_type Type;

	vec3 P;			// x, y, _z_ coordinates on screen
	
	u32 X;			// Tile's X coordinate
	u32 Y;			// Tile's Y coordinate

	bool IsMoving;	// True only if in moving state
	bool Collide;	// 
	
	u32 HP;
};

struct room
{
	u32 SizeX;
	u32 SizeY;

	u32	EntityCount;
	entity Entities[256];
	
	entity *Player; // Pointer to player entity. Only _ONE_ per room.

	u32 FloorTilesCount; // RoomX * RoomY
	floor_tile *FloorTiles;

	//bool *CollisionMap;
	u32 *SlotMap;
};

struct game_state
{
	room Room;

	render_resource WhiteTexture;

	render_resource SpriteShader;
	render_resource TilesVertexBuffer;
	render_cmd_list *TileRenderList;

	font *DebugFont;

	render_resource TextShader;
	render_resource DebugTextVertexBuffer;
	render_cmd_list *DebugTextCmdList;

	r32 MetersToPixels;
	r32 PixelsToMeters;

	vec3 RoomCenterOffset;
};

#endif