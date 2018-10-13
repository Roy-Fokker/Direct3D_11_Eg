#pragma once

#include "direct3d.h"

#include <Windows.h>
#include <memory>
#include <vector>

namespace direct3d_11_eg
{
	struct vertex;

	class graphics_renderer
	{
	public:
		graphics_renderer() = delete;
		graphics_renderer(HWND hWnd);
		~graphics_renderer();

		void draw_frame();
		void resize_frame();

	private:
		std::unique_ptr<direct3d> d3d = nullptr;
		std::unique_ptr<render_target> draw_buffer = nullptr;
		std::unique_ptr<pipeline_state> draw_pipeline = nullptr;
		std::unique_ptr<mesh_buffer> mesh = nullptr;
	};
};