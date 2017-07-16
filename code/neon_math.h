#ifndef NEON_MATH_H
#define NEON_MATH_H

#include <math.h>

#include "neon_platform.h"
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////
//// NOTE:
////	1. Matrix is row ordered.
////
////

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
	vec2(r32 x, r32 y);
	vec2(s32 x, s32 y);
	vec2 operator+(vec2 const & V2);
	vec2 operator-(vec2 const & V2);
	vec2 operator-();
	r32  Dot(vec2 const & V2);
	r32  Cross(vec2 const & V2);
	r32	Length();
	~vec2();
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
	vec3(r32 x, r32 y, r32 z);
	vec3(s32 x, s32 y, s32 z);
	vec3(vec2 const & V2, r32 z);
	vec3 operator+(vec3 const & V3);
	vec3 operator-(vec3 const & V3);
	vec3 operator-();
	r32  Dot(vec3 const & V3);
	vec3 Cross(vec3 const & V3);
	r32	Length();
	~vec3();
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
	vec4(r32 x, r32 y, r32 z, r32 w);
	vec4(s32 x, s32 y, s32 z, s32 w);
	vec4(vec2 const & V21, vec2 const & V22);
	vec4(vec3 const & V3, r32 w);
	vec4 operator+(vec4 const & V4);
	vec4 operator-(vec4 const & V4);
	vec4 operator-();
	r32  Dot(vec4 const & V4);
	vec4 Cross(vec4 const & V4);
	r32	 Length();
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
	~mat4();
};

mat4 OrthoMat4(r32 L, r32 R, r32 T, r32 B, r32 N, r32 F);

#endif