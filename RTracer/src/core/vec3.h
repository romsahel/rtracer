#pragma once

#include <cmath>

#include "utility.h"

struct vec3
{
	vec3(double x, double y, double z)
		: m_xyz{x, y, z}
	{
	}

	vec3(double xyz) : vec3(xyz, xyz, xyz)
	{
	}

	vec3() : vec3(0.0)
	{
	}

	double& x() { return m_xyz[0]; }
	double& y() { return m_xyz[1]; }
	double& z() { return m_xyz[2]; }

	const double& x() const { return m_xyz[0]; }
	const double& y() const { return m_xyz[1]; }
	const double& z() const { return m_xyz[2]; }

	double operator[](int index) const
	{
		return m_xyz[index];
	}

	double& operator[](int index)
	{
		return m_xyz[index];
	}

	vec3 operator-() const
	{
		return vec3(-m_xyz[0], -m_xyz[1], -m_xyz[2]);
	}

	vec3& operator+=(const vec3& v)
	{
		m_xyz[0] += v.m_xyz[0];
		m_xyz[1] += v.m_xyz[1];
		m_xyz[2] += v.m_xyz[2];
		return *this;
	}

	vec3& operator-=(const vec3& v)
	{
		return this->operator+=(-v);
	}

	vec3& operator*=(double v)
	{
		m_xyz[0] *= v;
		m_xyz[1] *= v;
		m_xyz[2] *= v;
		return *this;
	}

	vec3& operator/=(double v)
	{
		return *this *= 1.0 / v;
	}

	double length_squared() const
	{
		return x() * x() + y() * y() + z() * z();
	}

	double length() const
	{
		return std::sqrt(length_squared());
	}

	vec3& normalize()
	{
		operator/=(length());
		return *this;
	}

	bool is_near_zero() const
	{
		const static auto e = 1e-8;
		return fabs(m_xyz[0]) < e && fabs(m_xyz[1]) < e && fabs(m_xyz[2]) < e;
	}

	// return a random vec3
	static vec3 random(double min = 0.0, double max = 1.0)
	{
		return vec3(random::get<double>(min, max), random::get<double>(min, max), random::get<double>(min, max));
	}

	// return a random vec3 contained in a sphere placed at the origin and of a radius of 1
	static vec3 random_in_unit_sphere();
	// return a random vec3 contained in a disk placed at the origin and of a radius of 1
	static vec3 random_in_unit_disk();
	// return a random vec3 contained in a hemisphere placed at the origin and of a radius of 1
	static vec3 random_in_hemisphere(const vec3& normal);

	static vec3 zero() { return vec3(0.0, 0.0, 0.0); }
	static vec3 up() { return vec3(0.0, 1.0, 0.0); }
	static vec3 down() { return vec3(0.0, -1.0, 0.0); }
	static vec3 right() { return vec3(1.0, 0.0, 0.0); }
	static vec3 left() { return vec3(-1.0, 0.0, 0.0); }
	static vec3 forward() { return vec3(0.0, 0.0, 1.0); }
	static vec3 backward() { return vec3(0.0, 0.0, -1.0); }	

private:
	double m_xyz[3];
};