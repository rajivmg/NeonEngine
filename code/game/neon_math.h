#ifndef NEON_MATH_H
#define NEON_MATH_H

#define M_PI        3.14159265358979323846f
#define DEG2RAD(_a) ((_a)*M_PI/180.0f)
#define RAD2DEG(_a) ((_a)*180.0f/M_PI)
#define INT_MIN     (-2147483647 - 1)
#define INT_MAX     2147483647
#define FLT_MAX     3.402823466e+38F

#include <math.h>

#include "../platform/neon_platform.h"

//-----------------------------------------------------------------------------
// Vector
//-----------------------------------------------------------------------------

//union vec2
//{
//	r32 Elements[2];
//	struct
//	{
//		r32 x, y;
//	};
//	struct
//	{
//		r32 s, t;
//	};
//	struct
//	{
//		r32 u, v;
//	};
//	
//	vec2();	
//	vec2(r32 X, r32 Y);
//	vec2(s32 X, s32 Y);
//	vec2 operator+(vec2 const & V2);
//	vec2 operator-(vec2 const & V2);
//	vec2 operator-();
//	vec2 operator*(r32 const & C);
//	
//	r32  Dot(vec2 const & V2);
//	r32  Cross(vec2 const & V2);
//	r32	 Length();
//
//	static vec2 Normalize(vec2& V2);
//	~vec2();
//};

struct vec2
{
	union
	{
		struct
		{
			r32 x, y;
		};

		r32 Elements[2];
	};

	FORCE_INLINE vec2() { }
	FORCE_INLINE explicit vec2(r32 const *e) { x = e[0]; y = e[1]; }
	FORCE_INLINE explicit vec2(r32 _x, r32 _y) { x = _x; y = _y; }
	FORCE_INLINE explicit vec2(r32 xy) { x = xy; y = xy; }
};

FORCE_INLINE vec2 vec2i(s32 _x, s32 _y) { return vec2((r32)_x, (r32)_y); }
FORCE_INLINE vec2 operator+(vec2 const &A, r32 Scalar) { return vec2(A.x + Scalar, A.y + Scalar); }
FORCE_INLINE vec2 operator-(vec2 const &A, r32 Scalar) { return vec2(A.x - Scalar, A.y - Scalar); }
FORCE_INLINE vec2 operator*(vec2 const &A, r32 Scalar) { return vec2(A.x * Scalar, A.y * Scalar); }
FORCE_INLINE vec2 operator/(vec2 const &A, r32 Scalar) { return vec2(A.x / Scalar, A.y / Scalar); }

FORCE_INLINE vec2 operator+(r32 Scalar, vec2 const &A) { return vec2(Scalar + A.x, Scalar + A.y); }
FORCE_INLINE vec2 operator-(r32 Scalar, vec2 const &A) { return vec2(Scalar - A.x, Scalar - A.y); }
FORCE_INLINE vec2 operator*(r32 Scalar, vec2 const &A) { return vec2(Scalar * A.x, Scalar * A.y); }
FORCE_INLINE vec2 operator/(r32 Scalar, vec2 const &A) { return vec2(Scalar / A.x, Scalar / A.y); }

FORCE_INLINE vec2 operator+(vec2 const &A, vec2 const &B) { return vec2(A.x + B.x, A.y + B.y); }
FORCE_INLINE vec2 operator-(vec2 const &A, vec2 const &B) { return vec2(A.x - B.x, A.y - B.y); }
FORCE_INLINE vec2 operator*(vec2 const &A, vec2 const &B) { return vec2(A.x * B.x, A.y * B.y); }
FORCE_INLINE vec2 operator/(vec2 const &A, vec2 const &B) { return vec2(A.x / B.x, A.y / B.y); }

FORCE_INLINE vec2& operator+=(vec2 &A, vec2 const &B) { A = A + B; return A; }
FORCE_INLINE vec2& operator-=(vec2 &A, vec2 const &B) { A = A - B; return A; }
FORCE_INLINE vec2& operator*=(vec2 &A, vec2 const &B) { A = A * B; return A; }
FORCE_INLINE vec2& operator/=(vec2 &A, vec2 const &B) { A = A / B; return A; }
FORCE_INLINE vec2& operator+=(vec2 &A, r32 Scalar) { A = A + Scalar; return A; }
FORCE_INLINE vec2& operator-=(vec2 &A, r32 Scalar) { A = A - Scalar; return A; }
FORCE_INLINE vec2& operator*=(vec2 &A, r32 Scalar) { A = A * Scalar; return A; }
FORCE_INLINE vec2& operator/=(vec2 &A, r32 Scalar) { A = A / Scalar; return A; }

FORCE_INLINE vec2 operator-(vec2 const &A) { return 0.0f - A; }

//union vec3
//{
//	r32 Elements[3];
//	struct
//	{
//		r32 x, y, z;
//	};
//	struct
//	{
//		r32 r, g, b;
//	};
//
//	vec3();
//	vec3(r32 X, r32 Y, r32 Z);
//	vec3(s32 X, s32 Y, s32 Z);
//	vec3(vec2 const & V2, r32 Z);
//	vec3 operator+(vec3 const & V3);
//	vec3 operator-(vec3 const & V3);
//	vec3 operator-();
//	vec3 operator*(r32 const & C);
//	
//	r32  Dot(vec3 const & V3);
//	vec3 Cross(vec3 const & V3);
//	r32	 Length();
//
//	static vec3 Normalize(vec3& V3);
//	~vec3();
//};

struct vec3
{
	union
	{
		struct
		{
			r32 x, y, z;
		};

		r32 Elements[3];
	};

	FORCE_INLINE vec3() { }
	FORCE_INLINE explicit vec3(r32 const *e) { x = e[0]; y = e[1]; z = e[2]; }
	FORCE_INLINE explicit vec3(r32 _x, r32 _y, r32 _z) { x = _x; y = _y; z = _z; }
	FORCE_INLINE explicit vec3(r32 xyz) { x = xyz; y = xyz; z = xyz; }
};

FORCE_INLINE vec3 vec3i(s32 _x, s32 _y, s32 _z) { return vec3((r32)_x, (r32)_y, (r32)_z); }
FORCE_INLINE vec3 operator+(vec3 const &A, r32 Scalar) { return vec3(A.x + Scalar, A.y + Scalar, A.z + Scalar); }
FORCE_INLINE vec3 operator-(vec3 const &A, r32 Scalar) { return vec3(A.x - Scalar, A.y - Scalar, A.z - Scalar); }
FORCE_INLINE vec3 operator*(vec3 const &A, r32 Scalar) { return vec3(A.x * Scalar, A.y * Scalar, A.z * Scalar); }
FORCE_INLINE vec3 operator/(vec3 const &A, r32 Scalar) { return vec3(A.x / Scalar, A.y / Scalar, A.z / Scalar); }

FORCE_INLINE vec3 operator+(r32 Scalar, vec3 const &A) { return vec3(Scalar + A.x, Scalar + A.y, Scalar + A.z); }
FORCE_INLINE vec3 operator-(r32 Scalar, vec3 const &A) { return vec3(Scalar - A.x, Scalar - A.y, Scalar - A.z); }
FORCE_INLINE vec3 operator*(r32 Scalar, vec3 const &A) { return vec3(Scalar * A.x, Scalar * A.y, Scalar * A.z); }
FORCE_INLINE vec3 operator/(r32 Scalar, vec3 const &A) { return vec3(Scalar / A.x, Scalar / A.y, Scalar / A.z); }

FORCE_INLINE vec3 operator+(vec3 const &A, vec3 const &B) { return vec3(A.x + B.x, A.y + B.y, A.z + B.z); }
FORCE_INLINE vec3 operator-(vec3 const &A, vec3 const &B) { return vec3(A.x - B.x, A.y - B.y, A.z - B.z); }
FORCE_INLINE vec3 operator*(vec3 const &A, vec3 const &B) { return vec3(A.x * B.x, A.y * B.y, A.z * B.z); }
FORCE_INLINE vec3 operator/(vec3 const &A, vec3 const &B) { return vec3(A.x / B.x, A.y / B.y, A.z / B.z); }

FORCE_INLINE vec3& operator+=(vec3 &A, vec3 const &B) { A = A + B; return A; }
FORCE_INLINE vec3& operator-=(vec3 &A, vec3 const &B) { A = A - B; return A; }
FORCE_INLINE vec3& operator*=(vec3 &A, vec3 const &B) { A = A * B; return A; }
FORCE_INLINE vec3& operator/=(vec3 &A, vec3 const &B) { A = A / B; return A; }
FORCE_INLINE vec3& operator+=(vec3 &A, r32 Scalar) { A = A + Scalar; return A; }
FORCE_INLINE vec3& operator-=(vec3 &A, r32 Scalar) { A = A - Scalar; return A; }
FORCE_INLINE vec3& operator*=(vec3 &A, r32 Scalar) { A = A * Scalar; return A; }
FORCE_INLINE vec3& operator/=(vec3 &A, r32 Scalar) { A = A / Scalar; return A; }

FORCE_INLINE vec3 operator==(vec3 const &A, vec3 const &B) { return vec3(A.x == B.x, A.y == B.y, A.z == B.z); }

FORCE_INLINE vec3 operator-(vec3 const &A) { return 0.0f - A; }

FORCE_INLINE vec3 Cross(vec3 const &A, vec3 const &B) { return vec3(A.y * B.z - A.z * B.y, A.z * B.x - A.x * B.z, A.x * B.y - A.y * B.x); }
FORCE_INLINE r32 Sum(vec3 const &A) { return A.x + A.y + A.z; }
FORCE_INLINE r32 Dot(vec3 const &A, vec3 const &B) { return Sum(A * B); }
FORCE_INLINE r32 Length(vec3 const &A) { return sqrtf(Dot(A, A)); }
FORCE_INLINE r32 LengthSq(vec3 const &A) { return Dot(A, A); }
FORCE_INLINE vec3 Normalize(vec3 const &A) { return A * (1.0f / Length(A)); }
FORCE_INLINE vec3 Lerp(vec3 const &A, vec3 const &B, r32 T) { return A + (B - A) * T; }

struct vec4
{
	union
	{
		struct
		{
			r32 x, y, z, w;
		};

		r32 Elements[4];
	};

	FORCE_INLINE vec4() { }
	FORCE_INLINE explicit vec4(r32 const *e) { x = e[0]; y = e[1]; z = e[2]; w = e[3]; }
	FORCE_INLINE explicit vec4(r32 _x, r32 _y, r32 _z, r32 _w) { x = _x; y = _y; z = _z; w = _w; }
	FORCE_INLINE explicit vec4(r32 _xyzw) { x = _xyzw; y = _xyzw; z = _xyzw; w = _xyzw; }
};

FORCE_INLINE vec4 vec4i(s32 _x, s32 _y, s32 _z, s32 _w) { return vec4((r32)_x, (r32)_y, (r32)_z, (r32)_w); }
FORCE_INLINE vec4 operator+(vec4 const &A, r32 Scalar) { return vec4(A.x + Scalar, A.y + Scalar, A.z + Scalar, A.w + Scalar); }
FORCE_INLINE vec4 operator-(vec4 const &A, r32 Scalar) { return vec4(A.x - Scalar, A.y - Scalar, A.z - Scalar, A.w - Scalar); }
FORCE_INLINE vec4 operator*(vec4 const &A, r32 Scalar) { return vec4(A.x * Scalar, A.y * Scalar, A.z * Scalar, A.w * Scalar); }
FORCE_INLINE vec4 operator/(vec4 const &A, r32 Scalar) { return vec4(A.x / Scalar, A.y / Scalar, A.z / Scalar, A.w / Scalar); }

FORCE_INLINE vec4 operator+(r32 Scalar, vec4 const &A) { return vec4(Scalar + A.x, Scalar + A.y, Scalar + A.z, Scalar + A.w); }
FORCE_INLINE vec4 operator-(r32 Scalar, vec4 const &A) { return vec4(Scalar - A.x, Scalar - A.y, Scalar - A.z, Scalar - A.w); }
FORCE_INLINE vec4 operator*(r32 Scalar, vec4 const &A) { return vec4(Scalar * A.x, Scalar * A.y, Scalar * A.z, Scalar * A.w); }
FORCE_INLINE vec4 operator/(r32 Scalar, vec4 const &A) { return vec4(Scalar / A.x, Scalar / A.y, Scalar / A.z, Scalar / A.w); }

FORCE_INLINE vec4 operator+(vec4 const &A, vec4 const &B) { return vec4(A.x + B.x, A.y + B.y, A.z + B.z, A.w + B.w); }
FORCE_INLINE vec4 operator-(vec4 const &A, vec4 const &B) { return vec4(A.x - B.x, A.y - B.y, A.z - B.z, A.w - B.w); }
FORCE_INLINE vec4 operator*(vec4 const &A, vec4 const &B) { return vec4(A.x * B.x, A.y * B.y, A.z * B.z, A.w * B.w); }
FORCE_INLINE vec4 operator/(vec4 const &A, vec4 const &B) { return vec4(A.x / B.x, A.y / B.y, A.z / B.z, A.w / B.w); }

FORCE_INLINE vec4& operator+=(vec4 &A, vec4 const &B) { A = A + B; return A; }
FORCE_INLINE vec4& operator-=(vec4 &A, vec4 const &B) { A = A - B; return A; }
FORCE_INLINE vec4& operator*=(vec4 &A, vec4 const &B) { A = A * B; return A; }
FORCE_INLINE vec4& operator/=(vec4 &A, vec4 const &B) { A = A / B; return A; }
FORCE_INLINE vec4& operator+=(vec4 &A, r32 Scalar) { A = A + Scalar; return A; }
FORCE_INLINE vec4& operator-=(vec4 &A, r32 Scalar) { A = A - Scalar; return A; }
FORCE_INLINE vec4& operator*=(vec4 &A, r32 Scalar) { A = A * Scalar; return A; }
FORCE_INLINE vec4& operator/=(vec4 &A, r32 Scalar) { A = A / Scalar; return A; }

FORCE_INLINE vec4 operator-(vec4 const &A) { return 0.0f - A; }

//-----------------------------------------------------------------------------
// Matrix
//-----------------------------------------------------------------------------

//union mat4
//{
//	r32 Elements[16];
//	struct
//	{
//		r32 m00, m01, m02, m03;
//		r32 m10, m11, m12, m13;
//		r32 m20, m21, m22, m23;
//		r32 m30, m31, m32, m33;
//	};
//
//	mat4();
//	mat4 operator+(mat4 const & M);
//	mat4 operator-(mat4 const & M);
//	mat4 operator*(mat4 const & M);
//	mat4 operator/(mat4 const & M);
//
//	static mat4 Identity();
//	static mat4 Transpose(mat4 const & M);
//	static mat4 Translate(r32 X, r32 Y, r32 Z);
//	static mat4 Scale(r32 Sx, r32 Sy, r32 Sz);
//	
//	//static mat4 LookAt(vec3 Eye, vec3 Direction, vec3 Up);
//	static mat4 Orthographic(r32 L, r32 R, r32 T, r32 B, r32 N, r32 F);
//	static mat4 Perspective(r32 Fov, r32 Aspect, r32 Near, r32 Far);
//	~mat4();
//};

struct mat4
{
	union
	{
		struct
		{
			r32 m00, m10, m20, m30;
			r32 m01, m11, m21, m31;
			r32 m02, m12, m22, m32;
			r32 m03, m13, m23, m33;
		};

		r32 Elements[16];
	};

	FORCE_INLINE mat4() { }
};

FORCE_INLINE mat4 operator*(mat4 const &A, mat4 const &B)
{
	mat4 Result;

	Result.m00 = A.m00*B.m00 + A.m01*B.m10 + A.m02*B.m20 + A.m03*B.m30;
	Result.m01 = A.m00*B.m01 + A.m01*B.m11 + A.m02*B.m21 + A.m03*B.m31;
	Result.m02 = A.m00*B.m02 + A.m01*B.m12 + A.m02*B.m22 + A.m03*B.m32;
	Result.m03 = A.m00*B.m03 + A.m01*B.m13 + A.m02*B.m23 + A.m03*B.m33;

	Result.m10 = A.m10*B.m00 + A.m11*B.m10 + A.m12*B.m20 + A.m13*B.m30;
	Result.m11 = A.m10*B.m01 + A.m11*B.m11 + A.m12*B.m21 + A.m13*B.m31;
	Result.m12 = A.m10*B.m02 + A.m11*B.m12 + A.m12*B.m22 + A.m13*B.m32;
	Result.m13 = A.m10*B.m03 + A.m11*B.m13 + A.m12*B.m23 + A.m13*B.m33;

	Result.m20 = A.m20*B.m00 + A.m21*B.m10 + A.m22*B.m20 + A.m23*B.m30;
	Result.m21 = A.m20*B.m01 + A.m21*B.m11 + A.m22*B.m21 + A.m23*B.m31;
	Result.m22 = A.m20*B.m02 + A.m21*B.m12 + A.m22*B.m22 + A.m23*B.m32;
	Result.m23 = A.m20*B.m03 + A.m21*B.m13 + A.m22*B.m23 + A.m23*B.m33;

	Result.m20 = A.m30*B.m00 + A.m31*B.m10 + A.m32*B.m20 + A.m33*B.m30;
	Result.m31 = A.m30*B.m01 + A.m31*B.m11 + A.m32*B.m21 + A.m33*B.m31;
	Result.m32 = A.m30*B.m02 + A.m31*B.m12 + A.m32*B.m22 + A.m33*B.m32;
	Result.m33 = A.m30*B.m03 + A.m31*B.m13 + A.m32*B.m23 + A.m33*B.m33;

	return Result;
}

FORCE_INLINE mat4& operator*=(mat4 &A, mat4 const &B)
{
	A = A * B; return A;
}

FORCE_INLINE mat4 Identity()
{
	mat4 A;
	A.m00 = 1.0f; A.m01 = 0.0f; A.m02 = 0.0f; A.m03 = 0.0f;
	A.m10 = 0.0f; A.m11 = 1.0f; A.m12 = 0.0f; A.m13 = 0.0f;
	A.m20 = 0.0f; A.m21 = 0.0f; A.m22 = 1.0f; A.m23 = 0.0f;
	A.m30 = 0.0f; A.m31 = 0.0f; A.m32 = 0.0f; A.m33 = 1.0f;

	return A;
}

FORCE_INLINE mat4 Translate(r32 _x, r32 _y, r32 _z)
{
	mat4 A = Identity();
	A.m03 = _x;
	A.m13 = _y;
	A.m23 = _z;
	return A;
}

FORCE_INLINE mat4 Scale(r32 _x, r32 _y, r32 _z)
{
	mat4 A = Identity();
	A.m00 = _x;
	A.m11 = _y;
	A.m22 = _z;
	return A;
}

FORCE_INLINE mat4 Orthographic(r32 L, r32 R, r32 T, r32 B, r32 N, r32 F)
{
	mat4 Matrix;

	Matrix.m00 = 2/(R-L); 	Matrix.m01 = 0; 		Matrix.m02 = 0; 		Matrix.m03 = -(R+L)/(R-L);
	Matrix.m10 = 0; 		Matrix.m11 = 2/(T-B); 	Matrix.m12 = 0; 		Matrix.m13 = -(T+B)/(T-B);
	Matrix.m20 = 0; 		Matrix.m21 = 0; 		Matrix.m22 = -2/(F-N); 	Matrix.m23 = -(F+N)/(F-N);
	Matrix.m30 = 0; 		Matrix.m31 = 0; 		Matrix.m32 = 0; 		Matrix.m33 = 1;

	return Matrix;
}

#endif