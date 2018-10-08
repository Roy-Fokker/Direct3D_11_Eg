#include "graphics_renderer.h"

#include <array>

using namespace direct3d_11_eg;

graphics_renderer::graphics_renderer(HWND hWnd)
{
	d3d = std::make_unique<direct3d>(hWnd);
	
	draw_buffer = std::make_unique<render_target>(d3d->get_device(), d3d->get_swap_chain());
	draw_buffer->activate(d3d->get_context());

	draw_pipeline = std::make_unique<pipeline_state>(d3d->get_device(),
	                                                 pipeline_state::description{
	                                                 	// blend state
	                                                 	{
	                                                 		D3D11_BLEND_ONE,
	                                                 		D3D11_BLEND_SRC_ALPHA,
	                                                 		D3D11_BLEND_ONE,
	                                                 		D3D11_BLEND_SRC_ALPHA,
	                                                 		D3D11_BLEND_OP_ADD,
	                                                 		D3D11_BLEND_OP_ADD
	                                                 	},
	                                                 	// depth stencil state
	                                                 	{
	                                                 		true,
	                                                 		true
	                                                 	},
	                                                 	// rasterizer state
	                                                 	{
	                                                 		D3D11_CULL_BACK,
	                                                 		D3D11_FILL_SOLID
	                                                 	},
	                                                 	// sampler state
	                                                 	{
	                                                 		D3D11_FILTER_MIN_MAG_MIP_POINT,
	                                                 		D3D11_TEXTURE_ADDRESS_WRAP
	                                                 	}
	                                                 });
}

graphics_renderer::~graphics_renderer()
{}

void graphics_renderer::draw_frame()
{
	static std::array<float, 4> clear_color{ 0.25f, 0.50f, 0.25f, 1.0f };
	draw_buffer->clear_views(d3d->get_context(), clear_color);

	draw_pipeline->activate(d3d->get_context());

	// set shaders and input layout and input topology

	// set per frame shader constants 

	// vertex data and draw
	
	d3d->present();
}

void graphics_renderer::resize_frame()
{
	draw_buffer.reset(nullptr);

	d3d->resize_swap_chain();

	draw_buffer = std::make_unique<render_target>(d3d->get_device(), d3d->get_swap_chain());
	draw_buffer->activate(d3d->get_context());
}
