#pragma once

#include <algorithm>
#include <cstdint>

#define UnwrapColor(TYPE, COLOR) COLOR.r<TYPE>(), COLOR.g<TYPE>(), COLOR.b<TYPE>(), COLOR.a<TYPE>()

struct Color
{
	uint8_t m_r, m_g, m_b, m_a;

	Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : m_r(r), m_g(g), m_b(b), m_a(a)
	{
	}

	static Color Black()
	{
		return Color(0, 0, 0, 255);
	}

	static Color White()
	{
		return Color(255, 255, 255, 255);
	}

	static Color Red()
	{
		return Color(255, 0, 0, 255);
	}

	static Color Green()
	{
		return Color(0, 255, 0, 255);
	}

	static Color Blue()
	{
		return Color(0, 0, 255, 255);
	}

	static Color Transparent()
	{
		return Color(0, 0, 0, 0);
	}

	static Color FromFloats(float r, float g, float b, float a)
	{
		return Color(
			static_cast<uint8_t>(r * 255.0f),
			static_cast<uint8_t>(g * 255.0f),
			static_cast<uint8_t>(b * 255.0f),
			static_cast<uint8_t>(a * 255.0f)
		);
	}

#define COLOR_GETTER(COMPONENT)										\
	template <typename T = char>									\
	T COMPONENT() const												\
	{																\
		if constexpr (std::_Is_any_of_v<T, char>)					\
			return m_##COMPONENT;									\
		if constexpr (std::_Is_any_of_v<T, float>)					\
			return static_cast<float>(m_##COMPONENT) / 255.0f;		\
		if constexpr (std::_Is_any_of_v<T, double, long double>)	\
			return static_cast<double>(m_##COMPONENT) / 255.0;		\
	}

	COLOR_GETTER(r)
	COLOR_GETTER(g)
	COLOR_GETTER(b)
	COLOR_GETTER(a)

	friend bool operator==(const Color& lhs, const Color& rhs)
	{
		return lhs.m_r == rhs.m_r
			&& lhs.m_g == rhs.m_g
			&& lhs.m_b == rhs.m_b
			&& lhs.m_a == rhs.m_a;
	}

	friend bool operator!=(const Color& lhs, const Color& rhs)
	{
		return !(lhs == rhs);
	}

	Color operator+(const Color& lhs) const
	{
		return Color(*this) += lhs;
	}
	
	Color& operator+=(const Color& lhs)
	{
		m_r = static_cast<uint8_t>(std::clamp(lhs.m_r + m_r, 0, 255));
		m_g = static_cast<uint8_t>(std::clamp(lhs.m_g + m_g, 0, 255));
		m_b = static_cast<uint8_t>(std::clamp(lhs.m_b + m_b, 0, 255));
		m_a = static_cast<uint8_t>(std::clamp(lhs.m_a + m_a, 0, 255));
		return *this;
	}

	Color operator*(float factor) const
	{
		return Color(*this) *= factor;
	}
	
	Color& operator*=(float factor)
	{
		m_r = static_cast<uint8_t>(std::clamp(static_cast<float>(m_r) * factor, 0.0f, 255.0f));
		m_g = static_cast<uint8_t>(std::clamp(static_cast<float>(m_g) * factor, 0.0f, 255.0f));
		m_b = static_cast<uint8_t>(std::clamp(static_cast<float>(m_b) * factor, 0.0f, 255.0f));
		return *this;
	}
};
