#pragma once

#include "vec3.h"
#include "vec3_utility.h"

struct matrix4x4
{
	vec4 data[4];

	// | [0][0]	[0][1]	[0][2]	[0][3]	 |
	// | [1][0]	[1][1]	[1][2]	[1][3]	 |
	// | [2][0]	[2][1]	[2][2]	[2][3]	 |
	// | [3][0]	[3][1]	[3][2]	[3][3]	 |

	matrix4x4(float a1 = 1.0f, float a2 = 0.0f, float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f, float a6 = 1.0f,
	          float a7 = 0.0f, float a8 = 0.0f, float a9 = 0.0f, float a10 = 0.0f,
	          float a11 = 1.0f, float a12 = 0.0f, float a13 = 0.0f, float a14 = 0.0f, float a15 = 0.0f,
	          float a16 = 1.0f)
		: data{
			vec4(a1, a2, a3, a4),
			vec4(a5, a6, a7, a8),
			vec4(a9, a10, a11, a12),
			vec4(a13, a14, a15, a16)
		}
	{
	}

	matrix4x4(const vec4& row0, const vec4& row1, const vec4& row2, const vec4& row3)
		: data{row0, row1, row2, row3}
	{
	}

	__forceinline float& at(int l, int c) { return data[l][c]; }
	__forceinline float at(int l, int c) const { return data[l][c]; }

	static matrix4x4 multiply(const matrix4x4& a, const matrix4x4& b)
	{
		matrix4x4 result;
		for (int j = 0; j < 4; j++)
			for (int i = 0; i < 4; i++)
				result.at(j, i) = a.at(j, 0) * b.at(0, i) + a.at(j, 1) * b.at(1, i) + a.at(j, 2) * b.at(2, i) + a.
					at(j, 3) * b.at(3, i);
		return result;
	}

	template <typename TVec>
	static TVec multiply(const matrix4x4& a, const TVec& b)
	{
		auto t = a.transpose();
		return TVec{
			dot(b, t.data[0]),
			dot(b, t.data[1]),
			dot(b, t.data[2])
		};
	}

	static matrix4x4 from_euler_degrees(const vec3& euler)
	{
		auto t = degrees_to_radians(euler.x());
		const matrix4x4 x{
			1, 0, 0, 0,
			0, cos(t), sin(t), 0,
			0, -sin(t), cos(t), 0,
			0, 0, 0, 1
		};
		t = degrees_to_radians(euler.y());
		const matrix4x4 y{
			cos(t), 0, -sin(t), 0,
			0, 1, 0, 0,
			sin(t), 0, cos(t), 0,
			0, 0, 0, 1
		};
		t = degrees_to_radians(euler.z());
		const matrix4x4 z{
			cos(t), sin(t), 0, 0,
			-sin(t), cos(t), 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		};

		return multiply(multiply(x, y), z);
	}

	[[nodiscard]] matrix4x4 transpose() const
	{
		matrix4x4 transposed(*this);
		_MM_TRANSPOSE4_PS(transposed.data[0].data.sse, transposed.data[1].data.sse, transposed.data[2].data.sse, transposed.data[3].data.sse);
		return transposed;
	}

	void translate(const vec3& v)
	{
		data[3][0] += v[0];
		data[3][1] += v[1];
		data[3][2] += v[2];
	}
};