#pragma once

#include "renderer/raytrace_renderer.h"
#include "world.h"
#include "gui/gui_image.h"

/// <summary>
/// aggregate a render and an gui_image for the selection overlay.
/// The selection overlay allows to visualize the selection with a red overlay on the selected object
/// </summary>
class selection_overlay
{
public:
	selection_overlay(raytrace_renderer& renderer)
		: m_renderer(renderer)
		  , m_selection_material{material_store().add<lambertian_material>("Selection material", *solid_color::white())}
		  , m_render{renderer.current_render}
	{
		// orange emissive material
		m_selection_material.emission = &texture_store().add<solid_color>(color(1.0f, 0.5f, 0.0f));
		m_selection_material.emission_strength = 0.25f;

		// limit depth and number of iteration so that it renders as quick as possible
		m_render.settings.bounce_depth = 2;
		m_render.target_iteration = 2;
		
		// background is black so that it masks everything.
		m_render.settings.bounce_depth_limit_color = color::white();
		m_render.settings.background_bottom_color = color::black();
		m_render.settings.background_top_color = color::black();
		m_render.settings.background_strength = 0.0f;

		// we want to render every pixel for the first iteration
		m_render.extra_progressive = false;

		// no bvh needed for only one object
		m_world.use_bvh = false;
	}

	/// <summary>
	/// signal the overlay the selection changed (modified or newly selected) and update the render (synchronously)
	/// </summary>
	void signal_change(void* selection, const camera& camera)
	{
		m_render.reset(m_renderer.empty_render);
		reset_alpha();

		// clear the world and only add a clone of the selection to it
		m_world.clear();
		hittable* clone = static_cast<hittable*>(selection)->clone();
		clone->material = &m_selection_material;
		m_world.add(clone);

		// render the new selection mask and update the ui image
		m_renderer.thread.render(camera, m_world, m_render);
		m_image.update(m_render);
	}

	// Use ImGui to draw the mask at the given position and with the given size.
	// Both these arguments should be the same as for the regular render so that the overlay renders on top
	void draw_overlay(ImVec2 image_position, ImVec2 size)
	{
		const float alpha = m_alpha * (m_render.iteration - 1.0f);
		ImGui::SetCursorPos(image_position);
		ImGui::Image(m_image.texture_id(), size, ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, alpha));
		m_alpha *= 0.96f;
	}

	void reset_alpha()
	{
		m_alpha = 1.0f;
	}

private:
	raytrace_renderer& m_renderer;
	lambertian_material& m_selection_material;
	raytrace_render_data m_render;
	gui_image m_image{true};
	world m_world;
	float m_alpha;
};
