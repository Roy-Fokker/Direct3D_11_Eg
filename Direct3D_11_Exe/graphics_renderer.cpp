#include "graphics_renderer.h"

#include <array>
#include <fstream>
#include <vector>
#include <cstdint>

using namespace direct3d_11_eg;

namespace
{
	// TODO: Move somewhere else later
	using file_in_mem = std::vector<byte>;
	// TODO: Move somewhere else later
	file_in_mem read_binary_file(const std::wstring &fileName)
	{
		file_in_mem buffer;

		std::ifstream inFile(fileName, std::ios::in | std::ios::binary);

		if (!inFile.is_open())
		{
			throw std::runtime_error("Cannot open file");
		}

		buffer.assign((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());

		return buffer;
	}
}

graphics_renderer::graphics_renderer(HWND hWnd)
{
	d3d = std::make_unique<direct3d>(hWnd);
	
	draw_buffer = std::make_unique<render_target>(d3d->get_device(), d3d->get_swap_chain());
	draw_buffer->activate(d3d->get_context());

	auto vso = read_binary_file(L"position.vs.cso"),
	     pso = read_binary_file(L"green.ps.cso");
	draw_pipeline = std::make_unique<pipeline_state>(d3d->get_device(),
	                                                 pipeline_state::description{
	                                                     pipeline_state::blend_e::Opaque,
	                                                     pipeline_state::depth_stencil_e::ReadWrite,
	                                                     pipeline_state::rasterizer_e::CullAntiClockwise,
	                                                     pipeline_state::sampler_e::PointWrap,
	                                                     
	                                                     pipeline_state::input_layout_e::position,
	                                                     D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
	                                                     vso,
	                                                     pso});
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
