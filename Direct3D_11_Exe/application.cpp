#include "application.h"

#include <functional>
#include <ratio>

using namespace direct3d_11_eg;


application::application()
{
	constexpr uint16_t height = 600;
	constexpr uint16_t width = height * 16 / 10;

	app_window = std::make_unique<window>(L"Direct3D 11.x Example",
	                                      window::size{ width, height });

	app_window->set_message_callback(window::message_type::keypress, 
	                                 [&](uintptr_t key_code, uintptr_t extension) -> bool
	                                 {
	                                 	return keypress_callback(key_code, extension);
	                                 });

	app_window->set_message_callback(window::message_type::resize,
	                                 [&](uintptr_t wParam, uintptr_t lParam) -> bool
	                                 {
	                                 	return resize_callback(wParam, lParam);
	                                 });

	gfx_renderer = std::make_unique<graphics_renderer>(app_window->handle());
}

int application::run()
{
	app_window->show();

	while (app_window->handle() and (not exit_application))
	{
		gfx_renderer->draw_frame();

		app_window->process_messages();
	}

	return 0;
}

bool application::keypress_callback(uintptr_t key_code, uintptr_t extension)
{
	switch (key_code)
	{
		case VK_ESCAPE:
			exit_application = true;
			break;
	}
	return true;
}

bool direct3d_11_eg::application::resize_callback(uintptr_t wParam, uintptr_t lParam)
{
	gfx_renderer->resize_frame();
	return false;
}
