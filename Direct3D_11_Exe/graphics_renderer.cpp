#include "graphics_renderer.h"
#include "vertex.h"

#include <array>
#include <fstream>
#include <vector>
#include <cstdint>
#include <tuple>

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

	using vertex_array_t = std::vector<vertex>;
	using index_array_t = std::vector<uint32_t>;
	std::tuple<vertex_array_t, index_array_t> get_triangle_mesh(float base, float height, float delta)
	{
		float halfHeight = height / 2.0f;
		float halfBase = base / 2.0f;

		float x1 = -halfBase, y1 = -halfHeight,
			x3 = base * delta, y3 = halfHeight,
			x2 = halfBase, y2 = y1;

		return {
			// Vertex List
			{
				{ { x1, y1, +0.5f } },
				{ { x3, y3, +0.5f } },
				{ { x2, y2, +0.5f } },
			},

			// Index List
			{ 0, 1, 2 }
		};
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
	                                                     pipeline_state::sampler_e::AnisotropicClamp,
	                                                     
	                                                     pipeline_state::input_layout_e::position,
	                                                     D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
	                                                     vso,
	                                                     pso});

	auto[vertex_array, index_array] = get_triangle_mesh(1.0f, 1.0f, 0.0f);
	mesh = std::make_unique<mesh_buffer>(d3d->get_device(),
	                                     vertex_array,
	                                     index_array);
}

graphics_renderer::~graphics_renderer()
{}

void graphics_renderer::draw_frame()
{
	static std::array<float, 4> clear_color{ 0.35f, 0.25f, 0.35f, 1.0f };
	draw_buffer->clear_views(d3d->get_context(), clear_color);

	draw_pipeline->activate(d3d->get_context());

	// set per frame shader constants 

	mesh->activate(d3d->get_context());
	mesh->draw(d3d->get_context());
	
	d3d->present();
}

void graphics_renderer::resize_frame()
{
	draw_buffer.reset(nullptr);

	d3d->resize_swap_chain();

	draw_buffer = std::make_unique<render_target>(d3d->get_device(), d3d->get_swap_chain());
	draw_buffer->activate(d3d->get_context());
}
