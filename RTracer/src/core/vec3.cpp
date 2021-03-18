#include "vec3.h"

#include "random.h"

template <int N>
struct static_cbrt
{
	static_cbrt(const random::static_random_generator<N, float>& generator) : arr()
	{
		for (auto i = 0; i != N; ++i)
			arr[i] = std::cbrt(generator.arr[i]);
	}

	float get(random::static_random_generator<N, float>& generator)
	{
		return arr[generator.index++];
	}

	float arr[N];
	int index = 0;
};

static auto s_cbrt = static_cbrt(random::static_float);

namespace vector3
{
	glm::vec3 multiply_point_fast(const glm::vec3& v, const glm::mat4& m)
	{
		return vec3{
			(m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z) + m[3][0],
			(m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z) + m[3][1],
			(m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z) + m[3][2]
		};
	}

	vec3 random_in_unit_sphere(float sphere_angle)
	{
		const auto u = random::static_float.get();
		const auto v = random::static_float.get();
		const auto theta = u * sphere_angle;
		const auto phi = std::acos(2.0f * v - 1.0f);
		const auto r = s_cbrt.get(random::static_float);
		const auto sinTheta = std::sin(theta);
		const auto cosTheta = std::cos(theta);
		const auto sinPhi = std::sin(phi);
		const auto cosPhi = std::cos(phi);
		const auto x = r * sinPhi * cosTheta;
		const auto y = r * sinPhi * sinTheta;
		const auto z = r * cosPhi;
		return vec3(x, y, z);
	}

	vec3 random_in_unit_disk()
	{
		const auto u = random::static_float.get();
		const auto v = random::static_float.get();
		const auto theta = u * constants::pi * 2.0f;
		const auto phi = std::acos(2.0f * v - 1.0f);
		const auto r = s_cbrt.get(random::static_float);
		const auto sinTheta = std::sin(theta);
		const auto cosTheta = std::cos(theta);
		const auto sinPhi = std::sin(phi);
		const auto x = r * sinPhi * cosTheta;
		const auto y = r * sinPhi * sinTheta;
		return vec3(x, y, 0.0f);
	}

	vec3 random_in_hemisphere(const vec3& normal)
	{
		return random_in_unit_sphere(constants::pi * sign(dot(normal, up()) + constants::epsilon));
	}
}
