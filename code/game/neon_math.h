#ifndef NEON_MATH_H
#define NEON_MATH_H

#define M_PI        3.14159265358979323846f
#define DEG2RAD(Angle) ((Angle)*M_PI/180.0f)
#define RAD2DEG(Angle) ((Angle)*180.0f/M_PI)
#define INT_MIN     (-2147483647 - 1)
#define INT_MAX     2147483647
#define FLT_MAX     3.402823466e+38F

#include <math.h>

#include "../platform/neon_platform.h"

inline
r32 Clamp(r32 Min, r32 Value, r32 Max)
{
    r32 Result = Value;
    if(Value < Min)
    {
        Result = Min;
    }
    else if(Value > Max)
    {
        Result = Max;
    }
    return Result;
}

inline
s32 Clamp(s32 Min, s32 Value, s32 Max)
{
    s32 Result = Value;
    if(Value < Min)
    {
        Result = Min;
    }
    else if(Value > Max)
    {
        Result = Max;
    }
    return Result;
}

inline
r32 Lerp(r32 A, r32 t, r32 B)
{
    r32 Result = (1.0f - t) * A + t * B;

    return Result;
}

//-----------------------------------------------------------------------------
// Vector
//-----------------------------------------------------------------------------

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

struct vec3
{
    union
    {
        struct
        {
            r32 x, y, z;
        };
        struct
        {
            vec2 xy;
            r32 z;
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

inline vec3 Lerp(vec3 A, r32 t, vec3 B) { vec3 Result = (1.0f - t) * A + t * B; return Result; }

struct vec4
{
    union
    {
        struct
        {
            union
            {
                vec3 xyz;
                struct
                {
                    r32 x, y, z;
                };
            };
            r32 w;
        };
        struct
        {
            r32 r, g, b, a;
        };
        struct
        {
            vec2 xy;
            r32 _Ignored0;
            r32 _Ignored1;
        };
        struct
        {
            r32 _Ignored2;
            vec2 yz;
            r32 _Ignored3;
        };
        struct
        {
            r32 _Ignored4;
            r32 _Ignored5;
            vec2 zw;
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
    FORCE_INLINE explicit mat4(r32 e) { m00 = e; m10 = e; m20 = e; m30 = e; 
                                        m01 = e; m11 = e; m21 = e; m31 = e;
                                        m02 = e; m12 = e; m22 = e; m32 = e;
                                        m03 = e; m13 = e; m23 = e; m33 = e; }
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

FORCE_INLINE vec4 operator*(mat4 const &A, vec4 const &B)
{
    return vec4(
        A.m00 * B.x + A.m01 * B.y + A.m02 * B.z + A.m03 * B.w,
        A.m10 * B.x + A.m11 * B.y + A.m12 * B.z + A.m13 * B.w,
        A.m20 * B.x + A.m21 * B.y + A.m22 * B.z + A.m23 * B.w,
        A.m30 * B.x + A.m31 * B.y + A.m32 * B.z + A.m33 * B.w
    );
}

FORCE_INLINE mat4 Mat4Identity()
{
    mat4 A;
    A.m00 = 1.0f; A.m01 = 0.0f; A.m02 = 0.0f; A.m03 = 0.0f;
    A.m10 = 0.0f; A.m11 = 1.0f; A.m12 = 0.0f; A.m13 = 0.0f;
    A.m20 = 0.0f; A.m21 = 0.0f; A.m22 = 1.0f; A.m23 = 0.0f;
    A.m30 = 0.0f; A.m31 = 0.0f; A.m32 = 0.0f; A.m33 = 1.0f;
    return A;
}

FORCE_INLINE mat4 Transpose(mat4 const &A)
{
    mat4 Result;

    for(int I = 0; I <= 3; ++I)
    {
        for(int J = 0; J <= 3; ++J)
        {
            Result.Elements[4 * I + J] = A.Elements[I + J * 4];
        }
    }

    return Result;
}

FORCE_INLINE mat4 Translate(r32 _x, r32 _y, r32 _z)
{
    mat4 A = Mat4Identity();
    A.m03 = _x;
    A.m13 = _y;
    A.m23 = _z;
    return A;
}

FORCE_INLINE mat4 Scale(r32 _x, r32 _y, r32 _z)
{
    mat4 A = Mat4Identity();
    A.m00 = _x;
    A.m11 = _y;
    A.m22 = _z;
    return A;
}

FORCE_INLINE mat4 Rotation(vec3 Axis, r32 Angle)
{
    Axis = Normalize(Axis);
    r32 S = sinf(Angle);
    r32 C = cosf(Angle);
    r32 OC = 1 - C;

    mat4 A;

    A.m00 = C+Axis.x*Axis.x*OC;         A.m01 = Axis.x*Axis.y* OC-Axis.z*S; A.m02 = Axis.x*Axis.z*OC+Axis.y*S;  A.m03 = 0.0f;
    A.m10 = Axis.y*Axis.x*OC+Axis.z*S;  A.m11 = C+Axis.y*Axis.y*OC;         A.m12 = Axis.y*Axis.z*OC-Axis.x*S;  A.m13 = 0.0f;
    A.m20 = Axis.z*Axis.x*OC-Axis.y*S;  A.m21 = Axis.z*Axis.y*OC+Axis.x*S;  A.m22 = C+Axis.z*Axis.z*OC;         A.m23 = 0.0f;
    A.m30 = 0.0f;                       A.m31 = 0.0f;                       A.m32 = 0.0f;                       A.m33 = 1.0f;

    return A;
}

FORCE_INLINE mat4 XRotation(r32 Angle)
{
    r32 C = cosf(Angle);
    r32 S = sinf(Angle);

    mat4 A;
    
    A.m11 = C;
    A.m12 = -S;
    A.m21 = S;
    A.m22 = C;

    return A;
}

FORCE_INLINE mat4 YRotation(r32 Angle)
{
    r32 C = cosf(Angle);
    r32 S = sinf(Angle);

    mat4 A = Mat4Identity();

    A.m00 = C;
    A.m02 = S;
    A.m20 = -S;
    A.m22 = C;

    return A;
}

FORCE_INLINE mat4 ZRotation(r32 Angle)
{
    r32 C = cosf(Angle);
    r32 S = sinf(Angle);

    mat4 A = Mat4Identity();

    A.m00 = C;
    A.m01 = -S;
    A.m10 = S;
    A.m11 = C;

    return A;
}

FORCE_INLINE mat4 Screenspace(s32 WindowWidth, s32 WindowHeight)
{
    mat4 Matrix = mat4(0.0f);

    Matrix.m00 = 2.0f / WindowWidth;
    Matrix.m03 = -1.0f;
    Matrix.m11 = 2.0f / WindowHeight;
    Matrix.m13 = -1.0f;
    Matrix.m22 = -1.0f;
    Matrix.m33 = 1.0f;

    return Matrix;
}

FORCE_INLINE mat4 Orthographic(r32 Left, r32 Right, r32 Top, r32 Bottom, r32 Near, r32 Far)
{
    mat4 Matrix = mat4(0.0f);

    Matrix.m00 = 2.0f / (Right - Left);
    Matrix.m03 = -(Right + Left) / (Right - Left);
    Matrix.m11 = 2.0f / (Top - Bottom);
    Matrix.m13 = -(Top + Bottom) / (Top - Bottom);
    Matrix.m22 = -2.0f / (Far - Near);
    Matrix.m23 = -(Far + Near) / (Far - Near);
    Matrix.m33 = 1.0f;

    return Matrix;
}

FORCE_INLINE mat4 Perspective(r32 Fov, r32 Aspect, r32 Near, r32 Far)
{
    mat4 Matrix = mat4(0.0f);

    r32 TanFovOver2 = tanf(Fov / 2.0f);

    Matrix.m00 = 1.0f / (Aspect * TanFovOver2);
    Matrix.m11 = 1.0f / TanFovOver2;
    Matrix.m22 = -(Far + Near) / (Far - Near);
    Matrix.m23 = -(2.0f * Far * Near) / (Far - Near);
    Matrix.m32 = -1.0f;

    return Matrix;
}

FORCE_INLINE mat4 LookAt(vec3 Eye, vec3 Direction, vec3 Up)
{
    mat4 Matrix;

    vec3 Forward = Normalize(Eye - Direction);
    vec3 Left = Normalize(Cross(Up, Forward));
    
    Up = Cross(Forward, Left);

    r32 TX, TY, TZ;
    TX = Dot(Left, -Eye);
    TY = Dot(Up, -Eye);
    TZ = Dot(Forward, -Eye);

    Matrix.m00 = Left.x;     Matrix.m01 = Left.y;    Matrix.m02 = Left.z;    Matrix.m03 = TX;
    Matrix.m10 = Up.x;       Matrix.m11 = Up.y;      Matrix.m12 = Up.z;      Matrix.m13 = TY;
    Matrix.m20 = Forward.x;  Matrix.m21 = Forward.y; Matrix.m22 = Forward.z; Matrix.m23 = TZ;
    Matrix.m30 = 0;          Matrix.m31 = 0;         Matrix.m32 = 0;         Matrix.m33 = 1;

    return Matrix;
}

inline vec4 RGBAUnpack4x8(u32 Packed)
{
    vec4 Result;
    Result.r = (r32)((Packed >> 24) & 0xFF);
    Result.g = (r32)((Packed >> 16) & 0xFF);
    Result.b = (r32)((Packed >> 8) & 0xFF);
    Result.a = (r32)((Packed >> 0) & 0xFF);
    return Result;
}

inline vec4 RGBA255To01(vec4 C)
{
    vec4 Result;
    r32 Inv255 = 1.0f / 255.0f;
    Result.r = C.r * Inv255;
    Result.g = C.g * Inv255;
    Result.b = C.b * Inv255;
    Result.a = C.a * Inv255;
    return Result;
}

inline vec4 SRGBToLinear(vec4 C)
{
    vec4 Result;
    Result.r = powf(C.r, 2.2f);
    Result.g = powf(C.g, 2.2f);
    Result.b = powf(C.b, 2.2f);
    Result.a = C.a;
    return Result;
}

inline vec4 LinearToSRGB(vec4 C)
{
    vec4 Result;
    r32 InvGamma = 1.0f / 2.2f;
    Result.r = powf(C.r, InvGamma);
    Result.g = powf(C.g, InvGamma);
    Result.b = powf(C.b, InvGamma);
    Result.a = C.a;
    return Result;
}

inline vec4 SRGB255ToLinear1(vec4 C)
{
    vec4 Result;
    r32 Inv255 = 1.0f / 255.0f;
    Result.r = powf(C.r * Inv255, 2.2f);
    Result.g = powf(C.g * Inv255, 2.2f);
    Result.b = powf(C.b * Inv255, 2.2f);
    Result.a = C.a / 255.0f;
    return Result;
}

inline vec4 Linear1ToSRGB255(vec4 C)
{
    vec4 Result;
    r32 InvGamma = 1.0f / 2.2f;
    Result.r = 255.0f * powf(C.r, InvGamma);
    Result.g = 255.0f * powf(C.g, InvGamma);
    Result.b = 255.0f * powf(C.b, InvGamma);
    Result.a = 255.0f * C.a;
    return Result;
}

struct rect
{
    r32 x;
    r32 y;
    r32 width;
    r32 height;
};

FORCE_INLINE rect Rect(r32 _x, r32 _y, r32 _width, r32 _height)
{
    rect R;
    R.x = _x;
    R.y = _y;
    R.width = _width;
    R.height = _height;
    return R;
}
#endif