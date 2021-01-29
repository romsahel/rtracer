#pragma once

#include "raytrace_renderer.h"
#include "world.h"
#include "gui/gui_image.h"

/// <summary>
/// aggregate a render and an gui_image for the selection overlay.
/// The selection overlay allows to visualize the selection with a red overlay on the selected object
/// </summary>
class selection_overlay
{
public:
	selection_overlay(const raytrace_render_data& base)
	{
		m_render = raytrace_render_data(base);
		m_render.settings.bounce_depth = 1;
		m_render.settings.bounce_depth_limit_color = color::white();
		m_render.settings.background_bottom_color = color::black();
		m_render.settings.background_top_color = color::black();
	}

	void signal_change()
	{
		m_render.iteration = 1.0;
	}

	bool has_valid_render()
	{
		return m_render.iteration >= 2.0;
	}

	// Use ImGui to draw the mask at the given position and with the given size.
	// Both these arguments should be the same as for the regular render so that the overlay renders on top
	void draw_overlay(ImVec2 image_position, ImVec2 size)
	{
		if (!has_valid_render()) return;

		ImGui::SetCursorPos(image_position);
		ImGui::Image(m_image.texture_id(), size);
	}

	// render only the given selected object
	// with minimal bounce depth and a solid-white sky to produce a black and white mask
	void render(void* selection, material& selection_material, const camera& camera, const raytrace_renderer& renderer)
	{
		if (has_valid_render()) return;

		//auto* hittable_selection = static_cast<hittable*>(selection);
		//material* saved_material = hittable_selection->material;
		//hittable_selection->material = &selection_material;
		//m_world.shallow_add(hittable_selection);

		//m_world.signal_scene_change();
		//m_render.set_pixels_from(renderer.empty_render);
		//m_render.iteration = 10.0;

		//renderer.render(camera, m_world, m_render);

		//m_image.update(renderer.settings.image_width, renderer.settings.image_height,
		//               m_render.colors.data());

		//hittable_selection->material = saved_material;
		//m_world.shallow_clear();
	}

private:
	raytrace_render_data m_render;
	gui_image m_image{true};
	world m_world;
};
