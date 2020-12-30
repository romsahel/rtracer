#pragma once

struct hit_info;
class ray;
struct color;

class material
{
public:
	explicit material(const char* name)
		: name(name)
	{
	}

	virtual bool scatter(const ray& raycast, const hit_info& rec, color& attenuation, ray& scattered) const = 0;
	const char* name;
};