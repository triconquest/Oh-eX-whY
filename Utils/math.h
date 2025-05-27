#pragma once

#include "vector.h"

#include <algorithm>

//using matrix3x4_t = float[3][4];

struct matrix3x4_t {
	float m[3][4];

	float* operator[](int i) {
		return m[i];
	}

	const float* operator[](int i) const {
		return m[i];
	}
};

struct ViewMatrix
{
	float* operator[](int index) {
		return matrix[index];
	}

	float matrix[4][4];
};

class VMatrix
{
public:
	float m[4][4];

	inline const matrix3x4_t& As3x4() const {
		return *((const matrix3x4_t*)this);
	}

	inline float* operator[](int i) {
		return m[i];
	}

	inline const float* operator[](int i) const {
		return m[i];
	}
};

namespace Math
{
	inline float GetDistance(Vector3 pos1, Vector3 pos2)
	{
		return sqrt(powf((pos2.x - pos1.x), 2) + powf((pos2.y - pos1.y), 2) + powf((pos2.z - pos1.z), 2));
	}
	inline float GetFOV(const Vector3& viewAngle, const Vector3& aimAngle) {
		Vector3 delta = aimAngle - viewAngle;
		delta.x = std::remainderf(delta.x, 360.0f);
		delta.y = std::remainderf(delta.y, 360.0f);
		return std::sqrt(powf(delta.x, 2.0f) + powf(delta.y, 2.0f));
	}

	inline Vector3 CalcAngle(const Vector3& src, const Vector3& dst) {
		Vector3 delta = src - dst;
		float hyp = delta.Length();
		if (hyp == 0.f) return Vector3(0.f, 0.f, 0.f);
		Vector3 angles;
		angles.x = std::asinf(delta.z / hyp) * (180.0f / M_PI);
		angles.y = std::atan2f(delta.y, delta.x) * (180.0f / M_PI) + 180.0f;
		angles.z = 0.0f;
		return angles;
	}

	inline void NormalizeAngles(Vector3& angles)
	{
		while (angles.x > 89.0f)
			angles.x -= 180.0f;

		while (angles.x < -89.0f)
			angles.x += 180.0f;

		// Normalize yaw
		while (angles.y > 180.0f)
			angles.y -= 360.0f;

		while (angles.y < -180.0f)
			angles.y += 360.0f;

		angles.z = 0.0f;
	}

	inline void Clamp(Vector3& angles) {
		NormalizeAngles(angles);

		if (angles.x > 89.0f)
			angles.x = 89.0f;
		if (angles.x < -89.0f)
			angles.x = -89.0f;

		angles.z = 0.0f;
	}

	inline void SinCos(float radians, float* sine, float* cosine)
	{
		*sine = std::sin(radians);
		*cosine = std::cos(radians);
	}

	inline void AngleVectors(const Vector3& angles, Vector3* forward, Vector3* right, Vector3* up)
	{
		float sr, sp, sy, cr, cp, cy;
		SinCos(DEG2RAD(angles.x), &sp, &cp);
		SinCos(DEG2RAD(angles.y), &sy, &cy);
		SinCos(DEG2RAD(angles.z), &sr, &cr);

		if (forward)
		{
			forward->x = cp * cy;
			forward->y = cp * sy;
			forward->z = -sp;
		}

		if (right)
		{
			right->x = (-1 * sr * sp * cy + -1 * cr * -sy);
			right->y = (-1 * sr * sp * sy + -1 * cr * cy);
			right->z = -1 * sr * cp;
		}

		if (up)
		{
			up->x = (cr * sp * cy + -sr * -sy);
			up->y = (cr * sp * sy + -sr * cy);
			up->z = cr * cp;
		}
	}

	inline void MatrixPosition(const matrix3x4_t& matrix, Vector& out) {
		out.x = matrix[0][3];
		out.y = matrix[1][3];
		out.z = matrix[2][3];
	}

	inline void VectorAngles(const Vector3& forward, Vector3& angles)
	{
		float yaw, pitch;

		if (forward.y == 0.0f && forward.x == 0.0f)
		{
			yaw = 0.0f;
			pitch = (forward.z > 0.0f) ? 270.0f : 90.0f;
		}
		else
		{
			yaw = RAD2DEGF(::atan2f(forward.y, forward.x));
			yaw += (360.0f * (yaw < 0.0f));

			const float tmp = forward.Length2();

			pitch = RAD2DEGF(::atan2f(-forward.z, tmp));
			pitch += (360.0f * (pitch < 0.0f));
		}

		angles[0] = pitch;
		angles[1] = yaw;
		angles[2] = 0.0f;
	}
}

// so studio.h stops bitching
struct Quaternion
{
	float x, y, z, w;
};
