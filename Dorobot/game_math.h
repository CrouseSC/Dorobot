#pragma once
#include "vectors.h"
#include "cod4_structs.h"

namespace mm
{
	constexpr float PI = 3.14159265358979323846f;
	Vec2<float> truncate_vector(const Vec3<float>& v);
	Vec2<float> rotate_point(const Vec2<float>& p1, const Vec2<float>& p2, float angle);  //p1 around p2
	float normalise(float value, float start, float end);
	double short_to_angle(int angle);
	float tilt_angle(const Vec2<float>& v);
	float to_degrees(float radians);
	void angle_vectors(Vec3<float> angles, float* forward, float* right, float* up);
	void angle_vectors(Vec3<float> angles, Vec3<float>* forward, Vec3<float>* right, Vec3<float>* up);
	float toRadians(float angle);
}
