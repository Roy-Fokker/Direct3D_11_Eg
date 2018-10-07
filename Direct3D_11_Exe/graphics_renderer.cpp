#include "graphics_renderer.h"

#include <array>

using namespace direct3d_11_eg;

graphics_renderer::graphics_renderer(HWND hWnd)
{
	d3d = std::make_unique<direct3d>(hWnd);
	d3d_render_target = std::make_unique<render_target>(d3d->get_device(), d3d->get_swap_chain());
	d3d_render_target->activate(d3d->get_context());
}

graphics_renderer::~graphics_renderer()
{}

void graphics_renderer::draw_frame()
{
	static std::array<float, 4> clear_color{ 0.25f, 0.50f, 0.25f, 1.0f };
	d3d_render_target->clear_views(d3d->get_context(), clear_color);

	d3d->present();
}

void graphics_renderer::resize_frame()
{
	d3d_render_target.reset(nullptr);

	d3d->resize_swap_chain();

	d3d_render_target = std::make_unique<render_target>(d3d->get_device(), d3d->get_swap_chain());
	d3d_render_target->activate(d3d->get_context());
}
