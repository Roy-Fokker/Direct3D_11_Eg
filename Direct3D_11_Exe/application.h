#pragma once

#include "window.h"
#include "graphics_renderer.h"

#include <memory>


namespace direct3d_11_eg
{
	class application
	{
	public:
		application();

		int run();

	private:
		bool keypress_callback(uintptr_t key_code, uintptr_t extension);
		bool resize_callback(uintptr_t wParam, uintptr_t lParam);

	private:
		bool exit_application = false;
		std::unique_ptr<window> app_window = nullptr;
		std::unique_ptr<graphics_renderer> gfx_renderer = nullptr;
	};

};