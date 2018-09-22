#include "application.h"

#include <functional>

using namespace direct3d_11_eg;

application::application()
{
	app_window = std::make_unique<window>(window::size{ 800, 600 } , L"Direct3D 11.x Example");

	app_window->set_message_callback(window::message_type::keypress, 
	                                 [&](uintptr_t key_code, uintptr_t extension) -> bool
	{
		return keypress_callback(key_code, extension);
	});
}

application::~application()
{}

int application::run()
{
	app_window->show();

	while (app_window->handle() and (not exit_application))
	{
		app_window->process_messages();
	}

	return 0;
}

bool application::keypress_callback(uintptr_t key_code, uintptr_t extension)
{
	if (key_code == VK_ESCAPE)
	{
		exit_application = true;
	}
	return true;
}
