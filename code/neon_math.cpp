#include "neon_math.h"

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
////
////  vec2
////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
vec2::vec2()
{
	Elements[0] = 0;
	Elements[1] = 0;
}

vec2::vec2(r32 x, r32 y)
{
	Elements[0] = x;
	Elements[1] = y;
}

vec2::vec2(s32 x, s32 y)
{
	Elements[0] = (r32)x;
	Elements[1] = (r32)y;
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

r32 vec2::Dot(vec2 const & V2)
{
	return (r32)((x * V2.x) + (y * V2.y));
}

r32 vec2::Cross(vec2 const & V2)
{
	return x * V2.y - y * V2.x;
	// r = x1 y2 - y1 x2
}

r32 vec2::Length()
{
	return (r32)sqrt((x*x) + (y*y));
}

vec2::~vec2()
{
}

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
////
////  vec3
////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
vec3::vec3()
{
	Elements[0] = 0;
	Elements[1] = 0;
	Elements[2] = 0;
}

vec3::vec3(r32 x, r32 y, r32 z)
{
	Elements[0] = x;
	Elements[1] = y;
	Elements[2] = z;
}

vec3::vec3(s32 x, s32 y, s32 z)
{
	Elements[0] = (r32)x;
	Elements[1] = (r32)y;
	Elements[2] = (r32)z;
}

vec3::vec3(vec2 const & V2, r32 z)
{
	Elements[0] = V2.x;
	Elements[1] = V2.y;
	Elements[2] = z;
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

r32 vec3::Dot(vec3 const & V3)
{
	return (r32)((x * V3.x) + (y * V3.y) + (z * V3.z));
}

vec3 vec3::Cross(vec3 const & V3)
{
	return vec3(y * V3.z - z * V3.y, z * V3.x - x * V3.z, x * V3.y - y * V3.x);

	// C = A B - A B
	// x = y z - z y
	// y = z x - x z
	// z = x y - y x
}

r32 vec3::Length()
{
	return (r32)sqrt((x*x) + (y*y) + (z*z));
}

vec3::~vec3()
{
}

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
////
////  vec4
////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

vec4::vec4()
{
	Elements[0] = 0;
	Elements[1] = 0;
	Elements[2] = 0;
	Elements[3] = 0;
}

vec4::vec4(r32 x, r32 y, r32 z, r32 w)
{
	Elements[0] = x;
	Elements[1] = y;
	Elements[2] = z;
	Elements[3] = w;
}

vec4::vec4(s32 x, s32 y, s32 z, s32 w)
{
	Elements[0] = (r32)x;
	Elements[1] = (r32)y;
	Elements[2] = (r32)z;
	Elements[3] = (r32)w;
}

vec4::vec4(vec3 const & V3, r32 w)
{
	Elements[0] = V3.x;
	Elements[1] = V3.y;
	Elements[2] = V3.z;
	Elements[3] = w;
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

vec4::~vec4()
{
}

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
////
////  mat2
////
/////////////////////////////////////////////////////////////////////////////////
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
/////////////////////////////////////////////////////////////////////////////////
////
////  mat3
////
/////////////////////////////////////////////////////////////////////////////////
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
/////////////////////////////////////////////////////////////////////////////////
////
////  mat4
////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
mat4::mat4()
{
	m00 = 0; m01 = 0; m02 = 0; m03 = 0;
	m10 = 0; m11 = 0; m12 = 0; m13 = 0;
	m20 = 0; m21 = 0; m22 = 0; m23 = 0;
	m30 = 0; m31 = 0; m32 = 0; m33 = 0;
}

mat4::~mat4()
{

}

mat4 OrthoMat4(r32 L, r32 R, r32 T, r32 B, r32 N, r32 F)
{
	mat4 Matrix;
	Matrix.m00 = 2/(R-L); 		Matrix.m01 = 0; 			Matrix.m02 = 0; 			Matrix.m03 = -(R+L)/(R-L);
	Matrix.m10 = 0; 			Matrix.m11 = 2/(T-B); 		Matrix.m12 = 0; 			Matrix.m13 = -(T+B)/(T-B);
	Matrix.m20 = 0; 			Matrix.m21 = 0; 			Matrix.m22 = -2/(F-N); 		Matrix.m23 = -(F+N)/(F-N);
	Matrix.m30 = 0; 			Matrix.m31 = 0; 			Matrix.m32 = 0; 			Matrix.m33 = 1;

	return Matrix;
}