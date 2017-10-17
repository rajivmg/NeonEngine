#ifndef NEON_MATH_H
#define NEON_MATH_H

#define _USE_MATH_DEFINES
#include <math.h>

#include "../platform/neon_platform.h"

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////
//// NOTE:
////	1. Matrix is row-major in memory.
////	2. Translation matrix assume vector to be column vector.
////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

union ivec2
{
	s32 Elements[2];
	struct
	{
		s32 x, y;
	};

	ivec2();
	ivec2(s32 X, s32 Y);
	ivec2(r32 X, r32 Y);
	ivec2 operator+(ivec2 const & V2);
	ivec2 operator-(ivec2 const & V2);
	ivec2 operator-();
	ivec2 operator*(s32 const & C);
	~ivec2();
};

union vec2
{
	r32 Elements[2];
	struct
	{
		r32 x, y;
	};
	struct
	{
		r32 s, t;
	};
	struct
	{
		r32 u, v;
	};
	
	vec2();	
	vec2(r32 X, r32 Y);
	vec2(s32 X, s32 Y);
	vec2 operator+(vec2 const & V2);
	vec2 operator-(vec2 const & V2);
	vec2 operator-();
	vec2 operator*(r32 const & C);
	r32  Dot(vec2 const & V2);
	r32  Cross(vec2 const & V2);
	r32	 Length();

	static vec2 Normalize(vec2& V2);
	~vec2();
};

union ivec3
{
	s32 Elements[3];
	struct
	{
		s32 x, y, z;
	};
	struct
	{
		s32 r, g, b;
	};

	ivec3();
	ivec3(s32 X, s32 Y, s32 Z);
	ivec3(ivec2 const & V2, s32 Z);
	ivec3 operator+(ivec3 const & V3);
	ivec3 operator-(ivec3 const & V3);
	ivec3 operator-();
	ivec3 operator*(s32 const & C);
	~ivec3();
};

union vec3
{
	r32 Elements[3];
	struct
	{
		r32 x, y, z;
	};
	struct
	{
		r32 r, g, b;
	};

	vec3();
	vec3(r32 X, r32 Y, r32 Z);
	vec3(s32 X, s32 Y, s32 Z);
	vec3(vec2 const & V2, r32 Z);
	vec3 operator+(vec3 const & V3);
	vec3 operator-(vec3 const & V3);
	vec3 operator-();
	vec3 operator*(r32 const & C);
	r32  Dot(vec3 const & V3);
	vec3 Cross(vec3 const & V3);
	r32	 Length();

	static vec3 Normalize(vec3& V3);
	~vec3();
};

union ivec4
{
	s32 Elements[4];
	struct
	{
		s32 x, y, z, w;
	};
	struct
	{
		s32 r, g, b, a;
	};

	ivec4();
	ivec4(s32 X, s32 Y, s32 Z, s32 W);
	ivec4(ivec2 const & V21, ivec2 const & V22);
	ivec4(ivec3 const & V3, s32 W);
	ivec4 operator+(ivec4 const & V4);
	ivec4 operator-(ivec4 const & V4);
	ivec4 operator-();
	ivec4 operator*(s32 const & C);
	s32	 Length();
	~ivec4();
};

union vec4
{
	r32 Elements[4];
	struct
	{
		r32 x, y, z, w;
	};
	struct
	{
		r32 r, g, b, a;
	};

	vec4();
	vec4(r32 X, r32 Y, r32 Z, r32 W);
	vec4(s32 X, s32 Y, s32 Z, s32 W);
	vec4(vec2 const & V21, vec2 const & V22);
	vec4(vec3 const & V3, r32 W);
	vec4 operator+(vec4 const & V4);
	vec4 operator-(vec4 const & V4);
	vec4 operator-();
	vec4 operator*(r32 const & C);
	vec3 Normalize();
	r32  Dot(vec4 const & V4);
	vec4 Cross(vec4 const & V4);
	r32	 Length();

	static vec4 Normalize(vec4& V4);
	~vec4();
};

union mat2
{
	r32 Elements[4];
	struct
	{
		r32 m00, m01;
		r32 m10, m11;
	};
	
	mat2();
	~mat2();
};

union mat3
{
	r32 Elements[9];
	struct
	{
		r32 m00, m01, m02;
	 	r32 m10, m11, m12;
	 	r32 m20, m21, m22;
	};
	
	mat3();
	~mat3();
};

union mat4
{
	r32 Elements[16];
	struct
	{
		r32 m00, m01, m02, m03;
		r32 m10, m11, m12, m13;
		r32 m20, m21, m22, m23;
		r32 m30, m31, m32, m33;
	};

	mat4();
	mat4 operator+(mat4 const & M);
	mat4 operator-(mat4 const & M);
	mat4 operator*(mat4 const & M);
	mat4 operator/(mat4 const & M);

	static mat4 Identity();
	static mat4 Transpose(mat4 const & M);
	static mat4 Translate(r32 X, r32 Y, r32 Z);
	static mat4 Scale(r32 Sx, r32 Sy, r32 Sz);
	
	static mat4 LookAt(vec3 Eye, vec3 Direction, vec3 Up);
	static mat4 Orthographic(r32 L, r32 R, r32 T, r32 B, r32 N, r32 F);
	static mat4 Perspective(r32 Fov, r32 Aspect, r32 Near, r32 Far);
	~mat4();
};

#endif