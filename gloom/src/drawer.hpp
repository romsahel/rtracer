#pragma once

#include "canvas.hpp"
#include "pool.hpp"

enum class DrawMode
{
	Wireframe,
	Filled
};

struct Vertex
{
	int x, y;
	uint8_t h;

	Vertex(int x, int y, uint8_t h = 255) : x(x), y(y), h(h)
	{
	}
};

struct Point
{
	float x, y, z;
};

struct Camera
{
	Camera(float canvas_width, float canvas_height, float d)
		: canvasWidth(canvas_width),
		  canvasHeight(canvas_height),
		  d(d),
          viewportSizeX(1.0f),
          viewportSizeY(canvas_height / canvas_width)
	{
	}

	float canvasWidth, canvasHeight;
	float d;
	float viewportSizeX, viewportSizeY;

	[[nodiscard]] Vertex ViewportToCanvas(float x, float y) const
	{
		return Vertex(static_cast<int>(x * canvasWidth / viewportSizeX), static_cast<int>(y * canvasHeight / viewportSizeY));
	}

	[[nodiscard]] Vertex Project(const Point& p) const
	{
		return ViewportToCanvas(p.x * d / p.z, p.y * d / p.z);
	}
};

class Drawer
{
public:
	explicit Drawer(Canvas& canvas)
		: m_canvas(canvas)
	{
	}

	void drawLine(Vertex p0, Vertex p1, const Color& color)
	{
		if (std::abs(p1.x - p0.x) > std::abs(p1.y - p0.y))
		{
			if (p0.x > p1.x)
				std::swap(p0, p1);
	        auto& ys = interpolate(p0.x, p0.y, p1.x, p1.y);
			for (int x = p0.x; x <= p1.x; ++x)
				m_canvas.putPixel(x, ys[x - p0.x], color);
			m_pool.Release(ys);
		}
		else
		{
			if (p0.y > p1.y)
				std::swap(p0, p1);
	        auto& xs = interpolate(p0.y, p0.x, p1.y, p1.x);
			for (int y = p0.y; y <= p1.y; ++y)
				m_canvas.putPixel(xs[y - p0.y], y, color);
			m_pool.Release(xs);
		}
	}

	void drawTriangle(Vertex p0, Vertex p1, Vertex p2, const Color& color, DrawMode mode)
	{
		if (mode == DrawMode::Wireframe)
		{
			drawLine(p0, p1, color);
			drawLine(p1, p2, color);
			drawLine(p2, p0, color);
		}
		else
		{
			// Sort the points so that y0 <= y1 <= y2
			if (p1.y < p0.y) std::swap(p0, p1);
			if (p2.y < p0.y) std::swap(p0, p2);
			if (p2.y < p1.y) std::swap(p1, p2);

			// Compute the x coordinates of the triangle edges
			auto& x02 = interpolate(p0.y, p0.x, p2.y, p2.x);
			auto& x012 = interpolate(p0.y, p0.x, p1.y, p1.x);
			// Concatenate the short sides into x012
			x012.pop_back(); // last value of x01 and first value of x12 are duplicated
			interpolate(p1.y, p1.x, p2.y, p2.x, x012);
			
			// compute the h values of the triangle edges
			auto& h02 = interpolate(p0.y, p0.h, p2.y, p2.h);
			auto& h012 = interpolate(p0.y, p0.h, p1.y, p1.h);
			// Concatenate the short sides into h012
			h012.pop_back(); // last value of h01 and first value of h12 are duplicated
			interpolate(p1.y, p1.h, p2.y, p2.h, h012);

			// Determine which is left and which is right
			const auto m = static_cast<int>(std::floor(x012.size() / 2));
			const std::vector<int>& x_left = x02[m] < x012[m] ? x02 : x012;
			const std::vector<int>& x_right = x02[m] < x012[m] ? x012 : x02;
			const std::vector<int>& h_left = x02[m] < x012[m] ? h02 : h012;
			const std::vector<int>& h_right = x02[m] < x012[m] ? h012 : h02;

			// Draw the horizontal segments
			for (int y = p0.y; y <= p2.y; ++y)
			{
				const int iy = y - p0.y;
				const int x_start = x_left[iy];
				const int x_end = x_right[iy];
				const auto& h_segment = interpolate(x_start, h_left[iy], x_end, h_right[iy]);
				for (int x = x_start; x <= x_end; ++x)
				{
					static constexpr float inv255 = 1.0f / 255.0f;
					m_canvas.putPixel(x, y, color * (static_cast<float>(h_segment[x - x_start]) * inv255));
				}
				m_pool.Release(h_segment);
			}
			
			m_pool.Release(x012);
			m_pool.Release(x02);
			m_pool.Release(h012);
			m_pool.Release(h02);
		}
	}

private:
	std::vector<int>& interpolate(int i0, int d0, int i1, int d1)
	{
		return interpolate(i0, static_cast<float>(d0), i1, static_cast<float>(d1));
	}

	static std::vector<int>& interpolate(int i0, int d0, int i1, int d1, std::vector<int>& values)
	{
		return interpolate(i0, static_cast<float>(d0), i1, static_cast<float>(d1), values);
	}

	std::vector<int>& interpolate(int i0, float d0, int i1, float d1)
	{
		std::vector<int>& values = m_pool.Take();
		values.clear();
		return interpolate(i0, d0, i1, d1, values);
	}

	static std::vector<int>& interpolate(int i0, float d0, int i1, float d1, std::vector<int>& values)
	{
		if (i0 != i1)
		{
			values.reserve(i1 - i0 + 1);

			auto d = static_cast<float>(d0);
			const auto a = static_cast<float>(d1 - d0) / static_cast<float>(i1 - i0);

			while (i0++ <= i1)
			{
				values.emplace_back(static_cast<int>(d));
				d += a;
			}
		}
		else
		{
			values.push_back(i0);
		}

		return values;
	}

	Canvas& m_canvas;
	Pool<std::vector<int>> m_pool;
};