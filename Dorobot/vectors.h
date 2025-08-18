#pragma once
#define _USE_MATH_DEFINES
#include <math.h>
#include <sstream>
#include <iomanip>
#include "imgui-docking/imgui.h"
#include "imgui-docking/imgui_internal.h"
typedef float vec_t;
typedef vec_t vec2_t[2];
typedef vec_t vec3_t[3];
typedef vec_t vec4_t[4];
typedef vec_t vec5_t[5];

template <typename T>
struct Vec2
{
public:
	Vec2() { x = 0;  y = 0; };
	Vec2(T vec_x, T vec_y) { x = vec_x; y = vec_y; }
	Vec2(const Vec2& v) { x = v.x; y = v.y; }
	inline Vec2 operator + (const Vec2& v) const { return Vec2(x + v.x, y + v.y); }
	inline Vec2 operator - (const Vec2& v) const { return Vec2(x - v.x, y - v.y); }
	inline Vec2 operator / (const Vec2& v) const { return Vec2(x / v.x, y / v.y); }
	inline Vec2 operator * (const Vec2& v) const { return Vec2(x * v.x, y * v.y); }
	inline bool operator == (const Vec2& v) const { return x == v.x && y == v.y; }
	inline bool operator != (const Vec2& v) const { return x != v.x && y != v.y; }
	inline Vec2& operator += (const Vec2& v) { this->x += v.x; this->y += v.y; return *this; }
	inline Vec2& operator -= (const Vec2& v) { this->x += v.x; this->y += v.y; return *this; }
	inline Vec2& operator *= (const Vec2& v) { this->x *= v.x, this->y *= v.y; return *this; }
	inline Vec2& operator /= (const Vec2& v) { this->x += v.x; this->y += v.y; return *this; }
	inline T& operator [] (int index) { T f = FLT_MIN; if (index == 0) return x;  if (index == 1) return y; return f; }
	const T& operator [] (int index) const { if (index == 0) return x;  if (index == 1) return y; return FLT_MIN; }
	ImVec2 toImvec2() const { return ImVec2(x, y); }
	inline double Length() { return sqrtf(pow(x, 2) + pow(y, 2)); }
	static Vec2 tovec2(const ImVec2& v) { return Vec2(v.x, v.y); }
	inline double Dist(const Vec2 v) { return (*this - v).Length(); }
	inline Vec2 Snapped() { return Vec2(roundf(x), roundf(y)); }
	T x, y;
};


template <typename T>
struct Vec3
{
	T x, y, z;
	Vec3() { x = 0; y = 0; z = 0; };
	Vec3(vec3_t vec) { x = vec[0]; y = vec[1]; z = vec[2]; }
	Vec3(T vec_x, T vec_y, T vec_z) { x = vec_x; y = vec_y; z = vec_z; }
	Vec3(const Vec3& v) { x = v.x; y = v.y; z = v.z; }
	inline Vec3 operator + (const Vec3& v) { return Vec3(x + v.x, y + v.y, z + v.z); }
	inline Vec3 operator - (const Vec3& v) { return Vec3(x - v.x, y - v.y, z - v.z); }
	inline Vec3 operator / (const Vec3& v) { return Vec3(x / v.x, y / v.y, z / v.z); }
	inline Vec3 operator * (const Vec3& v) { return Vec3(x * v.x, y * v.y, z * v.z); }
	inline Vec3 operator * (double c) { return Vec3(x * c, y * c, z * c); }
	inline bool operator == (const Vec3& v) { return x == v.x && y == v.y && z == v.z; }
	inline bool operator != (const Vec3& v) { return x != v.x || y != v.y || z != v.z; }
	inline T& operator [] (int index) { if (index == 0) return x;  if (index == 1) return y; if (index == 2) return z; return z; }
	const T& operator [] (int index) const { if (index == 0) return x;  if (index == 1) return y; if (index == 2) return z; return FLT_MIN; }
	inline Vec3& operator += (const Vec3& v) { this->x += v.x; this->y += v.y; this->z += v.z; return *this; }
	inline Vec3& operator -= (const Vec3& v) { this->x -= v.x; this->y -= v.y; this->z -= v.z; return *this; }
	inline Vec3& operator *= (const Vec3& v) { this->x *= v.x; this->y *= v.y; this->z *= v.z; return *this; }
	inline Vec3& operator *= (double c) { this->x *= c; this->y *= c; this->z *= c; return *this; }
	inline Vec3& operator /= (const Vec3& v) { this->x /= v.x; this->y /= v.y; this->z /= v.z; return *this; }

	inline std::string toString() { std::stringstream ss; std::setprecision(6); ss << std::fixed << x << " " << y << " " << z; return ss.str(); }
	inline Vec3 CrossProduct(const Vec3 v) { return Vec3(v.y * this.z - v.z * this.y, -(v.x * this.z - v.z * this.x), v.x * this.y - v.y * this.x); }
	inline double Length() { return sqrtf(pow(x, 2) + pow(y, 2) + pow(z, 2)); }
	inline double Length2D() { return sqrtf(pow(x, 2) + pow(y, 2)); }
	inline double Length3D() { return sqrtf(pow(x, 2) + pow(y, 2) + pow(z,2)); }
	inline double Length2DRounded() { return sqrtf(pow(roundf(x), 2) + pow(roundf(y), 2)); }
	inline double LengthZ() { return sqrt(pow(y, 2)); }
	inline T DotProduct(const Vec3 v) { return (x * v.x + y * v.y + z * v.z); }
	inline Vec3 Rounded() { return Vec3(roundf(this->x), roundf(this->y), roundf(this->z)); }
	inline Vec3 Floored() { return Vec3(floorf(this->x), floorf(this->y), floorf(this->z)); }
	inline Vec3 Ceiled() { return Vec3(ceilf(this->x), ceilf(this->y), ceilf(this->z)); }
	inline double Dist(const Vec3 v) { return (*this - v).Length(); }
	inline double Dist2D(const Vec3 v) { return (Vec2(x, y) - Vec2(v.x, v.y)).Length(); }
	inline T* v3t() { T x[3]; x[0] = this->x; x[1] = this->y; x[2] = this->z;  return x; }
	inline Vec3 normalize360() { return Vec3(fmodf(this->x + 360, 360), fmodf(this->y + 360, 360), fmodf(this->z + 360, 360)); }
	inline Vec3 getFractional() { T whole; return { std::modf(x, &whole), std::modf(y, &whole),std::modf(z, &whole) }; }
	inline Vec3 getFractionalAbs() { T whole; return { fabs(std::modf(x, &whole)), fabs(std::modf(y, &whole)), fabs(std::modf(z, &whole)) }; }
	inline Vec3 Normalized() { return Vec3(x / Length(), y / Length(), z / Length()); }
	inline void Normalize() { *this = Vec3(x / Length(), y / Length(), z / Length()); }
	inline Vec3 originFromStance(int stance) {
		if (stance == 0) { return { this->x, this->y, this->z - 60 }; };
		if (stance == 1) { return { this->x, this->y, this->z - 40 }; };
		if (stance == 2) { return { this->x, this->y, this->z - 11 }; };
		return *this;
	}
	inline Vec3 clearXYZ() { return Vec3(0,0,0); };
};

template <typename T>
struct Vec4
{
	T x, y, z, w;
	Vec4() { x = 0; y = 0; z = 0; w = 0; };
	Vec4(vec4_t vec) { x = vec[0]; y = vec[1]; z = vec[2]; w = vec[3]; }
	Vec4(T vec_x, T vec_y, T vec_z, T vec_w) { x = vec_x; y = vec_y; z = vec_z; w = vec_w; }
	Vec4(const Vec4& v) { x = v.x; y = v.y; z = v.z; w = v.w; }
	inline Vec4 operator + (const Vec4& v) { return Vec4(x + v.x, y + v.y, z + v.z, w + v.w); }
	inline Vec4 operator - (const Vec4& v) { return Vec4(x - v.x, y - v.y, z - v.z, w - v.w); }
	inline Vec4 operator / (const Vec4& v) { return Vec4(x / v.x, y / v.y, z / v.z, w / v.w); }
	inline Vec4 operator * (const Vec4& v) { return Vec4(x * v.x, y * v.y, z * v.z, w * v.w); }
	inline bool operator == (const Vec4& v) { return x == v.x && y == v.y && z == v.z && w == v.w; }
	inline bool operator != (const Vec4& v) { return x != v.x || y != v.y || z != v.z || w != v.w; }
	inline T& operator [] (int index) { if (index == 0) return x;  if (index == 1) return y; if (index == 2) return z; if (index == 3) return w; return w; }
	const T& operator [] (int index) const { if (index == 0) return x;  if (index == 1) return y; if (index == 2) return z; if (index == 3) return w; return FLT_MIN; }
	inline Vec4& operator += (const Vec4& v) { this->x += v.x; this->y += v.y; this->z += v.z; this->w += v.w; return *this; }
	inline Vec4& operator -= (const Vec4& v) { this->x -= v.x; this->y -= v.y; this->z -= v.z; this->w -= v.w; return *this; }
	inline Vec4& operator *= (const Vec4& v) { this->x *= v.x; this->y *= v.y; this->z *= v.z; this->w *= v.w; return *this; }
	inline Vec4& operator /= (const Vec4& v) { this->x /= v.x; this->y /= v.y; this->z /= v.z; this->w /= v.w; return *this; }
	inline T* v4t() { T x[4]; x[0] = this->x; x[1] = this->y; x[2] = this->z; x[3] = this->w; return x; }
	inline ImVec4 toImvec4() { return { x, y, z, w }; }
	inline ImColor toImColor() { return { x, y, z, w }; }
};



