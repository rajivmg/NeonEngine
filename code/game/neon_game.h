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

struct tilemap
{
	char	Filename[32];
	vec2	MapSize;
	texture Tileset;
	u32		TileSize;
};

struct cg_common
{
	r32 Time;
};

struct game_state
{
	render_resource WhiteTexture;
	render_resource WaterDisplacementTexture;
	render_resource WaterShader;
	render_resource WaterVertexBuffer;
	render_cmd_list *Water;

	render_resource TextShader;
	render_resource DebugTextVertexBuffer;
	render_cmd_list *DebugTextCmdList;

	render_resource CommonConstBuffer;

	r32 MetersToPixels;
	r32 PixelsToMeters;
};

#endif