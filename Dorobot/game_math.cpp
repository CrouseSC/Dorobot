#include "game_math.h"
#include "pch.h"

namespace mm
{
	float to_radians(float degrees)
	{
		return degrees * PI / 180.f;
	}

	Vec2<float> truncate_vector(const Vec3<float>& v)
	{
		return Vec2<float>(v.x, v.y);
	}

	Vec2<float> rotate_point(const Vec2<float>& p1, const Vec2<float>& p2, float angle)
	{
		Vec2<float> p;
		angle = to_radians(angle);
		p.x = cosf(angle) * (p1.x - p2.x) - sinf(angle) * (p1.y - p2.y) + p2.x;
		p.y = sinf(angle) * (p1.x - p2.x) + cosf(angle) * (p1.y - p2.y) + p2.y;
		return p;
	}

	float normalise(float value, float start, float end)
	{
		if (end == 90.f) {
			if (value < 90.f) {
				value = normalise(value, 0, 360);  //LOL
			}
			value -= static_cast<int>(value / 90.f) * 90.f;
		}
		else if (end == 180.f) {
			value -= 360.f * std::floor((value + 180.f) * (1.f / 360.f));
		}
		else if (end == 360.f) {
			value -= 360.f * std::floor(value * (1.f / 360.f));
		}

		return value;
	}

	double short_to_angle(int angle)
	{
		return ((angle) * (360.0 / 65536));
	}

	float tilt_angle(const Vec2<float>& v)
	{
		return normalise(std::atan2f(v.y, v.x) * 180.f / PI, 0.f, 360.f);
	}
	float to_degrees(float radians)
	{
		return radians * 180.f / PI;
	}

	void angle_vectors(const Vec3<float> angles, float* forward, float* right, float* up)
	{
		float angle;
		static float  sr, sp, sy, cr, cp, cy, cpi = M_PI * 2 / 360;

		angle = angles[1] * cpi;
		sy = sin(angle);
		cy = cos(angle);
		angle = angles[0] * cpi;
		sp = sin(angle);
		cp = cos(angle);
		angle = angles[2] * cpi;
		sr = sin(angle);
		cr = cos(angle);

		if (forward) {
			forward[0] = cp * cy;
			forward[1] = cp * sy;
			forward[2] = -sp;
		}

		if (right) {
			right[0] = (-1 * sr * sp * cy + -1 * cr * -sy);
			right[1] = (-1 * sr * sp * sy + -1 * cr * cy);
			right[2] = -1 * sr * cp;
		}

		if (up) {
			up[0] = (cr * sp * cy + -sr * -sy);
			up[1] = (cr * sp * sy + -sr * cy);
			up[2] = cr * cp;
		}
	}

	void angle_vectors(Vec3<float> angles, Vec3<float>* forward, Vec3<float>* right, Vec3<float>* up) {
		float angle;
		static float sr, sp, sy, cr, cp, cy, cpi = (M_PI * 2 / 360);

		angle = angles[1] * cpi;
		sy = sin(angle);
		cy = cos(angle);
		angle = angles[0] * cpi;
		sp = sin(angle);
		cp = cos(angle);
		angle = angles[2] * cpi;
		sr = sin(angle);
		cr = cos(angle);

		if (forward) {
			forward->x = cp * cy;
			forward->y = cp * sy;
			forward->z = -sp;
		}

		if (right) {
			right->x = (-1 * sr * sp * cy + -1 * cr * -sy);
			right->y = (-1 * sr * sp * sy + -1 * cr * cy);
			right->z = -1 * sr * cp;
		}

		if (up) {
			up->x = (cr * sp * cy + -sr * -sy);
			up->y = (cr * sp * sy + -sr * cy);
			up->z = cr * cp;
		}
	}

	float toRadians(float angle)
	{
		return angle * (PI / 180);
	}

	Vec3<float> sphericalToCartesian(float magnitude, float yawDeg, float pitchDeg)
	{
		// Convert degrees to radians
		float yaw = yawDeg * static_cast<float>(M_PI) / 180.0f;
		float pitch = pitchDeg * static_cast<float>(M_PI) / 180.0f;

		float x = magnitude * cosf(pitch) * cosf(yaw);
		float y = magnitude * cosf(pitch) * sinf(yaw);
		float z = magnitude * sinf(pitch);

		return Vec3<float>(x, y, z);
	}
}
