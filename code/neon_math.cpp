#include "neon_math.h"

/////////////////////////////////////////////////////////////////////////////////
//
//	ivec2
//
/////////////////////////////////////////////////////////////////////////////////
ivec2::ivec2()
{
	Elements[0] = 0;
	Elements[1] = 0;
}

ivec2::ivec2(s32 X, s32 Y)
{
	x = X;
	y = Y;	
}

ivec2::ivec2(r32 X, r32 Y)
{
	x = (s32)X;
	y = (s32)Y;
}

ivec2 ivec2::operator+(ivec2 const & V2)
{
	return ivec2(x + V2.x, y + V2.y);
}

ivec2 ivec2::operator-(ivec2 const & V2)
{
	return ivec2(x - V2.x, y - V2.y);
}

ivec2 ivec2::operator-()
{
	return ivec2(-x, -y);
}

ivec2 ivec2::operator*(s32 const & C)
{
	return ivec2(x * C, y * C);
}

ivec2::~ivec2()
{
}

/////////////////////////////////////////////////////////////////////////////////
//
//	vec2
//
/////////////////////////////////////////////////////////////////////////////////
vec2::vec2()
{
	Elements[0] = 0;
	Elements[1] = 0;
}

vec2::vec2(r32 X, r32 Y)
{
	Elements[0] = X;
	Elements[1] = Y;
}

vec2::vec2(s32 X, s32 Y)
{
	Elements[0] = (r32)X;
	Elements[1] = (r32)Y;
}

vec2 vec2::operator+(vec2 const & V2)
{
	return vec2(x + V2.x, y + V2.y);
}

vec2 vec2::operator-(vec2 const & V2)
{
	return vec2(x - V2.x, y - V2.y);
}

vec2 vec2::operator-()
{
	return vec2(-x, -y);
}

vec2 vec2::operator*(r32 const & C)
{
	return vec2(x * C, y * C);
}

r32 vec2::Dot(vec2 const & V2)
{
	return (r32)((x * V2.x) + (y * V2.y));
}

r32 vec2::Cross(vec2 const & V2)
{
	////////A B   A B
	////r = x y - y x
	return x * V2.y - y * V2.x;
}

r32 vec2::Length()
{
	return (r32)sqrt((x*x) + (y*y));
}

vec2 vec2::Normalize(vec2& V2)
{
	r32 Length = V2.Length();
	if(Length <= 0)
	{
		Length = 1;
	}
	return vec2(V2.x/Length, V2.y/Length);
}

vec2::~vec2()
{
}

/////////////////////////////////////////////////////////////////////////////////
//
//	ivec3
//
/////////////////////////////////////////////////////////////////////////////////
ivec3::ivec3()
{
	Elements[0] = 0;
	Elements[1] = 0;
	Elements[2] = 0;
}

ivec3::ivec3(s32 X, s32 Y, s32 Z)
{
	Elements[0] = X;
	Elements[1] = Y;
	Elements[2] = Z;
}

ivec3::ivec3(ivec2 const & V2, s32 Z)
{
	Elements[0] = V2.x;
	Elements[1] = V2.y;
	Elements[2] = Z;
}

ivec3 ivec3::operator+(ivec3 const & V3)
{
	return ivec3(x + V3.x, y + V3.y, z + V3.z);
}

ivec3 ivec3::operator-(ivec3 const & V3)
{
	return ivec3(x - V3.x, y - V3.y, z - V3.z);
}

ivec3 ivec3::operator-()
{
	return ivec3(-x, -y, -z);
}

ivec3 ivec3::operator*(s32 const & C)
{
	return ivec3(x * C, y * C, z * C);
}

ivec3::~ivec3()
{
}

/////////////////////////////////////////////////////////////////////////////////
//
//	vec3
//
/////////////////////////////////////////////////////////////////////////////////
vec3::vec3()
{
	Elements[0] = 0;
	Elements[1] = 0;
	Elements[2] = 0;
}

vec3::vec3(r32 X, r32 Y, r32 Z)
{
	Elements[0] = X;
	Elements[1] = Y;
	Elements[2] = Z;
}

vec3::vec3(s32 X, s32 Y, s32 Z)
{
	Elements[0] = (r32)X;
	Elements[1] = (r32)Y;
	Elements[2] = (r32)Z;
}

vec3::vec3(vec2 const & V2, r32 Z)
{
	Elements[0] = V2.x;
	Elements[1] = V2.y;
	Elements[2] = Z;
}

vec3 vec3::operator+(vec3 const & V3)
{
	return vec3(x + V3.x, y + V3.y, z + V3.z);
}

vec3 vec3::operator-(vec3 const & V3)
{
	return vec3(x - V3.x, y - V3.y, z - V3.z);
}

vec3 vec3::operator-()
{
	return vec3(-x, -y, -z);
}

vec3 vec3::operator*(r32 const & C)
{
	return vec3(x * C, y * C, z * C);
}

r32 vec3::Dot(vec3 const & V3)
{
	return (r32)((x * V3.x) + (y * V3.y) + (z * V3.z));
}

vec3 vec3::Cross(vec3 const & V3)
{
	// Cross = A B - A B
	////// x = y z - z y
	////// y = z x - x z
	////// z = x y - y x
	return vec3(y * V3.z - z * V3.y, z * V3.x - x * V3.z, x * V3.y - y * V3.x);
}

r32 vec3::Length()
{
	return (r32)sqrt((x*x) + (y*y) + (z*z));
}

vec3 vec3::Normalize(vec3& V3)
{
	r32 Length = V3.Length();
	if(Length <= 0)
	{
		Length = 1;
	}
	return vec3(V3.x/Length, V3.y/Length, V3.z/Length);
}

vec3::~vec3()
{
}

/////////////////////////////////////////////////////////////////////////////////
//
//	ivec4
//
/////////////////////////////////////////////////////////////////////////////////
ivec4::ivec4()
{
	Elements[0] = 0;
	Elements[1] = 0;
	Elements[2] = 0;
	Elements[3] = 0;
}

ivec4::ivec4(s32 X, s32 Y, s32 Z, s32 W)
{
	Elements[0] = X;
	Elements[1] = Y;
	Elements[2] = Z;
	Elements[3] = W;
}

ivec4::ivec4(ivec3 const & V3, s32 W)
{
	Elements[0] = V3.x;
	Elements[1] = V3.y;
	Elements[2] = V3.z;
	Elements[3] = W;
}

ivec4::ivec4(ivec2 const & V21, ivec2 const & V22)
{
	Elements[0] = V21.x;
	Elements[1] = V21.y;
	Elements[2] = V22.x;
	Elements[3] = V22.y;
}

ivec4 ivec4::operator+(ivec4 const & V4)
{
	return ivec4(x + V4.x, y + V4.y, z + V4.z, w + V4.w);
}

ivec4 ivec4::operator-(ivec4 const & V4)
{
	return ivec4(x - V4.x, y - V4.y, z - V4.z, w - V4.w);
}

ivec4 ivec4::operator-()
{
	return ivec4(-x, -y, -z, -w);
}

ivec4 ivec4::operator*(s32 const & C)
{
	return ivec4(x * C, y * C, z * C, w *C);
}

ivec4::~ivec4()
{
}

/////////////////////////////////////////////////////////////////////////////////
//
//	vec4
//
/////////////////////////////////////////////////////////////////////////////////
vec4::vec4()
{
	Elements[0] = 0;
	Elements[1] = 0;
	Elements[2] = 0;
	Elements[3] = 0;
}

vec4::vec4(r32 X, r32 Y, r32 Z, r32 W)
{
	Elements[0] = X;
	Elements[1] = Y;
	Elements[2] = Z;
	Elements[3] = W;
}

vec4::vec4(s32 X, s32 Y, s32 Z, s32 W)
{
	Elements[0] = (r32)X;
	Elements[1] = (r32)Y;
	Elements[2] = (r32)Z;
	Elements[3] = (r32)W;
}

vec4::vec4(vec3 const & V3, r32 W)
{
	Elements[0] = V3.x;
	Elements[1] = V3.y;
	Elements[2] = V3.z;
	Elements[3] = W;
}

vec4::vec4(vec2 const & V21, vec2 const & V22)
{
	Elements[0] = V21.x;
	Elements[1] = V21.y;
	Elements[2] = V22.x;
	Elements[3] = V22.y;
}

vec4 vec4::operator+(vec4 const & V4)
{
	return vec4(x + V4.x, y + V4.y, z + V4.z, w + V4.w);
}

vec4 vec4::operator-(vec4 const & V4)
{
	return vec4(x - V4.x, y - V4.y, z - V4.z, w - V4.w);
}

vec4 vec4::operator-()
{
	return vec4(-x, -y, -z, -w);
}

vec4 vec4::operator*(r32 const & C)
{
	return vec4(x * C, y * C, z * C, w *C);
}

r32 vec4::Dot(vec4 const & V4)
{
	return (r32)((x*V4.x) + (y*V4.y) + (z*V4.z) + (w*V4.w));
}

vec4 vec4::Cross(vec4 const & V4)
{
	return vec4();
}

r32 vec4::Length()
{
	return (r32)sqrt((x*x) + (y*y) + (z*z) + (w*w));
}

vec4 vec4::Normalize(vec4& V4)
{
	r32 Length = V4.Length();
	if(Length <= 0)
	{
		Length = 1;
	}
	return vec4(V4.x/Length, V4.y/Length, V4.z/Length, V4.w/Length);
}

vec4::~vec4()
{
}

/////////////////////////////////////////////////////////////////////////////////
//
//	mat2
//
/////////////////////////////////////////////////////////////////////////////////
mat2::mat2()
{
	m00 = 1; m01 = 0;
	m10 = 0; m11 = 1;
}

mat2::~mat2()
{
}

/////////////////////////////////////////////////////////////////////////////////
//
//	mat3
//
/////////////////////////////////////////////////////////////////////////////////
mat3::mat3()
{
	m00 = 0; m01 = 0; m02 = 0;
	m10 = 0; m11 = 0; m12 = 0;
	m20 = 0; m21 = 0; m22 = 0;
}

mat3::~mat3()
{
}

/////////////////////////////////////////////////////////////////////////////////
//
//	mat4
//
/////////////////////////////////////////////////////////////////////////////////
mat4::mat4()
{
	m00 = 0; m01 = 0; m02 = 0; m03 = 0;
	m10 = 0; m11 = 0; m12 = 0; m13 = 0;
	m20 = 0; m21 = 0; m22 = 0; m23 = 0;
	m30 = 0; m31 = 0; m32 = 0; m33 = 0;
}

mat4 mat4::operator+(mat4 const & M)
{
	mat4 Result;
	Result.m00 = m00+M.m00; Result.m01 = m01+M.m01; Result.m02 = m02+M.m02; Result.m03 = m03+M.m03;
	Result.m10 = m10+M.m10; Result.m11 = m11+M.m11; Result.m12 = m12+M.m12; Result.m13 = m13+M.m13;
	Result.m20 = m20+M.m20; Result.m21 = m21+M.m21; Result.m22 = m22+M.m22; Result.m23 = m23+M.m23;
	Result.m30 = m30+M.m30; Result.m31 = m31+M.m31; Result.m32 = m32+M.m32; Result.m33 = m33+M.m33;
	return Result;
}

mat4 mat4::operator-(mat4 const & M)
{
	mat4 Result;
	Result.m00 = m00-M.m00; Result.m01 = m01-M.m01; Result.m02 = m02-M.m02; Result.m03 = m03-M.m03;
	Result.m10 = m10-M.m10; Result.m11 = m11-M.m11; Result.m12 = m12-M.m12; Result.m13 = m13-M.m13;
	Result.m20 = m20-M.m20; Result.m21 = m21-M.m21; Result.m22 = m22-M.m22; Result.m23 = m23-M.m23;
	Result.m30 = m30-M.m30; Result.m31 = m31-M.m31; Result.m32 = m32-M.m32; Result.m33 = m33-M.m33;
	return Result;
}

mat4 mat4::operator*(mat4 const & M)
{
	mat4 Result;

	Result.m00 = m00*M.m00 + m01*M.m10 + m02*M.m20 + m03*M.m30;
	Result.m01 = m00*M.m01 + m01*M.m11 + m02*M.m21 + m03*M.m31;
	Result.m02 = m00*M.m02 + m01*M.m12 + m02*M.m22 + m03*M.m32;
	Result.m03 = m00*M.m03 + m01*M.m13 + m02*M.m23 + m03*M.m33;

	Result.m10 = m10*M.m00 + m11*M.m10 + m12*M.m20 + m13*M.m30;
	Result.m11 = m10*M.m01 + m11*M.m11 + m12*M.m21 + m13*M.m31;
	Result.m12 = m10*M.m02 + m11*M.m12 + m12*M.m22 + m13*M.m32;
	Result.m13 = m10*M.m03 + m11*M.m13 + m12*M.m23 + m13*M.m33;

	Result.m20 = m20*M.m00 + m21*M.m10 + m22*M.m20 + m23*M.m30;
	Result.m21 = m20*M.m01 + m21*M.m11 + m22*M.m21 + m23*M.m31;
	Result.m22 = m20*M.m02 + m21*M.m12 + m22*M.m22 + m23*M.m32;
	Result.m23 = m20*M.m03 + m21*M.m13 + m22*M.m23 + m23*M.m33;

	Result.m20 = m30*M.m00 + m31*M.m10 + m32*M.m20 + m33*M.m30;
	Result.m31 = m30*M.m01 + m31*M.m11 + m32*M.m21 + m33*M.m31;
	Result.m32 = m30*M.m02 + m31*M.m12 + m32*M.m22 + m33*M.m32;
	Result.m33 = m30*M.m03 + m31*M.m13 + m32*M.m23 + m33*M.m33;

	return Result;
}

mat4 mat4::operator/(mat4 const & M)
{
	mat4 Result;
	Result.m00 = m00/M.m00; Result.m01 = m01/M.m01; Result.m02 = m02/M.m02; Result.m03 = m03/M.m03;
	Result.m10 = m10/M.m10; Result.m11 = m11/M.m11; Result.m12 = m12/M.m12; Result.m13 = m13/M.m13;
	Result.m20 = m20/M.m20; Result.m21 = m21/M.m21; Result.m22 = m22/M.m22; Result.m23 = m23/M.m23;
	Result.m30 = m30/M.m30; Result.m31 = m31/M.m31; Result.m32 = m32/M.m32; Result.m33 = m33/M.m33;
	return Result;
}
 
mat4 mat4::Identity()
{
	mat4 Result;
	Result.m00 = 1; Result.m01 = 0; Result.m02 = 0; Result.m03 = 0;
	Result.m10 = 0; Result.m11 = 1; Result.m12 = 0; Result.m13 = 0;
	Result.m20 = 0; Result.m21 = 0; Result.m22 = 1; Result.m23 = 0;
	Result.m30 = 0; Result.m31 = 0; Result.m32 = 0; Result.m33 = 1;
	return Result;
}
 
mat4 mat4::Transpose(mat4 const & M)
{
	mat4 Result;
	Result.m00 = M.m00; Result.m01 = M.m10; Result.m02 = M.m20; Result.m03 = M.m30;
	Result.m10 = M.m01; Result.m11 = M.m11; Result.m12 = M.m21; Result.m13 = M.m31;
	Result.m20 = M.m02; Result.m21 = M.m12; Result.m22 = M.m22; Result.m23 = M.m32;
	Result.m30 = M.m03; Result.m31 = M.m13; Result.m32 = M.m23; Result.m33 = M.m33;
	return Result;
}

mat4 mat4::Scale(r32 Sx, r32 Sy, r32 Sz)
{
	mat4 Result;
	Result.m00 = Sx; Result.m01 = 0;  Result.m02 = 0;  Result.m03 = 0;
	Result.m10 = 0;  Result.m11 = Sy; Result.m12 = 0;  Result.m13 = 0;
	Result.m20 = 0;  Result.m21 = 0;  Result.m22 = Sz; Result.m23 = 0;
	Result.m30 = 0;  Result.m31 = 0;  Result.m32 = 0;  Result.m33 = 1;
	return Result;
}

mat4 mat4::Translate(r32 X, r32 Y, r32 Z)
{
	mat4 Result = mat4::Identity();
	Result.m03 = X;
	Result.m13 = Y;
	Result.m23 = Z;
	return Result;
}

mat4 mat4::LookAt(vec3 Eye, vec3 Direction, vec3 Up)
{
	mat4 Matrix;

	vec3 Forward = Eye - Direction;
	Forward = vec3::Normalize(Forward);
	vec3 Left = Up.Cross(Forward);
	Left = vec3::Normalize(Left);
	Up = Forward.Cross(Left);

	r32 TX, TY, TZ;
	TX = -Left.x*Eye.x 		- 	Left.y*Eye.y 	- 		Left.z*Eye.z;
	TY = -Up.x*Eye.x 		-	Up.y*Eye.y 		- 		Up.z*Eye.z;
	TZ = -Forward.x*Eye.x 	- 	Forward.y*Eye.y -	 	Forward.z*Eye.z;

	Matrix.m00 = Left.x;  	 Matrix.m01 = Left.y;  	 Matrix.m02 = Left.z;  	 Matrix.m03 = TX;
	Matrix.m10 = Up.x;  	 Matrix.m11 = Up.y;  	 Matrix.m12 = Up.z;  	 Matrix.m13 = TY;
	Matrix.m20 = Forward.x;  Matrix.m21 = Forward.y; Matrix.m22 = Forward.z; Matrix.m23 = TZ;
	Matrix.m30 = 0;  		 Matrix.m31 = 0;  		 Matrix.m32 = 0;  		 Matrix.m33 = 1;

	return Matrix;
}

mat4 mat4::Orthographic(r32 L, r32 R, r32 T, r32 B, r32 N, r32 F)
{
	mat4 Matrix;

	Matrix.m00 = 2/(R-L); 	Matrix.m01 = 0; 		Matrix.m02 = 0; 		Matrix.m03 = -(R+L)/(R-L);
	Matrix.m10 = 0; 		Matrix.m11 = 2/(T-B); 	Matrix.m12 = 0; 		Matrix.m13 = -(T+B)/(T-B);
	Matrix.m20 = 0; 		Matrix.m21 = 0; 		Matrix.m22 = -2/(F-N); 	Matrix.m23 = -(F+N)/(F-N);
	Matrix.m30 = 0; 		Matrix.m31 = 0; 		Matrix.m32 = 0; 		Matrix.m33 = 1;

	return Matrix;
}

mat4 mat4::Perspective(r32 Fov, r32 Aspect, r32 Near, r32 Far)
{
	mat4 Matrix;
	Matrix = mat4::Identity();
	r32 tanThetaOver2 = tanf(Fov * (r32)M_PI/360);

	Matrix.m00 = 1/tanThetaOver2;
	Matrix.m11 = Aspect/tanThetaOver2;
	Matrix.m22 = (Near + Far)/(Near - Far);
	Matrix.m23 = (2 * Near * Far)/(Near - Far);
	Matrix.m32 = -1;
	Matrix.m33 = 0;
 
	return Matrix;
}

mat4::~mat4()
{
}